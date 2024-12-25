/**
 * Copyright (C) 2012-2019 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      cpu_test.c
 * @brief     Tests for the CPU functions
 * @author    Craig Thomas
 */

/* I N C L U D E S ************************************************************/

#include <CUnit/CUnit.h>
#include "globals.h"

/* F U N C T I O N S **********************************************************/

void 
setup(void) 
{
    CU_TEST_FATAL(memory_init(MEM_SIZE));
    cpu_reset();
}

void 
teardown(void)
{
    memory_destroy();
}

void 
setup_cpu_screen_test(void) 
{
    scale_factor = 1;
    CU_TEST_FATAL(screen_init());
}

void 
teardown_cpu_screen_test(void)
{
    screen_destroy();
}

void 
test_jump_to_address(void) 
{
    setup();
    tword.WORD = 0x1FFF;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    CU_ASSERT_EQUAL(cpu.pc.WORD, address.WORD);
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0xFFF);
    teardown();
}

void
test_jump_to_subroutine(void)
{
    setup();
    tword.WORD = 0x2FFF;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    CU_ASSERT_EQUAL(cpu.pc.WORD, address.WORD);
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0xFFF);
    CU_ASSERT_EQUAL(cpu.sp.WORD, SP_START + 2);
    teardown();
}

void
test_skip_if_register_equal_value(void) 
{
    setup();
    tword.WORD = 0x3152;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x52;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();
}

void
test_skip_if_register_not_equal_value(void) 
{
    setup();
    tword.WORD = 0x4152;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x53;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x52;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0002);
    teardown();
}

void
test_skip_if_register_equal_register(void) 
{
    setup();
    tword.WORD = 0x5120;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xAA;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xBB;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0002);
    teardown();
}

void
test_move_value_to_register(void) 
{
    setup();
    tword.WORD = 0x61AA;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x00);
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0xAA);
    teardown();
}

void
test_add_value_to_register(void) 
{
    setup();
    tword.WORD = 0x71AA;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0xAB);
    teardown();

    setup();
    tword.WORD = 0x71FF;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x00);
    teardown();
}

void
test_load_register_to_register(void) 
{
    setup();
    tword.WORD = 0x8120;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu.v[0x2] = 0x02;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x02);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x02);
    teardown();
}

void
test_logical_or(void) 
{
    setup();
    tword.WORD = 0x8121;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0x55;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0xFF);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x55);
    teardown();
}

void
test_logical_and(void) 
{
    setup();
    tword.WORD = 0x8122;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xFF;
    cpu.v[0x2] = 0x55;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x55);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x55);
    teardown();
}

void
test_logical_xor(void) 
{
    setup();
    tword.WORD = 0x8123;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x09;
    cpu.v[0x2] = 0x0A;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x03);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x0A);
    teardown();
}

void
test_add_register_to_register(void) 
{
    setup();
    tword.WORD = 0x8124;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x10;
    cpu.v[0x2] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x11);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x00);
    teardown();

    /* Overflow condition */
    setup();
    tword.WORD = 0x8124;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xFF;
    cpu.v[0x2] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x00);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();

    /* Overflow condition with remainder */
    setup();
    tword.WORD = 0x8124;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xFF;
    cpu.v[0x2] = 0x02;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x02);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();
}

void
test_sub_register_from_register(void) 
{
    setup();
    tword.WORD = 0x8125;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x10;
    cpu.v[0x2] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x0F);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();

    /* Overflow condition */
    setup();
    tword.WORD = 0x8125;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu.v[0x2] = 0x02;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0xFF);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x02);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x00);
    teardown();
}

void
test_shift_register_right(void) 
{
    setup();
    tword.WORD = 0x8106;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x02;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x00);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x03;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();
}

void
test_sub_register_from_register_source_from_target(void) 
{
    setup();
    tword.WORD = 0x8127;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x10;
    cpu.v[0x2] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0xF1);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu.v[0x2] = 0x02;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x02);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x00);
    teardown();
}

void
test_shift_register_left(void) 
{
    setup();
    tword.WORD = 0x810E;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x02);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x00);
    teardown();

    /* Overflow condition */
    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x80;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x00);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();
}

void
test_skip_if_register_not_equal_register(void) 
{
    setup();
    tword.WORD = 0x9120;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xBB;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xAA;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0002);
    teardown();
}

void
test_load_index(void) 
{
    setup();
    tword.WORD = 0xAFFF;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.i.WORD, 0x0FFF);
    teardown();
}

void
test_jump_index_plus_value(void) 
{
    setup();
    tword.WORD = 0xB010;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.i.WORD = 0x0101;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0111);
    teardown();

    setup();
    tword.WORD = 0xB010;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.i.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x010);
    teardown();
}

