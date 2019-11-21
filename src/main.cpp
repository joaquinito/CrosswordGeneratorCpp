/* 
 * AED Project 2011 in C++
 * 
 *
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <memory>
#include "InputCheck.hpp"
#include "Crosswords.hpp"

int main(int argc, char *argv[])
{

    std::string cfgFileDir = "table_cfg/";
    std::string wordsFileDir = "words/";
    std::string outFileDir = "out/";
    std::string outputFileDir;
    
    FILE *cfgFile = NULL;
    FILE *wordsFile = NULL;
    FILE *outputFile = NULL;
    
    uint8_t reps;
    uint8_t status;

    status = IsNumOfArgsValid(argc);

    if(status == STATUS_OK)
    {
        status = IsCfgFileValid(argv[1]);
    }
    
    if(status == STATUS_OK)
    {
        cfgFileDir += argv[1];

        cfgFile = fopen(cfgFileDir.c_str(), "r");
        
        if(cfgFile == NULL)
        {
            std::cout << "ERROR: Configuration file not found.\n";
            status = STATUS_NOK;
        }
    }

    if(status == STATUS_OK)
    {
        status = IsWordsFileValid(argv[2]);
    }

    if(status == STATUS_OK)
    {
        wordsFileDir += argv[2];

        wordsFile = fopen(wordsFileDir.c_str(), "r");

        if(wordsFile == NULL)
        {
            std::cout << "ERROR: Words file not found.\n";
            status = STATUS_NOK;
        }
    }
    
    if(status == STATUS_OK)
    {
        if((argc > 3) && (IsFlagR(argv[3])==1))
        {
            reps = 0;
            for(int i = 2; i < strlen(argv[3]); i++)
            {
                reps = (reps*10) + (argv[3][i]-'0');
            }
        }
    }
    
    if(status == STATUS_OK)
    {
        std::unique_ptr<Crosswords> myCrosswordsBoard(new Crosswords(cfgFile));
        
        //myCrosswordsBoard->PrintBoard(colors);
        myCrosswordsBoard->PrintBoard(letters);
        //myCrosswordsBoard->PrintBoard(hWordIds);
        //myCrosswordsBoard->PrintBoard(hWordStart);
        //myCrosswordsBoard->PrintBoard(hWordFixed);
        //myCrosswordsBoard->PrintBoard(hWordSize);
        //myCrosswordsBoard->PrintBoard(hWordLetterPos);
        //myCrosswordsBoard->PrintBoard(vWordIds);
        //myCrosswordsBoard->PrintBoard(vWordStart);
        //myCrosswordsBoard->PrintBoard(vWordIds);
        //myCrosswordsBoard->PrintBoard(vWordSize);
        //myCrosswordsBoard->PrintBoard(vWordLetterPos);
       
    }

    if(cfgFile != NULL)
    {
        fclose(cfgFile);
    }
    
    if(wordsFile != NULL)
    {
        fclose(wordsFile);
    }
    
    if(outputFile != NULL)
    {
        fclose(outputFile);
    }

    return status;
}