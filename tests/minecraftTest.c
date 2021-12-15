
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
            CU_assertImplementation(seeds[(z + 625) * 1250 + (x + 625)]
                                        == cseed,
                                    __LINE__, msg, __FILE__, "", CU_FALSE);
        }
    }
}

static void seedtest(void)
{
    int64_t seed = 42056176818708L;
    int64_t ctx = 0;
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -196, -150) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -195, -150) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -194, -150) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -193, -150) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -196, -149) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -195, -149) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -194, -149) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -193, -149) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -196, -148) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -195, -148) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -194, -148) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -193, -148) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -196, -147) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -195, -147) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -194, -147) == 1);
    CU_ASSERT_TRUE(isSlimeChunk(&ctx, seed, -193, -147) == 1);
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
            CU_assertImplementation(seeds[(z + 625) * 1250 + (x + 625)]
                                        == cseed1,
                                    __LINE__, msg, __FILE__, "", CU_FALSE);
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
            CU_assertImplementation(seeds[(z + 625) * 1250 + (x + 625)]
                                        == s2(seed, x, z),
                                    __LINE__, msg, __FILE__, "", CU_FALSE);
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
        // データファイルのロードってどうやるんだぁ？
        char path[PATH_MAX] = PROJECT_SOURCE_DIR;
        snprintf(path, PATH_MAX, "%s/seeds.txt", getenv("srcdir"));
#ifdef _GNU_SOURCE
        char *p = canonicalize_file_name(path);
#elif _POSIX_VERSION >= 200809L || defined(linux)
        char *p = realpath(path, NULL);
#else
#error "unsupported realpath"
#endif
        if (p == NULL)
        {
            perror("");
            exit(1);
        }
        FILE *in = fopen(p, "r");
        free(p);
        if (in == NULL)
        {
            perror("fopen");
            return EXIT_FAILURE;
        }
        int ret = 0;
        size_t i = 0;
        char buf[21];
        while (fgets(buf, 21, in) != NULL)
        {
            char *p = strpbrk(buf, "\r\n");
            if (p != NULL)
            {
                *p = '\0';
            }
            /*
            size_t len = strlen(buf);
            size_t spn = strspn(buf, "-1234567890");
            if (len != spn)
            {
                fprintf(stderr, "%zu, %zu\n", len, spn);
                exit(EXIT_FAILURE);
            }
            */
            seeds[i++] = strtol(buf, NULL, 10);
        }
        // fscanfとかscanfとか嫌いなんですよね
        /*
        while ((ret = fscanf(in, " %ld", &seeds[i++])) != EOF)
        {
        }
        */
        fclose(in);
        fprintf(stderr, "hello!\n");
    }
    mc_slime_init();
    CU_pSuite testSuite;
    CU_initialize_registry();
    testSuite = CU_add_suite("minecraft test suite", NULL, NULL);

    CU_add_test(testSuite, "stest", stest);
    // CU_add_test(testSuite, "s1test", s1test);
    CU_add_test(testSuite, "s2test", s2test);
    CU_add_test(testSuite, "seedtest", seedtest);

    //CU_automated_run_tests();
    CU_console_run_tests();
    CU_cleanup_registry();
    free(seeds);

    return 0;
}
