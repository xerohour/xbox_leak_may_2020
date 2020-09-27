// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvKelvinProgram.h
//      Kelvin program header file
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        23May2000         NV20 development
//
// **************************************************************************

#ifndef _NVKELVINPROGRAM_H
#define _NVKELVINPROGRAM_H

#if (NVARCH >= 0x020)

#include "vtxpgmconsts.h"
#include "vtxpgmcomp.h"


// #define MAX_PROGRAM_COUNT             128
// #define GL_EXECUTE_PROGRAM           0x88888888
// #define GL_DRIVER_EXECUTE_PROGRAM    0x88888889

#define SET_ILU(pucode,x) (pucode->ilu = (x)&7)     /* cycle 0.. */
#define SET_MAC(pucode,x) (pucode->mac = (x)&15)    /* cycle 01. */
#define SET_CA(pucode,x)  (pucode->ca  = (x)&255)   /* cycle 0.2 */
#define SET_VA(pucode,x)  (pucode->va  = (x)&15)    /* cycle 0.. */

#define SET_AMX(pucode,x) (pucode->amx = (x)&3)     /* cycle 0.. */
#define SET_ARR(pucode,x) (pucode->arr = (x)&15)    /* cycle 0.. */
#define SET_ANE(pucode,x) (pucode->ane = (x)&1)     /* cycle 0.. */
#define SET_AXS(pucode,x) (pucode->axs = (x)&3)     /* cycle 0.. */
#define SET_AYS(pucode,x) (pucode->ays = (x)&3)     /* cycle 0.. */
#define SET_AZS(pucode,x) (pucode->azs = (x)&3)     /* cycle 0.. */
#define SET_AWS(pucode,x) (pucode->aws = (x)&3)     /* cycle 0.. */

#define SET_BMX(pucode,x) (pucode->bmx = (x)&3)     /* cycle 0.. */
#define SET_BRR(pucode,x) (pucode->brr = (x)&15)    /* cycle 0.. */
#define SET_BNE(pucode,x) (pucode->bne = (x)&1)     /* cycle 0.. */
#define SET_BXS(pucode,x) (pucode->bxs = (x)&3)     /* cycle 0.. */
#define SET_BYS(pucode,x) (pucode->bys = (x)&3)     /* cycle 0.. */
#define SET_BZS(pucode,x) (pucode->bzs = (x)&3)     /* cycle 0.. */
#define SET_BWS(pucode,x) (pucode->bws = (x)&3)     /* cycle 0.. */

#define SET_CMX(pucode,x) (pucode->cmx = (x)&3)     /* cycle 0.. */
#define SET_CRR(pucode,x) (pucode->crr = (x)&15)    /* cycle 0.. */
#define SET_CNE(pucode,x) (pucode->cne = (x)&1)     /* cycle 0.. */
#define SET_CXS(pucode,x) (pucode->cxs = (x)&3)     /* cycle 0.. */
#define SET_CYS(pucode,x) (pucode->cys = (x)&3)     /* cycle 0.. */
#define SET_CZS(pucode,x) (pucode->czs = (x)&3)     /* cycle 0.. */
#define SET_CWS(pucode,x) (pucode->cws = (x)&3)     /* cycle 0.. */

#define SET_RWM(pucode,x) (pucode->rwm = (x)&15)    /* cycle ..2 */
#define SET_RW(pucode,x)  (pucode->rw  = (x)&15)    /* cycle ..2 */
#define SET_SWM(pucode,x) (pucode->swm = (x)&15)    /* cycle ..2 */
#define SET_OWM(pucode,x) (pucode->owm = (x)&15)    /* cycle ..2 */
#define SET_OC(pucode,x)  (pucode->oc  = (x)&511)   /* cycle ..2 */
#define SET_OM(pucode,x)  (pucode->om  = (x)&1)     /* cycle ..2 */
#define SET_CIN(pucode,x) (pucode->cin = (x)&1)     /* cycle 0.. */
#define SET_EOS(pucode,x) (pucode->eos = (x)&1)     /* cycle 0.. */

#define PGM_UWORDW(pucode) ((pucode->eos << 0) |\
                            (pucode->cin << 1) |\
                            (pucode->om  << 2) |\
                            (pucode->oc  << 3) |\
                            (pucode->owm <<12) |\
                            (pucode->swm <<16) |\
                            (pucode->rw  <<20) |\
                            (pucode->rwm <<24) |\
                            (pucode->cmx <<28) |\
                            (pucode->crr <<30))

