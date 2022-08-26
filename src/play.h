#if !defined(PLAY_H)
#define PLAY_H

#include <AL/al.h>


void play(ALubyte capture_buffer[], ALsizei samples_captured);

void play_rt(ALubyte capture_buffer[], ALsizei samples_captured);

#endif // PLAY_H
