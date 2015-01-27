
#include "../includes/display.h"


// All info required for windows / renderer & event loop
struct sdl_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
};

void setUpDisplay()
{
    createSDL_Simplewin();
    initialiseSDL();
    drawBlackBackground();
}

void createSDL_Simplewin()
{
    SDL_Simplewin sw = (SDL_Simplewin) malloc(sizeof(struct sdl_Simplewin));
    if(sw == NULL) {
        fprintf(stderr, "ERROR - malloc failed for SDL_Simplewin in createSDL_Simplewin()\n");
        exit(1);
    }
    
    getSDL_SimplewinPointer(sw);
    
}

SDL_Simplewin getSDL_SimplewinPointer(SDL_Simplewin newSimplewin)
{
    static SDL_Simplewin sw;
    if(newSimplewin != NULL) {
        sw = newSimplewin;
    }
    
    return sw;
}

void initialiseSDL()
{

    SDL_Simplewin sw = getSDL_SimplewinPointer(NULL);

   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   } 

   sw->finished = 0;
   
   sw->win= SDL_CreateWindow("SDL Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          SCREEN_WIDTH, SCREEN_HEIGHT,
                          SDL_WINDOW_SHOWN);
   if(sw->win == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Window:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   sw->renderer = SDL_CreateRenderer(sw->win, -1, 0);
   if(sw->renderer == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Renderer:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   SDL_RenderClear(sw->renderer);
   SDL_RenderPresent(sw->renderer);

}

void shutDownDisplay()
{
    SDL_Simplewin sw = getSDL_SimplewinPointer(NULL);
    
    free(sw);
}


//  SDL DRAWING FUNCTIONS  ////////////////////////////////////////////////////////////
/*...................................................................................*/


void drawBlackBackground()
{
    SDL_Simplewin sw = getSDL_SimplewinPointer(NULL);
    
    SDL_Rect rect;
    rect.w = SCREEN_WIDTH;
    rect.h = SCREEN_HEIGHT;
    rect.x = 0;
    rect.y = 0;
    
    setSDLDrawColour(sw, 0, 0, 0);
    SDL_RenderFillRect(sw->renderer, &rect);
    SDL_RenderPresent(sw->renderer);
    SDL_UpdateWindowSurface(sw->win);
    setSDLDrawColour(sw, 255, 255, 255);
}

void drawLine(int xFrom, int yFrom, int xTo, int yTo)
{
    SDL_Simplewin sw = getSDL_SimplewinPointer(NULL);
    if(!sw->finished) {
        SDL_Delay(MILLISECOND_DELAY);
        
        SDL_RenderDrawLine(sw->renderer, xFrom, yFrom, xTo, yTo);
        SDL_RenderPresent(sw->renderer);
        SDL_UpdateWindowSurface(sw->win);
        
        Neill_SDL_Events(sw);
    }
}


void setDrawColour(Clr colour)
{
    SDL_Simplewin sw = getSDL_SimplewinPointer(NULL);
    
    switch(colour) {
        case white :
            setSDLDrawColour(sw, WHITE_R, WHITE_G, WHITE_B);
            break;
        case red :
            setSDLDrawColour(sw, RED_R, RED_G, RED_B);
            break;
        case blue :
            setSDLDrawColour(sw, BLUE_R, BLUE_G, BLUE_B);
            break;
        case green :
            setSDLDrawColour(sw, GREEN_R, GREEN_G, GREEN_B);
            break;
        case yellow :
            setSDLDrawColour(sw, YELLOW_R, YELLOW_G, YELLOW_B);
            break;
        case purple :
            setSDLDrawColour(sw, PURPLE_R, PURPLE_G, PURPLE_B);
            break;
        default :
            fprintf(stderr, "ERROR - incorrect colour passed to setDrawColour\n");
    }
}

// Trivial wrapper to avoid complexities of renderer & alpha channels
void setSDLDrawColour(SDL_Simplewin sw, Uint8 r, Uint8 g, Uint8 b)
{

   SDL_SetRenderDrawColor(sw->renderer, r, g, b, SDL_ALPHA_OPAQUE);

}

// Gobble all events & ignore most
void Neill_SDL_Events(SDL_Simplewin sw)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) 
    {      
         if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
             sw->finished = 1;
             SDL_Quit();
         }
    }
}

void holdScreenUntilUserInput()
{
    SDL_Simplewin sw = getSDL_SimplewinPointer(NULL);
    while(!sw->finished) {
        Neill_SDL_Events(sw);
    }
}





















