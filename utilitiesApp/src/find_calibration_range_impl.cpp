#include <string.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>
#include <epicsExport.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "find_calibration_range_impl.h"
#include "find_calibration_range_utils.h"


long find_calibration_range_impl(aSubRecord *prec) {
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
