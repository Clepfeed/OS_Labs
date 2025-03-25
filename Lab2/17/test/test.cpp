#include <gtest/gtest.h>
#include "../main.h"

// Тест для функции вычисления среднего значения
TEST(AverageTest, BasicTest) {
    std::vector<int> test_vec = { 1, 2, 3, 4, 5 };
    Avg avg_data{ test_vec, 0 };

    average(&avg_data);

    EXPECT_EQ(avg_data.avg, 3);
}

TEST(AverageTest, EmptyVector) {
    std::vector<int> test_vec;
    Avg avg_data{ test_vec, 0 };

    average(&avg_data);
    EXPECT_EQ(avg_data.avg, 0);
}

TEST(AverageTest, SingleElement) {
    std::vector<int> test_vec = { 5 };
    Avg avg_data{ test_vec, 0 };

    average(&avg_data);

    EXPECT_EQ(avg_data.avg, 5);
}

// Тесты для функции поиска минимума и максимума
TEST(MinMaxTest, BasicTest) {
    std::vector<int> test_vec = { 1, 2, 3, 4, 5 };
    MinMax minmax_data{ test_vec, 0, 0 };

    min_max(&minmax_data);

    EXPECT_EQ(minmax_data.min, 1);
    EXPECT_EQ(minmax_data.max, 5);
}

TEST(MinMaxTest, EmptyVector) {
    std::vector<int> test_vec;
    MinMax minmax_data{ test_vec, 0, 0 };

    min_max(&minmax_data);

    EXPECT_EQ(minmax_data.min, 0);
    EXPECT_EQ(minmax_data.max, 0);
}

TEST(MinMaxTest, SingleElement) {
    std::vector<int> test_vec = { 5 };
    MinMax minmax_data{ test_vec, 0, 0 };

    min_max(&minmax_data);

    EXPECT_EQ(minmax_data.min, 5);
    EXPECT_EQ(minmax_data.max, 5);
}

TEST(MinMaxTest, NegativeNumbers) {
    std::vector<int> test_vec = { -5, -3, -8, -1, -6 };
    MinMax minmax_data{ test_vec, 0, 0 };

    min_max(&minmax_data);

    EXPECT_EQ(minmax_data.min, -8);
    EXPECT_EQ(minmax_data.max, -1);
}