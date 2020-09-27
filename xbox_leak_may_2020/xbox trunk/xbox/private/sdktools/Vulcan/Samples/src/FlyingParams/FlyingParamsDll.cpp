/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: FlyingParamsDll.cpp
*
* File Comments:
*
*
***********************************************************************/

#include "flyingparams.h"

extern "C" _declspec (dllexport) VALUE_DATA *rgValueData = 0;

void _stdcall IncForValue( int value, int* pDeref )
{
    __int64 val64 = value;
    
    if (rgValueData)
    {
        try
        {
            while (*pDeref >= 0)
            {
               if (*pDeref)
               {
                   val64 += *pDeref;
               }
               else
               {
                   val64 = *(__int64 *)(val64 & 0xFFFFFFFF);
               }
               pDeref++;
            }
        }
        catch(...)
        {
            return; //ignore value!
        }

        switch (*pDeref)
        {
        case -1:
            val64 &= 0xFF;
            break;

        case -2:
            val64 &= 0xFFFF;
            break;

        case -4:
            val64 &= 0xFFFFFFFF;
            break;

        case -8:
            break;

        default:
            // Is this a bit field?
            if ((-*pDeref) & 0x1000)
            {
               // Offset and size are packed into 6 bits each
               size_t bitOffset = ((-*pDeref)>>6) & 0x3f;
               size_t bitSize   = ((-*pDeref))    & 0x3f;

               val64 >>= bitOffset;
               val64 &=  ((1<<bitSize)-1);
            }
            break;
        }
    
        for (VALUE_DATA *pData = rgValueData; pData->count && pData < rgValueData + cValueMax; pData++)
        {
            if (pData->value == val64)
            {
                //UNDONE: consider slight re-order of array for perf
            
                pData->count++;
                return;
            }
        }

        if (pData < rgValueData + cValueMax)
        {
            // Make a new slot
            pData->value = val64;
            pData->count = 1;
        }
    }
}

extern "C" _declspec(naked) _declspec(dllexport) void _stdcall ValueHandler( void * )
{
    _asm push eax
    _asm push ecx
    _asm push edx

    _asm push [esp + 0x14]
    _asm push [esp + 0x14]
    _asm call IncForValue

    _asm pop edx
    _asm pop ecx
    _asm pop eax
    _asm ret 8
}