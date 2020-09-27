///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
//
//  File:       api.cpp
//  Content:    The public api functions for the shadeasm library
//
///////////////////////////////////////////////////////////////////////////

#include "pchshadeasm.h"

namespace XGRAPHICS
{

void PrintPixelShaderDef(D3DPIXELSHADERDEF *pPSD, Buffer* pListing);

#define D3DSP_WRITEMASK_RGBA (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3)
#define D3DSP_WRITEMASK_RGB (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2)
#define D3DSP_WRITEMASK_A (D3DSP_WRITEMASK_3)
#define D3DSP_WRITEMASK_SHIFT 16

#define MAX_SHADER_STAGES               4
#define MAX_COMBINER_STAGES             8
#define MAX_CONSTANTS                   16
#define PSHAD_MAX_ARGS_PER_INSTRUCTION  7
#define PSHADER_UNUSED                  0
#define CONSTANT_UNUSED                 0xffffffff
#define PSHAD_COLOR 0
#define PSHAD_ALPHA 1

D3DPIXELSHADERDEF          *g_pPSD;     // pointer to output storage
DWORD                       g_dwStage;  // current combiner stage
DWORD                       g_constMapping[MAX_COMBINER_STAGES+1][2];
DWORD                       g_D3DConstants[MAX_CONSTANTS];
XD3DXErrorLog*              g_pErrorLog;

int NumSrcRegs[] = {
     0, // D3DSIO_NOP
     1, // D3DSIO_MOV
     2, // D3DSIO_ADD
     2, // D3DSIO_SUB
     3, // D3DSIO_MAD
     2, // D3DSIO_MUL
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     2, // D3DSIO_DP3
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     3, // D3DSIO_LRP
     3, // D3DSIO_CND
     4, // D3DSIO_XMMA
     4, // D3DSIO_XMMC
     4, // D3DSIO_XDM
     4, // D3DSIO_XDD
     7, // D3DSIO_XFC
};

int NumDstRegs[] = {
     0, // D3DSIO_NOP
     1, // D3DSIO_MOV
     1, // D3DSIO_ADD
     1, // D3DSIO_SUB
     1, // D3DSIO_MAD
     1, // D3DSIO_MUL
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     1, // D3DSIO_DP3
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     0, // Undefined for pixel shaders
     1, // D3DSIO_LRP
     1, // D3DSIO_CND
     3, // D3DSIO_XMMA
     3, // D3DSIO_XMMC
     2, // D3DSIO_XDM
     2, // D3DSIO_XDD
     0, // D3DSIO_XFC
};

DWORD D3DModToNVMod[] = {
    PS_INPUTMAPPING_SIGNED_IDENTITY,    // D3DSPSM_NONE
    PS_INPUTMAPPING_SIGNED_NEGATE,      // D3DSPSM_NEG
    PS_INPUTMAPPING_HALFBIAS_NORMAL,    // D3DSPSM_BIAS
    PS_INPUTMAPPING_HALFBIAS_NEGATE,    // D3DSPSM_BIASNEG
    PS_INPUTMAPPING_EXPAND_NORMAL,      // D3DSPSM_SIGN
    PS_INPUTMAPPING_EXPAND_NEGATE,      // D3DSPSM_SIGNNEG
    PS_INPUTMAPPING_UNSIGNED_INVERT,    // D3DSPSM_COMP
    PS_INPUTMAPPING_UNSIGNED_IDENTITY,  // D3DSPSM_SAT
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    ~0
};

DWORD NVModToNVModInvert[] = 
{
    PS_INPUTMAPPING_UNSIGNED_INVERT,   // PS_INPUTMAPPING_UNSIGNED_IDENTITY
    PS_INPUTMAPPING_UNSIGNED_IDENTITY, // PS_INPUTMAPPING_UNSIGNED_INVERT
    PS_INPUTMAPPING_EXPAND_NORMAL,     // PS_INPUTMAPPING_EXPAND_NORMAL
    PS_INPUTMAPPING_EXPAND_NEGATE,     // PS_INPUTMAPPING_EXPAND_NEGATE
    PS_INPUTMAPPING_HALFBIAS_NORMAL,   // PS_INPUTMAPPING_HALFBIAS_NORMAL
    PS_INPUTMAPPING_HALFBIAS_NEGATE,   // PS_INPUTMAPPING_HALFBIAS_NEGATE
    PS_INPUTMAPPING_UNSIGNED_INVERT,   // PS_INPUTMAPPING_SIGNED_IDENTITY
    PS_INPUTMAPPING_SIGNED_NEGATE      // PS_INPUTMAPPING_SIGNED_NEGATE
};

DWORD TypeOffsetToCombinerReg[][4] = 
{
    {0xC, 0xD, 0x0, 0x3}, // D3DSPR_TEMP
    {0x4, 0x5, 0xe, 0xf}, // D3DSPR_INPUT
    {0x1, 0x2, 0x0, 0x0}, // D3DSPR_CONST
    {0x8, 0x9, 0xA, 0xB}, // D3DSPR_TEXTURE
    {0x0, 0x0, 0x0, 0x0}, // Unused type for pixel shaders
    {0x0, 0x0, 0x0, 0x0}  // Unused type for pixel shaders
};

DWORD D3DOpToTexMode[] =
{
    PS_TEXTUREMODES_PASSTHRU,             //D3DSIO_TEXCOORD
    PS_TEXTUREMODES_CLIPPLANE,            //D3DSIO_TEXKILL
    PS_TEXTUREMODES_PROJECT2D,            //D3DSIO_TEX
    PS_TEXTUREMODES_BUMPENVMAP,           //D3DSIO_TEXBEM
    PS_TEXTUREMODES_BUMPENVMAP_LUM,       //D3DSIO_TEXBEML
    PS_TEXTUREMODES_DPNDNT_AR,            //D3DSIO_TEXREG2AR
    PS_TEXTUREMODES_DPNDNT_GB,            //D3DSIO_TEXREG2GB
    PS_TEXTUREMODES_DOTPRODUCT,           //D3DSIO_TEXM3x2PAD
    PS_TEXTUREMODES_DOT_ST,               //D3DSIO_TEXM3x2TEX
    PS_TEXTUREMODES_DOTPRODUCT,           //D3DSIO_TEXM3x3PAD
    PS_TEXTUREMODES_DOT_STR_3D,           //D3DSIO_TEXM3x3TEX
    PS_TEXTUREMODES_DOT_RFLCT_DIFF,       //D3DSIO_TEXM3x3DIFF
    PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST, //D3DSIO_TEXM3x3SPEC
    PS_TEXTUREMODES_DOT_RFLCT_SPEC,       //D3DSIO_TEXM3x3VSPEC
};

int NumTexArgs[] = 
{
    1, // D3DSIO_TEXCOORD:
    1, // D3DSIO_TEXKILL:
    1, // D3DSIO_TEX:
    2, // D3DSIO_TEXBEM:
    2, // D3DSIO_TEXBEML:
    2, // D3DSIO_TEXREG2AR:
    2, // D3DSIO_TEXREG2GB:
    2, // D3DSIO_TEXM3x2PAD:
    2, // D3DSIO_TEXM3x2TEX:
    2, // D3DSIO_TEXM3x3PAD:
    2, // D3DSIO_TEXM3x3TEX:
    2, // D3DSIO_TEXM3x3DIFF:
    3, // D3DSIO_TEXM3x3SPEC:
    2  // D3DSIO_TEXM3x3VSPEC:
};

DWORD RGBSwizzleToChannel(DWORD swiz)
{
    if(swiz == D3DSP_REPLICATEALPHA)
        return PS_CHANNEL_ALPHA;
    else
        return PS_CHANNEL_RGB;
}

DWORD AlphaSwizzleToChannel(DWORD swiz)
{
    if((swiz & (3L << (D3DSP_SWIZZLE_SHIFT + 6))) == (2L << (D3DSP_SWIZZLE_SHIFT + 6)))    // blue replicated to alpha?
        return PS_CHANNEL_BLUE;
    else
        return PS_CHANNEL_ALPHA;
}

#define PSHAD_INSTRUCTION_ARGS DWORD stage, DWORD color, DWORD *outputmap, DWORD *dst, DWORD *src, DWORD *swizzle, DWORD *inputmod, DWORD *mask

void InstructionMOV(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),                 // A is src operand
            PS_REGISTER_ONE | PS_CHANNEL_RGB,                                       // B is 1
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,  // C is 0
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB); // D is 0
    
        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            dst[0],
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),                 // A is src operand
            PS_REGISTER_ONE | PS_CHANNEL_ALPHA,                                       // B is 1
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,  // C is 0
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA); // D is 0
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            dst[0],
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
}

