#include "Util.hpp"
#include "sys/time.h"
#include <fstream>


double Util::getRandomRange(double lower, double upper) {
    return lower + (upper - lower) * (double) rand() / static_cast<double>(RAND_MAX);
}

// Start timer
static struct timeval tvalBefore;

void Util::time_before() {
    gettimeofday(&tvalBefore, NULL);
}

// End timer, return seconds since last time_b call
double Util::time_after() {
    struct timeval tvalAfter;
    gettimeofday(&tvalAfter, NULL);
    float timeSince = tvalAfter.tv_sec - tvalBefore.tv_sec;
    float diff = tvalBefore.tv_usec - tvalAfter.tv_usec;
    if (diff > 0)
        timeSince -= 1.0;
    timeSince += (diff < 0 ? -diff : diff) / 1000000;
    return timeSince;
}