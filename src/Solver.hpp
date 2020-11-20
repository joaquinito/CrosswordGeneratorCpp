#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <iostream>
#include <vector>
#include "Crosswords.hpp"
#include "Logger.hpp"

#define ALGORITHM_FIRST_STAGE   1
#define ALGORITHM_SECOND_STAGE  2

#define SUCCESS     0
#define FAILURE     1

#define MAX_LENGTH_OF_WORD 20

enum Orientation
{
    horizontal,
    vertical,
};

enum WordState
{
    FIXED,          /* Pre-filled word coming from the cfg file */
    UNSELECTED,     /* Default state of non-fixed words at start of algorithm */
    SELECTED,       /* A candidate has been selected as part of the second stage of the algorithm */
    CHOSEN          /* A candidate word has been chosen, either because it was the only candidate or because the candidate
                        passed all the checks in the second stage of the algorithm */
};


typedef struct Candidate_t
{
    std::string candidateWord;
    bool isNominated;

} Candidate;

typedef struct BoardWordInfo_t
{
    uint8_t id;
    Orientation orientation;
    uint8_t size;
    uint8_t wordStartLin;
    uint8_t wordStartCol;
    std::vector<Candidate> candidates;
    std::string chosenWord;
    uint8_t state;

} BoardWordInfo;

typedef struct Coordinates_t
{
    uint8_t lin;
    uint8_t col;

} Coordinates;


class Solver
{
    private:

        Logger Log;

        std::vector<std::string> WordBank[MAX_LENGTH_OF_WORD+1];

        std::vector<Coordinates> SharedSquares;

        std::vector<BoardWordInfo> BoardHorizontalWords;

        std::vector<BoardWordInfo> BoardVerticalWords;

        void GetListsOfWordsAndSharedSquares(Crosswords &inputCrosswords);

        void FillCandidateListsForTheWords();

        uint8_t SharedSquaresCheck(Crosswords &inputCrosswords, uint8_t algorithmStage);

        uint8_t ValidateHorizontalCandidates(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);

        uint8_t ValidateVerticalCandidates(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);

        uint8_t ValidateHorizontalCandidates_SecondStage(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);

        uint8_t ValidateVerticalCandidates_SecondStage(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);

        void ChooseHorizontalCandidate(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);  

        void ChooseVerticalCandidate(Square &boardSquare, std::vector<BoardWordInfo> &BoardHorizontalWords, 
                                          std::vector<BoardWordInfo> &BoardVerticalWords, int &m, int &n);

    public:
        
        Solver(FILE *wordsFile, Logger inputLog);

        Solver(FILE *wordsFile, Crosswords &inputCrosswords, Logger inputLog);

        uint8_t CreateWordBank(FILE * wordsFile, int maxLength);

        uint8_t FindSolution(Crosswords &inputCrosswords);

        void PrintBoardHorizontalWords();

        void PrintBoardVerticalWords();

        void PrintBoardSharedSquares();

        void PrintCandidateLists();
};

#endif