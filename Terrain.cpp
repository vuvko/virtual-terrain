#include "Terrain.h"

#include <iostream>
using namespace std;

Terrain::Terrain(const float *light_pos, int n_, bool use_seed, int seed_) :
    matrix(), humidity(), shadows(), n(n_), size(), seed(seed_)
{
    size = pow(2, n) + 1;

    if (use_seed)
        srand(seed);
    else
        srand(time(NULL));

    matrix = vector<vector<double> >(size, vector<double>(size, 0));
    humidity = vector<vector<double> >(size, vector<double>(size, 0));
    shadows = vector<vector<double> >(size, vector<double>(size, 0));

    matrix[0][0] = (next_rand(MIN, MAX) + 0.0);
    matrix[0][size - 1] = (next_rand(MIN, MAX) + 0.0);
    matrix[size - 1][0] = (next_rand(MIN, MAX) + 0.0);
    matrix[size - 1][size - 1] = (next_rand(MIN, MAX) + 0.0);

    double coeff = double(MAX) / size;
    generate(matrix, size, 0, 0, coeff);
    //cerr << coeff << endl;

    // post process
    // using gaussian filter
    int f_size = 0;
    double sigma = 1.5;//min(2.0, size / 50.0);
    double bound = 6, approx = 0.9;
    vector<vector<double> > filter = generate_filter(sigma, f_size);
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            double sum = 0;
            for (int i = 0; i < f_size; ++i) {
                int dx = -f_size / 2 + i;
                if ((x + dx < 0) || (x + dx >= size)) {
                    continue;
                }
                for (int j = 0; j < f_size; ++j) {
                    int dy = -f_size / 2 + j;
                    if ((y + dy < 0) || (y + dy >= size)) {
                        continue;
                    }
                    sum += matrix[x + dx][y + dy] * filter[i][j];
                }
            }
            if (fabs(sum - matrix[x][y]) > approx || matrix[x][y] < bound) {
                matrix[x][y] = sum;
            }
        }
    }

    vector<Coord> coords;
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if (matrix[x][y] < eps) {
                matrix[x][y] = 0;
                humidity[x][y] = MAX_HUMIDITY;
                coords.push_back(Coord(x, y));
            }
        }
    }

    // Calculating shadows
    for (int x = 0; x < size; ++x) {
        // for every row
        vector<double> coeffs(3, 0);
        coeffs[2] = 1;
        bool searching = false;
        double max = -10;
        int max_coord = 0;
        for (int y = size - 1; y >= 0; --y) {
            /*
              ��� ���-�� ����... =)
            if (searching) { // if we are serching for a new pixel that is lighted up
                if (above_line(coeffs, y, matrix[x][y])) {
                    searching = false; // found it
                    max = matrix[x][y]; // set local maximum
                    max_coord = y;
                    shadows[x][y] = 0;
                } else {
                    shadows[x][y] = SHADOW_MAX; // still in shadow
                }
            } else if (matrix[x][y] < max + eps) { // if new pixel is lower than local maximum
                coeffs = get_line_coefficients(max_coord, max, light_pos[1] - y, light_pos[2]);
                if (!above_line(coeffs, y, matrix[x][y])) {
                    shadows[x][y] = SHADOW_MAX; // it is in shadow
                    searching = true; //and we have to find another lighted pixel
                }
            } else {
                // ������ ����� ��-�� �����
                shadows[x][y] = -SHADOW_MAX;
                max = matrix[x][y]; // new local maximum
                max_coord = y;
            }
            */
            if (above_line(coeffs, y, matrix[x][y])) {
                shadows[x][y] = -SHADOW_MAX;
                coeffs = get_line_coefficients(y, matrix[x][y], light_pos[1] - y, light_pos[2]);
            } else {
                shadows[x][y] = SHADOW_MAX;
            }
        }
    }
    // Smoothing shadows with gaussian blur
    f_size = 5;
    sigma = 1;
    filter = generate_filter(sigma, f_size);
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            double sum = 0;
            for (int i = 0; i < f_size; ++i) {
                int dx = -f_size / 2 + i;
                if ((x + dx < 0) || (x + dx >= size)) {
                    continue;
                }
                for (int j = 0; j < f_size; ++j) {
                    int dy = -f_size / 2 + j;
                    if ((y + dy < 0) || (y + dy >= size)) {
                        continue;
                    }
                    sum += shadows[x + dx][y + dy] * filter[i][j];
                }
            }
            shadows[x][y] = sum;
        }
    }

    // Calculating humidity
    for (int i = 0; i < NUM_RAND_POINTS_PER_SIZE * size; ++i) {
        int x = next_rand(size - 1);
        int y = next_rand(size - 1);
        humidity[x][y] = next_rand(MIN_HUMIDITY, MAX_HUMIDITY);
        coords.push_back(Coord(x, y));
    }
    for (int i = 0; i < coords.size(); ++i) {
        int x = coords[i].first;
        int y = coords[i].second;
        flow_humidity(x, y, humidity[x][y]);
    }
}

