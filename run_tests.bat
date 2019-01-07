:: Run all tests
@echo off
SET TOP="."

SET Tests_failed=%errorlevel%

:: Change the directory depending on if you have a src sub directory
call utilitiesApp\src\O.windows-x64\runner.exe --gtest_output=xml:%TOP%\test-reports\TEST-IOCName.xml

exit /B %Tests_failed%
