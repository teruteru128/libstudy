
#include <shuffle.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <xorshift.h>

static uint32_t seed = 0;

/**
 * @brief
 * @deprecated
 */
uint32_t *getseedp(void) { return &seed; }

// TODO 乱数をDIできるようにする
/**
 * @brief
 *
 * @param base
 * @param nmemb
 * @param size
 */
void shuffle(void *base, size_t nmemb, size_t size)
{
    char w[nmemb];
    memset(w, 0, nmemb);
    char *array = base;
    const size_t a = size - 2;
    for (size_t i = 0; i < a; i++)
    {
        size_t j = ((seed = xorshift(seed)) % (size - i)) + i;
        memcpy(w, array + i * nmemb, nmemb);
        memcpy(array + i * nmemb, array + j * nmemb, nmemb);
        memcpy(array + j * nmemb, w, nmemb);
    }
}
