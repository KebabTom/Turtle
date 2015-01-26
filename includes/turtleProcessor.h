#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/sput.h"

#define MAX_ANGLE 360

typedef struct moveHistoryStack *MoveHistoryStack ;
typedef struct move *Move ;
typedef struct theTurtle *TheTurtle ;

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


void createMoveHistoryStack();
MoveHistoryStack getMoveHistoryStackPointer(MoveHistoryStack newStack);

void doMove(TokenType moveType, double val);

void runInterpreterWhiteBoxTests();
