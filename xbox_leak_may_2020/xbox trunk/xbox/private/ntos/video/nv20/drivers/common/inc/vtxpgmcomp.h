/*
** vtxpgmcomp.h
**
** Vertex Program Declarations.
**
** Copyright 2000 NVIDIA, Corporation.  All rights reserved.
**
** THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
** NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
** IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*/

/*
** This file is shared between OpenGL and D3D so don't mess with it!
**
** In particular, don't use any OpenGL or D3D specific types.  There are
** plenty of perfectly good types in the C language itself.
**
*/

#ifndef __gl_vtxpgmcomp_h_
#define __gl_vtxpgmcomp_h_

#include "vtxpgmconsts.h"

#define NV_INTERNAL_OPCODES 1

/*
** Optimization flags:
*/

#define OPT_LIVE_DEAD 1
#define OPT_HPOS_ONLY 2
#define OPT_ALL       1

/*
** Structures for instruction description.
*/

typedef enum OpcodeClasses_Enum {
    CLASS_ARL = 0,
    CLASS_VECTOR,
    CLASS_SCALAR,
    CLASS_BIN,
    CLASS_TRI,
#if defined(NV_INTERNAL_OPCODES)
    CLASS_NOP,
#endif
    CLASS_END,
} OpcodeClasses;

typedef enum Opcodes_Enum {

    // END op:

    OP_END = 0,
#if defined(NV_INTERNAL_OPCODES)
    OP_NOP,
#endif

    // ARL op:

    OP_ARL,

    // VECTOR ops:

    OP_LIT, OP_MOV,
#if defined(NV_INTERNAL_OPCODES)
    OP_IMV,
#endif

    // SCALAR ops:

    OP_EXP, OP_LOG, OP_RCP, OP_RSQ,
#if defined(NV_INTERNAL_OPCODES)
    OP_RCC,
#endif

    // BIN ops:

    OP_ADD, OP_DP3, OP_DP4, OP_DST, OP_MAX, OP_MIN, OP_MUL, OP_SGE, OP_SLT,
#if defined(NV_INTERNAL_OPCODES)
    OP_DPH,
#endif

    // TRI op:

    OP_MAD,
} Opcodes;

#define VERTEX_PROGRAM_OPCODE_NAMES                                         \
    "END", "NOP",                                                           \
    "ARL",                                                                  \
    "LIT", "MOV", "IMV",                                                    \
    "EXP", "LOG", "RCP", "RSQ", "RCC",                                      \
    "ADD", "DP3", "DP4", "DST", "MAX", "MIN", "MUL", "SGE", "SLT", "DPH",   \
    "MAD",

typedef enum Component_Enum {
    XX = 0, YY, ZZ, WW,
} Component_t;

typedef enum Component4_Enum {
    XYZW = (XX | YY << 2 | ZZ << 4 | WW << 6),
} Component4;

typedef enum DestMaskBits_Enum {
    DST_NONE_BITS = 0,
    DST_X_BIT = 1, DST_Y_BIT = 2, DST_Z_BIT = 4, DST_W_BIT = 8,
    DST_YZ_BITS = 6,
    DST_XYZ_BITS = 7,
    DST_XYW_BITS = 11,
    DST_XYZW_BITS = 15,
} DestMaskBits;

