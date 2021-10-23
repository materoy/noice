#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "capture.h"
#include "play.h"

int init() {

}


int main() {

  const ALCchar *devices;
  const ALCchar *ptr;
  ALCdevice *mainDev;
  ALCcontext *mainContext;
  ALCdevice *captureDev;
  ALubyte captureBuffer[1048576];
  ALubyte *captureBufPtr;
  ALint samplesAvailable;

  // Print the list of capture devices
  printf("Available playback devices:\n");

  devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
  ptr = devices;
  // while (ptr[0] != NULL)
  while (*ptr) {
    printf("   %s\n", ptr);
    ptr += strlen(ptr) + 1;
  }

  // Open a playback device and create a context first
  printf("Opening playback device:\n");
  mainDev = alcOpenDevice(NULL);
  if (mainDev == NULL) {
    printf("Unable to open playback device!\n");
    exit(1);
  }
  devices = alcGetString(mainDev, ALC_DEVICE_SPECIFIER);
  printf("   opened device '%s'\n", devices);
  mainContext = alcCreateContext(mainDev, NULL);
  if (mainContext == NULL) {
    printf("Unable to create playback context!\n");
    exit(1);
  }
  printf("   created playback context\n");

  // Make the playback context current
  alcMakeContextCurrent(mainContext);
  alcProcessContext(mainContext);

  // Print the list of capture devices

  printf("Available capture devices:\n");
  devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
  ptr = devices;

  // while (ptr[0] != NULL)
  while (*ptr) {
    printf("   %s\n", ptr);
    ptr += strlen(ptr) + 1;
  }

  // Open the default device
  printf("Opening capture device:\n");
  captureDev = alcCaptureOpenDevice(NULL, 8000, AL_FORMAT_MONO16, 800);
  if (captureDev == NULL) {
    printf("   Unable to open device!\n");
    exit(1);
  }
  devices = alcGetString(captureDev, ALC_CAPTURE_DEVICE_SPECIFIER);
  printf("   opened device %s\n", devices);

  int samples_captured = start_capture(captureDev, captureBuffer);

  pause_capture(captureDev);

  play(captureBuffer, samples_captured);

  // Shut down OpenAL
  alcMakeContextCurrent(NULL);
  alcCloseDevice(mainDev);
  alcCaptureCloseDevice(captureDev);

  return EXIT_SUCCESS;
}