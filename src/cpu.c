/**
 * Copyright (C) 2012-2024 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      cpu.c
 * @brief     An emulated Chip 8 CPU
 * @author    Craig Thomas
 */

/* I N C L U D E S ************************************************************/

#include <math.h>
#include <time.h>
#include "globals.h"

/* F U N C T I O N S **********************************************************/

/**
 * This function runs a set of routines based on the SDL timer that is set in
 * `cpu_timerinit`. In this case, the timer will decrement the sound and delay
 * timer registers.
 * 
 * @param interval the number of milliseconds to fire on
 * @param parameters additional parameters to the interrupt routine
 * @returns the new interval at which to fire the timer
 */
Uint32 
cpu_timerinterrupt(Uint32 interval, void *parameters)
{
    decrement_timers = TRUE;
    return interval;
}

/******************************************************************************/

/**
 * Initializes an SDL timer to tick at a rate of 60 times per second. Will call
 * `cpu_timerinterrupt` every time the timer counts down to zero.
 *
 * @returns the newly created SDL timer
 */
int 
cpu_timerinit(void)
{
    int result = TRUE;
    SDL_InitSubSystem(SDL_INIT_TIMER);
    cpu_timer = SDL_AddTimer(17, cpu_timerinterrupt, NULL);

    if (cpu_timer == NULL) {
        printf("Error: could not create timer: %s\n", SDL_GetError());
        result = FALSE;
    }

    return result;
}

/******************************************************************************/

/**
 * Resets the CPU registers.
 */
void 
cpu_reset(void)
{
    /* Reset all general purpose registers */
    cpu.v[0x0] = 0;
    cpu.v[0x1] = 0;
    cpu.v[0x2] = 0;
    cpu.v[0x3] = 0;
    cpu.v[0x4] = 0;
    cpu.v[0x5] = 0;
    cpu.v[0x6] = 0;
    cpu.v[0x7] = 0;
    cpu.v[0x8] = 0;
    cpu.v[0x9] = 0;
    cpu.v[0xA] = 0;
    cpu.v[0xB] = 0;
    cpu.v[0xC] = 0;
    cpu.v[0xD] = 0;
    cpu.v[0xE] = 0;
    cpu.v[0xF] = 0;

    /* Reset all RPL storage values */
    cpu.rpl[0x0] = 0;
    cpu.rpl[0x1] = 0;
    cpu.rpl[0x2] = 0;
    cpu.rpl[0x3] = 0;
    cpu.rpl[0x4] = 0;
    cpu.rpl[0x5] = 0;
    cpu.rpl[0x6] = 0;
    cpu.rpl[0x7] = 0;
    cpu.rpl[0x8] = 0;
    cpu.rpl[0x9] = 0;
    cpu.rpl[0xA] = 0;
    cpu.rpl[0xB] = 0;
    cpu.rpl[0xC] = 0;
    cpu.rpl[0xD] = 0;
    cpu.rpl[0xE] = 0;
    cpu.rpl[0xF] = 0;
 
    /* Reset special registers */
    cpu.i.WORD = 0;
    cpu.sp.WORD = SP_START;
    cpu.dt = 0;
    cpu.st = 0;
    cpu.pc.WORD = CPU_PC_START;
    cpu.oldpc.WORD = CPU_PC_START;
    cpu.operand.WORD = 0;
 
    srand(time(0));
    cpu.state = CPU_PAUSED;

    if (cpu.opdesc != NULL) {
        free(cpu.opdesc);
        cpu.opdesc = NULL;
    }
    cpu.opdesc = (char *)malloc(MAXSTRSIZE);
    awaiting_keypress = FALSE;
    playback_rate = 4000.0;
    pitch = 64;
}

/******************************************************************************/

/**
 * Process any events inside the SDL event queue. Will not block waiting for
 * events. Any event not processed by the emulator will be discarded.
 */
void 
cpu_process_sdl_events(void)
{
    int emulatorkey;
    SDLKey key;

    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                cpu.state = CPU_STOP;
                break;

            case SDL_KEYDOWN:
                key = event.key.keysym.sym;
                if (key == QUIT_KEY) {
                    cpu.state = CPU_STOP;
                } 
                keyboard_processkeydown(key);
                if (awaiting_keypress) {
                    emulatorkey = keyboard_isemulatorkey(key);
                    if (emulatorkey != -1) {
                        int x = cpu.operand.BYTE.high & 0xF;
                        cpu.v[x] = emulatorkey;
                        awaiting_keypress = FALSE;
                    }
                }
                break;

            case SDL_KEYUP:
                keyboard_processkeyup(event.key.keysym.sym);
                break;

            default:
                break;
        }
    }
}

