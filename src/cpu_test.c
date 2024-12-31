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
    for (int a = 0x0000; a < 0xFFFF; a += 0x10) {
        cpu.operand.WORD = a;
        cpu.pc.WORD = 0x0000;
        jump_to_address();
        CU_ASSERT_EQUAL(a & 0x0FFF, cpu.pc.WORD);
    }
}

void 
test_jump_to_address_integration(void) 
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
test_jump_to_subroutine_integration(void)
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
test_jump_to_subroutine(void)
{
    setup();
    for (int a = 0x0200; a < 0xFFFF; a++) {
        cpu.operand.WORD = a & 0xFFFF;
        cpu.sp.WORD = 0;
        cpu.pc.WORD = 0x100;
        jump_to_subroutine();
        CU_ASSERT_EQUAL((a & 0x0FFF), cpu.pc.WORD);
        CU_ASSERT_EQUAL(2, cpu.sp.WORD);
    }
}

void
test_skip_if_register_equal_value_integration(void) 
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
test_skip_if_register_equal_value(void) 
{
    setup();
    for (int r = 0; r < 0x10; r++) {
        for (int value = 0; value < 0xFF; value += 10) {
            for (int reg_value = 0; reg_value < 0xFF; reg_value++) {
                cpu.operand.WORD = r << 8;
                cpu.operand.WORD += value;
                cpu.v[r] = reg_value;
                cpu.pc.WORD = 0;
                skip_if_register_equal_value();
                if (value == reg_value) {
                    CU_ASSERT_EQUAL(2, cpu.pc.WORD);
                } else {
                    CU_ASSERT_EQUAL(0, cpu.pc.WORD);
                }
            }
        }
    }
}

void
test_skip_if_register_not_equal_value_integration(void) 
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
test_skip_if_register_equal_register_integration(void) 
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
test_skip_if_register_equal_register(void)
{
    for (int r = 0; r < 0x10; r++) {
        cpu.v[r] = r;
    }

    for (int r1 = 0; r1 < 0x10; r1++) {
        for (int r2 = 0; r2 < 0x10; r2++) {
            cpu.operand.WORD = r1;
            cpu.operand.WORD <<= 4;
            cpu.operand.WORD += r2;
            cpu.operand.WORD <<= 4;
            cpu.pc.WORD = 0;
            skip_if_register_equal_register();
            if (r1 == r2) {
                CU_ASSERT_EQUAL(2, cpu.pc.WORD);
            } else {
                CU_ASSERT_EQUAL(0, cpu.pc.WORD);
            }
        }
    }
}

void
test_move_value_to_register_integration(void) 
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
test_move_value_to_register(void) 
{
    setup();
    int value = 0x23;

    for (int r = 0; r < 0x10; r++) {
        cpu.operand.WORD = 0x60 + r;
        cpu.operand.WORD <<= 8;
        cpu.operand.WORD += value;
        move_value_to_register();
        for (int r_to_check = 0; r_to_check < 0x10; r_to_check++) {
            if (r_to_check != r) {
                CU_ASSERT_EQUAL(0, cpu.v[r_to_check]);
            } else {
                CU_ASSERT_EQUAL(value, cpu.v[r_to_check]);
            }
        }
        cpu.v[r] = 0;
    }
}

void
test_add_value_to_register(void)
{
    setup();
    for (int r = 0; r < 0x10; r++) {
        for (int reg_value = 0; reg_value < 0xFF; reg_value += 0x10) {
            for (int value = 0; value < 0xFF; value++) {
                cpu.v[r] = reg_value;
                cpu.operand.WORD = r << 8;
                cpu.operand.WORD += value;
                add_value_to_register();
                if (value + reg_value < 256) {
                    CU_ASSERT_EQUAL(value + reg_value, cpu.v[r]);
                } else {
                    CU_ASSERT_EQUAL(value + reg_value - 256, cpu.v[r]);
                }
            }
        }
    }
}

