#ifndef EXTERN_C_H
#define EXTERN_C_H

#ifdef __cplusplus
extern "C" {
#endif

void bar();
void drawBorder(
  char (*raster)[156][156],
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
  char (*header)[16][156],
  char (*raster)[156][156]
);
void updateHeader(
  char (*header)[16][156],
  char (*raster)[156][156],
  int audioFrameId
);
void updateGraph(
  double (*polynomialArray)[16][16],
  char (*raster)[156][156],
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

