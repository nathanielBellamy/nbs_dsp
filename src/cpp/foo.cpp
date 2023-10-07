#include<iostream>
#include<string>
#include "foo.h"
#include "Cli.h"

using namespace std;

void bar() {
  printf("Hello C! Yours truly, Cpp. \n");
  Cli::intro();
};
