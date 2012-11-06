#include "Terrain.h"

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
            (corners[0] + corners[1]) / 2.0 + size * next_rand();
    matrix[x_shift + med][y_shift + size - 1] =
            (corners[2] + corners[3]) / 2.0 + size * next_rand();
    matrix[x_shift][y_shift + med] =
            (corners[0] + corners[2]) / 2.0 + size * next_rand();
    matrix[x_shift + size - 1][y_shift + med] =
            (corners[1] + corners[3]) / 2.0 + size * next_rand();

    matrix[x_shift + med][y_shift + med] =
            (matrix[x_shift + med][y_shift] + matrix[x_shift + med][y_shift + size - 1] +
            matrix[x_shift][y_shift + med] + matrix[x_shift + size - 1][y_shift + med]) / 4.0 +
            size * next_rand();

    generate(matrix, (size + 1) / 2, x_shift, y_shift);
    generate(matrix, (size + 1) / 2, x_shift + med, y_shift);
    generate(matrix, (size + 1) / 2, x_shift, y_shift + med);
    generate(matrix, (size + 1) / 2, x_shift + med, y_shift + med);
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
