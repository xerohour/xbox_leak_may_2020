/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    HuffDec.cpp

Abstract:

    Huffman decoder. Simplified from Sanjeevm's huffman.cpp

Author:

    Wei-ge Chen (wchen) 19-July-1999

Revision History:


*************************************************************************/

#include "bldsetup.h"

#include <stdio.h>
#include <stdlib.h>
#include "macros.h"
#include "huffdec.h"

#ifdef PROFILE
#include "autoprofile.h"
#endif


WMARESULT huffDecGet(const U16 *pDecodeTable, CWMAInputBitStream *bs,
                     U32* puBitCnt, U32 *puResult, U32* puSign)
{
     const int FIRST_LOAD = 10;
     const int SECOND_LOAD = 12;

     unsigned int ret_value;
     const unsigned short* node_base = pDecodeTable;

     U32 uBits;
     U32 codeword;
     int i;

     WMARESULT  wmaResult;
#ifdef PROFILE
	 //profiling a function that gets called this often has too much overhead.
     //FunctionProfile fp;
     //FunctionProfileStart(&fp,HUFF_DEC_GET_PROFILE);
#endif

     TRACEWMA_EXIT(wmaResult, (*(bs->ibstrmpfnPeekBits))(bs, FIRST_LOAD + SECOND_LOAD + 1, &uBits));
     codeword = uBits;

     // Do first five 2-bit tables
     i = 5;
     do
     {
         node_base += ((codeword & 0xC0000000) >> 30);  // Use top 2 bits as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 2;            // Advance to next 2 bits
         i -= 1;
     } while (i > 0);

     // Do remaining three 2-bit tables
     i = 3;
     do
     {
         node_base += ((codeword & 0xC0000000) >> 30);  // Use top 2 bits as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 2;            // Advance to next 2 bits
         i -= 1;
     } while (i > 0);

     // Do six 1-bit tables
     i = 6;
     do
     {
         node_base += ((codeword & 0x80000000) >> 31);  // Use top bit as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 1;            // Advance to next bit
         i -= 1;
     } while (i > 0);


decode_complete:
     assert(ret_value & 0x8000);
     *puBitCnt = ((ret_value >> 10) & (0x0000001F));
     *puResult = ret_value & 0x000003FF;
     if (*puResult >= 0x03FC)
         *puResult = *(node_base + (*puResult & 0x0003) + 1);
     
     if (puSign != NULL)
        *puSign = uBits << *puBitCnt;

exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
     return wmaResult;
}
