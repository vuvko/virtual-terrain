#include "Terrain.h"

#include <iostream>
using namespace std;

Terrain::Terrain(int n_, bool use_seed, int seed_) :
    matrix(NULL), n(n_), size(), seed(seed_)
{
    size = pow(2, n) + 1;
    matrix = (double **)calloc(size, sizeof(*matrix));
    for (int i = 0; i < size; ++i)
    {
        matrix[i] = (double *)calloc(size, sizeof(*matrix[i]));
    }
    if (use_seed)
        srand(seed);
    else
        srand(time(NULL));

    matrix[0][0] = (next_rand(MIN, MAX) + 0.0) / MAX;
    matrix[0][size - 1] = (next_rand(MIN, MAX) + 0.0) / MAX;
    matrix[size - 1][0] = (next_rand(MIN, MAX) + 0.0) / MAX;
    matrix[size - 1][size - 1] = (next_rand(MIN, MAX) + 0.0) / MAX;

    generate(matrix, size, 0, 0);

    // post process
    // using gaussian filter
    int f_size;
    double sigma = min(2.0, size / 50.0);
    vector<vector<double> > filter = generate_filter(sigma, f_size);
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            double sum = 0;
            //cerr << "(" << x << ", " << y << ")" << endl;
            for (int i = 0; i < f_size; ++i) {
                int dx = -size / 2 + i;
                if ((x + dx < 0) || (x + dx >= size)) {
                    dx = -dx;
                }
                for (int j = 0; j < f_size; ++j) {
                    int dy = -size / 2 + j;
                    //cerr << "[" << dx << ", " << dy << "]" << endl;
                    if ((y + dy < 0) || (y + dy >= size)) {
                        dy = -dy;
                    }
                    sum += matrix[x + dx][y + dy] * filter[i][j];
                }
            }
            matrix[x][y] = sum;
        }
    }

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if (matrix[x][y] < eps) {
                matrix[x][y] = 0;
            }
        }
    }
}

void
Terrain::generate(double **matrix, int size, int x_shift, int y_shift)
{
    if (size <= 2)
        return;

    double corners[4];

    corners[0] = matrix[x_shift][y_shift];
    corners[1] = matrix[x_shift + size - 1][y_shift];
    corners[2] = matrix[x_shift][y_shift + size - 1];
    corners[3] = matrix[x_shift + size - 1][y_shift + size - 1];

    int med = size / 2;
    matrix[x_shift + med][y_shift] =
            max(-5.1, (corners[0] + corners[1]) / 2.0 + size * next_rand() / NORM);
    matrix[x_shift + med][y_shift + size - 1] =
            max(-5.1, (corners[2] + corners[3]) / 2.0 + size * next_rand() / NORM);
    matrix[x_shift][y_shift + med] =
            max(-5.1, (corners[0] + corners[2]) / 2.0 + size * next_rand() / NORM);
    matrix[x_shift + size - 1][y_shift + med] =
            max(-5.1, (corners[1] + corners[3]) / 2.0 + size * next_rand() / NORM);

    matrix[x_shift + med][y_shift + med] =
            max(-5.1, (matrix[x_shift + med][y_shift] + matrix[x_shift + med][y_shift + size - 1] +
            matrix[x_shift][y_shift + med] + matrix[x_shift + size - 1][y_shift + med]) / 4.0 +
            size * next_rand() / NORM);

    generate(matrix, (size + 1) / 2, x_shift, y_shift);
    generate(matrix, (size + 1) / 2, x_shift + med, y_shift);
    generate(matrix, (size + 1) / 2, x_shift, y_shift + med);
    generate(matrix, (size + 1) / 2, x_shift + med, y_shift + med);
}

vector<float>
Terrain::generate_line(int num) const
{
    vector<vector<float> > line(size * 2, vector<float>(3, 0));

    for (int i = 0; i < size; ++i) {
        //cerr << '(' << num << ' ' << i << " ) : " << matrix[num][i] << ", ";
        line[2 * i][0] = i;
        line[2 * i][1] = num;
        line[2 * i][2] = matrix[i][num];
        //cerr << '(' << num + 1 << ' ' << i << " ) : " << matrix[num + 1][i] << endl;
        line[2 * i + 1][0] = i;
        line[2 * i + 1][1] = num + 1;
        line[2 * i + 1][2] = matrix[i][num + 1];
    }

    vector<float> pointers;
    for (int i = 0; i < line.size(); ++i) {
        pointers.push_back(line[i][0]);
        pointers.push_back(line[i][1]);
        pointers.push_back(line[i][2]);
    }

    return pointers;
}

