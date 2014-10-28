#ifndef UTILITIES_H
#define UTILITIES_H

epicsShareFunc std::string epicsShareAPI setIOCName(const char* iocName);
epicsShareFunc std::string epicsShareAPI getIOCName();
epicsShareFunc std::string epicsShareAPI getIOCGroup();
epicsShareFunc std::string epicsShareAPI trimString(const std::string& str);
epicsShareFunc int epicsShareAPI compressString(const std::string& str, std::string& comp_str);
epicsShareFunc int epicsShareAPI uncompressString(const std::string& comp_str, std::string& str);

epicsShareFunc std::string epicsShareAPI getIOCName();
epicsShareFunc std::string epicsShareAPI getIOCGroup();

#ifdef _WIN32
#include "win32_dirent.h"
#else
#include <dirent.h>
#endif /* _WIN32 */

#endif /* UTILITIES_H */