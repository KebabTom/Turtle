

#include "parser.h"
#include <time.h>


#define TEST_WHITEBOX 1
#define TEST_BLACKBOX 2
#define TEST_SYSTEM 3
#define TEST_ALL 4

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
void testDOloops();
void testBackstepCommand();
void testWhileCommand();
void testPenSwitchCommand();
void testColourCommand();

// SYSTEM TESTS
void runSystemTesting();
void systemTesting();
