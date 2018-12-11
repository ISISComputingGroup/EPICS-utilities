#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <epicsExport.h>

static long find_calibration_range(aSubRecord *prec)
{
    /* Either call a C++ function (see below) or implement logic here. */
    return 0;
}

epicsRegisterFunction(find_calibration_range);