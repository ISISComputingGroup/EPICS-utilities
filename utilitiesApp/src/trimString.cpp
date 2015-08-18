#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include <epicsStdlib.h>
#include <epicsString.h>
#include <registryFunction.h>

#include <epicsExport.h>

#include "utilities.h"

epicsShareFunc std::string epicsShareAPI trimString(const std::string& str)
{
	static const char* whitespace = " \n\r\t\f\v";
    std::string s = str;
	size_t pos = s.find_last_not_of(whitespace);
	if (pos != std::string::npos)
	{
		s.erase(pos+1); // trim end
	}
	else
	{
		s.clear(); // all whitespace
	}
	pos = s.find_first_not_of(whitespace);
	if (pos != std::string::npos)
	{
		s.erase(0, pos); // trim start
	}
	return s;
}

//extern "C"
//{
//    epicsRegisterFunction(trimString); // needs to be in utilities.dbd as well
//}

