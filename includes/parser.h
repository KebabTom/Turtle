#include "interpreter.h"

#define TOKEN_LENGTH 100

#define TEST_WITH_SYNTAX_ERRORS 0 //set to 1 to display syntax errors during testing

typedef struct parseHandler *ParseHandler;


// PARSE HANDLER FUNCTIONS
void createParseHandler();
ParseHandler getParseHandlerPointer(ParseHandler pH);
void initialiseParseHandler(char *filePath, int testMode);
void readInTokensFromFile();
void freeParseHandler();

// PARSING FUNCTIONS
void setUpForParsing(char *filePath, int testMode);
int parse(char * filePath, int testMode);
int  interpret(char *filePath, int testMode);
void shutDownParsing();
int  getToken(ParseHandler pH);
void resizeTokenArray(ParseHandler pH);
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
int  processWhileLoop(ParseHandler pH, TokenType loopType, char loopVariable, double loopTargetVal, int loopStartIndex);
int  skipLoop(ParseHandler pH);
int  processColour(ParseHandler pH);

int checkValidOperator(char *c, ParseHandler pH);
int checkForColour(char *token, ParseHandler pH);

double getTokenVal(ParseHandler pH);

int sameString(char *a, char *b);
int syntaxError(ParseHandler pH, char *message);



// TESTING FUNCTIONS
void runParserWhiteBoxTests();
void testHandlerInitialisation();
void testSetAssignment();
void testDOloops();
