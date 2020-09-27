/***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/***************************** Functions for MCP1***************************\
*                                                                           *
* Module: CEPDspProgram.cpp                                                 *
*	interface for accessing DSP program information                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

extern "C"
{
#include <nvarm.h>
#include <nv_uep.h>
#include <AudioHw.h>
};

#include "CDspProgram.h"
#include "CEPDspProgram.h"

// the header files are generated at 
// build time... by the build batch file
static U032 AC3SuperExec[] = 
{
#include <AC3SuperExec.h>
};

static U032 AC3Loader[] =
{
#include <AC3Loader.h>
};

static U032 AC3Dolby1[] =
{
#include <AC3Dolby1.h>
};

static U032 AC3Dolby2[] =
{
#include <AC3Dolby2.h>
};

static U032 AC3Dolby3[] =
{
#include <AC3Dolby3.h>
};

static U032 AC3Dolby4[] =
{
#include <AC3Dolby4.h>
};

typedef struct
{
    U032 table_size;
    U032 do_surround_encode;
    U032 do_game_encode;
    U032 pcm_sample_rate_code;
    U032 input_ch_config;
    U032 lfe_present;
    U032 ac3_dialnorm;
    U032 ac3_bandwidth;
    U032 channel_lpf_enabled;
    U032 lfe_lpf_enabled;
    U032 dc_hpf_enabled;
    U032 dynrng_exists;
    U032 ac3_dynrng_code;
    U032 compr_exists;
    U032 ac3_compr_code;
    U032 surround_gain_enabled;
    U032 surround_gain;
    U032 surround_mode;
} DOLBY_CONFIG_TABLE;

static DOLBY_CONFIG_TABLE AC3ConfigTable =
  {
    18,        // table size
    1,         // perform dolby surround encode flag
    1,         // perform dolby game encode flag
    0,         // ac3 pcm sampling rate code
    7,         // ac3 dolby input channel config code (acmod)
    1,         // lfe channel present flag
    16,        // ac3 dialnorm value
    9,         // ac3 channel bandwidth code
    1,         // channel lpf enabled flag
    1,         // lfe channel lpf enabled flag
    1,         // dc hpf enabled flag
    0,         // dynrng exists flag
    0,         // ac3 dynrng code
    1,         // compr exists flag
    0xEF,      // ac3 compr code
    1,         // dolby surround gain enabled flag
    0x47FACD,  // dolby surround encoder gain value
    2          // dolby surround mode. Only used for 2/0 Mode
  };

U032
CEPDspProgram::AC3GetTotalScratchSize()
{
    // in bytes
    return (AC3_DOLBYBASE * sizeof(U032) + 
            sizeof(AC3Dolby1) + 
            sizeof(AC3Dolby2) + 
            sizeof(AC3Dolby3) + 
            sizeof(AC3Dolby4) +
            sizeof(DOLBY_CONFIG_TABLE) + 
            AC3_HEAPLENGTH * sizeof(U032));
}

VOID
CEPDspProgram::AC3GetSuperExec(VOID_PTR *ppData, U032 *pLength, U032 *pBase)
{
    *ppData  = (VOID_PTR)AC3SuperExec;
    *pLength = sizeof(AC3SuperExec)/sizeof(U032);
    *pBase   = AC3_SUPREXECBASE;
}

VOID
CEPDspProgram::AC3GetLoader(VOID_PTR *ppData, U032 *pLength, U032 *pBase)
{
    *ppData  = (VOID_PTR)AC3Loader;
    *pLength = sizeof(AC3Loader)/sizeof(U032);
    *pBase   = AC3_LOADERBASE;
}

VOID_PTR 
CEPDspProgram::AC3GetConfigTable()
{
    return (VOID_PTR)&AC3ConfigTable;
}

VOID
CEPDspProgram::AC3GetProgram(U032 uIndex, VOID_PTR *ppData, U032 *pLength)
{
    switch(uIndex)
    {
    case 0:
        *ppData  = (VOID_PTR)AC3Dolby1;
        *pLength = sizeof(AC3Dolby1)/sizeof(U032);
        break;
    case 1:
        *ppData  = (VOID_PTR)AC3Dolby2;
        *pLength = sizeof(AC3Dolby2)/sizeof(U032);
        break;
    case 2:
        *ppData  = (VOID_PTR)AC3Dolby3;
        *pLength = sizeof(AC3Dolby3)/sizeof(U032);
        break;
    case 3:
        *ppData  = (VOID_PTR)AC3Dolby4;
        *pLength = sizeof(AC3Dolby4)/sizeof(U032);
        break;

    default:
        *ppData  = NULL;
        *pLength = 0;
        break;
    }
}

VOID
CEPDspProgram::Initialize()
{
}

