
#include "Solver.hpp"
#include <algorithm>
#include <cctype>
#include <iterator>

/*
    Default constuctor.
    Input: wordsFile
*/
Solver::Solver(FILE * wordsFile)
{
    CreateWordBank(wordsFile, MAX_LENGTH_OF_WORD);
}

/*
    Constructor with wordsFile and Crosswords as inputs.
    Will create a word bank with words smaller or equal to the max
    dimension of the crosswords board. 
*/
Solver::Solver(FILE * wordsFile, Crosswords &inputCrosswords)
{
    CreateWordBank(wordsFile, inputCrosswords.GetMaxDimension());
}


uint8_t Solver::CreateWordBank(FILE * wordsFile, int maxLength)
{
    char buffer[maxLength+1];
    std::string bufferString;
    int wordLength = 0;
    int numWords = 0;

    // Scan all words in the words file and place them in WordBank.
    // WordBank is an array of vectors of strings. The array has a fixed length of 20.
    // The array index refers to the word size. For example, all words of size 4 will be in WordBank[4].
    while(fscanf(wordsFile, "%s", buffer) != EOF)
    {
        bufferString = buffer;

        //Convert all letters to upper case
        std::for_each(bufferString.begin(), bufferString.end(), [](char &c)
        {
	        c = std::toupper(c);
        });

        wordLength = bufferString.size();
        
        if(wordLength <= maxLength)
        {
            WordBank[wordLength].push_back(bufferString);
        }
        
    }
    
    for(int j = 0; j <= maxLength; j++)
    {
        numWords += WordBank[j].size();
        //std::cout << "There are " << WordBank[j].size() << " words of size " << j << std::endl;

    }

    //std::cout << "Number of candidate words: " << numWords << std::endl;

    return true;
}


