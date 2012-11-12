#ifndef RANDOM_H
#define RANDOM_H

#include <cstdlib>

template<typename T>
T next_rand(const T &a, const T &b)
{
    return ((rand() + 0.0) / RAND_MAX) * (b - a) + a;
}

int next_rand(int n);
double next_rand(void);

#endif // RANDOM_H
