///
/// @file dbLoadRecordsFuncs.cpp Enhanced EPICS dbLoadRecords functions
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

#include "epicsStdlib.h"
#include "epicsString.h"
#include "epicsStdio.h"
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

#include <string.h>
#include <registryFunction.h>

#include <epicsExport.h>

#include "utilities.h"

/// load current environment into mac handle 
static void loadMacEnviron(MAC_HANDLE* pmh)
{
	for(char** cp = environ; *cp != NULL; ++cp)
	{
		char* str_tmp = strdup(*cp);
		char* equals_loc = strchr(str_tmp, '='); // split   name=value   string
		if (equals_loc != NULL)
		{
		    *equals_loc = '\0';
		    macPutValue(pmh, str_tmp, equals_loc + 1);
		}
		free(str_tmp);
	}
}

/// look for e.g. \$(I) and replace with $(I) so we can substitute later with macEnvExpand()
static void subMacros(std::string& new_macros, const char* macros, const char* loopVar)
{
    char loopSubFrom[32], loopSubTo[32];
    epicsSnprintf(loopSubFrom, sizeof(loopSubFrom), "\\$(%s)", loopVar);
    epicsSnprintf(loopSubTo, sizeof(loopSubTo), "$(%s)", loopVar);
    new_macros = macros;
    size_t start_pos = 0, lf = strlen(loopSubFrom), lt = strlen(loopSubTo);
    while( (start_pos = new_macros.find(loopSubFrom, start_pos)) != std::string::npos ) 
    {
        new_macros.replace(start_pos, lf, loopSubTo);
        start_pos += lt;
    }
    epicsSnprintf(loopSubFrom, sizeof(loopSubFrom), "\\${%s}", loopVar);
    epicsSnprintf(loopSubTo, sizeof(loopSubTo), "${%s}", loopVar);
	start_pos = 0;
    while( (start_pos = new_macros.find(loopSubFrom, start_pos)) != std::string::npos ) 
    {
        new_macros.replace(start_pos, lf, loopSubTo);
        start_pos += lt;
    }
}

/// load a db file multiple times according to a number range
epicsShareFunc void dbLoadRecordsLoop(const char* dbFile, const char* macros, const char* loopVar, int start, int stop, int step)
{
    char loopVal[32];
    if (loopVar == NULL)
    {
        dbLoadRecords(dbFile, macros);
        return;
    }
    if (step <= 0)
    {
        step = 1;
    }
    std::string macros_s, dbFile_s;
    subMacros(macros_s, macros, loopVar);
    subMacros(dbFile_s, dbFile, loopVar);
    MAC_HANDLE* mh = NULL;
	char macros_exp[1024], dbFile_exp[1024];
    macCreateHandle(&mh, NULL);
	loadMacEnviron(mh);
    for(int i = start; i <= stop; i += step)
    {
		macPushScope(mh);
        epicsSnprintf(loopVal, sizeof(loopVal), "%d", i);
        macPutValue(mh, loopVar, loopVal);   
        macExpandString(mh, macros_s.c_str(), macros_exp, sizeof(macros_exp));
        macExpandString(mh, dbFile_s.c_str(), dbFile_exp, sizeof(dbFile_exp));
        std::ostringstream new_macros;
        new_macros << macros_exp << (strlen(macros_exp) > 0 ? "," : "") << loopVar << "=" << i;
        std::cout << "--> (" << i << ") dbLoadRecords(\"" << dbFile_exp << "\",\"" << new_macros.str() << "\")" << std::endl;
        dbLoadRecords(dbFile_exp, new_macros.str().c_str());
		macPopScope(mh);
    }
	macDeleteHandle(mh);		
}

