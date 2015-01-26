
#include "../includes/turtle.h"

#define TEST_STR_LEN 100

int main(int argc, char *argv[])

{

    int inputOK = checkInput(argc, argv, NO_TESTING);
    if(!inputOK) {
        exit(1);
    }
    
    int startMode = processInput(argc, argv);
    switch (startMode) {
        case NO_TESTING :
        
            setUpForParsing(argv[1], NO_TESTING);
            
            int parsedOK = parse();
            
            shutDownParsing(NO_TESTING);
            
            if(parsedOK) {
                printf("Parsed OK\n");
            }
            
            break;
        case TEST_WHITEBOX :
            runWhiteBoxTesting();
            break;
        case TEST_BLACKBOX :
            runBlackBoxTesting();
            break;
        case TEST_ALL :
            runWhiteBoxTesting();
            runBlackBoxTesting();
            break;
        default :
            fprintf(stderr, "ERROR - invalid output from processInput()\n");
    }
    
    return 0;
    
    
}


int checkInput(int argc, char *argv[], int testing)
{
    // check for correct number of arguments
    if(argc == 1 || argc > 3) {
        printCommandLineError(testing);
        return 0;
    }
    
    // if 2 arguments, check for file located at filepath of argv[1]
    if(argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if(fp == NULL) {
            if(!testing) {
                fprintf(stderr, "ERROR: Unable to locate file at '%s'\n", argv[1]);
                printCommandLineError(testing);
                return 0;
            }
        }
        fclose(fp);
    }
    
    // if 3 arguments, check for valid testing input
    if(argc == 3) {
        if(strcmp(argv[1],"test") != 0) {
            printCommandLineError(testing);
            return 0;
        }
        if(strcmp(argv[2],"all") != 0 && strcmp(argv[2],"white") != 0 && strcmp(argv[2],"black") != 0) {
            printCommandLineError(testing);
            return 0;
        }
    }
    return 1;
}


void printCommandLineError(int testing)
{
    if(!testing) {
        fprintf(stderr,"please run the turtle program with one of the command line arguments as follows:\n\nTo parse a .txt file and draw a shape:\n./turtle <FILENAME>.txt\n\nFor testing enter one of the below:\n./turtle test all\n./turtle test white\n./turtle test black\n");
        exit(1);
    }
}

int processInput(int argc, char *argv[])
{
    // if two arguments, set up the parse heper using filepath from argv[1]
    if(argc == 2) {
        return NO_TESTING;
    }
    
    
    // if three arguments, choose testing type based on argv[2]
    if(argc == 3) {
        if(strcmp(argv[2],"white") == 0) {
            return TEST_WHITEBOX;
        }
        if(strcmp(argv[2],"black") == 0) {
            return TEST_BLACKBOX;
        }
        if(strcmp(argv[2],"all") == 0) {
            return TEST_ALL;
        }
    }
    
    fprintf(stderr, "ERROR - incorrect input allowed into processInput()\n");
    exit(1);
    
}
    



void runWhiteBoxTesting()
{
	  
    runCommandLineTests();
    runParserWhiteBoxTests();
    runInterpreterWhiteBoxTests();
    
}

void runBlackBoxTesting()
{
    runParserBlackBoxTests();
}

void runCommandLineTests()
{  
	  sput_start_testing();
	  sput_set_output_stream(NULL);	
    
    sput_enter_suite("testCommandLine(): Checking command line input is processed properly");
    sput_run_test(testCommandLine);
    sput_leave_suite();
    
	  sput_finish_testing();
    
}

