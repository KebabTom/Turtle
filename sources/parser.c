
#include "../includes/parser.h"

struct variable {
    
    char varName;
    double contents;
    int assigned;
} ;

struct parseHelper {

  FILE *tokenFP;
  
  int lineNumber;
  int hangingBraces;
  
  int numberOfTokens;
  int currentTokenIndex;
  char *token;
  char **tokenArray;
  
  double val;
  char varToSet;
  int currentVarIndex;
  mathSymbol currentOperation;
  
  Variable varList[NUMBER_OF_VARIABLES];
  
  int testing;
  int interpret;
} ;

struct valStack {

  int numOfVals;
  ValNode top;
} ;

struct valNode {

  double val;
  ValNode next;
} ;



//////////////////////////////////////////////////////////////////////////////////////////////////
// PARSE HELPER FUNCTIONS
/////////////////////////


void createParseHelper()
{
    ParseHelper pH = (ParseHelper) malloc(sizeof(struct parseHelper));
    if(pH == NULL) {
        fprintf(stderr, "ERROR: Unable to malloc space for parseHelper structure in createParseHelper()");
        exit(1);
    }
    getParseHelperPointer(pH);
}

ParseHelper getParseHelperPointer(ParseHelper pH)
{
    static ParseHelper newHelper;
    
    if(pH != NULL) {
        newHelper = pH;
    }
    
    return newHelper;
}

void initialiseParseHelper(char *filePath, int testing)
{
    ParseHelper pH = getParseHelperPointer(NULL);
    
    pH->tokenFP = fopen(filePath, "r");
    
    pH->lineNumber = 1;
    pH->hangingBraces = 0;
    pH->currentTokenIndex = 0;
    pH->numberOfTokens = 0;
    
    initialiseVariableList(pH);
    
    pH->testing = testing;
    if(testing) {
        pH->interpret = 0;
    } else {
        pH->interpret = 1;
    }
    
    pH->tokenArray = NULL;

}

void initialiseVariableList(ParseHelper pH)
{
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        pH->varList[i] = (Variable) malloc(sizeof(struct variable));
        if(pH->varList[i] == NULL) {
            fprintf(stderr, "ERROR - unable to malloc space for variable structure in initialiseVariableList()");
            exit(1);
        }
        pH->varList[i]->varName = 'A'+i;
        pH->varList[i]->assigned = 0;
    }
}
    

void freeParseHelper()
{
    ParseHelper pH = getParseHelperPointer(NULL);
    
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        free(pH->varList[i]);
    }
    
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

void setUpForParsing()
{
    ParseHelper pH = getParseHelperPointer(NULL);
    createValStack();
    if(pH->interpret) {
        createMoveHistoryStack();
    }
}

    
    
    

int parse()
{
    
    setUpForParsing();
    ParseHelper pH = getParseHelperPointer(NULL);
    
    int parsed = processMain(pH);
    shutDownParsing();
    
    return parsed;
}

void shutDownParsing()
{
    freeValStack();
}

int getToken(ParseHelper pH)
{
    
    if(pH->currentTokenIndex < pH->numberOfTokens-1) {
        pH->currentTokenIndex++;
        pH->token = pH->tokenArray[pH->currentTokenIndex];
    //printf("%s\n", pH->token);
        return 1;
    }
    
    pH->numberOfTokens++;
    pH->currentTokenIndex = pH->numberOfTokens-1;
    
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
        
    if(fscanf(pH->tokenFP, "%s", pH->tokenArray[pH->numberOfTokens-1]) != 1) {
        if(fgetc(pH->tokenFP) == EOF) {
            return syntaxError(pH, "TOKEN closing braces do not match opening braces. Are you missing a closing brace?");
        }
        return syntaxError(pH, "invalid token");
    } else {
        pH->token = pH->tokenArray[pH->numberOfTokens-1];
    }
    //printf("%s\n", pH->token);
    return 1;
}

