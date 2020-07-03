
#include <config.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <minecraft.h>

static void s1test()
{
    int64_t seed = 0;
    int32_t x;
    int32_t z;
    char msg[64];
    int64_t cseed = 0;
    int64_t cseed1 = 0;
    for(z = -625; z < -615; z++)
    {
        for(x = -625; x < 625; x++)
        {
            cseed = s(seed, x, z);
            cseed1 = s1(seed, x, z);
            snprintf(msg, 64, "s(%ld, %d, %d) == s1(%ld, %d, %d)", seed, x, z, seed, x, z);
            CU_assertImplementation(cseed == cseed1, __LINE__, msg, __FILE__, "", CU_FALSE);
            if(cseed != cseed1)
            {
                fprintf(stderr, "(%ld, %d, %d) : %ld, %ld\n", seed, x, z, cseed, cseed1);
            }
        }
    }
}

static void s2test()
{
    int64_t seed = 0;
    int32_t x;
    int32_t z;
    char msg[64];
    for(z = -625; z < 625; z++)
    {
        for(x = -625; x < 625; x++)
        {
            snprintf(msg, 64, "s(%ld, %d, %d) == s2(%ld, %d, %d)", seed, x, z, seed, x, z);
            CU_assertImplementation((s(seed, x, z) == s2(seed, x, z)), __LINE__, msg, __FILE__, "", CU_FALSE);
        }
    }
}

int main(int argc, char** argv)
{
    mc_slime_init();
    CU_pSuite testSuite;
    CU_initialize_registry();
    testSuite = CU_add_suite("minecraft test suite", NULL, NULL);

    CU_add_test(testSuite, "s1test", s1test);
    CU_add_test(testSuite, "s2test", s2test);

    CU_basic_set_mode(CU_BRM_NORMAL);
    CU_basic_run_tests();
    unsigned int f = CU_get_number_of_tests_failed();
    CU_cleanup_registry();

    return f >= 1;
}
