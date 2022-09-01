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

    class FindCalibrationRangeTest : public ::testing::Test {
    protected:
        aSubRecord prec;
        double valaArray[1] = { 10000.0 };
        double valbArray[1] = { 0.0 };
        const char* fileName = "calibration_range.txt";

        void setupASubRecord()
        {
            strcpy(prec.name, "name");
            prec.fta = menuFtypeSTRING;
            prec.ftb = menuFtypeSTRING;
            prec.ftc = menuFtypeSTRING;
            prec.a = (void*)".";
            prec.b = (void*)".";
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

    TEST_F(FindCalibrationRangeTest, test_GIVEN_valid_calibration_file_WHEN_file_read_THEN_range_set) {
        // Given
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 0);
        ASSERT_EQ((static_cast<double*>(prec.vala))[0], 10000);
        ASSERT_EQ((static_cast<double*>(prec.valb))[0], 0);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_calibration_file_path_wrong_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        prec.c = "Wrong.txt";
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_Calibration_file_empty_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        createCalibrationFile("");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_calibration_file_has_one_line_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        createCalibrationFile("0.0");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_calibration_file_missing_first_value_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        createCalibrationFile("\n10000.0");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_calibration_file_missing_second_value_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        createCalibrationFile("0.0\n");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_calibration_file_values_not_numbers_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        createCalibrationFile("test\ntest");

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_incorrect_arg_type_fta_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        prec.fta = menuFtypeDOUBLE;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_incorrect_arg_type_ftb_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        prec.ftb = menuFtypeDOUBLE;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_incorrect_arg_type_ftc_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        prec.ftc = menuFtypeDOUBLE;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_incorrect_arg_type_ftva_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        prec.ftva = menuFtypeSTRING;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }

    TEST_F(FindCalibrationRangeTest, test_GIVEN_incorrect_arg_type_ftvb_WHEN_file_read_THEN_error_status_is_returned) {
        // Given
        prec.ftvb = menuFtypeSTRING;
        createCalibrationFile();

        // When:
        long status = find_calibration_range_impl(&prec);

        // Then:
        ASSERT_EQ(status, 1);
    }
}
