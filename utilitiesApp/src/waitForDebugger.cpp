#include <windows.h>
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
#ifndef _WIN32
#include <unistd.h>
#endif

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
#include "pcrecpp.h"

#include <string.h>
#include <regex>
#include <registryFunction.h>

#include <epicsExport.h>

#include "utilities.h"

// 0 on success, -1 on timeout
epicsShareFunc int waitForDebugger(int timeout)
{
    epicsTimeStamp start_time, current;
    epicsTimeGetCurrent(&start_time);
    while(true) {
        if (IsDebuggerPresent()) {
            return 0;
        }
        epicsTimeGetCurrent(&current);
        if (timeout > 0 && epicsTimeDiffInSeconds(&current, &start_time) > timeout) {
            return -1;
        }
    }    
}

extern "C" {

// EPICS iocsh shell commands 

static const iocshArg waitForDebuggerInitArg0 = { "timeout", iocshArgInt };
static const iocshArg * const waitForDebuggerInitArgs[] = { &waitForDebuggerInitArg0 };

static const iocshFuncDef waitForDebuggerDef = {"waitForDebugger", sizeof(waitForDebuggerInitArgs) / sizeof(iocshArg*), waitForDebuggerInitArgs};

static void waitForDebuggerInitCallFunc(const iocshArgBuf *args)
{
    waitForDebugger(args[0].ival);
}

static void waitForDebuggerRegister(void)
{
    iocshRegister(&waitForDebuggerDef, waitForDebuggerInitCallFunc);
}

epicsExportRegistrar(waitForDebuggerRegister); // need to be declared via registrar() in utilities.dbd too

}
