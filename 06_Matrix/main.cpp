#include <cassert>
#include <iostream>

#include "Matrix.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    Matrix<int, 0> matrix;

    for (size_t i = 0; i < 10; ++i) {
        matrix[i][i] = i;
        matrix[i][9 - i] = 9 - i;
    }

    for (size_t i = 1; i < 9; ++i) {
        for (size_t j = 1; j < 9; ++j) {
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << "size: " << matrix.size() << std::endl;

    for ([[maybe_unused]] auto c : matrix) {
        int x{};
        int y{};
        int v{};
        std::tie(x, y, v) = c;
        std::cout << x << y << v << std::endl;
    }

    return 0;
}