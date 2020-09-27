/*
 * vpoptimize.c
 *
 * Copyright 2000 NVIDIA, Corporation.  All rights reserved.
 * 
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/*
** This file is shared between OpenGL and D3D so don't mess with it!
*/

/*
** This file should only be compiled if it's needed.  The normal OpenGL ifdefs:
**
**      #if defined(GL_NV_vertex_program) && __NV_ACCEL>=10
**      #ifdef __GL_USE_OG_INTEL_ASM
**
** and any OpenGL includes like:
**
**      #include "context.h"
**
** can't be used since this is used by D3D as well.
**
*/

#include <stddef.h>

#include "vtxpgmconsts.h"
#include "vtxpgmcomp.h"

#if !defined(TPRINTF)
#define TPRINTF(X)
#define NV_TRACE(X, Y, Z)
#define NV_TRACE_COND(X, Y, Z)
#endif

typedef struct vpOptimize_Rec {
    unsigned char LiveDead[__GL_NUMBER_OF_TEMPORARY_REGISTERS][4];
    unsigned char LiveMask[__GL_MAX_NUMBER_OF_INSTRUCTIONS];
    unsigned char resultRegsWritten[__GL_NUMBER_OF_RESULT_REGISTERS];
    int optimizeMask;
} vpOptimize;

/*
** lPrintVertexProgram() - Print a vertex program using TPRINTF.
*/

#if defined(_DEBUG)

static char *opcodes[] = { VERTEX_PROGRAM_OPCODE_NAMES };

static void lPrintReg(int freg, int Relative, int Offset)
{
    switch (freg) {
    case VA_0:  case VA_1:  case VA_2:  case VA_3:
    case VA_4:  case VA_5:  case VA_6:  case VA_7:
    case VA_8:  case VA_9:  case VA_10: case VA_11:
    case VA_12: case VA_13: case VA_14: case VA_15:
        TPRINTF(("v[%i]", freg - VA_0));
        break;
    case VR_0:  case VR_1:  case VR_2:  case VR_3:
    case VR_4:  case VR_5:  case VR_6:  case VR_7:
    case VR_8:  case VR_9:  case VR_10: case VR_11:
    case VR_12: case VR_13: case VR_14: case VR_15:
        TPRINTF(("o[%i]", freg - VR_0));
        break;
    case PR_0:  case PR_1:  case PR_2:  case PR_3:
    case PR_4:  case PR_5:  case PR_6:  case PR_7:
    case PR_8:  case PR_9:  case PR_10: case PR_11:
    case PR_12: case PR_13: case PR_14: case PR_15:
    case PR_16: case PR_17: case PR_18: case PR_19:
    case PR_20: case PR_21: case PR_22: case PR_23:
    case PR_24: case PR_25: case PR_26: case PR_27:
    case PR_28: case PR_29: case PR_30: case PR_31:
    case PR_32: case PR_33: case PR_34: case PR_35:
    case PR_36: case PR_37: case PR_38: case PR_39:
    case PR_40: case PR_41: case PR_42: case PR_43:
    case PR_44: case PR_45: case PR_46: case PR_47:
    case PR_48: case PR_49: case PR_50: case PR_51:
    case PR_52: case PR_53: case PR_54: case PR_55:
    case PR_56: case PR_57: case PR_58: case PR_59:
    case PR_60: case PR_61: case PR_62: case PR_63:
    case PR_64: case PR_65: case PR_66: case PR_67:
    case PR_68: case PR_69: case PR_70: case PR_71:
    case PR_72: case PR_73: case PR_74: case PR_75:
    case PR_76: case PR_77: case PR_78: case PR_79:
    case PR_80: case PR_81: case PR_82: case PR_83:
    case PR_84: case PR_85: case PR_86: case PR_87:
    case PR_88: case PR_89: case PR_90: case PR_91:
    case PR_92: case PR_93: case PR_94: case PR_95:
        if (Relative) {
            TPRINTF(("c[A0.x+%i]", Offset));
        } else {
            TPRINTF(("c[%i]", freg - PR_0));
        }
        break;
    case TR_0:  case TR_1:  case TR_2:  case TR_3:
    case TR_4:  case TR_5:  case TR_6:  case TR_7:
    case TR_8:  case TR_9:  case TR_10: case TR_11:
    case TR_12: case TR_13: case TR_14: case TR_15:
        TPRINTF(("R%i", freg - TR_0));
        break;
    default:
        TPRINTF(("<<Bad Reg #%02x>>", freg));
    }
} // lPrintSrcReg

