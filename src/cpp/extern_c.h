#ifndef EXTERN_C_H
#define EXTERN_C_H

#ifdef __cplusplus
extern "C" {
#endif

void bar();
void drawGraph(float* counter);
int xStepCount(void* settings);
double stepWidth(void* settings);
double stepHeight(void* settings);

#ifdef __cplusplus
}
#endif

#endif

