/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Helper functions

Notes:

*****************************************************************************/

#include "memCheck.h"

BOOL CompareBuff( unsigned char *buff1, unsigned char *buff2, unsigned length )
{
    BOOL bReturn = TRUE;

    for(unsigned i=0; i<length; i++)
    {
        if(buff1[i] != buff2[i])
        {
            // LogPrint("    %u: %02X %02X\n", i, buff1[i], buff2[i] );
            bReturn = FALSE;
            break;
        }
    }

    return bReturn;
}
