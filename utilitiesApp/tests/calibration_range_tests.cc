#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <epicsString.h>

#include "gtest/gtest.h"
#include "find_calibration_range_utils.h"

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

    TEST(ConvertEpicsOldString, GIVEN_an_epics_old_string_THEN_it_is_converted_to_a_std_string) {
        // Given
        epicsOldString epics_old_string = "This is an EPICS old string";
        std::string expected = "This is an EPICS old string";

        // When:
        std::string result = str_from_epics((void*)(&epics_old_string));

        // Then:
        ASSERT_EQ(result, expected);
    }    
    
    TEST(GetCalibrationFilePath, GIVEN_an_collection_of_directories_THEN_they_are_converted_to_an_absolute_path) {
        // Given
        epicsOldString BDIR = "C:/Instrument/Settings";
        epicsOldString TDIR = "config/common/temps";
        epicsOldString SPEC = "None.txt";
        std::string expected = "C:/Instrument/Settings/config/common/temps/None.txt";

        // When:
        std::string result = find_file((void*)(&BDIR), (void*)(&TDIR), (void*)(&SPEC));

        // Then:
        ASSERT_EQ(result, expected);
    }
} // namespace
