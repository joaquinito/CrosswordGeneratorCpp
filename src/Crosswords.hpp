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

        std::vector<uint8_t> wordSizes;

    public:
 
        Crosswords(FILE * cfgFile);

        void SetBoardDimensions(FILE * cfgFile);
        
        std::vector<std::vector<Square>> CreateBoard();

        void PreFillBoard(FILE * cfgFile);

        void PrintBoard(PrintableBoardSymbols symbol);

        void IdentifyHorizontalWords();

        void IdentifyVerticalWords();

        void GetWordSizes();
};