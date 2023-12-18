#ifndef PA_H
#define PA_H
#include <stdio.h>
#include <stdatomic.h>
#include <portaudio.h>
#include <fftw3.h>
#include "audio_data.h"

void freeAudioData(AUDIO_DATA *paData);
int init_pa(AUDIO_DATA *paData, atomic_int *atomicCounter, atomic_int *debugDisplayFlag);
void *audioMain(void *paData);

#endif
