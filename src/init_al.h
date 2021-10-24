#if !defined(INITAL_H)
#define INITAL_H

#include <AL/al.h>
#include <AL/alc.h>

#define FREQUENCY 8000

ALCdevice *open_playback_device();

ALCdevice *open_capture_device();

#endif // INITAL_H
