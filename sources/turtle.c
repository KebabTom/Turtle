
#include "../includes/turtle.h"

#define TEST_STR_LEN 100

int main(int argc, char *argv[])

{

    int inputOK = checkInput(argc, argv, NO_TESTING);
    if(!inputOK) {
        exit(1);
    }
    
    processInput(argc, argv, NO_TESTING);
    
    int parsedOK = parse();
    
    if(parsedOK) {
        printf("Parsed OK\n");
    }
    
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
                fclose(fp);
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
    }
}

void processInput(int argc, char *argv[], int testing)
{
    // if two arguments, set up the parse heper using filepath from argv[1]
    if(argc == 2) {
        createParseHelper();
        initialiseParseHelper(argv[1], testing);
    }
    
    
    // if three arguments, choose testing type based on argv[2]
    if(argc == 3) {
        if(strcmp(argv[2],"white") == 0) {
            runWhiteBoxTesting();
            exit(0);
        }
        if(strcmp(argv[2],"black") == 0) {
            runBlackBoxTesting();
            exit(0);
        }
        if(strcmp(argv[2],"all") == 0) {
            runWhiteBoxTesting();
            runBlackBoxTesting();
            exit(0);
        }
    }
    
}
    



void runWhiteBoxTesting()
{
	  
    runCommandLineTests();
    runParserWhiteBoxTests();
    
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
    
    sput_fail_unless(checkInput(argc, argv, TESTING) == 0, "Input check detects error when only one argument passed");
    
    argc = 4;
    sput_fail_unless(checkInput(argc, argv, TESTING) == 0, "Input check detects error when more than 3 arguments passed");
    
    argc = 3;
    strcpy(argv[1], "testwrong");
    sput_fail_unless(checkInput(argc, argv, TESTING) == 0, "Input check detects error when 2nd of 3 arguments is not 'test' passed");
    
    strcpy(argv[1], "test");
    strcpy(argv[2], "all");
    sput_fail_unless(checkInput(argc, argv, TESTING) == 1, "Input check OK when 3 arguments and third is 'all'");
    strcpy(argv[2], "white");
    sput_fail_unless(checkInput(argc, argv, TESTING) == 1, "Input check OK when 3 arguments and third is 'white'");
    strcpy(argv[2], "black");
    sput_fail_unless(checkInput(argc, argv, TESTING) == 1, "Input check OK when 3 arguments and third is 'black'");
    strcpy(argv[2], "testwrong");
    sput_fail_unless(checkInput(argc, argv, TESTING) == 0, "Input check OK when 3 arguments and third is neither 'all', 'black' or 'white'");
    
    for(int i = 0; i < 4; i++) {
        free(argv[i]);
    }

}




