typedef enum Register_Enum {

    VA_0 = 0,   VA_OPOS = 0,  VA_1 = 1,   VA_WGHT = 1,
    VA_2 = 2,   VA_NRML = 2,  VA_3 = 3,   VA_COL0 = 3,
    VA_4 = 4,   VA_COL1 = 4,  VA_5 = 5,   VA_FOGC = 5,
    VA_6 = 6,                 VA_7 = 7,
    VA_8 = 8,   VA_TEX0 = 8,  VA_9 = 9,   VA_TEX1 = 9,
    VA_10 = 10, VA_TEX2 = 10, VA_11 = 11, VA_TEX3 = 11,
    VA_12 = 12, VA_TEX4 = 12, VA_13 = 13, VA_TEX5 = 13,
    VA_14 = 14, VA_TEX6 = 14, VA_15 = 15, VA_TEX7 = 15,

    VR_0 = 16,  VR_HPOS = VR_0,
    VR_1 = 17,  
    VR_2 = 18,  
    VR_3 = 19,  VR_COL0 = VR_3,
    VR_4 = 20,  VR_COL1 = VR_4,
    VR_5 = 21,  VR_FOGC = VR_5,
    VR_6 = 22,  VR_PSIZ = VR_6,
    VR_7 = 23,  VR_BFC0 = VR_7,
    VR_8 = 24,  VR_BFC1 = VR_8,
    VR_9 = 25,  VR_TEX0 = VR_9,
    VR_10 = 26, VR_TEX1 = VR_10,
    VR_11 = 27, VR_TEX2 = VR_11,
    VR_12 = 28, VR_TEX3 = VR_12,
    VR_13 = 29, VR_TEX4 = VR_13,
    VR_14 = 30, VR_TEX5 = VR_14,
    VR_15 = 31, VR_TEX6 = VR_15, VR_TEX7 = VR_15,

    PR_0 = 32,   PR_1 = 33,   PR_2 = 34,   PR_3 = 35,
    PR_4 = 36,   PR_5 = 37,   PR_6 = 38,   PR_7 = 39,
    PR_8 = 40,   PR_9 = 41,   PR_10 = 42,  PR_11 = 43,
    PR_12 = 44,  PR_13 = 45,  PR_14 = 46,  PR_15 = 47,
    PR_16 = 48,  PR_17 = 49,  PR_18 = 50,  PR_19 = 51,
    PR_20 = 52,  PR_21 = 53,  PR_22 = 54,  PR_23 = 55,
    PR_24 = 56,  PR_25 = 57,  PR_26 = 58,  PR_27 = 59,
    PR_28 = 60,  PR_29 = 61,  PR_30 = 62,  PR_31 = 63,
    PR_32 = 64,  PR_33 = 65,  PR_34 = 66,  PR_35 = 67,
    PR_36 = 68,  PR_37 = 69,  PR_38 = 70,  PR_39 = 71,
    PR_40 = 72,  PR_41 = 73,  PR_42 = 74,  PR_43 = 75,
    PR_44 = 76,  PR_45 = 77,  PR_46 = 78,  PR_47 = 79,
    PR_48 = 80,  PR_49 = 81,  PR_50 = 82,  PR_51 = 83,
    PR_52 = 84,  PR_53 = 85,  PR_54 = 86,  PR_55 = 87,
    PR_56 = 88,  PR_57 = 89,  PR_58 = 90,  PR_59 = 91,
    PR_60 = 92,  PR_61 = 93,  PR_62 = 94,  PR_63 = 95,
    PR_64 = 96,  PR_65 = 97,  PR_66 = 98,  PR_67 = 99,
    PR_68 = 100, PR_69 = 101, PR_70 = 102, PR_71 = 103,
    PR_72 = 104, PR_73 = 105, PR_74 = 106, PR_75 = 107,
    PR_76 = 108, PR_77 = 109, PR_78 = 110, PR_79 = 111,
    PR_80 = 112, PR_81 = 113, PR_82 = 114, PR_83 = 115,
    PR_84 = 116, PR_85 = 117, PR_86 = 118, PR_87 = 119,
    PR_88 = 120, PR_89 = 121, PR_90 = 122, PR_91 = 123,
    PR_92 = 124, PR_93 = 125, PR_94 = 126, PR_95 = 127,

    TR_0 = 128,  TR_1 = 129,  TR_2 = 130,  TR_3 = 131,
    TR_4 = 132,  TR_5 = 133,  TR_6 = 134,  TR_7 = 135,
    TR_8 = 136,  TR_9 = 137,  TR_10 = 138, TR_11 = 139,
    TR_12 = 140, TR_13 = 141, TR_14 = 142, TR_15 = 143,

    ARL = 144,

    ZER = 145,

    PPR_0 = 146, PPR_23 = 169, PPR_58 = 204, PPR_59 = 205, PPR_62 = 208,

} Register_t;

typedef struct SrcReg_Rec {
    Register_t reg;
    unsigned char AddrReg;
    int addrRegOffset;
} SrcReg;

typedef struct ScalarSrcReg_Rec {
    SrcReg reg;
    Component_t com;
    unsigned char Signed;
} ScalarSrcReg;

typedef struct SwizzleSrcReg_Rec {
    SrcReg reg;
    unsigned char com4;
    unsigned char Signed;
} SwizzleSrcReg;

typedef struct MaskedDstReg_Rec {
    Register_t reg;
    unsigned char mask;
} MaskedDstReg;

typedef struct Arl_Instruction_Rec {
    Opcodes opcode;
    OpcodeClasses opclass;

    ScalarSrcReg src;
} Arl_Instruction;

