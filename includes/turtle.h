

#include "parser.h"


int checkInput(int argc, char *argv[], int testing);
void printCommandLineError(int testing);
int processInput(int argc, char *argv[]);



// TESTING FUNCTIONS
void runWhiteBoxTesting();
void runBlackBoxTesting();

// WHITE BOX TESTS
void runCommandLineTests();
void testCommandLine();

// PARSER BLACK BOX TESTS
void runParserBlackBoxTests();
void testSyntaxErrors();
void testVarNum();
void testSetCommand();
