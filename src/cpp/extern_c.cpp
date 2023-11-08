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
  float* bufferAtomicEQ_avg,
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
      bufferAtomicEQ_avg, 
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
