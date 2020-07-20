#include <string>
#include <list>
#include <map>
#include <libjson.h>
#include <epicsExport.h>

#include <utilities.h>

epicsShareFunc std::string json_list_to_array(const std::list<std::string>& items)
{
	JSONNODE *a = json_new(JSON_ARRAY);

	for (std::list<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		json_push_back(a, json_new_a(NULL, it->c_str()));
	}

	std::string out = json_write(a);

	json_delete(a);

	return out;
}

epicsShareFunc std::string json_map_to_node(const std::map<std::string, std::string>& items)
{
	JSONNODE *a = json_new(JSON_NODE);

	for (std::map<std::string, std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		json_push_back(a, json_new_a(it->first.c_str(), it->second.c_str()));
	}

	std::string out = json_write(a);

	json_delete(a);

	return out;
}