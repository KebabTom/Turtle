#include "../includes/parser.h"


struct parseHandler {

  FILE *tokenFP;
  
  int lineNumber;
  int hangingBraces;
  
  int numberOfTokens;
  int currentTokenIndex;
  char *token;
  char **tokenArray;
  
  double val;
  char varToSet;
  Clr colour;
  
  mathSymbol currentOperation;
  
  int showSyntaxErrors;
  int interpret;
} ;




//////////////////////////////////////////////////////////////////////////////////////////////////
// PARSE HANDLER FUNCTIONS
/////////////////////////


void createParseHandler()
{
    ParseHandler pH = (ParseHandler) malloc(sizeof(struct parseHandler));
    if(pH == NULL) {
        fprintf(stderr, "ERROR: Unable to malloc space for parseHandler structure in createParseHandler()");
        exit(1);
    }
    getParseHandlerPointer(pH);
}

ParseHandler getParseHandlerPointer(ParseHandler pH)
{
    static ParseHandler newHandler;
    
    if(pH != NULL) {
        newHandler = pH;
    }
    
    return newHandler;
}

void initialiseParseHandler(char *filePath, int testMode)
{
    ParseHandler pH = getParseHandlerPointer(NULL);
    
    pH->tokenFP = fopen(filePath, "r");
    
    pH->lineNumber = 1;
    pH->hangingBraces = 0;
    pH->currentTokenIndex = 0;
    pH->numberOfTokens = 0;
    
    if(testMode == NO_TESTING) {
        pH->showSyntaxErrors = 1;
    } else {
        pH->showSyntaxErrors = TEST_WITH_SYNTAX_ERRORS;
    }
    
    if(testMode == NO_TESTING) {
        pH->interpret = 1;
    } else {
        pH->interpret = 0;
    }
    
    pH->tokenArray = NULL;

}