void InstructionMUL(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),                 // A is src operand 0
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),                 // B is src operand 1
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,  // C is 0
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB); // D is 0

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            dst[0],
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),                 // A is src operand 0
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),                 // B is src operand 1
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,  // C is 0
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA); // D is 0
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            dst[0],
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
}

void InstructionDP3(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),                 // A is src operand 0
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),                 // B is src operand 1
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,  // C is 0
            PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB); // D is 0

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            dst[0],
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            outputmap[0] | 
            PS_COMBINEROUTPUT_AB_DOT_PRODUCT | 
            PS_COMBINEROUTPUT_CD_MULTIPLY | 
            PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        // copy dot product to alpha channel
        g_pPSD->PSRGBOutputs[stage] |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA << 12;
    }
}

void InstructionADD(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),                 // A is src operand 0
            PS_REGISTER_ONE | PS_CHANNEL_RGB,                                       // B is 1
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),                 // C is src operand 1
            PS_REGISTER_ONE | PS_CHANNEL_RGB);                                      // D is 1

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),               // A is src operand 0
            PS_REGISTER_ONE | PS_CHANNEL_ALPHA,                                     // B is 1
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),               // C is src operand 1
            PS_REGISTER_ONE | PS_CHANNEL_ALPHA);                                    // D is 1
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
}

void InstructionSUB(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),                 // A is src operand 0
            PS_REGISTER_ONE | PS_CHANNEL_RGB,                                       // B is 1
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),                 // C is src operand 1
            PS_REGISTER_NEGATIVE_ONE | PS_CHANNEL_RGB);                             // D is -1

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),               // A is src operand 0
            PS_REGISTER_ONE | PS_CHANNEL_ALPHA,                                     // B is 1
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),               // C is src operand 1
            PS_REGISTER_NEGATIVE_ONE | PS_CHANNEL_ALPHA);                           // D is -1
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
}

void InstructionMAD(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),                 // A is src operand 0
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),                 // B is src operand 1
            PS_REGISTER_ONE | PS_CHANNEL_RGB,                                       // C is 1
            src[2] | inputmod[2] | RGBSwizzleToChannel(swizzle[2]));                // D is src operand 2

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),               // A is src operand 0
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),               // B is src operand 1
            PS_REGISTER_ONE | PS_CHANNEL_ALPHA,                                     // C is 1
            src[2] | inputmod[2] | AlphaSwizzleToChannel(swizzle[2]));              // D is src operand 2
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
}

void InstructionLRP(PSHAD_INSTRUCTION_ARGS) 
{
    if((inputmod[0] != PS_INPUTMAPPING_UNSIGNED_INVERT) &&
       (inputmod[0] != PS_INPUTMAPPING_UNSIGNED_IDENTITY))
        inputmod[0] = PS_INPUTMAPPING_UNSIGNED_IDENTITY;   // force interpolant to be unsigned

    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),                 // A is src operand 0
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),                 // B is src operand 1
            src[2] | inputmod[2] | RGBSwizzleToChannel(swizzle[2]),                 // C is src operand 2
            src[0] | NVModToNVModInvert[inputmod[0]>>5] | RGBSwizzleToChannel(swizzle[0])); // D is -src operand 0

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),               // A is src operand 0
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),               // B is src operand 1
            src[2] | inputmod[2] | AlphaSwizzleToChannel(swizzle[2]),               // C is src operand 2
            src[0] | NVModToNVModInvert[inputmod[0]>>5] | AlphaSwizzleToChannel(swizzle[0])); // D is -src operand 0
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
}

void InstructionCND(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[2] | inputmod[2] | RGBSwizzleToChannel(swizzle[2]),                 // A is src operand 2
            PS_REGISTER_ONE | PS_CHANNEL_RGB,                                       // B is 1
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),                 // C is src operand 1
            PS_REGISTER_ONE | PS_CHANNEL_RGB);                                      // D is 1

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_MUX);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[2] | inputmod[2] | AlphaSwizzleToChannel(swizzle[2]),               // A is src operand 2
            PS_REGISTER_ONE | PS_CHANNEL_ALPHA,                                     // B is 1
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),               // C is src operand 1
            PS_REGISTER_ONE | PS_CHANNEL_ALPHA);                                    // D is 1
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            dst[0],
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_MUX);
    }
}

