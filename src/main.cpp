#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

const int SRATE = 44100;
const int SSIZE = 1024;

ALubyte captureBuffer[1048576];
ALubyte *captureBufferPtr;
ALCint samples;
ALCint samplesCaptured;
ALuint buffer;
ALuint source;
ALint playState;
time_t currentTime;

int main()
{
    alGetError();

    ALCdevice *device;
    ALCcontext *context;

    // Open default device
    device = alcOpenDevice(NULL);

    if (device == NULL)
    {
        cout << "ERROR::CANNOT OPEN DEFAULT DEVICE" << endl;
        exit(EXIT_FAILURE);
    }

    // Create context
    context = alcCreateContext(device, NULL);

    if (context == NULL)
    {
        cout << "ERROR::CANNOT CREATE CONTEXT" << endl;
        exit(EXIT_FAILURE);
    }
    // Set active context
    alcMakeContextCurrent(context);
    alcProcessContext(context);

    if (alcIsExtensionPresent(device, "ALC_EXT_CAPTURE") == AL_FALSE)
    {
        cout << "ERROR::NO AUDIO RECORDING POSSIBLE" << endl;
        exit(EXIT_FAILURE);
    }

    ALCdevice *captureDevice =
        alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_STEREO16, SSIZE);

    if (captureDevice == NULL)
    {
        cout << "ERROR::NO ACCESSS TO THE Open-AL-CAPUTRE-device" << endl;
        exit(EXIT_FAILURE);
    }

    captureBufferPtr = captureBuffer;
    samplesCaptured = 0;
    alcCaptureStart(captureDevice);

    sleep(2);
    currentTime = time(NULL);
    /// the main recording loop
    /// Records for 3 seconds
    while (time(NULL) - currentTime < 5)
    {
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, 1, &samples);
        if (samples > 0)
        {
            alcCaptureSamples(captureDevice, (ALCvoid *)captureBufferPtr, samples);
            samplesCaptured += samples;
            printf("Capturing %d samples: total samples %d \r", samples, samplesCaptured);
            fflush(stdout);
            captureBufferPtr += samples * 2;
        }
            // usleep(100000);
    }

    printf("\n  Done capturing ... \n ");
    alcCaptureStop(captureDevice);
    currentTime = time(NULL);

    sleep(2);
    // Play back the captured data
    printf("Starting playback...\n");
    fflush(stdout);

    alGenBuffers(1, &buffer);
    alGenSources(1, &source);
    alBufferData(buffer, AL_FORMAT_STEREO16, captureBuffer, samplesCaptured * 4, SRATE);

    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);
    playState = AL_PLAYING;
    while (playState == AL_PLAYING)
    {
        printf("Source %d is playing .... \r", source);
        fflush(stdout);
        alGetSourcei(source, AL_SOURCE_STATE, &playState);
        // usleep(100000);
    }

    printf("\n Finished playback ... \n");
    fflush(stdout);

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcCloseDevice(device);
    alcCaptureCloseDevice(captureDevice);
    alcDestroyContext(context);

    return EXIT_SUCCESS;
}