/******************************************************************************/

/**
 * This function executes a single CPU instruction and returns.
 */

void
cpu_execute_single(void) 
{
    cpu.oldpc = cpu.pc;
    cpu.operand.BYTE.high = memory_read(cpu.pc.WORD);
    cpu.pc.WORD++;
    cpu.operand.BYTE.low = memory_read(cpu.pc.WORD);
    cpu.pc.WORD++;

    switch ((cpu.operand.WORD & 0xF000) >> 12) {
        case 0x0:
            switch (cpu.operand.WORD & 0xFF) {
                case 0xE0:
                    clear_screen();
                    break;

                case 0xEE:
                    return_from_subroutine();
                    break;

                case 0xFB:
                    scroll_right();
                    break;

                case 0xFC:
                    scroll_left();
                    break;

                case 0xFD:
                    exit_interpreter();
                    break;

                case 0xFE:
                    disable_extended_mode();
                    break;

                case 0xFF:
                    enable_extended_mode();
                    break;

                default:
                    switch (cpu.operand.WORD & 0xF0) {
                        case 0xC0:
                            scroll_down();
                            break;

                        default:
                            break;
                    }
                    break;
            } 
            break;

        case 0x1:
            jump_to_address();
            break;

        case 0x2:
            jump_to_subroutine();
            break;

        case 0x3:
            skip_if_register_equal_value();
            break;

        case 0x4:
            skip_if_register_not_equal_value();
            break;

        case 0x5:
            skip_if_register_equal_register();
            break;

        case 0x6:
            move_value_to_register();
            break;

        case 0x7:
            add_value_to_register();
            break;

        case 0x8:
            switch (cpu.operand.WORD & 0xF) {
                case 0x0: 
                    move_register_into_register();
                    break;

                case 0x1:
                    logical_or();
                    break;

                case 0x2:
                    logical_and();
                    break;

                case 0x3:
                    exclusive_or();
                    break;
                
                case 0x4:
                    add_register_to_register();
                    break;

                case 0x5:
                    subtract_register_from_register();
                    break;

                case 0x6:
                    shift_right();
                    break;

                case 0x7:
                    subtract_register_from_register_borrow();
                    break;

                case 0xE:
                    shift_left();
                    break;

                default:
                    break;
            }
            break;

        case 0x9:
            skip_if_register_not_equal_register();
            break;

        case 0xA:
            load_index_with_value();
            break;

        case 0xB:
            jump_to_register_plus_value();
            break;

        case 0xC:
            generate_random_number();
            break;
    
        case 0xD:
            draw_sprite();
            break;

        case 0xE:
            switch (cpu.operand.WORD & 0xFF) {
                case 0x9E:
                    skip_if_key_pressed();
                    break;

                case 0xA1:
                    skip_if_key_not_pressed();
                    break;

                default:
                    break;
            }
            break;

        case 0xF:
            switch (cpu.operand.WORD & 0xFF) {
                case 0x07:
                    move_delay_timer_into_register();
                    break;

                case 0x0A:
                    wait_for_keypress();
                    break;     

                case 0x15:
                    move_register_into_delay();
                    break;             

                case 0x18:
                    move_register_into_sound();
                    break;

                case 0x1E:
                    add_register_to_index();
                    break;

                case 0x29:
                    load_index_with_sprite();
                    break;

                case 0x33:
                    store_bcd_in_memory();
                    break;

                case 0x3A:
                    load_pitch();
                    break;

                case 0x55:
                    store_registers_in_memory();
                    break;

                case 0x65:
                    load_registers_from_memory();
                    break;

                case 0x75:
                    store_registers_in_rpl();
                    break;

                case 0x85:
                    read_registers_from_rpl();
                    break;
                
                default:
                    break;
            }
            break;

        default:
            break; 
    }
}

/******************************************************************************/

/**
 * 00Cn - SCRD n
 * 
 * Scrolls the screen down n pixels.
 */
