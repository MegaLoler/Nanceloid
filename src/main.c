#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <jack/jack.h>
#include <jack/midiport.h>

jack_client_t *client;
jack_port_t *midi_input_port;
jack_port_t *audio_output_port_left;
jack_port_t *audio_output_port_right;

int jack_process (jack_nframes_t num_frames, void *arg) {

    // get the audio output buffers
    jack_default_audio_sample_t *audio_buffer_left, *audio_buffer_right;
    audio_buffer_left = jack_port_get_buffer (audio_output_port_left, num_frames);
    audio_buffer_right = jack_port_get_buffer (audio_output_port_right, num_frames);

    // get the midi input buffer
    uint8_t *midi_port_buffer = jack_port_get_buffer (midi_input_port, num_frames);

    // get midi events
    jack_midi_event_t event;
    jack_nframes_t num_events = jack_midi_get_event_count (midi_port_buffer);

    // process midi events
    // TODO: consider timing
    for (int i = 0; i < num_events; i++) {
        jack_midi_event_get (&event, midi_port_buffer, i);
        //uint8_t *data = event.buffer;
        // TODO: process data using midi.h/c
    }

    // process audio frames
    for (int i = 0; i < num_frames; i++) {
        // TODO: run synth and set audio buffer sample to result
        audio_buffer_left[0] = audio_buffer_right[0] = 0; // temporary
    }

    return 0;
}

// jack shutdown callback
void jack_shutdown (void *arg) {
    fprintf (stderr, "JACK server shutting down.\n");
    exit (EXIT_FAILURE);
}

int main (int argc, char **argv) {

    // create the jack client
    jack_status_t status;
    client = jack_client_open ("Nanceloid", JackNullOption, &status, NULL);

    if (client == NULL) {
        fprintf(stderr, "Could not create JACK client.\nStatus = 0x%x\n", status);
        if (status & JackServerFailed) {
            fprintf (stderr, "Could not connect to JACK server.\n");
        }
        exit (EXIT_FAILURE);
    } else if (status & JackServerStarted) {
        fprintf (stderr, "JACK server started.\n");
    }

    // set jack callbacks
    jack_set_process_callback (client, jack_process, 0);
    jack_on_shutdown (client, jack_shutdown, 0);

    // create in and out port
    midi_input_port = jack_port_register (client, "Nanceloid Controller", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    audio_output_port_left = jack_port_register (client, "L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    audio_output_port_right = jack_port_register (client, "R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    if ((midi_input_port == NULL) || (audio_output_port_left == NULL) || (audio_output_port_right == NULL)) {
        fprintf (stderr, "Could not create JACK ports.\n");
        exit (EXIT_FAILURE);
    }

    // setup the synth
    // TODO

    // activate the client
    if (jack_activate (client)) {
        fprintf (stderr, "Could not activate JACK client.\n");
        exit (EXIT_FAILURE);
    }

    // wait until user exits
    sleep(-1);
    
    // cleanup
    // TODO: destroy synth
    jack_client_close (client);

    return 0;
}
