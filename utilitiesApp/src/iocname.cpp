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

epicsShareFunc std::string epicsShareAPI setIOCName(const char* iocName)
{
    const char *iocBootDir = NULL, *old_iocName = macEnvExpand("$(IOCNAME=)");
	if (iocName != NULL)
	{
	    iocBootDir = iocName;
	}
    else if (old_iocName && *old_iocName)
    {
	    iocBootDir = old_iocName;
    }
    else
    {
        iocBootDir = macEnvExpand("$(IOC)");
    }
	if (iocBootDir == NULL || *iocBootDir == '\0')
	{
		return "";
	}
	// The IOC name may be a boot directory, in which case strip of the initial ioc prefix
	std::string iocBootDir_s = iocBootDir;
	std::transform(iocBootDir_s.begin(), iocBootDir_s.end(), iocBootDir_s.begin(), ::toupper); // uppercase
	std::replace(iocBootDir_s.begin(), iocBootDir_s.end(), '-', '_'); // hyphen to underscore
	std::string ioc_name, ioc_group; 
	if (!iocBootDir_s.compare(0, 3, "IOC")) // are we a boot area (iocYYYY) rather than an ioc name
	{
		ioc_name = iocBootDir_s.substr(3);
	}
	else
	{
		ioc_name = iocBootDir_s;
	}
	// strip off -IOC-01 trailer to get IOC group, and remove -IOC from -IOC- to get ioc name (we want YY-IOC-01 to beceom YY_01
	size_t pos = ioc_name.find("_IOC_");
	if (pos != std::string::npos)
	{
	    ioc_group = ioc_name.substr(0, pos); 
		ioc_name.erase(pos, strlen("_IOC")); // not _IOC_ as we want to leave an _ before the number
	}
	else if ( (pos = ioc_name.rfind("_")) != std::string::npos )   // old style nameing e.g. DEVICE32_01
	{
		if (atoi(ioc_name.substr(pos).c_str()) > 0)
		{
			ioc_group = ioc_name.substr(0, pos); 
		}
		else
		{
			ioc_group = ioc_name; 
		}
	}
	else
	{
	    ioc_group = ioc_name; 
	}
	epicsEnvSet("IOCNAME", ioc_name.c_str());
	epicsEnvSet("IOCGROUP", ioc_group.c_str());
	return ioc_name;
}	

epicsShareFunc std::string epicsShareAPI getIOCName()
{
    const char* iocName = macEnvExpand("$(IOCNAME=)");
	if (iocName == NULL || *iocName == '\0')
	{
	    setIOCName(NULL);
		iocName = macEnvExpand("$(IOCNAME)");
	}
    return (iocName != NULL ? iocName : "");
}

epicsShareFunc std::string epicsShareAPI getIOCGroup()
{
    const char* iocGroup = macEnvExpand("$(IOCGROUP=)");
	if (iocGroup == NULL || *iocGroup == '\0')
	{
	    setIOCName(NULL);
		iocGroup = macEnvExpand("$(IOCGROUP)");
	}
    return (iocGroup != NULL ? iocGroup : "");
}

extern "C" {

// EPICS iocsh shell commands 

static const iocshArg setInitArg0 = { "iocName", iocshArgString };			///< The name of the ioc
static const iocshArg * const setInitArgs[] = { &setInitArg0 };

static const iocshFuncDef setInitFuncDef = {"setIOCName", sizeof(setInitArgs) / sizeof(iocshArg*), setInitArgs};
static const iocshFuncDef getInitFuncDef = {"getIOCName", 0, NULL};
static const iocshFuncDef groupInitFuncDef = {"getIOCGroup", 0, NULL};

static void setInitCallFunc(const iocshArgBuf *args)
{
    setIOCName(args[0].sval);
}

static void getInitCallFunc(const iocshArgBuf *args)
{
    std::string iocName = getIOCName();
	std::cout << iocName;
}

static void groupInitCallFunc(const iocshArgBuf *args)
{
    std::string iocGroup = getIOCGroup();
	std::cout << iocGroup;
}

static void iocnameRegister(void)
{
    iocshRegister(&setInitFuncDef, setInitCallFunc);
    iocshRegister(&getInitFuncDef, getInitCallFunc);
    iocshRegister(&groupInitFuncDef, groupInitCallFunc);
}

epicsExportRegistrar(iocnameRegister); // need to be declared via registrar() in utilities.dbd too

// asub callable functions - need to be in utilities.dbd as function() 

//epicsRegisterFunction(setIOCName); 
//epicsRegisterFunction(getIOCName); 
//epicsRegisterFunction(getIOCGroup); 

}

