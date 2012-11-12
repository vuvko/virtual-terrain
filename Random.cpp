#include "Random.h"

using namespace std;

int
next_rand(int n)
{
    return (rand() + 0.0) / RAND_MAX * n;
}

double
next_rand(void)
{
    return (rand() + 0.0) / RAND_MAX * 2 - 1;
}
