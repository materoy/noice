#ifndef CAPTURE_H
#define CAPTURE_H

#include <AL/al.h>
#include <AL/alc.h>

int start_timed_capture(ALCdevice *capture_device, ALubyte capture_buffer[]);

// Captures samples and returns the number of samples captured
int capture_rt(ALCdevice *capture_device, ALubyte capture_buffer[]);

void pause_capture(ALCdevice *capture_device);

#endif // CAPTURE_H