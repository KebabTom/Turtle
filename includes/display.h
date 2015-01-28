#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>



#define MILLISECOND_DELAY 25
#define TURTLE_SPEED 10

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700

#define WHITE_R 255
#define WHITE_G 255
#define WHITE_B 255
#define RED_R 255
#define RED_G 0
#define RED_B 0
#define BLUE_R 0
#define BLUE_G 0
#define BLUE_B 255
#define GREEN_R 0
#define GREEN_G 255
#define GREEN_B 0
#define YELLOW_R 255
#define YELLOW_G 255
#define YELLOW_B 0
#define PURPLE_R 189
#define PURPLE_G 0
#define PURPLE_B 189
#define BLACK_R 0
#define BLACK_G 0
#define BLACK_B 0

#define NUM_OF_COLOURS 6
enum clr {
    white, red, blue, green, yellow, purple
} ;
typedef enum clr Clr;

typedef struct sdl_Simplewin *SDL_Simplewin;

// SDL INITIALISATION FUNCTIONS
void setUpDisplay();
void createSDL_Simplewin();
SDL_Simplewin getSDL_SimplewinPointer(SDL_Simplewin newSimplewin);
void initialiseSDL();
void shutDownDisplay();

// SDL DRAWING FUNCTIONS
void drawBlackBackground();
void drawLine();
void setDrawColour(Clr colour);
void setSDLDrawColour(SDL_Simplewin sw, Uint8 r, Uint8 g, Uint8 b);
void Neill_SDL_Events(SDL_Simplewin sw);
void holdScreenUntilUserInput();
