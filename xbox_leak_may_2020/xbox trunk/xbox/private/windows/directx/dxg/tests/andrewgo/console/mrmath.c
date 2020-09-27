/****************************** Module Header ******************************\
* Module Name: mrmath.c
*
* Copyright (c) 1992, Microsoft Corporation
*
* Simple test program for stressing the FPU.
*
* History:
*  18-Jul-1991 -by- J. Andrew Goossen [andrewgo]
* Wrote it.
\***************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

int __cdecl main(int argc, char** argv)
{
    UINT i;
    UINT result;

    printf("Divide by 3 approximation:\n");

    for (i = 0; i < 65536; i ++)
    {
        result = ((i * (0x10000 / 3)) + (0x10000 / 3)) >> 16;

        if (result != (i / 3))
            printf("i: %li  result: %li\n", i, result);
    }

    return(1);
}
