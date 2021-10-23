#include <stdio.h>
#include <unistd.h>

#include "play.h"

void play(ALubyte captureBuffer[], ALsizei samplesCaptured) {

  ALuint buffer;
  ALuint source;

  // play back the captured data
  printf("Starting playback...\n");
  fflush(stdout);

  // Generate an OpenAL buffer for the captured data
  alGenBuffers(1, &buffer);
  alGenSources(1, &source);
  alBufferData(buffer, AL_FORMAT_MONO16, captureBuffer, samplesCaptured * 2,
               8000);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcePlay(source);

  // Wait for the source to stop playing
  ALint playState = AL_PLAYING;
  while (playState == AL_PLAYING) {
    printf("  source %d is playing...\r", source);
    fflush(stdout);
    alGetSourcei(source, AL_SOURCE_STATE, &playState);
    usleep(100000);
  }
  printf("\nDone with playback.\n");
  fflush(stdout);

  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
}