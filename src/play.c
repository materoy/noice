#include <stdio.h>
#include <unistd.h>

#include "play.h"
#include "init_al.h"

void play(ALubyte capture_buffer[], ALsizei samples_captured)
{

  ALuint buffer;
  ALuint source;

  // play back the captured data
  printf("Starting playback...\n");
  fflush(stdout);

  // Generate an OpenAL buffer for the captured data
  alGenBuffers(1, &buffer);
  alGenSources(1, &source);
  alBufferData(buffer, AL_FORMAT_MONO16, capture_buffer, samples_captured * 2,
               8000);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcePlay(source);

  // Wait for the source to stop playing
  ALint playState = AL_PLAYING;
  while (playState == AL_PLAYING)
  {
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

static int init = 0;

void play_rt(ALubyte capture_buffer[], ALsizei samples_captured)
{
  if (!init)
  {
    // Generate an OpenAL channel_buffers for the captured data
    alGenBuffers(CHANNELS, &channel_buffers[0]);
    alGenSources(1, &source[0]);
    init = 1;
  }
  alBufferData(channel_buffers, AL_FORMAT_MONO16, capture_buffer, samples_captured * 2, FREQUENCY);
  alSourcei(source, AL_BUFFER, channel_buffers);
  alSourcePlay(source);
}