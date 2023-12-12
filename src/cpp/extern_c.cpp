#include<iostream>
#include<string>
#include "extern_c.h"
#include "../cpp/PolynomialConsoleGraph/src/Compute.h"
#include "../cpp/PolynomialConsoleGraph/src/Draw.h"
#include "../cpp/PolynomialConsoleGraph/src/Settings.h"
#include "Cli.h"
#include "GraphRef.h"
#include "SF_INFO.h"

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

  headerAudioFrameIdDisplay.placeString("Frame: ", 5, 22);
  headerAudioFrameIdDisplay.placeString(frameId, 5, 29);
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
