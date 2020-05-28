
#include "Logger.hpp"


void StartLog()
{
    FILE * logFile = fopen("LogCrossword.txt", "a+");

    time_t current_time_ = time(0);
    char * current_time = ctime(&current_time_);

    fprintf(logFile, "---------------------------------------------\n");
    fprintf(logFile, "%s\n", current_time);

    fclose(logFile);

}

void Log(std::string input)
{
    FILE * logFile = fopen("LogCrossword.txt", "a+"); // a+ will append new strings to end of file

    fprintf(logFile, "%s", input.c_str());

    fclose(logFile);
}

