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

void drawHeader(void *visualData, GraphRefHDR header, RasterRef raster)
{
  GraphRef<GraphRefHDR>
  headerTitle(
      "headerTitle",
      header,
      0,
      0
  );
  headerTitle.placeString("=== Welcome to NBSDSP-TerminalWAV === ", 2, 2);
  headerTitle.placeString("=== Glad you could be here === ", 3, 2);
  headerTitle.placeString("=== Hit ENTER to stop program. ===", 4, 2);
  headerTitle.updateText(raster);
};

void updateHeader(GraphRefHDR header, RasterRef raster, int audioFrameId)
{
  GraphRef<GraphRefHDR>
  headerAudioFrameIdDisplay(
      "headerAudioFrameId",
      header,
      0,
      0
  );
  
  std::string frameId = std::to_string(audioFrameId);

  headerAudioFrameIdDisplay.placeString("Frame: ", 5, 12);
  headerAudioFrameIdDisplay.placeString(frameId, 5, 19);
  headerAudioFrameIdDisplay.updateText(raster);
};

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

  graphRef.updateGraph(raster);
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
