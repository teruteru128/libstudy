
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <chainCookies.h>

void chainCookies_test1(void)
{
    double cookiesPs = 5.587e+26;
    double cookies = 5.877e+32;
    CU_ASSERT_DOUBLE_EQUAL(chainCookies(0, cookiesPs * 7, cookies),
                           8.777777777e+15, 12);
}

void chainCookiesTest(void);

int main(void)
{
    CU_pSuite suite;
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    suite = CU_add_suite("Chain Cookies Test", NULL, NULL);
    if ((NULL == CU_add_test(suite, "chain cookes test1", chainCookies_test1))
        || (NULL == CU_add_test(suite, "chain cookes test", chainCookiesTest)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    //CU_basic_set_mode(CU_BRM_VERBOSE);
    // CU_basic_run_tests();
    CU_automated_run_tests();
    //  CU_console_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
