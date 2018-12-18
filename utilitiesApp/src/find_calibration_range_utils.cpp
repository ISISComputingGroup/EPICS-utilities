#include <string.h>
#include <stdlib.h>
#include <epicsString.h>
#include <string>
#include <vector>

#include <sstream>
#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "find_calibration_range_utils.h"

/**
* Gets the next line of an input stream and splits on a comma.
*/
std::vector<std::string> getNextLineAndSplitOnComma(std::ifstream& str) {
    std::vector<std::string> parsed_line;
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    while (std::getline(lineStream, cell, ',')) {
        parsed_line.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty()) {
        // If there was a trailing comma then add an empty element.
        parsed_line.push_back("");
    }
    return parsed_line;
}

/**
* Extracts a std::string from an epics aSubRecord.
*/
std::string str_from_epics(void* raw_rec) {
    epicsOldString* rec = reinterpret_cast<epicsOldString*>(raw_rec);
    char buffer[sizeof(epicsOldString) + 1];  // +1 for null terminator in the case where epics str is exactly 40 chars (unterminated)
    buffer[sizeof(epicsOldString)] = '\0';
    return std::string(strncpy(buffer, *rec, sizeof(epicsOldString)));
}

/**
* Finds the directory to the table file
*/
std::string find_file(void* BDIR, void* TDIR, void* SPEC) {
    std::string base_directory = str_from_epics(BDIR);
    std::string table_directory = str_from_epics(TDIR);
    std::string filename = str_from_epics(SPEC);

    boost::trim(base_directory);
    boost::trim(table_directory);
    boost::trim(filename);

    return  base_directory + "/" + table_directory + "/" + filename;
}
