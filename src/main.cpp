#include <iostream>
#include <vector>
#include <unistd.h>
#include <RtMidi.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <nanceloid.h>

using namespace std;

// the vocal synth instance
Nanceloid *synth;

// the midi channel to listen on
// -1 means omni listen
int midi_channel = -1;

const float default_buffer_size = 2048;
const float default_sample_rate = 44100;

void exit_error (string message) {
    cerr << message << endl;
    exit (EXIT_FAILURE);
}

void process_midi (double dt, vector<unsigned char> *message, void *user_data) {
    // convert this vector to an array
    int num_bytes = message->size ();
    uint8_t *data = new uint8_t[num_bytes];
    copy (message->begin (), message->end (), data);

    // midi channel masking
    if (midi_channel != -1) {
        uint8_t channel = data[0] & 0x0f;
        if (channel != midi_channel) {
            delete data;
            return;
        }
    }

    // send to the synth
    synth->midi (data);

    // done with the data
    delete data;
}

void setup_midi () {
    // setup midi input
    RtMidiIn *midi_in = new RtMidiIn ();
    vector<unsigned char> message;

    // check midi in ports
    unsigned int num_ports = midi_in->getPortCount ();
    if (num_ports == 0)
        exit_error ("Could not find MIDI input ports.");

    // open the first available port
    midi_in->openPort (0);

    // don't ignore messages
    midi_in->ignoreTypes (false, false, false);

    // set midi callback
    midi_in->setCallback (&process_midi);
}

void print_usage_and_exit (char *command) {
    cerr << "Usage: " << command << " [-c channel] [-b buffer size] [-s sample rate] [-d]\n\n";
    cerr << "-c channel\n\tSpecify the midi channel to listen on.\n\tIf left unspecified it will listen on all channels.\n\n";
    cerr << "-b buffer size\n\tSpecify the size of the audio buffer in number of samples.\n\tIf left unspecified it is " << default_buffer_size << ".\n\n";
    cerr << "-s sample rate\n\tSpecify the audio sampling rate in samples per second.\n\tIf left unspecified it is " << default_sample_rate << ".\n\n";
    cerr << "-d\n\tDisable the GUI.\n\n";
    cerr << flush;
    exit (EXIT_FAILURE);
}

class SoundStream : public sf::SoundStream {
    private:
        Nanceloid *synth;
        sf::Int16 *m_samples;
        int buffer_size;

    public:
        SoundStream (Nanceloid *synth, int buffer_size, int rate)
            : synth (synth), buffer_size (buffer_size)
        {
            initialize (2, rate);
            synth->set_rate (rate);
            m_samples = new sf::Int16[buffer_size];
        }

        ~SoundStream () {
            delete m_samples;
        }

        virtual bool onGetData (Chunk &data) {
            data.samples = m_samples;
            data.sampleCount = buffer_size;

            // fill the buffer for sfml
            float samples[2];
            const int max = 32767;
            for (int i = 0; i < buffer_size; i += 2) {
                synth->run (samples);
                m_samples[i]     = (sf::Int16) (samples[0] * max);
                m_samples[i + 1] = (sf::Int16) (samples[1] * max);
            }

            return true;
        }
        
        virtual void onSeek (sf::Time timeOffset) {}
};

