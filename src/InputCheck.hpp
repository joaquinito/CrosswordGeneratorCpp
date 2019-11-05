#include <cstdint>

#define STATUS_OK   0
#define STATUS_NOK  1

uint8_t IsNumOfArgsValid(int argNum);

uint8_t IsCfgFileValid(char* input);

uint8_t IsFlagR(char* input);