static void lPrintScalarSrcReg(ScalarSrcReg *reg)
{
    if (reg->Signed)
        TPRINTF((", -"));
    else
        TPRINTF((", "));
    lPrintReg(reg->reg.reg, reg->reg.AddrReg, reg->reg.addrRegOffset);
    TPRINTF((".%c", "xyzw"[reg->com & 3]));
} // lPrintScalarSrcReg

static void lPrintSwizzleSrcReg(SwizzleSrcReg *reg)
{
    char s[6];

    if (reg->Signed)
        TPRINTF((", -"));
    else
        TPRINTF((", "));
    lPrintReg(reg->reg.reg, reg->reg.AddrReg, reg->reg.addrRegOffset);
    if (reg->com4 != XYZW) {
        s[0] = '.';
        s[1] = "xyzw"[reg->com4 & 3];
        s[2] = "xyzw"[(reg->com4 >> 2) & 3];
        s[3] = "xyzw"[(reg->com4 >> 4) & 3];
        s[4] = "xyzw"[(reg->com4 >> 6) & 3];
        s[5] = '\0';
        TPRINTF(("%s", s));
    }
} // lPrintSwizzleSrcReg

static void lPrintMaskedDstReg(MaskedDstReg *reg)
{
    int mask;

    lPrintReg(reg->reg, 0, 0);
    if (reg->mask != DST_XYZW_BITS) {
        mask = reg->mask;
        TPRINTF(("."));
        if (mask & DST_X_BIT)
            TPRINTF(("x"));
        if (mask & DST_Y_BIT)
            TPRINTF(("y"));
        if (mask & DST_Z_BIT)
            TPRINTF(("z"));
        if (mask & DST_W_BIT)
            TPRINTF(("w"));
    }
} // lPrintMaskedDstReg

static void lPrintVertexProgram(ParsedProgram *parsed, int numInstructions, char *LiveMask)
{
    Instruction *inst;
    Arl_Instruction *ainst;
    Scalar_Instruction *sinst;
    Vector_Instruction *vinst;
    Bin_Instruction *binst;
    Tri_Instruction *tinst;
    int pc, lop, lmask, DeadInstruction;

    inst = parsed->firstInstruction;
    for (pc = 0; pc < numInstructions; inst++, pc++) {
        lop = inst->end_op.opcode;
        DeadInstruction = 0;
        TPRINTF(("%03d:  ", pc));
        if (LiveMask) {
            lmask = LiveMask[pc];
            if (lmask & 0x10) {
                TPRINTF(("      "));
            } else if (lmask & 0x20) {
                TPRINTF(("....  "));
                DeadInstruction = 1;
            } else {
                TPRINTF(("%c%c%c%c  ", "-*"[lmask & 1], "-*"[(lmask >> 1) & 1],
                                       "-*"[(lmask >> 2) & 1], "-*"[(lmask >> 3) & 1]));
                if (!(lmask & 0xf))
                    DeadInstruction = 1;
            }
        }
        TPRINTF(("%s  ", opcodes[lop]));
        switch (lop) {

        // ARL op:

        case OP_ARL:
            ainst = (Arl_Instruction *) inst;
            TPRINTF(("A0.x"));
            lPrintScalarSrcReg(&ainst->src);
            break;

        // SCALAR ops:

        case OP_EXP:
        case OP_LOG:
#if defined(NV_INTERNAL_OPCODES)
        case OP_RCC:
#endif // NV_INTERNAL_OPCODES
        case OP_RCP:
        case OP_RSQ:
            sinst = (Scalar_Instruction *) inst;
            lPrintMaskedDstReg(&sinst->dst);
            lPrintScalarSrcReg(&sinst->src);
            break;

        // VECTOR ops:

        case OP_LIT:
#if defined(NV_INTERNAL_OPCODES)
        case OP_IMV:
#endif // NV_INTERNAL_OPCODES
        case OP_MOV:
            vinst = (Vector_Instruction *) inst;
            lPrintMaskedDstReg(&vinst->dst);
            lPrintSwizzleSrcReg(&vinst->src);
            break;

        // BIN ops:

        case OP_ADD:
        case OP_DP3:
        case OP_DP4:
#if defined(NV_INTERNAL_OPCODES)
        case OP_DPH:
#endif // NV_INTERNAL_OPCODES
        case OP_DST:
        case OP_MAX:
        case OP_MIN:
        case OP_MUL:
        case OP_SGE:
        case OP_SLT:
            binst = (Bin_Instruction *) inst;
            lPrintMaskedDstReg(&binst->dst);
            lPrintSwizzleSrcReg(&binst->srcA);
            lPrintSwizzleSrcReg(&binst->srcB);
            break;

        // TRIops:

        case OP_MAD:
            tinst = (Tri_Instruction *) inst;
            lPrintMaskedDstReg(&tinst->dst);
            lPrintSwizzleSrcReg(&tinst->srcA);
            lPrintSwizzleSrcReg(&tinst->srcB);
            lPrintSwizzleSrcReg(&tinst->srcC);
            break;

        // NOPops:

        case OP_NOP:
            break;

        // ENDops:

        case OP_END:
            break;

        default:
            TPRINTF((" ???"));
            break;
        }
        if (DeadInstruction) {
            TPRINTF(("  <<---<< Dead Instruction"));
        }
        TPRINTF(("\n"));
    }
} // lPrintVertexProgram
#endif // defined(_DEBUG)

