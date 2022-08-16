#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <epicsString.h>
#include <menuFtype.h>
#include <errlog.h>

#include "gtest/gtest.h"
#include "find_calibration_range_impl.h"
#include "find_calibration_range_utils.h"

namespace {

    double valaArray[1] = { 10000.0 };
    double valbArray[1] = { 0.0 };
	
	static void setupASubRecord(aSubRecord* prec)
    {
        strcpy(prec->name, "name");
        prec->fta = menuFtypeSTRING;
        prec->ftb = menuFtypeSTRING;
        prec->ftc = menuFtypeSTRING;
        prec->a = ".";
        prec->b = ".";
        prec->c = "calibration_range.txt";
        prec->ftva = menuFtypeDOUBLE;
        prec->ftvb = menuFtypeDOUBLE;
        prec->vala = valaArray;
        prec->valb = valbArray;
    }

    static void createCalibrationFile(std::string data = "0.0\n10000.0") {
        std::FILE* pFile;
        pFile = fopen("calibration_range.txt", "w");

        if (pFile != NULL) {
            std::fputs(data.c_str(), pFile);
            std::fclose(pFile);
        }
    }

    TEST(FindCalibrationRangeTest, reads_in_calibration_range) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);

        createCalibrationFile();
        
        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 0);
        ASSERT_EQ((static_cast<double*>(prec.vala))[0], 10000);
        ASSERT_EQ((static_cast<double*>(prec.valb))[0], 0);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, handles_incorrect_file_location) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.c = "Wrong.txt";

        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_empty_file) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);

        createCalibrationFile("");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_empty_only_one_line) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);

        createCalibrationFile("0.0");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{ "calibration_range.txt" }.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_empty_first_line) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);

        createCalibrationFile("\n10000.0");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{ "calibration_range.txt" }.c_str());
    }
	
	TEST(FindCalibrationRangeTest, reject_empty_second_line) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);

        createCalibrationFile("0.0\n");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_non_numbers) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);

        createCalibrationFile("test\ntest");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{ "calibration_range.txt" }.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_incorrect_arg_type_fta) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.fta = menuFtypeDOUBLE;

        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_incorrect_arg_type_ftb) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftb = menuFtypeDOUBLE;

        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_incorrect_arg_type_ftc) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftc = menuFtypeDOUBLE;

        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_incorrect_arg_type_ftva) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftva = menuFtypeSTRING;

        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

    TEST(FindCalibrationRangeTest, reject_incorrect_arg_type_ftvb) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftvb = menuFtypeSTRING;

        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(std::string{"calibration_range.txt"}.c_str());
    }

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
