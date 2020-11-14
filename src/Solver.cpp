
#include "Solver.hpp"
#include <algorithm>
#include <cctype>
#include <iterator>

/*
*   Default constuctor.
*/
Solver::Solver(FILE * wordsFile, Logger inputLog)
{
    Log = inputLog;
    CreateWordBank(wordsFile, MAX_LENGTH_OF_WORD);
}

/*
*   Constructor with wordsFile and Crosswords as inputs.
*   Will create a word bank with words smaller or equal to the max
*   dimension of the crosswords board. 
*/
Solver::Solver(FILE * wordsFile, Crosswords &inputCrosswords, Logger inputLog)
{
    Log = inputLog;
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
    
    Log.WriteToLog("> Solver algorithm started.\n");
    Log.WriteToLog("> Creating list of horizontal words, list of vertical words, and list of squares that are shared by an horizontal and a vertical word.\n");

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
    
    PrintBoardHorizontalWords();
    PrintBoardVerticalWords();  
    PrintBoardSharedSquares();

    Log.WriteToLog("> Filling candidate list for each horizontal word with the words from the WordBank with the corresponding length\n");
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

    Log.WriteToLog("> Filling candidate list for each vertical word with the words from the WordBank with the corresponding length\n");
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

    Log.WriteToLog("\nINITIAL CANDIDATE LIST:\n");
    PrintCandidateLists();

    Log.WriteToLog("\n> Algorithm Part 1: Checking the matching horizontal and vertical candidates in the shared squares\n");
    for(int c = 0; c < 2; c++) // Iterate two times
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

    Log.WriteToLog("\nCANDIDATE LIST AFTER FIRST PART OF ALGORITHM:\n");
    PrintCandidateLists();

    if(noSolution == false)
    {
        Log.WriteToLog("> Algorithm Part 2\n");

        // For each horizontal word to be filled in the board
        for(int i = 0; i < BoardHorizontalWords.size(); i++)
        {
            int aux = 0; 
            int escape = 0;

            Log.WriteToLog("-----------------------------------");
            Log.WriteToLog("\n> Time to choose the word for H" + std::to_string(BoardHorizontalWords[i].id) + " \n");

            // If the word is not pre-filled and has more than one candidate
            if((BoardHorizontalWords[i].state != FIXED) && 
               (BoardHorizontalWords[i].state != CHOSEN))
            {
                while(escape != 1)
                {
                    while(BoardHorizontalWords[i].candidates[aux].isNominated != true)
                    {
                        aux++; 
                    }
                    
                    if(aux > BoardHorizontalWords[i].candidates.size())
                    {
                        printf("Over!");
                        return 1;
                    }
                   
                    BoardHorizontalWords[i].state = SELECTED;
                    BoardHorizontalWords[i].chosenWord = BoardHorizontalWords[i].candidates[aux].candidateWord; 
                   
                    Log.WriteToLog("Candidate selected for H" + std::to_string(BoardHorizontalWords[i].id) + 
                                    ": " + BoardHorizontalWords[i].chosenWord);
      
                    // If checking of shared squares in unsucessful, reset the nominations and try another word
                    if(SharedSquaresCheck(inputCrosswords, ALGORITHM_SECOND_STAGE) == FAILURE)
                    {
                        Log.WriteToLog("No good candidate for this square. Another candidate must be selected for H" +
                                        std::to_string(BoardHorizontalWords[i].id) + "\n");
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

            Log.WriteToLog("-----------------------------------");
            Log.WriteToLog("\n> Time to choose the word for V" + std::to_string(BoardVerticalWords[i].id) + " \n");

            // If the word is not pre-filled and has more than one candidate
            if((BoardVerticalWords[i].state != FIXED) && (BoardVerticalWords[i].state != CHOSEN))
            {
                while(escape != 1)
                {
                    while(BoardVerticalWords[i].candidates[aux].isNominated != true)
                    {
                        aux++;
                    }

                    BoardVerticalWords[i].state = SELECTED;
                    BoardVerticalWords[i].chosenWord = BoardVerticalWords[i].candidates[aux].candidateWord;

                    Log.WriteToLog("Candidate selected for V" + std::to_string(BoardVerticalWords[i].id) + 
                                    ": " + BoardVerticalWords[i].chosenWord);

                    // If checking of shared squares in unsucessful, reset the nominations and try another word
                    if(SharedSquaresCheck(inputCrosswords, ALGORITHM_SECOND_STAGE) == FAILURE)
                    {
                        Log.WriteToLog("No good candidate for this square. Another candidate must be selected for V" +
                                        std::to_string(BoardVerticalWords[i].id) + "\n");
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

        Log.WriteToLog("\n>> Analysing square (" + std::to_string(sharedSquareLin) + 
                        "." + std::to_string(sharedSquareCol) + ")\n");

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

    Log.WriteToLog("   ----- HORIZONTAL CANDIDATE CHECK ----- \n");

    itr = BoardHorizontalWords[m].candidates.end();

    for(int j = BoardHorizontalWords[m].candidates.size()-1; j >= 0; j--)
    {
        match = false;

        if(boardSquare.IsVWordFixed())
        {
            if(BoardHorizontalWords[m].candidates[j].candidateWord[horWordPos] == boardSquare.GetLetter())
            {
                Log.WriteToLog("  " + BoardHorizontalWords[m].candidates[j].candidateWord + " matched with the fixed vertical word (letter " +
                               BoardHorizontalWords[m].candidates[j].candidateWord[horWordPos] + ")\n");
                match = true;
            }
        }
        else
        {
            for(int k = 0; k < BoardVerticalWords[n].candidates.size(); k++)
            {
                if(BoardHorizontalWords[m].candidates[j].candidateWord[horWordPos] == 
                    BoardVerticalWords[n].candidates[k].candidateWord[verWordPos])
                {
                    Log.WriteToLog("   " + BoardHorizontalWords[m].candidates[j].candidateWord + " matched with the vertical candidate " +
                                    BoardVerticalWords[n].candidates[k].candidateWord + " (letter " +
                                    BoardHorizontalWords[m].candidates[j].candidateWord[horWordPos] + ")\n");
                    match = true;
                }
            }
        }
            
        itr--;
        if(match != true)
        {
            Log.WriteToLog("   No match found for " + BoardHorizontalWords[m].candidates[j].candidateWord + ". Candidate discard.\n");
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

    Log.WriteToLog("   ----- VERTICAL CANDIDATE CHECK ----- \n");

    itr = BoardVerticalWords[n].candidates.end();

    for(int j = BoardVerticalWords[n].candidates.size()-1; j >= 0; j--)
    {
        match = false;

        if(boardSquare.IsHWordFixed())
        {
            if(BoardVerticalWords[n].candidates[j].candidateWord[verWordPos] == boardSquare.GetLetter())
            {
                Log.WriteToLog("  " + BoardVerticalWords[n].candidates[j].candidateWord + " matched with the fixed horizontal word (letter " +
                               BoardVerticalWords[n].candidates[j].candidateWord[verWordPos] + ")\n");
                match = true;
            }
        }
        else
        {
            for(int k = 0; k < BoardHorizontalWords[m].candidates.size(); k++)
            {
                if(BoardVerticalWords[n].candidates[j].candidateWord[verWordPos] == 
                    BoardHorizontalWords[m].candidates[k].candidateWord[horWordPos])
                {
                    Log.WriteToLog("   " + BoardVerticalWords[n].candidates[j].candidateWord + " matched with the horizontal candidate " +
                                    BoardHorizontalWords[m].candidates[k].candidateWord + " (letter " +
                                    BoardVerticalWords[n].candidates[j].candidateWord[verWordPos] + ")\n");
                    match = true;
                }
            }
        }
        
        itr--;

        if(match != true)
        {
            Log.WriteToLog("   No match found for " + BoardVerticalWords[n].candidates[j].candidateWord + ". Candidate discard.\n");
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

    Log.WriteToLog("State of H" + std::to_string(BoardHorizontalWords[m].id) +
                    ": " + std::to_string(BoardHorizontalWords[m].state) + "\n");

    // If the horizontal word has not been chosen or selected
    if(BoardHorizontalWords[m].state != FIXED && 
       BoardHorizontalWords[m].state != CHOSEN && 
       BoardHorizontalWords[m].state != SELECTED)
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

                        Log.WriteToLog("    For H" + std::to_string(BoardHorizontalWords[m].id) +
                                       " the word " + BoardHorizontalWords[m].candidates[i].candidateWord + 
                                       "is nominated.\n");
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

                        Log.WriteToLog("    For H" + std::to_string(BoardHorizontalWords[m].id) +
                                       " the word " + BoardHorizontalWords[m].candidates[i].candidateWord + 
                                       "is nominated.\n");
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

                            Log.WriteToLog("    For H" + std::to_string(BoardHorizontalWords[m].id) +
                                       " the word " + BoardHorizontalWords[m].candidates[i].candidateWord + 
                                       " is nominated because it's compatible with " + 
                                       BoardVerticalWords[n].candidates[k].candidateWord + "\n");
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
            Log.WriteToLog("   Failure to find a match!\n");
            returnStatus = FAILURE;
        }
    }

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

    Log.WriteToLog("State of V" + std::to_string(BoardVerticalWords[n].id) +
                    ": " + std::to_string(BoardVerticalWords[n].state) + "\n");

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

                        Log.WriteToLog("    For V" + std::to_string(BoardVerticalWords[n].id) +
                                       " the word " + BoardVerticalWords[n].candidates[i].candidateWord + 
                                       " is nominated.\n");
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

                        Log.WriteToLog("    For V" + std::to_string(BoardVerticalWords[n].id) +
                                       " the word " + BoardVerticalWords[n].candidates[i].candidateWord + 
                                       " is nominated.\n");
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
  
                        if((BoardVerticalWords[n].candidates[i].candidateWord[verWordPos] == 
                            BoardHorizontalWords[m].candidates[k].candidateWord[horWordPos]) &&
                            (BoardVerticalWords[n].candidates[i].isNominated) && BoardHorizontalWords[m].candidates[k].isNominated)
                        {
                            BoardVerticalWords[n].candidates[i].isNominated = true;
                            match = true;
                            aMatchWasFound = true;
                            
                            Log.WriteToLog("    For V" + std::to_string(BoardVerticalWords[n].id) +
                                       " the word " + BoardVerticalWords[n].candidates[i].candidateWord + 
                                       " is nominated because it's compatible with " + 
                                       BoardHorizontalWords[m].candidates[k].candidateWord + "\n");
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
            Log.WriteToLog("   Failure to find a match!\n");
            returnStatus = FAILURE;
        }
    }

    return returnStatus;
}


void Solver::PrintBoardHorizontalWords()
{
    Log.WriteToLog("HORIZONTAL BOARD WORDS:\n");
    for(int i = 0; i < BoardHorizontalWords.size(); i++)
    {
        Log.WriteToLog("H" + std::to_string(BoardHorizontalWords[i].id) + "(" +
                       std::to_string(BoardHorizontalWords[i].wordStartLin) + "." +
                       std::to_string(BoardHorizontalWords[i].wordStartCol) + ") Size: " +
                       std::to_string(BoardHorizontalWords[i].size) + "\n");
    }
    Log.WriteToLog("\n");
    return;
}

void Solver::PrintBoardVerticalWords()
{
    Log.WriteToLog("VERTICAL BOARD WORDS:\n");
    for(int i = 0; i < BoardVerticalWords.size(); i++)
    {
        Log.WriteToLog("H" + std::to_string(BoardVerticalWords[i].id) + "(" +
                       std::to_string(BoardVerticalWords[i].wordStartLin) + "." +
                       std::to_string(BoardVerticalWords[i].wordStartCol) + ") Size: " +
                       std::to_string(BoardVerticalWords[i].size) + "\n");
    }
    Log.WriteToLog("\n");
    return;
}

void Solver::PrintBoardSharedSquares()
{
    Log.WriteToLog("SHARED SQUARES:\n");
    for(int i = 0; i < SharedSquares.size(); i++)
    {
        Log.WriteToLog("(" + std::to_string(SharedSquares[i].lin) + "." + std::to_string(SharedSquares[i].col) + ")\n");
    }
    Log.WriteToLog("\n");

    return;
}

void Solver::PrintCandidateLists()
{
    for(int x = 0; x < BoardHorizontalWords.size(); x++)
        {
            Log.WriteToLog("Candidates for H" + std::to_string(BoardHorizontalWords[x].id) + ": ");

            for(int y = 0; y < BoardHorizontalWords[x].candidates.size(); y++)
            {
                Log.WriteToLog(BoardHorizontalWords[x].candidates[y].candidateWord + " ");
            }
            Log.WriteToLog("\n");
        }

        for(int x = 0; x < BoardVerticalWords.size(); x++)
        {
            Log.WriteToLog("Candidates for V" + std::to_string(BoardVerticalWords[x].id) + ": ");

            //std::cout << "Candidates for V" << (int)BoardVerticalWords[x].id << ": ";

            for(int y = 0; y < BoardVerticalWords[x].candidates.size(); y++)
            {
                Log.WriteToLog(BoardVerticalWords[x].candidates[y].candidateWord + " ");
            }
            Log.WriteToLog("\n");
        }

    return;
}

