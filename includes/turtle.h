

#include "parser.h"
#include <time.h>


void runFullProgram(char *filePath);
int checkInput(int argc, char *argv[], int testMode);
void exitWithCommandLineError();
int getStartMode(int argc, char *argv[]);



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
void testBackstepCommand();
void testWhileCommand();
void testPenSwitchCommand();
void testColourCommand();

// SYSTEM TESTS
void runSystemTesting();
void systemTesting();
