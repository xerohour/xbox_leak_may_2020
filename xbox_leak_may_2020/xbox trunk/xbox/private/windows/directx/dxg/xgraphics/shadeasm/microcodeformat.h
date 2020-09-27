///////////////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// Microcode structures, enumerations, and macros, for the xbox vertex shader
///////////////////////////////////////////////////////////////////////////////

#define MAC_NOP        0x00
#define MAC_MOV        0x01
#define MAC_MUL        0x02
#define MAC_ADD        0x03
#define MAC_MAD        0x04
#define MAC_DP3        0x05
#define MAC_DPH        0x06
#define MAC_DP4        0x07
#define MAC_DST        0x08
#define MAC_MIN        0x09
#define MAC_MAX        0x0a
#define MAC_SLT        0x0b
#define MAC_SGE        0x0c
#define MAC_ARL        0x0d

#define ILU_NOP        0x00
#define ILU_MOV        0x01
#define ILU_RCP        0x02
#define ILU_RCC        0x03
#define ILU_RSQ        0x04
#define ILU_EXP        0x05
#define ILU_LOG        0x06
#define ILU_LIT        0x07

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

typedef struct _D3DVsInstruction {
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
} D3DVsInstruction;


#define MAX_MICROCODE_LENGTH (136*3)

// a program that's been fully compiled and assembled into kelvin microcode
typedef struct _D3DVertexShaderProgram {
    long length;
    D3DVsInstruction ucode[MAX_MICROCODE_LENGTH];
} D3DVertexShaderProgram, *PD3DVertexShaderProgram;


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


//internal register enumeration

#pragma warning(disable:4244)
typedef enum Register_Enum {

    REG_V0 = 0,   REG_V_OPOS = 0,  
	REG_V1 = 1,   REG_V_WGHT = 1,
    REG_V2 = 2,   REG_V_NRML = 2,  
	REG_V3 = 3,   REG_V_COL0 = 3,
    REG_V4 = 4,   REG_V_COL1 = 4,  
	REG_V5 = 5,   REG_V_FOGC = 5,
    REG_V6 = 6,                    
	REG_V7 = 7,
    REG_V8 = 8,   REG_V_TEX0 = 8,  
	REG_V9 = 9,   REG_V_TEX1 = 9,
    REG_V10 = 10, REG_V_TEX2 = 10, 
	REG_V11 = 11, REG_V_TEX3 = 11,
    REG_V12 = 12, REG_V_TEX4 = 12, 
	REG_V13 = 13, REG_V_TEX5 = 13,
    REG_V14 = 14, REG_V_TEX6 = 14, 
	REG_V15 = 15, REG_V_TEX7 = 15,

    REG_O0 = 16,  REG_oPos = REG_O0,
    REG_O1 = 17,  
    REG_O2 = 18,  
    REG_O3 = 19,  REG_oD0  = REG_O3,
    REG_O4 = 20,  REG_oD1  = REG_O4,
    REG_O5 = 21,  REG_oFog = REG_O5,
    REG_O6 = 22,  REG_oPts = REG_O6,
    REG_O7 = 23,  REG_oB0  = REG_O7,
    REG_O8 = 24,  REG_oB1  = REG_O8,
    REG_O9 = 25,  REG_oT0  = REG_O9,
    REG_O10 = 26, REG_oT1  = REG_O10,
    REG_O11 = 27, REG_oT2  = REG_O11,
    REG_O12 = 28, REG_oT3  = REG_O12,
    REG_O13 = 29, REG_oT4  = REG_O13,
    REG_O14 = 30, REG_oT5  = REG_O14,
    REG_O15 = 31, REG_oT6  = REG_O15,

	//note: REG_C0 corresponds with c-96, REG_C95 is c-1, etc. 
	//todo: We don't really need these...
    REG_C0 = 32,   
	/*REG_C1 = 33,   REG_C2 = 34,   REG_C3 = 35,
    REG_C4 = 36,   REG_C5 = 37,   REG_C6 = 38,   REG_C7 = 39,
    REG_C8 = 40,   REG_C9 = 41,   REG_C10 = 42,  REG_C11 = 43,
    REG_C12 = 44,  REG_C13 = 45,  REG_C14 = 46,  REG_C15 = 47,
    REG_C16 = 48,  REG_C17 = 49,  REG_C18 = 50,  REG_C19 = 51,
    REG_C20 = 52,  REG_C21 = 53,  REG_C22 = 54,  REG_C23 = 55,
    REG_C24 = 56,  REG_C25 = 57,  REG_C26 = 58,  REG_C27 = 59,
    REG_C28 = 60,  REG_C29 = 61,  REG_C30 = 62,  REG_C31 = 63,
    REG_C32 = 64,  REG_C33 = 65,  REG_C34 = 66,  REG_C35 = 67,
    REG_C36 = 68,  REG_C37 = 69,  REG_C38 = 70,  REG_C39 = 71,
    REG_C40 = 72,  REG_C41 = 73,  REG_C42 = 74,  REG_C43 = 75,
    REG_C44 = 76,  REG_C45 = 77,  REG_C46 = 78,  REG_C47 = 79,
    REG_C48 = 80,  REG_C49 = 81,  REG_C50 = 82,  REG_C51 = 83,
    REG_C52 = 84,  REG_C53 = 85,  REG_C54 = 86,  REG_C55 = 87,
    REG_C56 = 88,  REG_C57 = 89,  REG_C58 = 90,  REG_C59 = 91,
    REG_C60 = 92,  REG_C61 = 93,  REG_C62 = 94,  REG_C63 = 95,
    REG_C64 = 96,  REG_C65 = 97,  REG_C66 = 98,  REG_C67 = 99,
    REG_C68 = 100, REG_C69 = 101, REG_C70 = 102, REG_C71 = 103,
    REG_C72 = 104, REG_C73 = 105, REG_C74 = 106, REG_C75 = 107,
    REG_C76 = 108, REG_C77 = 109, REG_C78 = 110, REG_C79 = 111,
    REG_C80 = 112, REG_C81 = 113, REG_C82 = 114, REG_C83 = 115,
    REG_C84 = 116, REG_C85 = 117, REG_C86 = 118, REG_C87 = 119,
    REG_C88 = 120, REG_C89 = 121, REG_C90 = 122, REG_C91 = 123,
    REG_C92 = 124, REG_C93 = 125, REG_C94 = 126, */REG_C95 = 127,
	// Also 128..223

    REG_R0 = 128+96,  REG_R1 = 129+96,  REG_R2 = 130+96,  REG_R3 = 131+96,
    REG_R4 = 132+96,  REG_R5 = 133+96,  REG_R6 = 134+96,  REG_R7 = 135+96,
    REG_R8 = 136+96,  REG_R9 = 137+96,  REG_R10 = 138+96, REG_R11 = 139+96,
	
	//the following aren't valid, but are used for spacing...
    REG_R12 = 140+96, REG_R13 = 141+96, REG_R14 = 142+96, REG_R15 = 143+96,

    REG_ARL = 144+96,

    REG_ZER = 145+96,

} Register_t;

