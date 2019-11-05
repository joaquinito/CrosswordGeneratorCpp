#include "InputCheck.hpp"
#include <iostream>
#include <cstdint>
#include <string.h>

uint8_t IsNumOfArgsValid(int argNum)
{
    uint8_t status = STATUS_OK;

    if(argNum < 3)
    {
        std::cout << "Error! Not enough arguments.\n";
        status = STATUS_NOK;
    }

    return status;
}

uint8_t IsCfgFileValid(char* input)
{
    uint8_t i = strlen(input);

    if( (input[i-1] == 'g') &&
        (input[i-2] == 'f') &&
        (input[i-3] == 'c') &&
        (input[i-4] == '.'))
    {
        return STATUS_OK;  
    }
    else
    {
        std::cout << "Error! First argument must be a .cfg file.\n";
        return STATUS_NOK;
    } 
}

uint8_t IsFlagR(char* input)
{
    if( (input[0] == '-') &&
        (input[1] == 'r') &&
        strlen(input)>2)
    {
        for(int i=2; i<strlen(input); i++)
        {
            if((input[i]-'0' <= 0) || (input[i]-'0' >= 9))
            {
                return STATUS_NOK;
            }
        }
        return STATUS_OK;
    }
    else
    {
        return STATUS_NOK;
    }
}