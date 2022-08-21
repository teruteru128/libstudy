
#include <CUnit/Automated.h>
#include <chainCookies.h>

void chainCookies_test1(void)
{
    double cookiesPs = 5.587e+26;
    double cookies = 5.877e+32;
    CU_ASSERT_DOUBLE_EQUAL(chainCookies(0, cookiesPs * 7, cookies),
                           777777777777777764728832., 12);
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

    CU_automated_run_tests();
    CU_ErrorCode code = CU_list_tests_to_file();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();
    return ret;
}