uint8_t Solver::FindSolution(Crosswords &inputCrosswords)
{   
    std::vector<std::string>::iterator itr;
    Square boardSquare;
    BoardWordInfo newWord;
    Coordinates newSharedSquare;
    bool noSolution = false;
    bool done = false;
    std::string outputFileDir = "out/";
    FILE * outputFile;
    std::string buffer;
    
    // Create list of horizontal words, list of vertical words, and list of squares that are shared by an horizontal 
    // and a vertical word.
    for(int i = 0; i < inputCrosswords.GetNumberOfLines(); i++)
    {
        for(int j = 0; j < inputCrosswords.GetNumberofColumns(); j++)
        {
            boardSquare = inputCrosswords.GetSquareInfo(i, j);

            if(boardSquare.IsHWordStart())
            {
                newWord.id = boardSquare.GetHWordId();
                newWord.orientation = horizontal;
                newWord.fixed = boardSquare.IsHWordFixed();
                newWord.wordStartLin = i;
                newWord.wordStartCol = j;
                newWord.size = boardSquare.GetHWordSize();
        
                BoardHorizontalWords.push_back(newWord);
            }

            if(boardSquare.IsVWordStart())
            {
                newWord.id = boardSquare.GetVWordId();
                newWord.orientation = vertical;
                newWord.fixed = boardSquare.IsVWordFixed();
                newWord.wordStartLin = i;
                newWord.wordStartCol = j;
                newWord.size = boardSquare.GetVWordSize();
        
                BoardVerticalWords.push_back(newWord);
            }

            if(boardSquare.GetHWordId() != 0 && boardSquare.GetVWordId() != 0)
            {
                newSharedSquare.lin = i;
                newSharedSquare.col = j;
                SharedSquares.push_back(newSharedSquare);
            }
        }
    }
    
    //PrintBoardHorizontalWords();
    //PrintBoardVerticalWords();  
    
    // Fill candidate list for each horizontal word with the words from the WordBank with the corresponding length 
    for(int i = 0; i < BoardHorizontalWords.size(); i++)
    {
        BoardHorizontalWords[i].candidates = WordBank[BoardHorizontalWords[i].size];
    }

    // Fill candidate list for each vertical word with the words from the WordBank with the corresponding length
    for(int i = 0; i < BoardVerticalWords.size(); i++)
    {
        BoardVerticalWords[i].candidates = WordBank[BoardVerticalWords[i].size];
    }

    //std::cout << "Candidate list before algorithm:" << std::endl;
    //PrintCandidateLists();

    // Iterate two times
    for(int c = 0; c < 2; c++)
    {
        //Go through each shared square
        for(int i = 0; i < SharedSquares.size(); i++)
        {
            int sharedSquareLin = SharedSquares[i].lin;
            int sharedSquareCol = SharedSquares[i].col;

            boardSquare = inputCrosswords.GetSquareInfo(sharedSquareLin, sharedSquareCol);

            int horId = boardSquare.GetHWordId();
            int verId = boardSquare.GetVWordId();

            int m = 0;
            int n = 0;

            // Go to word in BoardHorizontalWords list with the horizontal ID of the shared square 
            while(BoardHorizontalWords[m].id != horId)
            {
                m++;
            }

            // Go to word in BoardverticalWords list with the vertical ID of the shared square 
            while(BoardVerticalWords[n].id != verId)
            {
                n++;
            }
            
            ValidateHorizontalCandidates(boardSquare, BoardHorizontalWords, BoardVerticalWords, m, n);

            ValidateVerticalCandidates(boardSquare, BoardHorizontalWords, BoardVerticalWords, m, n);
            
        }   

        for(int i = 0; i < BoardHorizontalWords.size(); i++)
        {
            //If a vertical word has no possible candidates now, declare that there is no solution and abort. 
            if((BoardHorizontalWords[i].fixed != true) && (BoardHorizontalWords[i].candidates.size() == 0))
            {
               noSolution = true;
               done = true;
               break;
            }

            // If a horizontal word has only one possible candidate, put it in the chosen word.
            else if(BoardHorizontalWords[i].candidates.size() == 1)
            {
                BoardHorizontalWords[i].chosenWord = BoardHorizontalWords[i].candidates[0];
            }
        }

        for(int i = 0; i < BoardVerticalWords.size(); i++)
        {
            // If a vertical word has no possible candidates now, declare that there is no solution and abort. 
            if((BoardVerticalWords[i].fixed != true) && (BoardVerticalWords[i].candidates.size() == 0))
            {
               noSolution = true;
               done = true;
               break;
            }
            
            // If a vertical word has only one possible candidate, put it in the chosen word.
            else if(BoardVerticalWords[i].candidates.size() == 1)
            {
                BoardVerticalWords[i].chosenWord = BoardVerticalWords[i].candidates[0];
            }
        }

        if(noSolution == true)
        {
            done = true;
            break;
        }
    }

    std::cout << "Candidate list after first part of the algorithm:" << std::endl;
    PrintCandidateLists();

    if(noSolution == false)
    {
        for(int i = 0; i < BoardHorizontalWords.size(); i++)
        {
            
        }
        
    }


    

    // Writing of the output file
    if(noSolution == false)
    {
        outputFileDir = outputFileDir + inputCrosswords.GetName() + ".out";
        outputFile = fopen(outputFileDir.c_str(), "w");

        for(int i = 0; i < BoardHorizontalWords.size(); i++)
        {

            buffer = "H " + std::to_string(BoardHorizontalWords[i].wordStartLin) + " " + 
                        std::to_string(BoardHorizontalWords[i].wordStartCol) + " " +
                        BoardHorizontalWords[i].chosenWord + "\n";

            fwrite(buffer.c_str(), buffer.size(), 1, outputFile);
        }

        for(int i = 0; i < BoardVerticalWords.size(); i++)
        {
            buffer = "V " + std::to_string(BoardVerticalWords[i].wordStartLin) + " " + 
                        std::to_string(BoardVerticalWords[i].wordStartCol) + " " +
                        BoardVerticalWords[i].chosenWord + "\n";

            fwrite(buffer.c_str(), buffer.size(), 1, outputFile);
        }

        fclose(outputFile);
    }
    
    return !noSolution;
}

/*
For a given board square that is part of a horizontal and a vertical word, check if the words in the 
candidate list for the horizontal word are valid against the candidates for the vertical word.
Example:
- boardSquare contains the 2nd letter of H-Word, and 4rd letter of V-Word.
- First candidate for H1: "VIVER"
    - Search for a word in candidates of vertical word in which the 4th letter is 'I'.
        - If there is a match, keep "VIVER" in the candidates for H1. If there is no match, remove "VIVER" from the 
          candidates for H1.
- Repeat for all candidates. 
*/
void Solver::ValidateHorizontalCandidates(Square &boardSquare, 
                                          std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, 
                                          int &m, int &n)
{
    int horWordPos = boardSquare.GetHWordLetterPos();
    int verWordPos = boardSquare.GetVWordLetterPos(); 
    std::vector<std::string>::iterator itr;

    itr = BoardHorizontalWords[m].candidates.end();

    for(int j = BoardHorizontalWords[m].candidates.size()-1; j >= 0; j--)
    {
        bool match = false;

        if(boardSquare.IsVWordFixed())
        {
            if(BoardHorizontalWords[m].candidates[j][horWordPos] == boardSquare.GetLetter())
            {
                match = true;
            }
        }
        else
        {
            for(int k = 0; k < BoardVerticalWords[n].candidates.size(); k++)
        {
                /*      std::cout << BoardHorizontalWords[m].candidates[j] << "  " << BoardVerticalWords[n].candidates[k] << std::endl;
                        std::cout << "hor Letter: " << BoardHorizontalWords[m].candidates[j][horWordPos] << 
                        " ver Letter: " << BoardVerticalWords[n].candidates[k][verWordPos] << std::endl;
                */
                if(BoardHorizontalWords[m].candidates[j][horWordPos] == BoardVerticalWords[n].candidates[k][verWordPos])
                {
                    match = true;
                }
            }
        }
            
        itr--;
        if(match != true)
        {
            BoardHorizontalWords[m].candidates.erase(itr);
        }
    }

    return;
}