TokenType whatToken(char *token)
{
    ParseHelper pH = getParseHelperPointer(NULL);
    
    if(sameString(token, "FD")) {return instruction;}
    if(sameString(token, "LT")) {return instruction;}
    if(sameString(token, "RT")) {return instruction;}
    if(sameString(token, "SET")) {return instruction;}
    if(sameString(token, "DO")) {return instruction;}
    if(sameString(token, "FROM")) {return from;}
    if(sameString(token, "TO")) {return to;}
    if(sameString(token, "{")) {return openBrace;}
    if(sameString(token, "}")) {return closeBrace;}
    if(sameString(token, ";")) {return semicolon;}
    if(sameString(token, ":=")) {return equals;}
    
    if(checkValidOperator(token[0], pH) == 1) {return op;}
    
    if(strlen(token) == 1) {
        if(checkValidVariable(token[0], pH)) {
            if(checkVariableAssigned(token[0], pH)) {
                return assignedVar;
            } else {
                return unassignedVar;
            }
        }
    }
    if(checkForNumber(token, pH)) {return num;}
    
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

int processMain(ParseHelper pH)
{
    if(!getToken(pH)) {
        return 0;
    }
    
    if(whatToken(pH->token) != openBrace ) {
        return syntaxError(pH, "all code sections should begin with an opening brace");
    }
    pH->hangingBraces++;
    
    if(processInstrctList(pH) ) {
        if(pH->hangingBraces == 0) {
            
            char c;
            while((c = fgetc(pH->tokenFP) ) != EOF) {
                if(c != ' ' && c != '\n') {
                    return syntaxError(pH, "additional input detected. Are you missing an opening brace?");
                }
            }
            return 1;
        } else {
            return syntaxError(pH, "closing braces do not match opening braces. Are you missing a closing brace?");
        }
    } else {
        return 0;
    }
}
     

int processInstrctList(ParseHelper pH)
{
    if(!getToken(pH)) {
        return 0;
    }
    
    if(whatToken(pH->token) == openBrace) {
        pH->hangingBraces++;
        return(processInstrctList(pH));
    }
    
    if(whatToken(pH->token) == instruction){
        if(!processInstruction(pH)) {
            return 0;
        }
        return processInstrctList(pH);
    } else {
    
        if(whatToken(pH->token) == closeBrace) {
            pH->hangingBraces--;
            return 1;
        }
    }
    return syntaxError(pH, "Invalid instruction");
}

int processInstruction(ParseHelper pH)
{
    
    if(strcmp(pH->token, "FD") == 0) {
        if(!processVarNum(pH) ) {
            return 0;
        }
        if(pH->interpret) {
            doMove(fd, pH->val);
        }
        return 1;
    }
    
    if(strcmp(pH->token, "LT") == 0) {
        if(!processVarNum(pH) ) {
            return 0;
        }
        if(pH->interpret) {
            doMove(lt, pH->val);
        }
        return 1;
    }
    
    if(strcmp(pH->token, "RT") == 0) {
        if(!processVarNum(pH) ) {
            return 0;
        }
        if(pH->interpret) {
            doMove(rt, pH->val);
        }
        return 1;
    }
    
    if(strcmp(pH->token, "SET") == 0) {
        return processSet(pH);
    }
    
    if(strcmp(pH->token, "DO") == 0) {
        return processDo(pH);
    }
    else {
        fprintf(stderr,"ERROR - invalid token (%s) passed to processInstruction()\n", pH->token);
        exit(1);
    }
}

int processVarNum(ParseHelper pH)
{
    if(!getToken(pH)) {
        return 0;
    }
    if(whatToken(pH->token) == assignedVar || whatToken(pH->token) == num) {
        return 1;
    }
    if(whatToken(pH->token) == unassignedVar) {
        return syntaxError(pH, "attempted to process an unassigned variable");
    }
    return 0;
    
}
            
        

int processSet(ParseHelper pH)
{
    if(!getToken(pH)) {
        return 0;
    }
    if(!checkForAnyVar(pH->token)) {
        return 0;
    }
    
    char varToSet = pH->token[0];
    
    if(!getToken(pH)) {
        return 0;
    }
    if(whatToken(pH->token) != equals) {
        return 0;
    }
    if(!processPolish(pH)) {
        return 0;
    }
    assignValToVariable(pH, varToSet, pH->val);
    return 1;
    
}

int processPolish(ParseHelper pH)
{
    if(!getToken(pH)) {
        return 0;
    }
    
    if(whatToken(pH->token) == num) {
        pushToValStack(pH->val);
        return processPolish(pH);;
    }
    
    // if not a number, should be a variable or operator
    
    if(strlen(pH->token) != 1) {
        return syntaxError(pH, "all reverse polish operators/variables should only be 1 character long separated by spaces");
    }
    
    if(checkForVarNum(pH->token)) {
        double d = getTokenVal(pH);
        pushToValStack(d);
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


int processOperator(ParseHelper pH)
{
    double a, b;
    if(popFromValStack(&b) == 0 || popFromValStack(&a) == 0) {
        return syntaxError(pH, "too few variables/constants for operators in reverse polish expression");
    }
    
    pH->val = doMaths(a, b, pH->currentOperation);
    
    pushToValStack(pH->val);
    return 1;
}

int finishPolish(ParseHelper pH)
{
    popFromValStack(&pH->val);
    if(getNumberOfValsOnStack() != 0) {
        return syntaxError(pH, "more input than required operators in reverse polish expression\n");
    } else {
        return 1;
    }
}

int processDo(ParseHelper pH)
{
    if(!getToken(pH)) {
        return 0;
    }
    if(!checkForAnyVar(pH->token)) {
        return syntaxError(pH, "invalid variable following DO command");
    }
    char loopVariable = pH->token[0];
    
    if(!getToken(pH)) {
        return 0;
    }
    if(whatToken(pH->token) != from) {
        return syntaxError(pH, "missing FROM in DO command");
    }
    
    if(!getToken(pH)) {
        return 0;
    }
    if(!checkForVarNum(pH->token)) {
        return syntaxError(pH, "invalid variable/number following FROM in DO command");
    }
    
    int loopVal = (int) getTokenVal(pH);
    assignValToVariable(pH, loopVariable, (double)loopVal);
    
    if(!getToken(pH)) {
        return 0;
    }
    if(whatToken(pH->token) != to) {
        return syntaxError(pH, "missing TO in DO command");
    }
    
    if(!getToken(pH)) {
        return 0;
    }
    if(!checkForVarNum(pH->token)) {
        return syntaxError(pH, "invalid variable/number following TO in DO command");
    }
    
    int loopEndingVal = (int) getTokenVal(pH);
    int doLoopStartIndex = pH->currentTokenIndex;
    
    if(!getToken(pH)) {
        return 0;
    }
    if(whatToken(pH->token) != openBrace) {
        return syntaxError(pH, "missing opening brace following DO command");
    }
        
    for(int i = loopVal; i <= loopEndingVal; i++) {
        assignValToVariable(pH, loopVariable, (double)i);
        pH->currentTokenIndex = doLoopStartIndex;
        
        
        if(!processInstrctList(pH)) {
            return syntaxError(pH, "DO error");
        }
    }
    return 1;

}

/*
returns 1 if passed character is within list of potential variables. If not, returns 0.
*/
int checkValidVariable(char c, ParseHelper pH)
{
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(pH->varList[i]->varName == c) {
            pH->currentVarIndex = i;
            return 1;
        }
    }
    
    return 0;
}

int checkForNumber( char *token, ParseHelper pH)
{
    char *remainder;
    pH->val = strtod(token, &remainder);
    if(remainder[0] != '\0') {
        return 0;
    } else {
        return 1;
    }
}

int checkVariableAssigned(char c, ParseHelper pH)
{
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(pH->varList[i]->varName == c) {
            if(pH->varList[i]->assigned == 1) {
                pH->val = pH->varList[i]->contents;
                return 1;
            }
        }
    }
    return 0;
}

int checkValidOperator(char c, ParseHelper pH)
{
    switch(c) {
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


void assignValToCurrentVariable(ParseHelper pH)
{
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(pH->varList[i]->varName == pH->varToSet) {
            pH->varList[i]->contents = pH->val;
            pH->varList[i]->assigned = 1;
        }
    }
}


void assignValToVariable(ParseHelper pH, char varToSet, double val)
{
    int foundVar = 0;
    for(int i = 0; i < NUMBER_OF_VARIABLES && foundVar == 0; i++) {
        if(pH->varList[i]->varName == varToSet) {
            foundVar = 1;
            pH->varList[i]->contents = val;
            pH->varList[i]->assigned = 1;
        }
    }
    if(foundVar != 1) {
        fprintf(stderr, "ERROR - unable to locate variable in assignValToVariable()\n");
        exit(1);
    }
}

double getTokenVal(ParseHelper pH)
{
    if(checkForNumber(pH->token, pH) ) {
        return pH->val;
    }
    return getVariableVal(pH, pH->token[0]);
}

double getVariableVal(ParseHelper pH, char c)
{

    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(pH->varList[i]->varName == c) {
            return pH->varList[i]->contents;
        }
    }
    
    fprintf(stderr, "ERROR - attempted to extract contents of variable not in varList in getTokenVal()\n");
    exit(1);
}
    
int sameString(char *a, char *b)
{
    if(strcmp(a,b) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int syntaxError(ParseHelper pH, char *message)
{
    if(!pH->testing) {
        fprintf(stderr, "Syntax error - %s\nError at: %s\n", message, pH->token);
    }
    return 0;
}


///////////////////////////////////////////////////////////////////
// VARIABLE HANDLING FUNCTIONS
//////////////////////////////

double doMaths(double a, double b, mathSymbol op)
{
    switch(op) {
        case add :
            return a+b;
        case subtract :
            return a-b;
        case multiply :
            return a*b;
        case divide :
            return a/b;
        default :
            fprintf(stderr, "ERROR - invalid maths operator passed to doMaths()\n");
            exit(1);
    }
}

///////////////////////////////////////////////////////////////////
// VAL STACK FUNCTIONS
/////////////////////////

void createValStack()
{
    ValStack newStack = (ValStack) malloc(sizeof(struct valStack));
    if(newStack == NULL) {
        fprintf(stderr, "ERROR - unable to malloc space for valstack in createValStack()\n");
        exit(1);
    }
    
    newStack->top = NULL;
    newStack->numOfVals = 0;
    getValStackPointer(newStack);
}

ValStack getValStackPointer(ValStack newStack)
{
    static ValStack vStack;
    if(newStack != NULL) {
        vStack = newStack;
    }
    
    return vStack;
}

void pushToValStack(double val)
{
    ValStack vStack = getValStackPointer(NULL);
    
    ValNode vN = newValNode();
    vN->val = val;
    vN->next = vStack->top;
    vStack->numOfVals++;
    vStack->top = vN;
}


ValNode newValNode()
{
    ValNode newNode = (ValNode) malloc(sizeof(struct valNode));
    if(newNode == NULL) {
        fprintf(stderr, "ERROR - Unable to malloc space for val node in newValNode()\n");
        exit(1);
    }
    
    newNode->next = NULL;
    return newNode;
}

int popFromValStack(double *poppedVal)
{
    ValStack vStack = getValStackPointer(NULL);
    
    if(vStack->numOfVals > 0) {
        *poppedVal = vStack->top->val;
        ValNode tmp = vStack->top;
        vStack->top = vStack->top->next;
        free(tmp);
        vStack->numOfVals--;
        return 1;
    } else {
        return 0;
    }
}

int getNumberOfValsOnStack()
{
    return getValStackPointer(NULL)->numOfVals;
}
    
void freeValStack()
{
    ValStack vStack = getValStackPointer(NULL);
    
    for(int i = 0; i < vStack->numOfVals; i++) {
        ValNode tmp = vStack->top;
        vStack->top = vStack->top->next;
        free(tmp);
    }
    
    free(vStack);
}







////////////////////////////////////////////////////////////////////////
// WHITE BOX TESTING FUNCTIONS
//////////////////////////////
void runParserWhiteBoxTests()
{
	  sput_start_testing();
	  
	  sput_set_output_stream(NULL);
	  
	  sput_enter_suite("testHelperInitialisation(): Checking Parse Helper structure is properly initialised");
    sput_run_test(testHelperInitialisation);
    sput_leave_suite();
    
    sput_enter_suite("testSyntaxErrors(): Checking test scripts with missing brackets etc");
    sput_run_test(testSyntaxErrors);
    sput_leave_suite();
    
    sput_enter_suite("testVarNum(): Checking test scripts with variables and numbers");
    sput_run_test(testVarNum);
    sput_leave_suite();
    
    sput_enter_suite("testSetCommand(): Checking test scripts using the SET command");
    sput_run_test(testSetCommand);
    sput_leave_suite();
    
    sput_enter_suite("testValStack(): Testing pushing and popping from Val Stack");
    sput_run_test(testValStack);
    sput_leave_suite();
    
    sput_enter_suite("testDOloops(): Testing syntax parsing of DO loops");
    sput_run_test(testDOloops);
    sput_leave_suite();
    
    sput_finish_testing();

}



void testHelperInitialisation()
{
    createParseHelper();
    initialiseParseHelper("testingFiles/parserTesting.txt", TESTING);
    ParseHelper pH = getParseHelperPointer(NULL);
    
    sput_fail_unless(pH->tokenFP != NULL, "Parser Helper sets token file pointer on initialisation");
    sput_fail_unless(pH->lineNumber == 1, "Parser Helper initialises line number to 1");
    freeParseHelper();
}

void testSyntaxErrors()
{
    createParseHelper();
    initialiseParseHelper("testingFiles/test_simpleParse.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed simple RT, LT and FD commands ok");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/test_noClosingBrace.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse text with no closing brace");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/test_noOpeningBrace.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse text with no opening brace");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/test_textAfterClosingBrace.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse text when t is remaining text after last brace");
    freeParseHelper();
}

void testVarNum()
{
    createParseHelper();
    initialiseParseHelper("testingFiles/VarNum_Testing/test_simpleVarNum.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed simple access of set variable OK");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/VarNum_Testing/test_uninitialisedVarNum.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse text when variable is used uninitialised");
    freeParseHelper();
}

void testSetCommand()
{
    createParseHelper();
    initialiseParseHelper("testingFiles/SET_Testing/test_simpleSET.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed simple SET commands ok");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/SET_Testing/test_SETmultiple.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed multiple SET commands to same variable ok");
    freeParseHelper();
    
    createParseHelper();
    ParseHelper pH = getParseHelperPointer(NULL);
    initialiseParseHelper("testingFiles/SET_Testing/test_selfSET.txt", TESTING);
    sput_fail_unless(parse() == 1 && getVariableVal(pH, 'C') == 10, "Parsed variable setting itself with correct value (e.g. C += C ;)");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/SET_Testing/test_SETpolish.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed SET commands with reverse polish maths ok");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/SET_Testing/test_SETpolishUndefined.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse command performing reverse polish on undefined variable");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/SET_Testing/test_SETlongPolish.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed SET commands using long Polish expression ok");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/SET_Testing/test_SETpolishTooManyVariables.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse SET command with unbalanced (too many variables) reverse polish equation");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/SET_Testing/test_SETpolishTooManyOperators.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse SET command with unbalanced (too many operators) reverse polish equation");
    freeParseHelper();
}

