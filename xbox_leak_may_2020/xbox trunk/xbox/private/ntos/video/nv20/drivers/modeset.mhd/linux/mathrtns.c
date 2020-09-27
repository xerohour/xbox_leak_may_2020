/*
    FILE:   mathrtns.c
    DATE:   12/22/99

    This file contains math routines which need to be handled in an OS
    specific manner.  The X driver should not include system header files,
    so we have to include "xf86_ansic.h".
*/

#include "xf86_ansic.h"


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

