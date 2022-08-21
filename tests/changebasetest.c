
#include <CUnit/Automated.h>
#include <changebase.h>
#include <config.h>
#include <stdlib.h>
#include <string.h>

void base16test(void)
{
    char hex[] = "594152414e41494b413f00";
    unsigned char *bin;
    parseHex(&bin, hex);
    CU_ASSERT_STRING_EQUAL(bin, "YARANAIKA?");
    free(bin);
}
void base32decodetest(void)
{
    char string[] = "777P2";
    unsigned char buf[8] = "";
    unsigned char expected[] = { 1, 2, 3, 4, 5, 0xFF, 0xFE, 0xFD };
    size_t length = 0;
    base32decode(string, buf, &length);
    printf("length:%zu, ", length);
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x", buf[i]);
    }
    printf("\n");

    CU_ASSERT(memcmp(buf, expected, length) == 0);
}

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
    CU_pSuite suite = CU_add_suite("changebasetest", NULL, NULL);
    CU_add_test(suite, "base16 test1", base16test);
    CU_add_test(suite, "base32 test1", base32decodetest);

    CU_automated_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();
    return ret != 0;
}
