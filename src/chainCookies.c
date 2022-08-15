
#include <stdio.h>
#include <math.h>
#include <stdint.h>

double chainCookies(int64_t chain, double cookiesPs, double cookies)
{
    double digit = 7;
    double mult = 1;
    chain++;
    if (chain <= 1 && chain >= 1)
        chain += (int64_t)fmax(0, ceil(log10(cookies)) - 10);
    double maxPayout = fmin(cookiesPs * 60 * 60 * 6, cookies * 0.5) * mult;
    double moni = fmax(digit, fmin(floor(1. / 9 * pow(10, (double)chain) * digit * mult), maxPayout));
    double nextMoni = fmax(digit, fmin(floor(1. / 9 * pow(10, (double)(chain + 1)) * digit * mult), maxPayout));
    return moni;
}
