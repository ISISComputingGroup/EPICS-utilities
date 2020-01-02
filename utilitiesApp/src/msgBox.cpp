///
/// @file msgBox.cpp EPICS functions for launching a message box
/// @author Freddie Akeroyd, STFC ISIS Facility <freddie.akeroyd@stfc.ac.uk>
///
#ifdef _WIN32
#include "windows.h"
#endif

#include <string.h>
#include <registryFunction.h>
#include <iocsh.h>

#include <epicsExport.h>

#include "utilities.h"

extern "C" {

// EPICS iocsh shell commands 

static const iocshArg msgBoxArg0 = { "title", iocshArgString };	
static const iocshArg msgBoxArg1 = { "text", iocshArgString };	
static const iocshArg * const msgBoxInitArgs[] = { &msgBoxArg0, &msgBoxArg1 };

static const iocshFuncDef msgBoxFuncDef = {"msgBox", sizeof(msgBoxInitArgs) / sizeof(iocshArg*), msgBoxInitArgs};

/// Run up a message box on windows with the title and text from args[0] and args[1] respectively
static void msgBoxCallFunc(const iocshArgBuf *args)
{
	const char* title = (args[0].sval != NULL ? args[0].sval : "IOC Message Box");
	const char* text = (args[1].sval != NULL ? args[1].sval : "Press OK to continue");
#ifdef _WIN32
    MessageBox(NULL, text, title, MB_OK);
#endif
}

/// Register the message box function for use in an ioc boot cmd
static void msgBoxRegister(void)
{
    iocshRegister(&msgBoxFuncDef, msgBoxCallFunc);
}

epicsExportRegistrar(msgBoxRegister); // need to be declared via registrar() in utilities.dbd too
 
}

