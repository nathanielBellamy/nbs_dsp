#include<iostream>
#include<string>
#include "extern_c.h"
#include "Cli.h"

using namespace std;

void bar() {
  printf("Hello C! Yours truly, Cpp. \n");
  Cli::intro();
};
