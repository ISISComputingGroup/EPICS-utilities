TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

USR_INCLUDES += -I"$(TOP)/../../../libraries/master/boost/include" -I"$(TOP)/../../../libraries/master/jsonformoderncpp"

# googleTest Runner
ifeq ($(findstring 10.0,$(VCVERSION)),)
SRC_DIRS += $(TOP)/utilitiesApp/src

GTESTPROD_HOST += runner
runner_SRCS += calibration_range_tests.cc find_calibration_range_tests.cc dbLoadRecordsFuncs_tests.cc asynInterposeSecop_tests.cpp
runner_SRCS += find_calibration_range_utils.cpp find_calibration_range_impl.cpp dbLoadRecordsFuncs.cpp asynInterposeSecop.cpp
runner_LIBS += asyn zlib pcrecpp pcre libjson
runner_LIBS += $(EPICS_BASE_IOC_LIBS)
GTESTS += runner
endif

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
-include $(GTEST)/cfg/compat.RULES_BUILD
