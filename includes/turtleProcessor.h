#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/sput.h"

#define MAX_ANGLE 360
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

typedef struct moveHistoryStack *MoveHistoryStack ;
typedef struct move *Move ;
typedef struct turtle *Turtle ;

enum tokenType {
  instrctlist, instruction, fd, lt, rt, varnum, var, set, polish, op, equals, val, semicolon,
  from, to, openBrace, closeBrace, DO, num, noToken, assignedVar, unassignedVar
} ;
typedef enum tokenType TokenType;

enum penUpDown {
    penUp, penDown
} ;
typedef enum penUpDown PenUpDown;

enum clr {
    black, white, red, blue, green
} ;
typedef enum clr Clr;


void setUpForInterpreting(int testing);
void shutDownInterpreting(int testing);

// TURTLE STRUCTURE FUNCTIONS
void createTurtle();
Turtle getTurtlePointer(Turtle newTurtle);
void initialiseTurtle();

// MOVE HISTORY STACK FUNCTIONS
void createMoveHistoryStack();
MoveHistoryStack getMoveHistoryStackPointer(MoveHistoryStack newStack);

// MOVE HANDLING FUNCTIONS
void doMove(TokenType moveType, double val);

// WHITE BOX TESTING FUNCTIONS
void runInterpreterWhiteBoxTests();
