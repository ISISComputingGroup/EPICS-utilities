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

// options
// default: perform calc and return double in resultVar
// 0x1: verbose
static void iocdcalc(const char* resultvar, const char* expression, int options, int precision)
{
    if (resultvar == NULL || expression == NULL)
	{
	    errlogPrintf("iocdcalc: ERROR: NULL args");
		return;
	}
	bool verbose = (options & 0x1);
    char* expr_expand = macEnvExpand(expression);
	if (expr_expand == NULL)
	{
	    errlogPrintf("iocdcalc: ERROR: NULL expanded expression arg");
		return;
	}
	if (verbose)
	{
	    printf("iocdcalc: expanded expression=\"%s\"\n", expr_expand);
	}
	short calc_error;
	static const int CALC_NARGS = 12; // named A to L
	std::vector<double> parg(CALC_NARGS);
	for(int i=0; i<CALC_NARGS; ++i)
	{
	    parg[i] = 0.0;
	}
	double result;
	// need at add extra space to INFIX_TO_POSTFIX_SIZE - bug?
	boost::scoped_array<char> ppostfix(new char[INFIX_TO_POSTFIX_SIZE(strlen(expr_expand)) + 100]);   // cannot use  std::unique_ptr<char[]>  yet
	if ( postfix(expr_expand, ppostfix.get(), &calc_error) != 0 )
	{
	    errlogPrintf("iocdcalc: ERROR: postfix: %s\n", calcErrorStr(calc_error));
		return;
	}
	if ( calcPerform(&(parg[0]), &result, ppostfix.get()) != 0 )
	{
	    errlogPrintf("iocdcalc: ERROR: calcPerform: %s\n", "");
		return;
	}
	char result_str[32];
	double dsmall = 1.0E-20;
	std::ostringstream format_str;
	
	format_str << "%";
	if (precision > 0)
	{
		format_str << "." << precision;
	}
	format_str << "f";
	if ( sprintf(result_str, format_str.str().c_str(), result) < 0 )
	{
		errlogPrintf("iocdcalc: ERROR: sprintf: %s\n", format_str.str().c_str());
		return;
	}
	
	epicsEnvSet(resultvar, result_str);
	free(expr_expand);
}

extern "C" {

// EPICS iocsh shell commands 

// calc "result" "expression"
static const iocshArg dcalcInitArg0 = { "resultvar", iocshArgString };
static const iocshArg dcalcInitArg1 = { "expression", iocshArgString };
static const iocshArg dcalcInitArg2 = { "options", iocshArgInt };
static const iocshArg dcalcInitArg3 = { "precision", iocshArgInt };
static const iocshArg * const dcalcInitArgs[] = { &dcalcInitArg0, &dcalcInitArg1, &dcalcInitArg2, &dcalcInitArg3 };

static const iocshFuncDef dcalcInitFuncDef = {"dcalc", sizeof(dcalcInitArgs) / sizeof(iocshArg*), dcalcInitArgs};

static void dcalcInitCallFunc(const iocshArgBuf *args)
{
    iocdcalc(args[0].sval, args[1].sval, args[2].ival, args[3].ival);
}

static void iocdcalcRegister(void)
{
    iocshRegister(&dcalcInitFuncDef, dcalcInitCallFunc);
}

epicsExportRegistrar(iocdcalcRegister); // need to be declared via registrar() in utilities.dbd too

// asub callable functions - need to be in utilities.dbd as function() 

//epicsRegisterFunction(setIOCName); 
//epicsRegisterFunction(getIOCName); 
//epicsRegisterFunction(getIOCGroup); 

}

