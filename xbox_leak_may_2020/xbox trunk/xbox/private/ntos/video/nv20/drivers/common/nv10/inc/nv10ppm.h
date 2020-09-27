#ifndef _NV10PPM_H_
#define _NV10PPM_H_

 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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

//
// This is a portion of the chip REF file used for direct access to the 
// device performance monitor engine.
//

#define NV_PPM                                0x0000AFFF:0x0000A000 /* RW--D */
#define NV_PPM_NV_TRIG0_SEL                              0x0000A400 /* RW-4R */
#define NV_PPM_NV_TRIG0_SEL_SEL0                                7:0 /* RWXUF */
#define NV_PPM_NV_TRIG0_SEL_SEL1                               15:8 /* RWXUF */
#define NV_PPM_NV_TRIG0_SEL_SEL2                              23:16 /* RWXUF */
#define NV_PPM_NV_TRIG0_SEL_SEL3                              31:24 /* RWXUF */
#define NV_PPM_NV_TRIG0_OP                               0x0000A404 /* RW-4R */
#define NV_PPM_NV_TRIG0_OP_FUNC                                15:0 /* RWXUF */
#define NV_PPM_NV_TRIG0_OP_DSEL0                              16:16 /* RWXUF */
#define NV_PPM_NV_TRIG0_OP_DSEL1                              17:17 /* RWXUF */
#define NV_PPM_NV_TRIG1_SEL                              0x0000A408 /* RW-4R */
#define NV_PPM_NV_TRIG1_SEL_SEL0                                7:0 /* RWXUF */
#define NV_PPM_NV_TRIG1_SEL_SEL1                               15:8 /* RWXUF */
#define NV_PPM_NV_TRIG1_SEL_SEL2                              23:16 /* RWXUF */
#define NV_PPM_NV_TRIG1_SEL_SEL3                              31:24 /* RWXUF */
#define NV_PPM_NV_TRIG1_OP                               0x0000A40C /* RW-4R */
#define NV_PPM_NV_TRIG1_OP_FUNC                                15:0 /* RWXUF */
#define NV_PPM_NV_TRIG1_OP_DSEL0                              16:16 /* RWXUF */
#define NV_PPM_NV_TRIG1_OP_DSEL1                              17:17 /* RWXUF */
#define NV_PPM_NV_EVENT_SEL                              0x0000A410 /* RW-4R */
#define NV_PPM_NV_EVENT_SEL_SEL0                                7:0 /* RWXUF */
#define NV_PPM_NV_EVENT_SEL_SEL1                               15:8 /* RWXUF */
#define NV_PPM_NV_EVENT_SEL_SEL2                              23:16 /* RWXUF */
#define NV_PPM_NV_EVENT_SEL_SEL3                              31:24 /* RWXUF */
#define NV_PPM_NV_EVENT_OP                               0x0000A414 /* RW-4R */
#define NV_PPM_NV_EVENT_OP_FUNC                                15:0 /* RWXUF */
#define NV_PPM_NV_EVENT_OP_DSEL0                              16:16 /* RWXUF */
#define NV_PPM_NV_EVENT_OP_DSEL1                              17:17 /* RWXUF */
#define NV_PPM_NV_SAMPLE_SEL                             0x0000A418 /* RW-4R */
#define NV_PPM_NV_SAMPLE_SEL_SEL0                               7:0 /* RWXUF */
#define NV_PPM_NV_SAMPLE_SEL_SEL1                              15:8 /* RWXUF */
#define NV_PPM_NV_SAMPLE_SEL_SEL2                             23:16 /* RWXUF */
#define NV_PPM_NV_SAMPLE_SEL_SEL3                             31:24 /* RWXUF */
#define NV_PPM_NV_SAMPLE_OP                              0x0000A41C /* RW-4R */
#define NV_PPM_NV_SAMPLE_OP_FUNC                               15:0 /* RWXUF */
#define NV_PPM_NV_SAMPLE_OP_DSEL0                             16:16 /* RWXUF */
#define NV_PPM_NV_SAMPLE_OP_DSEL1                             17:17 /* RWXUF */
#define NV_PPM_NV_SETFLAG_SEL                            0x0000A420 /* RW-4R */
#define NV_PPM_NV_SETFLAG_SEL_SEL0                              7:0 /* RWXUF */
#define NV_PPM_NV_SETFLAG_SEL_SEL1                             15:8 /* RWXUF */
#define NV_PPM_NV_SETFLAG_SEL_SEL2                            23:16 /* RWXUF */
#define NV_PPM_NV_SETFLAG_SEL_SEL3                            31:24 /* RWXUF */
#define NV_PPM_NV_SETFLAG_OP                             0x0000A424 /* RW-4R */
#define NV_PPM_NV_SETFLAG_OP_FUNC                              15:0 /* RWXUF */
#define NV_PPM_NV_SETFLAG_OP_DSEL0                            16:16 /* RWXUF */
#define NV_PPM_NV_SETFLAG_OP_DSEL1                            17:17 /* RWXUF */
#define NV_PPM_NV_CLRFLAG_SEL                            0x0000A428 /* RW-4R */
#define NV_PPM_NV_CLRFLAG_SEL_SEL0                              7:0 /* RWXUF */
#define NV_PPM_NV_CLRFLAG_SEL_SEL1                             15:8 /* RWXUF */
#define NV_PPM_NV_CLRFLAG_SEL_SEL2                            23:16 /* RWXUF */
#define NV_PPM_NV_CLRFLAG_SEL_SEL3                            31:24 /* RWXUF */
#define NV_PPM_NV_CLRFLAG_OP                             0x0000A42C /* RW-4R */
#define NV_PPM_NV_CLRFLAG_OP_FUNC                              15:0 /* RWXUF */
#define NV_PPM_NV_CLRFLAG_OP_DSEL0                            16:16 /* RWXUF */
#define NV_PPM_NV_CLRFLAG_OP_DSEL1                            17:17 /* RWXUF */
#define NV_PPM_NV_ELAPSED_0                              0x0000A600 /* RR-4R */
#define NV_PPM_NV_ELAPSED_0_VAL                                31:0 /* RR-UF */
#define NV_PPM_NV_ELAPSED_1                              0x0000A604 /* RR-4R */
#define NV_PPM_NV_ELAPSED_1_VAL                                 7:0 /* RR-UF */
#define NV_PPM_NV_CYCLECNT_0                             0x0000A608 /* RR-4R */
#define NV_PPM_NV_CYCLECNT_0_VAL                               31:0 /* RR-UF */
#define NV_PPM_NV_CYCLECNT_1                             0x0000A60C /* RR-4R */
#define NV_PPM_NV_CYCLECNT_1_VAL                                7:0 /* RR-UF */
#define NV_PPM_NV_EVENTCNT_0                             0x0000A610 /* RR-4R */
#define NV_PPM_NV_EVENTCNT_0_VAL                               31:0 /* RR-UF */
#define NV_PPM_NV_EVENTCNT_1                             0x0000A614 /* RR-4R */
#define NV_PPM_NV_EVENTCNT_1_VAL                                7:0 /* RR-UF */
#define NV_PPM_NV_THRESHCNT_0                            0x0000A618 /* RR-4R */
#define NV_PPM_NV_THRESHCNT_0_VAL                              31:0 /* RR-UF */
#define NV_PPM_NV_THRESHCNT_1                            0x0000A61C /* RR-4R */
#define NV_PPM_NV_THRESHCNT_1_VAL                               7:0 /* RR-UF */
#define NV_PPM_NV_TRIGGERCNT                             0x0000A620 /* RW-4R */
#define NV_PPM_NV_TRIGGERCNT_VAL                               31:0 /* RWXUF */
#define NV_PPM_NV_SAMPLECNT                              0x0000A624 /* RW-4R */
#define NV_PPM_NV_SAMPLECNT_VAL                                31:0 /* RWXUF */
#define NV_PPM_NV_THRESHOLD_0                            0x0000A628 /* RW-4R */
#define NV_PPM_NV_THRESHOLD_0_VAL                              31:0 /* RWXUF */
#define NV_PPM_NV_THRESHOLD_1                            0x0000A62C /* RW-4R */
#define NV_PPM_NV_THRESHOLD_1_VAL                               7:0 /* RWXUF */
#define NV_PPM_NV_WATCH0                                 0x0000A430 /* RR-4R */
#define NV_PPM_NV_WATCH0_GR_XF2PM_XF_IDLE                      0: 0 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_XF2PM_XF2VTX_DATA_VALID            1: 1 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_XF2PM_XF2IDX_BUSY                  2: 2 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_VTX2PM_VTXPRIMIDLE                 3: 3 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_VTX2PM_VTX2STP_PRIMVALID           4: 4 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_VTX2PM_VTX2IDX_BUSY                5: 5 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_VTX2PM_VTX2COL_VALID               6: 6 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TP1_REPLACER_STALL          7: 7 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TP1_MP_QUEUE_STALL          8: 8 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TP1_COALESCER_STALL         9: 9 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TP0_REPLACER_STALL         10:10 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TP0_MP_QUEUE_STALL         11:11 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TP0_COALESCER_STALL        12:12 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TEX2MATH_TP1_BUSY          13:13 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TEX2MATH_TP0_BUSY          14:14 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TEX2LIT_REQ                15:15 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_TEX2LIT_REGLD              16:16 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_SNAP_PULSE                 17:17 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_RBFR_FULL_STALL            18:18 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_CACHE_IDLE                 19:19 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_TEX2PM_C2DMA_DV                   20:20 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_VTX_OFFSCREEN              21:21 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_STP_IDLE                   22:22 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_STP2VTX_BUSY               23:23 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_STP2RSTR_VALID             24:24 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_STP2RSTR_DO_SWATHS         25:25 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_STP2CLP_VALID              26:26 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_START_PRIM                 27:27 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_POLYMODE                   28:28 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_DUAL_TEXTURE               29:29 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_DO_POINT                   30:30 /* RR-UF */
#define NV_PPM_NV_WATCH0_GR_STP2PM_DO_LINE                    31:31 /* RR-UF */
#define NV_PPM_NV_WATCH1                                 0x0000A434 /* RR-4R */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CYL_WRAP                    0: 0 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CULL_SMALL                  1: 1 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CULL_NEAR                   2: 2 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CULL_FRONT                  3: 3 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CULL_FAR                    4: 4 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CULL_ENA                    5: 5 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CULL_BBOX                   6: 6 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_STP2PM_CULL_BACK                   7: 7 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_SEARCH_MODE                8: 8 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_RSTR_EOPRIM                9: 9 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_RSTR2PIPE_XY_VALID        10:10 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_RSTR2PIPE_EOPOLY          11:11 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_RSTR2MATH_NEWLINE         12:12 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_RSTR2MATH_ABC_VALID       13:13 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_PIXCNT_0                  14:14 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_PIXCNT_1                  15:15 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_PIXCNT_2                  16:16 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_FINE_STALL                17:17 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_FINE_IDLE                 18:18 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_FINE_EDGE_EVAL            19:19 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_FINE_ALIASED              20:20 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_COARSE_VALID              21:21 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_COARSE_STALL              22:22 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_RSTR2PM_COARSE_IDLE               23:23 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_ZWRITE                     24:24 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_ZPARTIAL                   25:25 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_WAITMEMWR                  26:26 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_WAITMEMRD                  27:27 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_WAIT_BUF                   28:28 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_ROP_IDLE                   29:29 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_ROP2FB_REQ                 30:30 /* RR-UF */
#define NV_PPM_NV_WATCH1_GR_ROP2PM_ROP2FB_RD                  31:31 /* RR-UF */
#define NV_PPM_NV_WATCH2                                 0x0000A438 /* RR-4R */
#define NV_PPM_NV_WATCH2_GR_ROP2PM_ROP2FB_ALOM                 0: 0 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_ROP2PM_CWRITE                      1: 1 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_PRE_OUTPUT_BUSY             2: 2 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_R2D2MATH_IM_NEWLINE         3: 3 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_RSTR2D_IDLE                 4: 4 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_R2D2FE_RSTR2D_BUSY          5: 5 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_MATH_CACHE_BUSY             6: 6 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_R2D2PRE_OUTPUT_REQ          7: 7 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_R2D2PRE_OUTPUT_NOP          8: 8 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_R2D2MATH_IM_RIGHT_DV        9: 9 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_R2D2MATH_IM_LEFT_DV        10:10 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_R2D2PM_FE_REQ                     11:11 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_PREROP_IDLE                12:12 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_PRE2ROP_ROP_REQ            13:13 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_PRE2ROP_POSTING_BUFFER     14:14 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_PRE2R2D_OUTPUT_BUSY        15:15 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_PRE2PIPE_BUSY              16:16 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_PRE2LIT_BUSY               17:17 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_COALESCE                   18:18 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PRE2PM_ALL3D_REQ                  19:19 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PIPE2PM_PIPE_XY_RDY               20:20 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_PIPE2PM_PIPE_IDLE                 21:21 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_MATH2PM_MATH2TEX_TP1_REQ          22:22 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_MATH2PM_MATH2TEX_TP0_REQ          23:23 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_MATH2PM_MATH2TEX_MAGNIFY          24:24 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_MATH2PM_MATH2TEX_ANISO_SAMPLE     25:25 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_MATH2PM_MATH2RSTR_RDY             26:26 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_MATH2PM_MATH2RSTR_ABC_RDY         27:27 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_LIT2PM_RECIRC_CYCLE               28:28 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_LIT2PM_LIT2TEX_BUSY               29:29 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_LIT2PM_LIT2PRE_REQ                30:30 /* RR-UF */
#define NV_PPM_NV_WATCH2_GR_LIT2PM_LIT2COL_BUSY               31:31 /* RR-UF */
#define NV_PPM_NV_WATCH3                                 0x0000A43C /* RR-4R */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_THREADSTALLED               0: 0 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_PTEBUSY                     1: 1 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_INTALIGN                    2: 2 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_IDXFRONTIDLE                3: 3 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_IDX2XF_VALID_0              4: 4 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_IDX2XF_VALID_1              5: 5 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_IDX2VTX_VALID               6: 6 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_IDX2PMI_VALID               7: 7 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_IDX2FBI_VALID               8: 8 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_IDX2DEC_BUSY                9: 9 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_COMPOVFL                   10:10 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_IDX2PM_CACHETHRASH                11:11 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FF2PM_FF_GR_RDY                   12:12 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FF2PM_FF_GR_B2B                   13:13 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_NVINTR_GR                   14:14 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_WAIT_GR_IDLE                15:15 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_NOP                         16:16 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_GR_SWITCH                   17:17 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_GR_IDLE                     18:18 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_FBI_REQ_BURST               19:19 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_FBI_REQ                     20:20 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_FBI_RD                      21:21 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_FBI_INST                    22:22 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FE2PM_CACHE_INVALIDATE            23:23 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_FBI2PM_FBI2IDX_BUSY               24:24 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_DMA2PM_DMA2C_DV                   25:25 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_DMA2PM_DMA2C_ASTALL               26:26 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_DEC2PM_DEC2IDX_VALID_0            27:27 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_DEC2PM_DEC2IDX_VALID_1            28:28 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_COL2PM_SPECULAR_RECIRC            29:29 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_COL2PM_COLOR_IDLE                 30:30 /* RR-UF */
#define NV_PPM_NV_WATCH3_GR_COL2PM_COL2XYPIPE_BUSY            31:31 /* RR-UF */
#define NV_PPM_NV_WATCH4                                 0x0000A630 /* RR-4R */
#define NV_PPM_NV_WATCH4_GR_COL2PM_COL2VTX_BUSY                0: 0 /* RR-UF */
#define NV_PPM_NV_WATCH4_GR_COL2PM_COL2LIT_REQ                 1: 1 /* RR-UF */
#define NV_PPM_NV_WATCH4_GR_COL2PM_COL2FCOMB_REQ               2: 2 /* RR-UF */
#define NV_PPM_NV_WATCH4_GR_COL2PM_COL2CLP_BUSY                3: 3 /* RR-UF */
#define NV_PPM_NV_WATCH4_GR_CLP2PM_CLP2STP_BUSY                4: 4 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_PMI2PM_PMI2IDX_VALID              5: 5 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_PMI2PM_PMI2IDX_BUSY               6: 6 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_HOST2PM_TIMER_262US               7: 7 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_HOST2PM_FF_GR_MDV                 8: 8 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_HOST2PM_FF_DHV                    9: 9 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_HOST2PM_FF_CHSW                  10:10 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_FBI2PM_X_DLY                     11:11 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_FBI2PM_S_DLY                     12:12 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_FBI2PM_R2W_DLY                   13:13 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_FBI2PM_H_DLY                     14:14 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_FB2PM_FB2ROP_BUSY                15:15 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_FBI2PM_D_DLY                     16:16 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_FBI2PM_BC                        17:17 /* RR-UF */
#define NV_PPM_NV_WATCH4_TOP_CRTC2PM_CRTC2DAC_VSYNC2DAC       18:18 /* RR-UF */
#define NV_PPM_CONTROL                                   0x0000A73C /* RW-4R */
#define NV_PPM_CONTROL_OUT_SEL                                  0:0 /* RWXUF */
#define NV_PPM_CONTROL_OUT_SEL_PM                        0x00000000 /* RWI-V */
#define NV_PPM_CONTROL_OUT_SEL_FBIDEBUG                  0x00000001 /* RWI-V */
#define NV_PPM_CONTROL_DRIVE_OUT                                1:1 /* RWXUF */
#define NV_PPM_CONTROL_DRIVE_OUT_NORMAL                  0x00000000 /* RWI-V */
#define NV_PPM_CONTROL_DRIVE_OUT_OBSERVE                 0x00000001 /* RWI-V */
#define NV_PPM_CONTROL_NV_ADDTOEVENT                            2:2 /* RWXUF */
#define NV_PPM_CONTROL_NV_ADDTOEVENT_INCR                0x00000000 /* RWI-V */
#define NV_PPM_CONTROL_NV_ADDTOEVENT_ADDTRIG1            0x00000001 /* RWI-V */
#define NV_PPM_CONTROL_NV_STATE                                 4:3 /* RRIUF */
#define NV_PPM_CONTROL_STATE_IDLE                        0x00000000 /* RWI-V */
#define NV_PPM_CONTROL_STATE_WAIT_TRIG0                  0x00000001 /* RWI-V */
#define NV_PPM_CONTROL_STATE_WAIT_TRIG1                  0x00000002 /* RWI-V */
#define NV_PPM_CONTROL_STATE_CAPTURE                     0x00000003 /* RWI-V */

#endif // _NV10PPM_H_
