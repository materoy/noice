#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "capture.h"
#include "play.h"
#include "init_al.h"

int main()
{
  const ALCchar *devices;
  const ALCchar *ptr;
  ALCcontext *mainContext;
  ALubyte capture_buffer[1048576];
  ALubyte *captureBufPtr;
  ALint samplesAvailable;

  ALCdevice *playback_device = open_playback_device();

  mainContext = alcCreateContext(playback_device, NULL);
  if (mainContext == NULL)
  {
    printf("Unable to create playback context!\n");
    exit(1);
  }
  printf("   created playback context\n");

  // Make the playback context current
  alcMakeContextCurrent(mainContext);
  alcProcessContext(mainContext);

  ALCdevice *capture_device = open_capture_device();

  int samples_captured;
  // Synthesizer playback mainloop
  for (;;)
  {
    samples_captured = capture_rt(capture_device, capture_buffer);
    if (samples_captured)
    {
      play_rt(capture_buffer, samples_captured);
      
      printf("\t\tPlaying %d samples .\r", samples_captured);
      fflush(stdout);

      usleep(10000);
    }
    
  }
  // int samples_captured = capture(capture_device, capture_buffer);

  // pause_capture(capture_device);

  // play(capture_buffer, samples_captured);

  // Shut down OpenAL
  alcMakeContextCurrent(NULL);
  alcCloseDevice(playback_device);
  alcCaptureCloseDevice(capture_device);

  return EXIT_SUCCESS;
}