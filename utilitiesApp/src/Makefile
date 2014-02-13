TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += utilities

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install icpconfig.dbd into <top>/dbd
DBD += utilities.dbd
INC += utilities.h

# specify all source files to be compiled and added to the library
utilities_SRCS += iocname.cpp trimString.cpp ioccalc.cpp

utilities_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

