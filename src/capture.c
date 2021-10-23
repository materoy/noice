#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "capture.h"


// Captures audio for a fixed time and returns the number of samples captured
int start_capture(ALCdevice *capture_device, ALubyte *capture_buffer) {

  ALubyte* captureBufPtr; 
  ALint samples_available;

  printf("Starting capture NOW!\n");
  fflush(stdout);
  time_t currentTime = time(NULL);
  time_t lastTime = currentTime;

  // Capture (roughly) five seconds of audio
  alcCaptureStart(capture_device);
  int samplesCaptured = 0;
  captureBufPtr = capture_buffer;
  while (currentTime < (lastTime + 5)) {
    // Get the number of samples available
    alcGetIntegerv(capture_device, ALC_CAPTURE_SAMPLES, 1, &samples_available);

    // Copy the samples to our capture buffer
    if (samples_available) {
      alcCaptureSamples(capture_device, captureBufPtr, samples_available);
      samplesCaptured += samples_available;
      printf("Captured %d samples (adding %d)\r", samplesCaptured,
             samples_available);
      fflush(stdout);

      // Advance the buffer (two bytes per sample * number of samples)
      captureBufPtr += samples_available * 2;
    }

    // Wait for a bit
    usleep(10000);

    // Update the clock
    currentTime = time(NULL);
  }

  return samplesCaptured;
}

void pause_capture(ALCdevice* capture_device) {

  printf("\nPausing capture.\n");
  alcCaptureStop(capture_device);
}