void Solver::ValidateVerticalCandidates(Square &boardSquare, 
                                        std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                        std::vector<BoardWordInfo> &BoardVerticalWords, 
                                        int &m, int &n)
{
    int horWordPos = boardSquare.GetHWordLetterPos();
    int verWordPos = boardSquare.GetVWordLetterPos(); 
    std::vector<std::string>::iterator itr;

    itr = BoardVerticalWords[n].candidates.end();

    for(int j = BoardVerticalWords[n].candidates.size()-1; j >= 0; j--)
    {
        bool match = false;

        if(boardSquare.IsHWordFixed())
        {
            if(BoardVerticalWords[n].candidates[j][verWordPos] == boardSquare.GetLetter())
            {
                match = true;
            }
        }
        else
        {
            for(int k = 0; k < BoardHorizontalWords[m].candidates.size(); k++)
            {

                /*     std::cout << BoardVerticalWords[n].candidates[j] << "  " << BoardHorizontalWords[m].candidates[k] << std::endl;
                            std::cout << "hor Letter: " << BoardHorizontalWords[m].candidates[k][horWordPos] << 
                            " ver Letter: " << BoardVerticalWords[n].candidates[j][verWordPos] << std::endl;
                */

                if(BoardVerticalWords[n].candidates[j][verWordPos] == BoardHorizontalWords[m].candidates[k][horWordPos])
                {
                    match = true;
                }
            }
        }
        
        itr--;

        if(match != true)
        {
            BoardVerticalWords[n].candidates.erase(itr);
        }
    }
    
    return;
}






void Solver::PrintBoardHorizontalWords()
{
    for(int i = 0; i < BoardHorizontalWords.size(); i++)
    {
        std::cout << "H" << (int)BoardHorizontalWords[i].id << "(" << (int)BoardHorizontalWords[i].wordStartLin
                << "." << (int)BoardHorizontalWords[i].wordStartCol << ") Size: " 
                << (int)BoardHorizontalWords[i].size << std::endl;
    }

    return;
}

void Solver::PrintBoardVerticalWords()
{
    for(int i = 0; i < BoardVerticalWords.size(); i++)
    {
        std::cout << "V" << (int)BoardVerticalWords[i].id << "(" << (int)BoardVerticalWords[i].wordStartLin
                << "." << (int)BoardVerticalWords[i].wordStartCol << ") Size: " 
                << (int)BoardVerticalWords[i].size << std::endl;
    }

    return;
}

void Solver::PrintBoardSharedSquares()
{
    std::cout << "\nShared Squares:\n";
    for(int i = 0; i < SharedSquares.size(); i++)
    {
        std::cout << "(" << (int)SharedSquares[i].lin << "." << (int)SharedSquares[i].col << ")" << std::endl;
    }

    return;
}

void Solver::PrintCandidateLists()
{
    for(int x = 0; x < BoardHorizontalWords.size(); x++)
        {
            std::cout << "Candidates for H" << (int)BoardHorizontalWords[x].id << ": ";

            for(int y = 0; y < BoardHorizontalWords[x].candidates.size(); y++)
            {
                std::cout << BoardHorizontalWords[x].candidates[y] << " ";
            }

            std::cout << std::endl;
        }

        for(int x = 0; x < BoardVerticalWords.size(); x++)
        {
            std::cout << "Candidates for V" << (int)BoardVerticalWords[x].id << ": ";

            for(int y = 0; y < BoardVerticalWords[x].candidates.size(); y++)
            {
                std::cout << BoardVerticalWords[x].candidates[y] << " ";
            }

            std::cout << std::endl;
        }

    return;
}

