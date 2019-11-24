#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <iostream>
#include <vector>
#include "Crosswords.hpp"

#define MAX_LENGTH_OF_WORD 20

enum Orientation
{
    horizontal,
    vertical,
};


typedef struct BoardWordInfo_t
{
    uint8_t id;
    Orientation orientation;
    bool fixed;
    uint8_t size;
    uint8_t wordStartLin;
    uint8_t wordStartCol;
    std::vector<std::string> candidates;

} BoardWordInfo;

typedef struct Coordinates_t
{
    uint8_t lin;
    uint8_t col;

} Coordinates;


class Solver
{
    private:

        std::vector<std::string> WordBank[MAX_LENGTH_OF_WORD+1];

        std::vector<Coordinates> SharedSquares;

        std::vector<BoardWordInfo> BoardHorizontalWords;

        std::vector<BoardWordInfo> BoardVerticalWords;

        void ValidateHorizontalCandidates(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);

        void ValidateVerticalCandidates(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);

    public:
        
        Solver(FILE *wordsFile);

        Solver(FILE *wordsFile, Crosswords &inputCrosswords);

        uint8_t CreateWordBank(FILE * wordsFile, int maxLength);

        uint8_t FindSolution(Crosswords &inputCrosswords);

        void PrintBoardHorizontalWords();

        void PrintBoardVerticalWords();

        void PrintBoardSharedSquares();

        void PrintCandidateLists();

        

};

#endif