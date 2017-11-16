///
/// @file iocstringtest.cpp EPICS functions for testing strings against each other
/// @author Freddie Akeroyd, STFC ISIS Facility <freddie.akeroyd@stfc.ac.uk>
///
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

/// Does the actual calculation comparing the given strings to each other.
/// Helper function, not to be called from an IOC.
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
    else // old default
    {
        if (strlen(lhs_expand) > 0)
        {
            result = 1;
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

/// Compares the given strings based on the \a operation flag and returns ' ' on true and '#' on false into the \a resultvar macro.
/// 
/// E.g. to check if a macro named \a DO_I_EXIST does actually exist and put the result in a macro called \a EXISTS:
/// @code
///		stringtest("EXISTS", "$(DO_I_EXIST)")
/// @endcode
///
/// @param[in] resultvar @copydoc stringtestInitArg0
/// @param[in] lhs @copydoc stringtestInitArg1
/// @param[in] operation @copydoc stringtestInitArg2
/// @param[in] rhs @copydoc stringtestInitArg3
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

/// Compares the given strings based on the \a operation flag, putting the result in two different macros. 
/// This function will create two macros called IF\a resultvar and IFNOT\a resultvar. If the comparison is 
/// true a '#' will be placed in IFNOT\a resultvar. If the comparison is false a '#' will be placed in 
/// IF\a resultvar.
/// 
/// E.g. to check if a macro named \a DO_I_EXIST does actually exist then do some follow up:
/// @code
///		stringtest("EXISTS", "$(DO_I_EXIST)")
///		$(IFEXISTS) < st-existing.cmd
///		$(IFNOTEXISTS) < st-not-existing.cmd
/// @endcode
///
/// to check a macro named \a AM_I_CORRECT is equal to \a CORRECT_ANSWER then do some follow up:
/// @code
///		stringtest("EXPECTED", "$(AM_I_CORRECT)", 5, "CORRECT_ANSWER")
///		$(IFEXPECTED) < st-expected.cmd
///		$(IFNOTEXPECTED) < st-not-expected.cmd
/// @endcode
///
/// @param[in] resultvar @copydoc stringtestInitArg0
/// @param[in] lhs @copydoc stringtestInitArg1
/// @param[in] operation @copydoc stringtestInitArg2
/// @param[in] rhs @copydoc stringtestInitArg3
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

static const iocshArg stringtestInitArg0 = { "resultvar", iocshArgString }; ///< The name of the macro to put the result of the calculation into.
static const iocshArg stringtestInitArg1 = { "lhs", iocshArgString }; ///< The left hand side argument.

/// The operation to perform on the string.
/// 0x1: verbose
/// 0x2: strlen(\a lhs) > 0 (default)
/// 0x4: \a lhs == \a rhs
/// 0x8: reverse the above logic
static const iocshArg stringtestInitArg2 = { "operation", iocshArgInt }; 
static const iocshArg stringtestInitArg3 = { "rhs", iocshArgString }; ///< The right hand side argument.
static const iocshArg * const stringtestInitArgs[] = { &stringtestInitArg0, &stringtestInitArg1, &stringtestInitArg2, &stringtestInitArg3 };

static const iocshFuncDef stringtestInitFuncDef = {"stringtest", sizeof(stringtestInitArgs) / sizeof(iocshArg*), stringtestInitArgs};

static void stringtestInitCallFunc(const iocshArgBuf *args)
{
    iocstringtest(args[0].sval, args[1].sval, args[2].ival, args[3].sval);
}

static const iocshArg stringiftestInitArg0 = { "resultvar", iocshArgString }; ///< The name of the macro to put the result of the calculation into.
static const iocshArg stringiftestInitArg1 = { "lhs", iocshArgString }; ///< The left hand side argument.

/// The operation to perform on the string.
/// 0x1: verbose
/// 0x2: strlen(\a lhs) > 0 (default)
/// 0x4: \a lhs == \a rhs
/// 0x8: reverse the above logic
static const iocshArg stringiftestInitArg2 = { "operation", iocshArgInt };
static const iocshArg stringiftestInitArg3 = { "rhs", iocshArgString }; ///< The right hand side argument.
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

