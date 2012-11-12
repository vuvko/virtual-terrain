#include "common.h"

using namespace std;

vector<Coord>
get_neighbours(int x, int y, int size)
{
    vector<Coord> neighbours;
    neighbours.push_back(Coord((size + x - 1) % size, y));
    neighbours.push_back(Coord((x + 1) % size, y));
    neighbours.push_back(Coord(x, (size + y - 1) % size));
    neighbours.push_back(Coord(x, (y + 1) % size));

    return neighbours;
}

bool
above_line(const vector<double> &coeff, double x, double y)
{
    return (x * coeff[0] + y * coeff[1] + coeff[2]) > 0.1;
}

vector<double>
get_line_coefficients(double x1, double y1, double x2, double y2)
{
    vector<double> coeff(3, 0);
    coeff[0] = y1 - y2;
    coeff[1] = x2 - x1;
    coeff[2] = x1 * y2 - x2 * y1;

    return coeff;
}

void
get_normal(float a[3], float b[3], float c[3], float *n)
{
    n[0] = (b[1] - a[1]) * (c[2] - a[2]) - (b[2] - a[2]) * (c[1] - a[1]);
    n[1] = (c[0] - a[0]) * (b[2] - a[2]) - (b[0] - a[0]) * (c[2] - a[2]);
    n[2] = (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1]);
    double sum = 0;
    for (int i = 0; i < 3; ++i) {
        sum += n[i] * n[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < 3; ++i) {
        n[i] /= sum;
    }
}

