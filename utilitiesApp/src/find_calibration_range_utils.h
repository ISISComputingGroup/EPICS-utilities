#ifndef find_calibration_range_utils
#define find_calibration_range_utils

std::vector<std::string> getNextLineAndSplitOnComma(std::ifstream& str);
std::string str_from_epics(void* raw_rec);
std::string find_file(void* BDIR, void* TDIR, void* SPEC);

#endif // find_calibration_range_utils
