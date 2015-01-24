#include "interpreter.h"

#define TOKEN_LENGTH 100
#define NUMBER_OF_VARIABLES 26
#define NUMBER_OF_OPERATORS 4

#define TESTING 1
#define NO_TESTING 0

typedef struct parseHelper *ParseHelper;
typedef struct variable *Variable;
typedef struct valStack *ValStack;
typedef struct valNode *ValNode;



enum tokenType {
  instrctlist, instruction, fd, lt, rt, varnum, set, polish, op
} ;
typedef enum tokenType TokenType;


// PARSE HELPER FUNCTIONS
void createParseHelper();
ParseHelper getParseHelperPointer(ParseHelper pH);
void initialiseParseHelper(char *filePath, int testing);
void initialiseVariableList(ParseHelper pH);
void freeParseHelper();

// PARSiNG FUNCTIONS
void setUpForParsing();
int  parse();
void shutDownParsing();
int  getToken(ParseHelper pH);
int  expect(TokenType tType);
int  processInstrctList(ParseHelper pH);
int  processInstruction(ParseHelper pH);
int  processSet(ParseHelper pH);
int  processPolish(ParseHelper pH);

int checkValidVariable(char c, ParseHelper pH);
int checkForNumber(ParseHelper pH);
int checkVariableAssigned(char c, ParseHelper pH);
void printSyntaxError(ParseHelper pH, char *message);

// VAL STACK FUNCTIONS
void createValStack();
ValStack getValStackPointer(ValStack newStack);
void pushToValStack(double val);
ValNode newValNode();
int popFromValStack(double *poppedVal);
void freeValStack();

// TESTING FUNCTIONS
void runParserWhiteBoxTests();
void testHelperInitialisation();
void testSyntaxErrors();
void testSetCommand();
void testValStack();
