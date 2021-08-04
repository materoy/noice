#include "AudioRecorder.h"
#include <iostream>
#include <stdio.h>

AudioRecorder::AudioRecorder()
{
    setupAudioProcessor();
}

void AudioRecorder::setupAudioProcessor()
{
    alGetError();

    // Open default device
    device = alcOpenDevice(NULL);

    if (device == NULL)
    {
        std::cout << "ERROR::CANNOT OPEN DEFAULT DEVICE" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create context
    context = alcCreateContext(device, NULL);

    if (context == NULL)
    {
        std::cout << "ERROR::CANNOT CREATE CONTEXT" << std::endl;
        exit(EXIT_FAILURE);
    }
    // Set active context
    alcMakeContextCurrent(context);
    alcProcessContext(context);

    if (alcIsExtensionPresent(device, "ALC_EXT_CAPTURE") == AL_FALSE)
    {
        std::cout << "ERROR::NO AUDIO RECORDING POSSIBLE" << std::endl;
        exit(EXIT_FAILURE);
    }

    ALCdevice *captureDevice =
        alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_STEREO16, SSIZE);

    if (captureDevice == NULL)
    {
        std::cout << "ERROR::NO ACCESSS TO THE Open-AL-CAPUTRE-device" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "INFO::CAPTURE DEVICE OPENED" << std::endl;
}

void AudioRecorder::startRecord()
{
    recording = true;

    printf("    INFO::START RECORDING AUDIO\n");
    alcCaptureStart(captureDevice);
    while (recording)
    {
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, 1, &samples);
        samplesCaptured += samples;
        printf("Capturing %d samples, %d samples captured \r", samples, samplesCaptured);
        fflush(stdout);
        //     if (samples)
        //     {
        //         alcCaptureSamples(captureDevice, (ALCvoid *)captureBufferPtr, samples);
        //         captureBufferPtr += samples * 4;
        //     }
    }
}

void AudioRecorder::stopRecord()
{
    recording = false;
    alcCaptureStop(captureDevice);
}

AudioRecorder::~AudioRecorder()
{
    alcCloseDevice(device);
    alcCaptureCloseDevice(captureDevice);
    alcDestroyContext(context);
}