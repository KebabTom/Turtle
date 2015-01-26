#include "../includes/interpreter.h"

struct turtle {
    int x, y;
    int angle;
    int moveLength;
    PenUpDown penStatus;
    Clr drawColour;
} ;

struct moveHistoryStack {
    int numOfMoves;
    Move top;
} ;

struct move {
    TokenType moveType;
    double val;
} ;

void setUpForInterpreting(int testing)
{
    createTurtle();
    createMoveHistoryStack();
}

void shutDownInterpreting(int testing)
{

}

//////////////////////////////////////////////////////////////////////////////////////////////
// TURTLE STRUCTURE FUNCTIONS
/////////////////////////////

void createTurtle() {
    Turtle newTurtle = (Turtle) malloc(sizeof(struct turtle));
    if(newTurtle == NULL) {
        fprintf(stderr, "ERROR - unable to malloc space for Turtle in createTurtle()\n");
        exit(1);
    }
    
    getTurtlePointer(newTurtle);
    
    initialiseTurtle();
}

Turtle getTurtlePointer(Turtle newTurtle)
{
    static Turtle t;
    if(newTurtle != NULL) {
        t = newTurtle;
    }
    
    return t;
}

void initialiseTurtle()
{
    Turtle t = getTurtlePointer(NULL);
    
    t->x = SCREEN_WIDTH/2;
    t->y = SCREEN_WIDTH/2;
    t->angle = 0;
    t->moveLength = 0;
    t->penStatus = penDown;
    t->drawColour = black;
}
    
    
//////////////////////////////////////////////////////////////////////////////////////////////
// MOVE HISTORY STACK FUNCTIONS
///////////////////////////////

void createMoveHistoryStack()
{
    MoveHistoryStack newStack = (MoveHistoryStack) malloc(sizeof(struct moveHistoryStack));
    if(newStack == NULL) {
        fprintf(stderr, "ERROR - unable to malloc space for moveHistoryStack in createMoveHistoryStack()\n");
        exit(1);
    }
    
    newStack->top = NULL;
    newStack->numOfMoves = 0;
    getMoveHistoryStackPointer(newStack);
}

MoveHistoryStack getMoveHistoryStackPointer(MoveHistoryStack newStack)
{
    static MoveHistoryStack mStack;
    if(newStack != NULL) {
        mStack = newStack;
    }
    
    return mStack;
}

void doMove(TokenType moveType, double val)
{

}

void runInterpreterWhiteBoxTests()
{
    
	sput_start_testing();
	
	sput_set_output_stream(NULL);	

	sput_finish_testing();
}
