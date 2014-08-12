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
	    str = argv[1];
	}
	else
	{
	    std::getline(std::cin, str);
//	    while(iit != eos)
//		{
//		    str.push_back(*iit);
//			++iit;
//		}
	}
	int ret = compressString(str, comp_str);
	if (ret == 0)
	{
	    std::cout << comp_str;
    }		
	return ret;
}