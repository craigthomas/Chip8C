/**
 * Copyright (C) 2012 Craig Thomas
 *
 * @file      screen.c
 * @brief     Routines for addressing emulator screen
 * @author    Craig Thomas
 * @copyright MIT style license - see the LICENSE file for details
 * @copyright @verbinclude LICENSE
 *
 * The emulator defines a primary SDL surface called `screen`. The surface is
 * used to draw pixels on. By default, it attempts to create a double-buffered
 * video memory based surface with 8 bits per pixel. Note that if you wish to
 * change the number of bits per pixel, you will need to update the 
 * `screen_getpixel` function accordingly. 
 */

/* I N C L U D E S ************************************************************/

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include "globals.h"

TTF_Font *message_font;

/* F U N C T I O N S **********************************************************/

/**
 * Attempts to initialize the colors WHITE and BLACK. Does so by mapping RGB
 * values based upon the screen format. 
 */
void 
init_colors (SDL_Surface *surface) 
{
    COLOR_BLACK = SDL_MapRGB(surface->format, 0, 0, 0);
    COLOR_WHITE = SDL_MapRGB(surface->format, 250, 250, 250);
    COLOR_DGREEN = SDL_MapRGB(surface->format, 0, 70, 0);
    COLOR_LGREEN = SDL_MapRGB(surface->format, 0, 200, 0);
    COLOR_TEXT.r = 255;
    COLOR_TEXT.g = 255;
    COLOR_TEXT.b = 255;
}

/******************************************************************************/

/**
 * Returns whether or not the pixel at location x, y is on or off. Returns 1
 * if the pixel is turned on, 0 otherwise. Pixel coordinates are based upon the
 * unscaled size of the screen (64 x 32).
 *
 * @param x the x coordinate of the pixel to check
 * @param y the y coordinate of the pixel to check
 * @returns 1 if the pixel is on, 0 otherwise
 */
int 
screen_getpixel(int x, int y)
{
    Uint8 r, g, b;
    Uint32 color = 0;
    int pixelcolor = 0;
    x = x * scale_factor;
    y = y * scale_factor;
    Uint32 *pixels = (Uint32 *)virtscreen->pixels;
    Uint32 pixel = pixels[(virtscreen->w * y) + x];
    SDL_GetRGB(pixel, virtscreen->format, &r, &g, &b);
    color = SDL_MapRGB(virtscreen->format, r, g, b);
    if (color == COLOR_WHITE) {
        pixelcolor = 1;
    }
    return pixelcolor;
}

/******************************************************************************/

/**
 * Helper routine to blit one surface onto another surface at the specified
 * location. This routine blits the entire source surface onto the destination.
 *
 * @param src the source surface to blit
 * @param dest the destination surface to blit to
 * @param x the x location on the destination to blit to
 * @param y the y location on the destination to blit to
 */
void 
screen_blit_surface(SDL_Surface *src, SDL_Surface *dest, int x, int y)
{
    SDL_Rect location;
    location.x = x;
    location.y = y;
    SDL_BlitSurface(src, NULL, dest, &location);
}

/******************************************************************************/

/**
 * Helper routine to create an SDL surface that contains a text message.
 *
 * @param msg the message to blit
 * @param text_color the color of the text to draw
 * @return the SDL_Surface with the message text
 */
SDL_Surface *
screen_render_message(char *msg, SDL_Color text_color)
{
    SDL_Surface *message_surface; 

    message_surface = TTF_RenderText_Solid(message_font, msg, text_color);
    return message_surface;
}

/******************************************************************************/

/**
 * Generates the current state of the CPU in an overlay on the main screen.
 * All of the CPU registers, along with the operand and a description of the
 * operand will be printed to the overlay. The overlay has an alpha channel
 * set such that it will not overwrite the Chip 8 screen - it will appear to 
 * be semi-transparent. 
 */
void 
screen_trace_message(void)
{
    SDL_Surface *msg_surface;
    SDL_Rect box;

    box.x = 5;
    box.y = screen_height - 58;
    box.w = 342;
    box.h = 53;
    SDL_FillRect(overlay, &box, COLOR_LGREEN);

    box.x = 6;
    box.y = screen_height - 57;
    box.w = 340;
    box.h = 51;
    SDL_FillRect(overlay, &box, COLOR_DGREEN);

    char *buffer = (char *)malloc(MAXSTRSIZE);
    
    sprintf(buffer, "I:%04X DT:%02X ST:%02X PC:%04X %04X %s", 
             cpu.i.WORD, cpu.dt, cpu.st, cpu.oldpc.WORD, cpu.operand.WORD, 
             cpu.opdesc);
    msg_surface = screen_render_message(buffer, COLOR_TEXT);
    screen_blit_surface(msg_surface, overlay, 10, screen_height - 53);
    SDL_FreeSurface(msg_surface);

    sprintf(buffer, 
             "V0:%02X V1:%02X V2:%02X V3:%02X V4:%02X V5:%02X V6:%02X V7:%02X",
             cpu.v[0], cpu.v[1], cpu.v[2], cpu.v[3], cpu.v[4], cpu.v[5],
             cpu.v[6], cpu.v[7]);
    msg_surface = screen_render_message(buffer, COLOR_TEXT);
    screen_blit_surface(msg_surface, overlay, 10, screen_height - 38);
    SDL_FreeSurface(msg_surface);

    sprintf(buffer, 
             "V8:%02X V9:%02X VA:%02X VB:%02X VC:%02X VD:%02X VE:%02X VF:%02X",
             cpu.v[8], cpu.v[9], cpu.v[10], cpu.v[11], cpu.v[12], cpu.v[13],
             cpu.v[14], cpu.v[15]);
    msg_surface = screen_render_message(buffer, COLOR_TEXT);
    screen_blit_surface(msg_surface, overlay, 10, screen_height - 23);
    SDL_FreeSurface(msg_surface);
  
    free(buffer); 
}

