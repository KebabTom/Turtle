#include "../includes/interpreter.h"

// pi is not always defined in all math.h libraries, so if not then define here
#ifndef M_PI
#define M_PI 3.1415926535
#endif


struct turtle {
    double x, y;
    int angle;
    PenUpDown penStatus;
    Clr drawColour;
    
    int drawTurtle;
} ;

struct positionStack {
    int numOfPositions;
    PositionNode top;
} ;

struct positionNode {
    double x, y;
    int angle;
    PositionNode next;
} ;

void setUpForInterpreting(int testing)
{
    createTurtle();
    initialiseTurtle(testing);
    createPositionStack();
    
    Turtle t = getTurtlePointer(NULL);
    if(t->drawTurtle) {
        setUpDisplay();
    }
}

void shutDownInterpreting()
{
    Turtle t = getTurtlePointer(NULL);
    if(t->drawTurtle) {
        shutDownDisplay();
    }
    freeTurtle();
    freePositionStack();
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
    
}

Turtle getTurtlePointer(Turtle newTurtle)
{
    static Turtle t;
    if(newTurtle != NULL) {
        t = newTurtle;
    }
    
    return t;
}

void initialiseTurtle(int testing)
{
    Turtle t = getTurtlePointer(NULL);
    
      //start turtle halfway across and halfway down the screen
    t->x = SCREEN_WIDTH/2.0;
    t->y = SCREEN_HEIGHT/2.0;
    
    t->angle = 0;
    t->penStatus = penDown;
    t->drawColour = white;
    
    if(testing == TEST_WHITEBOX || testing == TEST_BLACKBOX) {
        t->drawTurtle = DRAW_SDL_IN_TESTS;
    } else {
        t->drawTurtle = 1;
    }
}
    
void freeTurtle()
{
    Turtle t = getTurtlePointer(NULL);
    free(t);
}
    
//////////////////////////////////////////////////////////////////////////////////////////////
// POSITION STACK FUNCTIONS
///////////////////////////////

void createPositionStack()
{
    PositionStack newStack = (PositionStack) malloc(sizeof(struct positionStack));
    if(newStack == NULL) {
        fprintf(stderr, "ERROR - unable to malloc space for PositionStack in createPositionStack()\n");
        exit(1);
    }
    
    newStack->top = NULL;
    newStack->numOfPositions = 0;
    getPositionStackPointer(newStack);
}

PositionStack getPositionStackPointer(PositionStack newStack)
{
    static PositionStack pStack;
    if(newStack != NULL) {
        pStack = newStack;
    }
    
    return pStack;
}

void freePositionStack()
{
    PositionStack pStack = getPositionStackPointer(NULL);
    
    for(int i = 0; i < pStack->numOfPositions; i++) {
        PositionNode tmp = pStack->top;
        pStack->top = pStack->top->next;
        free(tmp);
    }
    
    free(pStack);
}


//  POSITION NODE FUNCTIONS  //////////////////////////////////////////
/*...................................................................*/

PositionNode newPositionNode()
{
    PositionNode newNode = (PositionNode) malloc (sizeof(struct positionNode));
    if(newNode == NULL) {
        fprintf(stderr, "ERROR - malloc failed for PositionNode in newPositionNode()\n");
        exit(1);
    }
    newNode->next = NULL;
    return newNode;
}

void pushToPositionStack(PositionNode pNode)
{
    PositionStack pStack = getPositionStackPointer(NULL);
    pNode->next = pStack->top;
    pStack->top = pNode;
    pStack->numOfPositions++;
}

