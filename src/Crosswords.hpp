#ifndef CROSSWORDS_HPP
#define CROSSWORDS_HPP

#include <iostream>
#include <vector>
#include "Square.hpp"

enum PrintableBoardSymbols
{
    colors, 
    letters, 
    hWordIds, 
    hWordStart, 
    hWordFixed, 
    hWordSize,
    hWordLetterPos,
    vWordIds, 
    vWordStart, 
    vWordFixed,
    vWordSize,
    vWordLetterPos
};

class Crosswords
{
    private:
        
        int numLin; // Number of lines in the board
        int numCol; // Number of columns in the board

        std::vector<std::vector<Square>> Board; // Board object (2-dimension vector of Squares)

        std::vector<int> wordSizes;

        std::vector<std::vector<Square>> CreateBoard();

        void PreFillBoard(FILE * cfgFile);

        void IdentifyHorizontalWords();

        void IdentifyVerticalWords();

    public:

        int numHWords;
        int numVHords;
 
        Crosswords(FILE * cfgFile);

        void SetBoardDimensions(FILE * cfgFile);
    
        int GetNumberOfLines();

        int GetNumberofColumns();

        int  GetMaxDimenson();
        
        Square GetSquareInfo(int lin, int col);

        void PrintBoard(PrintableBoardSymbols symbol);
};

#endif