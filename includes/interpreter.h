#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/sput.h"

typedef enum mathSymbol {
  add, subtract, divide, multiply
} mathSymbol;

double doMaths(double a, double b, mathSymbol op);


void runInterpreterWhiteBoxTests();
