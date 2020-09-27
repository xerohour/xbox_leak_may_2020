 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/**************************************************************************************************************
*
*   Header: devinit.h
*
*   Description:
*       Definitions for devinit.h.
*
*   Revision History:
*       Original -- 2/99 Jeff Westerinen
*
**************************************************************************************************************/

#include <bios.h>

// init table definitions
extern U032 Nv04_bmp_GenInitTbl[];
extern U032 Nv05_bmp_GenInitTbl[];
extern U032 Nv05Pro_bmp_GenInitTbl[];
extern bmpMemInitData Nv05_bmp_MemInitTbl[];

extern U032 Nv10_SDR_Sequence_Table[];
extern U032 Nv10_DDR_Sequence_Table[];
extern MEM_INIT_TABLE_NV10 Nv10_bmp_MemInitTbl;
extern U032 Nv10_bmp_GenInitTbl[];

// video RAM type enumerations
enum 
{
    DEVINIT_NORAM, 
    DEVINIT_SDRAM, 
    DEVINIT_SGRAM
};

// internal TMDS types
enum
{
    TMDSIT_TMDS_SINGLE_A,
    TMDSIT_TMDS_SINGLE_B,
    TMDSIT_TMDS_DUAL,
    TMDSIT_LVDS_SINGLE_A,
    TMDSIT_LVDS_SINGLE_B,
    TMDSIT_LVDS_DUAL,
    TMDSIT_OFF_SINGLE_A,
    TMDSIT_OFF_SINGLE_B
};

// table to hold copy of our devinit tables
typedef struct {
    U032 memInitTable[(MEM_INIT_STRAP_MAX+1) * (MEM_INIT_VALS_MAX_BYTE_SIZE / 4)];
    U032 SDRSeqTable[MEM_INIT_SEQ_TABLE_MAX];
    U032 DDRSeqTable[MEM_INIT_SEQ_TABLE_MAX];
    U032 *genInitCode;
    U008 *genInitData;
    U032 *extraInitCode;
    U008 *extraInitData;
 } devinit_tables_t;

#define DEVINIT_STACK_MAX 20
typedef struct
{
    U032 body[DEVINIT_STACK_MAX];
    U032 sp;
} DEVINIT_STACK, *PDEVINIT_STACK;

// tables from bios we extract (and convert) up front
typedef struct {
        MACRO_INDEX_ENTRY       macroIndexTable[256];
        MACRO_ENTRY             macroTable[256];
        CONDITION_ENTRY         conditionTable[256]; 
        IO_CONDITION_ENTRY      ioConditionTable[256];
        IO_FLAG_CONDITION_ENTRY ioFlagConditionTable[256];
        FUNCTION_TABLE_ENTRY    functionTable[256];
} BIOS_BIP3_TABLES;

typedef struct
{
    BOOL                    conditionFlag;
    DEVINIT_STACK           loopStack;
    BOOL                    doJump;
    U032                    jumpIndex;

    // offsets with the bios
    U032                    scriptTableOffset;
    U032                    macroIndexTableOffset;
    U032                    macroTableOffset;
    U032                    conditionTableOffset;
    U032                    ioConditionTableOffset;
    U032                    ioFlagConditionTableOffset;
    U032                    functionTableOffset;
    U032                    TMDSSingleAOffset;
    U032                    TMDSSingleBOffset;
    U032                    TMDSDualOffset;
    U032                    LVDSSingleAOffset;
    U032                    LVDSSingleBOffset;
    U032                    LVDSDualOffset;
    U032                    OffSingleAOffset;
    U032                    OffSingleBOffset;

    BIOS_BIP3_TABLES       *tables;

} EXECUTION_CONTEXT, *PEXECUTION_CONTEXT;

// MScript defines for the function DevinitRunManufacturerScript
#define MSCRIPT_LVDS_INIT_SCRIPT        0x0
#define MSCRIPT_LVDS_RESET_SCRIPT       0x1
#define MSCRIPT_BACKLIGHT_ON            0x2
#define MSCRIPT_BACKLIGHT_OFF           0x3
#define MSCRIPT_PANEL_ON                0x4
#define MSCRIPT_PANEL_OFF               0x5


