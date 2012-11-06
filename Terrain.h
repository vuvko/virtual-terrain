#ifndef TERRAIN_H
#define TERRAIN_H

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <stdexcept>

#include <iostream>

#include "Random.h"

class Terrain
{
public:
    Terrain(int n_ = 1, bool use_seed = false, int seed_ = 0);
    ~Terrain();

    double at(int x, int y) const;
    double &at(int x, int y);
    int get_size(void) const;
    void print(void) const;

    enum
    {
        MIN = -100,
        MAX = 100
    };
private:
    double **matrix;
    int n;
    int size;
    int seed;

    static void generate(double **matrix, int size, int x_shift, int y_shift);
};

#endif // TERRAIN_H
