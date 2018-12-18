TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

USR_INCLUDES += -I"$(TOP)/../../../libraries/master/boost/include"

LIBRARY_IOC += utilities

PROD_IOC += uzhex zhex jcaput

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install icpconfig.dbd into <top>/dbd
DBD += utilities.dbd

INC += utilities.h win32_dirent.h

# specify all source files to be compiled and added to the library
utilities_SRCS += iocname.cpp trimString.cpp ioccalc.cpp iocdcalc.cpp compress.cpp mkdir.cpp iocstringtest.cpp 
utilities_SRCS += FileList.cpp json.cpp dbLoadRecordsFuncs.cpp iocshCmdFuncs.cpp freeIPPort.cpp
utilities_SRCS_WIN32 += win32_dirent.c getProcessUptime.cpp

utilities_SRCS += find_calibration_range.c find_calibration_range_impl.cpp

utilities_LIBS += zlib pcre libjson
utilities_LIBS += $(EPICS_BASE_IOC_LIBS)

utilities_SYS_LIBS_WIN32 += ws2_32

uzhex_SRCS += uzhex.cpp
uzhex_LIBS += utilities zlib pcre libjson
uzhex_LIBS += $(EPICS_BASE_IOC_LIBS)

zhex_SRCS += zhex.cpp
zhex_LIBS += utilities zlib pcre libjson
zhex_LIBS += $(EPICS_BASE_IOC_LIBS)

jcaput_SRCS += jcaput.cpp
jcaput_LIBS += utilities zlib pcre libjson
jcaput_LIBS += $(EPICS_BASE_IOC_LIBS)

# Copy DLLs that executables depend on, this avoids having to put additional things into PATH in config_env 
BIN_INSTALLS_WIN32 += $(wildcard $(ZLIB)/bin/$(EPICS_HOST_ARCH)/*.dll)
BIN_INSTALLS_WIN32 += $(wildcard $(LIBJSON)/bin/$(EPICS_HOST_ARCH)/*.dll)
BIN_INSTALLS_WIN32 += $(wildcard $(PCRE)/bin/$(EPICS_HOST_ARCH)/*.dll)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

