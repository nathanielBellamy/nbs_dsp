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

void updateGraph(
  float* bufferAtomicEQ_norm,
  char (*graphCurr)[30][80],
  char (*graphNext)[30][80],
  void* settingsIn
)
{
  Settings *settings;
  settings = static_cast<Settings*>(settingsIn);

  for (int i = 0; i < 30; i++) // row
  {
    Draw::createRowPiecewise(
      bufferAtomicEQ_norm, 
      i, 
      settings,
      graphNext
    );
  }

  for (int i = 0; i < 30; i++) // row
  {
    for (int j = 0; j < 80; j++) // column
    {
      if ((*graphCurr)[i][j] != (*graphNext)[i][j])
      {
        (*graphCurr)[i][j] = (*graphNext)[i][j];
        // printf("\x1b[%i;%iH", i, j);
        // printf("%c", (*graphCurr)[i][j]);
      }
    }
  }
};

// void drawGraph(float* bufferAtomicEQ_norm, void* settingsIn)
// {
//   Draw draw;
//   Settings *settings;
//   settings = static_cast<Settings*>(settingsIn);

//   settings->yMax = 1.26;
//   settings->yMin = -0.1;
//   settings->epsilon = 0.05;
//   settings->displayHeight = 30;
//   settings->displayWidth = 90;
//   settings->stepHeight = Compute::stepHeight(settings);
//   settings->stepWidth = Compute::stepWidth(settings);
//   settings->xStepCount = Compute::xStepCount(settings);

//   vector<double> zeroPolynomialOfMaxDegree(1, 0);
//   vector<vector<double> > polynomialArray_L(16, zeroPolynomialOfMaxDegree);
//   vector<vector<double> > polynomialArray_R(16, zeroPolynomialOfMaxDegree);

//   // int counterMod;
//   // counterMod = counter % 1000;
//   // double skew;
//   // skew = ((-1.0 * counterMod) * (counterMod - 1000))/10000;
//   // skew = (1.0 * counterMod) / 3000;

//   for (int i = 1; i < 17; i++)
//   {
//     polynomialArray_L.at(i - 1).at(0) = bufferAtomicEQ_norm[i];
//   }
//   draw.renderPiecewise(polynomialArray_L, settings);
//   printf("\n ==>  <== \n");
//   // printf("\n ==>1,2,3<== ==>%f, %f, %f<===\n", bufferAtomicEQ_norm[2], bufferAtomicEQ_norm[5], bufferAtomicEQ_norm[8]);
// };

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
