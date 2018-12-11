#ifndef find_calibration_range_utils
#define find_calibration_range_utils

std::vector<std::string> getNextLineAndSplitOnComma(std::ifstream& str);
std::string find_file(void* BDIR, void* TDIR, void* SPEC);

#endif