void
test_add_value_to_register_integration(void) 
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
test_move_register_to_register_integration(void) 
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
test_move_register_to_register(void)
{
    setup();
    for (int x = 0; x < 0x10; x++) {
        for (int y = 0; y < 0x10; y++) {
            if (x != y) {
                cpu.v[y] = 0x32;
                cpu.v[x] = 0;
                cpu.operand.WORD = x << 8;
                cpu.operand.WORD += (y << 4);
                move_register_into_register();
                CU_ASSERT_EQUAL(0x32, cpu.v[x]);
            }
        }
    }
}

void
test_logical_or_integration(void) 
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
test_logical_or(void)
{
    setup();
    for (int x = 0; x < 0x10; x++) {
        for (int y = 0; y < 0x10; y++) {
            if (x != y) {
                for (int s = 0; s < 0xFF; s += 0x10) {
                    for (int t = 0; t < 0xFF; t += 0x10) {
                        cpu.v[x] = s;
                        cpu.v[y] = t;
                        cpu.operand.WORD = x << 8;
                        cpu.operand.WORD += (y << 4);
                        logical_or();
                        CU_ASSERT_EQUAL(s | t, cpu.v[x]);
                    }
                }
            }
        }
    }
}

void
test_logical_and(void)
{
    setup();
    for (int x = 0; x < 0x10; x++) {
        for (int y = 0; y < 0x10; y++) {
            if (x != y) {
                for (int s = 0; s < 0xFF; s += 0x10) {
                    for (int t = 0; t < 0xFF; t += 0x10) {
                        cpu.v[x] = s;
                        cpu.v[y] = t;
                        cpu.operand.WORD = x << 8;
                        cpu.operand.WORD += (y << 4);
                        logical_and();
                        CU_ASSERT_EQUAL(s & t, cpu.v[x]);
                    }
                }
            }
        }
    }
}

void
test_logical_and_integration(void) 
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
test_exclusive_or(void)
{
    setup();
    for (int x = 0; x < 0x10; x++) {
        for (int y = 0; y < 0x10; y++) {
            if (x != y) {
                for (int s = 0; s < 0xFF; s += 0x10) {
                    for (int t = 0; t < 0xFF; t += 0x10) {
                        cpu.v[x] = s;
                        cpu.v[y] = t;
                        cpu.operand.WORD = x << 8;
                        cpu.operand.WORD += (y << 4);
                        exclusive_or();
                        CU_ASSERT_EQUAL(s ^ t, cpu.v[x]);
                    }
                }
            }
        }
    }
}

void
test_exclusive_or_integration(void) 
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
    for (int source = 0; source < 0xF; source++) {
        for (int target = 0; target < 0xF; target++) {
            if (source != target) {
                for (int sourceVal = 0; sourceVal < 0xFF; sourceVal += 0x10) {
                    for (int targetVal = 0; targetVal < 0xFF; targetVal += 0x10) {
                        cpu.v[source] = sourceVal;
                        cpu.v[target] = targetVal;
                        cpu.operand.WORD = source << 8;
                        cpu.operand.WORD += (target << 4);
                        add_register_to_register();
                        if ((sourceVal + targetVal) > 255) {
                            CU_ASSERT_EQUAL(sourceVal + targetVal - 256, cpu.v[source]);
                            CU_ASSERT_EQUAL(1, cpu.v[0xF]);
                        } else {
                            CU_ASSERT_EQUAL(sourceVal + targetVal, cpu.v[source]);
                            CU_ASSERT_EQUAL(0, cpu.v[0xF]);
                        }
                    }
                }
            }
        }
    }
}

void
test_add_register_to_register_integration(void) 
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
    for (int source = 0; source < 0xF; source++) {
        for (int target = 0; target < 0xF; target++) {
            if (source != target) {
                for (int sourceVal = 0; sourceVal < 0xFF; sourceVal += 0x10) {
                    for (int targetVal = 0; targetVal < 0xFF; targetVal += 0x10) {
                        cpu.v[source] = (short) sourceVal;
                        cpu.v[target] = (short) targetVal;
                        cpu.operand.WORD = source << 8;
                        cpu.operand.WORD += (target << 4);
                        subtract_register_from_register();
                        if (sourceVal > targetVal) {
                            CU_ASSERT_EQUAL(sourceVal - targetVal, cpu.v[source]);
                            CU_ASSERT_EQUAL(1, cpu.v[0xF]);
                        }
                        if (sourceVal < targetVal) {
                            CU_ASSERT_EQUAL(sourceVal - targetVal + 256, cpu.v[source]);
                            CU_ASSERT_EQUAL(0, cpu.v[0xF]);
                        }
                        if (sourceVal == targetVal) {
                            CU_ASSERT_EQUAL(0, cpu.v[source]);
                            CU_ASSERT_EQUAL(1, cpu.v[0xF]);
                        }
                    }
                }
            }
        }
    }
}

