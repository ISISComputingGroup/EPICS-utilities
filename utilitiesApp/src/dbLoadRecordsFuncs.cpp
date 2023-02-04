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
void subMacros(std::string& new_macros, const char* macros)
{
    const std::regex bracketPattern = std::regex(R"(\\\$\(([0-9a-zA-Z_$(){}]+)\))");
    const std::regex bracePattern = std::regex(R"(\\\$\{([0-9a-zA-Z_$(){}]+)\})");
    new_macros = macros;
    std::smatch bracketSearch, braceSearch;
    // Check that we have done a search, and that the result of both is empty before finishing.
    while((!bracketSearch.empty())||(!braceSearch.empty())||(!bracketSearch.ready())){
        std::regex_search(new_macros, bracketSearch, bracketPattern);
        new_macros = std::regex_replace(new_macros, bracketPattern, std::string("$($1)"));
        // Have to look twice to guarantee that brackets/curly braces are properly matched.
        std::regex_search(new_macros, braceSearch, bracePattern);
        new_macros = std::regex_replace(new_macros, bracePattern, std::string("${$1}"));
    }
}

/// Load a db file multiple times substituting a specified macro according to a number range.
///
/// The \a dbFile and \a macros arguments are like the normal dbLoadRecords() however
/// it is possible to embed a macro within these whose value follows the range \a start to \a stop.
/// You can either load the same \a dbFile multiple times with different macros, or even load
/// different database files by using \a loopVar as part of the filename. If you want to use a list
/// of (non-numeric) substitutions rather than an integer range see dbLoadRecordsList()
///
/// The name of the macro to be used for substitution is contained in \a loopVar and needs to be
/// reference in an \\ escaped way to make sure EPICS does not try to substitute it too soon.
/// as well as the \a macros the \a dbFile is also passed the \a loopVar macro value
/// @code
///     dbLoadRecordsLoop("file\$(I).db", "P=1,Q=Hello\$(I)", "I", 1, 4)
/// @endcode 
///
/// @param[in] dbFile @copydoc dbLoadRecordsLoopInitArg0
/// @param[in] macros @copydoc dbLoadRecordsLoopInitArg1
/// @param[in] loopVar @copydoc dbLoadRecordsLoopInitArg2
/// @param[in] start @copydoc dbLoadRecordsLoopInitArg3
/// @param[in] stop @copydoc dbLoadRecordsLoopInitArg4
/// @param[in] step @copydoc dbLoadRecordsLoopInitArg5
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
    subMacros(macros_s, macros);
    subMacros(dbFile_s, dbFile);
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

/// Load a db file multiple times according to a list of items separated by known separator(s).
///
/// The \a dbFile and \a macros arguments are like the normal dbLoadRecords() however
/// it is possible to embed a macro within these whose value takes a value from the \a list.
/// You can either load the same \a dbFile multiple times with different macros, or even load
/// different database files by using \a loopVar as part of the filename. If you want to use a 
/// pure numeric range see dbLoadRecordsLoop()
///
/// The name of the macro to be used for substitution is contained in \a loopVar and needs to be
/// reference in an \\ escaped way to make sure EPICS does not try to substitute it too soon.
/// as well as the \a macros the \a dbFile is also passed the \a loopVar macro value
/// @code
///     dbLoadRecordsList("file\$(S).db", "P=1,Q=Hello\$(S)", "S", "A;B;C", ";")
/// @endcode 
///
/// @param[in] dbFile @copydoc dbLoadRecordsListInitArg0
/// @param[in] macros @copydoc dbLoadRecordsListInitArg1
/// @param[in] loopVar @copydoc dbLoadRecordsListInitArg2
/// @param[in] list @copydoc dbLoadRecordsListInitArg3
/// @param[in] sep @copydoc dbLoadRecordsListInitArg4
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
    subMacros(macros_s, macros);
    subMacros(dbFile_s, dbFile);
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


