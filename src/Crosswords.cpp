
#include "Crosswords.hpp"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <algorithm>

Crosswords::Crosswords(FILE * cfgFile, std::string name)
{
    crosswordsName = name.substr(0, name.length()-4);
    SetBoardDimensions(cfgFile);
    Board = CreateBoard();
    PreFillBoard(cfgFile);
    IdentifyHorizontalWords();
    IdentifyVerticalWords();

    return;
}


void Crosswords::SetBoardDimensions(FILE * cfgFile)
{
    fscanf(cfgFile, "%d %d\n", &numLin, &numCol);

    return;
}

std::string Crosswords::GetName()
{
    return crosswordsName;
}

int Crosswords::GetNumberOfLines()
{
    return numLin;
}

int Crosswords::GetNumberofColumns()
{
    return numCol;
}

int Crosswords::GetMaxDimension()
{
    int maxDim = 0;

    if(numLin >= numCol)
    {
        maxDim = numLin;
    }
    else
    {
        maxDim = numCol;
    }

    return maxDim;
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
        if((buffer[0] == '(') && (buffer[strlen(buffer)-1] == ')'))
        {
            // Mark black square
            sscanf(buffer,"(%d,%d)", &ln, &cn);
            Board[ln-1][cn-1].SetSquareColorBlack();
            Board[ln-1][cn-1].SetLetter('-');
        }

        else if(buffer[0] == 'H')
        {
            // Fill horizontal word
            fscanf(cfgFile, "%d", &sq_l);
            fscanf(cfgFile, "%d", &sq_c);
            fscanf(cfgFile, "%s", buffer);

            for(int i=0; i<strlen(buffer); i++)
            {
                Board[sq_l-1][sq_c-1+i].SetLetter(buffer[i]);
                Board[sq_l-1][sq_c-1+i].SetHWordFixed();
            }
        }

        else if(buffer[0] == 'V')
        {
            // Fill vertical word
            fscanf(cfgFile, "%d", &sq_c);
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


void Crosswords::IdentifyHorizontalWords()
{
    int wordId = 1;
    int posCounter = 0;
    int sizeCounter = 0;    

    for(int i=0; i<numLin; i++)
    {
        for(int j=0; j<numCol; j++)
        {
            if(Board[i][j].GetSquareColor() == WHITE_SQUARE)
            {
                // If first column... 
                if(j == 0)
                {
                    // ...and if there's a white square after
                    if(Board[i][j+1].GetSquareColor() == WHITE_SQUARE) 
                    {
                        Board[i][j].SetHWordId(wordId);
                        Board[i][j].SetHWordStart();
                        Board[i][j].SetHWordLetterPos(posCounter);
                        wordId++;
                        posCounter++;
                        sizeCounter++;
                    }
                }
                // If not first column...
                else
                {
                    // ..and if there's a black square before and no black square after
                    if( (j > 0) && 
                        (j != numCol-1) && 
                        (Board[i][j-1].GetSquareColor() == BLACK_SQUARE) && 
                        (Board[i][j+1].GetSquareColor() == WHITE_SQUARE))
                    {
                        Board[i][j].SetHWordId(wordId);
                        Board[i][j].SetHWordStart();
                        wordId++;
                        posCounter++;
                        sizeCounter++;
                    }

                    // ...and if there's a white square before
                    else if((Board[i][j-1].GetSquareColor() == WHITE_SQUARE))
                    {
                        Board[i][j].SetHWordId(Board[i][j-1].GetHWordId());
                        Board[i][j].SetHWordLetterPos(posCounter);
                        posCounter++;
                        sizeCounter++;

                        // ... and if the next square is black or end of line
                        if( j == numCol-1 ||
                            Board[i][j+1].GetSquareColor() == BLACK_SQUARE)
                        {
                            for(int k = 0; k < sizeCounter; k++)
                            {
                                Board[i][j-k].SetHWordSize(sizeCounter);
                            } 

                            sizeCounter = 0;
                            posCounter = 0;
                        }
                    }
                }
            }
        }
    }
    
    numHWords = wordId;
    std::cout << "We have " << wordId-1 << " horizontal words.\n";
    
    return;
}

void Crosswords::IdentifyVerticalWords()
{
    int wordId = 1;
    int posCounter = 0;
    int sizeCounter = 0; 

    for(int j=0; j<numCol; j++)
    {
        for(int i=0; i<numLin; i++)
        {
            if(Board[i][j].GetSquareColor() == WHITE_SQUARE)
            {
                // If first line...
                if(i == 0)
                {
                    // ...and if there's a white square after
                    if(Board[i+1][j].GetSquareColor() == WHITE_SQUARE)
                    {
                        Board[i][j].SetVWordId(wordId);
                        Board[i][j].SetVWordStart();
                        Board[i][j].SetVWordLetterPos(posCounter);
                        wordId++;
                        posCounter++;
                        sizeCounter++;
                    }
                }
                   
                // If not first line...
                else
                {
                    // ...and if there's a black square before and no black square after
                    if( (i > 0) && 
                        (i != numLin-1) && 
                        (Board[i-1][j].GetSquareColor() == BLACK_SQUARE) && 
                        (Board[i+1][j].GetSquareColor() == WHITE_SQUARE))
                    {
                        Board[i][j].SetVWordId(wordId);
                        Board[i][j].SetVWordStart();
                        wordId++;
                        posCounter++;
                        sizeCounter++;
                    }

                    // ...and if there's a white square before
                    else if((Board[i-1][j].GetSquareColor() == WHITE_SQUARE))
                    {
                        Board[i][j].SetVWordId(Board[i-1][j].GetVWordId());
                        Board[i][j].SetVWordLetterPos(posCounter);
                        posCounter++;
                        sizeCounter++;

                        // ... and if the next square is black or end of column
                        if( i == numLin-1 || 
                            Board[i+1][j].GetSquareColor() == BLACK_SQUARE)
                        {
                            for(int k = 0; k < sizeCounter; k++)
                            {
                                Board[i-k][j].SetVWordSize(sizeCounter);
                            } 

                            sizeCounter = 0;
                            posCounter = 0;
                        }
                    }
                }
            }
        }
    }

    numVHords = wordId;
    std::cout << "We have " << wordId-1 << " vertical words.\n";

    return;
}

Square Crosswords::GetSquareInfo(int lin, int col)
{
    return Board[lin][col];
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
                    
                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill('0') << std::setw(2) << Board[i][j].GetHWordId() << " ";
                    }
                    
                    break;
                
                case hWordStart:
                    
                    std::cout << Board[i][j].IsHWordStart() << " ";
                    break;

                case hWordFixed:

                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill(' ') << std::setw(2) << Board[i][j].IsHWordFixed() << " ";
                    }

                    break;

                case hWordSize:

                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill(' ') << std::setw(2) <<  Board[i][j].GetHWordSize() << " ";
                    }

                    break;

                case hWordLetterPos:
                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill(' ') << std::setw(2) << Board[i][j].GetHWordLetterPos() << " ";
                    }

                    break;

                case vWordIds:
                    
                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill(' ') << std::setw(2) << Board[i][j].GetVWordId() << " ";
                    }
                    
                    break;

                case vWordStart:
                    
                    std::cout << Board[i][j].IsVWordStart() << " ";
                    break;

                case vWordFixed:

                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill(' ') << std::setw(2) << Board[i][j].IsVWordFixed() << " ";
                    }

                    break;

                case vWordSize:

                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill(' ') << std::setw(2) << Board[i][j].GetVWordSize() << " ";
                    }

                    break;

                case vWordLetterPos:

                    if (Board[i][j].GetSquareColor() == BLACK_SQUARE)
                    {
                        std::cout <<  "-- ";
                    }
                    else
                    {
                        std::cout << std::setfill(' ') << std::setw(2) << Board[i][j].GetVWordLetterPos() << " ";
                    }

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