#include "common.h"

using namespace std;

vector<Coord>
get_neighbours(int x, int y, int size)
{
    vector<Coord> neighbours;
    neighbours.push_back(Coord((size + x - 1) % size, y));
    neighbours.push_back(Coord((size + x + 1) % size, y));
    neighbours.push_back(Coord(x, (size + y - 1) % size));
    neighbours.push_back(Coord(x, (size + y + 1) % size));

    return neighbours;
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
/*
double
sqr(double a)
{
    return a * a;
}
*/
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

void
set_color(double h, unsigned char *colors)
{
    enum
    {
        MOUNTAIN = 13,
        GRASS = 7,
        SAND = 2,
        WATER = 0,

        SNOW_R = 255,
        SNOW_G = 255,
        SNOW_B = 255,

        MOUNTAIN_R = 200,
        MOUNTAIN_G = 150,
        MOUNTAIN_B = 150,

        GRASS_R = 5,
        GRASS_G = 200,
        GRASS_B = 5,

        SAND_R = 250,
        SAND_G = 250,
        SAND_B = 15,

        WATER_R = 0,
        WATER_G = 10,
        WATER_B = 150
    };

    h = fabs(h);

    if (h < WATER + eps) {
        //h = 0;
        //glColor3ub(WATER_R, WATER_G, WATER_B);
        colors[0] = WATER_R;
        colors[1] = WATER_G;
        colors[2] = WATER_B;
        return;
    }

    if (h < SAND + eps) {
        //glColor3ub(SAND_R, SAND_G, SAND_B);
        colors[0] = SAND_R;
        colors[1] = SAND_G;
        colors[2] = SAND_B;
        return;
    }
    if (h < GRASS + eps) {
        //glColor3ub(GRASS_R, GRASS_G, GRASS_B);
        colors[0] = GRASS_R;
        colors[1] = GRASS_G;
        colors[2] = GRASS_B;
        return;
    }
    if (h < MOUNTAIN + eps) {
        //glColor3ub(MOUNTAIN_R, MOUNTAIN_G, MOUNTAIN_B);
        colors[0] = MOUNTAIN_R;
        colors[1] = MOUNTAIN_G;
        colors[2] = MOUNTAIN_B;
        return;
    }
    //glColor3ub(SNOW_R, SNOW_G, SNOW_B);
    colors[0] = SNOW_R;
    colors[1] = SNOW_G;
    colors[2] = SNOW_B;
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
        M_TUNDRA = 3,
        M_WASTE = 2,
        M_DRY = 1,

        // Hight forest height
        H_TAIGA = 6,
        H_SHRUB = 4,
        H_TEMPERATE_DESERT = 2,

        // Low forest height
        L_TEMPERATE_RAIN = 6,
        L_TEMPERATE_FOREST = 5,
        L_GRASS = 3,
        L_TEMPERATE_DESERT = 1,

        // Low ground height
        G_TROPIC = 6,
        G_TEMPERATE_TROPIC = 4,
        G_GRASS = 2,
        G_DESERT = 1,

        MOUNTAIN_HEIGHT = 13,
        HIGH_FOREST_HEIGHT = 9,
        LOW_FOREST_HEIGHT = 5,
        LOW_GROUND_HEIGHT = 1,
        WATER_HEIGHT = 0,

        SNOW_R = 230,
        SNOW_G = 230,
        SNOW_B = 230,

        TUNDRA_R = 150,
        TUNDRA_G = 220,
        TUNDRA_B = 200,

        WASTE_R = 170,
        WASTE_G = 160,
        WASTE_B = 130,

        DRY_R = 100,
        DRY_G = 90,
        DRY_B = 90,

        TAIGA_R = 130,
        TAIGA_G = 180,
        TAIGA_B = 5,

        SHRUB_R = 150,
        SHRUB_G = 215,
        SHRUB_B = 50,

        TEMP_DESERT_R = 200,
        TEMP_DESERT_G = 234,
        TEMP_DESERT_B = 30,

        TEMP_RAIN_R = 50,
        TEMP_RAIN_G = 210,
        TEMP_RAIN_B = 70,

        TEMP_FOREST_R = 120,
        TEMP_FOREST_G = 240,
        TEMP_FOREST_B = 40,

        GRASS_R = 30,
        GRASS_G = 212,
        GRASS_B = 55,

        TROPIC_R = 15,
        TROPIC_G = 235,
        TROPIC_B = 80,

        TEMP_TROPIC_R = 80,
        TEMP_TROPIC_G = 255,
        TEMP_TROPIC_B = 50,

        DESERT_R = 235,
        DESERT_G = 167,
        DESERT_B = 50,

        WATER_R = 0,
        WATER_G = 10,
        WATER_B = 150
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
        //cerr << "Low ground" << endl;
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
        //cerr << "Low forest" << endl;
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
        //cerr << "High forest" << endl;
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
        //cerr << "Mountain" << endl;
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

void
set_water_color(unsigned char *colors)
{
    enum
    {
        WATER_R = 0,
        WATER_G = 10,
        WATER_B = 150,
        ALPHA = 175
    };

    colors[0] = WATER_R;
    colors[1] = WATER_G;
    colors[2] = WATER_B;
    colors[3] = ALPHA;
}

vector<unsigned char>
get_water_color(void)
{
    vector<unsigned char> colors(4, 0);
    enum
    {
        WATER_R = 0,
        WATER_G = 10,
        WATER_B = 150,
        ALPHA = 175
    };

    colors[0] = WATER_R;
    colors[1] = WATER_G;
    colors[2] = WATER_B;
    colors[3] = ALPHA;

    return colors;
}

vector<unsigned char>
generate_colors(const vector<float> &pointers)
{
    int size = pointers.size() / 3;
    vector<vector<unsigned char> > colors(size, vector<unsigned char>(3, 0));
    for (int i = 0; i < size; ++i) {
        colors[i] = get_color(pointers[i * 3 + 2], 0);
    }

    vector<unsigned char> c;
    for (int i = 0; i < size; ++i) {
        c.push_back(colors[i][0]);
        c.push_back(colors[i][1]);
        c.push_back(colors[i][2]);
    }

    return c;
}

vector<float>
generate_normals(const vector<float> &pointers)
{
    int size = pointers.size() / 3;
    vector<vector<float> > normals(size, vector<float>(3, 0));
    for (int i = 2; i < size; ++i) {
        //normals[i] = get_normal(pointers[i - 2], pointers[i - 1], pointers[i]);
        vector<vector<float> > points(3, vector<float>(3, 0));
        for (int j = 0; j < 3; ++j) {
            points[j][0] = pointers[(i - 2 + j) * 3];
            points[j][1] = pointers[(i - 2 + j) * 3 + 1];
            points[j][2] = pointers[(i - 2 + j) * 3 + 2];
        }
        normals[i] = get_normal(points[0], points[1], points[2]);
    }
    for (int i = 0; i < 3; ++i) {
        normals[0][i] = normals[2][i];
        normals[1][i] = normals[2][i];
    }

    vector<float> n;
    for (int i = 0; i < size; ++i) {
        n.push_back(normals[i][0]);
        n.push_back(normals[i][1]);
        n.push_back(normals[i][2]);
    }

    return n;
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

void
generate_land_indecies(float **pointers, int size, unsigned *indices, int &idx_size)
{
    /*
    if (!pointers) {
        return;
    }
    vector<unsigned> local;
    for (int i = 2; i < size; ++i) {
        if (pointers[i][2] < eps ||
                pointers[i - 1][2] < eps ||
                pointers[i - 2][2] < eps) {
            local.push_back(i - 2);
            local.push_back(i - 1);
            local.push_back(i);
        }
    }
    idx_size = local.size();
    indices = (unsigned *)calloc(idx_size, sizeof(*indices));
    for (int i = 0; i < local.size(); ++i) {
        //cerr << local[i] << " ";
        indices[i] = local[i];
    }
    */
    //cerr << endl;
}

void
watch_array(int size, float *ptr)
{
    if (!ptr) {
        return;
    }

    for (int i = 0; i < size; ++i) {
        cerr << '(' << ptr[i * 3] << ", " <<
                ptr[i * 3 + 1] << ") -> " <<
                ptr[i * 3 + 2] << endl;
    }
    /*
    for (int i = 0; i < 100; ++i) {
        cerr << ptr[i] << endl;
    }
    */
}

void
watch_array(int size, unsigned char *ptr)
{
    if (!ptr) {
        return;
    }

    for (int i = 0; i < size; ++i) {
        cerr << int(ptr[i * 3]) << ", " <<
                int(ptr[i * 3 + 1]) << ", " <<
                int(ptr[i * 3 + 2]) << endl;
    }
}

void
watch_array(int size, unsigned *ptr)
{
    if (!ptr) {
        return;
    }

    for (int i = 0; i < size; ++i) {
        cerr << ptr[i] << endl;
    }
}