/// load a db file multiple times according to a list of items separated by known separator(s)
epicsShareFunc void dbLoadRecordsList(const char* dbFile, const char* macros, const char* loopVar, const char* list, const char* sep)
{
    static const char* default_sep = ";";
    if (loopVar == NULL || list == NULL)
    {
        dbLoadRecords(dbFile, macros);
        return;
    }
	if (sep == NULL)
	{
		sep = default_sep;
	}
    std::string macros_s, dbFile_s;
    subMacros(macros_s, macros, loopVar);
    subMacros(dbFile_s, dbFile, loopVar);
    MAC_HANDLE* mh = NULL;
	char macros_exp[1024], dbFile_exp[1024];
    macCreateHandle(&mh, NULL);
	loadMacEnviron(mh);
    char* saveptr = NULL;
    char* list_tmp = strdup(list);
    char* list_item = epicsStrtok_r(list_tmp, sep, &saveptr);
    while(list_item != NULL)
    {
		macPushScope(mh);
        macPutValue(mh, loopVar, list_item);   
        macExpandString(mh, macros_s.c_str(), macros_exp, sizeof(macros_exp));
        macExpandString(mh, dbFile_s.c_str(), dbFile_exp, sizeof(dbFile_exp));
        std::ostringstream new_macros;
        new_macros << macros_exp << (strlen(macros_exp) > 0 ? "," : "") << loopVar << "=" << list_item;
        std::cout << "--> (" << list_item << ") dbLoadRecords(\"" << dbFile_exp << "\",\"" << new_macros.str() << "\")" << std::endl;
        dbLoadRecords(dbFile_exp, new_macros.str().c_str());
        list_item = epicsStrtok_r(NULL, sep, &saveptr);
		macPopScope(mh);
    }
    free(list_tmp);
	macDeleteHandle(mh);		
}

extern "C" {

// EPICS iocsh shell commands 

static const iocshArg dbLoadRecordsLoopInitArg0 = { "dbFile", iocshArgString };			///< DB filename
static const iocshArg dbLoadRecordsLoopInitArg1 = { "macros", iocshArgString };			///< macros
static const iocshArg dbLoadRecordsLoopInitArg2 = { "loopVar", iocshArgString };			///< macros
static const iocshArg dbLoadRecordsLoopInitArg3 = { "start", iocshArgInt };			///< start
static const iocshArg dbLoadRecordsLoopInitArg4 = { "stop", iocshArgInt };			///< step
static const iocshArg dbLoadRecordsLoopInitArg5 = { "step", iocshArgInt };			///< stop
static const iocshArg * const dbLoadRecordsLoopInitArgs[] = { &dbLoadRecordsLoopInitArg0, &dbLoadRecordsLoopInitArg1,
     &dbLoadRecordsLoopInitArg2, &dbLoadRecordsLoopInitArg3, &dbLoadRecordsLoopInitArg4, &dbLoadRecordsLoopInitArg5 };

static const iocshArg dbLoadRecordsListInitArg0 = { "dbFile", iocshArgString };			///< DB filename
static const iocshArg dbLoadRecordsListInitArg1 = { "macros", iocshArgString };			///< macros
static const iocshArg dbLoadRecordsListInitArg2 = { "loopVar", iocshArgString };			///< macros
static const iocshArg dbLoadRecordsListInitArg3 = { "list", iocshArgString };			///< list
static const iocshArg dbLoadRecordsListInitArg4 = { "sep", iocshArgString };			///< separator
static const iocshArg * const dbLoadRecordsListInitArgs[] = { &dbLoadRecordsListInitArg0, &dbLoadRecordsListInitArg1,
     &dbLoadRecordsListInitArg2, &dbLoadRecordsListInitArg3, &dbLoadRecordsListInitArg4 };

static const iocshFuncDef dbLoadRecordsLoopDef = {"dbLoadRecordsLoop", sizeof(dbLoadRecordsLoopInitArgs) / sizeof(iocshArg*), dbLoadRecordsLoopInitArgs};

static const iocshFuncDef dbLoadRecordsListDef = {"dbLoadRecordsList", sizeof(dbLoadRecordsListInitArgs) / sizeof(iocshArg*), dbLoadRecordsListInitArgs};

static void dbLoadRecordsLoopInitCallFunc(const iocshArgBuf *args)
{
    dbLoadRecordsLoop(args[0].sval, args[1].sval, args[2].sval, args[3].ival, args[4].ival, args[5].ival);
}

static void dbLoadRecordsListInitCallFunc(const iocshArgBuf *args)
{
    dbLoadRecordsList(args[0].sval, args[1].sval, args[2].sval, args[3].sval, args[4].sval);
}

static void dbLoadRecordsFuncsRegister(void)
{
    iocshRegister(&dbLoadRecordsLoopDef, dbLoadRecordsLoopInitCallFunc);
    iocshRegister(&dbLoadRecordsListDef, dbLoadRecordsListInitCallFunc);
}

epicsExportRegistrar(dbLoadRecordsFuncsRegister); // need to be declared via registrar() in utilities.dbd too

}
