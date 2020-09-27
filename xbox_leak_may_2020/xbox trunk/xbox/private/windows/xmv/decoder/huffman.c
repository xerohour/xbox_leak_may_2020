/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       huffman.c
 *  Content:    Holds all of our huffman tables.
 *
 ****************************************************************************/

#include <xtl.h>
#include <xdbg.h>
#include <xmv.h>

#include "decoder.h"

/* 
 * Entry for the huffman decoder tables.
 */

typedef struct _HUFFMAN_ENTRY
{
    WORD Symbol:12;
    WORD Length:4;
}
HUFFMAN_ENTRY;

/*
 * Huffman decoder routine
 *
 * andrewso: I am not at all convinced that the complexity of the huffman
 *   tables is truely necessary.  This might be an area where we can save a 
 *   ton of memory (which is a lot as those chips are light).  This needs
 *   more investigation.
 */

DWORD HuffmanDecode
(
    XMVDecoder *pDecoder, 
    WORD *pHuffmanTable
)
{
    DWORD Index, Table;
    HUFFMAN_ENTRY *pTables;
    HUFFMAN_ENTRY *pEntryTable;

    pTables = (HUFFMAN_ENTRY *)pHuffmanTable;

    Table = 0;

    for (;;)
    {
        pEntryTable = pTables + pTables[Table].Symbol;

        Index = PeekBits(pDecoder, pTables[Table].Length);

        if (pEntryTable[Index].Length > 0)
        {
            SkipBits(pDecoder, pEntryTable[Index].Length);

            return pEntryTable[Index].Symbol;
        }
        else
        {
            SkipBits(pDecoder, pTables[Table].Length);

            Table = pEntryTable[Index].Symbol;
        }
    }
}
