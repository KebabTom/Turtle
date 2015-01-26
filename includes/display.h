#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>


typedef struct sdl_Simplewin *SDL_Simplewin;

#define MILLISECOND_DELAY 25
#define TURTLE_SPEED 4

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700

// SDL INITIALISATION FUNCTIONS
void setUpDisplay();
void createSDL_Simplewin();
SDL_Simplewin getSDL_SimplewinPointer(SDL_Simplewin newSimplewin);
void initialiseSDL();
void shutDownDisplay();

// SDL DRAWING FUNCTIONS
void drawBlackBackground();
void drawLine();
void setSDLDrawColour(SDL_Simplewin sw, Uint8 r, Uint8 g, Uint8 b);
void Neill_SDL_Events(SDL_Simplewin sw);
void holdScreenUntilUserInput();
