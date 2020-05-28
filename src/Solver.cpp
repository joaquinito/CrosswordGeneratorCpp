
#include "Solver.hpp"
#include <algorithm>
#include <cctype>
#include <iterator>

/*
*   Default constuctor.
*   Input: wordsFile
*/
Solver::Solver(FILE * wordsFile)
{
    CreateWordBank(wordsFile, MAX_LENGTH_OF_WORD);
}

/*
*   Constructor with wordsFile and Crosswords as inputs.
*   Will create a word bank with words smaller or equal to the max
*   dimension of the crosswords board. 
*/
Solver::Solver(FILE * wordsFile, Crosswords &inputCrosswords)
{
    CreateWordBank(wordsFile, inputCrosswords.GetMaxDimension());
}

/*


*/
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
    }

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
                newWord.wordStartLin = i;
                newWord.wordStartCol = j;
                newWord.size = boardSquare.GetHWordSize();

                if(boardSquare.IsHWordFixed())
                {
                    newWord.state = FIXED;
                }
                else
                {
                    newWord.state = UNSELECTED;
                }
                
                BoardHorizontalWords.push_back(newWord);
            }

            if(boardSquare.IsVWordStart())
            {
                newWord.id = boardSquare.GetVWordId();
                newWord.orientation = vertical;
                newWord.wordStartLin = i;
                newWord.wordStartCol = j;
                newWord.size = boardSquare.GetVWordSize();

                if(boardSquare.IsVWordFixed())
                {
                    newWord.state = FIXED;
                }
                else
                {
                    newWord.state = UNSELECTED;
                }
        
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

    /* Sort the vertical words by increasing order of the ID 
    (see: https://stackoverflow.com/questions/279854/how-do-i-sort-a-vector-of-pairs-based-on-the-second-element-of-the-pair) */
    sort(BoardVerticalWords.begin(), BoardVerticalWords.end(), 
        [](const BoardWordInfo &left, const BoardWordInfo &right) { return left.id < right.id; });
    
    //PrintBoardHorizontalWords();
    //PrintBoardVerticalWords();  
    
    // Fill candidate list for each horizontal word with the words from the WordBank with the corresponding length 
    for(int i = 0; i < BoardHorizontalWords.size(); i++)
    {
        Candidate newCandidate;

        for(int j = 0; j < WordBank[BoardHorizontalWords[i].size].size(); j++)
        {
            newCandidate.candidateWord = WordBank[BoardHorizontalWords[i].size][j];
            // All candidates start by default with this flag as true - this will be used in the second stage of the algorithm
            newCandidate.isNominated = true; 

            BoardHorizontalWords[i].candidates.push_back(newCandidate);
        }
    }

    // Fill candidate list for each vertical word with the words from the WordBank with the corresponding length
    for(int i = 0; i < BoardVerticalWords.size(); i++)
    {
        Candidate newCandidate;

        for(int j = 0; j < WordBank[BoardVerticalWords[i].size].size(); j++)
        {
            newCandidate.candidateWord = WordBank[BoardVerticalWords[i].size][j];
            // All candidates start by default with this flag as true - this will be used in the second stage of the algorithm
            newCandidate.isNominated = true;

            BoardVerticalWords[i].candidates.push_back(newCandidate);
        }

       // BoardVerticalWords[i].candidates = WordBank[BoardVerticalWords[i].size];
    }

    //std::cout << "Candidate list before algorithm:" << std::endl;
    //PrintCandidateLists();

    // Iterate two times
    for(int c = 0; c < 2; c++)
    {
        SharedSquaresCheck(inputCrosswords, ALGORITHM_FIRST_STAGE);

        for(int i = 0; i < BoardHorizontalWords.size(); i++)
        {
            //If a vertical word has no possible candidates now, declare that there is no solution and abort. 
            if((BoardHorizontalWords[i].state != FIXED) && (BoardHorizontalWords[i].candidates.size() == 0))
            {
               noSolution = true;
               done = true;
               break;
            }

            // If a horizontal word has only one possible candidate, put it in the chosen word.
            else if(BoardHorizontalWords[i].candidates.size() == 1)
            {
                BoardHorizontalWords[i].chosenWord = BoardHorizontalWords[i].candidates[0].candidateWord;
                BoardHorizontalWords[i].state = CHOSEN;
            }
        }

        for(int i = 0; i < BoardVerticalWords.size(); i++)
        {
            // If a vertical word has no possible candidates now, declare that there is no solution and abort. 
            if((BoardVerticalWords[i].state != FIXED) && (BoardVerticalWords[i].candidates.size() == 0))
            {
               noSolution = true;
               done = true;
               break;
            }
            
            // If a vertical word has only one possible candidate, put it in the chosen word.
            else if(BoardVerticalWords[i].candidates.size() == 1)
            {
                BoardVerticalWords[i].chosenWord = BoardVerticalWords[i].candidates[0].candidateWord;
                BoardVerticalWords[i].state = CHOSEN;
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
        
        printf("\nAlgorithm Second Stage\n\n");

        // For each horizontal word to be filled in the board
        for(int i = 0; i < BoardHorizontalWords.size(); i++)
        {
            int aux = 0; // SOMETHING VERY WRONG WITH THIS VARIABLE
            int escape = 0;

            std::cout << "Hor number " << i << std::endl;

            // If the word is not pre-filled and has more than one candidate
            if((BoardHorizontalWords[i].state != FIXED) && (BoardHorizontalWords[i].state != CHOSEN))
            {
                while(escape != 1)
                {
                   // printf("aux = %d\n", aux);
                    while(BoardHorizontalWords[i].candidates[aux].isNominated != true)
                    {
                        aux++; 
                    //    printf("aux = %d\n", aux);
                    }
                    if(aux > BoardHorizontalWords[i].candidates.size())
                    {
                        return 1;
                    }
                   
                    printf("aux = %d\n", aux);

                    BoardHorizontalWords[i].state = SELECTED;
                    BoardHorizontalWords[i].chosenWord = BoardHorizontalWords[i].candidates[aux].candidateWord; // <- PROBLEM HERE!!!
                   
                   // std::cout << "Trying " << BoardHorizontalWords[i].chosenWord << " for HWord " << (int)BoardHorizontalWords[i].id << std::endl;
      
                    // If checking of shared squares in unsucessful, reset the nominations and try another word
                    if(SharedSquaresCheck(inputCrosswords, ALGORITHM_SECOND_STAGE) == FAILURE)
                    {
                        aux++;

                        for(int p = 0; p < BoardHorizontalWords.size(); p++)
                        {
                            if(BoardHorizontalWords[p].state != FIXED || BoardHorizontalWords[p].state != SELECTED)
                            {
                                for(int q = 0; q < BoardHorizontalWords[p].candidates.size(); q++)
                                {
                                    BoardHorizontalWords[p].candidates[q].isNominated = true;
                                }
                            }
                        }
                        for(int p = 0; p < BoardVerticalWords.size(); p++)
                        {
                            if(BoardVerticalWords[p].state != FIXED || BoardVerticalWords[p].state != SELECTED)
                            {
                                for(int q = 0; q < BoardVerticalWords[p].candidates.size(); q++)
                                {
                                    BoardVerticalWords[p].candidates[q].isNominated = true;
                                }
                            }
                        }
                    } 
                    else
                    {
                        escape = 1;
                    }
                }
            }
        }

        // For each vertical word to be filled in the board
        for(int i = 0; i < BoardVerticalWords.size(); i++)
        {
            int aux = 0;
            int escape = 0;

            std::cout << "Ver number " << i << std::endl;

            // If the word is not pre-filled and has more than one candidate
            if((BoardVerticalWords[i].state != FIXED) && (BoardVerticalWords[i].state != CHOSEN))
            {
                while(escape != 1)
                {
                    while(BoardVerticalWords[i].candidates[aux].isNominated != true)
                    {
                        aux++;
                    }

                    printf("aux = %d\n", aux);

                    BoardVerticalWords[i].state = SELECTED;
                    BoardVerticalWords[i].chosenWord = BoardVerticalWords[i].candidates[aux].candidateWord;

                  //  std::cout << "Trying " << BoardVerticalWords[i].chosenWord << " for VWord " << (int)BoardVerticalWords[i].id << std::endl;

                    // If checking of shared squares in unsucessful, reset the nominations and try another word
                    if(SharedSquaresCheck(inputCrosswords, ALGORITHM_SECOND_STAGE) == FAILURE)
                    {
                        aux++;

                        for(int p = 0; p < BoardHorizontalWords.size(); p++)
                        {
                            if(BoardHorizontalWords[p].state != FIXED || BoardHorizontalWords[p].state != SELECTED)
                            {
                                for(int q = 0; q < BoardHorizontalWords[p].candidates.size(); q++)
                                {
                                    BoardHorizontalWords[p].candidates[q].isNominated = true;
                                }
                            }
                        }
                        for(int p = 0; p < BoardVerticalWords.size(); p++)
                        {
                            if(BoardVerticalWords[p].state != FIXED || BoardVerticalWords[p].state != SELECTED)
                            {
                                for(int q = 0; q < BoardVerticalWords[p].candidates.size(); q++)
                                {
                                    BoardVerticalWords[p].candidates[q].isNominated = true;
                                }
                            }
                        }
                    } 
                    else
                    {
                        escape = 1;
                    }
                }
            }
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


uint8_t Solver::SharedSquaresCheck(Crosswords &inputCrosswords, uint8_t algorithmStage)
{
    Square boardSquare;
    int sharedSquareLin;
    int sharedSquareCol;
    int horId;
    int verId;
    int m;
    int n;
    uint8_t returnStatus;

    // Go through each shared square
    for(int i = 0; i < SharedSquares.size(); i++)
    {
        sharedSquareLin = SharedSquares[i].lin;
        sharedSquareCol = SharedSquares[i].col;

        boardSquare = inputCrosswords.GetSquareInfo(sharedSquareLin, sharedSquareCol);

        horId = boardSquare.GetHWordId();
        verId = boardSquare.GetVWordId();

        m = 0;
        n = 0;

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
        
        if(algorithmStage == ALGORITHM_FIRST_STAGE)
        {
            ValidateHorizontalCandidates(boardSquare, BoardHorizontalWords, BoardVerticalWords, m, n);
            ValidateVerticalCandidates(boardSquare, BoardHorizontalWords, BoardVerticalWords, m, n);
        }
        else if(algorithmStage == ALGORITHM_SECOND_STAGE)
        {
          //  std::cout << "Testing square [" << (int)SharedSquares[i].lin << "][" << (int)SharedSquares[i].col << "]\n" << std::endl;

            returnStatus = ValidateHorizontalCandidates_SecondStage(boardSquare, BoardHorizontalWords, BoardVerticalWords, m, n);

            if(returnStatus == SUCCESS)
            {
                returnStatus = ValidateVerticalCandidates_SecondStage(boardSquare, BoardHorizontalWords, BoardVerticalWords, m, n);
            }

            if(returnStatus != SUCCESS)
            {
                break;
            }   
        }        
    }  
    
    return returnStatus;
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
    std::vector<Candidate>::iterator itr;
    bool match;

    itr = BoardHorizontalWords[m].candidates.end();

    for(int j = BoardHorizontalWords[m].candidates.size()-1; j >= 0; j--)
    {
        match = false;

        if(boardSquare.IsVWordFixed())
        {
            if(BoardHorizontalWords[m].candidates[j].candidateWord[horWordPos] == boardSquare.GetLetter())
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
                if(BoardHorizontalWords[m].candidates[j].candidateWord[horWordPos] == 
                    BoardVerticalWords[n].candidates[k].candidateWord[verWordPos])
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
    std::vector<Candidate>::iterator itr;
    bool match;

    itr = BoardVerticalWords[n].candidates.end();

    for(int j = BoardVerticalWords[n].candidates.size()-1; j >= 0; j--)
    {
        match = false;

        if(boardSquare.IsHWordFixed())
        {
            if(BoardVerticalWords[n].candidates[j].candidateWord[verWordPos] == boardSquare.GetLetter())
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

                if(BoardVerticalWords[n].candidates[j].candidateWord[verWordPos] == 
                    BoardHorizontalWords[m].candidates[k].candidateWord[horWordPos])
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

uint8_t Solver::ValidateHorizontalCandidates_SecondStage(
    Square &boardSquare, 
    std::vector<BoardWordInfo> &BoardHorizontalWords, 
    std::vector<BoardWordInfo> &BoardVerticalWords, 
    int &m, int &n)
{
    int horWordPos = boardSquare.GetHWordLetterPos();
    int verWordPos = boardSquare.GetVWordLetterPos();

    uint8_t returnStatus = SUCCESS;

    bool match = false;
    bool aMatchWasFound = false;

    //std::cout << "State of HWord " << (int)BoardHorizontalWords[m].id << ": " << (int)BoardHorizontalWords[m].state << std::endl;

    // If the horizontal word has not been chosen or selected
    if(BoardHorizontalWords[m].state != FIXED && BoardHorizontalWords[m].state != CHOSEN && BoardHorizontalWords[m].state != SELECTED)
    {
        // Iterate on all candidates for that horizontal word
        for(int i = 0; i < BoardHorizontalWords[m].candidates.size(); i++)
        {
            match = false;
            
            // If the candidate is nominated
            if(BoardHorizontalWords[m].candidates[i].isNominated)
            {
                // If the vertical word is fixed, we test with that fixed letter
                if(BoardVerticalWords[n].state == FIXED)
                {
                    if((BoardHorizontalWords[m].candidates[i].candidateWord[horWordPos] == boardSquare.GetLetter()) &&
                        (BoardHorizontalWords[m].candidates[i].isNominated))
                    {
                        BoardHorizontalWords[m].candidates[i].isNominated = true;
                        match = true;
                        aMatchWasFound = true;

                      //  std::cout << "For HorWord " << (int)BoardHorizontalWords[m].id <<  ", the word " <<  BoardHorizontalWords[m].candidates[i].candidateWord << " is nominated" << std::endl;
                    }
                    else
                    {
                        BoardHorizontalWords[m].candidates[i].isNominated = false;
                    }
                }    

                // If the vertical word has been chosen or selected, we test with that chosen/nominated word
                else if(BoardVerticalWords[n].state == CHOSEN || BoardVerticalWords[n].state == SELECTED)
                {
                    if((BoardHorizontalWords[m].candidates[i].candidateWord[horWordPos] == 
                        BoardVerticalWords[n].chosenWord[verWordPos]) &&
                        (BoardHorizontalWords[m].candidates[i].isNominated))
                    {
                        BoardHorizontalWords[m].candidates[i].isNominated = true;
                        match = true;
                        aMatchWasFound = true;

                     //   std::cout << "For HorWord " << (int)BoardHorizontalWords[m].id <<  ", the word " <<  BoardHorizontalWords[m].candidates[i].candidateWord << " is nominated" << std::endl;
                    }
                    else
                    {
                        BoardHorizontalWords[m].candidates[i].isNominated = false;
                    }
                    
                }
                // If the vertical word has not been chosen or selected yet, we test every vertical candidate with the horizontal candidate
                else
                {
                    for(int k = 0; k < BoardVerticalWords[n].candidates.size(); k++)
                    {
                        if(BoardHorizontalWords[m].candidates[i].candidateWord[horWordPos] == 
                            BoardVerticalWords[n].candidates[k].candidateWord[verWordPos] &&
                        (BoardHorizontalWords[m].candidates[i].isNominated) && BoardVerticalWords[n].candidates[k].isNominated)
                        {
                            BoardHorizontalWords[m].candidates[i].isNominated = true;
                            match = true;
                            aMatchWasFound = true;

                      //      std::cout << "For HorWord " << (int)BoardHorizontalWords[m].id <<  ", the word " <<  
                      //          BoardHorizontalWords[m].candidates[i].candidateWord << " is nominated because it's compatible with word " <<
                      //          BoardVerticalWords[n].candidates[k].candidateWord << std::endl;
                        }
                                        
                    }

                    // If there was no match in the vertical candidates for this horizontal candidate, discard it from the nominations
                    if(match == false)
                    {
                        BoardHorizontalWords[m].candidates[i].isNominated = false;
                    }
                }
            }
        }

        if(aMatchWasFound == false)
        {
            returnStatus = FAILURE;
        }
    }

    //printf("\n");

    return returnStatus;
}

uint8_t Solver::ValidateVerticalCandidates_SecondStage(
    Square &boardSquare, 
    std::vector<BoardWordInfo> &BoardHorizontalWords, 
    std::vector<BoardWordInfo> &BoardVerticalWords, 
    int &m, int &n)
{

    int horWordPos = boardSquare.GetHWordLetterPos();
    int verWordPos = boardSquare.GetVWordLetterPos(); 

    uint8_t returnStatus = SUCCESS;

    bool match = false;
    bool aMatchWasFound = false;

    // If the vertical word has not been chosen or selected, we proceed with the nominations. Otherwise, no need.
    if(BoardVerticalWords[n].state != FIXED && BoardVerticalWords[n].state != CHOSEN && BoardVerticalWords[n].state != SELECTED)
    {   

        // Iterate on all candidates for that vertical word
        for(int i = 0; i < BoardVerticalWords[n].candidates.size(); i++)
        {
            
            match = false; 
            // If the candidate is nominated
            if(BoardVerticalWords[n].candidates[i].isNominated)
            {
                // If the vertical word is fixed, we test with that fixed letter
                if(BoardHorizontalWords[m].state == FIXED)
                {
                    if((BoardVerticalWords[n].candidates[i].candidateWord[verWordPos] == boardSquare.GetLetter()) &&
                        (BoardVerticalWords[n].candidates[i].isNominated))
                    {
                        BoardVerticalWords[n].candidates[i].isNominated = true;
                        match = true;
                        aMatchWasFound = true;

                    //    std::cout << "For HorWord " << (int)BoardVerticalWords[n].id <<  ", the word " <<  BoardVerticalWords[n].candidates[i].candidateWord << " is nominated" << std::endl;
                    }
                    else
                    {
                        BoardHorizontalWords[m].candidates[i].isNominated = false;
                    }
                } 

                // If the horizontal word has been chosen or selected, we compare with that chosen/nominated word
                else if(BoardHorizontalWords[m].state == CHOSEN || BoardHorizontalWords[m].state == SELECTED)
                {
                    if((BoardVerticalWords[n].candidates[i].candidateWord[verWordPos] == 
                        BoardHorizontalWords[m].chosenWord[horWordPos]))
                    {
                        BoardVerticalWords[n].candidates[i].isNominated = true;
                        match = true;
                        aMatchWasFound = true;
                    //    std::cout << "For VerWord " << (int)BoardVerticalWords[n].id <<  ", the word " <<  BoardVerticalWords[n].candidates[i].candidateWord << " is nominated" << std::endl;
                    }
                    else
                    {
                        BoardVerticalWords[n].candidates[i].isNominated = false;
                    }
                    
                }

                // If the horizontal word has not been chosen or selected yet, we test every horizontal candidate with the vertical candidate
                else
                {
                    for(int k = 0; k < BoardHorizontalWords[m].candidates.size(); k++)
                    {
                      //  std::cout << BoardHorizontalWords[m].candidates[k].candidateWord << " " << BoardHorizontalWords[m].candidates[k].isNominated << " vs " << 
                      //  BoardVerticalWords[n].candidates[i].candidateWord << " " << BoardVerticalWords[n].candidates[i].isNominated << std::endl;
                        //std::cout << BoardHorizontalWords[m].candidates[k].candidateWord[horWordPos] << " vs " << BoardVerticalWords[n].candidates[i].candidateWord[verWordPos] << std::endl;
                        
                        if((BoardVerticalWords[n].candidates[i].candidateWord[verWordPos] == 
                            BoardHorizontalWords[m].candidates[k].candidateWord[horWordPos]) &&
                            (BoardVerticalWords[n].candidates[i].isNominated) && BoardHorizontalWords[m].candidates[k].isNominated)
                        {
                            BoardVerticalWords[n].candidates[i].isNominated = true;
                            match = true;
                            aMatchWasFound = true;

                     //       std::cout << "For VerWord " << (int)BoardVerticalWords[n].id <<  ", the word " <<  
                     //           BoardVerticalWords[n].candidates[i].candidateWord << " is nominated because it's compatible with word " <<
                     //           BoardHorizontalWords[m].candidates[k].candidateWord << std::endl;
                        }
                    }

                    // If there was no match in the horizontal candidates for this vertical candidate, discard it from the nominations
                    if(match == false)
                    {
                        BoardVerticalWords[n].candidates[i].isNominated = false;
                    }
                }
            }
        }

        if(aMatchWasFound == false)
        {
            returnStatus = FAILURE;
        }
    }

    //printf("\n");

    return returnStatus;
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
                std::cout << BoardHorizontalWords[x].candidates[y].candidateWord << " ";
            }

            std::cout << std::endl;
        }

        for(int x = 0; x < BoardVerticalWords.size(); x++)
        {
            std::cout << "Candidates for V" << (int)BoardVerticalWords[x].id << ": ";

            for(int y = 0; y < BoardVerticalWords[x].candidates.size(); y++)
            {
                std::cout << BoardVerticalWords[x].candidates[y].candidateWord << " ";
            }

            std::cout << std::endl;
        }

    return;
}

