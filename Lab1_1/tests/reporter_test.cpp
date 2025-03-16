#include <gtest/gtest.h>
#include "../src/reporter/reporter.h"
#include <sstream>

class ReporterTest : public ::testing::Test {
protected:
    void SetUp() override {
        binaryFilename = "test_employees.bin";
        reportFilename = "test_report.txt";
    }

    void TearDown() override {
        std::remove(binaryFilename.c_str());
        std::remove(reportFilename.c_str());
    }

    // Вспомогательная функция для создания тестового бинарного файла
    void createTestBinaryFile(const std::vector<employee>& employees) {
        std::ofstream file(binaryFilename, std::ios::binary);
        for (const auto& emp : employees) {
            file.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
        }
        file.close();
    }

    std::string binaryFilename;
    std::string reportFilename;
};

TEST_F(ReporterTest, ReadEmptyFile) {
    auto employees = readEmployeesFromFile("nonexistent.bin");
    EXPECT_TRUE(employees.empty());
}

TEST_F(ReporterTest, ReadValidFile) {
    std::vector<employee> testData = {
        {1, "John", 40},
        {2, "Alice", 35}
    };
    createTestBinaryFile(testData);

    auto employees = readEmployeesFromFile(binaryFilename);
    ASSERT_EQ(employees.size(), 2);
    EXPECT_EQ(employees[0].num, 1);
    EXPECT_STREQ(employees[0].name, "John");
    EXPECT_EQ(employees[0].hours, 40);
}

TEST_F(ReporterTest, GenerateReportNegativeRate) {
    EXPECT_FALSE(generateReport(binaryFilename, reportFilename, -1.0));
}

TEST_F(ReporterTest, GenerateReportZeroRate) {
    EXPECT_FALSE(generateReport(binaryFilename, reportFilename, 0.0));
}

TEST_F(ReporterTest, GenerateValidReport) {
    std::vector<employee> testData = {
        {1, "John", 40},
        {2, "Alice", 35}
    };
    createTestBinaryFile(testData);

    EXPECT_TRUE(generateReport(binaryFilename, reportFilename, 10.0));

    std::ifstream report(reportFilename);
    ASSERT_TRUE(report.is_open());

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(report, line)) {
        lines.push_back(line);
    }

    ASSERT_GE(lines.size(), 3);  // Заголовок + строка с колонками + минимум 1 запись
    EXPECT_TRUE(lines[0].find("test_employees.bin") != std::string::npos);
}