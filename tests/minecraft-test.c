
#include <config.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <minecraft.h>
#include <limits.h>

static int64_t *seeds;

static void stest(void)
{
    int64_t seed = 0;
    int32_t x;
    int32_t z;
    char msg[64];
    int64_t cseed = 0;
    int64_t cseed1 = 0;
    for (z = -625; z < 625; z++)
    {
        for (x = -625; x < 625; x++)
        {
            cseed = s(seed, x, z);
            snprintf(msg, 64, "s(%ld, %d, %d)", seed, x, z);
            CU_assertImplementation(seeds[(z + 625) * 1250 + (x + 625)] == cseed, __LINE__, msg, __FILE__, "", CU_FALSE);
        }
    }
}

static void s1test(void)
{
    int64_t seed = 0;
    int32_t x;
    int32_t z;
    char msg[64];
    int64_t cseed = 0;
    int64_t cseed1 = 0;
    for (z = -625; z < 625; z++)
    {
        for (x = -625; x < 625; x++)
        {
            cseed1 = s1(seed, x, z);
            snprintf(msg, 64, "s1(%ld, %d, %d)", seed, x, z);
            CU_assertImplementation(seeds[(z + 625) * 1250 + (x + 625)] == cseed1, __LINE__, msg, __FILE__, "", CU_FALSE);
        }
    }
}

static void s2test(void)
{
    int64_t seed = 0;
    int32_t x;
    int32_t z;
    char msg[64];
    for (z = -625; z < 625; z++)
    {
        for (x = -625; x < 625; x++)
        {
            snprintf(msg, 64, "s2(%ld, %d, %d)", seed, x, z);
            CU_assertImplementation(seeds[(z + 625) * 1250 + (x + 625)] == s2(seed, x, z), __LINE__, msg, __FILE__, "", CU_FALSE);
        }
    }
}

int main(int argc, char **argv)
{
    seeds = calloc(1250 * 1250, sizeof(int64_t));
    if (seeds == NULL)
    {
        perror("seeds = calloc");
        return EXIT_FAILURE;
    }
    {
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/epsp-packet-sample-success.txt", getenv("srcdir"));
        FILE *in = fopen(path, "r");
        if (in == NULL)
        {
            perror("fopen");
            return EXIT_FAILURE;
        }
        int ret = 0;
        size_t i = 0;
        while ((ret = fscanf(in, "%ld", &seeds[i++])) != EOF)
        {
        }
        fclose(in);
    }
    mc_slime_init();
    CU_pSuite testSuite;
    CU_initialize_registry();
    testSuite = CU_add_suite("minecraft test suite", NULL, NULL);

    CU_add_test(testSuite, "stest", stest);
    //CU_add_test(testSuite, "s1test", s1test);
    CU_add_test(testSuite, "s2test", s2test);

    CU_basic_set_mode(CU_BRM_NORMAL);
    CU_basic_run_tests();
    unsigned int f = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    free(seeds);

    return f >= 1;
}