void
scroll_down(void)
{
    int x = cpu.operand.WORD & 0xF;
    screen_scroll_down(x);
    sprintf(cpu.opdesc, "SCRD %d", x);
}

/******************************************************************************/

/**
 * 00E0 - CLS
 * 
 * Clear screen
 */
void
clear_screen(void)
{
    screen_blank();
    sprintf(cpu.opdesc, "CLS");
}

/******************************************************************************/

/**
 * 00EE - RTS
 * 
 * Return from subroutine. Pop the current value in the stack off of the 
 * stack, and set the program counter to the value popped.
 */
void
return_from_subroutine(void)
{
    cpu.sp.WORD--;
    cpu.pc.BYTE.high = memory_read(cpu.sp.WORD);
    cpu.sp.WORD--;
    cpu.pc.BYTE.low = memory_read(cpu.sp.WORD);
    sprintf(cpu.opdesc, "RTS");
}

/******************************************************************************/

/**
 * 00FB - SCRR
 * 
 * Scroll the screen right by 4 pixels.
 */
void
scroll_right(void)
{
    screen_scroll_right();
    sprintf(cpu.opdesc, "SCRR");
}

/******************************************************************************/

/**
 * 00FC - SCRL
 * 
 * Scroll the screen left by 4 pixels.
 */
void
scroll_left(void)
{
    screen_scroll_left();
    sprintf(cpu.opdesc, "SCRL");
}

/******************************************************************************/

/**
 * 00FD - EXIT
 *
 * Exit interpreter.
 */
void
exit_interpreter(void)
{
    cpu.state = CPU_STOP;
    sprintf(cpu.opdesc, "EXIT");
}

/******************************************************************************/

/**
 * 00FE - EXTD
 * 
 * Disable extended mode
 */
void
disable_extended_mode(void)
{
    screen_set_normal_mode();
    sprintf(cpu.opdesc, "EXTD");
}

/******************************************************************************/

/**
 * 00FF - EXTE
 * Enable extended mode
 */
void
enable_extended_mode(void)
{
    screen_set_extended_mode();
    sprintf(cpu.opdesc, "EXTE");
}

/******************************************************************************/

/**
 * 1nnn - JUMP nnn 
 * 
 * Jump to address.
 */
void
jump_to_address(void)
{
    cpu.pc.WORD = (cpu.operand.WORD & 0x0FFF);
    sprintf(cpu.opdesc, "JUMP %03X", cpu.pc.WORD);
}

/******************************************************************************/

/**
 * 2nnn - CALL nnn
 * 
 * Jump to subroutine. Save the current program counter on the stack.
 */
void
jump_to_subroutine(void)
{
    memory_write(cpu.sp, cpu.pc.WORD & 0x00FF);
    cpu.sp.WORD++;
    memory_write(cpu.sp, (cpu.pc.WORD & 0xFF00) >> 8);
    cpu.sp.WORD++;
    cpu.pc.WORD = (cpu.operand.WORD & 0x0FFF);
    sprintf(cpu.opdesc, "CALL %03X", cpu.pc.WORD);
}

/******************************************************************************/

/**
 * 3xnn - SKE Vx, nn
 * 
 * Skip if source register equal constant value. The program counter
 * is updated to skip the next instruction by advancing it by 2 bytes.
 */ 
void
skip_if_register_equal_value(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    if (cpu.v[x] == (cpu.operand.WORD & 0x00FF)) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKE V%X, %02X", x, cpu.operand.BYTE.low);
}

/******************************************************************************/

/**
 * 4xnn - SKNE Vx, nn
 * 
 * Skip if source register contents not equal constant value. The program
 * counter is updated to skip the next instruction by advancing it by 2 bytes.
 */
void
skip_if_register_not_equal_value(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    if (cpu.v[x] != (cpu.operand.WORD & 0x00FF)) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKNE V%X, %02X", x, cpu.operand.BYTE.low);
}

/******************************************************************************/

/**
 * 5xy0 - SKE Vx, Vy
 * 
 * Skip if source register value is equal to target register. The program 
 * counter is updated to skip the next instruction by advancing it by 2 bytes.
 */
