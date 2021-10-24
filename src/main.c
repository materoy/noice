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


int main() {

  const ALCchar *devices;
  const ALCchar *ptr;
  ALCcontext *mainContext;
  ALubyte captureBuffer[1048576];
  ALubyte *captureBufPtr;
  ALint samplesAvailable;

  ALCdevice *playback_device = open_playback_device();

  mainContext = alcCreateContext(playback_device, NULL);
  if (mainContext == NULL) {
    printf("Unable to create playback context!\n");
    exit(1);
  }
  printf("   created playback context\n");

  // Make the playback context current
  alcMakeContextCurrent(mainContext);
  alcProcessContext(mainContext);

  // Synthesizer playback mainloop
  // for(;;) {}
  ALCdevice *capture_device = open_capture_device();

  int samples_captured = start_capture(capture_device, captureBuffer);

  pause_capture(capture_device);

  play(captureBuffer, samples_captured);

  // Shut down OpenAL
  alcMakeContextCurrent(NULL);
  alcCloseDevice(playback_device);
  alcCaptureCloseDevice(capture_device);

  return EXIT_SUCCESS;
}