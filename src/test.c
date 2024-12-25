/**
 * Copyright (C) 2019-2024 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      test.c
 * @brief     Defines the test framework for the project
 * @author    Craig Thomas
 */

/* I N C L U D E S ************************************************************/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "globals.h"

/* G L O B A L S **************************************************************/

word tword;
word address;

/* M A I N ********************************************************************/

int
main() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_pSuite cpu_suite = CU_add_suite("CPU TESTS", 0, 0);
    CU_pSuite screen_suite = CU_add_suite("SCREEN TESTS", 0, 0);
    CU_pSuite keyboard_suite = CU_add_suite("KEYBOARD TESTS", 0, 0);

    if (cpu_suite == NULL || screen_suite == NULL || keyboard_suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(cpu_suite, "test_jump_to_address", test_jump_to_address) == NULL ||
        CU_add_test(cpu_suite, "test_jump_to_subroutine", test_jump_to_subroutine) == NULL ||
        CU_add_test(cpu_suite, "test_skip_if_register_equal_value", test_skip_if_register_equal_value) == NULL ||
        CU_add_test(cpu_suite, "test_skip_if_register_not_equal_value", test_skip_if_register_not_equal_value) == NULL ||
        CU_add_test(cpu_suite, "test_skip_if_register_equal_register", test_skip_if_register_equal_register) == NULL ||
        CU_add_test(cpu_suite, "test_skip_if_register_not_equal_register", test_skip_if_register_not_equal_register) == NULL ||
        CU_add_test(cpu_suite, "test_move_value_to_register", test_move_value_to_register) == NULL ||
        CU_add_test(cpu_suite, "test_add_value_to_register", test_add_value_to_register) == NULL ||
        CU_add_test(cpu_suite, "test_load_register_to_register", test_load_register_to_register) == NULL ||
        CU_add_test(cpu_suite, "test_logical_or", test_logical_or) == NULL ||
        CU_add_test(cpu_suite, "test_logical_and", test_logical_and) == NULL ||
        CU_add_test(cpu_suite, "test_logical_xor", test_logical_xor) == NULL ||
        CU_add_test(cpu_suite, "test_add_register_to_register", test_add_register_to_register) == NULL ||
        CU_add_test(cpu_suite, "test_sub_register_from_register", test_sub_register_from_register) == NULL ||
        CU_add_test(cpu_suite, "test_shift_register_right", test_shift_register_right) == NULL ||
        CU_add_test(cpu_suite, "test_sub_register_from_register_source_from_target", test_sub_register_from_register_source_from_target) == NULL ||
        CU_add_test(cpu_suite, "test_shift_register_left", test_shift_register_left) == NULL ||
        CU_add_test(cpu_suite, "test_load_index", test_load_index) == NULL ||
        CU_add_test(cpu_suite, "test_jump_index_plus_value", test_jump_index_plus_value) == NULL ||
        CU_add_test(cpu_suite, "test_generate_random", test_generate_random) == NULL ||
        CU_add_test(cpu_suite, "test_load_delay_into_target", test_load_delay_into_target) == NULL ||
        CU_add_test(cpu_suite, "test_load_source_into_delay", test_load_source_into_delay) == NULL ||
        CU_add_test(cpu_suite, "test_load_source_into_sound", test_load_source_into_sound) == NULL ||
        CU_add_test(cpu_suite, "test_add_index", test_add_index) == NULL ||
        CU_add_test(cpu_suite, "test_load_sprite_index", test_load_sprite_index) == NULL ||
        CU_add_test(cpu_suite, "test_bcd", test_bcd) == NULL ||
        CU_add_test(cpu_suite, "test_store_registers", test_store_registers) == NULL ||
        CU_add_test(cpu_suite, "test_load_registers", test_load_registers) == NULL ||
        CU_add_test(cpu_suite, "test_store_registers_in_rpl", test_store_registers_in_rpl) == NULL ||
        CU_add_test(cpu_suite, "test_read_registers_from_rpl", test_read_registers_from_rpl) == NULL ||
        CU_add_test(cpu_suite, "test_return_from_subroutine", test_return_from_subroutine) == NULL ||
        CU_add_test(cpu_suite, "test_exit_interpreter", test_exit_interpreter) == NULL ||
        CU_add_test(cpu_suite, "test_cpu_scroll_left", test_cpu_scroll_left) == NULL ||
        CU_add_test(cpu_suite, "test_cpu_scroll_right", test_cpu_scroll_right) == NULL ||
        CU_add_test(cpu_suite, "test_cpu_scroll_down", test_cpu_scroll_down) == NULL ||
        CU_add_test(cpu_suite, "test_cpu_screen_blank", test_cpu_screen_blank) == NULL ||
        CU_add_test(cpu_suite, "test_cpu_enable_extended_mode", test_cpu_enable_extended_mode) == NULL ||
        CU_add_test(cpu_suite, "test_cpu_disable_extended_mode", test_cpu_disable_extended_mode) == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(screen_suite, "test_set_get_pixel", test_set_get_pixel) == NULL ||
        CU_add_test(screen_suite, "test_set_pixel_color_zero_turns_pixel_off", test_set_pixel_color_zero_turns_pixel_off) == NULL ||
        CU_add_test(screen_suite, "test_screen_blank", test_screen_blank) == NULL ||
        CU_add_test(screen_suite, "test_screen_get_width_normal", test_screen_get_width_normal) == NULL ||
        CU_add_test(screen_suite, "test_screen_get_width_extended", test_screen_get_width_extended) == NULL ||
        CU_add_test(screen_suite, "test_screen_get_height_normal", test_screen_get_height_normal) == NULL ||
        CU_add_test(screen_suite, "test_screen_get_height_extended", test_screen_get_height_extended) == NULL ||
        CU_add_test(screen_suite, "test_screen_scroll_right", test_screen_scroll_right) == NULL ||
        CU_add_test(screen_suite, "test_screen_scroll_left", test_screen_scroll_left) == NULL ||
        CU_add_test(screen_suite, "test_screen_scroll_down", test_screen_scroll_down) == NULL ||
        CU_add_test(screen_suite, "test_screen_get_mode_scale_normal", test_screen_get_mode_scale_normal) == NULL ||
        CU_add_test(screen_suite, "test_screen_get_mode_scale_extended", test_screen_get_mode_scale_extended) == NULL ||
        CU_add_test(screen_suite, "test_screen_is_mode_extended_correct", test_screen_is_mode_extended_correct) == NULL
    )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(keyboard_suite, "test_keyboard_checkforkeypress_returns_false_on_no_keypress", test_keyboard_checkforkeypress_returns_false_on_no_keypress) == NULL ||
        CU_add_test(keyboard_suite, "test_keyboard_process_keydown", test_keyboard_process_keydown) == NULL ||
        CU_add_test(keyboard_suite, "test_keyboard_process_keyup", test_keyboard_process_keyup) == NULL ||
        CU_add_test(keyboard_suite, "test_keyboard_isemulatorkey", test_keyboard_isemulatorkey) == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_basic_run_tests();
    printf("\n");
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n");
    CU_cleanup_registry();
    return CU_get_error();
}

/* E N D   O F   F I L E *****************************************************/
