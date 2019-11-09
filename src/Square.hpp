
#define FALSE   0
#define TRUE    1

#define WHITE_SQUARE 0
#define BLACK_SQUARE 1

class Square
{
    private:

        short color;        // WHITE_SQUARE or BLACK_SQUARE
        char  letter;       // Letter filling the square, if white square
        short hWordId;      // Horizontal word ID
        short hWordStart;   // TRUE = it is the first letter of an horizontal word
        short hWordFixed;   // FALSE = hor. word is not fixed, TRUE = hor. word is fixed
        short vWordId;      // Vertical word ID 
        short vWordStart;   // TRUE = it is the first letter of an horizontal word
        short vWordFixed;   // FALSE = ver. word is not fixed, TRUE = ver. word is fixed
        
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

        short GetVWordId();
        void  SetVWordId(short newId);

        short IsVWordStart();
        void  SetVWordStart();

        short IsVWordFixed();
        void  SetVWordFixed();

};