#define PGM_UWORDZ(pucode) ((pucode->crr >> 2) |\
                            (pucode->cws << 2) |\
                            (pucode->czs << 4) |\
                            (pucode->cys << 6) |\
                            (pucode->cxs << 8) |\
                            (pucode->cne <<10) |\
                            (pucode->bmx <<11) |\
                            (pucode->brr <<13) |\
                            (pucode->bws <<17) |\
                            (pucode->bzs <<19) |\
                            (pucode->bys <<21) |\
                            (pucode->bxs <<23) |\
                            (pucode->bne <<25) |\
                            (pucode->amx <<26) |\
                            (pucode->arr <<28))

#define PGM_UWORDY(pucode) ((pucode->aws << 0) |\
                            (pucode->azs << 2) |\
                            (pucode->ays << 4) |\
                            (pucode->axs << 6) |\
                            (pucode->ane << 8) |\
                            (pucode->va  << 9) |\
                            (pucode->ca  <<13) |\
                            (pucode->mac <<21) |\
                            (pucode->ilu <<25))

#define PGM_UWORDX(pucode) 0

#define ADD_MASK  (0x0000ff)
#define MUX_MASK  (0x000300)
#define SWZ_MASK  (0x0ff000)
#define SGN_MASK  (0x100000)
#define IDX_MASK  (0x200000)
#define DST_MASK  (0x400000)

#define NV_IGRAPH_XF_V_NOP        0x00
#define NV_IGRAPH_XF_V_MOV        0x01
#define NV_IGRAPH_XF_V_MUL        0x02
#define NV_IGRAPH_XF_V_ADD        0x03
#define NV_IGRAPH_XF_V_MAD        0x04
#define NV_IGRAPH_XF_V_DP3        0x05
#define NV_IGRAPH_XF_V_DPH        0x06
#define NV_IGRAPH_XF_V_DP4        0x07
#define NV_IGRAPH_XF_V_DST        0x08
#define NV_IGRAPH_XF_V_MIN        0x09
#define NV_IGRAPH_XF_V_MAX        0x0a
#define NV_IGRAPH_XF_V_SLT        0x0b
#define NV_IGRAPH_XF_V_SGE        0x0c
#define NV_IGRAPH_XF_V_ARL        0x0d

#define NV_IGRAPH_XF_S_NOP        0x00
#define NV_IGRAPH_XF_S_MOV        0x01
#define NV_IGRAPH_XF_S_RCP        0x02
#define NV_IGRAPH_XF_S_RCC        0x03
#define NV_IGRAPH_XF_S_RSQ        0x04
#define NV_IGRAPH_XF_S_EXP        0x05
#define NV_IGRAPH_XF_S_LOG        0x06
#define NV_IGRAPH_XF_S_LIT        0x07

#define CSW_X                     0x00  // take swizzled source from x
#define CSW_Y                     0x01  // ...from y
#define CSW_Z                     0x02  // ...from z
#define CSW_W                     0x03  // ...from w

#define MX_M                      0     // mux select
#define MX_R                      1     // mux select scratch (a.k.a. temporary) register
#define MX_V                      2     // mux select input (a.k.a. vertex attribute) register
#define MX_C                      3     // mux select context (a.k.a. parameter/constant) register
#define MX_O                      MX_M

#define OM_MAC                    0x00  // output mux selects value produced by MLU/ALU
#define OM_ILU                    0x01  // output mux selects value produced by ILU

