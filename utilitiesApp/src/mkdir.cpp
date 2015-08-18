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

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif /* _WIN32 */

#include <string.h>
#include <registryFunction.h>

#include <epicsExport.h>

#include "utilities.h"

static void my_mkdir(const char* path, int options)
{
    if (path == NULL)
	{
	    errlogPrintf("mkdir: ERROR: NULL args");
		return;
	}
    if (access(path, 06) == 0)
    {
        return; // already exists with read + write access
    }
#ifdef _WIN32
    if (mkdir(path) != 0)
#else
    if (mkdir(path, 0755) != 0)
#endif /* _WIN32 */
    {
	    errlogPrintf("mkdir: ERROR: unable to create \"%s\"", path);
    }
}

extern "C" {

// EPICS iocsh shell commands 

static const iocshArg mkdirInitArg0 = { "path", iocshArgString };
static const iocshArg mkdirInitArg1 = { "options", iocshArgInt };
static const iocshArg * const mkdirInitArgs[] = { &mkdirInitArg0, &mkdirInitArg1 };

static const iocshFuncDef mkdirInitFuncDef = {"mkdir", sizeof(mkdirInitArgs) / sizeof(iocshArg*), mkdirInitArgs};

static void mkdirInitCallFunc(const iocshArgBuf *args)
{
    my_mkdir(args[0].sval, args[1].ival);
}

static void mkdirRegister(void)
{
    iocshRegister(&mkdirInitFuncDef, mkdirInitCallFunc);
}

epicsExportRegistrar(mkdirRegister); // need to be declared via registrar() in utilities.dbd too

}

