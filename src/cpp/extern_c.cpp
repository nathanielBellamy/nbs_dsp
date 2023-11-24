#include<iostream>
#include<string>
#include "extern_c.h"
#include "../cpp/PolynomialConsoleGraph/src/Compute.h"
#include "../cpp/PolynomialConsoleGraph/src/Draw.h"
#include "../cpp/PolynomialConsoleGraph/src/Settings.h"
#include "Cli.h"
#include "GraphRef.h"

using namespace std;

void bar() 
{
  printf("Hello C! Yours truly, Cpp. \n");
  Cli::intro();
};

void drawHeader(void *visualData)
{

  GraphRefS headerTitle;
  // TODO: 
  //  - method to input string into a Graph
  //  - set offset and color
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
// -[X] reduce graph width 80 -> 64
// -[ ] use a belowChar to fill in EQ bar integrals
//
void updateGraph(
  double (*polynomialArray)[16][16],
  RasterRef raster,
  GraphRefM graphNext,
  int offsetX,
  int offsetY,
  void* settingsIn
)
{
  Settings *settings;
  settings = static_cast<Settings*>(settingsIn);

	double image[64] = { 0 };
	Compute::piecewsieImage(polynomialArray, &image, settings);

  for (int i = 0; i < 32; i++) // row
  {
    Draw::createRowPiecewise(
      &image,
      i, 
      settings,
      graphNext
    );
  }

  GraphRef<GraphRefM> 
  graphRef(
      "foo",
      graphNext,
      offsetX,
      offsetY
  );

  graphRef.update(raster);
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
