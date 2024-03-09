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

/// look for e.g. \$() and replace with $() so we can substitute later with macEnvExpand()
static void subMacros(std::string& new_macros, const char* macros, const char* loopVar)
{
    new_macros = macros;
    std::smatch bracketSearch, braceSearch;
    // Check that we have done a search, and that the result of both is empty before finishing.
    while((!bracketSearch.empty())||(!braceSearch.empty())||(!bracketSearch.ready())){
        std::regex_search(new_macros, bracketSearch, std::regex(R"(\\\$\(([0-9a-zA-Z_$(){}]+)\))"));
        new_macros = std::regex_replace(new_macros, std::regex(R"(\\\$\(([0-9a-zA-Z_$(){}]+)\))"), "$($1)");
        // Have to look twice to guarantee that brackets/curly braces are properly matched.
        std::regex_search(new_macros, braceSearch, std::regex(R"(\\\$\{([0-9a-zA-Z_$(){}]+)\})"));
        new_macros = std::regex_replace(new_macros, std::regex(R"(\\\$\{([0-9a-zA-Z_$(){}]+)\})"), "${$1}");
    }
}

static void doit(const char* macros)
{
    std::string new_macros;
    subMacros(new_macros, macros, NULL);
    std::cerr << macros << " -> " << new_macros << std::endl;
}

int main(int argc, char* argv[])
{
    doit("$(A)");
    doit("\\$(A)");
    doit("\\\\$(A)");
    doit("\\\\\\$(A)");
    doit("$(A$(B))");
    doit("\\$(A\\$(B))");
    doit("\\\\$(A\\\\$(B))");
    doit("\\\\\\$(A\\\\\\$(B))");
    doit("\\${A\\$(B)}");
    doit("\\$(A\\${B})");
    doit("\\$(A\\${B})\\t");
    doit("\\${A\\$(B)}\\$(C)");
    return 0;
}
    