/// Create an alias for all records starting with prefix \a recordPrefix by replacing this with \a aliasPrefix
///
/// This is a bit like having an alias gateway, but without needing to run a gateway.
/// It could be extended to only prefix records with e.g. a particular info field present
///
/// So if a piece of movable equipoment was being used locally and its PVs/autosave were all set
/// up in the ME: domain, then aliases for local use could be created using
/// @code
///     dbAliasRecordsPrefix("ME:", "$(MYPVPREFIX)")
/// @endcode 
///
/// @param[in] recordPrefix @copydoc dbAliasRecordsPrefixInitArg0
/// @param[in] aliasPrefix @copydoc dbAliasRecordsPrefixInitArg1
/// @param[in] verbose @copydoc dbAliasRecordsPrefixInitArg2
epicsShareFunc void dbAliasRecordsPrefix(const char* recordPrefix, const char* aliasPrefix, int verbose)
{
    if (!*aliasPrefix || !*recordPrefix) {
        return;
    }
    size_t nprefix = strlen(recordPrefix);
    DBENTRY dbentry;
    DBENTRY *pdbentry=&dbentry;
    long status;
    if (!pdbbase) {
        throw std::runtime_error("No database loaded\n");
    }
    dbInitEntry(pdbbase, pdbentry);
    status = dbFirstRecordType(pdbentry);
    if (status) {
        return;
    }
    int n = 0;
    while (!status) {
        status = dbFirstRecord(pdbentry);
        while (!status) {
            if (!strncmp(dbGetRecordName(pdbentry), recordPrefix, nprefix)) {
                std::string alias_str(aliasPrefix);
                alias_str += (dbGetRecordName(pdbentry) + nprefix);
                if (dbCreateAlias(pdbentry, alias_str.c_str())) {
                    epicsPrintf("dbAliasRecordsPrefix: Can't create alias %s -> %s\n", alias_str.c_str(), dbGetRecordName(pdbentry));
                } else {
                    if (verbose) {
                        epicsPrintf("dbAliasRecordsPrefix: %s -> %s\n", alias_str.c_str(), dbGetRecordName(pdbentry));
                    } else if (++n % 10000 == 0) {
                        epicsPrintf("dbAliasRecordsPrefix: created %d aliases so far...\n", n);
                    }
                }                    
            }
            status = dbNextRecord(pdbentry);
        }
        status = dbNextRecordType(pdbentry);
    }
    dbFinishEntry(pdbentry);
    epicsPrintf("dbAliasRecordsPrefix: created %d aliases\n", n);
}

/// Create an alias for all records matching regular expression \a recordMatch by replacing with \a aliasSub
///
/// This is a bit like having an alias gateway, but without needing to run a gateway.
/// It could be extended to only prefix records with e.g. a particular info field present
///
/// So if a piece of movable equipoment was being used locally and its PVs/autosave were all set
/// up in the ME: domain, then aliases for local use could be created using
/// @code
///     dbAliasRecordsRE("ME:(.*)", "$(MYPVPREFIX)\1")
/// @endcode 
///
/// @param[in] recordMatch @copydoc dbAliasRecordsREInitArg0
/// @param[in] aliasSub @copydoc dbAliasRecordsREInitArg1
/// @param[in] verbose @copydoc dbAliasRecordsREInitArg2
epicsShareFunc void dbAliasRecordsRE(const char* recordMatch, const char* aliasSub, int verbose)
{
    if (!*aliasSub || !*recordMatch) {
        return;
    }
    pcrecpp::RE re(recordMatch);
    std::string s;
    DBENTRY dbentry;
    DBENTRY *pdbentry=&dbentry;
    long status;
    if (!pdbbase) {
        throw std::runtime_error("No database loaded\n");
    }
    dbInitEntry(pdbbase, pdbentry);
    status = dbFirstRecordType(pdbentry);
    if (status) {
        return;
    }
    int n = 0;
    while (!status) {
        status = dbFirstRecord(pdbentry);
        while (!status) {
            s = dbGetRecordName(pdbentry);
            if ( re.FullMatch(s) && re.Replace(aliasSub, &s) ) {
                if (dbCreateAlias(pdbentry, s.c_str())) {
                    epicsPrintf("dbAliasRecordsRE: Can't create alias %s -> %s\n", s.c_str(), dbGetRecordName(pdbentry));
                } else {
                    if (verbose) {
                        epicsPrintf("dbAliasRecordsRE: %s -> %s\n", s.c_str(), dbGetRecordName(pdbentry));
                    } else if (++n % 10000 == 0) {
                        epicsPrintf("dbAliasRecordsRE: created %d aliases so far...\n", n);
                    }
                }
            }
            status = dbNextRecord(pdbentry);
        }
        status = dbNextRecordType(pdbentry);
    }
    dbFinishEntry(pdbentry);
    epicsPrintf("dbAliasRecordsRE: created %d aliases\n", n);
}

