#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include "dbg.h"
#include "settings.h"
#include "visual.h"
#include "visual_data.h"
#include "../cpp/extern_c.h"

// extern_c.h
void drawBorder(
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH],
  char charVert,
  char charHoriz,
  char charCorner,
  int height,
  int width,
  int offsetY,
  int offsetX
);
void drawHeader(
  void* visualData,
  char (*header)[16][RASTER_SIDE_LENGTH],
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH]
);
void updateHeader(
  char (*header)[16][RASTER_SIDE_LENGTH],
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH],
  int audioFrameId,
  DBG* debug
);
void updateGraph(
  double (*polynomialArray)[16][16],
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH],
  char (*graphNext)[32][64],
  int offsetX,
  int offsetY,
  void *settingsIn
);
int xStepCount(void* settings);
double stepWidth(void* settings);
double stepHeight(void* settings);

int loadAtomicEq(VISUAL_DATA* visualData, int index)
{
  // values are shared between threads as atomic_ints
  // we convert them back and forth from floats using a factor of 1000.0
  return atomic_load(visualData->atomicEQ + index);
}

#define FRAME_RATE 1250000 // how frequently we render
#define LOAD_RATE 1250000  // how frequently we load EQ data from the audio thread

void *visualMain(void *visualData_) 
{
  DBG debug;

  //setup STARe
  int frameCounter = -1; // we eager increment so this makes 0 is first value used by loop
  int loadCounter = -1;

  int smoothing_t = -1;

  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  // NOTE:
  // - values are loaded from the visual thread into bufferAtomicEq_targ
  // - they are then normalized in place
  // - on loops when new values are not fetched, the values in bufferAtomicEq_curr
  //   are incrementally shifted index-wise toward the values in bufferAtomicEq_targ
  //   in a straight-line homotopy
  //
  // 34 = 2 * audioData->buffer_frames_d2p1
  int bufferAtomicEq_load[34] = { 0.0 };
  double bufferAtomicEq_targ[34] = { 0.0 };
  float bufferAtomicEq_curr[34] = { 0.0 };

  // TODO:
  // - get and update settings from user
  SETTINGS settings;
	settings.backgroundChar= '.';
	settings.originChar = '+';
	settings.xAxisChar = '-';
	settings.yAxisChar = '|';
	settings.xMin = -1.3;
	settings.xMax = 1.3;
	settings.yMin = -0.0;
	settings.yMax = 1.0;
	settings.epsilon = 0.05;
  settings.displayWidth = 64;
  settings.displayHeight = 32;
  settings.stepWidth = stepWidth((void *) &settings);
  settings.stepHeight = stepHeight((void *) &settings);
  settings.xStepCount = xStepCount((void *) &settings);

  char raster[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH] = {{'\0'}};

  char graphNextL[32][64] = {{'L'}};
  char graphNextR[32][64] = {{'R'}};
  
  double polynomialArrayL[16][16] = {{ 0.0 }};
  double polynomialArrayR[16][16] = {{ 0.0 }};

  //
  // RENDER
  // 
  system("clear");

  // hide cursor
  // printf("\e[?25l");

  char header[16][RASTER_SIDE_LENGTH] = {{ '\0' }};
  
  drawBorder(&raster, '|', '~', '=', 8, 156, 0, 0);
  drawHeader((void*) &visualData->audioData->sfinfo, &header, &raster);

  // TODO:
  //   -[X] L + R graphs
  //   -[X] header
  //   -[ ] footer
  //   -[X] border
  //   -[ ] pick a band
  //   -[ ] increase/decrease boost with up/down keys


  // setup END
  // init playback loop
  while( true )
  {
    frameCounter += 1;
    loadCounter += 1;

    // load EQ data from audio thread
    if (loadCounter == LOAD_RATE) 
    {
      // TODO: 
        // - rework smoothing
        // - smoothing shouldn't average many values
        // - it should sample values
        // - and use PCG to start moving toward new value from old inbetween taking samples
      int maxMag[2] = { 1 }; // - each time we load we need to normalize 
                               //   all values by the max value for that channel
        //

        // load data from visual thread
      for (int i = 0; i < visualData->buffer_frames_d2p1; i++)
      {
        for (int ch = 0; ch < 2; ch++)
        {
          int index = i + (ch * visualData->buffer_frames_d2p1);
          int loadedVal = atomic_load(visualData->atomicEQ + ( index ) );
          if (loadedVal > 0 && loadedVal < 1000)
          {
            bufferAtomicEq_load[index] = loadedVal;
          }
          if ( loadedVal >= maxMag[ch] && loadedVal > 0 && loadedVal < 10000 )
          {
            maxMag[ch] = bufferAtomicEq_load[index];
          }
        }
      }


      // normalize loaded data while copying into render target array
      for (int i = 0; i < visualData->buffer_frames_d2p1; i++)
      {
        for (int ch = 0; ch < 2; ch++)
        {
          int index = i + (ch * visualData->buffer_frames_d2p1);
          bufferAtomicEq_targ[index] = (bufferAtomicEq_load[index]) / ((double)maxMag[ch]);
          debug.int_ = bufferAtomicEq_load[3];
          debug.float_ = (float) maxMag[0];
          debug.double_ = bufferAtomicEq_targ[3];
        }
      }

      
      loadCounter = 0;
    }
    
    if ( frameCounter == FRAME_RATE )
    {
      smoothing_t += 1;

      // float t = (float) smoothing_t / (float) FRAME_RATE;

      // prep polynomials to graph
      for (int i = 0; i < 16; i++)
      {
        // polynomialArrayL[i][0] = (1.0 - t) * bufferAtomicEq_curr[i + 1] + t * bufferAtomicEq_targ[i + 1];
        polynomialArrayL[i][0] = bufferAtomicEq_targ[i + 1];
      }

      for (int i = 0; i < 16; i++)
      {
        // polynomialArrayR[i][0] = (1.0 - t) * bufferAtomicEq_curr[i + 17] + t * bufferAtomicEq_targ[i + 17];
        polynomialArrayR[i][0] = bufferAtomicEq_targ[i + 17];
      }

      // prep audioFrameId to display in header
      int audioFrameId = atomic_load(visualData->atomicCounter);
      updateHeader(&header, &raster, audioFrameId, &debug);

      updateGraph(
        &polynomialArrayL,
        &raster,
        &graphNextL,
        12,
        5,
        (void *) &settings
      );

      updateGraph(
        &polynomialArrayR,
        &raster,
        &graphNextR,
        12,
        80,
        (void *) &settings
      );

      frameCounter = 0;
    }
  }

  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