void
test_sub_register_from_register_integration(void) 
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
    for (int x = 0; x < 0xF; x++) {
        for (int y = 0; y < 0xF; y++) {
            for (int value = 0; value < 256; value++) {
                cpu.operand.WORD = x << 8;
                cpu.operand.WORD |= y << 4;
                cpu.v[y] = (short) value;
                short shifted_val = (short) (value >> 1);
                short bit_zero = (short) (cpu.v[y] & 0x1);
                cpu.v[0xF] = (short) 0;
                shift_right();
                CU_ASSERT_EQUAL(shifted_val, cpu.v[x]);
                CU_ASSERT_EQUAL(bit_zero, cpu.v[0xF]);
            }
        }
    }
}

void
test_shift_register_right_integration(void) 
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
    for (int source = 0; source < 0xF; source++) {
        for (int target = 0; target < 0xF; target++) {
            if (source != target) {
                for (int sourceValue = 0; sourceValue < 0xFF; sourceValue += 10) {
                    for (int targetValue = 0; targetValue < 0xF; targetValue++) {
                        cpu.v[source] = (short) sourceValue;
                        cpu.v[target] = (short) targetValue;
                        cpu.operand.WORD = source << 8;
                        cpu.operand.WORD += (target << 4);
                        subtract_register_from_register_borrow();
                        if (targetValue > sourceValue) {
                            CU_ASSERT_EQUAL(targetValue - sourceValue, cpu.v[source]);
                            CU_ASSERT_EQUAL(1, cpu.v[0xF]);
                        }
                        if (targetValue < sourceValue) {
                            CU_ASSERT_EQUAL(256 + targetValue - sourceValue, cpu.v[source]);
                            CU_ASSERT_EQUAL(0, cpu.v[0xF]);
                        }
                        if (targetValue == sourceValue) {
                            CU_ASSERT_EQUAL(0, cpu.v[source]);
                            CU_ASSERT_EQUAL(1, cpu.v[0xF]);
                        }
                    }
                }
            }
        }
    }
}

void
test_sub_register_from_register_source_from_target_integration(void) 
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
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x00);
    teardown();

    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu.v[0x2] = 0x02;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x1], 0x01);
    CU_ASSERT_EQUAL(cpu.v[0x2], 0x02);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();
}

void
test_shift_register_left(void)
{
    setup();
    for (int x = 0; x < 0xF; x++) {
        for (int y = 0; y < 0xF; y++) {
            for (int value = 0; value < 256; value++) {
                cpu.v[y] = value;
                cpu.operand.WORD = x << 8;
                cpu.operand.WORD += (y << 4);
                short bit_seven = ((value & 0x80) >> 7);
                short shifted_val = ((value << 1) & 0xFF);
                cpu.v[0xF] = 0;
                shift_left();
                CU_ASSERT_EQUAL(shifted_val, cpu.v[x]);
                CU_ASSERT_EQUAL(bit_seven, cpu.v[0xF]);
            }
        }
    }
}

void
test_shift_register_left_integration(void) 
{
    setup();
    tword.WORD = 0x801E;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x0], 0x02);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x00);
    teardown();

    /* Overflow condition */
    setup();
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0x1] = 0x80;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.v[0x0], 0x00);
    CU_ASSERT_EQUAL(cpu.v[0xF], 0x01);
    teardown();
}

