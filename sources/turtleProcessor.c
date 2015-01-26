#include "../includes/turtleProcessor.h"

struct theTurtle {
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
