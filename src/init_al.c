#include <stdio.h>
#include <stdlib.h>

#include "init_al.h"


ALCdevice *open_playback_device() {

  // Open a playback device and create a context first
  printf("Opening playback device:\n");
  ALCdevice *playback_device = alcOpenDevice(NULL);
  if (playback_device == NULL) {
    printf("Unable to open playback device!\n");
    exit(1);
  }
  const ALCchar *devices = alcGetString(playback_device, ALC_DEVICE_SPECIFIER);
  printf("   opened device '%s'\n", devices);

  return playback_device;
}

ALCdevice *open_capture_device(){

  // Open the default device
  printf("Opening capture device:\n");
  ALCdevice *capture_device = alcCaptureOpenDevice(NULL, 8000, AL_FORMAT_MONO16, 800);
  if (capture_device == NULL) {
    printf("   Unable to open device!\n");
    exit(1);
  }
  const ALCchar *devices = alcGetString(capture_device, ALC_CAPTURE_DEVICE_SPECIFIER);
  printf("   opened device %s\n", devices);

}