void
Terrain::generate_land_arrays(std::vector<float> &pointers,
        std::vector<float> &normals,
        std::vector<unsigned char> &colors,
        std::vector<unsigned> &indices)
{
    vector<vector<float> > local_p(size * size, vector<float>(3, 0));
    vector<vector<float> > local_n(size * size, vector<float>(3, 0));
    vector<vector<unsigned char> > local_c(size * size, vector<unsigned char>(3, 0));
    vector<bool> flags(size * size, false);
    for (int x = 0; x < size - 1; ++x) {
        for (int y = 0; y < size - 1; ++y) {
            vector<float> a(3), b(3), c(3), d(3), n(3);
            a[0] = x; a[1] = y; a[2] = matrix[x][y];
            b[0] = x + 1; b[1] = y; b[2] = matrix[x + 1][y];
            c[0] = x; c[1] = y + 1; c[2] = matrix[x][y + 1];
            d[0] = x + 1; d[1] = y + 1; d[2] = matrix[x + 1][y + 1];
            if (a[2] > eps || b[2] > eps || d[2] > eps) {
                local_p[a[0] * size + a[1]] = a;
                local_p[b[0] * size + b[1]] = b;
                local_p[d[0] * size + d[1]] = d;

                local_c[a[0] * size + a[1]] = get_color(a[2]);
                local_c[b[0] * size + b[1]] = get_color(b[2]);
                local_c[d[0] * size + d[1]] = get_color(d[2]);

                if (flags[a[0] * size + a[1]]) {
                    n = get_normal(a, b, d);
                    local_n[a[0] * size + a[1]] =
                            sum_vect(n, local_n[a[0] * size + a[1]]);
                } else {
                    local_n[a[0] * size + a[1]] = get_normal(a, b, d);
                    flags[a[0] * size + a[1]] = true;
                }
                if (flags[b[0] * size + b[1]]) {
                    n = get_normal(a, b, d);
                    local_n[b[0] * size + b[1]] =
                            sum_vect(n, local_n[b[0] * size + b[1]]);
                } else {
                    local_n[b[0] * size + b[1]] = get_normal(a, b, d);
                    flags[b[0] * size + b[1]] = true;
                }
                if (flags[d[0] * size + d[1]]) {
                    n = get_normal(a, b, d);
                    local_n[d[0] * size + d[1]] =
                            sum_vect(n, local_n[d[0] * size + d[1]]);
                } else {
                    local_n[d[0] * size + d[1]] = get_normal(a, b, d);
                    flags[d[0] * size + d[1]] = true;
                }

                indices.push_back(a[0] * size + a[1]);
                indices.push_back(b[0] * size + b[1]);
                indices.push_back(d[0] * size + d[1]);
            }

            if (a[2] > eps || c[2] > eps || d[2] > eps) {
                local_p[a[0] * size + a[1]] = a;
                local_p[c[0] * size + c[1]] = c;
                local_p[d[0] * size + d[1]] = d;

                local_c[a[0] * size + a[1]] = get_color(a[2]);
                local_c[c[0] * size + c[1]] = get_color(c[2]);
                local_c[d[0] * size + d[1]] = get_color(d[2]);

                if (flags[a[0] * size + a[1]]) {
                    n = get_normal(a, d, c);
                    local_n[a[0] * size + a[1]] =
                            sum_vect(n, local_n[a[0] * size + a[1]]);
                } else {
                    local_n[a[0] * size + a[1]] = get_normal(a, d, c);
                    flags[a[0] * size + a[1]] = true;
                }
                if (flags[c[0] * size + c[1]]) {
                    n = get_normal(a, d, c);
                    local_n[c[0] * size + c[1]] =
                            sum_vect(n, local_n[c[0] * size + c[1]]);
                } else {
                    local_n[c[0] * size + c[1]] = get_normal(a, d, c);
                    flags[c[0] * size + c[1]] = true;
                }
                if (flags[d[0] * size + d[1]]) {
                    n = get_normal(a, d, c);
                    local_n[d[0] * size + d[1]] =
                            sum_vect(n, local_n[d[0] * size + d[1]]);
                } else {
                    local_n[d[0] * size + d[1]] = get_normal(a, d, c);
                    flags[d[0] * size + d[1]] = true;
                }

                indices.push_back(a[0] * size + a[1]);
                indices.push_back(c[0] * size + c[1]);
                indices.push_back(d[0] * size + d[1]);
            }
        }
    }

    for (int i = 0; i < local_p.size(); ++i) {
        pointers.push_back(local_p[i][0]);
        pointers.push_back(local_p[i][1]);
        pointers.push_back(local_p[i][2]);

        normals.push_back(local_n[i][0]);
        normals.push_back(local_n[i][1]);
        normals.push_back(local_n[i][2]);

        colors.push_back(local_c[i][0]);
        colors.push_back(local_c[i][1]);
        colors.push_back(local_c[i][2]);
    }
}