typedef struct _t_CheopsUcode {
    unsigned long eos:1;        /* 0:00 last instruction */
    unsigned long cin:1;        /* 0:01 ctx indexed address */
    unsigned long  om:1;        /* 0:02 output mux */
    unsigned long  oc:9;        /* 0:03 output write control */
    unsigned long owm:4;        /* 0:12 output write mask */
    unsigned long swm:4;        /* 0:16 secondary register write mask */
    unsigned long  rw:4;        /* 0:20 register write */
    unsigned long rwm:4;        /* 0:24 primary register write mask */
    unsigned long cmx:2;        /* 0:28 c mux (NA,r1,v,c) */
    unsigned long crr:4;        /* 0:30 c register read */
    unsigned long cws:2;        /* 1:02 c w swizzle */
    unsigned long czs:2;        /* 1:04 c z swizzle */
    unsigned long cys:2;        /* 1:06 c y swizzle */
    unsigned long cxs:2;        /* 1:08 c x swizzle */
    unsigned long cne:1;        /* 1:10 c negate */
    unsigned long bmx:2;        /* 1:11 b mux (NA,r1,v,c) */
    unsigned long brr:4;        /* 1:13 b register read */
    unsigned long bws:2;        /* 1:17 b w swizzle */
    unsigned long bzs:2;        /* 1:19 b z swizzle */
    unsigned long bys:2;        /* 1:21 b y swizzle */
    unsigned long bxs:2;        /* 1:23 b x swizzle */
    unsigned long bne:1;        /* 1:25 b negate */
    unsigned long amx:2;        /* 1:26 a mux (NA,r0,v,c) */
    unsigned long arr:4;        /* 1:28 a register read */
    unsigned long aws:2;        /* 2:00 a w swizzle */
    unsigned long azs:2;        /* 2:02 a z swizzle */
    unsigned long ays:2;        /* 2:04 a y swizzle */
    unsigned long axs:2;        /* 2:06 a x swizzle */
    unsigned long ane:1;        /* 2:08 a negate */
    unsigned long  va:4;        /* 2:09 ibuffer address */
    unsigned long  ca:8;        /* 2:13 ctx address */
    unsigned long mac:4;        /* 2:21 MLU/ALU op */
    unsigned long ilu:3;        /* 2:25 ILU op */
} t_CheopsUcode;

// a program that's been fully compiled and assembled into kelvin microcode
typedef struct _KELVIN_MICRO_PROGRAM {
    //GLuint nid;
    //long pType;
    long ctxwri;
    long outwri;
    long length;
    //long stadd;
    //long bound;
    //long loaded;
    //float x,y,z,w;
    t_CheopsUcode ucode[136];
} KELVIN_MICRO_PROGRAM, *PKELVIN_MICRO_PROGRAM;

// a compiled program in the intermediate format between a D3D token stream and microcode
typedef struct _KELVIN_PROGRAM {
    Instruction code[136];
    DWORD dwNumInstructions;
} KELVIN_PROGRAM, *PKELVIN_PROGRAM;


// prototypes
void nvKelvinInitProgram           (PKELVIN_MICRO_PROGRAM pProgram);
void nvKelvinParseProgram          (PKELVIN_MICRO_PROGRAM pProgram, char *s);
void nvKelvinDownloadProgram       (PNVD3DCONTEXT pContext, PKELVIN_MICRO_PROGRAM pProgram,DWORD dwStartAddress);
void nvKelvinDownloadProgram       (PNVD3DCONTEXT pContext, VertexProgramOutput *pProgramOutput, DWORD dwStartAddress);
void nvKelvinDownloadConstants     (PNVD3DCONTEXT pContext);
void nvKelvinParseVertexShaderCode (PKELVIN_PROGRAM pProgram, ParsedProgram* parsed, DWORD *pCode, DWORD dwCodeSize);


#ifdef KPFS
class CVertexShader;

class CKelvinProgramMan {
    
    class CInstr {
    public:
        CInstr *next;
        DWORD len;    
        DWORD handle; 
        DWORD index;  
    };
    
    PNVD3DCONTEXT m_pContext, m_pPrevContext;
    DWORD m_dwInstructions;
    DWORD m_dwOffset;
    DWORD m_dwLastHandle;
    DWORD m_dwDirty;
    CInstr* ins; 
    
    void add (DWORD handle, DWORD num_instr);
    bool find(DWORD handle, DWORD num_instr);
    
    public:
        
        CKelvinProgramMan()  {};
        ~CKelvinProgramMan() {};
        
        void create(DWORD);
        void destroy();
        void flush();
        void download(PNVD3DCONTEXT pContext, CVertexShader* pVertexShader);
        void download_special(PNVD3DCONTEXT pContext, PKELVIN_MICRO_PROGRAM pProgram, DWORD handle);
        void display();
        inline void dirty() {m_dwDirty=1;}
};
#endif 

#endif  // NVARCH >= 0x020

#endif  // _NVKELVINPROGRAM_H
