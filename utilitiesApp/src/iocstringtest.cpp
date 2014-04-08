#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <exception>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <time.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>

#include "epicsStdlib.h"
#include "epicsString.h"
#include "dbDefs.h"
#include "epicsMutex.h"
#include "dbBase.h"
#include "dbStaticLib.h"
#include "dbFldTypes.h"
#include "dbCommon.h"
#include "dbAccessDefs.h"
#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <iocsh.h>
#include "envDefs.h"
#include "macLib.h"
#include "errlog.h"

#include "postfix.h"
#include "cvtFast.h"

#include <boost/scoped_array.hpp>

#include <string.h>
#include <registryFunction.h>

#include <epicsExport.h>

#include "utilities.h"

// return ' ' on true, '#' on false
// operation
// 0x1: verbose
// 0x2: strlen(lhs) > 0
// 0x4: lhs == rhs
static void iocstringtest(const char* resultvar, const char* lhs, int operation, const char* rhs)
{
    if (resultvar == NULL || lhs == NULL)
	{
	    errlogPrintf("iocstringtest: ERROR: NULL args");
		return;
	}
	bool verbose = (operation & 0x1);
    char* lhs_expand = macEnvExpand(lhs);
	if (lhs_expand == NULL)
	{
	    errlogPrintf("iocstringtest: ERROR: NULL expanded expression arg");
		return;
	}
	if (verbose)
	{
	    printf("iocstringtest: expanded expression=\"%s\"\n", lhs_expand);
	}
	long result = 0;
    if (strlen(lhs_expand) > 0)
    {
        result = 1;
    }
	char result_str[32];
	if ( sprintf(result_str, "%s", (result != 0 ? " " : "#")) < 0 )
    {
		errlogPrintf("iocstringtest: ERROR: sprintf (hash mode)\n");
		return;
	}
	if (verbose)
	{
	    printf("iocstringtest: setting %s=\"%s\" (%ld)\n", resultvar, result_str, result);
	}
	epicsEnvSet(resultvar, result_str);
	free(lhs_expand);
}

extern "C" {

// EPICS iocsh shell commands 

// calc "result" "expression"
static const iocshArg stringtestInitArg0 = { "resultvar", iocshArgString };
static const iocshArg stringtestInitArg1 = { "lhs", iocshArgString };
static const iocshArg stringtestInitArg2 = { "operation", iocshArgInt };
static const iocshArg stringtestInitArg3 = { "rhs", iocshArgString };
static const iocshArg * const stringtestInitArgs[] = { &stringtestInitArg0, &stringtestInitArg1, &stringtestInitArg2, &stringtestInitArg3 };

static const iocshFuncDef stringtestInitFuncDef = {"stringtest", sizeof(stringtestInitArgs) / sizeof(iocshArg*), stringtestInitArgs};

static void stringtestInitCallFunc(const iocshArgBuf *args)
{
    iocstringtest(args[0].sval, args[1].sval, args[2].ival, args[3].sval);
}

static void iocstringtestRegister(void)
{
    iocshRegister(&stringtestInitFuncDef, stringtestInitCallFunc);
}

epicsExportRegistrar(iocstringtestRegister); // need to be declared via registrar() in utilities.dbd too

// asub callable functions - need to be in utilities.dbd as function() 

//epicsRegisterFunction(setIOCName); 
//epicsRegisterFunction(getIOCName); 
//epicsRegisterFunction(getIOCGroup); 

}

