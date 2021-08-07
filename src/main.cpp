#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "AudioPlayer.h"
#include "AudioRecorder.h"

using namespace std;

time_t currentTime;

int main()
{
    AudioRecorder *audioRecorder = new AudioRecorder();
    AudioPlayer *audioPlayer = new AudioPlayer(audioRecorder->captureDevice, 44100);

    currentTime = time(NULL);
    /// the main recording loop
    /// Records for 3 seconds

    printf("INFO::START RECORDING AUDIO\n");

    while (time(NULL) - currentTime < 3)
    {
        audioRecorder->startRecord();
    }

    printf("\n  Done capturing ... \n ");

    audioRecorder->stopRecord();

    currentTime = time(NULL);

    sleep(2);
    // Play back the captured data
    printf("Starting playback...\n");
    fflush(stdout);

    printf("\n Finished playback ... \n");
    fflush(stdout);

    return EXIT_SUCCESS;
}