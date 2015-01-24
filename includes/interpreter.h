#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/sput.h"

typedef enum mathSymbol {
  add, subtract, divide, multiply
} mathSymbol;

int doMaths(int a, int b, mathSymbol op);


void runInterpreterWhiteBoxTests();
