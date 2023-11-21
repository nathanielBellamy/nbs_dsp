#ifndef EXTERN_C_H
#define EXTERN_C_H

#ifdef __cplusplus
extern "C" {
#endif

void bar();
void drawHeader(void* visualData);
void updateGraph(
  double (*polynomialArray)[16][16],
  char (*raster)[256][256],
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

