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
    Terrain(
            const float *light_pos,
            int n_ = 1,
            bool use_seed = false,
            int seed_ = 0);
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
        MIN = -2,
        MAX = 15,

        ROUGH = 5,

        MIN_HUMIDITY = 0,
        MAX_HUMIDITY = 5,

        NUM_RAND_POINTS_PER_SIZE = 25,
        SHADOW_NORM = 3,
        SHADOW_BOUND = 1,
        SHADOW_MAX = 10
    };
private:
    std::vector<std::vector<double> > matrix;
    std::vector<std::vector<double> > humidity;
    std::vector<std::vector<double> > shadows;
    int n;
    int size;
    int seed;

    void flow_humidity(int x, int y, double val);
};

#endif // TERRAIN_H
