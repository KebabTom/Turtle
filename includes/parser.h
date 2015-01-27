#include "interpreter.h"

#define TOKEN_LENGTH 100
#define NUMBER_OF_VARIABLES 26

#define TEST_WITH_SYNTAX_ERRORS 0 //set to 1 to display syntax errors during testing

typedef struct parseHandler *ParseHandler;
typedef struct variable *Variable;
typedef struct valStack *ValStack;
typedef struct valNode *ValNode;




enum mathSymbol {
  add, subtract, divide, multiply
} ;
typedef enum mathSymbol mathSymbol;

// PARSE HANDLER FUNCTIONS
void createParseHandler();
ParseHandler getParseHandlerPointer(ParseHandler pH);
void initialiseParseHandler(char *filePath, int testing);
void initialiseVariableList(ParseHandler pH);
void readInTokensFromFile();
void freeParseHandler();

// PARSING FUNCTIONS
void setUpForParsing(char *filePath, int testing);
int  parse();
void shutDownParsing(int testing);
int  getToken(ParseHandler pH);
TokenType whatToken(char *token);
int  checkForVarNum(char * token);
int  checkForAnyVar(char * token);
int  checkForInstruction(char *chkToken);
int  processMain(ParseHandler pH);
int  checkForEndOfCode(ParseHandler pH);
int  processInstructionList(ParseHandler pH);
int  processInstruction(ParseHandler pH);
int  processVarNum(ParseHandler pH);
int  processSet(ParseHandler pH);
int  processPolish(ParseHandler pH);
int  processOperator(ParseHandler pH);
int  finishPolish(ParseHandler pH);
int  processDo(ParseHandler pH);
int  processWhile(ParseHandler pH);
int  processColour(ParseHandler pH);

int checkValidVariable(char c, ParseHandler pH);
int checkForNumber(char *token, ParseHandler pH);
int checkVariableAssigned(char c, ParseHandler pH);
int checkValidOperator(char *c, ParseHandler pH);
int checkForColour(char *token, ParseHandler pH);

void assignValToVariable(ParseHandler pH, char varToSet, double val);
double getTokenVal(ParseHandler pH);
double getVariableVal(ParseHandler pH, char c);

int sameString(char *a, char *b);
int syntaxError(ParseHandler pH, char *message);

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
void testHandlerInitialisation();
void testSetAssignment();
void testValStack();
void testDOloops();
