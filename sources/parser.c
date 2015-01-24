
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
  
  Variable varList[NUMBER_OF_VARIABLES];
  
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
        if(!pH->testing) {
            fprintf(stderr, "Syntax error - program should start with a single {\n");
            exit(1);
        }
    } else {
        pH->hangingBraces++;
    }
    
    
}

void getToken(ParseHelper pH)
{
    if(fscanf(pH->tokenFP, "%s", pH->token) != 1) {
        fprintf(stderr, "ERROR - invalid token");
        exit(1);
    }
    
}

int expect(TokenType tType)
{
    getToken(pH);
    switch(tType) {
        case instrctlist :
            processInstrctList(pH);
            break;
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
    if(strcmp(pH->token, "}") == 0) {
        


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



