void
Terrain::generate_water_arrays(std::vector<float> &pointers,
        std::vector<float> &normals,
        std::vector<unsigned char> &colors,
        std::vector<unsigned> &indices)
{
    vector<vector<float> > local_p(size * size, vector<float>(3, 0));
    vector<vector<float> > local_n(size * size, vector<float>(3, 0));
    vector<vector<unsigned char> > local_c(size * size, vector<unsigned char>(4, 0));
    vector<bool> flags(size * size, false);
    for (int x = 0; x < size - 1; ++x) {
        for (int y = 0; y < size - 1; ++y) {
            vector<float> a(3), b(3), c(3), d(3), n(3);
            a[0] = x; a[1] = y; a[2] = matrix[x][y];
            b[0] = x + 1; b[1] = y; b[2] = matrix[x + 1][y];
            c[0] = x; c[1] = y + 1; c[2] = matrix[x][y + 1];
            d[0] = x + 1; d[1] = y + 1; d[2] = matrix[x + 1][y + 1];
            if (a[2] < eps && b[2] < eps && d[2] < eps) {
                local_p[a[0] * size + a[1]] = a;
                local_p[b[0] * size + b[1]] = b;
                local_p[d[0] * size + d[1]] = d;

                local_c[a[0] * size + a[1]] = get_water_color();
                local_c[b[0] * size + b[1]] = get_water_color();
                local_c[d[0] * size + d[1]] = get_water_color();

                if (flags[a[0] * size + a[1]]) {
                    n = get_normal(a, b, d);
                    local_n[a[0] * size + a[1]] =
                            sum_vect(n, local_n[a[0] * size + a[1]]);
                } else {
                    local_n[a[0] * size + a[1]] = get_normal(a, b, d);
                    flags[a[0] * size + a[1]] = true;
                }
                if (flags[b[0] * size + b[1]]) {
                    n = get_normal(a, b, d);
                    local_n[b[0] * size + b[1]] =
                            sum_vect(n, local_n[b[0] * size + b[1]]);
                } else {
                    local_n[b[0] * size + b[1]] = get_normal(a, b, d);
                    flags[b[0] * size + b[1]] = true;
                }
                if (flags[d[0] * size + d[1]]) {
                    n = get_normal(a, b, d);
                    local_n[d[0] * size + d[1]] =
                            sum_vect(n, local_n[d[0] * size + d[1]]);
                } else {
                    local_n[d[0] * size + d[1]] = get_normal(a, b, d);
                    flags[d[0] * size + d[1]] = true;
                }

                indices.push_back(a[0] * size + a[1]);
                indices.push_back(b[0] * size + b[1]);
                indices.push_back(d[0] * size + d[1]);
            }

            if (a[2] < eps && c[2] < eps && d[2] < eps) {
                local_p[a[0] * size + a[1]] = a;
                local_p[c[0] * size + c[1]] = c;
                local_p[d[0] * size + d[1]] = d;

                local_c[a[0] * size + a[1]] = get_water_color();
                local_c[c[0] * size + c[1]] = get_water_color();
                local_c[d[0] * size + d[1]] = get_water_color();

                if (flags[a[0] * size + a[1]]) {
                    n = get_normal(a, d, c);
                    local_n[a[0] * size + a[1]] =
                            sum_vect(n, local_n[a[0] * size + a[1]]);
                } else {
                    local_n[a[0] * size + a[1]] = get_normal(a, d, c);
                    flags[a[0] * size + a[1]] = true;
                }
                if (flags[c[0] * size + c[1]]) {
                    n = get_normal(a, d, c);
                    local_n[c[0] * size + c[1]] =
                            sum_vect(n, local_n[c[0] * size + c[1]]);
                } else {
                    local_n[c[0] * size + c[1]] = get_normal(a, d, c);
                    flags[c[0] * size + c[1]] = true;
                }
                if (flags[d[0] * size + d[1]]) {
                    n = get_normal(a, d, c);
                    local_n[d[0] * size + d[1]] =
                            sum_vect(n, local_n[d[0] * size + d[1]]);
                } else {
                    local_n[d[0] * size + d[1]] = get_normal(a, d, c);
                    flags[d[0] * size + d[1]] = true;
                }

                indices.push_back(a[0] * size + a[1]);
                indices.push_back(c[0] * size + c[1]);
                indices.push_back(d[0] * size + d[1]);
            }
        }
    }

    for (int i = 0; i < local_p.size(); ++i) {
        pointers.push_back(local_p[i][0]);
        pointers.push_back(local_p[i][1]);
        pointers.push_back(local_p[i][2]);

        normals.push_back(local_n[i][0]);
        normals.push_back(local_n[i][1]);
        normals.push_back(local_n[i][2]);

        colors.push_back(local_c[i][0]);
        colors.push_back(local_c[i][1]);
        colors.push_back(local_c[i][2]);
        colors.push_back(local_c[i][3]);
    }
}


double &
Terrain::at(int x, int y)
{
    if (x < 0 || y < 0 || x >= size || y >= size)
        throw std::range_error("Invalid arguments.");
    return *&matrix[x][y];
}

double
Terrain::at(int x, int y) const
{
    if (x < 0 || y < 0 || x >= size || y >= size)
        throw std::range_error("Invalid arguments.");
    return matrix[x][y];
}

int
Terrain::get_size() const
{
    return size;
}

void
Terrain::print(void) const
{
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cerr << matrix[i][j] << ' ';
        }
        std::cerr << std::endl;
    }
}

Terrain::~Terrain()
{
    for (int i = 0; i < size; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
}
