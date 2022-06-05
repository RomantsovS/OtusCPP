#include <gtest/gtest.h>

#include <sstream>

#include "Matrix.h"

TEST(TestAll, TestMatrix) {
    {
        // бесконечная матрица int заполнена значениями -1
        Matrix<int, -1> matrix;
        EXPECT_EQ(matrix.size(), 0);  // все ячейки свободны

        auto a = matrix[0][0];
        EXPECT_EQ(a, -1);
        EXPECT_EQ(matrix.size(), 0);

        matrix[100][100] = 314;
        EXPECT_EQ(matrix[100][100], 314);
        EXPECT_EQ(matrix.size(), 1);

        std::ostringstream os;

        // выведется одна строка
        // 100100314
        for ([[maybe_unused]] auto c : matrix) {
            int x{};
            int y{};
            int v{};
            std::tie(x, y, v) = c;
            os << x << y << v << std::endl;
        }

        const std::string expected = R"(100100314
)";

        EXPECT_EQ(os.str(), expected);

        ((matrix[100][100] = 314) = 0) = 217;

        EXPECT_EQ(matrix[100][100], 217);
    }
    {
        Matrix<int, 0> matrix;

        for (size_t i = 0; i < 10; ++i) {
            matrix[i][i] = i;
        }
        EXPECT_EQ(matrix.size(), 9);
        for (size_t i = 0; i < 10; ++i) {
            matrix[i][i] = 0;
        }
        EXPECT_EQ(matrix.size(), 0);
    }
}