PositionNode popFromPositionStack()
{
    PositionStack pStack = getPositionStackPointer(NULL);
    if(pStack->numOfPositions > 0) {
        PositionNode poppedNode = pStack->top;
        pStack->top = pStack->top->next;
        pStack->numOfPositions--;
        return poppedNode;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////
// MOVE PROCESSING FUNCTIONS
////////////////////////////

void doAction(TokenType actionType, double val)
{
    Turtle t = getTurtlePointer(NULL);
    
    switch (actionType) {
        case rt :
            storeTurtlePosition(t);
            t->angle = (t->angle + (int) val) % MAX_ANGLE;
            break;
        case lt :
            storeTurtlePosition(t);
            t->angle = (t->angle + (MAX_ANGLE - (int) val) ) % MAX_ANGLE;
            break;
        case fd :
            storeTurtlePosition(t);
            moveTurtle(val);
            break;
        case bkStep :
            backstep(t, val);
            break;
        default :
            fprintf(stderr,"ERROR - incorrect token passed to doAction()\n");
            exit(1);
    }
}

void moveTurtle(int moveLength)
{
    Turtle t = getTurtlePointer(NULL);
    
    double radAngle = degreesToRad(t->angle);
    int moveDistance = 0;
    int stepLength = TURTLE_SPEED;
    while(moveDistance < moveLength) {
    
        if(moveDistance+stepLength > moveLength) {
            stepLength = moveLength - moveDistance;
        }
        
        double xAdjust = (double) stepLength * sin(radAngle);
        double yAdjust = (double) stepLength * cos(radAngle);
        
        if(t->drawTurtle) {
            if(t->penStatus == penDown) {
                drawLine((int)t->x, (int)t->y, (int)(t->x + xAdjust), (int)(t->y - yAdjust));
            }
        }
        t->x += xAdjust;
        t->y -= yAdjust;
        
        moveDistance+=stepLength;
    }
}

void storeTurtlePosition(Turtle t)
{
    PositionNode pNode = newPositionNode();
    pNode->x = t->x;
    pNode->y = t->y;
    pNode->angle = t->angle;
    
    pushToPositionStack(pNode);
}

void backstep(Turtle t, int steps)
{
    for(int i = 0; i < steps; i++) {
        PositionNode pNode = popFromPositionStack();
        if(pNode != NULL) {
            t->x = pNode->x;
            t->y = pNode->y;
            t->angle = pNode->angle;
            free(pNode);
        }
    }
}

void switchPenStatus()
{
    Turtle t = getTurtlePointer(NULL);
    
    if(t->penStatus == penUp) {
        t->penStatus = penDown;
    } else {
        t->penStatus = penUp;
    }
}

//////////////////////////////////////////////////////////////////////
// MATHS FUNCTIONS
//////////////////

double degreesToRad(int deg)
{
    return (double) deg * (M_PI/180.0); //multiply angle in degrees by pi/180 to give angle in radians
}


//////////////////////////////////////////////////////////////////////
// WHITE BOX TESTING FUNCTIONS
//////////////////////////////

void runInterpreterWhiteBoxTests()
{
    
	  sput_start_testing();
	
	  sput_set_output_stream(NULL);	
	
	  sput_enter_suite("testTurtleInitialisation(): Checking Turtle structure is properly initialised");
    sput_run_test(testTurtleInitialisation);
    sput_leave_suite();
    
    sput_enter_suite("testTurtleActions(): Checking actions are properly carried out");
    sput_run_test(testTurtleActions);
    sput_leave_suite();

	  sput_finish_testing();
}

void testTurtleInitialisation()
{
    setUpForInterpreting(TEST_WHITEBOX);
    Turtle t = getTurtlePointer(NULL);
    
    sput_fail_unless(t->x == SCREEN_WIDTH/2, "Turtle initialised with correct x coordinate");
    sput_fail_unless(t->y == SCREEN_HEIGHT/2, "Turtle initialised with correct y coordinate");
    sput_fail_unless(t->penStatus == penDown, "Turtle initialised with correct pen status");
    sput_fail_unless(t->angle == 0, "Turtle initialised with correct angle");
    sput_fail_unless(t->drawColour == white, "Turtle initialised with correct colour");
    
    shutDownInterpreting();
}


void testTurtleActions()
{
    setUpForInterpreting(TEST_WHITEBOX);
    Turtle t = getTurtlePointer(NULL);
    
    doAction(rt, 60);
    sput_fail_unless(t->angle == 60, "Turtle rotated right with correct angle");
    doAction(lt, 30);
    sput_fail_unless(t->angle == 30, "Turtle rotated left with correct angle");
    doAction(lt, 90);
    sput_fail_unless(t->angle == 300, "Turtle rotated left past 0 with correct angle");
    doAction(rt, 65);
    sput_fail_unless(t->angle == 5, "Turtle rotated right past 360 with correct angle");
    
    t->angle = 0;
    doAction(fd, 50);
    sput_fail_unless((int) t->y == (SCREEN_HEIGHT/2) - 50, "Turtle moved up the correct ammount");
    doAction(rt, 90);
    doAction(fd, 50);
    sput_fail_unless((int) t->x == (SCREEN_WIDTH/2) + 50, "Turtle moved right the correct ammount");
    
    t->x = 0; t->y = 0; t->angle = 0;
    // set up 3, 4, 5 triangle and check movement ammounts
    doAction(rt, 45);
    doAction(fd, 5000);
    sput_fail_unless((int) t->x + (int) t->y == 0, "After move at 45 degrees, x and y have been changed by the same ammount");
    
    shutDownInterpreting();
}

















