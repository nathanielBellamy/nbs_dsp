#ifndef EXTERN_C_H
#define EXTERN_C_H

#ifdef __cplusplus
extern "C" {
#endif

// must match definition in src/c/visual.h
#define RASTER_SIDE_LENGTH 156

void bar();
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
  void *visualData,
  char (*header)[16][RASTER_SIDE_LENGTH],
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH]
);
void updateHeader(
  char (*header)[16][RASTER_SIDE_LENGTH],
  char (*raster)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH],
  int audioFrameId,
  int debugDisplayFlag
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

#ifdef __cplusplus
}
#endif

#endif

