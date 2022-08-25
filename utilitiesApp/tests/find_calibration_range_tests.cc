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

    class CalRangeTest : public ::testing::Test {
    protected:
        aSubRecord prec;
        double valaArray[1] = { 10000.0 };
        double valbArray[1] = { 0.0 };
        const char fileName[22] = "calibration_range.txt";

        void setupASubRecord()
        {
            strcpy(prec.name, "name");
            prec.fta = menuFtypeSTRING;
            prec.ftb = menuFtypeSTRING;
            prec.ftc = menuFtypeSTRING;
            prec.a = ".";
            prec.b = ".";
            prec.c = (void*)fileName;
            prec.ftva = menuFtypeDOUBLE;
            prec.ftvb = menuFtypeDOUBLE;
            prec.vala = valaArray;
            prec.valb = valbArray;
        }

        void createCalibrationFile(std::string data = "0.0\n10000.0") {
            std::FILE* pFile;
            pFile = fopen(fileName, "w");

            if (pFile != NULL) {
                std::fputs(data.c_str(), pFile);
                std::fclose(pFile);
            }
        }

        void SetUp() override {
            memset(&prec, 0, sizeof(prec));
            setupASubRecord();
        }

        void TearDown() override {
            std::remove(fileName);
        }
    };

    TEST_F(CalRangeTest, reads_in_calibration_range) {
        // Given
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 0);
        ASSERT_EQ((static_cast<double*>(prec.vala))[0], 10000);
        ASSERT_EQ((static_cast<double*>(prec.valb))[0], 0);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, handles_incorrect_file_location) {
        // Given
        prec.c = "Wrong.txt";
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_empty_file) {
        // Given
        createCalibrationFile("");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_empty_only_one_line) {
        // Given
        createCalibrationFile("0.0");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_empty_first_line) {
        // Given
        createCalibrationFile("\n10000.0");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_empty_second_line) {
        // Given
        createCalibrationFile("0.0\n");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_non_numbers) {
        // Given
        createCalibrationFile("test\ntest");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_incorrect_arg_type_fta) {
        // Given
        prec.fta = menuFtypeDOUBLE;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_incorrect_arg_type_ftb) {
        // Given
        prec.ftb = menuFtypeDOUBLE;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_incorrect_arg_type_ftc) {
        // Given
        prec.ftc = menuFtypeDOUBLE;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_incorrect_arg_type_ftva) {
        // Given
        prec.ftva = menuFtypeSTRING;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }

    TEST_F(CalRangeTest, reject_incorrect_arg_type_ftvb) {
        // Given
        prec.ftvb = menuFtypeSTRING;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);

        // Clean up
        std::remove(fileName);
    }
}