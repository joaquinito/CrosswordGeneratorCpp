#include "InputCheck.hpp"
#include <iostream>
#include <cstdint>
#include <string.h>

/*
Check if number of arguments from command-line is equal of more than 2.
(argc also counts the binary execution command)
*/
uint8_t IsNumOfArgsValid(int argNum)
{
    uint8_t status = STATUS_OK;

    if(argNum < 3)
    {
        std::cout << "ERROR: Not enough arguments.\n";
        status = STATUS_NOK;
    }

    return status;
}

/*
Check if configuration file argument has a ".cfg" extension.
*/
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
        std::cout << "ERROR: First argument is not a .cfg file.\n";
        return STATUS_NOK;
    } 
}

/*
Check if words file argument has a ".txt" extension.
*/
uint8_t IsWordsFileValid(char* input)
{
    uint8_t i = strlen(input);

    if( (input[i-1] == 't') &&
        (input[i-2] == 'x') &&
        (input[i-3] == 't') &&
        (input[i-4] == '.'))
    {
        return STATUS_OK;  
    }
    else
    {
        std::cout << "ERROR: Second argument is not a .txt file.\n";
        return STATUS_NOK;
    } 
}

/*
Check if repetition flag argument has a valid syntax ( "-r<x>", where <x> is a positive integer).
*/
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