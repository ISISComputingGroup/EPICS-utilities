#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <epicsExport.h>

#include "find_calibration_range_impl.h"

static long find_calibration_range(aSubRecord *prec)
{
    return find_calibration_range_impl(prec);
}

epicsRegisterFunction(find_calibration_range);
