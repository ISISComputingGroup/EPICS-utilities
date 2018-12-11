#include <string.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>
#include <epicsExport.h>

#include <vector>
#include <sstream>
#include <fstream>
#include <string>


#include "find_calibration_range_utils.h"

std::vector<std::string> getNextLineAndSplitOnComma(std::ifstream& str) {
    std::vector<std::string>   parsed_line;
    std::string                line;
    std::getline(str,line);

    std::stringstream          lineStream(line);
    std::string                cell;

    while(std::getline(lineStream,cell, ','))
    {
        parsed_line.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        parsed_line.push_back("");
    }
    return parsed_line;
}

// Finds the directory to the table file
std::string find_file(void* BDIR, void* TDIR, void* SPEC) {

    std::string base_directory(*((epicsOldString*)(BDIR)), sizeof(epicsOldString));
    std::string table_directory(*((epicsOldString*)(TDIR)), sizeof(epicsOldString));
    std::string filename(*((epicsOldString*)(SPEC)), sizeof(epicsOldString));
    
    std::stringstream directory;
    directory << base_directory << "\\" << table_directory << "\\" << filename;
    return directory.str();
}
