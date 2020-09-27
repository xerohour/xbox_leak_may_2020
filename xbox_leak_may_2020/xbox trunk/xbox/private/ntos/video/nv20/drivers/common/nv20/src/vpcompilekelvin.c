/*
 * vpcompilekelvin.c
 *
 * Copyright 2000 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
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

#if defined(IS_OPENGL)
#include "nvassert.h"
#endif

#include "vtxpgmconsts.h"
#include "vtxpgmcomp.h"

#if !defined(FALSE)
#define FALSE 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif

// Duplicate values so they can be seen by the common code:

#define __GLNV20_TEX_SCALE_FIELD_BITS                4 // 4 bits per field

#define __GLNV20_TEX_SCALE_IDENTITY                  0x0 // no scaling needed (MUST be zero value)
#define __GLNV20_TEX_SCALE_R_65535                   0x1 // scale R by 2^16-1 for 16-bit shadow mapping
#define __GLNV20_TEX_SCALE_R_16777215                0x2 // scale R by 2^24-1 for 24-bit shadow mapping
#define __GLNV20_TEX_SCALE_STR_65535                 0x3 // scale STR by 2^16-1 for 16-bit depth replace
#define __GLNV20_TEX_SCALE_STR_16777215              0x4 // scale STR by 2^24-1 for 24-bit depth replace
#define __GLNV20_NUM_TEX_SCALES                      5
#define __GLNV20_TEX_SCALE_MASK                      ((1 << __GLNV20_TEX_SCALE_FIELD_BITS) - 1)

// End duplicate values

#if defined(IS_OPENGL)

// Definitions so we can include nvTrace.h:

#define CDECLCALL
#define GL_POLYGON 9
typedef int GLenum;

#include "nvTrace.h"

#else // !IS_OPENGL

#define NV_FUN_NAME(X, Y, Z)
#define NV_TRACE_FUNC(A)
#define NV_TRACE_PUSH()
#define NV_TRACE_POP()

#ifdef DEBUG
    #define NV_TRACE(X, Y, Z)           if (dbgLevel & NVDBG_LEVEL_VSHADER_INS) { DPF_PLAIN Z; }
    #define NV_TRACE_COND(X, Y, Z)      Z
    #define NV_TRACE_CODE(X)            X
    #define TPRINTF(X)                  if (dbgLevel & NVDBG_LEVEL_VSHADER_INS) { DPF_PLAIN X; }
#else // !DEBUG
    #define NV_TRACE(X, Y, Z)
    #define NV_TRACE_COND(X, Y, Z)
    #define NV_TRACE_CODE(X)
    #define TPRINTF(X)
#endif // !DEBUG

#endif // !IS_OPENGL

typedef enum ILUop_Enum {
    ILU_NOP = 0x0, ILU_MOV = 0x1, ILU_RCP = 0x2, ILU_RCC = 0x3,
    ILU_RSQ = 0x4, ILU_EXP = 0x5, ILU_LOG = 0x6, ILU_LIT = 0x7,
} ILUop;

typedef enum MACop_Enum {
    MAC_NOP = 0x0, MAC_MOV = 0x1, MAC_MUL = 0x2, MAC_ADD = 0x3,
    MAC_MAD = 0x4, MAC_DP3 = 0x5, MAC_DPH = 0x6, MAC_DP4 = 0x7,
    MAC_DST = 0x8, MAC_MIN = 0x9, MAC_MAX = 0xa, MAC_SLT = 0xb,
    MAC_SGE = 0xc, MAC_ARL = 0xd,
} MACop;

typedef enum MUXVals_Enum {
    IN_MUX_CA = 3, IN_MUX_VA = 2, IN_MUX_RR = 1, IN_MUX_M_ILLEGAL = 0,
    OUT_MUX_OM_ALU = 0, OUT_MUX_OM_ILU = 1,
    OC_SELECT_CONTEXT = 0x000, OC_SELECT_OUTPUT = 0x100,
} MUXVals;

typedef enum WRITEMask_Enum {
    OUT_MASK_X = 8, OUT_MASK_Y = 4, OUT_MASK_Z = 2, OUT_MASK_W = 1,
    OUT_MASK_XYZW = 0xf,
} WRITEMask;

typedef struct SwizzleSrcReg20_Rec {
    unsigned char ne;
    unsigned char xs;
    unsigned char ys;
    unsigned char zs;
    unsigned char ws;
    unsigned char rr;
    unsigned char mx;
} SwizzleSrcReg20;

typedef struct vtxpgmInst_Rec {
    ILUop ilu;
    MACop mac;
    unsigned char ca;
    unsigned char va;
    SwizzleSrcReg20 a;
    SwizzleSrcReg20 b;
    SwizzleSrcReg20 c;
    unsigned char rwm;
    unsigned char rw;
    unsigned char swm;
    unsigned char owm;
    unsigned short oc;
    unsigned char om;
    unsigned char cin;
    unsigned char eos;
} vtxpgmInst;

#define ALU_TEMP 0x40
#define ILU_TEMP 0x00

///////////////////////////////////////////////////////////////////////////////

static void lInitInst(vtxpgmInst *pInst)
{
    char *pCode;
    int ii;

    pCode = (char *) pInst;
    for (ii = 0; ii < sizeof(vtxpgmInst); ii++)
        pCode[ii] = 0;

    // Setting unused inputs to IN_MUX_RR can cause unnecessary stalls, so we now use IN_MUX_VA.

    pInst->a.mx = IN_MUX_VA;
    pInst->b.mx = IN_MUX_VA;
    pInst->c.mx = IN_MUX_VA;
    pInst->oc = 0x1ff;
} // lInitInst

///////////////////////////////////////////////////////////////////////////////

typedef struct vtxpgmTLiveDead_Rec {
    unsigned char v[__GL_NUMBER_OF_TEMPORARY_REGISTERS], tmpReg;
} vtxpgmTLiveDead;

/*
** lDeftMaskedDstReg() - Mark registers set as "dead" prior to this point.
*/

