#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include "pa.h"
#include "pa_data.h"
#include "../cpp/foo.h"

// pa.h
int pa(PA_DATA *paData);

// ../cpp/foo.h
void bar(void);

int main(void);
int main(void) {
  bar();
  
  SNDFILE *file;
  SF_INFO sfinfo;

  PA_DATA paData;

  sfinfo.format = 0;

  if (! (paData.file = sf_open("gtfam.wav", SFM_READ, &paData.sfinfo)))
  {
		printf ("Not able to open input file.\n") ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
    return 1 ;
  };
  
  // Display some information about the file.
  printf("Sample rate: %d \n", paData.sfinfo.samplerate);
  printf("Channels: %d \n", paData.sfinfo.channels);
  printf("Frames: %lli \n", paData.sfinfo.frames);
  
  // Allocate memory for data
  paData.buffer = malloc(paData.sfinfo.frames * paData.sfinfo.channels * sizeof(float));
  if (!paData.buffer) {
      printf("Cannot allocate memory\n");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(paData.file, paData.buffer, paData.sfinfo.frames * paData.sfinfo.channels);
  
  printf("readcount: %ld\n", readcount);
  printf("buffer: %f\n", paData.buffer[10]);

  // TODO:
  //  send buffer and sample/channel info to portaudio here
  if ( pa(&paData) != 0 )
  {
    return 1;
  }

  // Cleanup
  free(paData.buffer);
  sf_close(file);

  return 0;
}