#define LIVE 1
#define DEAD 0

/*
** lSetMaskedDstReg() - Mark registers set as "dead" prior to this point.
*/

static void lSetMaskedDstReg(vpOptimize *opData, int pc, MaskedDstReg *reg)
{
    unsigned char *LiveDead, *LiveMask;
    int mask, lreg;

    LiveMask = &opData->LiveMask[pc];
    lreg = reg->reg;
    if (lreg >= TR_0 && lreg <= TR_11) {
        LiveDead = opData->LiveDead[lreg - TR_0];
        mask = reg->mask;

        // Save current live bits for this register:

        LiveMask[0] = 0; // Mark as a temp register
        if (LiveDead[0] && (mask & DST_X_BIT))
            LiveMask[0] |= DST_X_BIT;
        if (LiveDead[1] && (mask & DST_Y_BIT))
            LiveMask[0] |= DST_Y_BIT;
        if (LiveDead[2] && (mask & DST_Z_BIT))
            LiveMask[0] |= DST_Z_BIT;
        if (LiveDead[3] && (mask & DST_W_BIT))
            LiveMask[0] |= DST_W_BIT;

        // Are we computing any values that won't be used?

        if (mask & ~LiveMask[0]) {
            NV_TRACE(TR_PROGRAM, 30, ("** Dead values: %03d  R%d sets=%d, but live=%d",
                                      pc, lreg - TR_0, mask, LiveMask[0]));
            mask = mask & LiveMask[0];
            if (opData->optimizeMask & (OPT_LIVE_DEAD | OPT_HPOS_ONLY)) {
                reg->mask = (unsigned char) mask;
                NV_TRACE(TR_PROGRAM, 30, ("\n"))
            } else {
                NV_TRACE(TR_PROGRAM, 30, (" ** Optimization Disabled\n"))
            }
        }

        // Mark any registers set as dead:

        if (mask & DST_X_BIT)
            LiveDead[0] = DEAD;
        if (mask & DST_Y_BIT)
            LiveDead[1] = DEAD;
        if (mask & DST_Z_BIT)
            LiveDead[2] = DEAD;
        if (mask & DST_W_BIT)
            LiveDead[3] = DEAD;
    } else {
        if (lreg >= VR_0 && lreg <= VR_15) {
            if ((opData->optimizeMask & OPT_HPOS_ONLY) && lreg != VR_0) {
                reg->mask = 0;
                LiveMask[0] = 0x20; // Dead write to non HPOS output register
            }
            opData->resultRegsWritten[lreg - VR_0] |= reg->mask;
        }
    }
} // lSetMaskedDstReg

/*
** lUseScalarSrcReg() - Mark referenced registers as "live" prior to this point.
*/

static void lUseScalarSrcReg(vpOptimize *opData, char mask, ScalarSrcReg *reg)
{
    unsigned char *LiveDead;
    int lreg;

    lreg = reg->reg.reg;
    if (lreg >= TR_0 && lreg <= TR_11) {
        LiveDead = opData->LiveDead[lreg - TR_0];
        if (mask) // Any output means we need the input
            LiveDead[reg->com & 3] = LIVE;
    }
} // lUseScalarSrcReg

/*
** lUseSwizzleSrcReg() - Mark referenced registers as "live" prior to this point.
*/

