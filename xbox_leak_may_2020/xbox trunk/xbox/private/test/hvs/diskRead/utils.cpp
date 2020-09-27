/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Helper functions

Notes:

*****************************************************************************/

#include "diskRead.h"

void CompareBuff(unsigned char *buff1, unsigned char *buff2, unsigned char *buff3, unsigned length)
    {
    for(unsigned i=0; i<length; i++)
        {
        if(buff1[i] != buff2[i])
            {
            LogPrint("    %08X: %02X %02X %02X\n", i, buff1[i], buff2[i], buff3[i]);
            }
        }
    }

void PrintPreRetryError(DWORD block, SCSIError &err)
    {
    LogPrint("Block %u: Internal Error ", block);
    err.Print();
    LogPrint(" retrying\n");
    }


