
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <CUnit/Automated.h>
#include <assert.h>
#include <limits.h>
#include <minecraft.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "simpletestcases.h"

int main(void)
{
    CU_initialize_registry();
    CU_pSuite testSuite = CU_add_suite("minecraft test suite", NULL, NULL);
    if (testSuite == NULL)
    {
        return CU_get_error();
    }

    // CU_add_test(testSuite, "Fail", fail);
    CU_add_test(testSuite, "Pass", pass);

    CU_automated_run_tests();
    int ret = CU_get_number_of_failures();
    CU_ErrorCode code = CU_list_tests_to_file();
    // CU_console_run_tests();
    CU_cleanup_registry();

    return (int)code;
}
