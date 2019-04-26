/**
 * Copyright (C) 2019 Craig Thomas
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

/* M A I N ********************************************************************/

int
main() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("cpu_test_suite", 0, 0);

    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(suite, "test_jump_to_address", test_jump_to_address) == NULL ||
        CU_add_test(suite, "test_jump_to_subroutine", test_jump_to_subroutine) == NULL ||
        CU_add_test(suite, "test_skip_if_register_equal_value", test_skip_if_register_equal_value) == NULL ||
        CU_add_test(suite, "test_skip_if_register_not_equal_value", test_skip_if_register_not_equal_value) == NULL ||
        CU_add_test(suite, "test_skip_if_register_equal_register", test_skip_if_register_equal_register) == NULL ||
        CU_add_test(suite, "test_skip_if_register_not_equal_register", test_skip_if_register_not_equal_register) == NULL ||
        CU_add_test(suite, "test_move_value_to_register", test_move_value_to_register) == NULL) 
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
