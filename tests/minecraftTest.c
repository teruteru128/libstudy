
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <CUnit/Console.h>
#include <assert.h>
#include <limits.h>
#include <minecraft.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void fail(void) { CU_FAIL(""); }

int main(void)
{
    CU_initialize_registry();
    CU_pSuite testSuite = CU_add_suite("minecraft test suite", NULL, NULL);

    CU_add_test(testSuite, "Fail", fail);

    CU_automated_run_tests();
    // CU_console_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
