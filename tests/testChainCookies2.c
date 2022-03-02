
#include <CUnit/Console.h>
#include <chainCookies.h>
#include <math.h>
#include <stdio.h>

void chainCookiesTest(void)
{
    long chain = 0;
    double cookiesPs = 5.587e+26;
    double cookies = 5.877e+32;
    fprintf(stderr, "%f\n", chainCookies(chain, cookiesPs * 7, cookies));
    fprintf(stderr, "%f = 10^%f\n", 15., log10(15));
    CU_FAIL("a");
}
