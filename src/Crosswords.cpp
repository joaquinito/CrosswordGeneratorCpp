
#include "Crosswords.hpp"
#include <iostream>
#include <cstring>

Crosswords::Crosswords(FILE * cfgFile)
{
    SetBoardDimensions(cfgFile);
    Board = CreateBoard();
    PreFillBoard(cfgFile);

    return;
}


void Crosswords::SetBoardDimensions(FILE * cfgFile)
{
    fscanf(cfgFile, "%d %d\n", &numLin, &numCol);

    return;
}


std::vector<std::vector<Square>> Crosswords::CreateBoard()
{
    std::vector<std::vector<Square>> newBoard(numCol, std::vector<Square>(numLin));

    return newBoard;  
}


void Crosswords::PreFillBoard(FILE * cfgFile)
{
    char buffer[64];
    int ln, cn;
    int sq_l, sq_c;

    while(fscanf(cfgFile, "%s",buffer)!=EOF)
    {
        if((buffer[0]=='(') && (buffer[strlen(buffer)-1]==')'))
        {
            // Black Square coordinates
            sscanf(buffer,"(%d,%d)", &ln, &cn);
            Board[ln-1][cn-1].SetSquareColorBlack();
            Board[ln-1][cn-1].SetLetter('-');
        }
        else if(buffer[0]=='H')
        {
            // Fill horizontal word
            sscanf(buffer, "H%d", &sq_l);
            fscanf(cfgFile, "%d", &sq_c);
            fscanf(cfgFile, "%s", buffer);

            for(int i=0; i<strlen(buffer); i++)
            {
                Board[sq_l-1][sq_c-1+i].SetLetter(buffer[i]);
                Board[sq_l-1][sq_c-1+i].SetHWordFixed();
            }
        }
        else if(buffer[0]=='V')
        {
            // Fill vertical word
            sscanf(buffer, "V%d", &sq_c);
            fscanf(cfgFile, "%d", &sq_l);
            fscanf(cfgFile, "%s", buffer);

            for(int i=0; i<strlen(buffer); i++)
            {
                Board[sq_l-1+i][sq_c-1].SetLetter(buffer[i]);
                Board[sq_l-1+i][sq_c-1].SetVWordFixed();
            }
        }  
    }

    return;
}


void Crosswords::PrintBoard(PrintableBoardSymbols symbol)
{
    std::cout << "-------------------\n";

    for(int i=0; i<numLin; i++)
    {
        for(int j=0; j<numCol; j++)
        {
            switch(symbol)
            {
                case colors:
                    
                    std::cout << Board[i][j].GetSquareColor() << " ";
                    break;

                case letters:
                    
                    std::cout << Board[i][j].GetLetter() << " ";
                    break;
                
                case hWordIds:
                    
                    std::cout << Board[i][j].GetHWordId() << " ";
                    break;
                
                case hWordStart:
                    
                    std::cout << Board[i][j].IsHWordFixed() << " ";
                    break;

                case hWordFixed:
                    
                    std::cout << Board[i][j].GetLetter() << " ";
                    break;

                case vWordIds:
                    
                    std::cout << Board[i][j].GetLetter() << " ";
                    break;

                case vWordStart:
                    
                    std::cout << Board[i][j].GetLetter() << " ";
                    break;

                case vWordFixed:
                    
                    std::cout << Board[i][j].GetLetter() << " ";
                    break;

                default:
                    std::cout << "  ";
                    break;
            }

            
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------\n";
    
    return;
}