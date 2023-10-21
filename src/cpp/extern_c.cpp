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
  // TODO:
  // settings = Settings::default(); 
  //
  vector<double> zeroPolynomialOfMaxDegree(4, 0);
  vector<vector<double> > polynomialArray(1, zeroPolynomialOfMaxDegree);

  int counterMod;
  counterMod = counter % 1000;

  if ( counterMod < 250 )
  {
    polynomialArray.at(0).at(0) = -0.75;
    polynomialArray.at(0).at(1) = 0.5;
    polynomialArray.at(0).at(2) = -1;
    polynomialArray.at(0).at(3) = 2;
  }

  if ( counterMod > 250 && counterMod <= 500)
  {
    polynomialArray.at(0).at(0) = -0.25;
    polynomialArray.at(0).at(1) = 0.5;
    polynomialArray.at(0).at(2) = -1;
    polynomialArray.at(0).at(3) = 2;
  }

  if ( counterMod > 500 && counterMod <= 750)
  {
    polynomialArray.at(0).at(0) = 0.25;
    polynomialArray.at(0).at(1) = 0.5;
    polynomialArray.at(0).at(2) = -1;
    polynomialArray.at(0).at(3) = 2;
  }

  if ( counterMod > 750 && counterMod <= 1000)
  {
    polynomialArray.at(0).at(0) = 0.75;
    polynomialArray.at(0).at(1) = 0.5;
    polynomialArray.at(0).at(2) = -1;
    polynomialArray.at(0).at(3) = 2;
  }
  // TODO:
  // pass in atomic Counter
  // polynomialArray = constructPolynomialArray(counter)
  draw.render(polynomialArray, settings);
};
