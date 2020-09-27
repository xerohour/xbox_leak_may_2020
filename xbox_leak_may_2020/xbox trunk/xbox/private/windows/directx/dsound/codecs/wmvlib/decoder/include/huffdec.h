/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    HuffDec.h

Abstract:

    Huffman decoder. Simplified from Sanjeevm's huffman.hpp

Author:

    Wei-ge Chen (wchen) 19-July-1999

Revision History:


*************************************************************************/

#ifndef _HUFFDEC_H
#define _HUFFDEC_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "macros.h"
#include "strmdec_wma.h"

#define BITSTREAM_READ 1

WMARESULT huffDecGet(const U16 *pDecodeTable, CWMAInputBitStream *bs,
                     U32* puBitCnt, U32 *puResult, U32* puSign);

#ifdef __cplusplus
}
#endif
#endif //_HUFFDEC_H

