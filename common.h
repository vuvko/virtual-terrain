#ifndef COMMON_H
#define COMMON_H

#include <GL/gl.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>

const double eps = 0.0001;

void get_normal(float a[], float b[], float c[], float *n);

std::vector<float>
sum_vect(
        const std::vector<float> &a,
        const std::vector<float> &b);

std::vector<float> get_normal(
        const std::vector<float> &a,
        const std::vector<float> &b,
        const std::vector<float> &c);

void set_color(double h, unsigned char *colors);

std::vector<unsigned char> get_color(float h);

void set_water_color(unsigned char *colors);

std::vector<unsigned char> get_water_color(void);

std::vector<unsigned char>
generate_colors(const std::vector<float> &pointers);

std::vector<float>
generate_normals(const std::vector<float> &pointers);

void generate_land_indecies(float **pointers, int size, unsigned *indices, int &idx_size);

std::vector<std::vector<double> > generate_filter(double sigma, int &size);

void watch_array(int size, float *ptr);
void watch_array(int size, unsigned char *ptr);
void watch_array(int size, unsigned *ptr);

#endif // COMMON_H