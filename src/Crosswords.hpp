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
    vWordIds, 
    vWordStart, 
    vWordFixed 
};

class Crosswords
{
    private:
        
        int numLin;
        int numCol;
        std::vector<std::vector<Square>> Board;

    public:
 
        Crosswords(FILE * cfgFile);

        void SetBoardDimensions(FILE * cfgFile);
        
        std::vector<std::vector<Square>> CreateBoard();

        void PreFillBoard(FILE * cfgFile);

        void PrintBoard(PrintableBoardSymbols symbol);
};