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

void drawGraph(int counter)
{
  Draw draw;
  Settings settings;

  vector<double> zeroPolynomialOfMaxDegree(1, 0);
  vector<vector<double> > polynomialArray(16, zeroPolynomialOfMaxDegree);

  int counterMod;
  counterMod = counter % 1000;
  double skew;
  // skew = ((-1.0 * counterMod) * (counterMod - 1000))/10000;
  skew = (1.0 * counterMod) / 3000;

  // if ( counterMod < 250 )
  // {
    polynomialArray.at(0).at(0) = -0.75 + skew;
    polynomialArray.at(1).at(0) = 0.5 + skew;
    polynomialArray.at(2).at(0) = -1 + skew;
    polynomialArray.at(3).at(0) = 1 + skew;
    polynomialArray.at(4).at(0) = -0.75 + skew;
    polynomialArray.at(5).at(0) = 0.5 + skew;
    polynomialArray.at(6).at(0) = -1 + skew;
    polynomialArray.at(7).at(0) = 1 + skew;
    polynomialArray.at(8).at(0) = -0.75 + skew;
    polynomialArray.at(9).at(0) = 0.5 + skew;
    polynomialArray.at(10).at(0) = -1 + skew;
    polynomialArray.at(11).at(0) = 1 + skew;
    polynomialArray.at(12).at(0) = -0.75 + skew;
    polynomialArray.at(13).at(0) = 0.5 + skew;
    polynomialArray.at(14).at(0) = -1 + skew;
    polynomialArray.at(15).at(0) = 1 + skew;
  // }

  // if ( counterMod > 250 && counterMod <= 500)
  // {
  //   polynomialArray.at(0).at(0) = -0.75 + 1.5 * skew;
  //   polynomialArray.at(1).at(0) = 0.5 + 1.5 * skew;
  //   polynomialArray.at(2).at(0) = -1 + 1.5 * skew;
  //   polynomialArray.at(3).at(0) = 1 + 1.5 * skew;
  // }

  // if ( counterMod > 500 && counterMod <= 750)
  // {
  //   polynomialArray.at(0).at(0) = -0.75 + 2.0 * skew;
  //   polynomialArray.at(1).at(0) = 0.5 + 2.0 * skew;
  //   polynomialArray.at(2).at(0) = -1 + 2.0 * skew;
  //   polynomialArray.at(3).at(0) = 1 + 2.0 * skew;
  // }

  // if ( counterMod > 750 && counterMod <= 1000)
  // {
  //   polynomialArray.at(0).at(0) = -0.75 + 2.5 * skew;
  //   polynomialArray.at(1).at(0) = 0.5 + 2.5 * skew;
  //   polynomialArray.at(2).at(0) = -1 + 2.5 * skew;
  //   polynomialArray.at(3).at(0) = 1 + 2.5 * skew;
  // }
  draw.renderPiecewise(polynomialArray, settings);
};
