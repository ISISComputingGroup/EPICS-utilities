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
        std::vector<std::vector<std::string>> parsed_file;

        file.open(filename);
        if (file.is_open()) {
            // When:
            parsed_file.push_back(getNextLineAndSplitOnComma(file));
            file.close();
        }
        else {
            FAIL() << "Did not open calibration file.";
        }

        // Then:
        ASSERT_EQ(parsed_file.size(), 1);
        ASSERT_EQ(parsed_file[0].size(), 1);
        ASSERT_EQ(parsed_file[0][0], line);

        // Clean up
        std::remove(filename.c_str());
    }
} // namespace
