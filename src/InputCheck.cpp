#include "InputCheck.hpp"

#include <iostream>
#include <cstdint>

#define STATUS_OK   0;
#define STATUS_NOK  1;

uint8_t CheckNumOfArgs(int argNum)
{
    uint8_t status = STATUS_OK;

    if(argNum < 3)
    {
        std::cout << "Error! Not enough arguments.\n";
        status = STATUS_NOK;
        garbage
    }

    return status;
}