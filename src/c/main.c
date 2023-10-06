#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include "pa.h"
#include "../cpp/foo.h"

// pa.h
int pa(const void *inputBuffer, int *sampleRate);

// ../cpp/foo.h
void bar(void);

int main(void);
int main(void) {
  bar();
  
  SNDFILE *file;
  SF_INFO sfinfo;

  sfinfo.format = 0;

  if (! (file = sf_open("gtfam.wav", SFM_READ, &sfinfo)))
  {
		printf ("Not able to open input file.\n") ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
    return 1 ;
  };
  
  // Display some information about the file.
  printf("Sample rate: %d \n", sfinfo.samplerate);
  printf("Channels: %d \n", sfinfo.channels);
  printf("Frames: %lli \n", sfinfo.frames);
  
  // Allocate memory for data
  float *buffer = malloc(sfinfo.frames * sfinfo.channels * sizeof(float));
  if (!buffer) {
      printf("Cannot allocate memory\n");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(file, buffer, sfinfo.frames * sfinfo.channels);
  
  printf("readcount: %ld\n", readcount);
  printf("buffer: %f\n", buffer[10]);

  // TODO:
  //  send buffer and sample/channel info to portaudio here
  if ( pa(&buffer, &sfinfo.samplerate) != 0 )
  {
    return 1;
  }

  // Cleanup
  free(buffer);
  sf_close(file);

  return 0;
}