#define EXTENDED_INSTRUCTION(summux, abdot, cddot, dst2) \
    if(color == PSHAD_COLOR) \
    { \
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS( \
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]), \
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]), \
            src[2] | inputmod[2] | RGBSwizzleToChannel(swizzle[2]), \
            src[3] | inputmod[3] | RGBSwizzleToChannel(swizzle[3])); \
 \
        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS( \
            dst[0], \
            dst[1], \
            dst2, \
            outputmap[0] | abdot | cddot | summux); \
    } \
    else \
    { \
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS( \
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]), \
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]), \
            src[2] | inputmod[2] | AlphaSwizzleToChannel(swizzle[2]), \
            src[3] | inputmod[3] | AlphaSwizzleToChannel(swizzle[3])); \
     \
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS( \
            dst[0], \
            dst[1], \
            dst2, \
            outputmap[0] | summux); \
        if((abdot == PS_COMBINEROUTPUT_AB_DOT_PRODUCT) && (mask[0] & D3DSP_WRITEMASK_A))\
            g_pPSD->PSRGBOutputs[stage] |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA << 12; \
        if((cddot == PS_COMBINEROUTPUT_CD_DOT_PRODUCT) && (mask[1] & D3DSP_WRITEMASK_A))\
            g_pPSD->PSRGBOutputs[stage] |= PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA << 12; \
    }

void InstructionXMMA(PSHAD_INSTRUCTION_ARGS) 
{
    EXTENDED_INSTRUCTION(PS_COMBINEROUTPUT_AB_CD_SUM, 
                         PS_COMBINEROUTPUT_AB_MULTIPLY, 
                         PS_COMBINEROUTPUT_CD_MULTIPLY,
                         dst[2])
}

void InstructionXMMC(PSHAD_INSTRUCTION_ARGS) 
{
    EXTENDED_INSTRUCTION(PS_COMBINEROUTPUT_AB_CD_MUX, 
                         PS_COMBINEROUTPUT_AB_MULTIPLY, 
                         PS_COMBINEROUTPUT_CD_MULTIPLY,
                         dst[2])
}

void InstructionXDM(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),
            src[2] | inputmod[2] | RGBSwizzleToChannel(swizzle[2]),
            src[3] | inputmod[3] | RGBSwizzleToChannel(swizzle[3]));

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            dst[0],
            dst[1],
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),
            src[2] | inputmod[2] | AlphaSwizzleToChannel(swizzle[2]),
            src[3] | inputmod[3] | AlphaSwizzleToChannel(swizzle[3]));
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            dst[1],
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
            
        g_pPSD->PSRGBOutputs[stage] |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA << 12;
    }
}

void InstructionXDD(PSHAD_INSTRUCTION_ARGS) 
{
    if(color == PSHAD_COLOR)
    {
        g_pPSD->PSRGBInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),
            src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),
            src[2] | inputmod[2] | RGBSwizzleToChannel(swizzle[2]),
            src[3] | inputmod[3] | RGBSwizzleToChannel(swizzle[3]));

        g_pPSD->PSRGBOutputs[stage] = PS_COMBINEROUTPUTS(
            dst[0],
            dst[1],
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT | PS_COMBINEROUTPUT_AB_CD_SUM);
    }
    else
    {
        g_pPSD->PSAlphaInputs[stage] = PS_COMBINERINPUTS(
            src[0] | inputmod[0] | AlphaSwizzleToChannel(swizzle[0]),
            src[1] | inputmod[1] | AlphaSwizzleToChannel(swizzle[1]),
            src[2] | inputmod[2] | AlphaSwizzleToChannel(swizzle[2]),
            src[3] | inputmod[3] | AlphaSwizzleToChannel(swizzle[3]));
    
        g_pPSD->PSAlphaOutputs[stage] = PS_COMBINEROUTPUTS(
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            outputmap[0] | PS_COMBINEROUTPUT_AB_CD_SUM);
        
        g_pPSD->PSRGBOutputs[stage] |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA << 12;
        g_pPSD->PSRGBOutputs[stage] |= PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA << 12;
    }
}

void InstructionXFC(PSHAD_INSTRUCTION_ARGS) 
{
    g_pPSD->PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
        src[0] | inputmod[0] | RGBSwizzleToChannel(swizzle[0]),
        src[1] | inputmod[1] | RGBSwizzleToChannel(swizzle[1]),
        src[2] | inputmod[2] | RGBSwizzleToChannel(swizzle[2]),
        src[3] | inputmod[3] | RGBSwizzleToChannel(swizzle[3]));

    g_pPSD->PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
        src[4] | inputmod[4] | RGBSwizzleToChannel(swizzle[4]),
        src[5] | inputmod[5] | RGBSwizzleToChannel(swizzle[5]),
        src[6] | inputmod[6] | RGBSwizzleToChannel(swizzle[6]),
        PS_FINALCOMBINERSETTING_CLAMP_SUM);

}

void (*PSInstructionLUT[])(PSHAD_INSTRUCTION_ARGS) = 
{
    0,                            // D3DSIO_NOP
    InstructionMOV,               // D3DSIO_MOV
    InstructionADD,               // D3DSIO_ADD
    InstructionSUB,               // D3DSIO_SUB
    InstructionMAD,               // D3DSIO_MAD
    InstructionMUL,               // D3DSIO_MUL
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    InstructionDP3,               // D3DSIO_DP3
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    0,                            // Undefined for pixel shaders
    InstructionLRP,               // D3DSIO_LRP
    InstructionCND,               // D3DSIO_CND
    InstructionXMMA,              // D3DSIO_XMMA
    InstructionXMMC,              // D3DSIO_XMMC
    InstructionXDM,               // D3DSIO_XDM
    InstructionXDD,               // D3DSIO_XDD
    InstructionXFC,               // D3DSIO_XFC
};

DWORD ShiftAndBiasToMap(DWORD shift, DWORD bias)
{
    if(bias)
    {
        switch((DSTSHIFT)shift)
        {
        case DSTSHIFT_NONE:
            return PS_COMBINEROUTPUT_BIAS;
        case DSTSHIFT_X2:
            return PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS;
        default:
            // output error here
            return PS_COMBINEROUTPUT_IDENTITY;
        }
    }
    switch(shift) // no bias
    {
    case DSTSHIFT_NONE:
        return PS_COMBINEROUTPUT_IDENTITY;
    case DSTSHIFT_X2:
        return PS_COMBINEROUTPUT_SHIFTLEFT_1;
    case DSTSHIFT_X4:
        return PS_COMBINEROUTPUT_SHIFTLEFT_2;
    case DSTSHIFT_D2:
        return PS_COMBINEROUTPUT_SHIFTRIGHT_1;
    default:
        // output error here
        return PS_COMBINEROUTPUT_IDENTITY;
    }
}

