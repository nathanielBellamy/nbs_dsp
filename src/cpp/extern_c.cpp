#include<iostream>
#include<string>
#include "extern_c.h"
#include "../cpp/PolynomialConsoleGraph/src/Draw.h"
#include "../cpp/PolynomialConsoleGraph/src/Settings.h"
#include "Cli.h"

using namespace std;

void bar() 
{
  printf("Hello C! Yours truly, Cpp. \n");
  Cli::intro();
};

void drawGraph(float* bufferAtomicEQ_norm)
{
  Draw draw;
  Settings settings;
  settings.yMax = 1.5;
  settings.yMin = -1.5;
  settings.epsilon = 0.05;
  settings.displayHeight = 30;

  vector<double> zeroPolynomialOfMaxDegree(1, 0);
  vector<vector<double> > polynomialArray_L(32, zeroPolynomialOfMaxDegree);
  vector<vector<double> > polynomialArray_R(32, zeroPolynomialOfMaxDegree);

  // int counterMod;
  // counterMod = counter % 1000;
  // double skew;
  // skew = ((-1.0 * counterMod) * (counterMod - 1000))/10000;
  // skew = (1.0 * counterMod) / 3000;

  for (int i = 1; i < 17; i++)
  {
    polynomialArray_L.at(i - 1).at(0) = bufferAtomicEQ_norm[i];
  }
  draw.renderPiecewise(polynomialArray_L, settings);
  printf("\n ==>1,2,3<== ==>%f, %f, %f<===\n", bufferAtomicEQ_norm[6], bufferAtomicEQ_norm[7], bufferAtomicEQ_norm[8]);
};
