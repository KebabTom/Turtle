#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/sput.h"

#define TOKEN_LENGTH 100
#define NUMBER_OF_VARIABLES 26

#define TESTING 1
#define NO_TESTING 0

typedef struct parseHelper *ParseHelper;
typedef struct variable *Variable;


enum tokenType {
  instrctlist, instruction, fd, lt, rt, varnum
} ;
typedef enum tokenType TokenType;


// PARSE HELPER FUNCTIONS
void createParseHelper();
ParseHelper getParseHelperPointer(ParseHelper pH);
void initialiseParseHelper(char *filePath, int testing);
void initialiseVariableList(ParseHelper pH);
void freeParseHelper();

// PARSiNG FUNCTIONS
void parse();
void getToken(ParseHelper pH);
int expect(TokenType tType);


// TESTING FUNCTIONS
void runParserWhiteBoxTests();
void testHelperInitialisation();