void
Terrain::generate(
        vector<vector<double> > &matrix,
        int size, int x_shift, int y_shift, double coeff)
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
            max(-3.0, (corners[0] + corners[1]) / 2.0 +
                size * coeff * next_rand() * ROUGH);
    matrix[x_shift + med][y_shift + size - 1] =
            max(-3.0, (corners[2] + corners[3]) / 2.0 +
                size * coeff * next_rand() * ROUGH);
    matrix[x_shift][y_shift + med] =
            max(-3.0, (corners[0] + corners[2]) / 2.0 +
                size * coeff * next_rand() * ROUGH);
    matrix[x_shift + size - 1][y_shift + med] =
            max(-3.0, (corners[1] + corners[3]) / 2.0 +
                size * coeff * next_rand() * ROUGH);

    double one = (corners[0] + corners[1] + corners[2] + corners[3]) / 4.0;
    double two = size * coeff * next_rand() * ROUGH;
    //cerr << size << " " << med << endl;
    //cerr << "[" << corners[0] << ", " << corners[1] << ", " << corners[2] << ", " << corners[3] << "]" << endl;
    //cerr << one << " " << two << endl;
    matrix[x_shift + med][y_shift + med] =
            max(-3.0, one + two);
    //return;
    generate(matrix, (size + 1) / 2, x_shift, y_shift, coeff);
    generate(matrix, (size + 1) / 2, x_shift + med, y_shift, coeff);
    generate(matrix, (size + 1) / 2, x_shift, y_shift + med, coeff);
    generate(matrix, (size + 1) / 2, x_shift + med, y_shift + med, coeff);
}

void
Terrain::flow_humidity(int x, int y, double val)
{
    double cur_val = val;
    double shift = 0.1;
    humidity[x][y] = val;
    queue<Coord> coords;
    coords.push(Coord(x, y));
    while(!coords.empty()) {
        Coord cur = coords.front();
        coords.pop();
        cur_val = humidity[cur.first][cur.second] - shift;
        if (cur_val < eps) {
            continue;
        }
        vector<Coord> neighbours = get_neighbours(cur.first, cur.second, size);
        for (int i = 0; i < neighbours.size(); ++i) {
            int x = neighbours[i].first;
            int y = neighbours[i].second;
            if (humidity[x][y] < cur_val) {
                humidity[x][y] = cur_val;
                coords.push(neighbours[i]);
            }
        }
    }
}

