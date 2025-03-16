#include <gtest/gtest.h>
#include "../src/main/main.h"
#include <sstream>

class MainTest : public ::testing::Test {
protected:
    void SetUp() override {
        binaryFilename = "test_display.bin";
        textFilename = "test_display.txt";
    }

    void TearDown() override {
        std::remove(binaryFilename.c_str());
        std::remove(textFilename.c_str());
    }

    void createTestBinaryFile() {
        std::ofstream file(binaryFilename, std::ios::binary);
        employee emp{ 1, "Test", 40 };
        file.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
        file.close();
    }

    void createTestTextFile() {
        std::ofstream file(textFilename);
        file << "Test line 1\n";
        file << "Test line 2\n";
        file.close();
    }

    std::string binaryFilename;
    std::string textFilename;
};

TEST_F(MainTest, DisplayBinaryFileNonexistent) {
    std::stringstream output;
    displayBinaryFile("nonexistent.bin", output);
    EXPECT_TRUE(output.str().find("Error") != std::string::npos);
}

TEST_F(MainTest, DisplayBinaryFileValid) {
    createTestBinaryFile();
    std::stringstream output;
    displayBinaryFile(binaryFilename, output);
    std::string result = output.str();
    EXPECT_TRUE(result.find("Test") != std::string::npos);
    EXPECT_TRUE(result.find("40") != std::string::npos);
}

TEST_F(MainTest, DisplayTextFileNonexistent) {
    std::stringstream output;
    displayTextFile("nonexistent.txt", output);
    EXPECT_TRUE(output.str().find("Error") != std::string::npos);
}

TEST_F(MainTest, DisplayTextFileValid) {
    createTestTextFile();
    std::stringstream output;
    displayTextFile(textFilename, output);
    std::string result = output.str();
    EXPECT_TRUE(result.find("Test line 1") != std::string::npos);
    EXPECT_TRUE(result.find("Test line 2") != std::string::npos);
}

TEST_F(MainTest, StartProcessInvalid) {
    EXPECT_FALSE(startProcess("nonexistent.exe"));
}

TEST_F(MainTest, StartProcessValid) {
    // Этот тест может потребовать модификации в зависимости от вашего окружения
    EXPECT_TRUE(startProcess("cmd.exe /c echo test"));
}