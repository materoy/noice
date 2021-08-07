#pragma once

#include <AL/al.h>
#include <AL/alc.h>

class AudioRecorder
{
public:
    ALCdevice *captureDevice;
    AudioRecorder();

    ~AudioRecorder();

    void startRecord();

    void stopRecord();

private:
    const ALint SRATE = 44100;
    const ALsizei SSIZE = 1024;

    ALubyte captureBuffer[1048576];
    ALubyte *captureBufferPtr = captureBuffer;
    ALCint samples;
    ALCint samplesCaptured = 0;


    ALCdevice *device;
    ALCcontext *context;

    bool recording;

    void setupAudioProcessor();
};

