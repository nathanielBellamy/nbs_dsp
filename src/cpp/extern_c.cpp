#include<iostream>
#include<string>
#include "extern_c.h"
#include "./PolynomialConsoleGraph/src/Compute.h"
#include "./PolynomialConsoleGraph/src/Draw.h"
#include "./PolynomialConsoleGraph/src/Settings.h"
#include "Cli.h"
#include "GraphRef.h"
#include "SF_INFO.h"
#include "Constants.h"

using namespace std;

void bar() 
{
  printf("Hello C! Yours truly, Cpp. \n");
  Cli::intro();
};

void drawBorder(
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH],
  char charVert,
  char charHoriz,
  char charCorner,
  int height,
  int width,
  int offsetY,
  int offsetX
)
{
  char patch[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH];
  // TODO: bring in <vector.h> ?
  GraphRef<RasterRef>
  border(
      "border",
      &patch,
      offsetY,
      offsetX
  );

  // boundary-top-bottom
  for (int x = offsetX + 2; x < offsetX + width ; x++)
  {
    patch[offsetY][x] = charHoriz;
    patch[offsetY + height][x] = charHoriz;
  }

  // boundary-left-right
  for (int y = offsetY + 2; y < offsetY + height; y++)
  {
    patch[y][offsetX] = charVert;
    patch[y][offsetX + width] = charVert;
  }

  // corners
  {
    patch[offsetY][offsetX] = charCorner;
    patch[offsetY][offsetX + width] = charCorner;
    patch[offsetY + height][offsetX] = charCorner;
    patch[offsetY + height][offsetX + width] = charCorner;
  }

  border.updateText(raster);
};

void drawHeader(void *sfInfo_, GraphRefHDR header, RasterRef raster)
{
  GraphRef<GraphRefHDR>
  headerTitle(
      "headerTitle",
      header,
      0,
      0
  );
  headerTitle.placeString("=== Welcome to NBSDSP-TerminalWAV === ", 2, 12);
  headerTitle.placeString("=== Glad you could be here === ", 3, 12);
  headerTitle.placeString("=== Hit ENTER to stop program. ===", 4, 12);

  SF_INFO *sfInfo = (SF_INFO *) sfInfo_;

  int offsetXLabel = 90;
  int offsetXValue = 110;
  int offsetYBase = 2;

  string frameCount = std::to_string(sfInfo->frames);
  headerTitle.placeString("=== Frames: ", offsetYBase, offsetXLabel);
  headerTitle.placeString(frameCount, offsetYBase, offsetXValue);

  string sampleRate = std::to_string(sfInfo->samplerate);
  headerTitle.placeString("=== SampleRate: ", offsetYBase + 1, offsetXLabel);
  headerTitle.placeString(sampleRate, offsetYBase + 1, offsetXValue);

  string channels = std::to_string(sfInfo->channels);
  headerTitle.placeString("=== Channels: ", offsetYBase + 2, offsetXLabel);
  headerTitle.placeString(channels, offsetYBase + 2, offsetXValue);


  string format = std::to_string(sfInfo->format);
  headerTitle.placeString("=== Format: ", offsetYBase + 3, offsetXLabel);
  headerTitle.placeString(format, offsetYBase + 3, offsetXValue);

  headerTitle.updateText(raster);
};

void updateHeader(GraphRefHDR header, RasterRef raster, int audioFrameId, DBG* debug)
{
  GraphRef<GraphRefHDR>
  headerAudioFrameIdDisplay(
      "headerAudioFrameId",
      header,
      0,
      0
  );
  
  std::string frameId = std::to_string(audioFrameId);

  std::string debugDouble = std::to_string(debug->double_);
  std::string debugInt = std::to_string(debug->int_);
  std::string debugFloat = std::to_string(debug->float_);

  headerAudioFrameIdDisplay.placeString("Frame: ", 5, 22);
  headerAudioFrameIdDisplay.placeString(frameId, 5, 29);

  headerAudioFrameIdDisplay.placeString("Debug I: ", 5, 45);
  headerAudioFrameIdDisplay.placeString(debugInt, 5, 58);

  headerAudioFrameIdDisplay.placeString("Debug F: ", 6, 45);
  headerAudioFrameIdDisplay.placeString(debugFloat, 6, 58);
 
  headerAudioFrameIdDisplay.placeString("Debug D: ", 7, 45);
  headerAudioFrameIdDisplay.placeString(debugDouble, 7, 58);

  headerAudioFrameIdDisplay.updateText(raster);
};

void updateGraph(
  double (*polynomialArray)[POLYNOMIAL_ARRAY_LENGTH][POLYNOMIAL_DEGREE_P1],
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH],
  char (*graphNextIn)[EQ_IMAGE_HEIGHT][EQ_IMAGE_WIDTH],
  int offsetX,
  int offsetY,
  void* settingsIn
)
{
  Settings *settings;
  settings = static_cast<Settings*>(settingsIn);

  GraphRefEqIm graphNext;
  graphNext = static_cast<GraphRefEqIm>(graphNextIn);

	double image[EQ_IMAGE_WIDTH] = { 0 };
	Compute::piecewiseImage(polynomialArray, &image, settings);

  for (int i = 0; i < EQ_IMAGE_HEIGHT; i++) // row
  {
    Draw::createRowPiecewise(
      &image,
      i, 
      settings,
      graphNext
    );
  }

  GraphRef<GraphRefEqIm> 
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
  return Compute::stepHeight(settings);
};
