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

#include <SDL2/SDL.h>
#include <stdlib.h>
#include "globals.h"

/* F U N C T I O N S **********************************************************/

SDL_Surface *
create_surface(int width, int height)
{
    SDL_Surface *temp_surface = SDL_CreateRGBSurfaceWithFormat(
        0, 
        width, 
        height, 
        SCREEN_DEPTH,
        SDL_PIXELFORMAT_ARGB8888
    );

    if (temp_surface == NULL) {
        printf("Error: Unable to initialize surface:\n%s\n", SDL_GetError());
    }

    SDL_SetSurfaceBlendMode(temp_surface, SDL_BLENDMODE_NONE);

    return temp_surface;
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

    window = SDL_CreateWindow(
        "YAC8 Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL
    );
    
    if (window == NULL) {
        printf("Error: Unable to initialize window:\n%s\n", SDL_GetError());
        return FALSE;
    } 

    renderer = SDL_CreateRenderer(
        window, 
        -1,
        SDL_RENDERER_ACCELERATED 
    );

    if (renderer == NULL) {
        printf("Error: Unable to initialize renderer:\n%s\n", SDL_GetError());
        return FALSE;
    }
    
    surface = create_surface(width, height);

    if (surface == NULL) {
        return FALSE;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );

    if (texture == NULL) {
        printf("Error: Unable to initialize texture:\n%s\n", SDL_GetError());
        return FALSE;
    }

    COLOR_0 = SDL_MapRGBA(surface->format, 0,    0,    0, 0);
    COLOR_1 = SDL_MapRGBA(surface->format, 250, 51,  204, 255);
    COLOR_2 = SDL_MapRGBA(surface->format, 51,  204, 250, 0);
    COLOR_3 = SDL_MapRGBA(surface->format, 250, 250, 250, 0);

    return TRUE;
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

    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 pixel = pixels[(surface->w * y) + x];
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    color = SDL_MapRGB(surface->format, r, g, b);
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
        screen_clear(bitplane_color);
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
 * @param color the color to write to the surface
 */
void 
screen_clear(Uint32 color)
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
 * Refreshes the screen. 
 */
void 
screen_refresh(void)
{
    SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
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
    SDL_FillRect(surface, &pixel, draw_color);
}

/******************************************************************************/

/**
 * Destroys all the surfaces used by the screen.
 */
void
screen_destroy(void)
{
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
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
 * 
 * @param plane the bitplane to scroll
 */
void
screen_scroll_left(int plane) 
{
    if (plane == 0) {
        return;
    }

    int mode_scale = screen_get_mode_scale();
    int width = screen_get_width() * scale_factor * mode_scale;
    int height = screen_get_height() * scale_factor * mode_scale;

    int max_x = screen_get_width();
    int max_y = screen_get_height();
    if (plane == 3) {    
        SDL_Rect source_rect, dest_rect;

        SDL_Surface *temp_surface = create_surface(width, height);

        source_rect.x = 4 * (scale_factor * mode_scale);
        source_rect.y = 0;
        source_rect.w = width - (4 * (scale_factor * mode_scale));
        source_rect.h = height;

        dest_rect.x = 0;
        dest_rect.y = 0;
        dest_rect.w = 0;
        dest_rect.h = 0;

        SDL_BlitSurface(surface, &source_rect, temp_surface, &dest_rect);
        SDL_FreeSurface(surface);
        surface = temp_surface;
        return;
    }

    // Blank out any pixels in the left 4 vertical lines we will copy to
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < max_y; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }

    // Start copying pixels from the right to the left and shift by 4 pixels
    for (int x = 4; x < max_x; x++) {
        for (int y = 0; y < max_y; y++) {
            int current_pixel = get_pixel(x, y, plane);
            draw_pixel(x, y, FALSE, plane);
            draw_pixel(x - 4, y, current_pixel, plane);
        }
    }

    // Blank out any pixels in the right 4 vertical columns
    for (int x = max_x - 4; x < max_x; x++) {
        for (int y = 0; y < max_y; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }
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

        SDL_Surface *temp_surface = create_surface(width, height);

        source_rect.x = 0;
        source_rect.y = 0;
        source_rect.w = width - right;
        source_rect.h = height;

        dest_rect.x = right;
        dest_rect.y = 0;
        dest_rect.w = 0;
        dest_rect.h = 0;

        SDL_BlitSurface(surface, &source_rect, temp_surface, &dest_rect);
        SDL_FreeSurface(surface);
        surface = temp_surface;
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
screen_scroll_down(int num_pixels, int plane) 
{
    if (plane == 0) {
        return;
    }

    int width = screen_get_width() * scale_factor;
    int height = screen_get_height() * scale_factor;
    int mode_scale = screen_is_extended_mode() ? 1 : 2;

    if (plane == 3) {    
        SDL_Surface *temp_surface = create_surface(width, height);
        SDL_Rect source_rect, dest_rect;

        source_rect.x = 0;
        source_rect.y = 0;
        source_rect.w = width;
        source_rect.h = height;

        dest_rect.x = 0;
        dest_rect.y = (num_pixels * scale_factor * mode_scale);
        dest_rect.w = 0;
        dest_rect.h = 0;

        SDL_BlitSurface(surface, &source_rect, temp_surface, &dest_rect);
        SDL_FreeSurface(surface);
        surface = temp_surface;
        return;
    }

    int max_x = width;
    int max_y = height;

    // Blank out any pixels in the bottom num_pixels that we will copy to
    for (int x = 0; x < max_x; x++) {
        for (int y = max_y - num_pixels; y < max_y; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }

    // Start copying pixels from the top to the bottom and shift by num_pixels
    for (int x = 0; x < max_x; x++) {
        for (int y = max_y - num_pixels - 1; y > -1; y--) {
            int current_pixel = get_pixel(x, y, plane);
            draw_pixel(x, y, FALSE, plane);
            draw_pixel(x, y + num_pixels, current_pixel, plane);
        }
    }

    // Blank out any pixels in the first num_pixels horizontal lines
    for (int x = 0; x < max_x; x++) {
        for (int y = 0; y < num_pixels; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }
}

/******************************************************************************/

/**
 * Scrolls the screen up the specified number of pixels.
 * 
 * @param num_pixels the number of pixels to scroll up
 * @param plane the bitplane to scroll
 */
void
screen_scroll_up(int num_pixels, int plane) 
{
    if (plane == 0) {
        return;
    }

    int width = screen_get_width() * scale_factor;
    int height = screen_get_height() * scale_factor;
    int mode_scale = screen_is_extended_mode() ? 1 : 2;

    if (plane == 3) {    
        SDL_Surface *temp_surface = create_surface(width, height);
        SDL_Rect source_rect, dest_rect;

        source_rect.x = 0;
        source_rect.y = (num_pixels * scale_factor * mode_scale);
        source_rect.w = width;
        source_rect.h = height;

        dest_rect.x = 0;
        dest_rect.y = 0;
        dest_rect.w = 0;
        dest_rect.h = 0;

        SDL_BlitSurface(surface, &source_rect, temp_surface, &dest_rect);
        SDL_FreeSurface(surface);
        surface = temp_surface;
        return;
    }

    int max_x = screen_get_width();
    int max_y = screen_get_height();

    // Blank out any pixels in the top numPixels that we will copy to
    for (int x = 0; x < max_x; x++) {
        for (int y = 0; y < num_pixels; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }

    // Start copying pixels from the top to the bottom and shift up by numPixels
    for (int x = 0; x < max_x; x++) {
        for (int y = num_pixels; y < max_y; y++) {
            int currentPixel = get_pixel(x, y, plane);
            draw_pixel(x, y, FALSE, plane);
            draw_pixel(x, y - num_pixels, currentPixel, plane);
        }
    }

    // Blank out any piels in the bottom numPixels
    for (int x = 0; x < max_x; x++) {
        for (int y = max_y - num_pixels; y < max_y; y++) {
            draw_pixel(x, y, FALSE, plane);
        }
    }    
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
