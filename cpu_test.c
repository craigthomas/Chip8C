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

/* L O C A L S ****************************************************************/

word tword;
word address;

/* F U N C T I O N S **********************************************************/

void 
setup(void) 
{
    CU_TEST_FATAL(memory_init(MEM_4K));
    cpu_reset();
}

void 
teardown(void)
{
    memory_destroy();
}

void 
test_jump_to_address(void) 
{
    setup();
    address.WORD = 0x000;
    tword.WORD = 0x1FFF;
    memory_write_word(address, tword);
    cpu.pc = address;
    CU_ASSERT_EQUAL(cpu.pc.WORD, address.WORD);
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0xFFF);
    teardown();
}

void
test_jump_to_subroutine(void)
{
    setup();
    address.WORD = 0x000;
    tword.WORD = 0x2FFF;
    memory_write_word(address, tword);
    cpu.pc = address;
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
    address.WORD = 0x0000;
    tword.WORD = 0x3152;
    memory_write_word(address, tword);
    cpu.pc = address;
    cpu.v[0x1] = 0x52;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();
}

void
test_skip_if_register_not_equal_value(void) 
{
    setup();
    address.WORD = 0x0000;
    tword.WORD = 0x4152;
    memory_write_word(address, tword);
    cpu.pc = address;
    cpu.v[0x1] = 0x53;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc = address;
    cpu.v[0x1] = 0x52;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0002);
    teardown();
}

void
test_skip_if_register_equal_register(void) 
{
    setup();
    address.WORD = 0x0000;
    tword.WORD = 0x5120;
    memory_write_word(address, tword);
    cpu.pc = address;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xAA;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc = address;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xBB;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0002);
    teardown();
}

void
test_skip_if_register_not_equal_register(void) 
{
    setup();
    address.WORD = 0x0000;
    tword.WORD = 0x9120;
    memory_write_word(address, tword);
    cpu.pc = address;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xBB;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0004);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc = address;
    cpu.v[0x1] = 0xAA;
    cpu.v[0x2] = 0xAA;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0002);
    teardown();
}

void
test_move_value_to_register(void) 
{
    setup();
    address.WORD = 0x0000;
    tword.WORD = 0x61AA;
    memory_write_word(address, tword);
    cpu.pc = address;
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x00);
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0xAA);
    teardown();
}

/* E N D   O F   F I L E ******************************************************/
