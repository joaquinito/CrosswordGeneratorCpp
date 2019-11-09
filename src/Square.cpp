
#include "Square.hpp"

/*  
    Square default constructor
*/
Square::Square()
{
    color = WHITE_SQUARE;
    letter = '?';
    hWordId = 0;
    hWordStart = FALSE; 
    hWordFixed = FALSE; 
    vWordId = 0;
    vWordStart = FALSE; 
    vWordFixed = FALSE;
}

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