void freeParseHandler()
{
    ParseHandler pH = getParseHandlerPointer(NULL);
    
    fclose(pH->tokenFP);
    
    for(int i = 0; i < pH->numberOfTokens; i++) {
        free(pH->tokenArray[i]);
    }
    free(pH->tokenArray);
    
    free(pH);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// PARSING FUNCTIONS
////////////////////

void setUpForParsing(char *filePath, int testMode)
{
    createParseHandler();
    initialiseParseHandler(filePath, testMode);
    createValStack();
    setUpForInterpreting(testMode);
    
}

int parse(char * filePath, int testMode)
{
    setUpForParsing(filePath, testMode);
    
    ParseHandler pH = getParseHandlerPointer(NULL);
    pH->interpret = 0;
    
    int parsed = processMain(pH);
    return parsed;
}

int interpret(char *filePath, int testMode)
{
    
    setUpForParsing(filePath, testMode);
    
    ParseHandler pH = getParseHandlerPointer(NULL);
    pH->interpret = 1;
    
    int interpreted = processMain(pH);
    return interpreted;
}

void shutDownParsing()
{
    freeValStack();
    freeParseHandler();
    shutDownInterpreting();
}

int getToken(ParseHandler pH)
{
    
    if(pH->currentTokenIndex < pH->numberOfTokens-1) {
        pH->currentTokenIndex++;
        pH->token = pH->tokenArray[pH->currentTokenIndex];
    //printf("%s\n", pH->token);
        return 1;
    }
    
    pH->numberOfTokens++;
    pH->currentTokenIndex = pH->numberOfTokens-1;
    
    
    resizeTokenArray(pH);
    
    if(fscanf(pH->tokenFP, "%s", pH->tokenArray[pH->numberOfTokens-1]) != 1) {
        if(fgetc(pH->tokenFP) == EOF) {
            return syntaxError(pH, "TOKEN closing braces do not match opening braces. Are you missing a closing brace?");
        }
        return syntaxError(pH, "invalid character in file");
    } else {
        pH->token = pH->tokenArray[pH->numberOfTokens-1];
    }
    //printf("%s\n", pH->token);
    return 1;
}

void resizeTokenArray(ParseHandler pH)
{
    pH->tokenArray = (char**)realloc(pH->tokenArray,pH->numberOfTokens * sizeof(char*));
    if(pH->tokenArray == NULL) {
        fprintf(stderr, "ERROR - realloc failed in getToken()\n");
        exit(1);
    }
    pH->tokenArray[pH->numberOfTokens-1] = (char*)calloc(TOKEN_LENGTH, sizeof(char));
    if(pH->tokenArray[pH->numberOfTokens-1] == NULL) {
        fprintf(stderr, "ERROR - malloc failed in getToken()\n");
        exit(1);
    }
}
        
// analyses the token string and returns an enum for the type of token.
TokenType whatToken(char *token)
{
    ParseHandler pH = getParseHandlerPointer(NULL);
    
    if(sameString(token, "FD"))    {return fd;}
    if(sameString(token, "LT"))    {return lt;}
    if(sameString(token, "RT"))    {return rt;}
    if(sameString(token, "SET"))   {return set;}
    if(sameString(token, "DO"))    {return doToken;}
    if(sameString(token, "BKSTP")) {return bkStep;}
    if(sameString(token, "PN"))    {return penChange;}
    if(sameString(token, "CLR"))   {return colour;}
    if(sameString(token, "FROM"))  {return from;}
    if(sameString(token, "TO"))    {return to;}
    if(sameString(token, "{"))     {return openBrace;}
    if(sameString(token, "}"))     {return closeBrace;}
    if(sameString(token, ";"))     {return semicolon;}
    if(sameString(token, ":="))    {return equals;}
    if(sameString(token, "WHILE")) {return whileToken;}
    if(sameString(token, "<"))     {return lessThan;}
    if(sameString(token, ">"))     {return moreThan;}
    if(sameString(token, "RAND"))  {return randomColour;}
    if(sameString(token, "ADV"))   {return advanceColour;}
    
    if(checkValidOperator(token, pH) == 1) {return op;}
    
    if(strlen(token) == 1) {
        if(checkValidVariable(token[0])) {
            if(checkVariableAssigned(token[0], pH->interpret, &pH->val)) {
                return assignedVar;
            } else {
                return unassignedVar;
            }
        }
    }
    if(checkForNumber(token, &pH->val)) {return num;}
    
    else {return noToken;}
}

int checkForVarNum(char * token)
{
    if(whatToken(token) == assignedVar || whatToken(token) == num) {
        return 1;
    }
    return 0;
}

int checkForAnyVar(char * token)
{
    if(whatToken(token) == assignedVar || whatToken(token) == unassignedVar) {
        return 1;
    }
    return 0;
}

int checkForInstruction(char *chkToken)
{
    TokenType t = whatToken(chkToken);
    
    switch(t) {
        case fd :
        case rt :
        case lt :
        case set :
        case doToken :
        case whileToken :
        case bkStep :
        case penChange :
        case colour :
            return 1;
        default :
            return 0;
    }
}
        
// "{" <INSTRCTLIST>
int processMain(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    
      // "{"
    if(whatToken(pH->token) != openBrace ) {
        return syntaxError(pH, "all code sections should begin with an opening brace");
    }
    pH->hangingBraces++;
    
      // <INSTRCTLIST>
    if(processInstructionList(pH) ) {
        if(pH->hangingBraces == 0) {
            return checkForEndOfCode(pH);
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}
     
int checkForEndOfCode(ParseHandler pH)
{
    char c;
    while((c = fgetc(pH->tokenFP) ) != EOF) {
        if(c != ' ' && c != '\n') {
            return syntaxError(pH, "additional input detected. Are you missing an opening brace?");
        }
    }
    return 1;
}

// <INSTRUCTION> <INSTRCTLIST> | "}"
int processInstructionList(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    
      // <INSTRUCTON>
    if(checkForInstruction(pH->token) ){
        if(!processInstruction(pH)) {
            return 0;
        }
          // <INSTRCTLIST>
        return processInstructionList(pH);
    }
    
      // "}"
    if(whatToken(pH->token) == closeBrace) {
        pH->hangingBraces--;
        return 1;
    }
        
    return syntaxError(pH, "Invalid instruction");
}

int processInstruction(ParseHandler pH)
{
    
    TokenType t = whatToken(pH->token);
    
    switch(t) {
        case fd :
        case lt :
        case rt :
        case bkStep :
            if(!processVarNum(pH)) {
                return 0;
            }
            if(pH->interpret) {
                doAction(t, pH->val);
            }
            return 1;
        case set :
            return processSet(pH);
        case doToken :
            return processDo(pH);
        case whileToken :
            return processWhile(pH);
        case colour :
            return processColour(pH);
        case penChange :
            if(pH->interpret) {
                switchPenStatus();
             }
            return 1;
        default :
            fprintf(stderr,"ERROR - invalid token (%s) passed to processInstruction()\n", pH->token);
            exit(1);
    }
}
            


int processVarNum(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    if(checkForVarNum(pH->token)) {
        return 1;
    }
    if(whatToken(pH->token) == unassignedVar) {
        return syntaxError(pH, "attempted to process an unassigned variable");
    }
    return 0;
    
}
            
        

int processSet(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    if(!checkForAnyVar(pH->token)) {
        return 0;
    }
    
    char varToSet = pH->token[0];
    
    if(!getToken(pH)) {return 0;}
    if(whatToken(pH->token) != equals) {
        return 0;
    }
    if(!processPolish(pH)) {
        return 0;
    }
    assignValToVariable(varToSet, pH->val, pH->interpret);
    return 1;
    
}

int processPolish(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    
    if(whatToken(pH->token) == num) {
        pushToValStack(pH->val);
        return processPolish(pH);;
    }
    
    // if not a number, token should be a variable or operator
    
    if(strlen(pH->token) != 1) {
        return syntaxError(pH, "all reverse polish operators/variables should only be 1 character long separated by spaces");
    }
    
    if(checkForVarNum(pH->token)) {
        pushToValStack(pH->val);
        return processPolish(pH);
    }
    
    if(whatToken(pH->token) == op) {
        if(!processOperator(pH) ) {
            return 0;
        }
        return processPolish(pH);
    }
    
    if(whatToken(pH->token) == semicolon) {
        return finishPolish(pH);
    }
    
    return syntaxError(pH, "reverse polish expression not completed properly");
}


int processOperator(ParseHandler pH)
{
    double a, b;
    if(popFromValStack(&b) == 0 || popFromValStack(&a) == 0) {
        return syntaxError(pH, "too few variables/constants for operators in reverse polish expression");
    }
    
    pH->val = doMaths(a, b, pH->currentOperation);
    
    pushToValStack(pH->val);
    return 1;
}

int finishPolish(ParseHandler pH)
{
    popFromValStack(&pH->val);
    if(getNumberOfValsOnStack() != 0) {
        return syntaxError(pH, "more input than required operators in reverse polish expression\n");
    } else {
        return 1;
    }
}

int processDo(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    if(!checkForAnyVar(pH->token)) {
        return syntaxError(pH, "invalid variable following DO command");
    }
    char loopVariable = pH->token[0];
    
    if(!getToken(pH)) {return 0;}
    if(whatToken(pH->token) != from) {
        return syntaxError(pH, "missing FROM in DO command");
    }
    
    if(!getToken(pH)) {return 0;}
    if(!checkForVarNum(pH->token)) {
        return syntaxError(pH, "invalid variable/number following FROM in DO command");
    }
    
    int loopVal = (int) getTokenVal(pH);
    assignValToVariable(loopVariable, (double)loopVal, pH->interpret);
    
    if(!getToken(pH)) {return 0;}
    if(whatToken(pH->token) != to) {
        return syntaxError(pH, "missing TO in DO command");
    }
    
    if(!getToken(pH)) {return 0;}
    if(!checkForVarNum(pH->token)) {
        return syntaxError(pH, "invalid variable/number following TO in DO command");
    }
    
    
    int loopEndingVal = (int) getTokenVal(pH);
    
    
    
    
    if(!getToken(pH)) {return 0;}
    if(whatToken(pH->token) != openBrace) {
        return syntaxError(pH, "missing opening brace in DO loop initialisation");
    }
    
    // record token index to restart loop
    int doLoopStartIndex = pH->currentTokenIndex;
    
    for(int i = loopVal; i <= loopEndingVal; i++) {
        assignValToVariable(loopVariable, (double)i, pH->interpret);
        pH->currentTokenIndex = doLoopStartIndex;
        pH->hangingBraces++;
        
        
        if(!processInstructionList(pH)) {
            return syntaxError(pH, "DO error");
        }
    }
    return 1;

}

int processWhile(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    if(whatToken(pH->token) != assignedVar) {
        return syntaxError(pH, "unassigned variable in WHILE command declaration");
    }
    char loopVariable = pH->token[0];
    
    if(!getToken(pH)) {return 0;}
    TokenType loopType = whatToken(pH->token);
    
    if(loopType != lessThan && loopType != moreThan) {
        return syntaxError(pH, "missing comparator in WHILE command");
    }
    
    if(!getToken(pH)) {return 0;}
    if(!checkForVarNum(pH->token)) {
        return syntaxError(pH, "invalid variable/number following operator in WHILE command");
    }
    
    double loopTargetVal = getTokenVal(pH);
    
    
    if(!getToken(pH)) {return 0;}
    if(whatToken(pH->token) != openBrace) {
        return syntaxError(pH, "missing opening brace in DO loop initialisation");
    }
    
    // record token index to restart loop
    int loopStartIndex = pH->currentTokenIndex;
    
    if(!pH->interpret) {
        return skipLoop(pH);
    }
    
    return processWhileLoop(pH, loopType, loopVariable, loopTargetVal, loopStartIndex);
}

// processes while loop until while condition is met. If consition is initially unset, parses loop once but doesn't set any values
int processWhileLoop(ParseHandler pH, TokenType loopType, char loopVariable, double loopTargetVal, int loopStartIndex)
{
    if(loopType == lessThan) {
        if(getVariableVal(loopVariable) > loopTargetVal) {
            return skipLoop(pH);
        }
        while(getVariableVal(loopVariable) < loopTargetVal) {
            pH->currentTokenIndex = loopStartIndex;
            pH->hangingBraces++;
            
            if(!processInstructionList(pH)) {
                return syntaxError(pH, "WHERE error");
            }
        }
    } else {
        if(getVariableVal(loopVariable) < loopTargetVal) {
            return skipLoop(pH);
        }
        while(getVariableVal(loopVariable) > loopTargetVal) {
            pH->currentTokenIndex = loopStartIndex;
            pH->hangingBraces++;
            
            if(!processInstructionList(pH)) {
                return syntaxError(pH, "WHERE error");
            }
        }
    }
    return 1;
}

// parses a loop but doesn't alter any values from SET commands within the loop. Used when not interpretting or when interpreter comes accross a loop it should never enter
int skipLoop(ParseHandler pH)
{
    int returnToInterpretting;
    if(pH->interpret) {
        returnToInterpretting = 1;
        pH->interpret = 0;
    } else {
        returnToInterpretting = 0;
    }
    
    pH->hangingBraces++;
    
    int successfulParse = processInstructionList(pH);
    
    pH->interpret = returnToInterpretting;
    
    return successfulParse;
}
    

int processColour(ParseHandler pH)
{
    if(!getToken(pH)) {return 0;}
    
    if(whatToken(pH->token) == advanceColour) {
        if(pH->interpret) {
            advanceTurtleColour();
        }
        return 1;
    }
    
    if(whatToken(pH->token) == randomColour) {
        if(pH->interpret) {
            setRandomTurtleColour();
        }
        return 1;
    }
    
    if(checkForColour(pH->token, pH) ) {
        if(pH->interpret) {
            applyTurtleColour(pH->colour);
        }
        return 1;
    }
    
    return syntaxError(pH, "invalid token following CLR");
}





int checkValidOperator(char *c, ParseHandler pH)
{
    if(strlen(c) > 1) {
        return 0;
    }
    switch(c[0]) {
        case '+' :
            pH->currentOperation = add;
            return 1;
        case '-' :
            pH->currentOperation = subtract;
            return 1;
        case '/' :
            pH->currentOperation = divide;
            return 1;
        case '*' :
            pH->currentOperation = multiply;
            return 1;
        default :
            return 0;
    }
}

int checkForColour(char *token, ParseHandler pH)
{
    if(sameString(token, "WHTE")) {
        pH->colour = white;
        return 1;
    }
    if(sameString(token, "RED")) {
        pH->colour = red;
        return 1;
    }
    if(sameString(token, "BLUE")) {
        pH->colour = blue;
        return 1;
    }
    if(sameString(token, "GREEN")) {
        pH->colour = green;
        return 1;
    }
    if(sameString(token, "YLLW")) {
        pH->colour = yellow;
        return 1;
    }
    if(sameString(token, "PRPL")) {
        pH->colour = purple;
        return 1;
    }
    return 0;
}



double getTokenVal(ParseHandler pH)
{
    if(checkForNumber(pH->token, &pH->val) ) {
        return pH->val;
    }
    return getVariableVal(pH->token[0]);
}

    
int sameString(char *a, char *b)
{
    if(strcmp(a,b) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int syntaxError(ParseHandler pH, char *message)
{
    if(pH->showSyntaxErrors) {
        fprintf(stderr, "Syntax error - %s\nError at: %s\n", message, pH->token);
    }
    return 0;
}


///////////////////////////////////////////////////////////////////
// VARIABLE HANDLING FUNCTIONS
//////////////////////////////






////////////////////////////////////////////////////////////////////////
// WHITE BOX TESTING FUNCTIONS
//////////////////////////////
void runParserWhiteBoxTests()
{
	  sput_start_testing();
	  
	  sput_set_output_stream(NULL);
	  
	  sput_enter_suite("testHandlerInitialisation(): Checking Parse Handler structure is properly initialised");
    sput_run_test(testHandlerInitialisation);
    sput_leave_suite();
    
    sput_enter_suite("testSetAssignment(): Checking test scripts using the SET command");
    sput_run_test(testSetAssignment);
    sput_leave_suite();
    
    sput_enter_suite("testDOloops(): Testing syntax parsing of DO loops");
    sput_run_test(testDOloops);
    sput_leave_suite();
    
    sput_finish_testing();

}



void testHandlerInitialisation()
{
    createParseHandler();
    initialiseParseHandler("testingFiles/parserTesting.txt", TEST_WHITEBOX);
    ParseHandler pH = getParseHandlerPointer(NULL);
    
    sput_fail_unless(pH->tokenFP != NULL, "Parser Handler sets token file pointer on initialisation");
    sput_fail_unless(pH->lineNumber == 1, "Parser Handler initialises line number to 1");
    freeParseHandler();
}

void testSetAssignment()
{
    sput_fail_unless(interpret("testingFiles/SET_Testing/test_selfSET.txt", TEST_WHITEBOX) == 1 && getVariableVal('C') == 10, "Interpreted variable setting itself with correct value (e.g. C += C ;)");
    shutDownParsing();
}

   
   
void testDOloops()
{
    sput_fail_unless(parse("testingFiles/DO_Testing/test_simpleDO.txt", TEST_WHITEBOX) == 1, "Parsed simple DO loop ok");
    shutDownParsing();

    sput_fail_unless(parse("testingFiles/DO_Testing/test_DOwithoutOpeningBrace.txt", TEST_WHITEBOX) == 0, "Will not parse DO without opening brace");
    shutDownParsing();
    
    sput_fail_unless(parse("testingFiles/DO_Testing/test_DOwithoutClosingBrace.txt", TEST_WHITEBOX) == 0, "Will not parse DO without closing brace");
    shutDownParsing();
    
    sput_fail_unless(parse("testingFiles/DO_Testing/test_noFROM_DO.txt", TEST_WHITEBOX) == 0, "Will not parse DO without FROM keyword");
    shutDownParsing();
    
    sput_fail_unless(parse("testingFiles/DO_Testing/test_noTO_DO.txt", TEST_WHITEBOX) == 0, "Will not parse DO without TO keyword");
    shutDownParsing();
    
    sput_fail_unless(parse("testingFiles/DO_Testing/test_nestedDO.txt", TEST_WHITEBOX) == 1, "Parsed nested DO loop ok");
    shutDownParsing();
    
    sput_fail_unless(parse("testingFiles/DO_Testing/test_polishDO.txt", TEST_WHITEBOX) == 1, "Parsed DO loop containing reverse polish ok");
    shutDownParsing();
    
    sput_fail_unless(parse("testingFiles/DO_Testing/test_complexDO.txt", TEST_WHITEBOX) == 1, "Parsed complex nested DO loop containing SET & reverse polish ok");
    shutDownParsing();
    
    sput_fail_unless(interpret("testingFiles/DO_Testing/test_simpleDOvalues.txt", TEST_WHITEBOX) == 1 && (int) getVariableVal('B') == 20, "Interpreted simple DO loop with correct value at end");
    shutDownParsing();
    
    sput_fail_unless(interpret("testingFiles/DO_Testing/test_nestedDOvalues.txt", TEST_WHITEBOX) == 1 && (int) getVariableVal('D') == 31, "Interpreted nested DO loop with correct value at end");
    shutDownParsing();
}

// independent main function - used in testing
// command line compile code: gcc -O4 -Wall -pedantic -std=c99 -lm -o parseTest parser.c interpreter.c
/*
int main(void)
{
    runParserWhiteBoxTests();
    
    //runInterpreterBlackBoxTests();
}
*/







