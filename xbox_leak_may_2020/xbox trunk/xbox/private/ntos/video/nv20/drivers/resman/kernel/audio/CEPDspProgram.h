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
* Module: CEPDspProgram.h                                                   *
*	interface for accessing DSP program information  for GP                 *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CEPDSPPROGRAMH_
#define _CEPDSPPROGRAMH_

// reserve the last one for AC-3
#define AC3_EFFECT_INDEX        (MAX_EFFECTS - 1)

#define AC3_SUPREXECBASE        0x000
#define AC3_LOADERBASE          0x180
#define AC3_LOADERTABLEBASE     0x100 // xxxxx temp 0x2C0 until dolby modifies their code
#define AC3_DOLBYBASE           0x300

#define AC3_MAX_PROGRAMS        4

#define AC3_HEAPLENGTH          8192  // length in words

typedef struct
{
    // the entries in this table are actually all 24-bit DSP words, but when
    // we store DSP code in system memory, we extend each word to fill 32 bits,
    // so we can store them in U032S.  
    U032 tableSize;         // table size                  
    U032 maxProgs;          // number of programs

    struct
    {
        U032 ptr;
        U032 size;
    } prog[AC3_MAX_PROGRAMS];

    U032 pcm_ptr;              // pointer to input PCM buffer
    U032 pcm_size;             // size of input PCM buffer
    U032 ltrt_ptr;             // pointer to output Lt/Rt buffer  (Note: actually a FIFO index!!)
    U032 ltrt_size;            // size of output Lt/Rt buffer
    U032 ac3_ptr;              // pointer to output AC-3 buffer   (Note: actually a FIFO index!!)
    U032 ac3_size;             // size of output AC-3 buffer
    U032 config_ptr;           // pointer to config table
    U032 config_size;          // size of config table
    U032 pingpong_offset;      // current pingpong buffer offset
    U032 reserved1;
    U032 ac3_zero_fill;        // AC3 zero fill (Note: actually a FIFO index!!)
    U032 reserved2;
    U032 ac3_preamble;         // AC3 preamble (Note: actually a FIFO index!!)
    U032 reserved3;
    U032 heap_ptr;             // pointer to heap data buffer
    U032 heap_size;            // size of heap data buffer
} DOLBY_LOADER_TABLE;


#define DOLBY_LOADER_TABLE_OFFSET(x)    (U032)&(((DOLBY_LOADER_TABLE *)0)->x)

class CDspProgram;

class CEPDspProgram : public CDspProgram
{
public:
    VOID Initialize();

    // AC3 functions
    U032 AC3GetTotalScratchSize();
    VOID AC3GetSuperExec(VOID_PTR *ppData, U032 *pLength, U032 *pBase);
    VOID AC3GetLoader(VOID_PTR *ppData, U032 *pLength, U032 *pBase);
    VOID AC3GetProgram(U032 uIndex, VOID_PTR *ppData, U032 *pLength);
    
    VOID_PTR AC3GetConfigTable();
    
    U032 AC3GetLoaderTableBase() { return AC3_LOADERTABLEBASE; }
    U032 AC3GetProgramBase()     { return AC3_DOLBYBASE;       }
    U032 AC3GetMaxPrograms()     { return AC3_MAX_PROGRAMS;    }
    U032 AC3GetHeapLength()      { return AC3_HEAPLENGTH;      }
};

#endif