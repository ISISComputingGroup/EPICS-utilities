#include <string>
#include <iostream>
#include <iterator>

#include <shareLib.h>
#include "utilities.h"

int main(int argc, char* argv[])
{
    std::string comp_str, str;
	std::istreambuf_iterator<char> iit (std::cin.rdbuf()); 
	std::istreambuf_iterator<char> eos; 
	if (argc >= 2)
	{
	    comp_str = argv[1];
	}
	else
	{
	    std::getline(std::cin, comp_str);
//	    while(iit != eos)
//		{
//		    comp_str.push_back(*iit);
//			++iit;
//		}
	}
	if (comp_str.find_first_of(" \t\r\n") != std::string::npos)
	{
	    std::cerr << "uzhex: whitespace characters found, did you forget the \"-t\" or \"-S\" options for caget?" << std::endl;
		return -1;
	}
	int ret = uncompressString(comp_str, str);
	if (ret == 0)
	{
	    std::cout << str;
    }		
	return ret;
}