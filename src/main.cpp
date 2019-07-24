#include <iostream>
#include <soundio/soundio.h>
#include <RtMidi.h>
#include <nanceloid.h>
#include <saw_source.h>

using namespace std;

// the vocal synth instance
Nanceloid *synth;

void exit_error (string message) {
    cerr << message << endl;
    exit (EXIT_FAILURE);
}

void process_midi (double dt, vector<unsigned char> *message, void *user_data) {
    // convert this vector to an array
    int num_bytes = message->size ();
    uint8_t *data = new uint8_t[num_bytes];
    copy (message->begin (), message->end (), data);

    // send to the synth
    synth->midi (data);

    // done with the data
    delete data;
}

void process_audio (struct SoundIoOutStream *stream, int min_frames, int max_frames) {

    const struct SoundIoChannelLayout *layout = &stream->layout;
    float sample_rate = stream->sample_rate;
    struct SoundIoChannelArea *areas;

    int frames_left = max_frames;
    int error;

    synth->set_rate (sample_rate);

    while (frames_left > 0) {

        int frame_count = frames_left;

        if ((error = soundio_outstream_begin_write (stream, &areas, &frame_count)))
            exit_error ("Error: " + string (soundio_strerror (error)));

        if (!frame_count)
            break;

        for (int frame = 0; frame < frame_count; frame++) {
            float sample = synth->run ();

            for (int channel = 0; channel < layout->channel_count; channel++) {
                float *out = (float *) (areas[channel].ptr + areas[channel].step * frame);
                *out = sample;
            }
        }

        if ((error = soundio_outstream_end_write (stream)))
            exit_error ("Error: " + string (soundio_strerror (error)));

        frames_left -= frame_count;
    }
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

void setup_audio () {

    int error;

    // initialize soundio
    struct SoundIo *soundio = soundio_create();
    if (!soundio)
        exit_error ("Could not create SoundIo instance.");

    // connect soundio
    if ((error = soundio_connect (soundio)))
        exit_error ("Could not connect: " + string (soundio_strerror (error)));

    // flush events
    soundio_flush_events (soundio);

    // get index of default sound output device
    int default_out_device_index = soundio_default_output_device_index (soundio);
    if (default_out_device_index < 0)
        exit_error ("Could not find audio output device.");

    // get the device
    struct SoundIoDevice *device = soundio_get_output_device (soundio, default_out_device_index);
    if (!device)
        exit_error ("Could not get audio output device.");
    cout << "Using audio output device: " << device->name << endl;

    // create the audio output stream
    struct SoundIoOutStream *stream = soundio_outstream_create (device);
    if (!stream)
        exit_error ("Could not create audio output stream.");
    stream->format = SoundIoFormatFloat32NE;
    stream->write_callback = process_audio;

    // open the stream
    if ((error = soundio_outstream_open (stream)))
        exit_error ("Could not open audio output stream: " + string (soundio_strerror (error)));

    // make sure the layout is correct
    if (stream->layout_error)
        exit_error ("Could not set channel layout: " + string (soundio_strerror (stream->layout_error)));

    // start the stream
    if ((error = soundio_outstream_start (stream)))
        exit_error ("Could not start audio output stream: " + string (soundio_strerror (error)));

    // process audio events
    while (1)
        soundio_wait_events (soundio);

    // clean up
    soundio_outstream_destroy (stream);
    soundio_device_unref (device);
    soundio_destroy (soundio);

}

int main (int argc, char **argv) {

    // setup the synth
    synth = new Nanceloid (new SawSource);

    // setup midi
    setup_midi ();

    // setup audio
    setup_audio ();
    
    return 0;
}
