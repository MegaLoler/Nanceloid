#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <nanceloid.h>
#include <midi.h>

// the vocal synth instance
Voice *voice;

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
    // TODO: consider event timing
    for (int i = 0; i < num_events; i++) {
        jack_midi_event_get (&event, midi_port_buffer, i);
        process_midi (voice, event.buffer);
    }

    // process audio frames
    for (int i = 0; i < num_frames; i++) {
        audio_buffer_left[i] = audio_buffer_right[i] = step_voice (voice);
    }

    return 0;
}

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
    midi_input_port = jack_port_register (client, "Controller", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    audio_output_port_left = jack_port_register (client, "L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    audio_output_port_right = jack_port_register (client, "R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    if ((midi_input_port == NULL) || (audio_output_port_left == NULL) || (audio_output_port_right == NULL)) {
        fprintf (stderr, "Could not create JACK ports.\n");
        exit (EXIT_FAILURE);
    }

    // setup the synth
    voice = create_voice (SAWTOOTH, jack_get_sample_rate(client));

    // activate the client
    if (jack_activate (client)) {
        fprintf (stderr, "Could not activate JACK client.\n");
        exit (EXIT_FAILURE);
    }

    // automatically connect to system audio out if possible
    const char **ports;
	ports = jack_get_ports (client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
    if (ports == NULL) {
        fprintf (stderr, "Could not find system audio output ports.\n");
        exit (EXIT_FAILURE);
    } else if (jack_connect (client, jack_port_name (audio_output_port_left), ports[0]) ||
               jack_connect (client, jack_port_name (audio_output_port_right), ports[1])) {
        fprintf (stderr, "Could not connect to system audio output ports.\n");
        exit (EXIT_FAILURE);
    }

    // setup alsa connection for midi input
    snd_seq_t *seq_handle;
    snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_INPUT, 0);
    snd_seq_set_client_name(seq_handle, "Nanceloid");
    snd_seq_create_simple_port(seq_handle, "Controller",
            SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_APPLICATION);

    // receive alsa midi events
    while (1) {
        snd_seq_event_t *event = NULL;
        snd_seq_event_input (seq_handle, &event);

        // idk a better way to get raw midi data to the midi processor
        // TODO: find a better way!!!
        // how to preserve midi channel too?
        uint8_t buffer[3];
        if ((event->type == SND_SEQ_EVENT_NOTEON)) {
            buffer[0] = 0x90;
            buffer[1] = event->data.note.note;
            buffer[2] = event->data.note.velocity;
        } else if ((event->type == SND_SEQ_EVENT_NOTEOFF)) {
            buffer[0] = 0x80;
            buffer[1] = event->data.note.note;
            buffer[2] = event->data.note.velocity;
        } else if ((event->type == SND_SEQ_EVENT_CONTROLLER)) {
            buffer[0] = 0xb0;
            buffer[1] = event->data.control.param;
            buffer[2] = event->data.control.value;
        } else {
            continue;
        }
        process_midi (voice, buffer);
    }

    // wait until user exits
    sleep(-1);
    
    // cleanup
    destroy_voice (voice);
    jack_client_close (client);

    return 0;
}
