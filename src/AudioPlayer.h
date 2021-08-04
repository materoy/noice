#pragma once

#include <AL/al.h>
#include <AL/alc.h>

class AudioPlayer
{
private:
    ALuint buffer;
    ALCdevice *captureDevice;
    ALuint source;
    ALint playState;
    ALsizei srate;

public:
    AudioPlayer(ALCdevice *captureDevice, ALsizei srate);
    ~AudioPlayer();

    void startPlaying(ALvoid *data, ALsizei totalSamples);
    void stopPlaying();
    void pausePlaying();
};



