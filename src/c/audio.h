#ifndef PA_H
#define PA_H
#include <stdio.h>
#include <portaudio.h>
#include <fftw3.h>
#include "pa_data.h"

void freePaData(PA_DATA *paData);
int init_pa(PA_DATA *paData);
void *audioMain(void *paData);

#endif