extern "C" {

// EPICS iocsh shell commands 

static const iocshArg dbLoadRecordsLoopInitArg0 = { "dbFile", iocshArgString };			///< DB filename
static const iocshArg dbLoadRecordsLoopInitArg1 = { "macros", iocshArgString };			///< macros to pass to \a dbFile
static const iocshArg dbLoadRecordsLoopInitArg2 = { "loopVar", iocshArgString };	    ///< loop macro variable name
static const iocshArg dbLoadRecordsLoopInitArg3 = { "start", iocshArgInt };			///< start loop value
static const iocshArg dbLoadRecordsLoopInitArg4 = { "stop", iocshArgInt };			///< end loop value
static const iocshArg dbLoadRecordsLoopInitArg5 = { "step", iocshArgInt };			///< loop step (default: 1)
static const iocshArg * const dbLoadRecordsLoopInitArgs[] = { &dbLoadRecordsLoopInitArg0, &dbLoadRecordsLoopInitArg1,
     &dbLoadRecordsLoopInitArg2, &dbLoadRecordsLoopInitArg3, &dbLoadRecordsLoopInitArg4, &dbLoadRecordsLoopInitArg5 };

static const iocshArg dbLoadRecordsListInitArg0 = { "dbFile", iocshArgString };			///< DB filename
static const iocshArg dbLoadRecordsListInitArg1 = { "macros", iocshArgString };			///< macros to pass to \a dbFile
static const iocshArg dbLoadRecordsListInitArg2 = { "loopVar", iocshArgString };			///< list macro variable name
static const iocshArg dbLoadRecordsListInitArg3 = { "list", iocshArgString };			///< list of values to substitute
static const iocshArg dbLoadRecordsListInitArg4 = { "sep", iocshArgString };			///< \a list value separator character
static const iocshArg * const dbLoadRecordsListInitArgs[] = { &dbLoadRecordsListInitArg0, &dbLoadRecordsListInitArg1,
     &dbLoadRecordsListInitArg2, &dbLoadRecordsListInitArg3, &dbLoadRecordsListInitArg4 };

static const iocshArg dbAliasRecordsPrefixInitArg0 = { "recordPrefix", iocshArgString };   ///< prefix of records that we wish to alias
static const iocshArg dbAliasRecordsPrefixInitArg1 = { "aliasPrefix", iocshArgString };    ///< what to replace \a recordPrefix with
static const iocshArg dbAliasRecordsPrefixInitArg2 = { "verbose", iocshArgInt };           ///< set to 1 to print out aliases created
static const iocshArg * const dbAliasRecordsPrefixInitArgs[] = { &dbAliasRecordsPrefixInitArg0, &dbAliasRecordsPrefixInitArg1, &dbAliasRecordsPrefixInitArg2 };

static const iocshArg dbAliasRecordsREInitArg0 = { "recordMatch", iocshArgString };   ///< pattern of records that we wish to alias
static const iocshArg dbAliasRecordsREInitArg1 = { "aliasSub", iocshArgString };      ///< what to substitute \a recordMatch with
static const iocshArg dbAliasRecordsREInitArg2 = { "verbose", iocshArgInt };          ///< set to 1 to print out aliases created
static const iocshArg * const dbAliasRecordsREInitArgs[] = { &dbAliasRecordsREInitArg0, &dbAliasRecordsREInitArg1, &dbAliasRecordsREInitArg2 };

static const iocshFuncDef dbLoadRecordsLoopDef = {"dbLoadRecordsLoop", sizeof(dbLoadRecordsLoopInitArgs) / sizeof(iocshArg*), dbLoadRecordsLoopInitArgs};

static const iocshFuncDef dbLoadRecordsListDef = {"dbLoadRecordsList", sizeof(dbLoadRecordsListInitArgs) / sizeof(iocshArg*), dbLoadRecordsListInitArgs};

static const iocshFuncDef dbAliasRecordsPrefixDef = {"dbAliasRecordsPrefix", sizeof(dbAliasRecordsPrefixInitArgs) / sizeof(iocshArg*), dbAliasRecordsPrefixInitArgs};

static const iocshFuncDef dbAliasRecordsREDef = {"dbAliasRecordsRE", sizeof(dbAliasRecordsREInitArgs) / sizeof(iocshArg*), dbAliasRecordsREInitArgs};

static void dbLoadRecordsLoopInitCallFunc(const iocshArgBuf *args)
{
    dbLoadRecordsLoop(args[0].sval, args[1].sval, args[2].sval, args[3].ival, args[4].ival, args[5].ival);
}

static void dbLoadRecordsListInitCallFunc(const iocshArgBuf *args)
{
    dbLoadRecordsList(args[0].sval, args[1].sval, args[2].sval, args[3].sval, args[4].sval);
}

static void dbAliasRecordsPrefixInitCallFunc(const iocshArgBuf *args)
{
    dbAliasRecordsPrefix(args[0].sval, args[1].sval, args[2].ival);
}

static void dbAliasRecordsREInitCallFunc(const iocshArgBuf *args)
{
    dbAliasRecordsRE(args[0].sval, args[1].sval, args[2].ival);
}

static void dbLoadRecordsFuncsRegister(void)
{
    iocshRegister(&dbLoadRecordsLoopDef, dbLoadRecordsLoopInitCallFunc);
    iocshRegister(&dbLoadRecordsListDef, dbLoadRecordsListInitCallFunc);
    iocshRegister(&dbAliasRecordsPrefixDef, dbAliasRecordsPrefixInitCallFunc);
    iocshRegister(&dbAliasRecordsREDef, dbAliasRecordsREInitCallFunc);
}

epicsExportRegistrar(dbLoadRecordsFuncsRegister); // need to be declared via registrar() in utilities.dbd too

}
