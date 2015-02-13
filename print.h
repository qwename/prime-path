#define PRINT_H
#ifdef PRINT_H

#include <SDL.h>
#include <SDL_ttf.h>

int InitSDL(void);
void QuitSDL(void);
SDL_Surface* InitScreen(int width, int height, const char *title);
void FreeScreen(SDL_Surface *screen);
void printPoint(SDL_Surface *screen, int x, int y, Uint32 colour);
void delay(int ms);

#endif