int main (int argc, char **argv) {
    // default cli args
    float buffer_size = default_buffer_size;
    float sample_rate = default_sample_rate;
    int enable_gui = true;

    // parse cli args
    int c;
    while ((c = getopt (argc, argv, "c:b:s:d")) != -1) {
        switch (c) {
            case 'c':
                midi_channel = atoi (optarg);
                break;
            case 'b':
                buffer_size = atoi (optarg);
                break;
            case 's':
                sample_rate = atoi (optarg);
                break;
            case 'd':
                enable_gui = false;
                break;
            default:
                print_usage_and_exit (argv[0]);
        }
    }

    // setup the synth
    synth = new Nanceloid ();

    // setup midi
    setup_midi ();

    // create and start playing the audio stream
    SoundStream stream (synth, buffer_size, sample_rate);
    stream.play ();

    if (enable_gui) {
        // setup gui window
        const int screen_width = 600;
        const int screen_height = 200;
        sf::RenderWindow window (sf::VideoMode (screen_width, screen_height), "Nanceloid", sf::Style::Default);
        auto desktop = sf::VideoMode::getDesktopMode ();
        sf::Vector2i desktop_size (desktop.width, desktop.height);
        window.setPosition ((desktop_size - (sf::Vector2i) window.getSize ()) / 2);
        window.setFramerateLimit (60);

        sf::Font font;
        if (!font.loadFromFile("font.ttf"))
            exit_error ("Could not load font!");
        sf::Text text;
        text.setFont (font);
        text.setString ("Hi");
        text.setCharacterSize (12);
        text.setPosition (-1, -1);
        text.setScale (2.0 / screen_width, 2.0 / screen_height);
        text.setStyle (sf::Text::Regular);

        sf::View view (sf::FloatRect(-1, -1, 2, 2));
        window.setView (view);

        // event loop
        bool mouse_down = false;
        double mouse_x = 0;
        double mouse_y = 0;
        while (window.isOpen ())
        {
            window.clear ();

            // draw tract shape
            const int res = 64;
            sf::VertexArray lines (sf::LinesStrip, res);
            sf::VertexArray lines2 (sf::LinesStrip, res);
            for (int j = 0; j < res; j++) {
                double n = (double) j / (res - 1);
                double sample = synth->shape.sample (n);
                lines[j].position = sf::Vector2f (n * 2 - 1, sample);
                lines2[j].position = sf::Vector2f (n * 2 - 1, -sample);
            }
            // text display
            stringstream display_string;
            display_string << "Patch         #" << synth->get_shape_id () << "\n";
            display_string << "Velic closure: " << (int) round (synth->shape.velic_closure * 100) << "%\n";
            display_string << "Voicing:       " << (int) round (synth->get_voicing () * 100) << "%\n";
            display_string << "Second Fold:   " << (int) round (synth->params.second_fold.value * 100) << "%\n";
            display_string << "Uvula:         " << (int) round (synth->params.uvula.value * 100) << "%\n";
            display_string << "Frequency:     " << round (synth->get_frequency () * 100) / 100 << "hz\n";
            display_string << "Detected:      " << round (synth->get_detected_frequency () * 100) / 100 << "hz\n";
            text.setString (display_string.str ());
            // scope
            synth->prepare_scope ();
            int samples = synth->get_scope_samples ();
            sf::VertexArray lines_scope (sf::LinesStrip, samples);
            for (int j = 0; j < samples; j++) {
                double n = (double) j / (samples - 1);
                float sample = synth->get_scope (n);
                lines_scope[j].position = sf::Vector2f (n * 2 - 1, -sample);
                lines_scope[j].color = sf::Color::Red;
            }
            // scope auto correlation
            //int scope_size2 = synth->get_scope_size ();
            //sf::VertexArray lines_scope2 (sf::LinesStrip, scope_size2);
            //for (int j = 0; j < scope_size2; j++) {
            //    double n = (double) j / (scope_size2 - 1);
            //    float sample = freq ? synth->get_auto_scope (j) : 0;
            //    lines_scope2[j].position = sf::Vector2f (n * 2 - 1, -sample);
            //    lines_scope2[j].color = sf::Color::Blue;
            //}
            // draw em
            window.draw (lines);
            window.draw (lines2);
            //window.draw (lines_scope2);
            window.draw (lines_scope);
            window.draw (text);
            window.display ();

            sf::Event event;
            while (window.pollEvent (event))
            {
                if (event.type == sf::Event::Closed)
                    window.close ();
                else if (event.type == sf::Event::MouseButtonPressed)
                    mouse_down = true;
                else if (event.type == sf::Event::MouseButtonReleased)
                    mouse_down = false;
                else if (event.type == sf::Event::MouseMoved) {
                    mouse_x = (double) event.mouseMove.x / screen_width * 2 - 1;
                    mouse_y = (double) event.mouseMove.y / screen_height * 2 - 1;
                    mouse_x = fmax (-1, fmin (1, mouse_x));
                    mouse_y = fmax (-1, fmin (1, mouse_y));
                } else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape)
                        window.close ();
                    else if (event.key.code == sf::Keyboard::Equal)
                        synth->params.uvula.value = synth->params.uvula.value ?  0 : 0.1;
                    else if (event.key.code == sf::Keyboard::Enter)
                        synth->params.second_fold.value = synth->params.second_fold.value ?  0 : 1;
                    else if (event.key.code == sf::Keyboard::Tab)
                        synth->get_shape ().velic_closure = synth->get_shape ().velic_closure ?  0 : 1;
                    else if (event.key.code == sf::Keyboard::Backspace)
                        synth->params.voicing.value = synth->params.voicing.value ?  0 : 1;
                    else if (event.key.code == sf::Keyboard::Space) {
                        int note = synth->playing_note ();
                        if (note == -1)
                            synth->note_on (45+12+3, 1);
                        else
                            synth->note_off (note);
                    }
                } else if (event.type == sf::Event::TextEntered) {
                    // switch to patches corresponding to lower case letters
                    char c = event.text.unicode;
                    if (c >= 97 && c <= 122)
                        synth->set_shape_id (c);
                }
                if (mouse_down) {
                    double sample = fmax (0, mouse_y);
                    double n = (mouse_x + 1) / 2;
                    synth->get_shape ().set_sample (n, sample);
                }
            }
        }
    } else {
        while (stream.getStatus () == sf::Sound::Playing)
            sf::sleep (sf::seconds (1));
    }

    // cleanup and done
    delete synth;
    return 0;
}
