#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#include "capture.h"
#include "init_al.h"

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
static int init = 0;

  // ALubyte *capture_buffer_ptr = capture_buffer;
static  ALint available_buffers = 0;   // Buffers to be recovered
static ALint buffers_holder[CHANNELS];   // Array to hold catch the unqueued buffers
static short buffer[FREQUENCY * 2];
static ALint actual_buffer;

int capture_rt(ALCdevice *capture_device, ALubyte capture_buffer[])
{
  if (!init)
  {
    printf("Starting capture ...\n");
    fflush(stdout);
    alcCaptureStart(capture_device);
    init = 1;
  }



  // Check for recoverable buffers
  alGetSourcei(source[0], AL_BUFFERS_PROCESSED, &available_buffers);
  if (available_buffers > 0)
  {
    alSourceUnqueueBuffers(source[0], available_buffers, buffers_holder);
    for (size_t i = 0; i < available_buffers; ++i)
    {
      buffer_queue[i] = buffers_holder[i];
    }
    
  }
  

  alcGetIntegerv(capture_device, ALC_CAPTURE_SAMPLES, 1, &samples_available);
  if (samples_available > CAP_SIZE)
  {
    alcCaptureSamples(capture_device, buffer, CAP_SIZE);
    // printf("Samples captured: %d\r", samples_available);
    // fflush(stdout);

    size_t buffer_queue_size = sizeof(buffer_queue) / sizeof(ALint);
    if (buffer_queue_size != 0)
    {
        actual_buffer = buffer_queue[0];

        for (size_t i = 0; i < buffer_queue_size; i++)
        {
            buffer_queue[i] = buffer_queue[i+1]; 
        }
        
        alBufferData(actual_buffer, AL_FORMAT_MONO16, buffer, CAP_SIZE * sizeof(short), FREQUENCY);

        // Queue the buffer
        alSourceQueueBuffers(source[0], 1, &actual_buffer);

        ALint source_state = 0;
        alGetSourcei(source[0], AL_SOURCE_STATE, &source_state);
        if (source_state != AL_PLAYING)
        {
          alSourcePlay(source[0]);
          printf("playing %d samples \r", samples_available);
          fflush(stdout);
        }
    }

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
