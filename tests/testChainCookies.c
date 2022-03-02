
#include <CUnit/Automated.h>
#include <CUnit/Console.h>
#include <chainCookies.h>

void chainCookies_test1(void)
{
    double cookiesPs = 5.587e+26;
    double cookies = 5.877e+32;
    CU_ASSERT_DOUBLE_EQUAL(chainCookies(0, cookiesPs * 7, cookies),
                           7.777777777e+1, 10);
}

void chainCookiesTest(void);

int main(void)
{
    CU_pSuite suite;
    CU_initialize_registry();

    suite = CU_add_suite("Chain Cookies Test", NULL, NULL);
    CU_add_test(suite, "chain cookes test1", chainCookies_test1);
    CU_add_test(suite, "chain cookes test", chainCookiesTest);

    CU_automated_run_tests();
    // CU_console_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
