#ifndef asynInterposeSecop_H
#define asynInterposeSecop_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int secopParse(const std::string& secop_input, std::string& action, std::string& specifier, std::string& xml_data);
int asynInterposeSecopConfig(const char *portName, int addr, const char* options);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* asynInterposeSecop_H */
