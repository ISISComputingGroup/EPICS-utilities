#ifdef __cplusplus
extern "C" {
#endif

    extern long find_calibration_range_impl(aSubRecord *prec);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
    static std::vector<std::string> getNextLineAndSplitOnComma(std::ifstream& str);
    static std::string str_from_epics(void* raw_rec);
    static std::string find_file(void* BDIR, void* TDIR, void* SPEC);
#endif
