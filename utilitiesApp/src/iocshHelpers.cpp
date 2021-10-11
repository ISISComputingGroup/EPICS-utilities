#include "dbAccessDefs.h"
#include "dbStaticLib.h"
#include "epicsString.h"
#include "envDefs.h"
#include "iocsh.h"
#include "epicsExport.h"

/// Search for PVs that match the specified pattern (as dbgrep) and put the number of found PVs in the specified macro.
///
/// @code
///     countdbgrep("NUM_AXES", "*AXIS*")
/// @endcode 
///
/// @param[in] resultvar The name of the macro to output the result into
/// @param[in] pmask The pattern to search for
long countdbgrep(const char* resultvar, const char *pmask) {
    int count = 0;
    DBENTRY dbentry;
    DBENTRY *pdbentry = &dbentry;
    long status;
    if (!pmask || !*pmask) {
        printf("Usage: dbgrep \"pattern\"\n");
        return 1;
    }
    if (!pdbbase) {
        printf("No database loaded\n");
        return 0;
    }
    dbInitEntry(pdbbase, pdbentry);
    status = dbFirstRecordType(pdbentry);
    while (!status) {
        status = dbFirstRecord(pdbentry);
        while (!status) {
            char *pname = dbGetRecordName(pdbentry);
            if (epicsStrGlobMatch(pname, pmask))
                count++;
            status = dbNextRecord(pdbentry);
        }
        status = dbNextRecordType(pdbentry);
    }

    dbFinishEntry(pdbentry);
    char result_str[32];
    sprintf(result_str, "%i", count);
    epicsEnvSet(resultvar, result_str);
    return 0;
}


extern "C" {

// EPICS iocsh shell commands 

static const iocshArg countdbgrepInitArg0 = { "resultvar", iocshArgString }; ///< The name of the macro to put the result of the calculation into.
static const iocshArg countdbgrepInitArg1 = { "pmask", iocshArgString }; ///< The left hand side argument.

static const iocshArg * const countdbgrepInitArgs[] = { &countdbgrepInitArg0, &countdbgrepInitArg1};

static const iocshFuncDef countdbgrepInitFuncDef = {"countdbgrep", sizeof(countdbgrepInitArgs) / sizeof(iocshArg*), countdbgrepInitArgs};

static void countdbgrepInitCallFunc(const iocshArgBuf *args)
{
    countdbgrep(args[0].sval, args[1].sval);
}

static void ioccountdbgrepRegister(void)
{
    iocshRegister(&countdbgrepInitFuncDef, countdbgrepInitCallFunc);
}

epicsExportRegistrar(ioccountdbgrepRegister); // need to be declared via registrar() in utilities.dbd too

}

