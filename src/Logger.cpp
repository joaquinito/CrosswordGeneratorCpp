
#include "Logger.hpp"


std::string Logger::CreateLogFileName(char* cfgFile, char* wordsFile)
{
    logFileName = "LogCrossword_";
    logFileName.append(cfgFile);
    logFileName.append("_");
    logFileName.append(wordsFile);
    logFileName.append(".txt");

    return logFileName;
}


void Logger::StartLog(char* cfgFile, char* wordsFile)
{
    std::string logFileName = CreateLogFileName(cfgFile, wordsFile);

    FILE * logFile = fopen(logFileName.c_str(), "w+");

    time_t current_time_ = time(0);
    char * current_time = ctime(&current_time_);

    fprintf(logFile, "---------------------------------------------\n");
    fprintf(logFile, "%s\n", current_time);

    fclose(logFile);

}

void Logger::WriteToLog(std::string input)
{
    FILE * logFile = fopen(logFileName.c_str(), "a+"); // a+ will append new strings to end of file

    fprintf(logFile, "%s", input.c_str());

    fclose(logFile);
}

