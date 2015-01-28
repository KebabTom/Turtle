#include "display.h"
#include "../includes/sput.h"
#include <math.h>

#define DRAW_SDL_IN_TESTS 0 // set to 1 to draw SDL during testing

#define MAX_ANGLE 360 // used to mod turtle's angle so it never goes beyond 360 degrees. Technically not necessary but useful in testing
#define NUMBER_OF_VARIABLES 26 // the number of variables in the variable list (26 as A-Z)

#define INTERPRET 1 
#define DONT_INTERPRET 0


#define NO_TESTING 0 // passed as input to functions when user runs program. Means SDL will always be drawn when interpretting
#define TESTING 1 // passed as input to test functions - if passed, SDL is only displayed if indicated by DRAW_SDL_IN_TESTS


typedef struct positionStack *PositionStack ;
typedef struct positionNode *PositionNode ;
typedef struct turtle *Turtle ;
typedef struct variable *Variable;
typedef struct valStack *ValStack;
typedef struct valNode *ValNode;

// all possible tokens
enum tokenType {
  fd, lt, rt, varnum, var, set, polish, op, equals, val, semicolon,
  from, to, openBrace, closeBrace, doToken, whileToken, num, noToken, assignedVar, unassignedVar, bkStep,
  moreThan, lessThan, penChange, colour, randomColour, advanceColour
} ;
typedef enum tokenType TokenType;

enum mathSymbol {
  add, subtract, divide, multiply
} ;
typedef enum mathSymbol mathSymbol;

enum penUpDown {
    penUp, penDown
} ;
typedef enum penUpDown PenUpDown;

// SETUP/SHUTDOWN FUNCTIONS
void setUpForInterpreting(int testMode, int interpretMode);
void shutDownInterpreting();

// TURTLE STRUCTURE FUNCTIONS
void   createTurtle();
Turtle getTurtlePointer(Turtle newTurtle);
void   initialiseTurtle(int testMode, int interpretMode);
void   initialiseVariableList(Turtle t);
void   freeTurtle();

// POSITION STACK FUNCTIONS
void          createPositionStack();
PositionStack getPositionStackPointer(PositionStack newStack);
void          freePositionStack();

// POSITION NODE FUNCTIONS
PositionNode newPositionNode();
void         pushToPositionStack(PositionNode pNode);
PositionNode popFromPositionStack();

// VAL STACK FUNCTIONS
void     createValStack();
ValStack getValStackPointer(ValStack newStack);
void     pushToValStack(double val);
ValNode  newValNode();
int      popFromValStack(double *poppedVal);
int      getNumberOfValsOnStack();
void     freeValStack();

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
double doMaths(double a, double b, mathSymbol op);

// INFORMATION RETURNING FUNCTIONS
double getVariableVal(char c);
int    checkValidVariable(char c);
int    checkVariableAssigned(char c, int interpret, double *valToSet);
int    checkForNumber(char *token, double *valToSet);
void   assignValToVariable(char varToSet, double val, int interpret);
int    getTurtleX();
int    getTurtleY();
int    getTurtleAngle();
Clr    getTurtleColour();

// WHITE BOX TESTING FUNCTIONS
void runInterpreterWhiteBoxTests();
void testTurtleInitialisation();
void testTurtleActions();
void testValStack();
void testPositionStack();














