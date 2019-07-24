#include <iostream>
#include <portaudio.h>
//#include <nanceloid.h>
//#include <midi.h>

using namespace std;

const int sample_rate = 44100;

// the vocal synth instance
//Voice *voice;

// the port audio audio callback
int process (const void *input_buffer, void *output_buffer, long unsigned int num_frames,
             const PaStreamCallbackTimeInfo *timeInfo,
             PaStreamCallbackFlags statusFlags,
             void *userData) {

    float *out = (float *) output_buffer;

    // process audio frames
    for (long unsigned int i = 0; i < num_frames; i++) {
        double sample = 0; // step_voice (voice);
        *out++ = sample;  // left
        *out++ = sample;  // right
    }

    return 0;
}

void exit_error (string message, PaError error) {
    cerr << message << ": " << Pa_GetErrorText (error) << endl;
    exit (EXIT_FAILURE);
}

int main (int argc, char **argv) {

    PaError error;
    
    // initialize port audio
    error = Pa_Initialize ();
    if (error != paNoError)
        exit_error ("Could not initialize PortAudio", error);

    // create the audio stream
    PaStream *stream;
    error = Pa_OpenDefaultStream (&stream, 0, 2, paFloat32, sample_rate,
                                  paFramesPerBufferUnspecified, process, nullptr);
    if (error != paNoError)
        exit_error ("Could not create PortAudio stream", error);

    // start the audio stream
    error = Pa_StartStream (stream);
    if (error != paNoError)
        exit_error ("Could not start PortAudio stream", error);

    // sleep until user closes
    Pa_Sleep (-1);

    // stop the audio stream
    error = Pa_StopStream (stream);
    if (error != paNoError)
        exit_error ("Could not stop PortAudio stream", error);

    // close the audio stream
    error = Pa_CloseStream (stream);
    if (error != paNoError)
        exit_error ("Could not close PortAudio stream", error);

    // terminate port audio
    error = Pa_Terminate ();
    if (error != paNoError)
        exit_error ("Could not terminate PortAudio", error);

    return 0;
}
