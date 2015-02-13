#include "print.h"
#include <stdio.h>

int InitSDL(void)
{
    if (-1 == SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    printf("Initialized SDL\n");
    return 1;
}

SDL_Surface* InitScreen(int width, int height, const char *title)
{

    SDL_Surface *screen;
    // bits per pixel, 0 for current bpp
    int bpp = 8;
    Uint32 flags = SDL_SWSURFACE | SDL_RESIZABLE | SDL_ANYFORMAT;

    screen = SDL_SetVideoMode(width, height, bpp, flags);
    if (NULL == screen)
    {
        fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
        return NULL;
    }

    printf("Set %dx%d at %d bits-per-pixel mode\n",
            screen->w, screen->h, screen->format->BitsPerPixel);

    SDL_WM_SetCaption(title, title);

    return screen;
}

void FreeScreen(SDL_Surface *screen)
{
    if (screen)
    {
        SDL_FreeSurface(screen);
    }
}

void printPoint(SDL_Surface *screen, int x, int y, Uint32 colour)
{
    putpixel(screen, x, y, colour);
}

void delay(int ms)
{
    SDL_Delay(ms);
}

void QuitSDL(void)
{
    SDL_Quit();
}

/*
 *  * Set the pixel at (x, y) to the given value
 *   * NOTE: The surface must be locked before calling this!
 *    */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    if (SDL_MUSTLOCK(surface))
    {
        if (-1 == SDL_LockSurface(surface))
        {
            fprintf(stderr, "SDL_LockSurface failed: %s\n", SDL_GetError());
            return;
        }
    }
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
    SDL_UpdateRect(surface, x, y, 1, 1);
}
