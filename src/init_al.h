#if !defined(INITAL_H)
#define INITAL_H

#include <AL/al.h>
#include <AL/alc.h>

#define FREQUENCY 22050
#define CAP_SIZE 1024
#define CHANNELS 16


static ALuint channel_buffers[CHANNELS];
static ALuint source[1];
static ALint buffer_queue[CHANNELS];

ALCdevice *open_playback_device();

ALCdevice *open_capture_device();

#endif // INITAL_H