vector<float>
Terrain::generate_line(int num) const
{
    vector<vector<float> > line(size * 2, vector<float>(3, 0));

    for (int i = 0; i < size; ++i) {
        line[2 * i][0] = i;
        line[2 * i][1] = num;
        line[2 * i][2] = matrix[i][num];

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
    /*
      ���� �� �������� ������-��...
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if (matrix[x][y] > eps) {
                local_p[x * size + y][0] = x;
                local_p[x * size + y][1] = y;
                local_p[x * size + y][2] = matrix[x][y];
                local_c[x * size + y] = get_color(matrix[x][y], humidity[x][y]);
            }
        }
    }
    */
    for (int x = 0; x < size - 1; ++x) {
        for (int y = 0; y < size - 1; ++y) {
            vector<float> a(3), b(3), c(3), d(3);
            vector<float> n(3);
            vector<unsigned char> clr(3);
            a[0] = x; a[1] = y; a[2] = matrix[x][y];
            b[0] = x + 1; b[1] = y; b[2] = matrix[x + 1][y];
            c[0] = x; c[1] = y + 1; c[2] = matrix[x][y + 1];
            d[0] = x + 1; d[1] = y + 1; d[2] = matrix[x + 1][y + 1];
            if (a[2] > eps || b[2] > eps || d[2] > eps) {

                local_p[a[0] * size + a[1]] = a;
                local_p[b[0] * size + b[1]] = b;
                local_p[d[0] * size + d[1]] = d;

                clr = get_color(a[2], humidity[int(a[0])][int(a[1])]);
                if (shadows[int(a[0])][int(a[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                }
                local_c[a[0] * size + a[1]] = clr;
                clr = get_color(b[2], humidity[int(b[0])][int(b[1])]);
                if (shadows[int(b[0])][int(b[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                }
                local_c[b[0] * size + b[1]] = clr;
                clr = get_color(d[2], humidity[int(d[0])][int(d[1])]);
                if (shadows[int(d[0])][int(d[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                }
                local_c[d[0] * size + d[1]] = clr;

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

                clr = get_color(a[2], humidity[int(a[0])][int(a[1])]);
                if (shadows[int(a[0])][int(a[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                }
                local_c[a[0] * size + a[1]] = clr;
                clr = get_color(c[2], humidity[int(c[0])][int(c[1])]);
                if (shadows[int(c[0])][int(c[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                }
                local_c[c[0] * size + c[1]] = clr;
                clr = get_color(d[2], humidity[int(d[0])][int(d[1])]);
                if (shadows[int(d[0])][int(d[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                }
                local_c[d[0] * size + d[1]] = clr;

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
            vector<unsigned char> clr(4, 0);
            a[0] = x; a[1] = y; a[2] = matrix[x][y];
            b[0] = x + 1; b[1] = y; b[2] = matrix[x + 1][y];
            c[0] = x; c[1] = y + 1; c[2] = matrix[x][y + 1];
            d[0] = x + 1; d[1] = y + 1; d[2] = matrix[x + 1][y + 1];
            if (a[2] < eps && b[2] < eps && d[2] < eps) {
                local_p[a[0] * size + a[1]] = a;
                local_p[b[0] * size + b[1]] = b;
                local_p[d[0] * size + d[1]] = d;

                clr = get_water_color();
                if (shadows[int(a[0])][int(a[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[3] += SHADOW_NORM;
                }
                local_c[a[0] * size + a[1]] = clr;
                clr = get_water_color();
                if (shadows[int(b[0])][int(b[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[3] += SHADOW_NORM;
                }
                local_c[b[0] * size + b[1]] = clr;
                clr = get_water_color();
                if (shadows[int(d[0])][int(d[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[3] += SHADOW_NORM;
                }
                local_c[d[0] * size + d[1]] = clr;

                if (flags[a[0] * size + a[1]]) {
                    n = get_normal(a, b, d);
                    for (int i = 0; i < 3; ++i) {
                        n[i] += next_rand();
                    }
                    local_n[a[0] * size + a[1]] =
                            sum_vect(n, local_n[a[0] * size + a[1]]);
                } else {
                    local_n[a[0] * size + a[1]] = get_normal(a, b, d);
                    flags[a[0] * size + a[1]] = true;
                }
                if (flags[b[0] * size + b[1]]) {
                    n = get_normal(a, b, d);
                    for (int i = 0; i < 3; ++i) {
                        n[i] += next_rand();
                    }
                    local_n[b[0] * size + b[1]] =
                            sum_vect(n, local_n[b[0] * size + b[1]]);
                } else {
                    local_n[b[0] * size + b[1]] = get_normal(a, b, d);
                    flags[b[0] * size + b[1]] = true;
                }
                if (flags[d[0] * size + d[1]]) {
                    n = get_normal(a, b, d);
                    for (int i = 0; i < 3; ++i) {
                        n[i] += next_rand();
                    }
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

                clr = get_water_color();
                if (shadows[int(a[0])][int(a[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[3] += SHADOW_NORM;
                }
                local_c[a[0] * size + a[1]] = clr;
                clr = get_water_color();
                if (shadows[int(c[0])][int(c[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[3] += SHADOW_NORM;
                }
                local_c[c[0] * size + c[1]] = clr;
                clr = get_water_color();
                if (shadows[int(d[0])][int(d[1])] > SHADOW_BOUND) {
                    clr[0] = max(0, clr[0] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[1] = max(0, clr[1] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[2] = max(0, clr[2] / SHADOW_NORM);// * shadows[int(a[0])][int(a[1])]);
                    clr[3] += SHADOW_NORM;
                }
                local_c[d[0] * size + d[1]] = clr;

                if (flags[a[0] * size + a[1]]) {
                    n = get_normal(a, d, c);
                    for (int i = 0; i < 3; ++i) {
                        n[i] += next_rand();
                    }
                    local_n[a[0] * size + a[1]] =
                            sum_vect(n, local_n[a[0] * size + a[1]]);
                } else {
                    local_n[a[0] * size + a[1]] = get_normal(a, d, c);
                    flags[a[0] * size + a[1]] = true;
                }
                if (flags[c[0] * size + c[1]]) {
                    n = get_normal(a, d, c);
                    for (int i = 0; i < 3; ++i) {
                        n[i] += next_rand();
                    }
                    local_n[c[0] * size + c[1]] =
                            sum_vect(n, local_n[c[0] * size + c[1]]);
                } else {
                    local_n[c[0] * size + c[1]] = get_normal(a, d, c);
                    flags[c[0] * size + c[1]] = true;
                }
                if (flags[d[0] * size + d[1]]) {
                    n = get_normal(a, d, c);
                    for (int i = 0; i < 3; ++i) {
                        n[i] += next_rand();
                    }
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

Terrain::~Terrain() {}