// Decode the destination register token
DWORD DSTdecode(DWORD *dstreg, DWORD *outputmap, DWORD *colormask, DWORD op)
{
    DWORD type = (op & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT;
    DWORD offset = op & D3DSP_REGNUM_MASK;
    DWORD shift = ((op & D3DSP_DSTSHIFT_MASK) >> D3DSP_DSTSHIFT_SHIFT) & 0xf;
    DWORD dstmod = (op & D3DSP_DSTMOD_MASK) >> D3DSP_DSTMOD_SHIFT;

    *outputmap = ShiftAndBiasToMap(shift, dstmod);
    *dstreg = TypeOffsetToCombinerReg[type][offset];

    *colormask = op & D3DSP_WRITEMASK_ALL;

    return TRUE;
}

// Decode the source register token
DWORD SRCdecode(DWORD *src, DWORD *swizzle, DWORD *mod, DWORD op, DWORD IsFC)
{
    DWORD type = (op & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT; // const reg, texture reg, etc
    DWORD regnum = op & D3DSP_REGNUM_MASK;   // which reg number within a type set is it?
    DWORD offset = regnum;                   // save this, we may have to remap it later
    
    *swizzle = (op & D3DSP_SWIZZLE_MASK);

    if ((op & D3DSP_REGTYPE_MASK) == D3DSPR_CONST) 
    {
        if (regnum >= MAX_CONSTANTS) 
        {
            g_pErrorLog->Log(TRUE, ERROR_ASM_INVALID_SOURCE_REGISTER, 
                           NULL, 0, "Invalid constant source register.\n");
            return FALSE;
        }

        if(IsFC)
        {
            if((g_constMapping[8][0] == CONSTANT_UNUSED) ||
               (g_constMapping[8][0] == offset))
            {
                g_constMapping[8][0] = offset;
                regnum = 0; // register is now constant 0
                g_pPSD->PSFinalCombinerConstant0 = g_D3DConstants[offset];
            } 
            else if((g_constMapping[8][1] == CONSTANT_UNUSED) ||
                    (g_constMapping[8][1] == offset))
            {
                g_constMapping[8][1] = offset;
                regnum = 1; // register is now constant 1
                g_pPSD->PSFinalCombinerConstant1 = g_D3DConstants[offset];
            }
            else 
            {
                g_pErrorLog->Log(TRUE, ERROR_ASM_INVALID_CONSTANT, 
                               NULL, 0, "More than 2 constants used in final combiner.\n");
                return FALSE;
            }
        }
        else
        {
            if((g_constMapping[g_dwStage][0] == CONSTANT_UNUSED) ||
               (g_constMapping[g_dwStage][0] == offset))
            {
                g_constMapping[g_dwStage][0] = offset;
                regnum = 0; // register is now constant 0
                g_pPSD->PSConstant0[g_dwStage] = g_D3DConstants[offset];
            } 
            else if((g_constMapping[g_dwStage][1] == CONSTANT_UNUSED) ||
                    (g_constMapping[g_dwStage][1] == offset))
            {
                g_constMapping[g_dwStage][1] = offset;
                regnum = 1; // register is now constant 1
                g_pPSD->PSConstant1[g_dwStage] = g_D3DConstants[offset];
            }
            else 
            {
                char buf[256];
                sprintf(buf, "More than 2 constants used in stage %d.\n", g_dwStage);
                g_pErrorLog->Log(TRUE, ERROR_ASM_INVALID_CONSTANT, 
                               NULL, 0, buf);

                return FALSE;
            }
        }
    }

    *src = TypeOffsetToCombinerReg[type][regnum];
    *mod = D3DModToNVMod[(op & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT];
    
    // default mod for final combiner is UNSIGNED_IDENTITY (SIGNED_IDENTITY invalid)
    if(IsFC && (*mod == PS_INPUTMAPPING_SIGNED_IDENTITY))
        *mod = PS_INPUTMAPPING_UNSIGNED_IDENTITY;
    
    return TRUE;
}


HRESULT CompilePixelShaderToUCode(bool optimize, DWORD shaderType,
							 LPXGBUFFER pTokenizedShader, 
							 LPXGBUFFER* ppCompiledShader,
							 Buffer* pListing,
                             XD3DXErrorLog* pErrorLog)
{
	LPXGBUFFER pCompiledShader; // Output buffer
	
    g_pErrorLog = pErrorLog;    // make it global

    if(FAILED(XGBufferCreate(sizeof(D3DPIXELSHADERDEF), &pCompiledShader)))
    {
        g_pErrorLog->Log(TRUE, ERROR_ASM_INTERNAL_ASSEMBLER_ERROR, 
                       NULL, 0, "Unable to create output buffer.\n");
		return E_OUTOFMEMORY;
    }
	
    *ppCompiledShader = pCompiledShader;
    g_pPSD = (D3DPIXELSHADERDEF *)pCompiledShader->GetBufferPointer();
    memset(g_pPSD, 0x0, sizeof(D3DPIXELSHADERDEF));     // zero out the structure
    
    DWORD dwCodeSize = pTokenizedShader->GetBufferSize();
	DWORD *pCode = (DWORD *)pTokenizedShader->GetBufferPointer();
    DWORD op, opcode;
    DWORD texturesUsed = 0;
    g_dwStage = 0;                  // stage being built

    DWORD mask[PSHAD_MAX_ARGS_PER_INSTRUCTION];
    DWORD dst[PSHAD_MAX_ARGS_PER_INSTRUCTION];
    DWORD src[PSHAD_MAX_ARGS_PER_INSTRUCTION];
    DWORD swizzle[PSHAD_MAX_ARGS_PER_INSTRUCTION];
    DWORD inputmod[PSHAD_MAX_ARGS_PER_INSTRUCTION];
    DWORD outputmap[PSHAD_MAX_ARGS_PER_INSTRUCTION];

    DWORD texturemode[MAX_SHADER_STAGES];
    DWORD otherreg[MAX_SHADER_STAGES];
    DWORD dotmap[MAX_SHADER_STAGES];
    DWORD shaderstage = 0;
    DWORD docoissue;

    // initialize constant mapping
    DWORD i;
    for (i=0; i<(MAX_COMBINER_STAGES+1); i++) 
        g_constMapping[i][0] = g_constMapping[i][1] = CONSTANT_UNUSED;

    for(i=0; i<MAX_CONSTANTS; i++)
        g_D3DConstants[i] = 0;

    // initialize texturemodes
    for(i=0; i<MAX_SHADER_STAGES; i++)
    {
        texturemode[i] = PS_TEXTUREMODES_NONE;
        otherreg[i] = 0;
        dotmap[i] = PS_DOTMAPPING_ZERO_TO_ONE;
    }

    pCode++;    // skip version info

    for(UINT xx=0; xx<dwCodeSize; xx += sizeof(DWORD))
    {
        // step through tokens
        
        
        op = *pCode++;
        opcode = op & D3DSI_OPCODE_MASK;
        docoissue = (op & D3DSI_COISSUE);

        if(opcode == D3DSIO_END) // End of tokens
        {
            break;
        }
        else if(opcode == D3DSIO_NOP) // no-op
        {   // do nothing
        }
        else if(opcode == D3DSIO_COMMENT) // comment
        {
            INT cmtsize = (op & D3DSI_COMMENTSIZE_MASK) >> 
                          D3DSI_COMMENTSIZE_SHIFT;
            // skip comment DWORDs
            while(cmtsize-- > 0)
                pCode++;
        }
        else if(opcode == D3DSIO_DEF)
        {
            // get destination register
            op = *pCode++;
            // components are in RGBA order
            DWORD constant = op & D3DSP_REGNUM_MASK;
            g_D3DConstants[constant] = (DWORD)(((*(FLOAT*)pCode++) * 255.0f) + 0.5f) & 0xff;
            g_D3DConstants[constant] <<= 8;
            g_D3DConstants[constant] |= (DWORD)(((*(FLOAT*)pCode++) * 255.0f) + 0.5f) & 0xff;
            g_D3DConstants[constant] <<= 8;
            g_D3DConstants[constant] |= (DWORD)(((*(FLOAT*)pCode++) * 255.0f) + 0.5f) & 0xff;
            DWORD alpha = (DWORD)(((*(FLOAT*)pCode++) * 255.0f) + 0.5f) & 0xff;
            g_D3DConstants[constant] |= alpha << 24;
        }
        else if((opcode < D3DSIO_TEXCOORD) || (opcode == D3DSIO_CND) ||
                ((opcode >= D3DSIO_XMMA) && (opcode <= D3DSIO_XFC)))
        {
            // store the opcode offset for lookups
            DWORD opoffset;
            if(opcode == D3DSIO_CND)
                opoffset = D3DSIO_LRP+1;
            else if((opcode >= D3DSIO_XMMA) && (opcode <= D3DSIO_XFC))
                opoffset = opcode - D3DSIO_XMMA+D3DSIO_LRP+2;
            else
                opoffset = opcode;

            // It's a standard instruction
			if (docoissue) 
            {
				// They are issuing a color op and an alpha op in parallel.
				// Decrease stage count to match last instruction and check consistency.
				g_dwStage--; 
            }

            
            int i;
            BOOL bWritesToAlpha = FALSE, bWritesToRGB = FALSE;

            for (i=0; i < NumDstRegs[opoffset]; i++) 
            {
                op = *pCode++;
                if (!DSTdecode(&(dst[i]), &(outputmap[i]), &(mask[i]), op)) 
                    return E_FAIL; // error message already logged
                if(mask[i] & D3DSP_WRITEMASK_RGB) bWritesToRGB = TRUE;
                if(mask[i] & D3DSP_WRITEMASK_A) bWritesToAlpha = TRUE;
            }
            // default dst values for XFC instruction (no dst param)
            DWORD IsFC = FALSE;
            if(i==0)
            {
                dst[0] = PS_REGISTER_ZERO;
                outputmap[0] = 0;
                mask[0] = D3DSP_WRITEMASK_RGB;
                IsFC = TRUE;
                bWritesToRGB = TRUE;
            }

            for (i=0; i < NumSrcRegs[opoffset]; i++) 
            {
                op = *pCode++;
                if (!SRCdecode(&src[i], &swizzle[i], &inputmod[i], op, IsFC)) 
                    return E_FAIL; // error message already logged
            }

            if (bWritesToRGB) 
            {
				(*PSInstructionLUT[opoffset])(g_dwStage, PSHAD_COLOR, outputmap, dst, src, swizzle, inputmod, mask);
			}
			if (bWritesToAlpha) 
            {
				(*PSInstructionLUT[opoffset])(g_dwStage, PSHAD_ALPHA, outputmap, dst, src, swizzle, inputmod, mask);
			}
            if(opcode != D3DSIO_XFC)    // XFC doesn't use a combiner stage
                g_dwStage++;
        }
        else if((opcode <= D3DSIO_TEXM3x3VSPEC) ||
                (opcode == D3DSIO_TEXM3x2DEPTH) ||
                (opcode == D3DSIO_TEXBRDF))
        {
            // It's a texture addressing instruction
            DWORD dstoffset, srcoffset;
            DWORD srcmod = 0;

            // make texture opcodes relative to D3DSIO_TEXCOORD
            opcode -= D3DSIO_TEXCOORD;

            // Get destination register offset
            op = *pCode++;
            dstoffset = op & D3DSP_REGNUM_MASK;

            // determine number of arguments
            int TexArgCount;
            if(opcode == (D3DSIO_TEXM3x2DEPTH-D3DSIO_TEXCOORD))
                TexArgCount = 2;
            else if(opcode == (D3DSIO_TEXBRDF-D3DSIO_TEXCOORD))
                TexArgCount = 1;
            else
                TexArgCount = NumTexArgs[opcode];

            if (TexArgCount >= 2) 
            {
                op = *pCode++;
                srcoffset = op & D3DSP_REGNUM_MASK;
                srcmod = op & D3DSP_SRCMOD_MASK;
            } 
            else 
            {
                srcoffset = PSHADER_UNUSED;
            }

            // Last register is always redundant
            if (TexArgCount >= 3) 
                op = *pCode++;

            if (dstoffset >= MAX_SHADER_STAGES) 
            {
                // destination register exceeds the available texture registers
                // error here
                g_pErrorLog->Log(TRUE, ERROR_ASM_INVALID_DESTINATION_REGISTER, 
                               NULL, 0, "Invalid texture destination register.\n");
                return E_FAIL;
            }

            switch(opcode)
            {
            case D3DSIO_TEXM3x2DEPTH-D3DSIO_TEXCOORD:
                texturemode[dstoffset] = PS_TEXTUREMODES_DOT_ZW;
                break;
            case D3DSIO_TEXBRDF-D3DSIO_TEXCOORD:
                texturemode[dstoffset] = PS_TEXTUREMODES_BRDF;
                break;
            default:
                texturemode[dstoffset] = D3DOpToTexMode[opcode];
                break;
            }
            otherreg[dstoffset] = srcoffset;
            switch(srcmod)
            {
            case D3DSPSM_NONE:    dotmap[dstoffset] = PS_DOTMAPPING_ZERO_TO_ONE; break;
            case D3DSPSM_SIGN:    dotmap[dstoffset] = PS_DOTMAPPING_MINUS1_TO_1_D3D; break;
            case D3DSPSM_NEG:     dotmap[dstoffset] = PS_DOTMAPPING_MINUS1_TO_1_GL; break;
            case D3DSPSM_BIAS:    dotmap[dstoffset] = PS_DOTMAPPING_MINUS1_TO_1; break;
            case D3DSPSM_BIASNEG: dotmap[dstoffset] = PS_DOTMAPPING_HILO_1; break;
            case D3DSPSM_SIGNNEG: dotmap[dstoffset] = PS_DOTMAPPING_HILO_HEMISPHERE_D3D; break;
            case D3DSPSM_COMP:    dotmap[dstoffset] = PS_DOTMAPPING_HILO_HEMISPHERE_GL; break;
            case D3DSPSM_SAT:     dotmap[dstoffset] = PS_DOTMAPPING_HILO_HEMISPHERE; break;
            }
            shaderstage++;
        }
        else
        {
            // Unknown instruction
            g_pErrorLog->Log(TRUE, ERROR_ASM_INVALID_INSTRUCTION, 
                           NULL, 0, "Unrecognized Instruction.\n");
            return E_FAIL;
        }
    }

    // set shader registers and global combiner registers
    g_pPSD->PSTextureModes = PS_TEXTUREMODES(
        texturemode[0],
        texturemode[1],
        texturemode[2],
        texturemode[3]);
    g_pPSD->PSInputTexture = PS_INPUTTEXTURE(
        otherreg[0],
        otherreg[1],
        otherreg[2],
        otherreg[3]);
    g_pPSD->PSDotMapping = PS_DOTMAPPING(dotmap[0], dotmap[1], dotmap[2], dotmap[3]);
    
    g_pPSD->PSCombinerCount = PS_COMBINERCOUNT(
        g_dwStage,
        PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);

    // set the D3D constant register mapping
    g_pPSD->PSC0Mapping = PS_CONSTANTMAPPING(g_constMapping[0][0], g_constMapping[1][0], \
                                             g_constMapping[2][0], g_constMapping[3][0], \
                                             g_constMapping[4][0], g_constMapping[5][0], \
                                             g_constMapping[6][0], g_constMapping[7][0]);
    g_pPSD->PSC1Mapping = PS_CONSTANTMAPPING(g_constMapping[0][1], g_constMapping[1][1], \
                                             g_constMapping[2][1], g_constMapping[3][1], \
                                             g_constMapping[4][1], g_constMapping[5][1], \
                                             g_constMapping[6][1], g_constMapping[7][1]);
    g_pPSD->PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(
        g_constMapping[8][0], g_constMapping[8][1], PS_GLOBALFLAGS_TEXMODE_ADJUST);

    // print listing if requested
    if(pListing)
        PrintPixelShaderDef(g_pPSD, pListing);

    return S_OK;
}



char *ccf0[] =
{
    "PS_COMBINERCOUNT_MUX_LSB",
    "PS_COMBINERCOUNT_MUX_MSB",
};
    
char *ccf1[] =
{
    "PS_COMBINERCOUNT_SAME_C0",
    "PS_COMBINERCOUNT_UNIQUE_C0",
};
    
char *ccf2[] =
{
    "PS_COMBINERCOUNT_SAME_C1",
    "PS_COMBINERCOUNT_UNIQUE_C1",
};

char *tm[] =
{
    "PS_TEXTUREMODES_NONE",
    "PS_TEXTUREMODES_PROJECT2D",
    "PS_TEXTUREMODES_PROJECT3D",
    "PS_TEXTUREMODES_CUBEMAP",
    "PS_TEXTUREMODES_PASSTHRU",
    "PS_TEXTUREMODES_CLIPPLANE",
    "PS_TEXTUREMODES_BUMPENVMAP",
    "PS_TEXTUREMODES_BUMPENVMAP_LUM",
    "PS_TEXTUREMODES_BRDF",
    "PS_TEXTUREMODES_DOT_ST",
    "PS_TEXTUREMODES_DOT_ZW",
    "PS_TEXTUREMODES_DOT_RFLCT_DIFF",
    "PS_TEXTUREMODES_DOT_RFLCT_SPEC",
    "PS_TEXTUREMODES_DOT_STR_3D",
    "PS_TEXTUREMODES_DOT_STR_CUBE",
    "PS_TEXTUREMODES_DPNDNT_AR",
    "PS_TEXTUREMODES_DPNDNT_GB",
    "PS_TEXTUREMODES_DOTPRODUCT",
    "PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
    "INVALID",
};

char *dm[] =
{
    "PS_DOTMAPPING_ZERO_TO_ONE",
    "PS_DOTMAPPING_MINUS1_TO_1_D3D",
    "PS_DOTMAPPING_MINUS1_TO_1_GL",
    "PS_DOTMAPPING_MINUS1_TO_1",
    "PS_DOTMAPPING_HILO_1",
    "PS_DOTMAPPING_HILO_HEMISPHERE_D3D",
    "PS_DOTMAPPING_HILO_HEMISPHERE_GL",
    "PS_DOTMAPPING_HILO_HEMISPHERE",
};

char *cms[] =
{
    "PS_COMPAREMODE_S_LT",
    "PS_COMPAREMODE_S_GE",
};
    
char *cmt[] =
{
    "PS_COMPAREMODE_T_LT",
    "PS_COMPAREMODE_T_GE",
};
    
char *cmr[] =
{
    "PS_COMPAREMODE_R_LT",
    "PS_COMPAREMODE_R_GE",
};
    
char *cmq[] =
{
    "PS_COMPAREMODE_Q_LT",
    "PS_COMPAREMODE_Q_GE",
};

char *inmap[] =
{
    "PS_INPUTMAPPING_UNSIGNED_IDENTITY",
    "PS_INPUTMAPPING_UNSIGNED_INVERT",
    "PS_INPUTMAPPING_EXPAND_NORMAL",
    "PS_INPUTMAPPING_EXPAND_NEGATE",
    "PS_INPUTMAPPING_HALFBIAS_NORMAL",
    "PS_INPUTMAPPING_HALFBIAS_NEGATE",
    "PS_INPUTMAPPING_SIGNED_IDENTITY",
    "PS_INPUTMAPPING_SIGNED_NEGATE",
};

char *inreg[] =
{
    "PS_REGISTER_ZERO",
    "PS_REGISTER_C0",
    "PS_REGISTER_C1",
    "PS_REGISTER_FOG",
    "PS_REGISTER_V0",
    "PS_REGISTER_V1",
    "INVALID",
    "INVALID",
    "PS_REGISTER_T0",
    "PS_REGISTER_T1",
    "PS_REGISTER_T2",
    "PS_REGISTER_T3",
    "PS_REGISTER_R0",
    "PS_REGISTER_R1",
    "PS_REGISTER_V1R0_SUM",
    "PS_REGISTER_EF_PROD",
};

char *outreg[] =
{
    "PS_REGISTER_DISCARD",
    "PS_REGISTER_C0",
    "PS_REGISTER_C1",
    "PS_REGISTER_FOG",
    "PS_REGISTER_V0",
    "PS_REGISTER_V1",
    "INVALID",
    "INVALID",
    "PS_REGISTER_T0",
    "PS_REGISTER_T1",
    "PS_REGISTER_T2",
    "PS_REGISTER_T3",
    "PS_REGISTER_R0",
    "PS_REGISTER_R1",
    "PS_REGISTER_V1R0_SUM",
    "PS_REGISTER_EF_PROD",
};

char *chanrgb[] =
{
    "PS_CHANNEL_RGB",
    "PS_CHANNEL_ALPHA",
};

char *chanalpha[] =
{
    "PS_CHANNEL_BLUE",
    "PS_CHANNEL_ALPHA",
};
    
char *outmap[] =
{
    "PS_COMBINEROUTPUT_IDENTITY",
    "PS_COMBINEROUTPUT_BIAS",
    "PS_COMBINEROUTPUT_SHIFTLEFT_1",
    "PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS",
    "PS_COMBINEROUTPUT_SHIFTLEFT_2",
    "INVALID",
    "PS_COMBINEROUTPUT_SHIFTRIGHT_1",
    "INVALID",
};
    
char *abmult[] =
{
    "PS_COMBINEROUTPUT_AB_MULTIPLY",
    "PS_COMBINEROUTPUT_AB_DOT_PRODUCT",
};

char *cdmult[] =
{
    "PS_COMBINEROUTPUT_CD_MULTIPLY",
    "PS_COMBINEROUTPUT_CD_DOT_PRODUCT",
};

char *muxsum[] =
{
    "PS_COMBINEROUTPUT_AB_CD_SUM",
    "PS_COMBINEROUTPUT_AB_CD_MUX",
};

#define ENUMVAL(val, upper, lower) ((val >> lower) & ((2 << (upper-lower))-1))
#define ENUMNAME(val, upper, lower, array) array[ENUMVAL(val, upper, lower)]


void PrintPixelShaderDef(D3DPIXELSHADERDEF *pPSD, Buffer* pListing)
{
    pListing->Printf("D3DPIXELSHADERDEF psd;\n");
    pListing->Printf("ZeroMemory(&psd, sizeof(psd));\n");
    pListing->Printf("psd.PSCombinerCount=PS_COMBINERCOUNT(\n");
    DWORD stagecount = ENUMVAL(pPSD->PSCombinerCount, 3, 0);
    pListing->Printf("    %d,\n", stagecount);
    pListing->Printf("    %s | %s | %s);\n", ENUMNAME(pPSD->PSCombinerCount,  8,  8, ccf0),
                                   ENUMNAME(pPSD->PSCombinerCount, 12, 12, ccf1),
                                   ENUMNAME(pPSD->PSCombinerCount, 16, 16, ccf2));
    
    pListing->Printf("psd.PSTextureModes=PS_TEXTUREMODES(\n");
    pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSTextureModes,  4,  0, tm));
    pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSTextureModes,  9,  5, tm));
    pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSTextureModes, 14, 10, tm));
    pListing->Printf("    %s);\n", ENUMNAME(pPSD->PSTextureModes, 19, 15, tm));

    pListing->Printf("psd.PSInputTexture=PS_INPUTTEXTURE(%d,%d,%d,%d);\n", 
           0,
           0,
           ENUMVAL(pPSD->PSInputTexture, 16, 16),
           ENUMVAL(pPSD->PSInputTexture, 21, 20));

    pListing->Printf("psd.PSDotMapping=PS_DOTMAPPING(\n");
    pListing->Printf("    0,\n");
    pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSDotMapping,  2,  0, dm));
    pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSDotMapping,  6,  4, dm));
    pListing->Printf("    %s);\n", ENUMNAME(pPSD->PSDotMapping, 10,  8, dm));

    pListing->Printf("psd.PSCompareMode=PS_COMPAREMODE(\n");
    pListing->Printf("    %s | %s | %s | %s,\n",
           ENUMNAME(pPSD->PSCompareMode,  0,  0, cms),
           ENUMNAME(pPSD->PSCompareMode,  1,  1, cmt),
           ENUMNAME(pPSD->PSCompareMode,  2,  2, cmr),
           ENUMNAME(pPSD->PSCompareMode,  3,  3, cmq));
    pListing->Printf("    %s | %s | %s | %s,\n",
           ENUMNAME(pPSD->PSCompareMode,  4,  4, cms),
           ENUMNAME(pPSD->PSCompareMode,  5,  5, cmt),
           ENUMNAME(pPSD->PSCompareMode,  6,  6, cmr),
           ENUMNAME(pPSD->PSCompareMode,  7,  7, cmq));
    pListing->Printf("    %s | %s | %s | %s,\n",
           ENUMNAME(pPSD->PSCompareMode,  8,  8, cms),
           ENUMNAME(pPSD->PSCompareMode,  9,  9, cmt),
           ENUMNAME(pPSD->PSCompareMode, 10, 10, cmr),
           ENUMNAME(pPSD->PSCompareMode, 11, 11, cmq));
    pListing->Printf("    %s | %s | %s | %s);\n",
           ENUMNAME(pPSD->PSCompareMode, 12, 12, cms),
           ENUMNAME(pPSD->PSCompareMode, 13, 13, cmt),
           ENUMNAME(pPSD->PSCompareMode, 14, 14, cmr),
           ENUMNAME(pPSD->PSCompareMode, 15, 15, cmq));

    for(UINT i=0; i<stagecount; i++)
    {
        pListing->Printf("\n//------------- Stage %d -------------\n", i);
        pListing->Printf("psd.PSRGBInputs[%d]=PS_COMBINERINPUTS(\n", i);
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSRGBInputs[i], 27, 24, inreg),
            ENUMNAME(pPSD->PSRGBInputs[i], 28, 28, chanrgb),
            ENUMNAME(pPSD->PSRGBInputs[i], 31, 29, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSRGBInputs[i], 19, 16, inreg),
            ENUMNAME(pPSD->PSRGBInputs[i], 20, 20, chanrgb),
            ENUMNAME(pPSD->PSRGBInputs[i], 23, 21, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSRGBInputs[i], 11,  8, inreg),
            ENUMNAME(pPSD->PSRGBInputs[i], 12, 12, chanrgb),
            ENUMNAME(pPSD->PSRGBInputs[i], 15, 13, inmap));
        pListing->Printf("    %s | %s | %s);\n", 
            ENUMNAME(pPSD->PSRGBInputs[i],  3,  0, inreg),
            ENUMNAME(pPSD->PSRGBInputs[i],  4,  4, chanrgb),
            ENUMNAME(pPSD->PSRGBInputs[i],  7,  5, inmap));
        
        pListing->Printf("psd.PSAlphaInputs[%d]=PS_COMBINERINPUTS(\n", i);
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSAlphaInputs[i], 27, 24, inreg),
            ENUMNAME(pPSD->PSAlphaInputs[i], 28, 28, chanalpha),
            ENUMNAME(pPSD->PSAlphaInputs[i], 31, 29, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSAlphaInputs[i], 19, 16, inreg),
            ENUMNAME(pPSD->PSAlphaInputs[i], 20, 20, chanalpha),
            ENUMNAME(pPSD->PSAlphaInputs[i], 23, 21, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSAlphaInputs[i], 11,  8, inreg),
            ENUMNAME(pPSD->PSAlphaInputs[i], 12, 12, chanalpha),
            ENUMNAME(pPSD->PSAlphaInputs[i], 15, 13, inmap));
        pListing->Printf("    %s | %s | %s);\n", 
            ENUMNAME(pPSD->PSAlphaInputs[i],  3,  0, inreg),
            ENUMNAME(pPSD->PSAlphaInputs[i],  4,  4, chanalpha),
            ENUMNAME(pPSD->PSAlphaInputs[i],  7,  5, inmap));

        pListing->Printf("psd.PSRGBOutputs[%d]=PS_COMBINEROUTPUTS(\n", i);
        pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSRGBOutputs[i],  7,  4, outreg));
        pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSRGBOutputs[i],  3,  0, outreg));
        pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSRGBOutputs[i], 11,  8, outreg));
        pListing->Printf("    %s | %s | %s | %s",
               ENUMNAME(pPSD->PSRGBOutputs[i], 17, 15, outmap),
               ENUMNAME(pPSD->PSRGBOutputs[i], 13, 13, abmult),
               ENUMNAME(pPSD->PSRGBOutputs[i], 12, 12, cdmult),
               ENUMNAME(pPSD->PSRGBOutputs[i], 14, 14, muxsum));
        if(ENUMVAL(pPSD->PSRGBOutputs[i], 19, 19))
            pListing->Printf(" | PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA");
        if(ENUMVAL(pPSD->PSRGBOutputs[i], 18, 18))
            pListing->Printf(" | PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA");
        pListing->Printf(");\n");

        pListing->Printf("psd.PSAlphaOutputs[%d]=PS_COMBINEROUTPUTS(\n", i);
        pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSAlphaOutputs[i],  7,  4, outreg));
        pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSAlphaOutputs[i],  3,  0, outreg));
        pListing->Printf("    %s,\n", ENUMNAME(pPSD->PSAlphaOutputs[i], 11,  8, outreg));
        pListing->Printf("    %s | %s | %s | %s);\n",
               ENUMNAME(pPSD->PSAlphaOutputs[i], 17, 15, outmap),
               ENUMNAME(pPSD->PSAlphaOutputs[i], 13, 13, abmult),
               ENUMNAME(pPSD->PSAlphaOutputs[i], 12, 12, cdmult),
               ENUMNAME(pPSD->PSAlphaOutputs[i], 14, 14, muxsum));
        pListing->Printf("psd.PSConstant0[%d] = 0x%08x;\n", i, pPSD->PSConstant0[i]);
        pListing->Printf("psd.PSConstant1[%d] = 0x%08x;\n", i, pPSD->PSConstant1[i]);
    }

    // only print info on final combiner if it has been set
    if((pPSD->PSFinalCombinerInputsABCD | pPSD->PSFinalCombinerInputsEFG) != 0)
    {
        pListing->Printf("\n//------------- FinalCombiner -------------\n", i);
        pListing->Printf("psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(\n");
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 27, 24, inreg),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 28, 28, chanrgb),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 31, 29, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 19, 16, inreg),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 20, 20, chanrgb),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 23, 21, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 11,  8, inreg),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 12, 12, chanrgb),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD, 15, 13, inmap));
        pListing->Printf("    %s | %s | %s);\n", 
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD,  3,  0, inreg),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD,  4,  4, chanrgb),
            ENUMNAME(pPSD->PSFinalCombinerInputsABCD,  7,  5, inmap));
        
        pListing->Printf("psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(\n");
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 27, 24, inreg),
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 28, 28, chanrgb),
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 31, 29, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 19, 16, inreg),
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 20, 20, chanrgb),
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 23, 21, inmap));
        pListing->Printf("    %s | %s | %s,\n", 
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 11,  8, inreg),
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 12, 12, chanrgb),
            ENUMNAME(pPSD->PSFinalCombinerInputsEFG, 15, 13, inmap));
        
        if(ENUMVAL(pPSD->PSFinalCombinerInputsEFG, 7, 7))
            pListing->Printf("    PS_FINALCOMBINERSETTING_CLAMP_SUM");
        else
            pListing->Printf("    0");
        
        if(ENUMVAL(pPSD->PSFinalCombinerInputsEFG, 6, 6))
            pListing->Printf(" | PS_FINALCOMBINERSETTING_COMPLEMENT_V1");
        else
            pListing->Printf(" | 0");
        
        if(ENUMVAL(pPSD->PSFinalCombinerInputsEFG, 5, 5))
            pListing->Printf(" | PS_FINALCOMBINERSETTING_COMPLEMENT_R0);");
        else
            pListing->Printf(" | 0);\n");
        
        pListing->Printf("psd.PSFinalCombinerConstant0 = 0x%08x;\n", pPSD->PSFinalCombinerConstant0);
        pListing->Printf("psd.PSFinalCombinerConstant1 = 0x%08x;\n", pPSD->PSFinalCombinerConstant1);
    }
        
    pListing->Printf("\n");
    pListing->Printf("psd.PSC0Mapping = 0x%08x;\n", pPSD->PSC0Mapping);
    pListing->Printf("psd.PSC1Mapping = 0x%08x;\n", pPSD->PSC1Mapping);
    pListing->Printf("psd.PSFinalCombinerConstants = 0x%08x;\n", pPSD->PSFinalCombinerConstants);
}


} // namespace XGRAPHICS

