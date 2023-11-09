
#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdatomic.h>

typedef struct {
		char backgroundChar;
		char originChar;
		char xAxisChar;
		char yAxisChar;
		double xMin;
		double xMax;
		double yMin;
		double yMax;
		int	displayHeight;
		int	displayWidth;
		double stepHeight;
    double stepWidth;
    int xStepCount;
    double epsilon;
    bool pieceWise;
} SETTINGS;

#endif
