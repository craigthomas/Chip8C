/**
 * Copyright (C) 2012-2025 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      screen.c
 * @brief     Routines for addressing emulator screen
 * @author    Craig Thomas
 *
 * The emulator defines a primary SDL surface called `screen`. The surface is
 * used to draw pixels on. By default, it attempts to create a double-buffered
 * video memory based surface with 8 bits per pixel. Note that if you wish to
 * change the number of bits per pixel, you will need to update the 
 * `screen_get_pixel` function accordingly. 
 */

/* I N C L U D E S ************************************************************/

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include "globals.h"

/* F U N C T I O N S **********************************************************/

/**
 * Attempts to initialize the 4 colors used by the emulator to draw to the 
 * screen.
 * 
 * @param surface the SDL_Surface object to initialize
 */
void 
init_colors(SDL_Surface *surface) 
{
    COLOR_0 = SDL_MapRGB(surface->format, 0, 0, 0);
    COLOR_1 = SDL_MapRGB(surface->format, 250, 51, 204);
    COLOR_2 = SDL_MapRGB(surface->format, 51, 204, 250);
    COLOR_3 = SDL_MapRGB(surface->format, 250, 250, 250);
}

/******************************************************************************/

/**
 * Returns the color value for the specified bitplane.
 * 
 * @param plane the bitplane color to look up
 * @returns the bitplane color code 
 */
Uint32
get_bitplane_color(int plane) 
{
    switch (plane) {
        case 0:
            return COLOR_0;
            break;

        case 1:
            return COLOR_1;
            break;
        
        case 2:
            return COLOR_2;
            break;

        default:
            return COLOR_3;
            break;
    }
}

/******************************************************************************/

/**
 * Returns whether or not the pixel at location x, y is on or off. Returns 1
 * if the pixel is turned on, 0 otherwise. Pixel coordinates are based upon the
 * unscaled size of the screen (64 x 32).
 *
 * @param x the x coordinate of the pixel to check
 * @param y the y coordinate of the pixel to check
 * @param plane the bitplane to check
 * @returns TRUE if the pixel is on, FALSE otherwise
 */
int 
get_pixel(int x, int y, int plane)
{
    if (bitplane == 0) {
        return FALSE;
    }

    Uint8 r, g, b;
    Uint32 color = 0;
    Uint32 bitplane_color = get_bitplane_color(plane);
    int mode_scale = screen_mode == SCREEN_MODE_EXTENDED ? 1 : 2;

    x = x * scale_factor * mode_scale;
    y = y * scale_factor * mode_scale;

    Uint32 *pixels = (Uint32 *)virtscreen->pixels;
    Uint32 pixel = pixels[(virtscreen->w * y) + x];
    SDL_GetRGB(pixel, virtscreen->format, &r, &g, &b);
    color = SDL_MapRGB(virtscreen->format, r, g, b);
    return color == bitplane_color || color == COLOR_3;
}

/******************************************************************************/

/**
 * Returns TRUE if the screen is in extended mode, FALSE otherwise.
 * 
 * @returns TRUE if the screen is in extended mode, FALSE otherwise
 */
