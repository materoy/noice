#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "capture.h"

// Captures audio for a fixed time and returns the number of samples captured
int start_timed_capture(ALCdevice *capture_device, ALubyte capture_buffer[])
{

  ALubyte *capture_buf_ptr;
  ALint samples_available;

  printf("Starting capture NOW!\n");
  fflush(stdout);
  time_t currentTime = time(NULL);
  time_t lastTime = currentTime;

  // Capture (roughly) five seconds of audio
  alcCaptureStart(capture_device);
  int samples_captured = 0;
  capture_buf_ptr = capture_buffer;
  while (currentTime < (lastTime + 5))
  {
    // Get the number of samples available
    alcGetIntegerv(capture_device, ALC_CAPTURE_SAMPLES, 1, &samples_available);

    // Copy the samples to our capture buffer
    if (samples_available)
    {
      alcCaptureSamples(capture_device, capture_buf_ptr, samples_available);
      samples_captured += samples_available;
      printf("Captured %d samples (adding %d)\r", samples_captured,
             samples_available);
      fflush(stdout);

      // Advance the buffer (two bytes per sample * number of samples)
      capture_buf_ptr += samples_available * 2;
    }

    // Update the clock
    currentTime = time(NULL);
  }

  return samples_captured;
}

int samples_available;
int init = 0;

int capture_rt(ALCdevice *capture_device, ALubyte capture_buffer[])
{
  if (!init)
  {
    printf("Starting capture ...\n");
    fflush(stdout);
    alcCaptureStart(capture_device);
    init = 1;
  }

  time_t time_now = time(NULL);

  alcGetIntegerv(capture_device, ALC_CAPTURE_SAMPLES, 1, &samples_available);
  if (samples_available)
  {
    alcCaptureSamples(capture_device, capture_buffer, samples_available);
    // printf("Samples captured: %d\r", samples_available);
    // fflush(stdout);

    return samples_available;
  }
  // printf("No samples captured\n");
  return 0;
}

void pause_capture(ALCdevice *capture_device)
{

  printf("\nPausing capture.\n");
  alcCaptureStop(capture_device);
}