// prototypes
RM_STATUS   DevinitInitializeDevice     (PHWINFO, PHWREG, BOOL*);
RM_STATUS   DevinitProcessStaticTables  (PHWINFO);
RM_STATUS   DevinitProcessBip2          (PHWINFO, U032, BMP_Control_Block, BOOL*);
RM_STATUS   DevinitGetBMPControlBlock   (PHWINFO, BMP_Control_Block *, U032 *);
RM_STATUS   DevinitGetInitTableInfo     (PHWINFO, U032, PBMP_Control_Block, U032*, U032*, U032*, U032*, U032*, U032*);
RM_STATUS   DevinitInitDataToCode       (U032[], U008[]);
RM_STATUS   InitNV                      (PHWINFO, U032[], VOID*, U032[], U032[]);
U008        DevinitFetchByte            (U032[], U032*);
U016        DevinitFetchWord            (U032[], U032*);
U032        DevinitFetchDword           (U032[], U032*);
RM_STATUS   DevinitProcessBip3          (PHWINFO);
RM_STATUS   DevinitProcessBip3InternalTMDS (PHWINFO,U032,U032);
RM_STATUS   DevinitProcessBip3InternalTMDSTable (PHWINFO, PEXECUTION_CONTEXT, U032, U032);
RM_STATUS   DevinitGetBIP3TableOffsets  (PHWINFO, PEXECUTION_CONTEXT);
RM_STATUS   DevinitReadBIP3Tables       (PHWINFO, PEXECUTION_CONTEXT);
VOID        DevinitCleanupBIP3Tables    (PHWINFO, PEXECUTION_CONTEXT);
RM_STATUS   DevinitInterpretBIP3Script  (PHWINFO, PEXECUTION_CONTEXT, U032);
U008        DevinitFetchInstruction     (PHWINFO, U032, VOID*, U032*, U032*);
RM_STATUS   DevinitGetMinMaxVCOValues   (PHWINFO, U032*, U032*);
RM_STATUS   init_NV                     (PHWINFO, U032, U032, U032, BOOL);
RM_STATUS   init_PLL_Values             (PHWINFO, U032, U032, BOOL);
RM_STATUS   init_TIME_Delay             (PHWINFO, U032);
RM_STATUS   init_IO_Write               (PHWINFO, U016, U008, U008, BOOL);
RM_STATUS   init_INDEX_IO_Write         (PHWINFO, U032, U032, U032, U032, BOOL);
RM_STATUS   init_MEM_RESTRICT_SCRIPT    (PHWINFO, U032, U032, BOOL*);
RM_STATUS   init_STRAP_RESTRICT_SCRIPT  (PHWINFO, U032, U032, BOOL*);
RM_STATUS   init_CONFIGURE_Clocks       (PHWINFO, VOID*);
RM_STATUS   init_CONFIGURE_Memory       (PHWINFO, VOID*, U032[], U032[]);
RM_STATUS   init_CONFIGURE_PreInit      (PHWINFO);
RM_STATUS   init_COMPUTE_Memory         (PHWINFO);
RM_STATUS   init_Reset_Chip             (PHWINFO, U032, U032, U032);
RM_STATUS   init_IO                     (PHWINFO, U032, U032, U032, BOOL);
RM_STATUS   init_Condition              (PHWINFO, PEXECUTION_CONTEXT, U032, BOOL*);
RM_STATUS   init_IoCondition            (PHWINFO, PEXECUTION_CONTEXT, U032, BOOL*);
RM_STATUS   init_IoFlagCondition        (PHWINFO, PEXECUTION_CONTEXT, U032, BOOL*);
RM_STATUS   init_RestrictProg           (PHWINFO, PEXECUTION_CONTEXT, U032, U032, U032, U032, U032, U032, BOOL);
RM_STATUS   init_IoRestrictProg         (PHWINFO, PEXECUTION_CONTEXT, U032, U032, U032, U032, U032, U032, U032, BOOL);
RM_STATUS   init_IoRestrictPll          (PHWINFO, PEXECUTION_CONTEXT, U032, U032, U032, U032, U032, U032, U032, U032, BOOL);
RM_STATUS   init_IndexAddressLatched    (PHWINFO, U032, U032 ,U032, U032, U032, U032, BOOL);
RM_STATUS   init_Sub                    (PHWINFO, PEXECUTION_CONTEXT, U032, BOOL);
RM_STATUS   init_Macro                  (PHWINFO, PEXECUTION_CONTEXT, U032, BOOL);
RM_STATUS   init_ZM_IO                  (PHWINFO, U032, U032, BOOL);
RM_STATUS   init_Copy                   (PHWINFO, PEXECUTION_CONTEXT, U032, S008, U032, U032, U032, U032, BOOL);
RM_STATUS   init_Repeat                 (PHWINFO, PEXECUTION_CONTEXT, U032, U032);
RM_STATUS   init_EndRepeat              (PHWINFO, PEXECUTION_CONTEXT, U032*);
RM_STATUS   init_Function               (PHWINFO, PEXECUTION_CONTEXT, U032);
VOID        DevinitMeminitFormat        (PHWINFO, char **, int *);
RM_STATUS   DevinitReadPort             (PHWINFO, U032, U032, U008*);
RM_STATUS   DevinitWritePort            (PHWINFO, U032, U032, U032);
RM_STATUS   DevinitPortToPrivReg        (U032, U032*);
VOID        DevinitStackPush            (PDEVINIT_STACK,  U032);
U032        DevinitStackPop             (PDEVINIT_STACK);
U032        DevinitStackTop             (PDEVINIT_STACK);
RM_STATUS   DevinitShadowBios           (PHWINFO, PHWREG);
RM_STATUS   Nv04MemResizeMemory         (PHWINFO);
RM_STATUS   Nv04MemResizeSdram          (PHWINFO);
RM_STATUS   Nv04MemResizeSgram          (PHWINFO);
VOID        Nv04MemLatchMemConfig       (PHWINFO);
RM_STATUS   Nv05MemResizeMemory         (PHWINFO);
VOID        Nv05MemLatchMemConfig       (PHWINFO);
RM_STATUS   Nv10MemConfigureClocks      (PHWINFO, MEM_INIT_TABLE_NV10);
RM_STATUS   Nv10MemConfigureMemory      (PHWINFO, MEM_INIT_TABLE_NV10, U032[], U032[]);
RM_STATUS   Nv10MemChipPreInit          (PHWINFO);
RM_STATUS   Nv10MemComputeMemory        (PHWINFO);
RM_STATUS   Nv15MemConfigureClocks      (PHWINFO, MEM_INIT_TABLE_NV15);
RM_STATUS   Nv15MemConfigureMemory      (PHWINFO, MEM_INIT_TABLE_NV15, U032[], U032[]);

RM_STATUS DevinitGetFlatPanelFlags
(
    PHWINFO,
    U032,
    U008*
);
RM_STATUS   DevinitGetManufacturerInfo  (PHWINFO);
RM_STATUS   DevinitRunManufacturerScript(PHWINFO, U032);