int
screen_is_extended_mode(void) 
{
    return screen_mode == SCREEN_MODE_EXTENDED ? TRUE : FALSE;
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
 * Blanks out the virtual screen (needed for the CPU, since we want to keep the
 * CPU well separated from the SDL surfaces that are used in the screen
 * routines).
 * 
 * @param plane the bitplane to blank out
 */
void 
screen_blank(int plane)
{
    if (plane == 0) {
        return;
    }

    if (bitplane == 3) {
        Uint32 bitplane_color = get_bitplane_color(plane);
        screen_clear(virtscreen, bitplane_color);
    }

    int max_x = screen_get_width();
    int max_y = screen_get_height();
    for (int x = 0; x < max_x; x++) {
        for (int y = 0; y < max_y; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }
}

/******************************************************************************/

/**
 * Clears the screen by setting all pixels to off (0).
 * 
 * @param surface the SDL_Surface to clear
 * @param color the color to write to the surface
 */
void 
screen_clear(SDL_Surface *surface, Uint32 color)
{
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = SCREEN_WIDTH * scale_factor;
    rect.h = SCREEN_HEIGHT * scale_factor;
    SDL_FillRect(surface, &rect, color);
}

/******************************************************************************/

/**
 * Refreshes the screen. If overlay_on is a non-zero value, then the debug 
 * overlay will be turned on and will be painted with the refresh.
 */
void 
screen_refresh(void)
{
    screen_blit_surface(virtscreen, screen, 0, 0);
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
 * @param turn_on TRUE to turn the pixel on, FALSE to turn it off
 * @param plane the bitplane to draw to
 */
void 
draw_pixel(int x, int y, int turn_on, int plane)
{
    if (plane == 0) {
        return;
    }

    int other_bitplane = (plane == 1) ? 2 : 1;
    int other_pixel_on = get_pixel(x, y, other_bitplane);
    Uint32 draw_color = get_bitplane_color(0);
    SDL_Rect pixel;

    if (turn_on && other_pixel_on) {
        draw_color = get_bitplane_color(3);
    }

    if (turn_on && !other_pixel_on) {
        draw_color = get_bitplane_color(plane);
    }

    if (!turn_on && other_pixel_on) {
        draw_color = get_bitplane_color(other_bitplane);
    }

    int mode_scale = screen_get_mode_scale();
    pixel.x = x * scale_factor * mode_scale;
    pixel.y = y * scale_factor * mode_scale;
    pixel.w = scale_factor * mode_scale;
    pixel.h = scale_factor * mode_scale;
    SDL_FillRect(virtscreen, &pixel, draw_color);
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

    tempsurface = SDL_CreateRGBSurface(
        SDL_SWSURFACE | SDL_SRCALPHA, 
        SCREEN_WIDTH * scale_factor, 
        SCREEN_HEIGHT * scale_factor, 
        SCREEN_DEPTH,
        rmask, 
        gmask, 
        bmask, 
        amask
    );
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
        screen_clear(newsurface, COLOR_0);
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
    int width = SCREEN_WIDTH * scale_factor;
    int height = SCREEN_HEIGHT * scale_factor;

    screen = SDL_SetVideoMode(width, height, SCREEN_DEPTH, SDL_SWSURFACE);

    if (screen == NULL) {
        printf("Error: Unable to set video mode: %s\n", SDL_GetError());
        return FALSE;
    } 

    SDL_SetAlpha(screen, SDL_SRCALPHA, 255);
    SDL_WM_SetCaption("YAC8 Emulator", NULL);
    init_colors(screen);
    virtscreen = screen_create_surface(width, height, 255, -1);
    return (virtscreen != NULL);
}

/******************************************************************************/

/**
 * Destroys all the surfaces used by the screen.
 */
void
screen_destroy(void)
{
    SDL_FreeSurface(virtscreen);
    SDL_FreeSurface(screen);
}

/******************************************************************************/

/**
 * Sets extended mode for the screen.
 */
void
screen_set_extended_mode(void) 
{
    screen_mode = SCREEN_MODE_EXTENDED;
}

/******************************************************************************/

/**
 * Disables extended mode for the screen.
 */
void
screen_set_normal_mode(void)
{
    screen_mode = SCREEN_MODE_NORMAL;
}

/******************************************************************************/

/**
 * Scrolls the screen left by 4 pixels.
 */
void
screen_scroll_left(void) 
{
    SDL_Rect source_rect, dest_rect;

    int mode_scale = screen_get_mode_scale();
    int width = screen_get_width() * scale_factor * mode_scale;
    int height = screen_get_height() * scale_factor * mode_scale;
    
    SDL_Surface *tempsurface = screen_create_surface(width, height, 255, -1);

    source_rect.x = 0;
    source_rect.y = 0;
    source_rect.w = width;
    source_rect.h = height;

    dest_rect.x = (-4 * (scale_factor * mode_scale));
    dest_rect.y = 0;
    dest_rect.w = 0;
    dest_rect.h = 0;

    SDL_BlitSurface(virtscreen, &source_rect, tempsurface, &dest_rect);
    SDL_FreeSurface(virtscreen);
    virtscreen = tempsurface;
}

/******************************************************************************/

/**
 * Scrolls the screen right by 4 pixels.
 * 
 * @param plane the bitplane to scroll
 */
void
screen_scroll_right(int plane) 
{
    if (plane == 0) {
        return;
    }

    int mode_scale = screen_get_mode_scale();
    int width = screen_get_width() * scale_factor * mode_scale;
    int height = screen_get_height() * scale_factor * mode_scale;

    if (bitplane == 3) {
        SDL_Rect source_rect, dest_rect;
        int right = scale_factor * 4 * mode_scale;

        SDL_Surface *tempsurface = screen_create_surface(width, height, 255, -1);

        source_rect.x = 0;
        source_rect.y = 0;
        source_rect.w = width - right;
        source_rect.h = height;

        dest_rect.x = right;
        dest_rect.y = 0;
        dest_rect.w = 0;
        dest_rect.h = 0;

        SDL_BlitSurface(virtscreen, &source_rect, tempsurface, &dest_rect);
        SDL_FreeSurface(virtscreen);
        virtscreen = tempsurface;
        return;
    }

    int max_x = screen_get_width();
    int max_y = screen_get_height();

    // Blank out any pixels in the right vertical lines that we will copy to
    for (int x = max_x - 4; x < max_x; x++) {
        for (int y = 0; y < max_y; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }

    // Start copying pixels from the left to the right and shift by 4 pixels
    for (int x = max_x - 4 - 1; x > -1; x--) {
        for (int y = 0; y < max_y; y++) {
            int current_pixel = get_pixel(x, y, plane);
            draw_pixel(x, y, FALSE, bitplane);
            draw_pixel(x + 4, y, current_pixel, plane);
        }
    }

    // Blank out any pixels in the left 4 vertical lines
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < max_y; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }
}

/******************************************************************************/

/**
 * Scrolls the screen down by the specified number of pixels.
 * 
 * @param num_pixels the number of pixels to scroll down by
 * @param plane the bitplane to scroll
 */
void
screen_scroll_down(int num_pixels) 
{
    SDL_Rect source_rect, dest_rect;

    int width = screen_get_width() * scale_factor;
    int height = screen_get_height() * scale_factor;
    int mode_scale = screen_is_extended_mode() ? 1 : 2;
    
    SDL_Surface *tempsurface = screen_create_surface(width, height, 255, -1);

    source_rect.x = 0;
    source_rect.y = 0;
    source_rect.w = width;
    source_rect.h = height;

    dest_rect.x = 0;
    dest_rect.y = (num_pixels * scale_factor * mode_scale);
    dest_rect.w = 0;
    dest_rect.h = 0;

    SDL_BlitSurface(virtscreen, &source_rect, tempsurface, &dest_rect);
    SDL_FreeSurface(virtscreen);
    virtscreen = tempsurface;
}

/******************************************************************************/

/**
 * Returns the height of the screen in pixels. Note does not apply scaling 
 * factor (i.e. returns the logical height of the screen).
 * 
 * @return the height of the screen in pixels
 */
int
screen_get_height(void)
{
    return screen_is_extended_mode() ? 64 : 32;
}

/******************************************************************************/

/**
 * Returns the width of the screen in pixels. Note does not apply scaling 
 * factor (i.e. returns the logical width of the screen).
 * 
 * @return the width of the screen in pixels
 */
int
screen_get_width(void)
{
    return screen_is_extended_mode() ? 128 : 64;
}

/******************************************************************************/

/**
 * Returns the scaling factor that should be applied to the pixel size,
 * depending on the current video mode (normal or extended).
 * 
 * @returns the scale amount to apply to pixel drawing operations
 */
int
screen_get_mode_scale(void)
{
    return screen_is_extended_mode() ? 1 : 2;
}

/* E N D   O F   F I L E ******************************************************/