void 
skip_if_register_equal_register(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    if (cpu.v[x] == cpu.v[y]) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKE V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 6xnn - LOAD Vx, nn
 * 
 * Move the constant value into the specified register.
 */
void
move_value_to_register(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.v[x] = cpu.operand.WORD & 0x00FF;
    sprintf(cpu.opdesc, "LOAD V%X, %02X", x, (cpu.operand.WORD & 0x00FF));
}
            
/******************************************************************************/

/**
 * 7xnn - ADD Vx, nn
 * 
 * Add the constant value to the source register.
 */
void
add_value_to_register(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.v[x] = cpu.v[x] + ((cpu.operand.WORD & 0x00FF) % 256);
    sprintf(cpu.opdesc, "ADD V%X, %02X", x, cpu.operand.BYTE.low);
}

/******************************************************************************/

/**
 * 8xy0 - LOAD Vx, Vy
 *
 * Move the y register into the x register. 
 */
void
move_register_into_register(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] = cpu.v[y];
    sprintf(cpu.opdesc, "LOAD V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xy1 - OR Vx, Vy
 * 
 * Perform a logical OR operation between x and y and store the result
 * in x.
 */
void
logical_or(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] |= cpu.v[y];
    sprintf(cpu.opdesc, "OR V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xy2 - AND Vx, Vy
 * 
 * Perform a logical AND between x and y and store the result in x.
 */
void
logical_and(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] &= cpu.v[y];
    sprintf(cpu.opdesc, "AND V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xy3 - XOR  Vx, Vy
 * 
 * Perform a logical XOR between x and y and store the result in x.
 */
void
exclusive_or(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] ^= cpu.v[y];
    sprintf(cpu.opdesc, "XOR V%X, V%X", x, y);
}

/******************************************************************************/

/* 
 * 8xy4 - ADD  Vx, Vy
 *
 * Add the value in the source register to the value in the target register,
 * and store the result in the target register. If a carry is generated, set
 * a carry flag in register VF.
 */
void
add_register_to_register(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    int carry = (cpu.v[x] + cpu.v[y]) > 255 ? 1 : 0;
    cpu.v[x] = (cpu.v[x] + cpu.v[y]) % 256;
    cpu.v[0xF] = carry;
    sprintf(cpu.opdesc, "ADD V%X, V%X", x, y);
}

/******************************************************************************/

/* 
 * 8xy5 - SUB  Vx, Vy
 *
 * Subtract the value in the target register from the value in the source
 * register, and store the result in the target register. If a borrow is NOT
 * generated, set a carry flag in register VF.
 */
void
subtract_register_from_register(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    int borrow = (cpu.v[x] >= cpu.v[y]) ? 1 : 0;
    cpu.v[x] = (cpu.v[x] >= cpu.v[y]) ? (cpu.v[x] - cpu.v[y]) : 256 + (cpu.v[x] - cpu.v[y]);
    cpu.v[0xF] = borrow;
    sprintf(cpu.opdesc, "SUB V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xy6 - SHR Vx, Vy
 * 
 * Shift the bits in the specified register 1 bit to the right. Bit 0 will
 * be shifted into register VF.
 */
void
shift_right(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    int bit_one = cpu.v[y] & 0x1;
    cpu.v[x] = cpu.v[y] >> 1;
    cpu.v[0xF] = bit_one;
    sprintf(cpu.opdesc, "SHR V%X", x);
}
    
/******************************************************************************/

/**
 * 8xy7 - SUBN Vx, Vy
 * 
 * Subtract the value in the target register from the value in the source
 * register, and store the result in the target register. If a borrow is NOT
 * generated, set a carry flag in register VF.
 */
void
subtract_register_from_register_borrow(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    int not_borrow = (cpu.v[y] >= cpu.v[x]) ? 1 : 0;
    cpu.v[x] = (cpu.v[y] >= cpu.v[x]) ? cpu.v[y] - cpu.v[x] : 256 + cpu.v[y] - cpu.v[x];
    cpu.v[0xF] = not_borrow;
    sprintf(cpu.opdesc, "SUBN V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xyE - SHL Vx, Vy
 * 
 * Shift the bits in the specified register 1 bit to the left. Bit 7 will be
 * shifted into register VF.
 */
void
shift_left(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    int bit_seven = (cpu.v[y] & 0x80) >> 7;
    cpu.v[x] = (cpu.v[y] << 1) & 0xFF;
    cpu.v[0xF] = bit_seven;
    sprintf(cpu.opdesc, "SHL V%X", x);
}

/******************************************************************************/

/**
 * 9xy0 - SKNE Vx, Vy
 * 
 * Skip if source register is equal to target register. The program counter
 * is updated to skip the next instruction by advancing it by 2 bytes.
 */
void
skip_if_register_not_equal_register(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    if (cpu.v[x] != cpu.v[y]) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKNE V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * Annn - LOAD I, nnn
 * 
 * Load index register with constant value.
 */
void
load_index_with_value(void) 
{
    cpu.i.WORD = (cpu.operand.WORD & 0x0FFF);
    sprintf(cpu.opdesc, "LOAD I, %03X", (cpu.operand.WORD & 0x0FFF));
}

/******************************************************************************/

/**
 * 
 * Bnnn - JUMP V0 + nnn
 * 
 * Load the program counter with the memory value located at the specified
 * operand plus the value of the register.
 */
void
jump_to_register_plus_value(void)
{
    cpu.pc.WORD = (cpu.v[0] & 0xFF) + (cpu.operand.WORD & 0x0FFF);
    sprintf(cpu.opdesc, "JUMP V0 + %03X", (cpu.operand.WORD & 0x0FFF));
}

/******************************************************************************/

/**
 * Cxnn - RAND Vx, nn
 * 
 * A random number between 0 and 255 is generated. The contents
 * of it are then ANDed with the constant value passed in the
 * operand. The result is stored in the target register.
 */
void
generate_random_number(void)
{
    int x = cpu.operand.BYTE.high & 0xF;
    cpu.v[x] = (rand() % 255) & cpu.operand.BYTE.low;
    sprintf(cpu.opdesc, "RAND V%X, %02X", x, (cpu.operand.BYTE.low));
}

/******************************************************************************/

/**
 * Dxyn - DRAW x, y, n_bytes
 * 
 * Draws the sprite pointed to in the index register at the       
 * specified x and y coordinates. Drawing is done via an XOR      
 * routine, meaning that if the target pixel is already turned    
 * on, and a pixel is set to be turned on at that same location   
 * via the draw, then the pixel is turned off. The routine will  
 * wrap the pixels if they are drawn off the edge of the screen.  
 * Each sprite is 8 bits (1 byte) wide. The n_bytes parameter     
 * sets how tall the sprite is. Consecutive bytes in the memory   
 * pointed to by the index register make up the bytes of the      
 * sprite. Each bit in the sprite byte determines whether a pixel 
 * is turned on (1) or turned off (0). For example, assume that   
 * the index register pointed to the following 7 bytes:           
 *                                                                
 *                 bit 0 1 2 3 4 5 6 7                            
 *                                                                
 *     byte 0          0 1 1 1 1 1 0 0                            
 *     byte 1          0 1 0 0 0 0 0 0                            
 *     byte 2          0 1 0 0 0 0 0 0                            
 *     byte 3          0 1 1 1 1 1 0 0                            
 *     byte 4          0 1 0 0 0 0 0 0                            
 *     byte 5          0 1 0 0 0 0 0 0                            
 *     byte 6          0 1 1 1 1 1 0 0                            
 *                                                               
 * This would draw a character on the screen that looks like an   
 * 'E'. The x_source and y_source tell which registers contain    
 * the x and y coordinates for the sprite. If writing a pixel to  
 * a location causes that pixel to be turned off, then VF will be 
 * set to 1.                                                      
 */
void
draw_sprite(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    byte tbyte = cpu.operand.BYTE.low & 0xF;
    cpu.v[0xF] = 0;

    if (screen_is_extended_mode() && tbyte == 0) {
        for (int i = 0; i < 16; i++) {
            for (int k = 0; k < 2; k++) {
                tbyte = memory_read(cpu.i.WORD + (i * 2) + k);
                int ycor = cpu.v[y] + i;
                ycor = ycor % screen_get_height();

                for (int j = 0; j < 8; j++) {
                    int xcor = cpu.v[x] + j + (k * 8);
                    xcor = xcor % screen_get_width();

                    int color = (tbyte & 0x80) ? 1 : 0;
                    int currentcolor = screen_get_pixel(xcor, ycor);

                    cpu.v[0xF] = (currentcolor && color) ? 1 : cpu.v[0xF];
                    color = color ^ currentcolor;

                    screen_draw(xcor, ycor, color);
                    tbyte = tbyte << 1;
                } 
            }
        }
        sprintf(cpu.opdesc, "DRAWEX V%X, V%X, %X", x, y, (cpu.operand.WORD & 0xF));
    } else {
        for (int i = 0; i < (cpu.operand.BYTE.low & 0xF); i++) {
            tbyte = memory_read (cpu.i.WORD + i);
            int ycor = cpu.v[y] + i;
            ycor = ycor % screen_get_height();

            for (int j = 0; j < 8; j++) {
                int xcor = cpu.v[x] + j;
                xcor = xcor % screen_get_width();

                int color = (tbyte & 0x80) ? 1 : 0;
                int currentcolor = screen_get_pixel(xcor, ycor);

                cpu.v[0xF] = (currentcolor && color) ? 1 : cpu.v[0xF];
                color = color ^ currentcolor;

                screen_draw(xcor, ycor, color);
                tbyte = tbyte << 1;
            } 
        }
        sprintf(cpu.opdesc, "DRAW V%X, V%X, %X", x, y, (cpu.operand.WORD & 0xF));
    }

    screen_refresh(FALSE);
}

/******************************************************************************/

/**
 * Ex9E - SKPR Vx
 * 
 * Check to see if the key specified in the source        
 * register is pressed, and if it is, skips the next      
 * instruction.
 */
void
skip_if_key_pressed(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    if (keyboard_checkforkeypress(cpu.v[x])) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKPR V%X", x);
}

/******************************************************************************/

/**
 * ExA1 - SKUP Vx
 * 
 * Check for the specified keypress in the source         
 * register and if it is NOT pressed, will skip the next  
 * instruction                                           
 */
void
skip_if_key_not_pressed(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    if (!keyboard_checkforkeypress(cpu.v[x])) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKUP V%X", x);
}

/******************************************************************************/

/**
 * Fx07 - LOAD Vx, DELAY 
 * 
 * Move the value of the delay timer into the target      
 * register.                                              
 */
void
move_delay_timer_into_register(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.v[x] = cpu.dt; 
    sprintf(cpu.opdesc, "LOAD V%X, DELAY", x);
}

/******************************************************************************/

/**
 * Fx0A - KEYD Vx
 * 
 * Stop execution until a key is pressed. Move the value 
 * of the key pressed into the specified register.
 */
void
wait_for_keypress(void)
{
    awaiting_keypress = TRUE;
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    sprintf(cpu.opdesc, "KEYD V%X", x);
}

/******************************************************************************/

/**
 * Fx15 - LOAD DELAY, Vx
 * 
 * Move the value stored in the specified source register
 * into the delay timer.
 */
void
move_register_into_delay(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.dt = cpu.v[x];
    sprintf(cpu.opdesc, "LOAD DELAY, V%X", x);
}

/******************************************************************************/

/**
 * Fx18 - LOAD SOUND, Vx
 * 
 * Move the value stored in the specified source register
 * into the sound timer.
 */                                
void
move_register_into_sound(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.st = cpu.v[x];
    sprintf(cpu.opdesc, "LOAD SOUND, V%X", x);
}

/******************************************************************************/

/**
 * Fx1E - ADD  I, Vx
 * 
 * Add the value of the source register into the index
 * register.
 */
void
add_register_to_index(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.i.WORD += cpu.v[x];
    sprintf(cpu.opdesc, "ADD I, V%X", x);
}

/******************************************************************************/

/**
 * Fx29 - LOAD I, Vx
 * 
 * Load the index with the sprite indicated in the source
 * register. All sprites are 5 bytes long, so the        
 * location of the specified sprite is its index         
 * multiplied by 5. Font sprites start at memory index 0.
 */
void
load_index_with_sprite(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.i.WORD = cpu.v[x] * 5;
    sprintf(cpu.opdesc, "LOAD I, V%X", x);
}

/******************************************************************************/

/**
 * Fx33 - BCD
 * 
 * Take the value stored in source and place the digits   
 * in the following locations:                            
 *                                                        
 *      hundreds   -> self.memory[index]                  
 *      tens       -> self.memory[index + 1]              
 *      ones       -> self.memory[index + 2]              
 *                                                        
 * For example, if the value is 123, then the following   
 * values will be placed at the specified locations:      
 *                                                        
 *      1 -> self.memory[index]                           
 *      2 -> self.memory[index + 1]                       
 *      3 -> self.memory[index + 2]                       
 */
void
store_bcd_in_memory(void)
{
    word tword;

    int x = (cpu.operand.WORD & 0x0F00) >> 8;

    tword.WORD = cpu.i.WORD;
    int i = cpu.v[x] / 100;
    memory_write(tword, i);

    tword.WORD++;
    i = (cpu.v[x] % 100) / 10;
    memory_write(tword, i);

    tword.WORD++;
    i = (cpu.v[x] % 100) % 10;
    memory_write(tword, i);
    sprintf(cpu.opdesc, "BCD V%X (%03d)", x, cpu.v[x]);
}

/******************************************************************************/

/**
 * Fx3A - Pitch Vx
 * 
 * Loads the value from register x into the pitch register.
 */
void
load_pitch(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    pitch = cpu.v[x];
    playback_rate = 4000.0 * pow(2.0, (((float) pitch - 64.0) / 48.0));
    sprintf(cpu.opdesc, "PITCH V%X (%X)", x, cpu.v[x]);
}

/******************************************************************************/

/**
 * Fn55 - STOR n
 * 
 * Store all of the n registers in the memory pointed to  
 * by the index register. For example, to store all of    
 * the V registers, n would be the value 'F'              
 */
void
store_registers_in_memory(void)
{
    word tword;

    int n = (cpu.operand.WORD & 0x0F00) >> 8;
    for (int i = 0; i <= n; i++) {
        tword.WORD = cpu.i.WORD + i;
        memory_write(tword, cpu.v[i]);
    }
    cpu.i.WORD += n + 1;
    sprintf(cpu.opdesc, "STOR %X", n);
}

/******************************************************************************/

/**
 * Fn65 - LOAD n
 * 
 * Read all of the V registers from the memory pointed to 
 * by the index register. For example, to load all of the 
 * V registers, n would be 'F'.
 */
void
load_registers_from_memory(void)
{
    int n = (cpu.operand.WORD & 0x0F00) >> 8;
    for (int i = 0; i <= n; i++) {
        cpu.v[i] = memory_read(cpu.i.WORD + i);
    }
    cpu.i.WORD += n + 1;
    sprintf(cpu.opdesc, "LOAD %X", n);
}

/******************************************************************************/

/**
 * Fn75 - SRPL n
 * 
 * Stores the values from n number of registers        
 * (starting from 0) to the RPL store.
 */
void
store_registers_in_rpl(void)
{
    int n = (cpu.operand.WORD & 0x0F00) >> 8;
    for (int i = 0; i <= n; i++) {
        cpu.rpl[i] = cpu.v[i];
    }
    sprintf(cpu.opdesc, "SRPL %X", n);
}

/******************************************************************************/

/**
 * Fn85 - LRPL n
 * 
 * Loads n number of registers from the RPL store to      
 * their respective registers.
 */
void
read_registers_from_rpl(void)
{
    int n = (cpu.operand.WORD & 0x0F00) >> 8;
    for (int i = 0; i <= n; i++) {
        cpu.v[i] = cpu.rpl[i];
    }
    sprintf(cpu.opdesc, "LRPL %X", n);
}

/******************************************************************************/

/**
 * This function contains the main CPU execution loop. It is responsible for 
 * checking the SDL event structure for input events. It then fetches and 
 * decodes the next instruction, executes it and restarts the loop. This 
 * process continues until the `cpu.state` flag is set to `CPU_STOP`. It also
 * will decrement timers when the `decrement_timers` flag is set to `TRUE`.
 */
void 
cpu_execute(void)
{
    while (cpu.state != CPU_STOP) {
        if (awaiting_keypress != 1) {
            cpu_execute_single();
            if (decrement_timers) {
                cpu.dt -= (cpu.dt > 0) ? 1 : 0;
                cpu.st -= (cpu.st > 0) ? 1 : 0;
                decrement_timers = FALSE;
            }
        }
        cpu_process_sdl_events();
    }
}

/* E N D   O F   F I L E ******************************************************/
