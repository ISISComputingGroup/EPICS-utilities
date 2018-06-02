#include <windows.h>
#include <tlhelp32.h>

#include <string>

#include <epicsMutex.h>
#include <epicsGuard.h>

#include <epicsExport.h>

#include "utilities.h"

/// filetime uses 100ns units, returns difference in seconds
static double diffFileTimes(const FILETIME& f1, const FILETIME& f2)
{
	ULARGE_INTEGER u1, u2;
	u1.LowPart = f1.dwLowDateTime;
	u1.HighPart = f1.dwHighDateTime;
	u2.LowPart = f2.dwLowDateTime;
	u2.HighPart = f2.dwHighDateTime;
	return static_cast<double>(u1.QuadPart - u2.QuadPart) / 1e7;
}

/// returns -1.0 if process not running, else process uptime in seconds
epicsShareFunc double getProcessUptime(const char* procExecutableName)
{
  static epicsMutex procMutex;
  HANDLE hProcess;
  double procUptime = -1.0;
  int procCount = 0; // number of   procExecutableName   processes found
  PROCESSENTRY32 pe32;
  FILETIME procCreationTime, procExitTime, procKernelTime, procUserTime, sysTime;
  epicsGuard<epicsMutex> _lock(procMutex); // just to restrict number of simultaneous snapshots
  HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
	return procUptime;
  }
  pe32.dwSize = sizeof( PROCESSENTRY32 );
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );
    return procUptime;
  }
  do
  {
	if ( !stricmp(pe32.szExeFile, procExecutableName) )
	{
		++procCount;
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
        if( hProcess != NULL )
	    {
		   if (GetProcessTimes(hProcess, &procCreationTime, &procExitTime, &procKernelTime, &procUserTime) != 0)
		   {
               GetSystemTimeAsFileTime(&sysTime);
		       procUptime = diffFileTimes(sysTime, procCreationTime);
		   }
           CloseHandle( hProcess );
	    }
	}
  } while( Process32Next( hProcessSnap, &pe32 ) );
  CloseHandle( hProcessSnap );
  return procUptime;
}