vector<float>
sum_vect(const vector<float> &a, const vector<float> &b)
{
    if (a.size() != b.size()) {
        return vector<float>();
    }
    vector<float> c(a.size(), 0);
    double sum = 0;
    for (int i = 0; i < a.size(); ++i) {
        c[i] = a[i] + b[i];
        sum += c[i] * c[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < a.size(); ++i) {
        c[i] /= sum;
    }

    return c;

}

vector<float>
get_normal(
        const std::vector<float> &a,
        const std::vector<float> &b,
        const std::vector<float> &c)
{
    vector<float> n(3, 0);
    n[0] = (b[1] - a[1]) * (c[2] - a[2]) - (b[2] - a[2]) * (c[1] - a[1]);
    n[1] = (c[0] - a[0]) * (b[2] - a[2]) - (b[0] - a[0]) * (c[2] - a[2]);
    n[2] = (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1]);
    double sum = 0;
    for (int i = 0; i < 3; ++i) {
        sum += n[i] * n[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < 3; ++i) {
        n[i] /= sum;
    }

    return n;
}

double
norm(const std::vector<double> a)
{
    return sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
}

double
norm(const std::vector<double> a, const std::vector<double> b)
{
    return sqrt(sqr(b[0] - a[0]) + sqr(b[1] - a[1]) + sqr(b[2] - a[2]));
}

vector<unsigned char>
get_color(float h, double humidity)
{
    enum
    {
        // High mountain height
        // everything is snow?

        // Mountain height
        M_SNOW = 6,
        M_TUNDRA = 5,
        M_WASTE = 4,
        M_DRY = 2,

        // Hight forest height
        H_TAIGA = 6,
        H_SHRUB = 4,
        H_TEMPERATE_DESERT = 2,

        // Low forest height
        L_TEMPERATE_RAIN = 6,
        L_TEMPERATE_FOREST = 4,
        L_GRASS = 3,
        L_TEMPERATE_DESERT = 1,

        // Low ground height
        G_TROPIC = 6,
        G_TEMPERATE_TROPIC = 4,
        G_GRASS = 2,
        G_DESERT = 1,

        MOUNTAIN_HEIGHT = 30,
        HIGH_FOREST_HEIGHT = 21,
        LOW_FOREST_HEIGHT = 17,
        LOW_GROUND_HEIGHT = 4,
        WATER_HEIGHT = 0,

        SNOW_R = 230,
        SNOW_G = 230,
        SNOW_B = 230,

        TUNDRA_R = 128,
        TUNDRA_G = 155,
        TUNDRA_B = 143,

        WASTE_R = 170,
        WASTE_G = 160,
        WASTE_B = 130,

        DRY_R = 100,
        DRY_G = 90,
        DRY_B = 90,

        TAIGA_R = 16,
        TAIGA_G = 92,
        TAIGA_B = 23,

        SHRUB_R = 160,
        SHRUB_G = 190,
        SHRUB_B = 77,

        TEMP_DESERT_R = 207,
        TEMP_DESERT_G = 214,
        TEMP_DESERT_B = 13,

        TEMP_RAIN_R = 50,
        TEMP_RAIN_G = 210,
        TEMP_RAIN_B = 70,

        TEMP_FOREST_R = 153,
        TEMP_FOREST_G = 196,
        TEMP_FOREST_B = 87,

        GRASS_R = 59,
        GRASS_G = 172,
        GRASS_B = 75,

        TROPIC_R = 86,
        TROPIC_G = 174,
        TROPIC_B = 132,

        TEMP_TROPIC_R = 136,
        TEMP_TROPIC_G = 196,
        TEMP_TROPIC_B = 96,

        DESERT_R = 235,
        DESERT_G = 167,
        DESERT_B = 50,

        WATER_R = 69,
        WATER_G = 77,
        WATER_B = 106
    };
    vector<unsigned char> colors(3, 0);
    h = fabs(h);
    //cerr << "HEIGHT: " << h << "; HUMIDITY: " << humidity << ";" << endl;
    if (h < WATER_HEIGHT + eps) {
        colors[0] = WATER_R;
        colors[1] = WATER_G;
        colors[2] = WATER_B;
        return colors;
    }

    if (h < LOW_GROUND_HEIGHT + eps) {
        // Low ground height
        if (humidity < G_DESERT) {
            colors[0] = DESERT_R;
            colors[1] = DESERT_G;
            colors[2] = DESERT_B;
            return colors;
        }
        if (humidity < G_GRASS) {
            colors[0] = GRASS_R;
            colors[1] = GRASS_G;
            colors[2] = GRASS_B;
            return colors;
        }
        if (humidity < G_TEMPERATE_TROPIC) {
            colors[0] = TEMP_TROPIC_R;
            colors[1] = TEMP_TROPIC_G;
            colors[2] = TEMP_TROPIC_B;
            return colors;
        }
        if (humidity < G_TROPIC) {
            colors[0] = TROPIC_R;
            colors[1] = TROPIC_G;
            colors[2] = TROPIC_B;
            return colors;
        }
    }
    if (h < LOW_FOREST_HEIGHT + eps) {
        // Low forest height
        if (humidity < L_TEMPERATE_DESERT) {
            colors[0] = TEMP_DESERT_R;
            colors[1] = TEMP_DESERT_G;
            colors[2] = TEMP_DESERT_B;
            return colors;
        }
        if (humidity < L_GRASS) {
            colors[0] = GRASS_R;
            colors[1] = GRASS_G;
            colors[2] = GRASS_B;
            return colors;
        }
        if (humidity < L_TEMPERATE_FOREST) {
            colors[0] = TEMP_FOREST_R;
            colors[1] = TEMP_FOREST_G;
            colors[2] = TEMP_FOREST_B;
            return colors;
        }
        if (humidity < L_TEMPERATE_RAIN) {
            colors[0] = TEMP_RAIN_R;
            colors[1] = TEMP_RAIN_G;
            colors[2] = TEMP_RAIN_B;
            return colors;
        }
    }
    if (h < HIGH_FOREST_HEIGHT + eps) {
        // Hight forest height
        if (humidity < H_TEMPERATE_DESERT) {
            colors[0] = TEMP_DESERT_R;
            colors[1] = TEMP_DESERT_G;
            colors[2] = TEMP_DESERT_B;
            return colors;
        }
        if (humidity < H_SHRUB) {
            colors[0] = SHRUB_R;
            colors[1] = SHRUB_G;
            colors[2] = SHRUB_B;
            return colors;
        }
        if (humidity < H_TAIGA) {
            colors[0] = TAIGA_R;
            colors[1] = TAIGA_G;
            colors[2] = TAIGA_B;
            return colors;
        }
    }
    if (h < MOUNTAIN_HEIGHT + eps) {
        // Mountain height
        if (humidity < M_DRY) {
            colors[0] = DRY_R;
            colors[1] = DRY_G;
            colors[2] = DRY_B;
            return colors;
        }
        if (humidity < M_WASTE) {
            colors[0] = WASTE_R;
            colors[1] = WASTE_G;
            colors[2] = WASTE_B;
            return colors;
        }
        if (humidity < M_TUNDRA) {
            colors[0] = TUNDRA_R;
            colors[1] = TUNDRA_G;
            colors[2] = TUNDRA_B;
            return colors;
        }
        if (humidity < M_SNOW) {
            colors[0] = SNOW_R;
            colors[1] = SNOW_G;
            colors[2] = SNOW_B;
            return colors;
        }
    }

    colors[0] = SNOW_R;
    colors[1] = SNOW_G;
    colors[2] = SNOW_B;
    return colors;
}

vector<unsigned char>
get_water_color(void)
{
    vector<unsigned char> colors(4, 0);
    enum
    {
        WATER_R = 70,
        WATER_G = 80,
        WATER_B = 100,
        ALPHA = 175
    };

    colors[0] = WATER_R;
    colors[1] = WATER_G;
    colors[2] = WATER_B;
    colors[3] = ALPHA;

    return colors;
}

vector<vector<double> >
generate_filter(double sigma, int &size)
{
    if (size == 0) {
        size = 6 * sigma;
    }
    if (size < 3) {
        size = 3;
    }
    if (size % 2) {
        ++size;
    }
    vector<vector<double> > filter(size, vector<double>(size, 0));
    for (int i = 0; i < size; ++i) {
        int x = -size / 2 + i;
        for (int j = 0; j < size; ++j) {
            int y = -size / 2 + j;
            filter[i][j] = exp(-(x * x + y * y) / (2 * sigma * sigma)) /
                    (2 * M_PI * sigma * sigma);
        }
    }

    return filter;
}
