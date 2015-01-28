#include "../includes/interpreter.h"

// pi is not always defined in all math.h libraries, so if not then define here
#ifndef M_PI
#define M_PI 3.1415926535
#endif


struct variable {
    
    char varName;
    double contents;
    int assigned;
} ;


struct turtle {
    double x, y;
    int angle;
    PenUpDown penStatus;
    Clr drawColour;
    
    int drawTurtle;
    Variable varList[NUMBER_OF_VARIABLES];
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

struct valStack {

  int numOfVals;
  ValNode top;
} ;

struct valNode {

  double val;
  ValNode next;
} ;

////  SETUP/SHUTDOWN FUNCTIONS //////////////////////////////////////////////////////
/*..........................................................................*/

void setUpForInterpreting(int testMode, int interpretMode)
{
    createTurtle();
    initialiseTurtle(testMode, interpretMode);
    createPositionStack();
    createValStack();
    
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

void initialiseTurtle(int testMode, int interpretMode)
{
    Turtle t = getTurtlePointer(NULL);
    
      //start turtle halfway across and halfway down the screen
    t->x = SCREEN_WIDTH/2.0;
    t->y = SCREEN_HEIGHT/2.0;
    
    t->angle = 0;
    t->penStatus = penDown;
    t->drawColour = white;
    
    initialiseVariableList(t);
    
    if(testMode == NO_TESTING) {
        if(interpretMode == INTERPRET) {
            t->drawTurtle = 1;
        } else {
            t->drawTurtle = 0;
        }
    } else {
        t->drawTurtle = DRAW_SDL_IN_TESTS;
    }
}

void initialiseVariableList(Turtle t)
{
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        t->varList[i] = (Variable) malloc(sizeof(struct variable));
        if(t->varList[i] == NULL) {
            fprintf(stderr, "ERROR - unable to malloc space for variable structure in initialiseVariableList()");
            exit(1);
        }
        t->varList[i]->varName = 'A'+i;
        t->varList[i]->assigned = 0;
    }
}

void freeTurtle()
{
    Turtle t = getTurtlePointer(NULL);
    
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        free(t->varList[i]);
    }
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


///////////////////////////////////////////////////////////////////
// VAL STACK FUNCTIONS
/////////////////////////

void createValStack()
{
    ValStack newStack = (ValStack) malloc(sizeof(struct valStack));
    if(newStack == NULL) {
        fprintf(stderr, "ERROR - unable to malloc space for valstack in createValStack()\n");
        exit(1);
    }
    
    newStack->top = NULL;
    newStack->numOfVals = 0;
    getValStackPointer(newStack);
}

ValStack getValStackPointer(ValStack newStack)
{
    static ValStack vStack;
    if(newStack != NULL) {
        vStack = newStack;
    }
    
    return vStack;
}

void pushToValStack(double val)
{
    ValStack vStack = getValStackPointer(NULL);
    
    ValNode vN = newValNode();
    vN->val = val;
    vN->next = vStack->top;
    vStack->numOfVals++;
    vStack->top = vN;
}


ValNode newValNode()
{
    ValNode newNode = (ValNode) malloc(sizeof(struct valNode));
    if(newNode == NULL) {
        fprintf(stderr, "ERROR - Unable to malloc space for val node in newValNode()\n");
        exit(1);
    }
    
    newNode->next = NULL;
    return newNode;
}

int popFromValStack(double *poppedVal)
{
    ValStack vStack = getValStackPointer(NULL);
    
    if(vStack->numOfVals > 0) {
        *poppedVal = vStack->top->val;
        ValNode tmp = vStack->top;
        vStack->top = vStack->top->next;
        free(tmp);
        vStack->numOfVals--;
        return 1;
    } else {
        return 0;
    }
}

int getNumberOfValsOnStack()
{
    return getValStackPointer(NULL)->numOfVals;
}
    
void freeValStack()
{
    ValStack vStack = getValStackPointer(NULL);
    
    for(int i = 0; i < vStack->numOfVals; i++) {
        ValNode tmp = vStack->top;
        vStack->top = vStack->top->next;
        free(tmp);
    }
    
    free(vStack);
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


//  DRAWING STYLE FUNCTIONS //////////////////////////////////////////
/*..................................................................*/

void switchPenStatus()
{
    Turtle t = getTurtlePointer(NULL);
    
    if(t->penStatus == penUp) {
        t->penStatus = penDown;
    } else {
        t->penStatus = penUp;
    }
}

void advanceTurtleColour()
{
    Turtle t = getTurtlePointer(NULL);
    t->drawColour = (t->drawColour + 1) % NUM_OF_COLOURS;
    applyTurtleColour(t->drawColour);

}

void setRandomTurtleColour()
{
    Turtle t = getTurtlePointer(NULL);
    t->drawColour = rand()% NUM_OF_COLOURS;
    applyTurtleColour(t->drawColour);
}

void applyTurtleColour(Clr colour)
{
    Turtle t = getTurtlePointer(NULL);
    t->drawColour = colour;
    if(t->drawTurtle) {
        setDrawColour(t->drawColour);
    }
}

//////////////////////////////////////////////////////////////////////
// MATHS FUNCTIONS
//////////////////

double degreesToRad(int deg)
{
    return (double) deg * (M_PI/180.0); //multiply angle in degrees by pi/180 to give angle in radians
}

double doMaths(double a, double b, mathSymbol op)
{
    switch(op) {
        case add :
            return a+b;
        case subtract :
            return a-b;
        case multiply :
            return a*b;
        case divide :
            return a/b;
        default :
            fprintf(stderr, "ERROR - invalid maths operator passed to doMaths()\n");
            exit(1);
    }
}

///// INFORMATION RETURNING FUNCTIONS ////////////////////////////////
/*..................................................................*/

double getVariableVal(char c)
{
    Turtle t = getTurtlePointer(NULL);
    
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(t->varList[i]->varName == c) {
            return t->varList[i]->contents;
        }
    }
    
    fprintf(stderr, "ERROR - attempted to extract contents of variable not in varList in getTokenVal()\n");
    exit(1);
}

/*
returns 1 if passed character is within list of potential variables. If not, returns 0.
*/
int checkValidVariable(char c)
{
    Turtle t = getTurtlePointer(NULL);
    
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(t->varList[i]->varName == c) {
            return 1;
        }
    }
    