void testCommandLine()
{
    int argc = 1;
    char *argv[3];
    for(int i = 0; i < 4; i++) {
        argv[i] = calloc(TEST_STR_LEN, sizeof(char));
        if(argv[i] == NULL) {
            fprintf(stderr, "ERROR: Unable to calloc space for strings in testCommandLine()\n");
        }
    }
    
    sput_fail_unless(checkInput(argc, argv, TEST_WHITEBOX) == 0, "Input check detects error when only one argument passed");
    
    argc = 4;
    sput_fail_unless(checkInput(argc, argv, TEST_WHITEBOX) == 0, "Input check detects error when more than 3 arguments passed");
    
    argc = 3;
    strcpy(argv[1], "testwrong");
    sput_fail_unless(checkInput(argc, argv, TEST_WHITEBOX) == 0, "Input check detects error when 2nd of 3 arguments is not 'test' passed");
    
    strcpy(argv[1], "test");
    strcpy(argv[2], "all");
    sput_fail_unless(checkInput(argc, argv, TEST_WHITEBOX) == 1, "Input check OK when 3 arguments and third is 'all'");
    strcpy(argv[2], "white");
    sput_fail_unless(checkInput(argc, argv, TEST_WHITEBOX) == 1, "Input check OK when 3 arguments and third is 'white'");
    strcpy(argv[2], "black");
    sput_fail_unless(checkInput(argc, argv, TEST_WHITEBOX) == 1, "Input check OK when 3 arguments and third is 'black'");
    strcpy(argv[2], "testwrong");
    sput_fail_unless(checkInput(argc, argv, TEST_WHITEBOX) == 0, "Input check OK when 3 arguments and third is neither 'all', 'black' or 'white'");
    
    for(int i = 0; i < 4; i++) {
        free(argv[i]);
    }

}


void runParserBlackBoxTests()
{
	  sput_start_testing();
	  
    sput_enter_suite("testSyntaxErrors(): Checking test scripts with missing brackets etc");
    sput_run_test(testSyntaxErrors);
    sput_leave_suite();
    
    sput_enter_suite("testVarNum(): Checking test scripts with variables and numbers");
    sput_run_test(testVarNum);
    sput_leave_suite();
    
    sput_enter_suite("testSetCommand(): Checking test scripts using the SET command");
    sput_run_test(testSetCommand);
    sput_leave_suite();
    
	  sput_finish_testing();
}


void testSyntaxErrors()
{
    setUpForParsing("testingFiles/test_simpleParse.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 1, "Parsed simple RT, LT and FD commands ok");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/test_noClosingBrace.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 0, "Will not parse text with no closing brace");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/test_noOpeningBrace.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 0, "Will not parse text with no opening brace");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/test_textAfterClosingBrace.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 0, "Will not parse text when t is remaining text after last brace");
    shutDownParsing(TEST_WHITEBOX);
}

void testVarNum()
{
    setUpForParsing("testingFiles/VarNum_Testing/test_simpleVarNum.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 1, "Parsed simple access of set variable OK");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/VarNum_Testing/test_uninitialisedVarNum.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 0, "Will not parse text when variable is used uninitialised");
    shutDownParsing(TEST_WHITEBOX);
}

void testSetCommand()
{
    setUpForParsing("testingFiles/SET_Testing/test_simpleSET.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 1, "Parsed simple SET commands ok");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/SET_Testing/test_SETmultiple.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 1, "Parsed multiple SET commands to same variable ok");
    shutDownParsing(TEST_WHITEBOX);

    setUpForParsing("testingFiles/SET_Testing/test_SETpolish.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 1, "Parsed SET commands with reverse polish maths ok");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/SET_Testing/test_SETpolishUndefined.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 0, "Will not parse command performing reverse polish on undefined variable");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/SET_Testing/test_SETlongPolish.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 1, "Parsed SET commands using long Polish expression ok");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/SET_Testing/test_SETpolishTooManyVariables.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 0, "Will not parse SET command with unbalanced (too many variables) reverse polish equation");
    shutDownParsing(TEST_WHITEBOX);
    
    setUpForParsing("testingFiles/SET_Testing/test_SETpolishTooManyOperators.txt", TEST_WHITEBOX);
    sput_fail_unless(parse() == 0, "Will not parse SET command with unbalanced (too many operators) reverse polish equation");
    shutDownParsing(TEST_WHITEBOX);
}













