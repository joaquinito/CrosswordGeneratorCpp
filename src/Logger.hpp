
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <ctime>


class Logger
{
    private:
        std::string logFileName;

    public:
        std::string CreateLogFileName(char* cfgFile, char* wordsFile);

        void StartLog(char* cfg_file, char* words_file);

        void WriteToLog(std::string input);
};

#endif