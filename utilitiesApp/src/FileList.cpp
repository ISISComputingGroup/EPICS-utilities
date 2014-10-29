#include <string>
#include <list>
#include <iostream>
#include <pcre.h>

#include <epicsExport.h>

#include <utilities.h>

/// returns -1 if unable to open directory, else number of items in directory
epicsShareFunc int getFileList(const std::string& dirBase, std::list<std::string>& files)
{
	struct dirent *pDirent;
	DIR *pDir;
	files.clear();

	pDir = opendir(dirBase.c_str());

    if (pDir == NULL) {
        return -1;
    }

	//First two files are '.' and '..' so skip
	readdir(pDir);
	readdir(pDir);

    while ( (pDirent = readdir(pDir)) != NULL ) 
	{
       files.push_back(pDirent->d_name);
    }
    closedir (pDir);

	return files.size();

}
	

epicsShareFunc int filterList(std::list<std::string>& items, const std::string& regex)
{
	pcre *re;
	const char *error;
	int erroffset;

	re = pcre_compile(
	  regex.c_str(),              /* the pattern */
	  0,                    /* default options */
	  &error,               /* for error message */
	  &erroffset,           /* for error offset */
	  NULL);                /* use default character tables */

	if (re == NULL)
	{
		std::cerr << "PCRE compilation failed" << std::endl;
		return -1;
	}

	for (std::list<std::string>::iterator it = items.begin(); it != items.end(); )
	{
		std::string& item = *it; 
		if(pcre_exec(re, NULL, item.c_str(), static_cast<int>(item.length()), 0, 0, NULL, NULL) < 0)
			it = items.erase(it);
		else
			++it;
	}

	return 0;
}
