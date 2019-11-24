#ifndef SQUARE_HPP
#define SQUARE_HPP

#define FALSE   0
#define TRUE    1

#define WHITE_SQUARE 0
#define BLACK_SQUARE 1

class Square
{
    private:

        short color;            // WHITE_SQUARE or BLACK_SQUARE
        char  letter;           // Letter filling the square, if white square
        short hWordId;          // Horizontal word ID
        short hWordStart;       // TRUE = it is the first letter of an horizontal word
        short hWordFixed;       // FALSE = hor. word is not fixed, TRUE = hor. word is fixed
        short hWordSize;        // Size of the horizontal word.
        short hWordLetterPos;   // Letter position in the horizontal word.
        short vWordId;          // Vertical word ID 
        short vWordStart;       // TRUE = it is the first letter of an horizontal word
        short vWordFixed;       // FALSE = ver. word is not fixed, TRUE = ver. word is fixed
        short vWordSize;        // Size of the vertical word.
        short vWordLetterPos;   // Letter position in the vertical word.
        
    public:
        
        Square();

        short GetSquareColor();
        void  SetSquareColorBlack();

        char GetLetter();
        void SetLetter(char newLetter);

        short GetHWordId();
        void  SetHWordId(short newId);

        short IsHWordStart();
        void  SetHWordStart();

        short IsHWordFixed();
        void  SetHWordFixed();

        short GetHWordSize();
        void  SetHWordSize(short newSize);

        short GetHWordLetterPos();
        void  SetHWordLetterPos(short newLetterPos);

        short GetVWordId();
        void  SetVWordId(short newId);

        short IsVWordStart();
        void  SetVWordStart();

        short IsVWordFixed();
        void  SetVWordFixed();

        short GetVWordSize();
        void  SetVWordSize(short newSize);

        short GetVWordLetterPos();
        void  SetVWordLetterPos(short newLetterPos);

};

#endif