void
test_generate_random(void) 
{
    setup();
    tword.WORD = 0xC180;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT(cpu.v[0x1] >= 0x00);
    CU_ASSERT(cpu.v[0x1] <= 0x80);
    teardown();
}

void
test_load_delay_into_target(void)
{
    setup();
    tword.WORD = 0xF107;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.dt = 0xF1;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0xF1);
    teardown(); 
}

void
test_load_source_into_delay(void)
{
    setup();
    tword.WORD = 0xF115;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.dt = 0x00;
    cpu.v[0x1] = 0xAA;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.dt, 0xAA);
    teardown(); 
}

void
test_load_source_into_sound(void)
{
    setup();
    tword.WORD = 0xF118;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.st = 0x00;
    cpu.v[0x1] = 0xAA;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.st, 0xAA);
    teardown(); 
}

void
test_add_index(void)
{
    setup();
    tword.WORD = 0xF11E;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.i.WORD = 0x00;
    cpu.v[0x1] = 0x10;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.i.WORD, 0x10);
    teardown();  
}

void
test_load_sprite_index(void)
{
    setup();
    tword.WORD = 0xF129;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.i.WORD = 0x00;
    cpu.v[0x1] = 0x10;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.i.WORD, 0x50);
    teardown();  
}

void
test_bcd(void)
{
    setup();
    tword.WORD = 0xF133;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.i.WORD = 0x0010;
    cpu.v[0x1] = 0x7B;
    cpu_execute_single();
    CU_ASSERT_EQUAL(memory_read(0x0010), 0x01);
    CU_ASSERT_EQUAL(memory_read(0x0011), 0x02);
    CU_ASSERT_EQUAL(memory_read(0x0012), 0x03);
    teardown();  
}

void
test_store_registers(void)
{
    setup();
    tword.WORD = 0xF055;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.i.WORD = 0x0010;
    cpu.v[0x0] = 0xAA;
    cpu_execute_single();
    CU_ASSERT_EQUAL(memory_read(0x0010), 0xAA);
    teardown();  
}

void
test_load_registers(void)
{
    setup();
    tword.WORD = 0xF165;
    memory_write_word(address, tword);
    address.WORD = 0x0010;
    memory_write(address, 0xAA);
    address.WORD = 0x0011;
    memory_write(address, 0xBB);
    cpu.pc.WORD = 0x0000;
    cpu.i.WORD = 0x0010;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x00], 0xAA);
    CU_ASSERT_EQUAL(cpu.v[0x01], 0xBB);
    teardown();  
}

void
test_store_registers_in_rpl(void)
{
    setup();
    tword.WORD = 0xFF75;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.v[0] = 1;
    cpu.v[1] = 2;
    cpu.v[2] = 3;
    cpu.v[3] = 4;
    cpu.v[4] = 5;
    cpu.v[5] = 6;
    cpu.v[6] = 7;
    cpu.v[7] = 8;
    cpu.v[8] = 9;
    cpu.v[9] = 10;
    cpu.v[10] = 11;
    cpu.v[11] = 12;
    cpu.v[12] = 13;
    cpu.v[13] = 14;
    cpu.v[14] = 15;
    cpu.v[15] = 16;
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.rpl[0], 1);
    CU_ASSERT_EQUAL(cpu.rpl[1], 2);
    CU_ASSERT_EQUAL(cpu.rpl[2], 3);
    CU_ASSERT_EQUAL(cpu.rpl[3], 4);
    CU_ASSERT_EQUAL(cpu.rpl[4], 5);
    CU_ASSERT_EQUAL(cpu.rpl[5], 6);
    CU_ASSERT_EQUAL(cpu.rpl[6], 7);
    CU_ASSERT_EQUAL(cpu.rpl[7], 8);
    CU_ASSERT_EQUAL(cpu.rpl[8], 9);
    CU_ASSERT_EQUAL(cpu.rpl[9], 10);
    CU_ASSERT_EQUAL(cpu.rpl[10], 11);
    CU_ASSERT_EQUAL(cpu.rpl[11], 12);
    CU_ASSERT_EQUAL(cpu.rpl[12], 13);
    CU_ASSERT_EQUAL(cpu.rpl[13], 14);
    CU_ASSERT_EQUAL(cpu.rpl[14], 15);
    CU_ASSERT_EQUAL(cpu.rpl[15], 16);
    teardown();
}

