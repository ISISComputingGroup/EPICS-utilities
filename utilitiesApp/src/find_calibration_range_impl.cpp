#include <string.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>
#include <epicsExport.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "find_calibration_range_impl.h"


long find_calibration_range_impl(aSubRecord *prec)
{
    /* Returns the first input value back */
    // Check for input types
    if (prec->fta != menuFtypeSTRING)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument type A", prec->name);
        return 1;
    }
    if (prec->ftb != menuFtypeSTRING)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument type B", prec->name);
        return 1;
    }
    if (prec->ftc != menuFtypeSTRING)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument type C", prec->name);
        return 1;
    }
    try {
        // Load file
        std::string calibration_file_location = find_file(prec->a, prec->b, prec->c);
        std::ifstream calibration_file;
        calibration_file.open(calibration_file_location);

        std::vector<std::vector<std::string>> lines;

        // If the file is open
        if (calibration_file.is_open())
        {
            // Iterate over the lines to the end
            while (calibration_file.peek() != EOF)
            {
                lines.push_back(getNextLineAndSplitOnComma(calibration_file));
            }
            // Close file
            calibration_file.close();
        }

        //  Get highest and lowest values in the calibration file
        double low_limit = std::stod(lines[0][0]);
        double high_limit = std::stod(lines[lines.size() - 1][0]);

        // Set the output values
        // Check the types of output values
        if (prec->ftva != menuFtypeDOUBLE) {
            errlogSevPrintf(errlogMajor, "%s incorrect output argument type A", prec->name);
            return 1;
        }
        if (prec->ftvb != menuFtypeDOUBLE) {
            errlogSevPrintf(errlogMajor, "%s incorrect output argument type B", prec->name);
            return 1;
        }

        // Set the values to the output PVs
        (static_cast<double*>(prec->vala))[0] = high_limit;
        (static_cast<double*>(prec->valb))[0] = low_limit;
    }
    catch (std::exception& e) {
        errlogSevPrintf(errlogMajor, "%s exception: %s", prec->name, e.what());
        return 1;
    }
    return 0; /* process output links */
}

static std::vector<std::string> getNextLineAndSplitOnComma(std::ifstream& str) {
    std::vector<std::string>   parsed_line;
    std::string                line;
    std::getline(str, line);

    std::stringstream          lineStream(line);
    std::string                cell;

    while (std::getline(lineStream, cell, ','))
    {
        parsed_line.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        parsed_line.push_back("");
    }
    return parsed_line;
}

/**
* Extracts a std::string from an epics aSubRecord.
*/

static std::string str_from_epics(void* raw_rec) {
    epicsOldString* rec = reinterpret_cast<epicsOldString*>(raw_rec);
    char buffer[sizeof(epicsOldString) + 1];  // +1 for null terminator in the case where epics str is exactly 40 chars (unterminated)
    buffer[sizeof(epicsOldString)] = '\0';
    return std::string(strncpy(buffer, *rec, sizeof(epicsOldString)));
}


// Finds the directory to the table file
static std::string find_file(void* BDIR, void* TDIR, void* SPEC) {
    std::string base_directory = str_from_epics(BDIR);
    std::string table_directory = str_from_epics(TDIR);
    std::string filename = str_from_epics(SPEC);

    boost::trim(base_directory);
    boost::trim(table_directory);
    boost::trim(filename);

    return  base_directory + "/" + table_directory + "/" + filename;
}
