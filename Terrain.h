#ifndef TERRAIN_H
#define TERRAIN_H

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <stdexcept>
#include <vector>
#include <map>
#include <queue>

#include <iostream>

#include "Random.h"
#include "common.h"

class Terrain
{
public:
    Terrain(int n_ = 1, bool use_seed = false, int seed_ = 0);
    ~Terrain();

    double at(int x, int y) const;
    double &at(int x, int y);
    int get_size(void) const;
    std::vector<float> generate_line(int num) const;
    void generate_land_arrays(std::vector<float> &pointers,
            std::vector<float> &normals,
            std::vector<unsigned char> &colors,
            std::vector<unsigned> &indices);

    void generate_water_arrays(std::vector<float> &pointers,
            std::vector<float> &normals,
            std::vector<unsigned char> &colors,
            std::vector<unsigned> &indices);

    void print(void) const;

    enum
    {
        MIN = -10,
        MAX = 100,

        NORM = 10,

        MIN_HUMIDITY = 0,
        MAX_HUMIDITY = 5,

        NUM_RAND_POINTS_PER_SIZE = 25
    };
private:
    double **matrix;
    double **humidity;
    int n;
    int size;
    int seed;

    void flow_humidity(int x, int y, double val);
    static void generate(double **matrix, int size, int x_shift, int y_shift);
};

#endif // TERRAIN_H