typedef struct Vector_Instruction_Rec {
    Opcodes opcode;
    OpcodeClasses opclass;

    MaskedDstReg dst;
    SwizzleSrcReg src;
} Vector_Instruction;

typedef struct Scalar_Instruction_Rec {
    Opcodes opcode;
    OpcodeClasses opclass;

    MaskedDstReg dst;
    ScalarSrcReg src;
} Scalar_Instruction;

typedef struct Bin_Instruction_Rec {
    Opcodes opcode;
    OpcodeClasses opclass;

    MaskedDstReg dst;
    SwizzleSrcReg srcA;
    SwizzleSrcReg srcB;
} Bin_Instruction;

typedef struct Tri_Instruction_Rec {
    Opcodes opcode;
    OpcodeClasses opclass;

    MaskedDstReg dst;
    SwizzleSrcReg srcA;
    SwizzleSrcReg srcB;
    SwizzleSrcReg srcC;
} Tri_Instruction;

typedef struct End_Instruction_Rec {
    Opcodes opcode;
    OpcodeClasses opclass;
} End_Instruction;

typedef union Instruction_Rec {
    Arl_Instruction arl_op;
    Vector_Instruction vector_op;
    Scalar_Instruction scalar_op;
    Bin_Instruction bin_op;
    Tri_Instruction tri_op;
    End_Instruction end_op;
} Instruction;

typedef struct ParsedProgram_Rec {
    Instruction *firstInstruction;  // Pointer to array of instructions
    unsigned int liveOnEntry;       // Mask of "live" temp registers upon entry
    unsigned char resultRegsWritten[__GL_NUMBER_OF_RESULT_REGISTERS];
                                    // Mask of result registers written
    int IsStateProgram;             // True if this is a state program
    Instruction instArray[1];
} ParsedProgram;

void vp_Optimize(ParsedProgram *parsed, int numInstructions, int optimizeMask);

/*
** Common structure for X86 compilation of vertex programs
*/

#define CALLER_ID_D3D    0xD3D
#define CALLER_ID_OPENGL 0xD4D

typedef struct VtxProgCompileX86_Rec {
    void *(* malloc)(void *tag, size_t size);
    void (* free)(void *tag, void *address);
    void *(* exec_malloc)(void *tag, size_t size);
    void *mallocArg;
    float (* expf)(float f);
    float (* logf)(float f);
    int caller_id;
    int attrib_offset;
    int temp_offset;
    int result_offset;
    int param_offset;
    int float_zero_offset;
    int float_one_offset;
    // Used to compile code to fill pushbuffer directly:
    int channel_number;
    int enables_offset;
} VtxProgCompileX86;

typedef struct VertexProgramOutput_Rec {
    void *residentProgram;
    size_t residentSize;
    int residentNumInstructions;
} VertexProgramOutput;

int vp_CompileX86(VtxProgCompileX86 *, ParsedProgram *, int numInstructions,
                  VertexProgramOutput *);
int vp_OutputCelsiusX86(VtxProgCompileX86 *, ParsedProgram *, int numInstructions,
                  VertexProgramOutput *);

/*
** Common structure for Kelvin compilation of vertex programs
*/

typedef struct vtxpgmInstPacked_Rec {
    unsigned int x, y, z, w;
} vtxpgmInstPacked;

typedef struct VtxProgCompileKelvin_Rec {
    void *(* malloc)(void *tag, size_t size);
    void (* free)(void *tag, void *address);
    void *mallocArg;
    int caller_id;
    int inputRegMap[16];
    int outputRegMap[16];
    int userMode;
    // No need to init by D3D.  OpenGL only.
    unsigned int textureScaling;
    int passthroughMode;
    // Internally used members.  No need to initialize by caller:
    struct vtxpgmTLiveDead_Rec *tLiveDead;
    int numExtraInstructions;
} VtxProgCompileKelvin;

int vp_CompileKelvin(VtxProgCompileKelvin *, ParsedProgram *, int numInstructions,
                     VertexProgramOutput *);

/*
** Static global data used in shared code to call imported functions:
**
** TEMPORARY!!!!!!!
*/


struct VtxProgImports_Rec {
    float (* expf)(float f);
    float (* logf)(float f);
};


extern struct VtxProgImports_Rec VtxProgImports;

#endif /* __gl_vtxpgmcomp_h_ */