static void lDefMaskedDstReg(VtxProgCompileKelvin *env, vtxpgmTLiveDead *pLive,
                             MaskedDstReg *reg, int IsALU)
{
    unsigned char *LiveDead;
    int mask, lreg, ii, jj, bits;

    lreg = reg->reg;
    if (lreg >= TR_0 && lreg <= TR_11) {

        // Mark any registers set as dead:

        LiveDead = &pLive->v[lreg - TR_0];
        mask = reg->mask;
        *LiveDead &= ~mask;
    } else {
        if (lreg >= VR_TEX0 && lreg <= VR_TEX3) {

            // Potential patch needed:

            ii = lreg - VR_TEX0;
            bits = (env->textureScaling >> (ii*__GLNV20_TEX_SCALE_FIELD_BITS)) &
                   __GLNV20_TEX_SCALE_MASK;
            if (bits) {

                // We need to scale it.  Find a dead scratch register:

                for (jj = __GL_NUMBER_OF_TEMPORARY_REGISTERS - 1; jj >= 0; jj--) {
                    if (!pLive->v[jj]) {
                        pLive->tmpReg = jj | 0x80;
                        if (IsALU)
                            pLive->tmpReg |= ALU_TEMP;
                        // Always use 2 instructions for now.  Could be optimized.
                        env->numExtraInstructions += 2;
                        break;
                    }
                }
                if (!pLive->tmpReg) {
                    assert(!"Can't find a dead temp register");
                }
            }
        }
    }
} // lDefMaskedDstReg

/*
** lRefScalarSrcReg() - Mark referenced registers as "live" prior to this point.
*/

static void lRefScalarSrcReg(vtxpgmTLiveDead *pLive, char mask, ScalarSrcReg *reg)
{
    int lreg;

    lreg = reg->reg.reg;
    if (lreg >= TR_0 && lreg <= TR_11) {
        if (mask) // Any output means we need the input
            pLive->v[lreg - TR_0] |= 1 << (reg->com & 3);
    }
} // lRefScalarSrcReg

/*
** lRefSwizzleSrcReg() - Mark referenced registers as "live" prior to this point.
*/

static void lRefSwizzleSrcReg(vtxpgmTLiveDead *pLive, char mask, SwizzleSrcReg *reg)
{
    unsigned char *LiveDead;
    int lreg;

    lreg = reg->reg.reg;
    if (lreg >= TR_0 && lreg <= TR_11) {
        LiveDead = &pLive->v[lreg - TR_0];
        if (mask & DST_X_BIT)
            *LiveDead |= 1 << (reg->com4 & 3);
        if (mask & DST_Y_BIT)
            *LiveDead |= 1 << ((reg->com4 >> 2) & 3);
        if (mask & DST_Z_BIT)
            *LiveDead |= 1 << ((reg->com4 >> 4) & 3);
        if (mask & DST_W_BIT)
            *LiveDead |= 1 << ((reg->com4 >> 6) & 3);
    }
} // lRefSwizzleSrcReg

/*
** lInitTempLiveDead() - Build live/dead vectors for temp registers.  Needed for hidden
**         texture coordinate scaling used by depth replace, etc.
*/

