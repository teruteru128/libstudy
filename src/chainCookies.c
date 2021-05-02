
#include <stdio.h>
#include <math.h>

double chainCookies(long chain, double cookiesPs, double cookies)
{
#ifdef _DEBUG
    printf("chain!");
#endif
    double digit = 7;
    double mult = 1;
    chain++;
    if (chain <= 1 && chain >= 1)
        chain += (long)fmax(0, ceil(log10(cookies)) - 10);
    double maxPayout = fmin(cookiesPs * 60 * 60 * 6, cookies * 0.5) * mult;
    double moni = fmax(digit, fmin(floor(1. / 9 * pow(10, (double)chain) * digit * mult), maxPayout));
    double nextMoni = fmax(digit, fmin(floor(1. / 9 * pow(10, (double)(chain + 1)) * digit * mult), maxPayout));
#if defined(_DEBUG)
    printf("%ld, %e, %e, %e\n", chain, maxPayout, moni, nextMoni);
    if (nextMoni >= maxPayout)
    {
        printf("Cookie chain over.\n");
    }
#endif
    return moni;
}
