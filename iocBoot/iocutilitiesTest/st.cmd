#!../../bin/windows-x64-debug/utilitiesTest
## @file st.cmd

< envPaths

cd ${TOP}

# Register all support components
dbLoadDatabase "dbd/utilitiesTest.dbd"
utilitiesTest_registerRecordDeviceDriver pdbbase

# example usage of dbLoadRecordsLoop() and iocshCmdLoop() etc
# define a loop or list variable, then references to \$(VAR) will be replaced by loop/list values
# you can use \$(VAR) in any of the parameters, including the db file name
# the loop/list variable I here is passed to the DB file along with any other specified macros
# we now loop I over a range of 1 to 3
dbLoadRecordsLoop("db/utilitiesTest.db", "P=$(MYPVPREFIX),Q=A\$(I)", "I", 1, 3)

# here S is our list variable with values x, Y and Z
dbLoadRecordsList("db/utilitiesTest.db", "P=$(MYPVPREFIX),Q=A\$(S)", "S", "X;Y;Z", ";")

# as the loop/list variable is always passed though, we can instead iterate over Q
dbLoadRecordsList("db/utilitiesTest.db", "P=$(MYPVPREFIX)", "Q", "BX;BY;BZ", ";")

cd ${TOP}/iocBoot/${IOC}

# we can execute iocs commands in a similar way
iocshCmdLoop("dbgrep $(MYPVPREFIX)A\$(I)*", "", "I", 1, 3)
iocshCmdList("dbgrep $(MYPVPREFIX)A\$(S)*", "", "S", "X;Y;Z", ";")

# and also load files. The macro Q and I will be defined as temporary environment
# variables for use in the script and then reset back to their pre-script values
iocshCmdLoop("< st\$(I).cmd", "Q=Hello\$(I)", "I", 1, 2)

# confirm Q is unset on return
epicsEnvShow Q

iocInit
