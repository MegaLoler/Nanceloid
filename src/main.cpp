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
    cerr << "Usage: " << command << " [-c channel] [-b buffer size] [-s sample rate]\n\n";
    cerr << "-c channel\n\tSpecify the midi channel to listen on.\n\tIf left unspecified it will listen on all channels.\n\n";
    cerr << "-b buffer size\n\tSpecify the size of the audio buffer in number of samples.\n\tIf left unspecified it is " << default_buffer_size << ".\n\n" << flush;
    cerr << "-s sample rate\n\tSpecify the audio sampling rate in samples per second.\n\tIf left unspecified it is " << default_sample_rate << ".\n\n" << flush;
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

    // parse cli args
    int c;
    while ((c = getopt (argc, argv, "c:")) != -1) {
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
            default:
                print_usage_and_exit (argv[0]);
        }
    }

    // setup the synth
    synth = new Nanceloid ();

    // setup midi
    setup_midi ();

    // setup sfml
    const int screen_width = 600;
    const int screen_height = 200;
    sf::RenderWindow window (sf::VideoMode(screen_width, screen_height), "Nanceloid", sf::Style::Default);
    auto desktop = sf::VideoMode::getDesktopMode ();
    sf::Vector2i desktop_size (desktop.width, desktop.height);
    window.setPosition ((desktop_size - (sf::Vector2i) window.getSize ()) / 2);
    window.setFramerateLimit (60);

    sf::View view (sf::FloatRect(-1, -1, 2, 2));
    window.setView (view);
    
    // create and start playing the audio stream
    SoundStream stream (synth, buffer_size, sample_rate);
    stream.play ();

    // event loop
    bool mouse_down = false;
    double mouse_x = 0;
    double mouse_y = 0;
    while (window.isOpen ())
    {
        window.clear();

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
        window.draw(lines);
        window.draw(lines2);

        window.display();

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
            }
            if (mouse_down) {
                double sample = abs (mouse_y);
                double n = (mouse_x + 1) / 2;
                synth->shape.set_sample (n, sample);
            }
        }
    }

    // cleanup and done
    delete synth;
    return 0;
}
