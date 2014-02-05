#ifndef UTILITIES_H
#define UTILITIES_H

epicsShareFunc std::string epicsShareAPI setIOCName(const char* iocName);
epicsShareFunc std::string epicsShareAPI getIOCName();
epicsShareFunc std::string epicsShareAPI getIOCGroup();
epicsShareFunc std::string epicsShareAPI trimString(const std::string& str);


#endif /* UTILITIES_H */