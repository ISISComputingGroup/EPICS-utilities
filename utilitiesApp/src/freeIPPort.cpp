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

#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# define closesocket close
# define SOCKET int
#endif

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

static int getFreeIPPort()
{
    struct sockaddr_in serv_addr;
    socklen_t len;
    int enable = 1;
    SOCKET sock;
    int port = 0;
// EPICS should already have done this
//#ifdef _WIN32
//    WORD versionWanted = MAKEWORD(1, 1);
//    WSADATA wsaData;
//    WSAStartup(versionWanted, &wsaData);
//#endif    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
		errlogPrintf("freeIPPort: socket error\n");
        return port;
    }
#ifdef _WIN32
    if (setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE , (char*)&enable, sizeof(enable)) < 0 ) {
		errlogPrintf("freeIPPort: setsockopt error\n");
        return port;
    }
#else
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable)) < 0) {
		errlogPrintf("freeIPPort: setsockopt error\n");
        return port;
    }
#endif /* _WIN32 */
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = 0;
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		errlogPrintf("freeIPPort: bind error\n");
        return port;
    }
    len = sizeof(serv_addr);
    if (getsockname(sock, (struct sockaddr *)&serv_addr, &len) == -1) {
		errlogPrintf("freeIPPort: getsockname error\n");
        return port;
    }
    if (closesocket (sock) == 0 ) {
        port = ntohs(serv_addr.sin_port);
    }
    return port;
}

epicsShareFunc int freeIPPort(const char* macro)
{
	std::stringstream oss;
	if (macro != NULL)
	{
	    oss << getFreeIPPort();
	    epicsEnvSet(macro, oss.str().c_str());
	}
	else
	{
	    errlogPrintf("freeIPPort: please specify a macro to set\n");
	}
	return 0;
}

extern "C" {

// EPICS iocsh shell commands 

static const iocshArg freeIPPortArg0 = { "macro", iocshArgString };			///< macro to set
static const iocshArg * const freeIPPortArgs[] = { &freeIPPortArg0 };

static const iocshFuncDef freeIPPortFuncDef = {"freeIPPort", sizeof(freeIPPortArgs) / sizeof(iocshArg*), freeIPPortArgs};

static void freeIPPortCallFunc(const iocshArgBuf *args)
{
    freeIPPort(args[0].sval);
}

static void freeIPPortRegister(void)
{
    iocshRegister(&freeIPPortFuncDef, freeIPPortCallFunc);
}

epicsExportRegistrar(freeIPPortRegister); // need to be declared via registrar() in utilities.dbd too

}

