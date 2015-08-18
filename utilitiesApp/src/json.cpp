#include <string>
#include <list>
#include <libjson.h>
#include <epicsExport.h>

#include <utilities.h>

epicsShareFunc std::string json_list_to_array(std::list<std::string>& items)
{
	JSONNODE *a = json_new(JSON_ARRAY);

	for (std::list<std::string>::iterator it = items.begin(); it != items.end(); ++it)
	{
		std::string& item = *it;
		json_push_back(a, json_new_a(NULL, item.c_str()));
	}

	std::string out = json_write(a);

	json_delete(a);

	return out;
}