static void lUseSwizzleSrcReg(vpOptimize *opData, char mask, SwizzleSrcReg *reg)
{
    unsigned char *LiveDead;
    int lreg;

    lreg = reg->reg.reg;
    if (lreg >= TR_0 && lreg <= TR_11) {
        LiveDead = opData->LiveDead[lreg - TR_0];
        if (mask & DST_X_BIT)
            LiveDead[reg->com4 & 3] = LIVE;
        if (mask & DST_Y_BIT)
            LiveDead[(reg->com4 >> 2) & 3] = LIVE;
        if (mask & DST_Z_BIT)
            LiveDead[(reg->com4 >> 4) & 3] = LIVE;
        if (mask & DST_W_BIT)
            LiveDead[(reg->com4 >> 6) & 3] = LIVE;
    }
} // lUseSwizzleSrcReg

/*
** vp_Optimize() - Optimize a vertex program.
*/

void vp_Optimize(ParsedProgram *parsed, int numInstructions, int optimizeMask)
{
    vpOptimize opData;
    Instruction *inst;
    Arl_Instruction *ainst;
    Scalar_Instruction *sinst;
    Vector_Instruction *vinst;
    Bin_Instruction *binst;
    Tri_Instruction *tinst;
    int lop, pc, bit;
    char dstMask;

    NV_TRACE_COND(TR_PROGRAM, 30, {
        TPRINTF(("************ Entering optimizer.  Initial program is: ***************\n"));
        lPrintVertexProgram(parsed, numInstructions, NULL);
    });
    NV_TRACE_COND(TR_PROGRAM, 30, {
        if (optimizeMask & OPT_HPOS_ONLY) {
            TPRINTF(("************************************\n"));
            TPRINTF(("*** Generating HPOS-only Program ***\n"));
            TPRINTF(("************************************\n"));
        }
    });

    for (pc = numInstructions - 1; pc >= 0; pc--)
        opData.LiveMask[pc] = 0;
    for (pc = __GL_NUMBER_OF_TEMPORARY_REGISTERS - 1; pc >= 0; pc--)
        opData.LiveDead[pc][0] = opData.LiveDead[pc][1] =
                opData.LiveDead[pc][2] = opData.LiveDead[pc][3] = 0;
    for (pc = 0; pc < __GL_NUMBER_OF_RESULT_REGISTERS; pc++)
        opData.resultRegsWritten[pc] = 0;
    opData.optimizeMask = optimizeMask;

    for (pc = numInstructions - 1; pc >= 0; pc--) {
        opData.LiveMask[pc] = 0x10; // Not a temp register
        inst = &parsed->firstInstruction[pc];
        lop = inst->end_op.opcode;
        switch (lop) {

        // ARL op:

        case OP_ARL:
            ainst = (Arl_Instruction *) inst;
            lUseScalarSrcReg(&opData, DST_X_BIT, &ainst->src);
            break;

        // SCALAR ops:

        case OP_EXP:
        case OP_LOG:
#if defined(NV_INTERNAL_OPCODES)
        case OP_RCC:
#endif // NV_INTERNAL_OPCODES
        case OP_RCP:
        case OP_RSQ:
            sinst = (Scalar_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &sinst->dst);
            lUseScalarSrcReg(&opData, sinst->dst.mask, &sinst->src);
            break;

        // VECTOR ops:

        case OP_LIT:
            vinst = (Vector_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &vinst->dst);
            dstMask = (vinst->dst.mask != DST_NONE_BITS) ? DST_XYW_BITS : DST_NONE_BITS;
            lUseSwizzleSrcReg(&opData, dstMask, &vinst->src);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_IMV:
#endif // NV_INTERNAL_OPCODES
        case OP_MOV:
            vinst = (Vector_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &vinst->dst);
            lUseSwizzleSrcReg(&opData, vinst->dst.mask, &vinst->src);
            break;

        // BIN ops:

        case OP_ADD:
        case OP_MAX:
        case OP_MIN:
        case OP_MUL:
        case OP_SGE:
        case OP_SLT:
            binst = (Bin_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &binst->dst);
            lUseSwizzleSrcReg(&opData, binst->dst.mask, &binst->srcA);
            lUseSwizzleSrcReg(&opData, binst->dst.mask, &binst->srcB);
            break;

        case OP_DP3:
            binst = (Bin_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &binst->dst);
            dstMask = (binst->dst.mask != DST_NONE_BITS) ? DST_XYZ_BITS : DST_NONE_BITS;
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcA);
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcB);
            break;

        case OP_DP4:
            binst = (Bin_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &binst->dst);
            dstMask = (binst->dst.mask != DST_NONE_BITS) ? DST_XYZW_BITS : DST_NONE_BITS;
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcA);
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcB);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_DPH:
            binst = (Bin_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &binst->dst);
            dstMask = (binst->dst.mask != DST_NONE_BITS) ? DST_XYZ_BITS : DST_NONE_BITS;
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcA);
            dstMask = (binst->dst.mask != DST_NONE_BITS) ? DST_XYZW_BITS : DST_NONE_BITS;
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcB);
            break;
