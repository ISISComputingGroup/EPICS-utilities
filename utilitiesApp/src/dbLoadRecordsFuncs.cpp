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
    std::string macros_s;
    subMacros(macros_s, macros, loopVar);
    for(int i = start; i <= stop; i += step)
    {
        epicsSnprintf(loopVal, sizeof(loopVal), "%d", i);
        epicsEnvSet(loopVar, loopVal);   
        char* macros_exp = macEnvExpand(macros_s.c_str());
        std::ostringstream new_macros;
        new_macros << macros_exp << "," << loopVar << "=" << i;
        std::cout << "(" << i << ") dbLoadRecords(\"" << dbFile << "\",\"" << new_macros.str() << "\")" << std::endl;
        free(macros_exp);
        dbLoadRecords(dbFile, new_macros.str().c_str());
    }
}

/// load a db file multiple times according to a list of items separated by known separator(s)
epicsShareFunc void dbLoadRecordsList(const char* dbFile, const char* macros, const char* loopVar, const char* list)
{
    const char* sep = ";";
    if (loopVar == NULL || list == NULL)
    {
        dbLoadRecords(dbFile, macros);
        return;
    }
    std::string macros_s;
    subMacros(macros_s, macros, loopVar);
    char* saveptr = NULL;
    char* list_tmp = strdup(list);
    char* list_item = epicsStrtok_r(list_tmp, sep, &saveptr);
    while(list_item != NULL)
    {
        epicsEnvSet(loopVar, list_item);   
        char* macros_exp = macEnvExpand(macros_s.c_str());
        std::ostringstream new_macros;
        new_macros << macros_exp << "," << loopVar << "=" << list_item;
        std::cout << "(" << list_item << ") dbLoadRecords(\"" << dbFile << "\",\"" << new_macros.str() << "\")" << std::endl;
        free(macros_exp);
        dbLoadRecords(dbFile, new_macros.str().c_str());
        list_item = epicsStrtok_r(NULL, sep, &saveptr);
    }
    free(list_tmp);
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
static const iocshArg dbLoadRecordsListInitArg3 = { "list", iocshArgString };			///< start
static const iocshArg * const dbLoadRecordsListInitArgs[] = { &dbLoadRecordsListInitArg0, &dbLoadRecordsListInitArg1,
     &dbLoadRecordsListInitArg2, &dbLoadRecordsListInitArg3 };

static const iocshFuncDef dbLoadRecordsLoopDef = {"dbLoadRecordsLoop", sizeof(dbLoadRecordsLoopInitArgs) / sizeof(iocshArg*), dbLoadRecordsLoopInitArgs};

static const iocshFuncDef dbLoadRecordsListDef = {"dbLoadRecordsList", sizeof(dbLoadRecordsListInitArgs) / sizeof(iocshArg*), dbLoadRecordsListInitArgs};

static void dbLoadRecordsLoopInitCallFunc(const iocshArgBuf *args)
{
    dbLoadRecordsLoop(args[0].sval, args[1].sval, args[2].sval, args[3].ival, args[4].ival, args[5].ival);
}

static void dbLoadRecordsListInitCallFunc(const iocshArgBuf *args)
{
    dbLoadRecordsList(args[0].sval, args[1].sval, args[2].sval, args[3].sval);
}

static void dbLoadRecordsFuncsRegister(void)
{
    iocshRegister(&dbLoadRecordsLoopDef, dbLoadRecordsLoopInitCallFunc);
    iocshRegister(&dbLoadRecordsListDef, dbLoadRecordsListInitCallFunc);
}

epicsExportRegistrar(dbLoadRecordsFuncsRegister); // need to be declared via registrar() in utilities.dbd too

}
