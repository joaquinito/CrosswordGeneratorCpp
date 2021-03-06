/* 
 * AED Project 2011 in C++
 * 
 *
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <memory>
#include "Logger.hpp"
#include "InputCheck.hpp"
#include "Crosswords.hpp"
#include "Solver.hpp"


// argv[1]: cfg file
// argv[2]: words
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

    Logger Log;

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
        Log.StartLog(argv[1], argv[2]);
        Log.WriteToLog("Creating table.\n");

        std::unique_ptr<Crosswords> crosswordBoard(new Crosswords(cfgFile, argv[1], Log));
        
        crosswordBoard->PrintBoard(colors);
        crosswordBoard->PrintBoard(letters);
        crosswordBoard->PrintBoard(hWordIds);
        crosswordBoard->PrintBoard(hWordStart);
        crosswordBoard->PrintBoard(hWordFixed);
        crosswordBoard->PrintBoard(hWordSize);
        crosswordBoard->PrintBoard(hWordLetterPos);
        crosswordBoard->PrintBoard(vWordIds);
        crosswordBoard->PrintBoard(vWordStart);
        crosswordBoard->PrintBoard(vWordIds);
        crosswordBoard->PrintBoard(vWordSize);
        crosswordBoard->PrintBoard(vWordLetterPos);

        
        std::unique_ptr<Solver> theSolution(new Solver(wordsFile, *crosswordBoard, Log));

        if(!theSolution->FindSolution(*crosswordBoard))
        {
            std::cout << "No solution was found." << std::endl;
        };
        
        crosswordBoard->PrintBoard(letters);
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