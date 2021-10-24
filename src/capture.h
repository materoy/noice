#ifndef CAPTURE_H
#define CAPTURE_H

#include <AL/al.h>
#include <AL/alc.h>

int start_capture(ALCdevice *capture_device, ALubyte capture_buffer[]);

void pause_capture(ALCdevice *capture_device);

#endif // CAPTURE_H