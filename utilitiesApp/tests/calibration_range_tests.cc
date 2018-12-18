#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <stdio.h>

#include "gtest\gtest.h"
#include "..\src\find_calibration_range_utils.h"

namespace {

    TEST(CalibrationReadingTest, reads_in_line_with_no_commas) {
        // Given
        std::string line = "This is a line with no commas";
        std::string filename = "calibration.txt";

        std::FILE * pFile;
        pFile = fopen("calibration.txt", "w");

        if (pFile != NULL) {
            std::fputs(line.c_str(), pFile);
            std::fclose(pFile);
        }
        std::ifstream file;
        std::vector<std::string> parsed_line;

        file.open(filename);
        if (file.is_open()) {
            // When:
            parsed_line = getNextLineAndSplitOnComma(file);
            file.close();
        }
        else {
            FAIL() << "Did not open calibration file.";
        }

        // Then:
        ASSERT_EQ(parsed_line.size(), 1);
        ASSERT_EQ(parsed_line[0], line);

        // Clean up
        std::remove(filename.c_str());
    }

    TEST(CalibrationReadingTest, reads_in_line_with_one_comma) {
        // Given
        std::string first_entry = "This is a line";
        std::string second_entry = " with one comma";
        std::string line = first_entry + "," + second_entry;
        std::string filename = "calibration.txt";

        std::FILE * pFile;
        pFile = fopen("calibration.txt", "w");

        if (pFile != NULL) {
            std::fputs(line.c_str(), pFile);
            std::fclose(pFile);
        }
        std::ifstream file;
        std::vector<std::string> parsed_line;

        file.open(filename);
        if (file.is_open()) {
            // When:
            parsed_line = getNextLineAndSplitOnComma(file);
            file.close();
        }
        else {
            FAIL() << "Did not open calibration file.";
        }

        // Then:
        ASSERT_EQ(parsed_line.size(), 2);
        ASSERT_EQ(parsed_line[0], first_entry);
        ASSERT_EQ(parsed_line[1], second_entry);

        // Clean up
        std::remove(filename.c_str());
    }

    TEST(CalibrationReadingTest, reads_empty_file) {
        // Given
        std::string line = "";
        std::string filename = "calibration.txt";

        std::FILE * pFile;
        pFile = fopen("calibration.txt", "w");

        if (pFile != NULL) {
            std::fputs(line.c_str(), pFile);
            std::fclose(pFile);
        }
        std::ifstream file;
        std::vector<std::string> parsed_line;

        file.open(filename);
        if (file.is_open()) {
            // When:
            parsed_line = getNextLineAndSplitOnComma(file);
            file.close();
        }
        else {
            FAIL() << "Did not open calibration file.";
        }

        // Then:
        ASSERT_EQ(parsed_line.size(), 1);
        ASSERT_EQ(parsed_line[0], line);

        // Clean up
        std::remove(filename.c_str());
    }
} // namespace
