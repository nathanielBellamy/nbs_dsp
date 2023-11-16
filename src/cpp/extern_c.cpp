#include<iostream>
#include<string>
#include "extern_c.h"
#include "../cpp/PolynomialConsoleGraph/src/Compute.h"
#include "../cpp/PolynomialConsoleGraph/src/Draw.h"
#include "../cpp/PolynomialConsoleGraph/src/Settings.h"
#include "Cli.h"

using namespace std;

void bar() 
{
  printf("Hello C! Yours truly, Cpp. \n");
  Cli::intro();
};

// TODO:
//  -[ ] Printer.cpp
//    -[ ] pass in variables from C
//    -[ ] keep track of what is displayed where
//    -[ ] offer an update api

// TODO:
// -[X] updateGraph accepts an array doubles instead of a float
//   -[X] move cast into C
// -[X] prep L and R arrays in C and pass into updateGraph
// -[X] use offsetX and offsetY to align the graphs
// -[ ] reduce graph width 80 -> 64
// -[ ] use a belowChar to fill in EQ bar integrals
//
void updateGraph(
  double (*polynomialArray)[16][16],
  char (*graphCurr)[30][80],
  char (*graphNext)[30][80],
  int offsetX,
  int offsetY,
  void* settingsIn
)
{
  Settings *settings;
  settings = static_cast<Settings*>(settingsIn);

	double image[80];
  for (int i = 0; i < 80; i++)
  {
      image[i] = 0;
  }
	Compute::piecewsieImage(polynomialArray, &image, settings);

  for (int i = 0; i < 30; i++) // row
  {
    Draw::createRowPiecewise(
      &image,
      i, 
      settings,
      graphNext
    );
  }

  for (int i = 29; i > -1; i--) // draws from top to bottom
  {
    for (int j = 0; j < 80; j++)
    {
      if ((*graphCurr)[i][j] != (*graphNext)[i][j])
      {
        (*graphCurr)[i][j] = (*graphNext)[i][j];
        printf("\033[%d;%dH", 30 - i + offsetY, j + offsetX); // move to char location
        printf("%c", (*graphCurr)[i][j]); // update char on screen
      }
    }
  }
};

int xStepCount(void *settingsIn)
{
  Settings *settings;
  settings = static_cast<Settings*>(settingsIn);
  return Compute::xStepCount(settings);
};

double stepWidth(void *settingsIn)
{
  Settings *settings;
  settings = static_cast<Settings*>(settingsIn);
  return Compute::stepWidth(settings);
};

double stepHeight(void *settingsIn)
{
  Settings *settings;
  settings = static_cast<Settings*>(settingsIn);
  return Compute::stepWidth(settings);
};