void testValStack()
{
    createValStack();
    pushToValStack(3);
    pushToValStack(12.4);
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 2, "After pushing two values to stack, number of items in stack is 2");
    freeValStack();
    
    createValStack();
    pushToValStack(25);
    pushToValStack(13);
    pushToValStack(20);
    double d;
    sput_fail_unless(popFromValStack(&d) == 1 && (int) d == 20, "Stack successfully pops value from stack");
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 2, "After pushing 3 values and popping 1, number of items in stack is 2");
    
    popFromValStack(&d);
    popFromValStack(&d);
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 0, "After pushing 3 values and popping 3, number of items in stack is 0");
    sput_fail_unless((int) d == 25, "Successfully pops 3 values from stack");
    
    sput_fail_unless(popFromValStack(&d) == 0, "Does not alow pop from empty stack");
    sput_fail_unless((int) d == 25, "Target value unchanged after pop from empty stack");
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 0, "Items in stack is still 0 after pop from empty stack");
    freeValStack();
}
   
   
void testDOloops()
{
    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_simpleDO.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed simple DO loop ok");
    freeParseHelper();

    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_DOwithoutOpeningBrace.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse DO without opening brace");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_DOwithoutClosingBrace.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse DO without closing brace");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_noFROM_DO.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse DO without FROM keyword");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_noTO_DO.txt", TESTING);
    sput_fail_unless(parse() == 0, "Will not parse DO without TO keyword");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_nestedDO.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed nested DO loop ok");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_polishDO.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed DO loop containing reverse polish ok");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/DO_Testing/test_complexDO.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed complex nested DO loop containing SET & reverse polish ok");
    freeParseHelper();
    
    createParseHelper();
    ParseHelper pH = getParseHelperPointer(NULL);
    initialiseParseHelper("testingFiles/DO_Testing/test_simpleDOvalues.txt", TESTING);
    sput_fail_unless(parse() == 1 && (int) getVariableVal(pH, 'B') == 20, "Parsed simple DO loop with correct value at end");
    freeParseHelper();
    
    createParseHelper();
    pH = getParseHelperPointer(NULL);
    initialiseParseHelper("testingFiles/DO_Testing/test_nestedDOvalues.txt", TESTING);
    sput_fail_unless(parse() == 1 && (int) getVariableVal(pH, 'D') == 31, "Parsed nested DO loop with correct value at end");
    freeParseHelper();
}











