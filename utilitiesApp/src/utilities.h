#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <list>

epicsShareFunc std::string epicsShareAPI setIOCName(const char* iocName);
epicsShareFunc std::string epicsShareAPI getIOCName();
epicsShareFunc std::string epicsShareAPI getIOCGroup();
epicsShareFunc std::string epicsShareAPI trimString(const std::string& str);
epicsShareFunc int epicsShareAPI compressString(const std::string& str, std::string& comp_str);
epicsShareFunc int epicsShareAPI uncompressString(const std::string& comp_str, std::string& str);

epicsShareFunc std::string epicsShareAPI getIOCName();
epicsShareFunc std::string epicsShareAPI getIOCGroup();

epicsShareFunc int getFileList(const std::string& dirBase, std::list<std::string>& files);
epicsShareFunc int filterList(std::list<std::string>& items, const std::string& regex);

epicsShareFunc std::string json_list_to_array(std::list<std::string>& items);

epicsShareFunc void dbLoadRecordsLoop(const char* dbFile, const char* macros, const char* loopVar, int start, int stop, int step);
epicsShareFunc void dbLoadRecordsList(const char* dbFile, const char* macros, const char* loopVar, const char* list, const char* sep);

epicsShareFunc void iocshCmdLoop(const char* command, const char* macros, const char* loopVar, int start, int stop, int step);
epicsShareFunc void iocshCmdList(const char* command, const char* macros, const char* loopVar, const char* list, const char* sep);


#ifdef _WIN32
#include "win32_dirent.h"
#else
#include <dirent.h>
#endif /* _WIN32 */

#endif /* UTILITIES_H */