void
test_read_registers_from_rpl(void)
{
    setup();
    tword.WORD = 0xFF85;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.rpl[0] = 1;
    cpu.rpl[1] = 2;
    cpu.rpl[2] = 3;
    cpu.rpl[3] = 4;
    cpu.rpl[4] = 5;
    cpu.rpl[5] = 6;
    cpu.rpl[6] = 7;
    cpu.rpl[7] = 8;
    cpu.rpl[8] = 9;
    cpu.rpl[9] = 10;
    cpu.rpl[10] = 11;
    cpu.rpl[11] = 12;
    cpu.rpl[12] = 13;
    cpu.rpl[13] = 14;
    cpu.rpl[14] = 15;
    cpu.rpl[15] = 16;
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0], 1);
    CU_ASSERT_EQUAL(cpu.v[1], 2);
    CU_ASSERT_EQUAL(cpu.v[2], 3);
    CU_ASSERT_EQUAL(cpu.v[3], 4);
    CU_ASSERT_EQUAL(cpu.v[4], 5);
    CU_ASSERT_EQUAL(cpu.v[5], 6);
    CU_ASSERT_EQUAL(cpu.v[6], 7);
    CU_ASSERT_EQUAL(cpu.v[7], 8);
    CU_ASSERT_EQUAL(cpu.v[8], 9);
    CU_ASSERT_EQUAL(cpu.v[9], 10);
    CU_ASSERT_EQUAL(cpu.v[10], 11);
    CU_ASSERT_EQUAL(cpu.v[11], 12);
    CU_ASSERT_EQUAL(cpu.v[12], 13);
    CU_ASSERT_EQUAL(cpu.v[13], 14);
    CU_ASSERT_EQUAL(cpu.v[14], 15);
    CU_ASSERT_EQUAL(cpu.v[15], 16);
    teardown();
}

void
test_return_from_subroutine(void) 
{
    setup();
    tword.WORD = 0x00EE;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.sp.WORD = 0x000E;
    tword.WORD = 0xB2A1;
    memory_write_word(cpu.sp, tword);
    cpu.sp.WORD = 0x0010;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0xA1B2);
    teardown();
}

void
test_exit_interpreter(void) 
{
    setup();
    tword.WORD = 0x00FD;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.state = CPU_RUNNING;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.state, CPU_STOP);
    teardown();
}

void
test_cpu_scroll_left(void)
{
    setup();
    setup_cpu_screen_test();
    screen_draw(5, 1, 1);
    CU_ASSERT_TRUE(screen_get_pixel(5, 1));
    tword.WORD = 0x00FC;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_FALSE(screen_get_pixel(5, 1));
    CU_ASSERT_TRUE(screen_get_pixel(1, 1));
    teardown();
    teardown_cpu_screen_test();
}

void
test_cpu_scroll_right(void)
{
    setup();
    setup_cpu_screen_test();
    screen_draw(1, 1, 1);
    CU_ASSERT_TRUE(screen_get_pixel(1, 1));
    tword.WORD = 0x00FB;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_FALSE(screen_get_pixel(1, 1));
    CU_ASSERT_TRUE(screen_get_pixel(5, 1));
    teardown();
    teardown_cpu_screen_test();
}

void
test_cpu_scroll_down(void)
{
    setup();
    setup_cpu_screen_test();
    screen_draw(1, 5, 1);
    CU_ASSERT_TRUE(screen_get_pixel(1, 5));
    tword.WORD = 0x00C4;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_FALSE(screen_get_pixel(1, 5));
    CU_ASSERT_TRUE(screen_get_pixel(1, 9));
    teardown();
    teardown_cpu_screen_test();
}

void
test_cpu_screen_blank(void) 
{
    setup();
    setup_cpu_screen_test();
    for (int x = 0; x < screen_get_width(); x++) {
        for (int y = 0; y < screen_get_height(); y++) {
            screen_draw(x, y, 1);
        }
    }

    for (int x = 0; x < screen_get_width(); x++) {
        for (int y = 0; y < screen_get_height(); y++) {
            CU_ASSERT_TRUE(screen_get_pixel(x, y));
        }
    }

    tword.WORD = 0x00E0;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();

    for (int x = 0; x < screen_get_width(); x++) {
        for (int y = 0; y < screen_get_height(); y++) {
            CU_ASSERT_FALSE(screen_get_pixel(x, y));
        }
    }

    teardown();
    teardown_cpu_screen_test();
}

void
test_cpu_enable_extended_mode(void) 
{
    setup();
    setup_cpu_screen_test();

    CU_ASSERT_FALSE(screen_is_extended_mode());

    tword.WORD = 0x00FF;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();

    CU_ASSERT_TRUE(screen_is_extended_mode());

    teardown();
    teardown_cpu_screen_test();
}

void
test_cpu_disable_extended_mode(void) 
{
    setup();
    setup_cpu_screen_test();

    screen_mode = SCREEN_MODE_EXTENDED;
    tword.WORD = 0x00FE;
    address.WORD = 0x0000;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();

    CU_ASSERT_FALSE(screen_is_extended_mode());

    teardown();
    teardown_cpu_screen_test();
}

/* E N D   O F   F I L E ******************************************************/