static void lInitTempLiveDead(VtxProgCompileKelvin *env, ParsedProgram *parsed,
                              int numInstructions)
{
    vtxpgmTLiveDead *pLive;
    Instruction *inst;
    Arl_Instruction *ainst;
    Scalar_Instruction *sinst;
    Vector_Instruction *vinst;
    Bin_Instruction *binst;
    Tri_Instruction *tinst;
    int ii, lop, pc;

    env->tLiveDead = (struct vtxpgmTLiveDead_Rec *) NULL;
    env->numExtraInstructions = 0;
    if (!env->textureScaling) {
        return;
    }

    pLive = env->tLiveDead = (vtxpgmTLiveDead *)
                     (env->malloc (env->mallocArg, sizeof(vtxpgmTLiveDead)*(numInstructions + 1)));
    for (pc = numInstructions; pc >= 0; pc--) {
        for (ii = __GL_NUMBER_OF_TEMPORARY_REGISTERS - 1; ii >= 0; ii--)
            pLive->v[ii] = 0;
        pLive->tmpReg = 0;
        pLive++;
    }

    // Walk the code backwards marking live temp registers:

    for (pc = numInstructions - 1; pc >= 0; pc--) {
        pLive = &env->tLiveDead[pc];
        *pLive = env->tLiveDead[pc + 1];
        pLive->tmpReg = 0;
        inst = &parsed->firstInstruction[pc];
        lop = inst->end_op.opcode;
        switch (lop) {

        // ARL op:

        case OP_ARL:
            ainst = (Arl_Instruction *) inst;
            lRefScalarSrcReg(pLive, DST_X_BIT, &ainst->src);
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
            lDefMaskedDstReg(env, pLive, &sinst->dst, 0);
            lRefScalarSrcReg(pLive, sinst->dst.mask, &sinst->src);
            break;

        // VECTOR ops:

        case OP_LIT:
            vinst = (Vector_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &vinst->dst, 0);
            lRefSwizzleSrcReg(pLive, DST_XYW_BITS, &vinst->src);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_IMV:
            vinst = (Vector_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &vinst->dst, 0);
            lRefSwizzleSrcReg(pLive, vinst->dst.mask, &vinst->src);
            break;
#endif // NV_INTERNAL_OPCODES
        case OP_MOV:
            vinst = (Vector_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &vinst->dst, 1);
            lRefSwizzleSrcReg(pLive, vinst->dst.mask, &vinst->src);
            break;

        // BIN ops:

        case OP_ADD:
        case OP_MAX:
        case OP_MIN:
        case OP_MUL:
        case OP_SGE:
        case OP_SLT:
            binst = (Bin_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &binst->dst, 1);
            lRefSwizzleSrcReg(pLive, binst->dst.mask, &binst->srcA);
            lRefSwizzleSrcReg(pLive, binst->dst.mask, &binst->srcB);
            break;

        case OP_DP3:
            binst = (Bin_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &binst->dst, 1);
            lRefSwizzleSrcReg(pLive, DST_XYZ_BITS, &binst->srcA);
            lRefSwizzleSrcReg(pLive, DST_XYZ_BITS, &binst->srcB);
            break;

        case OP_DP4:
            binst = (Bin_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &binst->dst, 1);
            lRefSwizzleSrcReg(pLive, DST_XYZW_BITS, &binst->srcA);
            lRefSwizzleSrcReg(pLive, DST_XYZW_BITS, &binst->srcB);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_DPH:
            binst = (Bin_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &binst->dst, 1);
            lRefSwizzleSrcReg(pLive, DST_XYZ_BITS, &binst->srcA);
            lRefSwizzleSrcReg(pLive, DST_XYZW_BITS, &binst->srcB);
            break;
#endif // NV_INTERNAL_OPCODES

        case OP_DST:
            binst = (Bin_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &binst->dst, 1);
            lRefSwizzleSrcReg(pLive, DST_Y_BIT | DST_Z_BIT, &binst->srcA);
            lRefSwizzleSrcReg(pLive, DST_Y_BIT | DST_W_BIT, &binst->srcB);
            break;

       // TRIops:

        case OP_MAD:
            tinst = (Tri_Instruction *) inst;
            lDefMaskedDstReg(env, pLive, &tinst->dst, 1);
            lRefSwizzleSrcReg(pLive, tinst->dst.mask, &tinst->srcA);
            lRefSwizzleSrcReg(pLive, tinst->dst.mask, &tinst->srcB);
            lRefSwizzleSrcReg(pLive, tinst->dst.mask, &tinst->srcC);
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
} // lInitTempLiveDead

/*
** lDestroyTempLiveDead()
*/

static void lDestroyTempLiveDead(VtxProgCompileKelvin *env)
{
    if (env->tLiveDead) {
        env->free(env->mallocArg, env->tLiveDead);
        env->tLiveDead = NULL;
    }
} // lDestroyTempLiveDead

/*
** lInsertTextureScaling() - Insert the required scaling instructions.
*/

static void lInsertTextureScaling(VtxProgCompileKelvin *env, Instruction *fInsts,
                                  Instruction *tInsts, int fNum, int tNum)
{
#if 000
    const static int swizzles[] = {
        XX | (XX << 2) | (XX << 4) | (XX << 6), // __GLNV20_TEX_SCALE_IDENTITY
        XX | (XX << 2) | (YY << 4) | (XX << 6), // __GLNV20_TEX_SCALE_R_65535
        XX | (XX << 2) | (ZZ << 4) | (XX << 6), // __GLNV20_TEX_SCALE_R_16777215
        YY | (YY << 2) | (YY << 4) | (XX << 6), // __GLNV20_TEX_SCALE_STR_65535
        ZZ | (ZZ << 2) | (ZZ << 4) | (XX << 6), // __GLNV20_TEX_SCALE_STR_16777215
    };
#endif
    const static int scalemasks[] = {
        0,              // __GLNV20_TEX_SCALE_IDENTITY
        DST_Z_BIT,      // __GLNV20_TEX_SCALE_R_65535
        DST_Z_BIT,      // __GLNV20_TEX_SCALE_R_16777215
        DST_XYZ_BITS,   // __GLNV20_TEX_SCALE_STR_65535
        DST_XYZ_BITS,   // __GLNV20_TEX_SCALE_STR_16777215
    };
    const static int constreg[] = {
        0,        // __GLNV20_TEX_SCALE_IDENTITY
        PPR_58,   // __GLNV20_TEX_SCALE_R_65535
        PPR_59,   // __GLNV20_TEX_SCALE_R_16777215
        PPR_58,   // __GLNV20_TEX_SCALE_STR_65535
        PPR_59,   // __GLNV20_TEX_SCALE_STR_16777215
    };
    Instruction *frinst, *toinst;
    int fpc, tpc, tregno, texno, scaleinx, dstmask, count = 0;
    MaskedDstReg *dst;
    Scalar_Instruction *sinst;
    Vector_Instruction *vinst;
    Bin_Instruction *binst;
    Tri_Instruction *tinst;

    for (fpc = tpc = 0; fpc < fNum; fpc++, tpc++) {
        frinst = &fInsts[fpc];
        toinst = &tInsts[tpc];
        *toinst = *frinst;
        if (env->tLiveDead[fpc].tmpReg) {
            switch (frinst->bin_op.opclass) {
            case CLASS_VECTOR:
                vinst = (Vector_Instruction *) toinst;
                dst = &vinst->dst;
                break;
            case CLASS_SCALAR:
                sinst = (Scalar_Instruction *) toinst;
                dst = &sinst->dst;
                break;
            case CLASS_BIN:
                binst = (Bin_Instruction *) toinst;
                dst = &binst->dst;
                break;
            case CLASS_TRI:
                tinst = (Tri_Instruction *) toinst;
                dst = &tinst->dst;
                break;
            case CLASS_END:
            case CLASS_ARL:
            case CLASS_NOP:
            default:
                assert(0);
                continue;
            }
            texno = dst->reg - VR_TEX0;
            dstmask = dst->mask;
            tregno = env->tLiveDead[fpc].tmpReg & 0xf;
            scaleinx = (env->textureScaling >> (texno*__GLNV20_TEX_SCALE_FIELD_BITS)) &
                         __GLNV20_TEX_SCALE_MASK;
            // Patch old instruction to write to scratch register:

            dst->reg = (Register_t) (TR_0 + tregno);

#if 000
            // Build new instruction to scale temp value into final destination:
            // MUL o[TEXi].mask, Rtmp, c[23];  // c[23] = 1.0, 65535.0, 16777215.0

            count++;
            binst = (Bin_Instruction *) &tInsts[++tpc];
            binst->opclass = CLASS_BIN;
            binst->opcode = OP_MUL;
            binst->srcA.reg.reg = TR_0 + tregno;
            binst->srcA.reg.AddrReg = 0;
            binst->srcA.reg.addrRegOffset = 0;
            binst->srcA.com4 = XYZW;
            binst->srcA.Signed = 0;

            binst->srcB.reg.reg = PPR_23;
            binst->srcB.reg.AddrReg = 0;
            binst->srcB.reg.addrRegOffset = 0;
            binst->srcB.com4 = swizzles[scaleinx];
            binst->srcB.Signed = 0;

            binst->dst.reg = VR_TEX0 + texno;
            binst->dst.mask = dstmask;
#endif
            
            // Build two new instruction to scale temp value into final destination:
            // MUL o[TEXi].mask, Rtmp, c[58/59].w; // c[58].w = 65535, c[59].w = 16777215

            count++;
            binst = (Bin_Instruction *) &tInsts[++tpc];
            binst->opclass = CLASS_BIN;
            binst->opcode = OP_MUL;
            binst->srcA.reg.reg = (Register_t) (TR_0 + tregno);
            binst->srcA.reg.AddrReg = 0;
            binst->srcA.reg.addrRegOffset = 0;
            binst->srcA.com4 = XYZW;
            binst->srcA.Signed = 0;

            binst->srcB.reg.reg = (Register_t) constreg[scaleinx];
            binst->srcB.reg.AddrReg = 0;
            binst->srcB.reg.addrRegOffset = 0;
            binst->srcB.com4 = (WW | WW << 2 | WW << 4 | WW << 6);
            binst->srcB.Signed = 0;

            binst->dst.reg = (Register_t) (VR_TEX0 + texno);
            binst->dst.mask = dstmask & scalemasks[scaleinx];

            // MOV o[TEXi].mask, Rtmp;

            count++;
            vinst = (Vector_Instruction *) &tInsts[++tpc];
            vinst->opclass = CLASS_VECTOR;
            vinst->opcode = OP_MOV;
            vinst->src.reg.reg = (Register_t) (TR_0 + tregno);
            vinst->src.reg.AddrReg = 0;
            vinst->src.reg.addrRegOffset = 0;
            vinst->src.com4 = XYZW;
            vinst->src.Signed = 0;

            vinst->dst.reg = (Register_t) (VR_TEX0 + texno);
            vinst->dst.mask = dstmask & ~scalemasks[scaleinx];

            NV_TRACE(TR_PROGRAM, 50, ("Inserting texture scaling instruction(s) at: %03x\n", fpc));
        }
    }
    assert(count == env->numExtraInstructions);
} // lInsertTextureScaling

///////////////////////////////////////////////////////////////////////////////

/*
** lGenSrcReg()
*/

static void lGenSrcReg(VtxProgCompileKelvin *env, vtxpgmInst *pInst, SwizzleSrcReg20 *r, SrcReg *reg)
{
    int lreg = reg->reg;

    switch (lreg) {
    case VA_0:  case VA_1:  case VA_2:  case VA_3:
    case VA_4:  case VA_5:  case VA_6:  case VA_7:
    case VA_8:  case VA_9:  case VA_10: case VA_11:
    case VA_12: case VA_13: case VA_14: case VA_15:
        r->mx = IN_MUX_VA;
        pInst->va = (unsigned char) env->inputRegMap[lreg - VA_0];
        NV_TRACE(TR_PROGRAM, 50, ("v[%i]", pInst->va));
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
        if (reg->AddrReg) {
            r->mx = IN_MUX_CA;
            pInst->ca = (unsigned char) reg->addrRegOffset;
            if (env->userMode)
                pInst->ca += 96;
            pInst->cin = 1;
            NV_TRACE(TR_PROGRAM, 50, ("c[A0.x+%i]", pInst->ca));
        } else {
            r->mx = IN_MUX_CA;
            pInst->ca = lreg - PR_0;
            if (env->userMode)
                pInst->ca += 96;
            NV_TRACE(TR_PROGRAM, 50, ("c[%i]", pInst->ca));
        }
        break;
    case PPR_23:
    case PPR_58:
    case PPR_59:
    case PPR_62:
        // Special hook into priveledged (lower 96) registers:
        if (reg->AddrReg) {
            r->mx = IN_MUX_CA;
            pInst->ca = (unsigned char) reg->addrRegOffset;
            pInst->cin = 1;
            NV_TRACE(TR_PROGRAM, 50, ("ppr[A0.x+%i]", pInst->ca));
        } else {
            r->mx = IN_MUX_CA;
            pInst->ca = lreg - PPR_0;
            NV_TRACE(TR_PROGRAM, 50, ("ppr[%i]", pInst->ca));
        }
        break;
    case TR_0:  case TR_1:  case TR_2:  case TR_3:
    case TR_4:  case TR_5:  case TR_6:  case TR_7:
    case TR_8:  case TR_9:  case TR_10: case TR_11:
    case TR_12: case TR_13: case TR_14: case TR_15:
        r->mx = IN_MUX_RR;
        r->rr = lreg - TR_0;
        NV_TRACE(TR_PROGRAM, 50, ("R%i", r->rr));
        break;
    default:
        assert(!"lGenSrcReg() - Bad register number");
        NV_TRACE(TR_PROGRAM, 50, ("BAD-REG!"));
        break;
    }
} // lGenSrcReg

////////////////////////// NV20 Level Code Generation /////////////////////////

/*
** lGenScalarSrcReg()
*/
char *TEXT_XYZW = "xyzw";

static void lGenScalarSrcReg(VtxProgCompileKelvin *env, vtxpgmInst *pInst, SwizzleSrcReg20 *r, ScalarSrcReg *reg)
{
    // ???? Is there anything special about ScalarSrcReg?  ???? //
    // ???? I swizzled the input to all inputs;  I don't know if that's right/wrong/doesn't-matter ???? //
    NV_TRACE(TR_PROGRAM, 50, (", %s", reg->Signed ? "-" : ""));
    lGenSrcReg(env, pInst, r, &reg->reg);
    r->ne = reg->Signed;
    r->xs = reg->com;
    r->ys = reg->com;
    r->zs = reg->com;
    r->ws = reg->com;
    NV_TRACE(TR_PROGRAM, 50, (".%c", TEXT_XYZW[reg->com]));
} // lGenScalarSrcReg

/*
** lGenSwizzleSrcReg()
*/

static void lGenSwizzleSrcReg(VtxProgCompileKelvin *env, vtxpgmInst *pInst, SwizzleSrcReg20 *r, SwizzleSrcReg *reg)
{
    NV_TRACE(TR_PROGRAM, 50, (", %s", reg->Signed ? "-" : ""));
    lGenSrcReg(env, pInst, r, &reg->reg);
    r->ne = reg->Signed;
    r->xs = reg->com4 & 0x03;
    r->ys = (reg->com4 & 0x0c) >> 2;
    r->zs = (reg->com4 & 0x30) >> 4;
    r->ws = (reg->com4 & 0xc0) >> 6;
    NV_TRACE_COND(TR_PROGRAM, 50,
        if (reg->com4 != XYZW) {
            NV_TRACE(TR_PROGRAM, 50, (".%c", TEXT_XYZW[r->xs]));
            NV_TRACE(TR_PROGRAM, 50, ("%c", TEXT_XYZW[r->ys]));
            NV_TRACE(TR_PROGRAM, 50, ("%c", TEXT_XYZW[r->zs]));
            NV_TRACE(TR_PROGRAM, 50, ("%c", TEXT_XYZW[r->ws]));
        }
    )
} // lGenSwizzleSrcReg

/*
**
*/

static unsigned char lMakeWriteMask(int fval)
{
    unsigned char result = 0;

    if (fval & DST_X_BIT)
        result |= OUT_MASK_X;
    if (fval & DST_Y_BIT)
        result |= OUT_MASK_Y;
    if (fval & DST_Z_BIT)
        result |= OUT_MASK_Z;
    if (fval & DST_W_BIT)
        result |= OUT_MASK_W;
    return result;
} // lMakeWriteMask

/*
** lGenMaskedDstILU() - Store the masked output of the ILU in "reg".
*/

static void lGenMaskedDstILU(VtxProgCompileKelvin *env, vtxpgmInst *pInst, MaskedDstReg *reg)
{
    NV_TRACE_CODE(int mask);

    if (reg->reg >= TR_0 && reg->reg <= TR_15) {
        // Target is a scratch register.  Use swm:
        pInst->swm = lMakeWriteMask(reg->mask);
        pInst->rw = reg->reg - TR_0;
        NV_TRACE(TR_PROGRAM, 50, ("R%i", pInst->rw));
        NV_TRACE_CODE(mask = pInst->swm);
    } else {
        // Target is a context or output register.  Use owm:
        pInst->om = OUT_MUX_OM_ILU;
        pInst->owm = lMakeWriteMask(reg->mask);
        if (reg->reg >= PR_0 && reg->reg <= PR_95) {
            pInst->oc = (reg->reg - PR_0) | OC_SELECT_CONTEXT;
            if (env->userMode)
                pInst->oc += 96;
            NV_TRACE(TR_PROGRAM, 50, ("c[%i]", pInst->oc & 0x0ff));
        } else {
            assert(reg->reg >= VR_0 && reg->reg <= VR_15);
            pInst->oc = env->outputRegMap[(reg->reg - VR_0)] | OC_SELECT_OUTPUT;
            NV_TRACE(TR_PROGRAM, 50, ("o[%i]", pInst->oc & 0x0ff));
        }
        NV_TRACE_CODE(mask = pInst->owm);
    }
    NV_TRACE_COND(TR_PROGRAM, 50,
        if (mask != OUT_MASK_XYZW) {
            TPRINTF(("."));
            if (mask & OUT_MASK_X)
                TPRINTF(("x"));
            if (mask & OUT_MASK_Y)
                TPRINTF(("y"));
            if (mask & OUT_MASK_Z)
                TPRINTF(("z"));
            if (mask & OUT_MASK_W)
                TPRINTF(("w"));
        }
    );
} // lGenMaskedDstILU

/*
** lGenMaskedDstALU() - Store the masked output of the ALU in "reg".
*/

static void lGenMaskedDstALU(VtxProgCompileKelvin *env, vtxpgmInst *pInst, MaskedDstReg *reg)
{
    NV_TRACE_CODE(int mask);

    if (reg->reg >= TR_0 && reg->reg <= TR_15) {
        // Target is a scratch register.  Use rwm:
        pInst->rwm = lMakeWriteMask(reg->mask);
        pInst->rw = reg->reg - TR_0;
        NV_TRACE(TR_PROGRAM, 50, ("R%i", pInst->rw));
        NV_TRACE_CODE(mask = pInst->rwm);
    } else {
        // Target is a context or output register.  Use owm:
        pInst->om = OUT_MUX_OM_ALU;
        pInst->owm = lMakeWriteMask(reg->mask);
        if (reg->reg >= PR_0 && reg->reg <= PR_95) {
            pInst->oc = (reg->reg - PR_0) | OC_SELECT_CONTEXT;
            if (env->userMode)
                pInst->oc += 96;
            NV_TRACE(TR_PROGRAM, 50, ("c[%i]", pInst->oc & 0x0ff));
        } else {
            assert(reg->reg >= VR_0 && reg->reg <= VR_15);
            pInst->oc = env->outputRegMap[(reg->reg - VR_0)] | OC_SELECT_OUTPUT;
            NV_TRACE(TR_PROGRAM, 50, ("o[%i]", pInst->oc & 0x0ff));
        }
        NV_TRACE_CODE(mask = pInst->owm);
    }
    NV_TRACE_COND(TR_PROGRAM, 50,
        if (mask != OUT_MASK_XYZW) {
            TPRINTF(("."));
            if (mask & OUT_MASK_X)
                TPRINTF(("x"));
            if (mask & OUT_MASK_Y)
                TPRINTF(("y"));
            if (mask & OUT_MASK_Z)
                TPRINTF(("z"));
            if (mask & OUT_MASK_W)
                TPRINTF(("w"));
        }
    );
} // lGenMaskedDstALU

/*
** lAppendViewportXform(pCode, &pc);
*/

static void lAppendViewportXform(VtxProgCompileKelvin *env, vtxpgmInst *pCode, int *ppc)
{
    vtxpgmInst *curInst;
    int pc = *ppc;
    SwizzleSrcReg srcA, srcB, srcC;
    ScalarSrcReg src;
    MaskedDstReg dst;

    NV_TRACE(TR_PROGRAM, 50, ("--- Viewport Xform ---\n"));

    env->userMode = 0;

    curInst = &pCode[pc - 1];
    curInst->eos = 0;

    if (env->passthroughMode) {
        // If this vertex program is the passthrough vertex program, then
        // only apply the zscale and viewport offset.  Do not apply W-divide
        // since the vertices will already be in screen coordinates.

        // MUL o[HPOS].xyz, R12, c[58];
        srcA.reg.reg = TR_12;
        srcA.reg.AddrReg = FALSE;
        srcA.reg.addrRegOffset = 0;
        srcA.com4 = XYZW;
        srcA.Signed = 0;

        srcB.reg.reg = PPR_58;
        srcB.reg.AddrReg = FALSE;
        srcB.reg.addrRegOffset = 0;
        srcB.com4 = XYZW;
        srcB.Signed = 0;

        dst.reg = VR_HPOS;
        dst.mask = DST_XYZ_BITS;

        curInst = &pCode[pc];
        NV_TRACE(TR_PROGRAM, 50, ("%03x:  MUL ", pc));
        curInst->mac = MAC_MUL;
        lGenMaskedDstALU(env, curInst, &dst);
        lGenSwizzleSrcReg(env, curInst, &curInst->a, &srcA);
        lGenSwizzleSrcReg(env, curInst, &curInst->b, &srcB);
        NV_TRACE(TR_PROGRAM, 50, ("\n"));
        pc++;

        // ADD o[HPOS].xyz, R12, c[59]
        srcA.reg.reg = TR_12;
        srcA.reg.AddrReg = FALSE;
        srcA.reg.addrRegOffset = 0;
        srcA.com4 = XYZW;
        srcA.Signed = 0;

        srcC.reg.reg = PPR_59;
        srcC.reg.AddrReg = FALSE;
        srcC.reg.addrRegOffset = 0;
        srcC.com4 = XYZW;
        srcC.Signed = 0;

        dst.reg = VR_HPOS;
        dst.mask = DST_XYZ_BITS;

        curInst = &pCode[pc];
        NV_TRACE(TR_PROGRAM, 50, ("%03x:  ADD ", pc));
        curInst->mac = MAC_ADD;
        lGenMaskedDstALU(env, curInst, &dst);
        lGenSwizzleSrcReg(env, curInst, &curInst->a, &srcA);
        lGenSwizzleSrcReg(env, curInst, &curInst->c, &srcC);
        curInst->eos = 1;
        NV_TRACE(TR_PROGRAM, 50, ("\n"));
        pc++;
/*
        // MAD o[HPOS].xyz, R12, c[58], c[59];

        srcA.reg.reg = TR_12;
        srcA.reg.AddrReg = FALSE;
        srcA.reg.addrRegOffset = 0;
        srcA.com4 = XYZW;
        srcA.Signed = 0;

        srcB.reg.reg = PPR_58;
        srcB.reg.AddrReg = FALSE;
        srcB.reg.addrRegOffset = 0;
        srcB.com4 = XYZW;
        srcB.Signed = 0;

        srcC.reg.reg = PPR_59;
        srcC.reg.AddrReg = FALSE;
        srcC.reg.addrRegOffset = 0;
        srcC.com4 = XYZW;
        srcC.Signed = 0;

        dst.reg = VR_HPOS;
        dst.mask = DST_XYZ_BITS;

        curInst = &pCode[pc];
        NV_TRACE(TR_PROGRAM, 50, ("%03x:  MAD ", pc));
        curInst->mac = MAC_MAD;
        lGenMaskedDstALU(env, curInst, &dst);
        lGenSwizzleSrcReg(env, curInst, &curInst->a, &srcA);
        lGenSwizzleSrcReg(env, curInst, &curInst->b, &srcB);
        lGenSwizzleSrcReg(env, curInst, &curInst->c, &srcC);
        curInst->eos = 1;
        NV_TRACE(TR_PROGRAM, 50, ("\n"));
        pc++;
        */
    } else {
    // MUL R11.xyz, R12, c[58];

    srcA.reg.reg = TR_12;
    srcA.reg.AddrReg = FALSE;
    srcA.reg.addrRegOffset = 0;
    srcA.com4 = XYZW;
    srcA.Signed = 0;

    srcB.reg.reg = PPR_58;
    srcB.reg.AddrReg = FALSE;
    srcB.reg.addrRegOffset = 0;
    srcB.com4 = XYZW;
    srcB.Signed = 0;

    dst.reg = TR_11;
    dst.mask = DST_XYZ_BITS;

    curInst = &pCode[pc];
    NV_TRACE(TR_PROGRAM, 50, ("%03x:  MUL ", pc));
    curInst->mac = MAC_MUL;
    lGenMaskedDstALU(env, curInst, &dst);
    lGenSwizzleSrcReg(env, curInst, &curInst->a, &srcA);
    lGenSwizzleSrcReg(env, curInst, &curInst->b, &srcB);
    NV_TRACE(TR_PROGRAM, 50, ("\n"));
    pc++;

    // RCC R1, R12.w;  <<---<< Can be combined with previous instruction

    src.reg.reg = TR_12;
    src.reg.AddrReg = FALSE;
    src.reg.addrRegOffset = 0;
    src.com = WW;
    src.Signed = FALSE;

    dst.reg = TR_1;
    dst.mask = DST_XYZW_BITS;

    curInst = &pCode[pc];
    NV_TRACE(TR_PROGRAM, 50, ("%03x:  RCC ", pc));
    curInst->ilu = ILU_RCC;
    lGenMaskedDstILU(env, curInst, &dst);
    lGenScalarSrcReg(env, curInst, &curInst->c, &src);
    NV_TRACE(TR_PROGRAM, 50, ("\n"));
    pc++;

    // MAD o[HPOS].xyz, R1.w, R11, c[59];

    srcA.reg.reg = TR_1;
    srcA.reg.AddrReg = FALSE;
    srcA.reg.addrRegOffset = 0;
    srcA.com4 = WW | (WW << 2) | (WW << 4) | (WW << 6);
    srcA.Signed = 0;

    srcB.reg.reg = TR_11;
    srcB.reg.AddrReg = FALSE;
    srcB.reg.addrRegOffset = 0;
    srcB.com4 = XYZW;
    srcB.Signed = 0;

    srcC.reg.reg = PPR_59;
    srcC.reg.AddrReg = FALSE;
    srcC.reg.addrRegOffset = 0;
    srcC.com4 = XYZW;
    srcC.Signed = 0;

    dst.reg = VR_HPOS;
    dst.mask = DST_XYZ_BITS;

    curInst = &pCode[pc];
    NV_TRACE(TR_PROGRAM, 50, ("%03x:  MAD ", pc));
    curInst->mac = MAC_MAD;
    lGenMaskedDstALU(env, curInst, &dst);
    lGenSwizzleSrcReg(env, curInst, &curInst->a, &srcA);
    lGenSwizzleSrcReg(env, curInst, &curInst->b, &srcB);
    lGenSwizzleSrcReg(env, curInst, &curInst->c, &srcC);
    curInst->eos = 1;
    NV_TRACE(TR_PROGRAM, 50, ("\n"));
    pc++;
    }
    *ppc = pc;
} // lApendViewportXform

/*
** lPackCode() - Pack instruction fields into NV20 microcode format.
*/

static void lPackCode(vtxpgmInstPacked *theCode, vtxpgmInst *pCode, int pc)
{
    int ii;

    for (ii = 0; ii < pc; ii++) {
        theCode->x = 0;
        theCode->y = (pCode->ilu  << 25) |
                     (pCode->mac  << 21) |
                     (pCode->ca   << 13) |
                     (pCode->va   <<  9) |
                     (pCode->a.ne <<  8) |
                     (pCode->a.xs <<  6) |
                     (pCode->a.ys <<  4) |
                     (pCode->a.zs <<  2) |
                     (pCode->a.ws <<  0);
        theCode->z = (pCode->a.rr << 28) |
                     (pCode->a.mx << 26) |
                     (pCode->b.ne << 25) |
                     (pCode->b.xs << 23) |
                     (pCode->b.ys << 21) |
                     (pCode->b.zs << 19) |
                     (pCode->b.ws << 17) |
                     (pCode->b.rr << 13) |
                     (pCode->b.mx << 11) |
                     (pCode->c.ne << 10) |
                     (pCode->c.xs <<  8) |
                     (pCode->c.ys <<  6) |
                     (pCode->c.zs <<  4) |
                     (pCode->c.ws <<  2) |
                     (pCode->c.rr >>  2);
        theCode->w = (pCode->c.rr << 30) |
                     (pCode->c.mx << 28) |
                     (pCode->rwm  << 24) |
                     (pCode->rw   << 20) |
                     (pCode->swm  << 16) |
                     (pCode->owm  << 12) |
                     (pCode->oc   <<  3) |
                     (pCode->om   <<  2) |
                     (pCode->cin  <<  1) |
                     (pCode->eos  <<  0);
        NV_TRACE_COND(TR_PROGRAM, 50,
            if (!(ii & 0x1f)) {
                TPRINTF(("\n"));
                TPRINTF(("                                "));
                TPRINTF(("-I M       N              N              N              R   S O       C E\n"));
                TPRINTF(("                                "));
                TPRINTF(("-L A -C V  E X Y Z W R M  E X Y Z W R M  E X Y Z W R M  W R W W -O- O I O\n"));
                TPRINTF(("                                "));
                TPRINTF(("-U C -A A  G S S S S R X  G S S S S R X  G S S S S R X  M W M M -C- M N S\n"));
            }
        );
        NV_TRACE(TR_PROGRAM, 50, ("%02x: %08x %08x %08x: ",
                                  ii, theCode->y, theCode->z, theCode->w));
        NV_TRACE(TR_PROGRAM, 50, ("%02x %01x %02x %01x",
                                  (theCode->y >> 25) & 0x7f,
                                  (theCode->y >> 21) & 0xf,
                                  (theCode->y >> 13) & 0xff,
                                  (theCode->y >> 9) & 0xf
                                  ));
        NV_TRACE(TR_PROGRAM, 50, ("  %01x %01x %01x %01x %01x %01x %01x",
                                  (theCode->y >> 8) & 0x1,
                                  (theCode->y >> 6) & 0x3,
                                  (theCode->y >> 4) & 0x3,
                                  (theCode->y >> 2) & 0x3,
                                  (theCode->y >> 0) & 0x3,
                                  (theCode->z >> 28) & 0xf,
                                  (theCode->z >> 26) & 0x3
                                  ));
        NV_TRACE(TR_PROGRAM, 50, ("  %01x %01x %01x %01x %01x %01x %01x",
                                  (theCode->z >> 25) & 0x1,
                                  (theCode->z >> 23) & 0x3,
                                  (theCode->z >> 21) & 0x3,
                                  (theCode->z >> 19) & 0x3,
                                  (theCode->z >> 17) & 0x3,
                                  (theCode->z >> 13) & 0xf,
                                  (theCode->z >> 11) & 0x3
                                  ));
        NV_TRACE(TR_PROGRAM, 50, ("  %01x %01x %01x %01x %01x %01x %01x",
                                  (theCode->z >> 10) & 0x1,
                                  (theCode->z >> 8) & 0x3,
                                  (theCode->z >> 6) & 0x3,
                                  (theCode->z >> 4) & 0x3,
                                  (theCode->z >> 2) & 0x3,
                                  ((theCode->z << 2) | ((theCode->w >> 30) & 0x3)) & 0xf,
                                  (theCode->w >> 28) & 0x3
                                  ));
        NV_TRACE(TR_PROGRAM, 50, ("  %01x %01x %01x %01x %03x %01x %01x %01x",
                                  (theCode->w >> 24) & 0xf,
                                  (theCode->w >> 20) & 0xf,
                                  (theCode->w >> 16) & 0xf,
                                  (theCode->w >> 12) & 0xf,
                                  (theCode->w >> 3) & 0x1ff,
                                  (theCode->w >> 2) & 0x1,
                                  (theCode->w >> 1) & 0x1,
                                  (theCode->w >> 0) & 0x1
                                  ));
        NV_TRACE(TR_PROGRAM, 50, ("\n"));
        pCode++;
        theCode++;
    }
    NV_TRACE(TR_PROGRAM, 50, ("\n"));
} // lPackCode

/*
** vp_CompileKelvin() - Compile a vertex program into NV20 microcode.
*/

int vp_CompileKelvin(VtxProgCompileKelvin *env, ParsedProgram *fParsed,
                     int numInstructions, VertexProgramOutput *outArgs)
{
    NV_FUN_NAME(__glNV20MakeProgramResident, TR_PROGRAM, 20)
    vtxpgmInst *pCode, *curInst;
    vtxpgmInstPacked *theCode;
    Instruction *inst, *newinst;
    Arl_Instruction *ainst;
    Scalar_Instruction *sinst;
    Vector_Instruction *vinst;
    Bin_Instruction *binst;
    Tri_Instruction *tinst;
    int lop, lsize;
    int lNumInstructions;
    int pc, skipping, offset;

    NV_TRACE_FUNC(("ppi", env, fParsed, numInstructions));
    NV_TRACE_PUSH();

#if !defined(IS_OPENGL)
    env->textureScaling = __GLNV20_TEX_SCALE_IDENTITY;
    env->passthroughMode = 0;
#endif

    lInitTempLiveDead(env, fParsed, numInstructions);
    lNumInstructions = numInstructions + env->numExtraInstructions;
    if (env->numExtraInstructions > 0) {
        if (lNumInstructions > __GL_MAX_NUMBER_OF_INSTRUCTIONS) {
            // Too big!  Can't create program.
            assert(!"Program too big.  Can't add texture scaling instructions.");
            return FALSE;
        }
        newinst = (Instruction *) env->malloc(env->mallocArg,
                                                sizeof(Instruction)*(lNumInstructions));
        lInsertTextureScaling(env, fParsed->firstInstruction, newinst, numInstructions, lNumInstructions);
    } else {
        newinst = NULL;
    }

    pCode = (vtxpgmInst *) env->malloc(env->mallocArg,
                sizeof(vtxpgmInst)*(__GL_MAX_NUMBER_OF_INSTRUCTIONS + 3));
    if (!pCode) {
        NV_TRACE_POP();
        return FALSE;
    }

    // Initialize instructions:

    for (pc = 0; pc < (__GL_MAX_NUMBER_OF_INSTRUCTIONS + 3); pc++)
        lInitInst(&pCode[pc]);

    env->userMode = 1;

    offset = 0; skipping = 0;
    NV_TRACE(TR_PROGRAM, 50, ("\n"));
    inst = newinst ? newinst : fParsed->firstInstruction;
    for (pc = 0; pc < lNumInstructions; inst++, pc++) {

#if defined(ENABLE_TRACE_CODE)
        if (env->tLiveDead) {
            int ii;
            
            if (skipping) {
                NV_TRACE(TR_PROGRAM, 50, ("               "));
                skipping--;
                offset++;
            } else {
                for (ii = 0; ii < __GL_NUMBER_OF_TEMPORARY_REGISTERS; ii++) {
                    NV_TRACE(TR_PROGRAM, 50, ("%1x", env->tLiveDead[pc - offset].v[ii]));
                }
                if (env->tLiveDead[pc - offset].tmpReg) {
                    if (env->tLiveDead[pc - offset].tmpReg & ALU_TEMP) {
                        NV_TRACE(TR_PROGRAM, 50, ("A%1x ", env->tLiveDead[pc - offset].tmpReg & 0xf));
                    } else {
                        NV_TRACE(TR_PROGRAM, 50, ("I%1x ", env->tLiveDead[pc - offset].tmpReg & 0xf));
                    }
                    skipping = 2;
                } else {
                    NV_TRACE(TR_PROGRAM, 50, ("   "));
                }
            }
        }
#endif // defined(ENABLE_TRACE_CODE)

        curInst = &pCode[pc];
        lop = inst->end_op.opcode;
        switch (lop) {

        // ARL op:

        case OP_ARL:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  ARL ", pc));
            ainst = (Arl_Instruction *) inst;
            curInst->mac = MAC_ARL;
            NV_TRACE(TR_PROGRAM, 50, ("A0.x"));
            lGenScalarSrcReg(env, curInst, &curInst->a, &ainst->src);
            break;

        // SCALAR ops:

        case OP_EXP:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  EXP ", pc));
            sinst = (Scalar_Instruction *) inst;
            curInst->ilu = ILU_EXP;
            lGenMaskedDstILU(env, curInst, &sinst->dst);
            lGenScalarSrcReg(env, curInst, &curInst->c, &sinst->src);
            break;

        case OP_LOG:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  LOG ", pc));
            sinst = (Scalar_Instruction *) inst;
            curInst->ilu = ILU_LOG;
            lGenMaskedDstILU(env, curInst, &sinst->dst);
            lGenScalarSrcReg(env, curInst, &curInst->c, &sinst->src);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_RCC:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  RCC ", pc));
            sinst = (Scalar_Instruction *) inst;
            curInst->ilu = ILU_RCC;
            lGenMaskedDstILU(env, curInst, &sinst->dst);
            lGenScalarSrcReg(env, curInst, &curInst->c, &sinst->src);
            break;
#endif // NV_INTERNAL_OPCODES

        case OP_RCP:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  RCP ", pc));
            sinst = (Scalar_Instruction *) inst;
            curInst->ilu = ILU_RCP;
            lGenMaskedDstILU(env, curInst, &sinst->dst);
            lGenScalarSrcReg(env, curInst, &curInst->c, &sinst->src);
            break;

        case OP_RSQ:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  RSQ ", pc));
            sinst = (Scalar_Instruction *) inst;
            curInst->ilu = ILU_RSQ;
            lGenMaskedDstILU(env, curInst, &sinst->dst);
            lGenScalarSrcReg(env, curInst, &curInst->c, &sinst->src);
            break;

        // VECTOR ops:

        case OP_LIT:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  LIT ", pc));
            vinst = (Vector_Instruction *) inst;
            curInst->ilu = ILU_LIT;
            lGenMaskedDstILU(env, curInst, &vinst->dst);
            lGenSwizzleSrcReg/*lGenSwizzleSrcILU*/(env, curInst, &curInst->c, &vinst->src);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_IMV:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  IMV ", pc));
            vinst = (Vector_Instruction *) inst;
            curInst->ilu = ILU_MOV; // ???? Is there a separate ILU_IMV instruction? ???
            lGenMaskedDstILU(env, curInst, &vinst->dst);
            lGenSwizzleSrcReg/*lGenSwizzleSrcILU*/(env, curInst, &curInst->c, &vinst->src);
            break;
#endif // NV_INTERNAL_OPCODES

        case OP_MOV:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  MOV ", pc));
            vinst = (Vector_Instruction *) inst;
            curInst->ilu = ILU_MOV;
            lGenMaskedDstILU(env, curInst, &vinst->dst);
            lGenSwizzleSrcReg/*lGenSwizzleSrcILU*/(env, curInst, &curInst->c, &vinst->src);
            break;

        // BIN ops:

        case OP_ADD:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  ADD ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_ADD;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->c, &binst->srcB);
            break;

        case OP_DP3:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  DP3 ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_DP3;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

        case OP_DP4:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  DP4 ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_DP4;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

#if defined(NV_INTERNAL_OPCODES)
        case OP_DPH:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  DPH ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_DPH;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;
#endif // NV_INTERNAL_OPCODES

        case OP_DST:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  DST ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_DST;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

        case OP_MAX:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  MAX ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_MAX;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

        case OP_MIN:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  MIN ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_MIN;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

        case OP_MUL:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  MUL ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_MUL;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

        case OP_SGE:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  SGE ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_SGE;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

        case OP_SLT:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  SLT ", pc));
            binst = (Bin_Instruction *) inst;
            curInst->mac = MAC_SLT;
            lGenMaskedDstALU(env, curInst, &binst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &binst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &binst->srcB);
            break;

        // TRIops:

        case OP_MAD:
            NV_TRACE(TR_PROGRAM, 50, ("%03x:  MAD ", pc));
            tinst = (Tri_Instruction *) inst;
            curInst->mac = MAC_MAD;
            lGenMaskedDstALU(env, curInst, &tinst->dst);
            lGenSwizzleSrcReg(env, curInst, &curInst->a, &tinst->srcA);
            lGenSwizzleSrcReg(env, curInst, &curInst->b, &tinst->srcB);
            lGenSwizzleSrcReg(env, curInst, &curInst->c, &tinst->srcC);
            break;

        // ENDops:

        case OP_END:
            curInst = &pCode[pc - 1];
            curInst->eos = 1;
            break;

        default:
            break;
        }
        NV_TRACE(TR_PROGRAM, 50, ("\n"));
    }

    --pc;   // Don't need the last "end" instruction.

    // Append the viewport xfrom code:

    if (!fParsed->IsStateProgram)
        lAppendViewportXform(env, pCode, &pc);

    lsize = pc*sizeof(vtxpgmInstPacked);
    theCode = (vtxpgmInstPacked *) env->malloc(env->mallocArg, lsize);
    if (theCode) {
        lPackCode(theCode, pCode, pc);
        outArgs->residentProgram = theCode;
        outArgs->residentSize = lsize;
        outArgs->residentNumInstructions = pc;
    } else {
        outArgs->residentProgram = NULL;
        outArgs->residentSize = 0;
        outArgs->residentNumInstructions = 0;
    }

    env->free(env->mallocArg, pCode);
    if (newinst)
        env->free(env->mallocArg, newinst);

    NV_TRACE_POP();
    return TRUE;
} // vp_CompileKelvin