void 
test_skip_if_register_not_equal_register(void) 
{
    setup();
    for (int x = 0; x < 0x10; x++) {
        cpu.v[x] = (short) x;
    }

    for (int register1 = 0; register1 < 0x10; register1++) {
        for (int register2 = 0; register2 < 0x10; register2++) {
            cpu.operand.WORD = register1;
            cpu.operand.WORD <<= 4;
            cpu.operand.WORD += register2;
            cpu.operand.WORD <<= 4;
            cpu.pc.WORD = 0;
            skip_if_register_not_equal_register();
            if (register1 != register2) {
                CU_ASSERT_EQUAL(2, cpu.pc.WORD);
            } else {
                CU_ASSERT_EQUAL(0, cpu.pc.WORD);
            }
        }
    }
}

void
test_skip_if_register_not_equal_register_integration(void) 
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
    for (int value = 0; value < 0x10000; value++) {
        cpu.operand.WORD = value;
        load_index_with_value();
        CU_ASSERT_EQUAL(value & 0x0FFF, cpu.i.WORD);
    }
}

void
test_load_index_integration(void) 
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
    for (int index = 0; index < 0xFFF; index += 10) {
        for (int value = 0; value < 0xFFF; value += 10) {
            cpu.v[0] = index;
            cpu.pc.WORD = 0;
            cpu.operand.WORD = value;
            jump_to_register_plus_value();
            CU_ASSERT_EQUAL(((index & 0xFF) + value) & 0xFFFF, cpu.pc.WORD);
        }
    }
}

void
test_jump_index_plus_value_integration(void) 
{
    setup();
    tword.WORD = 0xB010;
    memory_write_word(address, tword);
    cpu.pc.WORD = 0x0000;
    cpu.v[0] = 0x01;
    cpu_execute_single();
    CU_ASSERT_EQUAL(cpu.pc.WORD, 0x0011);
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
    for (int r = 0; r < 0xF; r++) {
        for (int value = 0; value < 0xFF; value += 10) {
            cpu.dt = (short) value;
            cpu.operand.WORD = r << 8;
            cpu.v[r] = 0;
            move_delay_timer_into_register();
            CU_ASSERT_EQUAL(value, cpu.v[r]);
        }
    }
}

void
test_load_delay_into_target_integration(void)
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
    for (int r = 0; r < 0xF; r++) {
        for (int value = 0; value < 0xFF; value += 10) {
            cpu.v[r] = (short) value;
            cpu.operand.WORD = r << 8;
            cpu.dt = 0;
            move_register_into_delay();
            CU_ASSERT_EQUAL(value, cpu.dt);
        }
    }
}

void
test_load_source_into_delay_integration(void)
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
    for (int r = 0; r < 0xF; r++) {
        for (int value = 0; value < 0xFF; value += 10) {
            cpu.v[r] = (short) value;
            cpu.operand.WORD = r << 8;
            cpu.st = 0;
            move_register_into_sound();
            CU_ASSERT_EQUAL(value, cpu.st);
        }
    }
}

void
test_load_source_into_sound_integration(void)
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
    for (int r = 0; r < 0xF; r++) {
        for (int index = 0; index < 0xFFF; index += 10) {
            cpu.i.WORD = index;
            cpu.v[r] = 0x89;
            cpu.operand.WORD = r << 8;
            add_register_to_index();
            CU_ASSERT_EQUAL(index + 0x89, cpu.i.WORD);
        }
    }
}

void
test_add_index_integration(void)
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
    for (int number = 0; number < 0x10; number++) {
        cpu.i.WORD = 0xFFF;
        cpu.v[0] = (short) number;
        cpu.operand.WORD = 0xF029;
        load_index_with_sprite();
        CU_ASSERT_EQUAL(number * 5, cpu.i.WORD);
    }
}

void
test_load_sprite_index_integration(void)
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
    char bcd[4];
    for (int number = 0; number < 0x100; number++) {
        sprintf(bcd, "%03d", number);
        cpu.i.WORD = 0;
        cpu.v[0] = number;
        cpu.operand.WORD = 0xF033;
        store_bcd_in_memory();
        CU_ASSERT_EQUAL(bcd[0], memory_read(0x0000)+48);
        CU_ASSERT_EQUAL(bcd[1], memory_read(0x0001)+48);
        CU_ASSERT_EQUAL(bcd[2], memory_read(0x0002)+48);
    }
}