    return 0;
}

// if variable is marked as assigned, returns 1 and updates the parseHandler's current val to the variable's value (if interpreting)
int checkVariableAssigned(char c, int interpret, double *valToSet)
{
    Turtle t = getTurtlePointer(NULL);
    
    for(int i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if(t->varList[i]->varName == c) {
            if(t->varList[i]->assigned == 1) {
                if(interpret) {
                    *valToSet = t->varList[i]->contents;
                }
                return 1;
            }
        }
    }
    return 0;
}

int checkForNumber(char *token, double *valToSet)
{
    char *remainder;
    *valToSet = strtod(token, &remainder);
    if(remainder[0] != '\0') {
        return 0;
    } else {
        return 1;
    }
}

// if indicated variable existsand returns 1, else returns 0
// if parser is set in interpret mode, assigns value to variable
void assignValToVariable(char varToSet, double val, int interpret)
{
    Turtle t = getTurtlePointer(NULL);
    
    int foundVar = 0;
    for(int i = 0; i < NUMBER_OF_VARIABLES && foundVar == 0; i++) {
        if(t->varList[i]->varName == varToSet) {
            foundVar = 1;
            t->varList[i]->assigned = 1;
            if(interpret) {
                t->varList[i]->contents = val;
            }
        }
    }
    if(foundVar != 1) {
        fprintf(stderr, "ERROR - unable to locate variable in assignValToVariable()\n");
        exit(1);
    }
}

int getTurtleX()
{
    return (int) getTurtlePointer(NULL)->x;
}

int getTurtleY()
{
    return (int) getTurtlePointer(NULL)->y;
}

int getTurtleAngle()
{
    return getTurtlePointer(NULL)->angle;
}

Clr getTurtleColour()
{
    return getTurtlePointer(NULL)->drawColour;
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
    
    sput_enter_suite("testValStack(): Testing pushing and popping from Val Stack");
    sput_run_test(testValStack);
    sput_leave_suite();

	  sput_finish_testing();
}

void testTurtleInitialisation()
{
    setUpForInterpreting(TESTING, DONT_INTERPRET);
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
    setUpForInterpreting(TESTING, DONT_INTERPRET);
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


void testValStack()
{
    createValStack();
    pushToValStack(3);
    pushToValStack(12.4);
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 2, "After pushing two values to stack, number of items in stack is 2");
    freeValStack();
    
    createValStack();
    pushToValStack(25);
    pushToValStack(13);
    pushToValStack(20);
    double d = 0;
    sput_fail_unless(popFromValStack(&d) == 1 && (int) d == 20, "Stack successfully pops value from stack");
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 2, "After pushing 3 values and popping 1, number of items in stack is 2");
    
    popFromValStack(&d);
    popFromValStack(&d);
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 0, "After pushing 3 values and popping 3, number of items in stack is 0");
    sput_fail_unless((int) d == 25, "Successfully pops 3 values from stack");
    
    sput_fail_unless(popFromValStack(&d) == 0, "Does not alow pop from empty stack");
    sput_fail_unless((int) d == 25, "Target value unchanged after pop from empty stack");
    sput_fail_unless(getValStackPointer(NULL)->numOfVals == 0, "Items in stack is still 0 after pop from empty stack");
    freeValStack();
}
















