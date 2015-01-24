
#include "../includes/parser.h"

struct variable {
    
    char varName;
    int contents;
    int assigned;
} ;

struct parseHelper {

  FILE *tokenFP;
  FILE *lookAheadFP;
  
  int lineNumber;
  int hangingBraces;
  char token[TOKEN_LENGTH];
  
  double val;
  
  Variable varList[NUMBER_OF_VARIABLES];
  char operators[NUMBER_OF_OPERATORS];
  
  int testing;
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
    
    pH->operators[0] = '+';
    pH->operators[1] = '-';
    pH->operators[2] = '*';
    pH->operators[3] = '/';
    
    pH->testing = testing;

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

void parse()
{
    ParseHelper pH = getParseHelperPointer(NULL);
    
    getToken(pH);
    if(strcmp(pH->token, "{") != 0) {
        printSyntaxError(pH, "Syntax error - program should start with a single {");
    } else {
        pH->hangingBraces++;
        expect(instrctlist);
    }
      printf("Syntax ok\n");
    
    
}

void getToken(ParseHelper pH)
{
    if(fscanf(pH->tokenFP, "%s", pH->token) != 1) {
        if(fgetc(pH->tokenFP) == EOF) {
            printSyntaxError(pH, "Syntax error - closing brackets do not match opening brackets");
        }
        printSyntaxError(pH, "Syntax error - invalid token");
        exit(1);
    }
    
}

int expect(TokenType tType)
{
    ParseHelper pH = getParseHelperPointer(NULL);
    printf("here\n");
    getToken(pH);
    switch(tType) {
        case instrctlist :
            processInstrctList(pH);
            break;
        case instruction :
            processInstruction(pH);
            break;
        case set :
            processSet(pH);
            break;
        case polish :
            processPolish(pH);
            break;
        default :
            fprintf(stderr, "incorrect enum passed to expect()\n");
            exit(1);
    }
    return 1;
}

int processInstrctList(ParseHelper pH)
{
    if(strcmp(pH->token, "{") == 0 ) {
        pH->hangingBraces++;
        expect(instrctlist);
        return 1;
    }
    if(strcmp(pH->token, "FD") == 0 || strcmp(pH->token, "LT") == 0 || strcmp(pH->token, "RT") == 0) {
        expect(instruction);
        return 1;
    }
    if(strcmp(pH->token, "SET") == 0) {
        expect(set);
        return 1;
    }
    if(strcmp(pH->token, "}") == 0) {
        pH->hangingBraces--;
        if(pH->hangingBraces == 0) {
            char c;
            while((c = fgetc(pH->tokenFP) ) != EOF) {
                if(c != ' ' && c != '\n') {
                    printSyntaxError(pH, "Syntax error - additional input detected. Are you missing a closing brace?");
                    return 0;
                }
            }
        } else {
            expect(instrctlist);
        }
        return 1;
    }
    
    printSyntaxError(pH, "Syntax error - Invalid instruction");
    return 0;
}

int processInstruction(ParseHelper pH)
{
    char *remainder;
    pH->val = strtod(pH->token, &remainder);
    if(remainder[0] != '\0') {
        printSyntaxError(pH, "Syntax error - expecting number following instruction");
        return 0;
    } else {
        expect(instrctlist);
        return 1;
    }
}

int processSet(ParseHelper pH)
{
    if(strlen(pH->token) != 1) {
        printSyntaxError(pH, "Syntax error - SET should be followed by a single variable");
        return 0;
    }
    
    if(!checkValidVariable(pH->token[0], pH) ) {
        printSyntaxError(pH, "Syntax error - SET should be followed by a single variable from A-Z");
        return 0;
    }
    
    getToken(pH);
    if(strcmp(pH->token, ":=") != 0) {
        printSyntaxError(pH, "Syntax error - all SET commands should be followed by variable and ':='");
        return 0;
    }
    expext(polish);
    
    return 1;
}

int processPolish(ParseHelper pH)
{


    return 1;
}

/*
returns 1 if passed character is within list of potential variables. If not, returns 0.
*/
int checkValidVariable(char c, ParseHelper pH)
{
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(pH->varList[i]->varName == c) {
            return 1;
        }
    }
    
    return 0;
}

void printSyntaxError(ParseHelper pH, char *message)
{
    if(!pH->testing) {
        fprintf(stderr, "%s\nError at: %s\n", message, pH->token);
        exit(1);
    }
}


void runParserWhiteBoxTests()
{
	  sput_start_testing();
	  
	  sput_set_output_stream(NULL);
	  
	  sput_enter_suite("testHelperInitialisation(): Checking Parse Helper structure is properly initialised");
    sput_run_test(testHelperInitialisation);
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



















