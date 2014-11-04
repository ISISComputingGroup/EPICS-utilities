#include <string>
#include <iostream>
#include <iterator>

#include <epicsGetopt.h>

#include <shareLib.h>

#include "utilities.h"

static void usage()
{
	std::cerr << "Usage: jcaput [-h] [-v] [-s#] [-1] pv [value1] [value2] [value3]" << std::endl;
	std::cerr << "Usage: -v    verbose output" << std::endl;
	std::cerr << "Usage: -s#   use character # to split stdin" << std::endl;
	std::cerr << "Usage: -1    send single element array rather than simple number/string" << std::endl;
	std::cerr << "Usage: if no values specified, stdin is read and split on space, or character specified by -s" << std::endl;
}

// jcaput pv [value1] [value2] [value3]
// if no value, uses stdin
int main(int argc, char* argv[])
{
    std::string comp_str, str;
	std::list<std::string> items;
	const char* delims = " \t\r\n";
	int opt;
	bool single_element = false;
	bool verbose = false;
    while ((opt = getopt(argc, argv, "1vhs:")) != -1) 
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
		const char* item = strtok(buffer, delims);
		while( item != NULL )
		{
			items.push_back(item);
		    item = strtok(NULL, delims); 
		}
	}
	else
	{
	    for(int i=optind+1; i<argc; ++i)
		{
			items.push_back(argv[i]);
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
	    std::string command = std::string("caput -S ") + pv + " " + comp_str;
		if (verbose)
		{
			std::cout << "jcaput: JSON: " << json << std::endl;
			std::cout << "jcaput: " << command << std::endl;
		}
	    system(command.c_str());
    }		
	return ret;
}