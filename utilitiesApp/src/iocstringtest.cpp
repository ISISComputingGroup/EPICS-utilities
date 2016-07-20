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

// operation
// 0x1: verbose
// 0x2: strlen(lhs) > 0
// 0x4: lhs == rhs
// 0x8: reverse the above logic
static int stringtestres(const char* lhs, int operation, const char* rhs)
{
    if (lhs == NULL)
	{
	    errlogPrintf("iocstringtest: ERROR: NULL lhs arg");
		return 0;
	}
	bool verbose = (operation & 0x1);
    bool reverse = (operation & 0x8 ? true : false);
    char* lhs_expand = macEnvExpand(lhs);
    char* rhs_expand = (rhs != NULL ? macEnvExpand(rhs) : NULL);
	if (lhs_expand == NULL)
	{
	    errlogPrintf("iocstringtest: ERROR: NULL expanded lhs expression arg");
		return 0;
	}
	if (verbose)
	{
	    printf("iocstringtest: expanded expression=\"%s\"\n", lhs_expand);
	}
	long result = 0;
	if (operation & 0x2)
	{
        if (strlen(lhs_expand) > 0)
        {
            result = 1;
        }
	}
	else if (operation & 0x4)
	{
		if (rhs_expand != NULL)
		{
		    result = (strcmp(lhs_expand, rhs_expand) == 0 ? 1 : 0);
		}
		else
		{
	        errlogPrintf("iocstringtest: ERROR: NULL expanded rhs expression arg");
		}
	}
    if (reverse)
    {
        result = (result == 0 ? 1 : 0);
    }
	free(lhs_expand);
	if (rhs_expand != NULL)
	{
	    free(rhs_expand);
	}
	return result;
}

// return ' ' on true, '#' on false
// operation
// 0x1: verbose
// 0x2: strlen(lhs) > 0
// 0x4: lhs == rhs
// 0x8: reverse the above logic
static void iocstringtest(const char* resultvar, const char* lhs, int operation, const char* rhs)
{
    if (resultvar == NULL)
	{
	    errlogPrintf("iocstringtest: ERROR: NULL args");
		return;
	}
	bool verbose = (operation & 0x1);
    int result = stringtestres(lhs, operation, rhs);
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
}

// operation
// 0x1: verbose
// 0x2: strlen(lhs) > 0
// 0x4: lhs == rhs
// 0x8: reverse the above logic
static void iocstringiftest(const char* resultvar, const char* lhs, int operation, const char* rhs)
{
    if (resultvar == NULL)
	{
	    errlogPrintf("iocstringtest: ERROR: NULL args");
		return;
	}
	bool verbose = (operation & 0x1);
    int result = stringtestres(lhs, operation, rhs);
    char resultIfVar[128];
    char resultNotIfVar[128];
    sprintf(resultIfVar, "IF%s", resultvar);
    sprintf(resultNotIfVar, "IFNOT%s", resultvar);
    char resultIf[128];
    char resultNotIf[128];
    if (result != 0)
    {
        sprintf(resultIf, "%s", " ");
        sprintf(resultNotIf, "%s", "#");
    }
    else
    {
        sprintf(resultIf, "%s", "#");
        sprintf(resultNotIf, "%s", " ");
    }
	if (verbose)
	{
	    printf("iocstringiftest: setting %s=\"%s\"\n", resultIfVar, resultIf);
	    printf("iocstringiftest: setting %s=\"%s\"\n", resultNotIfVar, resultNotIf);
	}
	epicsEnvSet(resultIfVar, resultIf);
	epicsEnvSet(resultNotIfVar, resultNotIf);
}

extern "C" {

// EPICS iocsh shell commands 

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

static const iocshArg stringiftestInitArg0 = { "resultvar", iocshArgString };
static const iocshArg stringiftestInitArg1 = { "lhs", iocshArgString };
static const iocshArg stringiftestInitArg2 = { "operation", iocshArgInt };
static const iocshArg stringiftestInitArg3 = { "rhs", iocshArgString };
static const iocshArg * const stringiftestInitArgs[] = { &stringiftestInitArg0, &stringiftestInitArg1, &stringiftestInitArg2, &stringiftestInitArg3 };

static const iocshFuncDef stringiftestInitFuncDef = {"stringiftest", sizeof(stringiftestInitArgs) / sizeof(iocshArg*), stringiftestInitArgs};

static void stringiftestInitCallFunc(const iocshArgBuf *args)
{
    iocstringiftest(args[0].sval, args[1].sval, args[2].ival, args[3].sval);
}

static void iocstringtestRegister(void)
{
    iocshRegister(&stringtestInitFuncDef, stringtestInitCallFunc);
    iocshRegister(&stringiftestInitFuncDef, stringiftestInitCallFunc);
}

epicsExportRegistrar(iocstringtestRegister); // need to be declared via registrar() in utilities.dbd too

// asub callable functions - need to be in utilities.dbd as function() 

//epicsRegisterFunction(setIOCName); 
//epicsRegisterFunction(getIOCName); 
//epicsRegisterFunction(getIOCGroup); 

}