void
test_bcd_integration(void)
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
    int index = 0x500;

    for (int x = 0; x < 0x10; x++) {
        cpu.v[x] = x + 0x89;
    }

    for (int n = 0; n < 0x10; n++) {
        for (int c = 0; c < 0x10; c++) {
            cpu.i.WORD = index + c;
            memory_write(cpu.i, 0x00);
        }

        cpu.i.WORD = index;
        cpu.operand.WORD = n << 8;
        int indexBefore = index;
        store_registers_in_memory();
        CU_ASSERT_EQUAL(indexBefore + n + 1, cpu.i.WORD);

        for (int c = 0; c < 0x10; c++) {
            if (c > n) {
                CU_ASSERT_EQUAL(0, memory_read(index + c));
            } else {
                CU_ASSERT_EQUAL(0x89 + c, memory_read(index + c));
            }
        }
    }
}

void
test_store_registers_integration(void)
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
    int index = 0x500;
    cpu.i.WORD = index;

    for (int n = 0; n < 0x10; n++) {
        memory_write(cpu.i, n + 0x89);
        cpu.i.WORD++;
    }

    for (int n = 0; n < 0x10; n++) {
        cpu.i.WORD = index;
        for (int r = 0; r < 0x10; r++) {
            cpu.v[r] = 0;
        }

        cpu.operand.WORD = 0xF065;
        cpu.operand.WORD |= (n << 8);
        int indexBefore = cpu.i.WORD;
        load_registers_from_memory();
        CU_ASSERT_EQUAL(indexBefore + n + 1, cpu.i.WORD);

        for (int r = 0; r < 0x10; r++) {
            if (r > n) {
                CU_ASSERT_EQUAL(0, cpu.v[r]);
            } else {
                CU_ASSERT_EQUAL(r + 0x89, cpu.v[r]);
            }
        }
    }    
}

void
test_load_registers_integration(void)
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
    cpu.operand.WORD = 0xF00;
    for (int regNumber = 0; regNumber < 16; regNumber++) {
        cpu.v[regNumber] = regNumber;
    }
    store_registers_in_rpl();
    for (int regNumber = 0; regNumber < 16; regNumber++) {
        CU_ASSERT_EQUAL(regNumber, cpu.rpl[regNumber]);
    }
}

void
test_store_registers_in_rpl_integration(void)
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
    cpu.operand.WORD = 0xF00;
    for (int regNumber = 0; regNumber < 16; regNumber++) {
        cpu.rpl[regNumber] = (short) regNumber;
    }
    read_registers_from_rpl();
    for (int regNumber = 0; regNumber < 16; regNumber++) {
        CU_ASSERT_EQUAL(regNumber, cpu.v[regNumber]);
    }
}

void
test_read_registers_from_rpl_integration(void)
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
test_load_pitch(void)
{
    setup();
    cpu.v[1] = 112;
    cpu.operand.WORD = 0xF13A;
    load_pitch();
    CU_ASSERT_EQUAL(112, pitch);
    CU_ASSERT_EQUAL(8000.0, playback_rate);    
    teardown();
}

void
test_load_pitch_integration(void)
{
    setup();
    tword.WORD = 0xF13A;
    memory_write_word(address, tword);
    cpu.v[1] = 112;
    cpu.pc.WORD = 0x0000;
    cpu_execute_single();
    CU_ASSERT_EQUAL(112, pitch);
    CU_ASSERT_EQUAL(8000.0, playback_rate);    
    teardown();  
}

void 
test_return_from_subroutine(void)
{
    setup();
    for (int a = 0x200; a < 0xFFFF; a += 0x10) {
        memory_write(cpu.sp, a & 0x00FF);
        tword.WORD = cpu.sp.WORD;
        tword.WORD++;
        memory_write(tword, (a & 0xFF00) >> 8);
        cpu.sp.WORD += 2;
        cpu.pc.WORD = 0;
        return_from_subroutine();
        CU_ASSERT_EQUAL(a, cpu.pc.WORD);
    }
    teardown();
}

void
test_return_from_subroutine_integration(void) 
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
