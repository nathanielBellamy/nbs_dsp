#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>
#include <fftw3.h>
#include "audio.h"
#include "audio_data.h"
#include "visual.h"
#include "../cpp/extern_c.h"
#include "visual.h"
#include "visual_data.h"

// pa.h
int init_pa(AUDIO_DATA *audioData, atomic_int *atomicCounter);
void freeAudioData(AUDIO_DATA *audioData);
void *audioMain(void *audioData);

// visual.h
void *visualMain(void *foo);

// ../cpp/extern_c.h
void bar(void);

int main(void);
int main(void) {
  bar();
  
  // init data
  atomic_int atomicCounter = ATOMIC_VAR_INIT(0);
  AUDIO_DATA audioData;
  if ( init_pa(&audioData, &atomicCounter) != 0)
  {
    freeAudioData(&audioData);
    return 1;
  };
  // NOTE:
  // - atomicEQ consists of N=audioData.buffer_frames atomic_ints in contiguous memeory
  // - we do not assume synchronization to be maintained across atomic values (aka. this is not an atomic_array)
  atomic_int* atomicEQ;
  // NOTE:
  // - This assumes stereo - aka two channels
  // - We process N=audioData.buffer_frames samples per pa_callback
  // - that gives us N/2 bands of EQ per channel
  // - given that we have two channels, we have N EQ values to share between threads
  atomicEQ = (atomic_int *) malloc(audioData.buffer_frames * sizeof(atomic_int));
  for (int i = 0; i < audioData.buffer_frames; i++)
  {
    atomicEQ[i] = ATOMIC_VAR_INIT(0);
  }
  audioData.atomicEQ = atomicEQ;

  // TODO:
  // audioData.atomicEQ = &atomicEQ;
  // visualData.atomicEQ = &atomicEQl

  // init threads
  pthread_t thread_audio, thread_visual;
  int ta_create_err = pthread_create(
    &thread_audio, 
    NULL, 
    audioMain,
    &audioData
  );
  if (ta_create_err)
  {
    fprintf(stderr, "\nError - audio pthread_create() return code: %d", ta_create_err);
    return 1;
  }

  VISUAL_DATA visualData;
  visualData.atomicCounter = &atomicCounter;
  visualData.atomicEQ = atomicEQ;
  visualData.buffer_frames = audioData.buffer_frames;

  int tv_create_err = pthread_create(
    &thread_visual,
    NULL,
    visualMain,
    &visualData
  );
  if (tv_create_err)
  {
    fprintf(stderr, "\nError - visual pthread_create() return code: %d", ta_create_err);
    return 1;
  }

  // cleanup threads
  //
  // wait for audio thread to finish
  int ta_join_err = pthread_join(thread_audio, NULL);
  if ( ta_join_err )
  {
    fprintf(stderr, "\nError - audio pthread_join() return code: %d", ta_join_err);
  }

  // cancel visual thread
  int tv_cancel_err = pthread_cancel(thread_visual);
  if ( tv_cancel_err )
  {
    fprintf(stderr, "\nError - visual pthread_join() return code: %d", tv_cancel_err);
  }

  // Cleanup
  freeAudioData(&audioData);
  return 0;
}
