
#include "../includes/parser.h"

struct variable {
    
    char varName;
    double contents;
    int assigned;
} ;

struct parseHelper {

  FILE *tokenFP;
  FILE *lookAheadFP;
  
  int lineNumber;
  int hangingBraces;
  char token[TOKEN_LENGTH];
  
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
    pH->lookAheadFP = fopen(filePath, "r");
    
    pH->lineNumber = 1;
    pH->hangingBraces = 0;
    
    initialiseVariableList(pH);
    
    pH->testing = testing;
    if(testing) {
        pH->interpret = 0;
    } else {
        pH->interpret = 1;
    }

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
    fclose(pH->lookAheadFP);
    
    free(pH);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// PARSING FUNCTIONS
////////////////////

void setUpForParsing()
{
    createValStack();
}

int parse()
{
    
    setUpForParsing();
    ParseHelper pH = getParseHelperPointer(NULL);
    
    getToken(pH);
    if(strcmp(pH->token, "{") != 0) {
        printSyntaxError(pH, "program should start with a single {");
        return 0;
    } else {
        pH->hangingBraces++;
        if(!getToken(pH)) {
            return 0;
        }
        return processInstrctList(pH);
    }
    shutDownParsing();
}

void shutDownParsing()
{
    freeValStack();
}

int getToken(ParseHelper pH)
{
    if(fscanf(pH->tokenFP, "%s", pH->token) != 1) {
        if(fgetc(pH->tokenFP) == EOF) {
            printSyntaxError(pH, "closing braces do not match opening braces. Are you missing a closing brace?");
            return 0;
        }
        printSyntaxError(pH, "invalid token");
        return 0;
    }
    return 1;
}

int expect(TokenType tType)
{
    ParseHelper pH = getParseHelperPointer(NULL);
    
    
    switch(tType) {
        case instrctlist :
            return processInstrctList(pH);
        case instruction :
            return processInstruction(pH);
        case set :
            return processSet(pH);
        case varnum :
            if(checkForNumber(pH)) {
                return 1;
            } else {
                if(strlen(pH->token) != 1) {
                    printSyntaxError(pH, "all instructions should be followed by a number or single variable");
                    return 0;
                }
            }
            if(!checkVariableAssigned(pH->token[0], pH) ) {
                printSyntaxError(pH, "attempted to action command on unassigned variable");
                return 0;
            }
            return 1;
        case var :
            if(strlen(pH->token) != 1) {
                printSyntaxError(pH, "SET should be followed by a single variable");
                return 0;
            }
            
            if(!checkValidVariable(pH->token[0], pH) ) {
                printSyntaxError(pH, "SET should be followed by a single variable from A-Z");
                return 0;
            }
            return 1;
        case equals :
            if(strcmp(pH->token, ":=") != 0) {
                printSyntaxError(pH, "all SET commands should be followed by variable and ':='");
                return 0;
            }
            return 1;
        case val :
            return checkForNumber(pH);
        default :
            fprintf(stderr, "ERROR - incorrect enum passed to expect()\n");
            exit(1);
    }
}

int processInstrctList(ParseHelper pH)
{
    if(strcmp(pH->token, "{") == 0 ) {
        pH->hangingBraces++;
        getToken(pH);
        return processInstrctList(pH);
    }
    if(strcmp(pH->token, "FD") == 0 || strcmp(pH->token, "LT") == 0 || strcmp(pH->token, "RT") == 0 || strcmp(pH->token, "SET") == 0 || strcmp(pH->token, "DO") == 0) {
        return processInstruction(pH);
    }
    
    if(strcmp(pH->token, "}") == 0) {
        pH->hangingBraces--;
        if(pH->hangingBraces == 0) {
            
            char c;
            while((c = fgetc(pH->tokenFP) ) != EOF) {
                if(c != ' ' && c != '\n') {
                    printSyntaxError(pH, "additional input detected. Are you missing an opening brace?");
                    return 0;
                }
            }
            return 1;
        } else {
            getToken(pH);
            return processInstrctList(pH);
        }
    }
    
    printSyntaxError(pH, "Invalid instruction");
    return 0;
}

int processInstruction(ParseHelper pH)
{
    
    if(strcmp(pH->token, "FD") == 0) {
        if(!processVarNum(pH) ) {
            return 0;
        }
        return processInstrctList(pH);
    }
    
    if(strcmp(pH->token, "LT") == 0) {
        if(!processVarNum(pH) ) {
            return 0;
        }
        return processInstrctList(pH);
    }
    
    if(strcmp(pH->token, "RT") == 0) {
        if(!processVarNum(pH) ) {
            return 0;
        }
        return processInstrctList(pH);
    }
    
    if(strcmp(pH->token, "SET") == 0) {
        return processSet(pH);
    }
    
    if(strcmp(pH->token, "DO") == 0) {
        if(!getToken(pH)) {
            return 0;
        }
        return expect(set);
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
    if(expect(varnum) ) {
        if(!getToken(pH)) {
            return 0;
        }
        return 1;
    }
    return 0;
    
}
            
        

int processSet(ParseHelper pH)
{
    if(!getToken(pH)) {
        return 0;
    }
    if(!expect(var)) {
        return 0;
    }
    
    pH->varToSet = pH->token[0];
    
    if(!getToken(pH)) {
        return 0;
    }
    if(!expect(equals)) {
        return 0;
    }
    if(!processPolish(pH)) {
        return 0;
    }
    return processInstrctList(pH);
    
}

int processPolish(ParseHelper pH)
{
    getToken(pH);
    
    if(expect(val) ) {
        pushToValStack(pH->val);
        return processPolish(pH);
    }
    // if not a number, should be a variable or operator
    
    if(strlen(pH->token) != 1) {
        printSyntaxError(pH, "all reverse polish operators/variables should only be 1 character long separated by spaces");
        return 0;
    }
    
    if(checkValidVariable(pH->token[0], pH) ) {
        if(!checkVariableAssigned(pH->token[0], pH)) {
            printSyntaxError(pH, "attempted mathematical operation on undefined variable");
            return 0;
        } else {
            double d = pH->varList[pH->currentVarIndex]->contents;
            pushToValStack(d);
            return processPolish(pH);
        }
    }
    if(strcmp(pH->token, ";") == 0) {
        return finishPolish(pH);
    }
    
    if(checkValidOperator(pH->token[0], pH) ) {
        return processOperator(pH);
    }
    
    printSyntaxError(pH, "reverse polish expression not completed properly");
    return 0;
    
}


int processOperator(ParseHelper pH)
{
    double a, b;
    if(popFromValStack(&b) == 0 || popFromValStack(&a) == 0) {
        printSyntaxError(pH, "too few variables/constants for operators in reverse polish expression");
        return 0;
    }
    
    if(pH->interpret) {
        pH->val = doMaths(a, b, pH->currentOperation);
    }
    
    pushToValStack(pH->val);
    return processPolish(pH);
}

int finishPolish(ParseHelper pH)
{
    popFromValStack(&pH->val);
    assignValToCurrentVariable(pH);
    if(getNumberOfValsOnStack() != 0) {
        printf("Vals on stack: %d\n", getNumberOfValsOnStack());
        printSyntaxError(pH, "more input than required operators in reverse polish expression");
        return 0;
    } else {
        if(!getToken(pH)) {
            return 0;
        }
        return 1;
    }
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

int checkForNumber(ParseHelper pH)
{
    char *remainder;
    pH->val = strtod(pH->token, &remainder);
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


void printSyntaxError(ParseHelper pH, char *message)
{
    if(!pH->testing) {
        fprintf(stderr, "Syntax error - %s\nError at: %s\n", message, pH->token);
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
    
    sput_finish_testing();

}



void testHelperInitialisation()
{
    createParseHelper();
    initialiseParseHelper("testingFiles/parserTesting.txt", TESTING);
    ParseHelper pH = getParseHelperPointer(NULL);
    
    sput_fail_unless(pH->tokenFP != NULL && pH->lookAheadFP != NULL, "Parser Helper sets token and lookahead file pointers on initialisation");
    sput_fail_unless(pH->lineNumber == 1, "Parser Helper initialises line number to 1");
}

void testSyntaxErrors()
{
    createParseHelper();
    initialiseParseHelper("testingFiles/test_simpleParse.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed simple RT, LT and FD commands ok");
    freeParseHelper();
    
    createParseHelper();
    initialiseParseHelper("testingFiles/test_nestedBraces.txt", TESTING);
    sput_fail_unless(parse() == 1, "Parsed nested braces ok");
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
    sput_fail_unless(parse() == 0, "Will not parse text when there is remaining text after last brace");
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
    












