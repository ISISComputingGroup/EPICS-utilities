@ECHO OFF
setlocal
set "MYDIR=%~dp0"
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\support\utilities\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\support\libjson\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\support\pcre\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\support\utilities\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\support\zlib\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\support\oncrpc\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\support\googletest\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
XCOPY /R /I /Q /Y C:\Instrument\Apps\EPICS\base\master\bin\windows-x64\*.dll %MYDIR%..\..\bin\windows-x64