/******************************************************************************/

/**
 * Blanks out the virtual screen (needed for the CPU, since we want to keep the
 * CPU well separated from the SDL surfaces that are used in the screen
 * routines).
 */
void 
screen_blank(void)
{
    screen_clear(virtscreen, COLOR_BLACK);
}

/******************************************************************************/

/**
 * Clears the screen by setting all pixels to off (0).
 */
void 
screen_clear(SDL_Surface *surface, Uint32 color)
{
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = screen_width;
    rect.h = screen_height;
    SDL_FillRect(surface, &rect, color);
}

/******************************************************************************/

/**
 * Refreshes the screen. If overlay_on is a non-zero value, then the debug 
 * overlay will be turned on and will be painted with the refresh.
 */
void 
screen_refresh(int overlay_on)
{
    screen_blit_surface(virtscreen, screen, 0, 0);
    if (overlay_on) {
        screen_trace_message();
        screen_blit_surface(overlay, screen, 0, 0);
    }
    screen_clear(overlay, COLOR_BLACK);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/******************************************************************************/

/**
 * Draws a pixel on the screen at coordinates x, y with the specified color.
 * The color is simply 1 (for on) or 0 (for off). The x and y coordinates are
 * based on the unscaled size of the screen (64 x 32).
 *
 * @param x the x coordinate of the pixel
 * @param y the y coordinate of the pixel
 * @param color the color of the pixel - 1 (on) or 0 (off)
 */
void 
screen_draw(int x, int y, int color)
{
    SDL_Rect pixel;
    Uint32 pixelcolor = COLOR_BLACK;

    pixel.x = x * scale_factor;
    pixel.y = y * scale_factor;
    pixel.w = scale_factor;
    pixel.h = scale_factor;
    if (color) {
        pixelcolor = COLOR_WHITE;
    }
    SDL_FillRect(virtscreen, &pixel, pixelcolor);
}

/******************************************************************************/

/**
 * Creates a new SDL surface with the specified dimensions, per-surface alpha
 * value, and the specified color_key. If color_key is equal to -1, then color
 * keys are turned off for the surface. The new surface will be exclusively a
 * software surface. Returns NULL if there was a problem creating the surface.
 * Note that it is also up to the user to call SDL_FreeSurface when they are
 * done with the surface. 
 *
 * @param width the width of the new surface, in pixels
 * @param height the height of the new surface, in pixels
 * @param alpha the alpha blend value of the entire surface
 * @param color_key the key to use as a color key for the surface
 * @returns the newly created surface
 */
SDL_Surface *
screen_create_surface(int width, int height, int alpha, Uint32 color_key)
{
    Uint32 rmask, gmask, bmask, amask;
    SDL_Surface *tempsurface, *newsurface;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    tempsurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 
                                    screen_width, screen_height, SCREEN_DEPTH,
                                    rmask, gmask, bmask, amask);
    newsurface = SDL_DisplayFormat(tempsurface);
    SDL_FreeSurface(tempsurface);

    if (newsurface == NULL) {
        printf("Error: unable to create new surface\n");
    } 
    else {
        SDL_SetAlpha(newsurface, SDL_SRCALPHA, alpha);
        if (color_key != -1) {
            SDL_SetColorKey(newsurface, SDL_SRCCOLORKEY, color_key);
        }
        screen_clear(newsurface, COLOR_BLACK);
    }

    return newsurface;
}

/******************************************************************************/

/**
 * Initializes the emulator primary surface. By default, attempts to create
 * a hardware based surface that is double buffered. To update the screen, you
 * must call screen_refresh. Returns TRUE if the screen was created.
 *
 * @returns TRUE if the screen was created, FALSE otherwise
 */
int 
screen_init(void)
{
    int result = FALSE;

    TTF_Init();
    message_font = TTF_OpenFont("VeraMono.ttf", 11);

    screen_width = SCREEN_WIDTH * scale_factor;
    screen_height = SCREEN_HEIGHT * scale_factor;
    screen = SDL_SetVideoMode(screen_width, 
                              screen_height, 
                              SCREEN_DEPTH,
                              SDL_SWSURFACE);

    if (screen == NULL) {
        printf("Error: Unable to set video mode: %s\n", SDL_GetError());
    } 
    else {
        SDL_SetAlpha(screen, SDL_SRCALPHA, 255);
        SDL_WM_SetCaption("YAC8 Emulator", NULL);
        init_colors(screen);
        virtscreen = screen_create_surface(screen_width, screen_height, 
                                            255, -1);
        overlay = screen_create_surface(screen_width, screen_height, 
                                         200, COLOR_BLACK);
        result = TRUE;
    }

    return ((virtscreen != NULL) && (overlay != NULL) && result);
}

/* E N D   O F   F I L E ******************************************************/