#endif // NV_INTERNAL_OPCODES

        case OP_DST:
            binst = (Bin_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &binst->dst);
            dstMask = (binst->dst.mask != DST_NONE_BITS) ? (DST_Y_BIT | DST_Z_BIT) : DST_NONE_BITS;
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcA);
            dstMask = (binst->dst.mask != DST_NONE_BITS) ? (DST_Y_BIT | DST_W_BIT) : DST_NONE_BITS;
            lUseSwizzleSrcReg(&opData, dstMask, &binst->srcB);
            break;

       // TRIops:

        case OP_MAD:
            tinst = (Tri_Instruction *) inst;
            lSetMaskedDstReg(&opData, pc, &tinst->dst);
            lUseSwizzleSrcReg(&opData, tinst->dst.mask, &tinst->srcA);
            lUseSwizzleSrcReg(&opData, tinst->dst.mask, &tinst->srcB);
            lUseSwizzleSrcReg(&opData, tinst->dst.mask, &tinst->srcC);
            break;

        // NOPops:

        case OP_NOP:
            break;

        // ENDops:

        case OP_END:
            break;

        default:
            break;
        }
    }

    // Remember which result registers are written:

    for (pc = 0; pc < __GL_NUMBER_OF_RESULT_REGISTERS; pc++)
        parsed->resultRegsWritten[pc] = opData.resultRegsWritten[pc];

    NV_TRACE_COND(TR_PROGRAM, 30, {
        TPRINTF(("** Output 0: "));
        for (pc = 0; pc < __GL_NUMBER_OF_RESULT_REGISTERS; pc++) {
            if (pc == 8)
                TPRINTF(("\n**        8: "));
            lop = opData.resultRegsWritten[pc];
            TPRINTF((" %c%c%c%c", "-*"[lop & 1], "-*"[(lop >> 1) & 1],
                      "-*"[(lop >> 2) & 1], "-*"[(lop >> 3) & 1]));
        }
        TPRINTF(("\n"));
    });

    // Mark any Live on Entry temp registers:

    parsed->liveOnEntry = 0;
    for (bit = 1, pc = 0; pc < __GL_NUMBER_OF_TEMPORARY_REGISTERS; bit <<= 1, pc++) {
        if (opData.LiveDead[pc][0] | opData.LiveDead[pc][1] |
            opData.LiveDead[pc][2] | opData.LiveDead[pc][3])
            parsed->liveOnEntry |= bit;
    };
    NV_TRACE_COND(TR_PROGRAM, 30, {
        if (parsed->liveOnEntry) {
            TPRINTF(("** Live On Entry Temps: "));
            for (pc = 0; pc < __GL_NUMBER_OF_TEMPORARY_REGISTERS; pc++) {
                TPRINTF((" %c%c%c%c", "-*"[opData.LiveDead[pc][0]], "-*"[opData.LiveDead[pc][1]],
                          "-*"[opData.LiveDead[pc][2]], "-*"[opData.LiveDead[pc][3]]));
            }
            TPRINTF(("\n"));
        } else {
            TPRINTF(("** No Live On Entry Temps.\n"));
        }
    });

    // Remove any dead instructions:

    for (pc = numInstructions - 1; pc >= 0; pc--) {
        if (opData.LiveMask[pc] == 0 || opData.LiveMask[pc] == 0x20) {
            inst = &parsed->firstInstruction[pc];
            inst->end_op.opcode = OP_NOP;
            NV_TRACE(TR_PROGRAM, 30, ("** Dead instruction at %03d **\n", pc));
        }
    }

    NV_TRACE_COND(TR_PROGRAM, 30, {
        TPRINTF(("************ Final program is: ***************\n"));
        lPrintVertexProgram(parsed, numInstructions, opData.LiveMask);
        TPRINTF(("************ Leaving optimizer ***************\n"));
    });
} // vp_Optimize

