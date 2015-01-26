#include "turtleProcessor.h"

#define TOKEN_LENGTH 100
#define NUMBER_OF_VARIABLES 26

#define NO_TESTING 0
#define TEST_WHITEBOX 1
#define TEST_BLACKBOX 2
#define TEST_ALL 3
#define TEST_WITH_SYNTAX_ERRORS 0 //set to 1 to display syntax errors during testing

typedef struct parseHelper *ParseHelper;
typedef struct variable *Variable;
typedef struct valStack *ValStack;
typedef struct valNode *ValNode;




enum mathSymbol {
  add, subtract, divide, multiply
} ;
typedef enum mathSymbol mathSymbol;

// PARSE HELPER FUNCTIONS
void createParseHelper();
ParseHelper getParseHelperPointer(ParseHelper pH);
void initialiseParseHelper(char *filePath, int testing);
void initialiseVariableList(ParseHelper pH);
void readInTokensFromFile();
void freeParseHelper();

// PARSING FUNCTIONS
void setUpForParsing(char *filePath, int testing);
int  parse();
void shutDownParsing(int testing);
int  getToken(ParseHelper pH);
TokenType whatToken(char *token);
int checkForVarNum(char * token);
int checkForAnyVar(char * token);
int  processMain(ParseHelper pH);
int  processInstrctList(ParseHelper pH);
int  processInstruction(ParseHelper pH);
int  processVarNum(ParseHelper pH);
int  processSet(ParseHelper pH);
int  processPolish(ParseHelper pH);
int  processOperator(ParseHelper pH);
int  finishPolish(ParseHelper pH);
int  processDo(ParseHelper pH);

int checkValidVariable(char c, ParseHelper pH);
int checkForNumber(char *token, ParseHelper pH);
int checkVariableAssigned(char c, ParseHelper pH);
int checkValidOperator(char c, ParseHelper pH);
void assignValToCurrentVariable(ParseHelper pH);
void assignValToVariable(ParseHelper pH, char varToSet, double val);
double getTokenVal(ParseHelper pH);
double getVariableVal(ParseHelper pH, char c);

int sameString(char *a, char *b);
int syntaxError(ParseHelper pH, char *message);

double doMaths(double a, double b, mathSymbol op);

// VAL STACK FUNCTIONS
void createValStack();
ValStack getValStackPointer(ValStack newStack);
void pushToValStack(double val);
ValNode newValNode();
int popFromValStack(double *poppedVal);
int getNumberOfValsOnStack();
void freeValStack();

// TESTING FUNCTIONS
void runParserWhiteBoxTests();
void testHelperInitialisation();
void testSyntaxErrors();
void testVarNum();
void testSetCommand();
void testValStack();
void testDOloops();
