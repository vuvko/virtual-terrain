#ifndef COMMON_H
#define COMMON_H

#include <GL/gl.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <map>

const double eps = 0.0001;

typedef std::pair<int, int> Coord;
std::vector<Coord> get_neighbours(int x, int y, int size);

std::vector<double>
get_line_coefficients(double x1, double y1, double x2, double y2);

bool
above_line(const std::vector<double> &coeff, double x, double y);

template<typename T> T
sqr(T a)
{
    return a * a;
}

void get_normal(float a[], float b[], float c[], float *n);

std::vector<float>
sum_vect(
        const std::vector<float> &a,
        const std::vector<float> &b);

std::vector<float> get_normal(
        const std::vector<float> &a,
        const std::vector<float> &b,
        const std::vector<float> &c);

double norm(const std::vector<double> a);
double norm(const std::vector<double> a, const std::vector<double> b);

std::vector<unsigned char> get_color(float h, double humidity);

std::vector<unsigned char> get_water_color(void);

std::vector<std::vector<double> > generate_filter(double sigma, int &size);

#endif // COMMON_H
