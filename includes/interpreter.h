#include "display.h"
#include "../includes/sput.h"
#include <math.h>

#define MAX_ANGLE 360
#define DRAW_SDL_IN_TESTS 0


#define NO_TESTING 0
#define TEST_WHITEBOX 1
#define TEST_BLACKBOX 2
#define TEST_SYSTEM 3
#define TEST_ALL 4

typedef struct positionStack *PositionStack ;
typedef struct positionNode *PositionNode ;
typedef struct turtle *Turtle ;

enum tokenType {
  instrctlist, instruction, fd, lt, rt, varnum, var, set, polish, op, equals, val, semicolon,
  from, to, openBrace, closeBrace, doToken, whileToken, num, noToken, assignedVar, unassignedVar, bkStep,
  moreThan, lessThan, penChange, colour, randomColour, advanceColour
} ;
typedef enum tokenType TokenType;

enum penUpDown {
    penUp, penDown
} ;
typedef enum penUpDown PenUpDown;


void setUpForInterpreting(int testing);
void shutDownInterpreting();

// TURTLE STRUCTURE FUNCTIONS
void createTurtle();
Turtle getTurtlePointer(Turtle newTurtle);
void initialiseTurtle(int testing);
void freeTurtle();

// POSITION STACK FUNCTIONS
void createPositionStack();
PositionStack getPositionStackPointer(PositionStack newStack);
void freePositionStack();

// POSITION NODE FUNCTIONS

PositionNode newPositionNode();
void pushToPositionStack(PositionNode pNode);
PositionNode popFromPositionStack();

// MOVE HANDLING FUNCTIONS
void doAction(TokenType actionType, double val);
void moveTurtle(int moveLength);
void storeTurtlePosition(Turtle t);
void backstep(Turtle t, int steps);

// DRAWING STYLE FUNCTIONS
void switchPenStatus();
void advanceTurtleColour();
void setRandomTurtleColour();
void applyTurtleColour(Clr colour);


// MATHS FUNCTIONS
double degreesToRad(int deg);


int getTurtleX();
int getTurtleY();
int getTurtleAngle();
Clr getTurtleColour();

// WHITE BOX TESTING FUNCTIONS
void runInterpreterWhiteBoxTests();
void testTurtleInitialisation();
void testTurtleActions();














