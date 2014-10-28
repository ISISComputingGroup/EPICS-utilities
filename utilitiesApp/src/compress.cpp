#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "zlib.h"

#include <boost/scoped_array.hpp>

#include <epicsExport.h>

#include "utilities.h"

/// compress a string usinf zlib and then convert compressed bytes into an ascii hex sequence
/// suitable for transmission via an EPICS character waveform record
epicsShareFunc int epicsShareAPI compressString(const std::string& str, std::string& comp_str)
{
        uLong len = str.size(); // not including terminating NULL
        uLong comprLen = compressBound(len); // we should not need more bytes than this
        boost::scoped_array<Byte> compr(new Byte[comprLen]);
		comp_str.resize(0);
        int err = compress(compr.get(), &comprLen, (const Bytef*)str.c_str(), len);
		if (err != Z_OK)
		{
			std::cerr << "compressString: compress failed with error " << err << std::endl;
			return -1;
		}
        std::ostringstream oss;
        for(int i=0; i<comprLen; ++i)
        {
            oss << std::hex << std::setfill ('0') << std::setw (2) << static_cast<unsigned>(compr[i]);
        }
        comp_str = oss.str();
		return 0;
}

/// uncompress a string created using compressString()
epicsShareFunc int epicsShareAPI uncompressString(const std::string& comp_str, std::string& str)
{
	int length = comp_str.length();
	str.resize(0);
	if (length == 0)
	{
		return 0;
	}
	if (length % 2 != 0) // strings created by compressString() are always an even number of bytes
	{
		std::cerr << "uncompressString: input length not an even number of characters" << std::endl;
		return -1;
	}
	if (comp_str.find_first_of(" \t\r\n") != std::string::npos)
	{
	    std::cerr << "uncompressString: whitespace characters found, did you forget the \"-t\" or \"-S\" options for caget?" << std::endl;
		return -1;
	}
	std::vector<Byte> compr;
	compr.reserve(length/2);
	for(int i=0; i < length; i+=2)
	{
		compr.push_back(static_cast<Byte>(strtol(comp_str.substr(i,2).c_str(), NULL, 16)));
	}
	uLong uncomprLen = std::max(length * 10, 65536); // we have to guess a maximum for this
	boost::scoped_array<Byte> uncompr(new Byte[uncomprLen]);
	int err = uncompress(uncompr.get(), &uncomprLen, &(compr[0]), compr.size()); 
	if (err != Z_OK)
	{
		std::cerr << "uncompressString: uncompress failed with error " << err << std::endl;
		return -1;
	}
	str.reserve(uncomprLen);
	for(int i=0; (i < uncomprLen); ++i)
	{
		str.push_back(static_cast<char>(uncompr[i]));
	}
    return 0;
}
