
#include "Square.hpp"

/*  
*   Class Square default constructor
*/
Square::Square()
{
    color = WHITE_SQUARE;
    letter = '?';
    hWordId = 0;
    hWordStart = FALSE; 
    hWordFixed = FALSE;
    hWordSize = 0;        
    hWordLetterPos = 0;
    vWordId = 0;
    vWordStart = FALSE; 
    vWordFixed = FALSE;
    vWordSize = 0;       
    vWordLetterPos = 0;
}

/*
* Get the color of the square.
* Possible values: WHITE_SQUARE, BLACK SQUARE
* @return: 
*/
short Square::GetSquareColor()
{
    return color;
}

void Square::SetSquareColorBlack()
{
    color = BLACK_SQUARE;
    return;
}

char Square::GetLetter()
{
    return letter;
}

void Square::SetLetter(char newLetter)
{
    letter = newLetter;
    return;
}

short Square::GetHWordId()
{
    return hWordId;
}

void Square::SetHWordId(short newId)
{
    hWordId = newId;
    return;
}

short Square::IsHWordStart()
{
    return hWordStart;
}

void Square::SetHWordStart()
{
    hWordStart = TRUE;
    return;
}

short Square::IsHWordFixed()
{
    return hWordFixed;
}

void Square::SetHWordFixed()
{
    hWordFixed = TRUE;
    return;
}

short Square::GetHWordSize()
{
    return hWordSize;
}

void Square::SetHWordSize(short newSize)
{
    hWordSize = newSize;
    return;
}

short Square::GetHWordLetterPos()
{
    return hWordLetterPos;
}

void Square::SetHWordLetterPos(short newLetterPos)
{
    hWordLetterPos = newLetterPos;
    return;
}

short Square::GetVWordId()
{
    return vWordId;
}

void Square::SetVWordId(short newId)
{
    vWordId = newId;
    return;
}

short Square::IsVWordStart()
{
    return vWordStart;
}

void Square::SetVWordStart()
{
    vWordStart = TRUE;
    return;
}

short Square::IsVWordFixed()
{
    return vWordFixed;
}

void Square::SetVWordFixed()
{
    vWordFixed = TRUE;
    return;
}

short Square::GetVWordSize()
{
    return vWordSize;
}

void Square::SetVWordSize(short newSize)
{
    vWordSize = newSize;
    return;
}

short Square::GetVWordLetterPos()
{
    return vWordLetterPos;
}

void Square::SetVWordLetterPos(short newLetterPos)
{
    vWordLetterPos = newLetterPos;
    return;
}