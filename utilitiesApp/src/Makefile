TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

USR_INCLUDES += -I"$(TOP)/../../libraries/boost/include"

LIBRARY_IOC += utilities

PROD_IOC += uzhex zhex

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install icpconfig.dbd into <top>/dbd
DBD += utilities.dbd
INC += utilities.h

# specify all source files to be compiled and added to the library
utilities_SRCS += iocname.cpp trimString.cpp ioccalc.cpp compress.cpp mkdir.cpp iocstringtest.cpp

utilities_LIBS += zlib
utilities_LIBS += $(EPICS_BASE_IOC_LIBS)

uzhex_SRCS += uzhex.cpp
uzhex_LIBS += utilities zlib
uzhex_LIBS += $(EPICS_BASE_IOC_LIBS)

zhex_SRCS += zhex.cpp
zhex_LIBS += utilities zlib
zhex_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

