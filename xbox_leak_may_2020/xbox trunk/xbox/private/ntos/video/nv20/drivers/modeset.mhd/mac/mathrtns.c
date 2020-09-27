/*
    FILE:   mathrtns.c
    DATE:   12/22/99

    This file contains math routines which need to be handled in an OS
    specific manner.
*/

#include "math.h"

float calcFLOOR(float fInput);
float calcROUND(float fInput);
float calcSQRT(float fInput);

float calcFLOOR(float fInput)
{
    return (floor(fInput));
}

float calcROUND(float fInput)
{
    return (calcFLOOR(fInput+0.5f));
}

float calcSQRT(float fInput)
{
    return ((float)sqrt((double)fInput));
}

