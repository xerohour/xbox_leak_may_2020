//#define ALT_TRAIN
/*************************************************************************

Copyright (c) 1996 -- 2000  Microsoft Corporation

Module Name:

	localHuffman.h

Abstract:

	Local Huffman code generator.

Revision History:

    Sridhar Srinivasan: 3/16/00

*************************************************************************/

#include "xplatform.h"
#include "typedef.hpp"
#include "codehead.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "strmdec_wmv.hpp"
#include "huffman_wmv.hpp"

/********************************************************************
  end of definition of CLocalHuffman
********************************************************************/

enum { LH_INTER0, LH_INTER, LH_INTRAY0, LH_INTRAY,
// IntraUV uses Inter tables
       LH_ORIENT, LH_INTRAZ, LH_INTRANZ, LH_INTRAC0, LH_INTRAC1 };
// New Intra coding

enum { QP_AT_8BIT_LEVEL = 5 };

enum { L0a=46, L0b, L0c, L0d, L0e, L0f,  L0z, L0y, L0x, L0w, L0v,  L0u, L0t,
/*59*/ L1a, L1b, L1c, L1d, L1e, L1f,  L1g, L1h, L1i,  L1z, L1y, L1x, L1w, L1v,
/*73*/ Q0, Q1, X0, X1 };

static const I32_WMV SIGNSHIFT=16; // right shift leaves only info regarding sign
// low QP encoding (static works best)

static const I32_WMV iModeArray[]={ LH_INTER0, LH_INTRAY0, LH_INTER0,
    LH_INTRAY, LH_INTER /*experimental*/ };

/**********************************************************************
 CAltTablesDecoder : definition
**********************************************************************/
struct t_LocalHuffmanDecoder;

typedef struct t_AltTablesDecoder
{
    U32_WMV    m_uiUserData;
    I32_WMV    m_iNewFrame;
    I32_WMV    m_iNewIntraFrame;
    t_LocalHuffmanDecoder   *m_paLH[LH_INTRAC1+1];
    Bool_WMV   m_bIntraFrame;
    I32_WMV    m_iNContexts;

    I32_WMV    m_iQLev;
    Bool_WMV   m_bIsFirstRun;
} t_CAltTablesDecoder;

t_AltTablesDecoder *t_AltTablesDecoderConstruct (U32_WMV uiUserData);
Void_WMV    t_AltTablesDecoderDestruct (t_AltTablesDecoder *pAltTab);
Void_WMV    t_AltTablesSetNewFrame (t_AltTablesDecoder *pAltTab,
                                    I32_WMV, I32_WMV);

/**********************************************************************
 Static variables
**********************************************************************/

