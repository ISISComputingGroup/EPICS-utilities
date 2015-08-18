#include <string>
#include <iostream>
#include <iterator>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include <epicsGetopt.h>
#include <epicsString.h>

#include <shareLib.h>

#include "utilities.h"

static void usage()
{
	std::cerr << "Usage: jcaput [-h] [-v] [-s#] [-1] [-i] [-w] pv [value1] [value2] [value3]" << std::endl;
	std::cerr << "Usage: -h                        this help page" << std::endl;
	std::cerr << "Usage: -v                        verbose output" << std::endl;
	std::cerr << "Usage: -s#                       use character # to split stdin into an array" << std::endl;
	std::cerr << "Usage: -1                        send one element array rather than a single number/string" << std::endl;
	std::cerr << "Usage: -i                        ignore whitespace in the input rather than creating an array" << std::endl;
	std::cerr << "Usage: -w#                       set timeout for channel access to #, default 1.0 sec" << std::endl;
	std::cerr << "Usage: pv                        required: process variable to send output to" << std::endl;
	std::cerr << "Usage: [value1] [value2] ...     optional: values to form array from (default: stdin)" << std::endl;
	std::cerr << " " << std::endl;
	std::cerr << "Usage: if no values are specified, stdin is read and split on the character specified by -s (default: space)" << std::endl;
	std::cerr << " " << std::endl;
	std::cerr << "Usage:      echo first,second,third|jcaput -v -s, SOME:PROCESS:VARIABLE" << std::endl;
	std::cerr << "Usage:      jcaput -v SOME:PROCESS:VARIABLE first second third" << std::endl;
	std::cerr << " " << std::endl;
}

// jcaput pv [value1] [value2] [value3]
// if no value, uses stdin
int main(int argc, char* argv[])
{
    std::string comp_str, str;
	std::list<std::string> items;
	const char* delims = " \t\r\n";
	int opt;
	long double waittime = 1.0f;
	bool single_element = false;
	bool verbose = false;
	bool no_whitespace = false;
    while ((opt = getopt(argc, argv, "1vhiw:s:")) != -1) 
	{
        switch (opt) {
        case 'h':             
            usage();
            return 0;
			
        case 'v':              
            verbose = true;
            break;

		case 's':              
            delims = strdup(optarg);
            break;
			
		case '1':              
            single_element = true;
            break;
		
		case 'i':
			no_whitespace = true;
			break;
		
		case 'w':
			waittime = atof(optarg);
			break;
			
		default:
			break;
		}
	}
	int nargs = argc - optind;
	const char* pv = argv[optind];
	if (nargs < 1)
	{
	    usage();
		return -1;
	}
	if (nargs == 1)
	{
	    std::getline(std::cin, str);
		char* buffer = strdup(str.c_str());
		char* saveptr;
		const char* item = epicsStrtok_r(buffer, delims, &saveptr);
		while( item != NULL )
		{
			items.push_back(item);
		    item = epicsStrtok_r(NULL, delims, &saveptr); 
		}
	}
	else
	{
		if (!no_whitespace)
		{
			for(int i=optind+1; i<argc; ++i)
			{
				items.push_back(argv[i]);
			}
		} else {
			std::string input = "";
			for(int i=optind+1; i<argc; ++i)
			{
				input.append(argv[i]);
				input.append(" ");
			}
			items.push_back(input);
		}
	}
	std::string json;
	if (!single_element && (items.size() == 1))
	{
	    json = "\"" + items.front() + "\"";
	}
	else
	{
	    json = json_list_to_array(items);
	}
	int ret = compressString(json, comp_str);
	if (ret == 0)
	{
	    std::ostringstream command;
	    command <<  "caput -S -w" << waittime <<  " " << pv << " " << comp_str;
		if (verbose)
		{
			std::cout << "jcaput: JSON: " << json << std::endl;
			std::cout << "jcaput: " << command.str() << std::endl;
		}
	    system(command.str().c_str());
    }		
	return ret;
}
