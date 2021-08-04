#include <iostream>
#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(ALCdevice *captureDevice, ALsizei srate)
{
    captureDevice = captureDevice;
    srate = srate;
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);
}

AudioPlayer::~AudioPlayer()
{
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
}

void AudioPlayer::startPlaying(ALvoid* data, ALsizei totalSamples)
{
    playState = AL_PLAYING;
    alBufferData(buffer, AL_FORMAT_STEREO16, data, totalSamples * 4, srate);

    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);

    while (playState == AL_PLAYING)
    {
        alGetSourcei(source, AL_SOURCE_STATE, &playState);
    }
    
}

void AudioPlayer::pausePlaying()
{
    playState = AL_PAUSED;
}

void AudioPlayer::stopPlaying()
{
    playState = AL_STOPPED;
}
