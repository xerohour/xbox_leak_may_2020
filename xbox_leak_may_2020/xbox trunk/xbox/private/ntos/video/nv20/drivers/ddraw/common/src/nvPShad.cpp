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
//  Module: nvPShad.cpp
//
// **************************************************************************
//
//  History:
//      Hadden Hoppert?            May00            DX8 development
//
// **************************************************************************

#include "nvprecomp.h"
#include <stdio.h> // For sprintf
 
/*****************************************************************************
 ***  CPixelShader  **********************************************************
 *****************************************************************************/

// Lots of lookup tables

const DWORD CPixelShader::PSD3DModToNVMap[] = {
    NV_MAPPING_SIGNED_IDENTITY,   // D3DSPSM_NONE
    NV_MAPPING_SIGNED_NEGATE,     // D3DSPSM_NEG
    NV_MAPPING_HALF_BIAS_NORMAL,  // D3DSPSM_BIAS
    NV_MAPPING_HALF_BIAS_NEGATE,  // D3DSPSM_BIASNEG
    NV_MAPPING_EXPAND_NORMAL,     // D3DSPSM_SIGN
    NV_MAPPING_EXPAND_NEGATE,     // D3DSPSM_SIGNNEG
    NV_MAPPING_UNSIGNED_INVERT,   // D3DSPSM_COMP
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    ~0
};

// Take the complement of our mapping mode.
const DWORD CPixelShader::NVMapToNVMapInvert[] = {
    NV_MAPPING_UNSIGNED_INVERT,   // NV_MAPPING_UNSIGNED_IDENTITY
    NV_MAPPING_UNSIGNED_IDENTITY, // NV_MAPPING_UNSIGNED_INVERT
    ~0,                           // NV_MAPPING_EXPAND_NORMAL
    ~0,                           // NV_MAPPING_EXPAND_NEGATE
    ~0,                           // NV_MAPPING_HALF_BIAS_NORMAL
    ~0,                           // NV_MAPPING_HALF_BIAS_NEGATE
    NV_MAPPING_UNSIGNED_INVERT,   // NV_MAPPING_SIGNED_IDENTITY
    ~0,                           // NV_MAPPING_SIGNED_NEGATE
};

// We only support up to 4 of each register type.  This will probably change in NV30.
const DWORD CPixelShader::PSTypeOffsetToCombinerReg[][4] = {
    {0xC, 0xD, 0x0, 0x0}, // D3DSPR_TEMP
    {0x4, 0x5, 0x0, 0x0}, // D3DSPR_INPUT
    {0x1, 0x2, 0x0, 0x0}, // D3DSPR_CONST
    {0x8, 0x9, 0xA, 0xB}, // D3DSPR_TEXTURE
    {0x0, 0x0, 0x0, 0x0}, // Unused type for pixel shaders
    {0x0, 0x0, 0x0, 0x0}  // Unused type for pixel shaders
};

// Virtually all of these are ordinary instructions--only DEF is different.
// I would just make this a case statement, but a table makes for future
// expandability and easier debugging.
const DWORD CPixelShader::PSInstructionType[] = {
    PSHAD_INS, // D3DSIO_NOP
    PSHAD_INS, // D3DSIO_MOV
    PSHAD_INS, // D3DSIO_ADD
    PSHAD_INS, // D3DSIO_SUB
    PSHAD_INS, // D3DSIO_MAD
    PSHAD_INS, // D3DSIO_MUL
    ~0, ~0,
    PSHAD_INS, // D3DSIO_DP3
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    PSHAD_INS, // D3DSIO_LRP
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
    PSHAD_INS, // D3DSIO_NV_MMA
    PSHAD_INS, // D3DSIO_NV_DD
    PSHAD_INS, // D3DSIO_NV_MD
    PSHAD_INS, // D3DSIO_NV_MM
    PSHAD_INS, // D3DSIO_NV_SPF
    PSHAD_TEX, // D3DSIO_TEXCOORD
    PSHAD_TEX, // D3DSIO_TEXKILL
    PSHAD_TEX, // D3DSIO_TEX
    PSHAD_TEX, // D3DSIO_TEXBEM
    PSHAD_TEX, // D3DSIO_TEXBEML
    PSHAD_TEX, // D3DSIO_TEXREG2AR
    PSHAD_TEX, // D3DSIO_TEXREG2GB
    PSHAD_TEX, // D3DSIO_TEXM3x2PAD
    PSHAD_TEX, // D3DSIO_TEXM3x2TEX
    PSHAD_TEX, // D3DSIO_TEXM3x3PAD
    PSHAD_TEX, // D3DSIO_TEXM3x3TEX
    PSHAD_TEX, // D3DSIO_TEXM3x3DIFF
    PSHAD_TEX, // D3DSIO_TEXM3x3SPEC
    PSHAD_TEX, // D3DSIO_TEXM3x3VSPEC
    ~0, ~0,
    PSHAD_INS, // D3DSIO_CND
    PSHAD_DEF, // D3DSIO_DEF
};


#ifdef DEBUG

// Just some debugging strings

const char * CPixelShader::PSInstructionStrings[] = {
    "nop",
    "mov",
    "add",
    "sub",
    "mad",
    "mul",
    0, 0,
    "dp3",
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    "lrp",
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0,
    "nv_mma",
    "nv_dd",
    "nv_md",
    "nv_mm",
    "nv_spf",
    "texcoord",
    "texkill",
    "tex",
    "texbem",
    "texbeml",
    "texreg2ar",
    "texreg2gb",
    "texm3x2pad",
    "texm3x2tex",
    "texm3x3pad",
    "texm3x3tex",
    "texm3x3diff",
    "texm3x3spec",
    "texm3x3vspec",
    0, 0,
    "cnd",
    "def",
};

const char * CPixelShader::PSRegTypeToPrefix[] = {"r", "v", "c", "t"};
const char * CPixelShader::PSWriteMask[] = {
      ".",   ".b",   ".g",   ".gb",
     ".r",  ".rb",  ".rg",  ".rgb",
     ".a",  ".ab",  ".ag",  ".agb",
    ".ar", ".arb", ".arg",      ""};

const char * CPixelShader::PSProgramNames[] = { 
    "PROGRAM_NONE", 
    "2D_PROJECTIVE", 
    "3D_PROJECTIVE", 
    "CUBE_MAP",
    "PASS_THROUGH",
    "CLIP_PLANE",
    "BUMPENVMAP",
    "BUMPENVMAP_LUMINANCE",
    "BRDF",
    "DOT_ST",
    "DOT_ZW",
    "DOT_REFLECT_DIFFUSE",
    "DOT_REFLECT_SPECULAR",
    "DOT_STR_3D",
    "DOT_STR_CUBE",
    "DEPENDENT_AR",
    "DEPENDENT_GB",
    "DOT_PRODUCT",
    "DOT_REFLECT_SPECULAR_CONST",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>",
    "<invalid>"
};

#endif


// Does this shader type require a texture bound at this
// stage?  
const bool CPixelShader::PSShaderUsesTexture[] = {
    FALSE,   // D3DSIO_TEXCOORD
    FALSE,   // D3DSIO_TEXKILL
    TRUE,    // D3DSIO_TEX
    TRUE,    // D3DSIO_TEXBEM
    TRUE,    // D3DSIO_TEXBEML
    TRUE,    // D3DSIO_TEXREG2AR
    TRUE,    // D3DSIO_TEXREG2GB
    FALSE,   // D3DSIO_TEXM3x2PAD
    TRUE,    // D3DSIO_TEXM3x2TEX
    FALSE,   // D3DSIO_TEXM3x3PAD
    TRUE,    // D3DSIO_TEXM3x3TEX
    TRUE,    // D3DSIO_TEXM3x3DIFF
    TRUE,    // D3DSIO_TEXM3x3SPEC
    TRUE     // D3DSIO_TEXM3x3VSPEC
};

const bool CPixelShader::PSIsProjective[] = {
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PROGRAM_NONE
    TRUE,  // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_2D_PROJECTIVE
    TRUE,  // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_3D_PROJECTIVE
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CUBE_MAP
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP_LUMINANCE
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BRDF
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ST
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ZW
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_REFLECT_DIFFUSE
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_3D
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_CUBE
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_AR
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_GB
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT
    FALSE, // NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR_CONST
};

// Kelvin shaders are a function of both the type of shader and
// the texture bound at that stage.  For example, the ordinary "tex"
// shader has different constants for null, 2D, 3D, and cube map textures.  
// Other shaders are similar, but some combinations don't make sense
// (like BEM into a cube map)
const int CPixelShader::PSD3DTexToNVShader[][4] = {
    {
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH,               // D3DSIO_TEXCOORD
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH
    }, {
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE,                 // D3DSIO_TEXKILL
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE
    }, {
        ~0,                                                               // D3DSIO_TEX
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_2D_PROJECTIVE,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_3D_PROJECTIVE,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CUBE_MAP,
    }, {
        ~0,                                                               // D3DSIO_TEXBEM
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP,
        ~0,
        ~0
    }, {
        ~0,                                                               // D3DSIO_TEXBEML
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP_LUMINANCE,
        ~0,
        ~0
    }, {
        ~0,                                                               // D3DSIO_TEXREG2AR
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_AR,
        ~0,
        ~0
    }, {
        ~0,                                                               // D3DSIO_TEXREG2GB
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_GB,
        ~0,
        ~0
    }, {
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT,                // D3DSIO_TEXM3x2PAD
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT
    }, {
        ~0,                                                               // D3DSIO_TEXM3x2TEX
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ST,
        ~0,
        ~0
    }, {
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT,                // D3DSIO_TEXM3x3PAD:
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT
    }, {
        ~0,                                                               // D3DSIO_TEXM3x3TEX
        ~0,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_3D,                 
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_CUBE
    }, {
        ~0,                                                               // D3DSIO_TEXM3x3DIFF
        ~0,
        ~0,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_REFLECT_DIFFUSE
    }, {
        ~0,                                                               // D3DSIO_TEXM3x3SPEC
        ~0,
        ~0,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR_CONST
    }, {
        ~0,                                                               // D3DSIO_TEXM3x3VSPEC
        ~0,
        ~0,
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR
    }
};

const DWORD CPixelShader::PSNumSrcRegs[] = {
     0,                                 // D3DSIO_NOP
     1,                                 // D3DSIO_MOV
     2,                                 // D3DSIO_ADD
     2,                                 // D3DSIO_SUB
     3,                                 // D3DSIO_MAD
     2,                                 // D3DSIO_MUL
    -1, -1,                             // Undefined for pixel shaders
     2,                                 // D3DSIO_DP3
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
     3,                                 // D3DSIO_LRP
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1,                     // Undefined for pixel shaders
     4,                                 // D3DSIO_NV_MMA
     4,                                 // D3DSIO_NV_DD  
     4,                                 // D3DSIO_NV_MD  
     4,                                 // D3DSIO_NV_MM  
     7,                                 // D3DSIO_NV_SPF 
     0,                                 // D3DSIO_TEXCOORD:
     0,                                 // D3DSIO_TEXKILL:
     0,                                 // D3DSIO_TEX:
     1,                                 // D3DSIO_TEXBEM:
     1,                                 // D3DSIO_TEXBEML:
     0,                                 // D3DSIO_TEXREG2GR:
     0,                                 // D3DSIO_TEXREG2AB:
     1,                                 // D3DSIO_TEXM3x2PAD:
     1,                                 // D3DSIO_TEXM3x2TEX:
     1,                                 // D3DSIO_TEXM3x3PAD:
     1,                                 // D3DSIO_TEXM3x3TEX:
     1,                                 // D3DSIO_TEXM3x3DIFF:
     2,                                 // D3DSIO_TEXM3x3SPEC:
     1,                                 // D3DSIO_TEXM3x3VSPEC:
     -1, -1,                            // Undefined for pixel shaders
     3,                                 // D3DSIO_CND
};


const DWORD CPixelShader::PSNumDstRegs[] = {
     0,                                 // D3DSIO_NOP
     1,                                 // D3DSIO_MOV
     1,                                 // D3DSIO_ADD
     1,                                 // D3DSIO_SUB
     1,                                 // D3DSIO_MAD
     1,                                 // D3DSIO_MUL
    -1, -1,                             // Undefined for pixel shaders
     1,                                 // D3DSIO_DP3
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
     1,                                 // D3DSIO_LRP
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1, -1, -1, -1, -1, -1, // Undefined for pixel shaders
    -1, -1, -1, -1,                     // Undefined for pixel shaders
     1,                                 // D3DSIO_NV_MMA
     2,                                 // D3DSIO_NV_DD  
     2,                                 // D3DSIO_NV_MD  
     2,                                 // D3DSIO_NV_MM  
     1,                                 // D3DSIO_NV_SPF 
     1,                                 // D3DSIO_TEXCOORD:
     1,                                 // D3DSIO_TEXKILL:
     1,                                 // D3DSIO_TEX:
     1,                                 // D3DSIO_TEXBEM:
     1,                                 // D3DSIO_TEXBEML:
     1,                                 // D3DSIO_TEXREG2GR:
     1,                                 // D3DSIO_TEXREG2AB:
     1,                                 // D3DSIO_TEXM3x2PAD:
     1,                                 // D3DSIO_TEXM3x2TEX:
     1,                                 // D3DSIO_TEXM3x3PAD:
     1,                                 // D3DSIO_TEXM3x3TEX:
     1,                                 // D3DSIO_TEXM3x3DIFF:
     1,                                 // D3DSIO_TEXM3x3SPEC:
     1,                                 // D3DSIO_TEXM3x3VSPEC:
    -1, -1,                             // Undefined for pixel shaders
     1,                                 // D3DSIO_CND
};

void (CPixelShader::* const CPixelShader::PSInstructionLUT[])(PSHAD_INSTRUCTION_ARGS) = {
    0,                               // D3DSIO_NOP
    CPixelShader::InstructionMOV,    // D3DSIO_MOV
    CPixelShader::InstructionADD,    // D3DSIO_ADD
    CPixelShader::InstructionSUB,    // D3DSIO_SUB
    CPixelShader::InstructionMAD,    // D3DSIO_MAD
    CPixelShader::InstructionMUL,    // D3DSIO_MUL
    0, 0,                            // Undefined for pixel shaders
    CPixelShader::InstructionDP3,    // D3DSIO_DP3
    0, 0, 0, 0, 0, 0, 0, 0, 0,       // Undefined for pixel shaders
    CPixelShader::InstructionLRP,    // D3DSIO_LRP
    0, 0, 0, 0, 0, 0, 0, 0, 0,       // Undefined for pixel shaders
    0, 0, 0, 0, 0, 0, 0, 0, 0,       // Undefined for pixel shaders
    0, 0, 0, 0, 0, 0, 0, 0, 0,       // Undefined for pixel shaders
    0, 0, 0, 0, 0, 0, 0, 0, 0,       // Undefined for pixel shaders
    0, 0, 0, 0,                      // Undefined for pixel shaders
    CPixelShader::InstructionNV_MMA, // D3DSIO_NV_MMA
    CPixelShader::InstructionNV_DD,  // D3DSIO_NV_DD
    CPixelShader::InstructionNV_MD,  // D3DSIO_NV_MD
    CPixelShader::InstructionNV_MM,  // D3DSIO_NV_MM
    CPixelShader::InstructionNV_SPF, // D3DSIO_NV_SPF
    0, 0, 0, 0, 0, 0, 0, 0, 0,       // Undefined for pixel shaders
    0, 0, 0, 0, 0, 0, 0,             // Undefined for pixel shaders
    CPixelShader::InstructionCND,    // D3DSIO_CND
};

////////////////////////////////////////////////////////////////

#define SetCombinerInput(stage, color, var, mapping, alpha, src) \
    do { \
        m_cw[stage][color][PSHAD_ICW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_ICW_##var##_MAP); \
        m_cw[stage][color][PSHAD_ICW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_ICW_##var##_ALPHA); \
        m_cw[stage][color][PSHAD_ICW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_ICW_##var##_SOURCE); \
        m_cw[stage][color][PSHAD_ICW] |= DRF_NUM(097, _SET_COMBINER_COLOR_ICW, _##var##_MAP,    mapping) \
                                      |  DRF_NUM(097, _SET_COMBINER_COLOR_ICW, _##var##_ALPHA,  alpha ? 1 : 0) \
                                      |  DRF_NUM(097, _SET_COMBINER_COLOR_ICW, _##var##_SOURCE, src); \
    } while (0);

// Partial OCW setup for the multiply terms.  Inputs are side (AB or CD), destination register, and
// whether a dot product is performed
#define SetCombinerOutput(stage, color, side, dst, dot, bluetoalpha) \
    do { \
        m_cw[stage][color][PSHAD_OCW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_OCW_##side##_DST); \
        m_cw[stage][color][PSHAD_OCW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_OCW_##side##_DOT_ENABLE); \
        m_cw[stage][color][PSHAD_OCW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_##side##); \
        m_cw[stage][color][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _##side##_DST, dst); \
        m_cw[stage][color][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _##side##_DOT_ENABLE, dot ? 1 : 0); \
        m_cw[stage][color][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _BLUETOALPHA_##side##, bluetoalpha ? 1 : 0); \
    } while (0);


// Sets the global shift/bias settings in the OCW for this stage
#define SetCombinerOutputShiftSat(stage, color, shift, sat) \
    do { \
        m_cw[stage][color][PSHAD_OCW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_OCW_OP); \
        m_cw[stage][color][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _OP, (shift << 1)); /* need saturation support */\
    } while (0);


#define SetCombinerSumOutput(stage, color, dst, mux) \
    do { \
        m_cw[stage][color][PSHAD_OCW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE); \
        m_cw[stage][color][PSHAD_OCW] &= ~DRF_SHIFTMASK(NV097_SET_COMBINER_COLOR_OCW_SUM_DST); \
        m_cw[stage][color][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _MUX_ENABLE, mux ? 1 : 0); \
        m_cw[stage][color][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _SUM_DST, dst); \
    } while (0);

///////////////////////////////////////////////////////////

void CPixelShader::InstructionNOP(PSHAD_INSTRUCTION_ARGS) {
    SetCombinerInput(stage, color, A, NV_MAPPING_SIGNED_IDENTITY, FALSE, NV_REG_ZERO);
    SetCombinerInput(stage, color, B, NV_MAPPING_SIGNED_IDENTITY, FALSE, NV_REG_ZERO);
    SetCombinerInput(stage, color, C, NV_MAPPING_SIGNED_IDENTITY, FALSE, NV_REG_ZERO);
    SetCombinerInput(stage, color, D, NV_MAPPING_SIGNED_IDENTITY, FALSE, NV_REG_ZERO);

    SetCombinerOutput(stage, color, AB, NV_REG_ZERO, FALSE, FALSE);
    SetCombinerOutput(stage, color, CD, NV_REG_ZERO, FALSE, FALSE);
    SetCombinerSumOutput(stage, color, NV_REG_ZERO, FALSE);

    SetCombinerOutputShiftSat(stage, color, 0, 0);
}

void CPixelShader::InstructionMOV(PSHAD_INSTRUCTION_ARGS) {
    // O = A*1
    SetCombinerInput(stage, color, A, map[0],                     alpha[0], src[0]);
    SetCombinerInput(stage, color, B, NV_MAPPING_UNSIGNED_INVERT, FALSE,    NV_REG_ZERO);
    SetCombinerOutput(stage, color, AB, dst[0], FALSE, FALSE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionMUL(PSHAD_INSTRUCTION_ARGS) {
    // O = A*B
    SetCombinerInput(stage, color, A, map[0], alpha[0], src[0]);
    SetCombinerInput(stage, color, B, map[1], alpha[1], src[1]);
    SetCombinerOutput(stage, color, AB, dst[0], FALSE, FALSE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionDP3(PSHAD_INSTRUCTION_ARGS) {
    if (color == PSHAD_COLOR) {
        // O = dot(A, B)

        // DP3 only works on the color side.  We make sure to replicate alpha later
        SetCombinerInput(stage, color, A, map[0], alpha[0], src[0]);
        SetCombinerInput(stage, color, B, map[1], alpha[1], src[1]);
        SetCombinerOutput(stage, color, AB, dst[0], TRUE, FALSE);

        SetCombinerOutputShiftSat(stage, color, shift, sat);
    } else {
        // Here, we replicate blue to alpha on the color side if the program requests the DP3
        // to go to rgba.  It's something of a hack, but it's the only instruction where we
        // have to do this.
        m_cw[stage][PSHAD_COLOR][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _BLUETOALPHA_AB, TRUE);
        
        // We don't care about what actually happens on the alpha side, since the alpha
        // replication overwrites it anyway.
        InstructionNOP(stage, color, shift, sat, dst, src, alpha, map);
    }
}

void CPixelShader::InstructionADD(PSHAD_INSTRUCTION_ARGS) {
    // O = A*1 + C*1
    SetCombinerInput(stage, color, A, map[0],                     alpha[0], src[0]);
    SetCombinerInput(stage, color, B, NV_MAPPING_UNSIGNED_INVERT, FALSE,    NV_REG_ZERO);
    SetCombinerInput(stage, color, C, map[1],                     alpha[1], src[1]);
    SetCombinerInput(stage, color, D, NV_MAPPING_UNSIGNED_INVERT, FALSE,    NV_REG_ZERO);

    SetCombinerSumOutput(stage, color, dst[0], FALSE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionSUB(PSHAD_INSTRUCTION_ARGS) {
    // O = A*1 + C*-1
    SetCombinerInput(stage, color, A, map[0],                     alpha[0], src[0]);
    SetCombinerInput(stage, color, B, NV_MAPPING_UNSIGNED_INVERT, FALSE,    NV_REG_ZERO); // map zero to 1
    SetCombinerInput(stage, color, C, map[1],                     alpha[1], src[1]);
    SetCombinerInput(stage, color, D, NV_MAPPING_EXPAND_NORMAL,   FALSE,    NV_REG_ZERO); // map zero to -1

    SetCombinerSumOutput(stage, color, dst[0], FALSE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionMAD(PSHAD_INSTRUCTION_ARGS) {
    // O = A*B + C*1
    SetCombinerInput(stage, color, A, map[0],                     alpha[0], src[0]);
    SetCombinerInput(stage, color, B, map[1],                     alpha[1], src[1]);
    SetCombinerInput(stage, color, C, map[2],                     alpha[2], src[2]);
    SetCombinerInput(stage, color, D, NV_MAPPING_UNSIGNED_INVERT, FALSE,    NV_REG_ZERO);

    SetCombinerSumOutput(stage, color, dst[0], FALSE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionLRP(PSHAD_INSTRUCTION_ARGS) {
    // O = A*B + (1-C)*D
    SetCombinerInput(stage, color, A, map[0],                     alpha[0], src[0]);
    SetCombinerInput(stage, color, B, map[1],                     alpha[1], src[1]);
    SetCombinerInput(stage, color, C, NVMapToNVMapInvert[map[0]], alpha[0], src[0]);
    SetCombinerInput(stage, color, D, map[2],                     alpha[2], src[2]);

    SetCombinerSumOutput(stage, color, dst[0], FALSE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionCND(PSHAD_INSTRUCTION_ARGS) {
    // O = r0 ? A*1 : C*1
    SetCombinerInput(stage, color, A, map[2],                     alpha[2], src[2]);
    SetCombinerInput(stage, color, B, NV_MAPPING_UNSIGNED_INVERT, FALSE,    NV_REG_ZERO);
    SetCombinerInput(stage, color, C, map[1],                     alpha[1], src[1]);
    SetCombinerInput(stage, color, D, NV_MAPPING_UNSIGNED_INVERT, FALSE,    NV_REG_ZERO);

    SetCombinerSumOutput(stage, color, dst[0], TRUE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionNV_MMA(PSHAD_INSTRUCTION_ARGS) {
	// O = A*B + C*D
    SetCombinerInput(stage, color, A, map[0], alpha[0], src[0]);
    SetCombinerInput(stage, color, B, map[1], alpha[1], src[1]);
    SetCombinerInput(stage, color, C, map[2], alpha[2], src[2]);
    SetCombinerInput(stage, color, D, map[3], alpha[3], src[3]);

    SetCombinerSumOutput(stage, color, dst[0], FALSE);

    SetCombinerOutputShiftSat(stage, color, shift, sat);
}

void CPixelShader::InstructionNV_DD (PSHAD_INSTRUCTION_ARGS) {
	// O0 = dot(A, B)
	// O1 = dot(C, D)
	// see DP3 instruction comments for more details
    if (color == PSHAD_COLOR) {
        SetCombinerInput(stage, color, A, map[0], alpha[0], src[0]);
        SetCombinerInput(stage, color, B, map[1], alpha[1], src[1]);
        SetCombinerInput(stage, color, C, map[2], alpha[2], src[2]);
        SetCombinerInput(stage, color, D, map[3], alpha[3], src[3]);

        SetCombinerOutput(stage, color, AB, dst[0], TRUE, FALSE);
        SetCombinerOutput(stage, color, CD, dst[1], TRUE, FALSE);
    } else {
        m_cw[stage][PSHAD_COLOR][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _BLUETOALPHA_AB, TRUE);
        m_cw[stage][PSHAD_COLOR][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _BLUETOALPHA_CD, TRUE);
    }
}

void CPixelShader::InstructionNV_MD (PSHAD_INSTRUCTION_ARGS) {
	// O0 = A*B
	// O1 = dot(C, D)
    SetCombinerInput(stage, color, A, map[0], alpha[0], src[0]);
    SetCombinerInput(stage, color, B, map[1], alpha[1], src[1]);

    SetCombinerOutput(stage, color, AB, dst[0], FALSE, FALSE);

    if (color == PSHAD_COLOR) {
        SetCombinerInput(stage, color, C, map[2], alpha[2], src[2]);
        SetCombinerInput(stage, color, D, map[3], alpha[3], src[3]);
        SetCombinerOutput(stage, color, CD, dst[1], TRUE, FALSE);
    } else {
        m_cw[stage][PSHAD_COLOR][PSHAD_OCW] |= DRF_NUM(097, _SET_COMBINER_COLOR_OCW, _BLUETOALPHA_CD, TRUE);
    }
}

void CPixelShader::InstructionNV_MM (PSHAD_INSTRUCTION_ARGS) {
	// O0 = A*B
	// O1 = C*D
    SetCombinerInput(stage, color, A, map[0], alpha[0], src[0]);
    SetCombinerInput(stage, color, B, map[1], alpha[1], src[1]);
    SetCombinerInput(stage, color, C, map[2], alpha[2], src[2]);
    SetCombinerInput(stage, color, D, map[3], alpha[3], src[3]);

    SetCombinerOutput(stage, color, AB, dst[0], TRUE, FALSE);
    SetCombinerOutput(stage, color, CD, dst[1], TRUE, FALSE);
}

void CPixelShader::InstructionNV_SPF(PSHAD_INSTRUCTION_ARGS) {
	// r4 = E*F
	// O.rgb = A*B + (1-A)*C + D
	// O.a = G
}




/////////////////////////////////////////////////////////////

// In Celsius, we remap the texture registers, such that programs can access the
// textures defined in any stage, and to fight the stage compaction that Celsius
// devices perform.
// We don't do this in Kelvin, because the stages (and the shader programs) map
// directly to the hardware, so it would be very difficult to perform the remapping.
// There is also no problem with stage compaction.
void CPixelShader::GetTextureRegMapping(DWORD *newoffset, DWORD offset) {
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        *newoffset = offset;
    } else {
        if (m_celsiusTexStageMapping[offset] == PSHAD_UNUSED) {
            m_celsiusTexStageMapping[offset] = m_texturesUsed;
            m_texturesUsed++;
        }
        *newoffset = m_texturesUsed - 1;
    }
}

// Decode the destination register token
DWORD CPixelShader::DSTdecode(
    DWORD *dst,
    DWORD *shift,
    DWORD *sat,
    DWORD *mask,
    DWORD op
)
{
    DWORD type = (op & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT;
    DWORD offset = op & D3DSP_REGNUM_MASK;

    if ((op & D3DSP_REGTYPE_MASK) == D3DSPR_TEXTURE) {
        GetTextureRegMapping(&offset, offset);
    }

    *dst = PSTypeOffsetToCombinerReg[type][offset];
    *shift = ((op & D3DSP_DSTSHIFT_MASK) >> D3DSP_DSTSHIFT_SHIFT) & 0x3;
    *sat = (op & D3DSP_DSTMOD_MASK) == D3DSPDM_SATURATE;
    *mask = op & D3DSP_WRITEMASK_ALL;

    return TRUE;
}

// Decode the source register token
DWORD CPixelShader::SRCdecode(
    DWORD *src,
    DWORD *rgbAlpha,
    DWORD *alphaAlpha,
    DWORD *map,
    DWORD op,
    DWORD mask,
    BOOL  bSignedConst
)
{
    DWORD type = (op & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT; // const reg, texture reg, etc
    DWORD regnum = op & D3DSP_REGNUM_MASK;   // which reg number within a type set is it?
    DWORD offset = regnum;                   // save this, we may have to remap it later
    DWORD conststage = (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ? m_dwStage : 0;
    DWORD swiz = (op & D3DSP_SWIZZLE_MASK) >> D3DSP_SWIZZLE_SHIFT;

    *map = PSD3DModToNVMap[(op & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT];
    nvAssert(*map != ~0);

    if ((op & D3DSP_REGTYPE_MASK) == D3DSPR_CONST) {
        if (regnum >= PSHAD_MAX_CONSTANTS) {
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "Constant number too high (c%d)", regnum);
            return FALSE;
        }

        offset = AllocateConstRegister(m_dwStage, regnum, bSignedConst, *map);

        // We don't apply normal source mapping to const values, since the const registers can only
        // hold unsigned values, and we can't perform a double remapping when we need to hold
        // signed values.  Hence, we perform the remapping offline, and always use one of two
        // mapping modes, depending on whether the final result holds signed values.  
        // Two things to note:
        // - Sometimes we *can't* store signed values, such as when a constant is used as the first
        // argument to LRP (where we need to perform a complement operation).  Fortunately, the runtime
        // makes this case illegal (or at least it's supposed to).
        // - This mapping value may be changed later
        *map = bSignedConst ? NV_MAPPING_EXPAND_NORMAL : NV_MAPPING_UNSIGNED_IDENTITY;


    } else if ((op & D3DSP_REGTYPE_MASK) == D3DSPR_TEXTURE) {
        GetTextureRegMapping(&offset, offset);
    }

    *src = PSTypeOffsetToCombinerReg[type][offset];

    swiz = ((mask & D3DSP_WRITEMASK_RGB) ? (swiz & 0x3f) : 0x24) | ((mask & D3DSP_WRITEMASK_A) ? (swiz & 0xc0) : 0xc0);

    switch (swiz) {
    case 0xff:   // aaaa
        *rgbAlpha = 1;
        *alphaAlpha = 1;
        break;
    case 0xa4:   // rgbb
        *rgbAlpha = 0;
        *alphaAlpha = 0;
        break;
    case 0xbf:   // aaab
        *rgbAlpha = 1;
        *alphaAlpha = 0;
        break;
    default:
        // should always have one of these 4 cases
        nvAssert(0);
    case 0xe4:   // rgba
        *rgbAlpha = 0;
        *alphaAlpha = 1;
        break;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////

#ifdef DEBUG

void CPixelShader::DBGPrintInstruction(DWORD op, DWORD dstop, DWORD *srcop) {
    char str[256], str2[256];
    DWORD opcode;
    DWORD comma = 0;
    DWORD mod, neg, bias, scale;

    opcode = op & D3DSI_OPCODE_MASK;
    strcpy(str, (op & D3DSI_COISSUE) ? "+" : "");
    strcat(str, PSInstructionStrings[opcode]);


    if (PSNumDstRegs[opcode]) {
        switch ((dstop & D3DSP_DSTSHIFT_MASK) >> D3DSP_DSTSHIFT_SHIFT) {
        case 0x0:
            break;
        case 0x1:
            strcat(str, "_x2");
            break;
        case 0x2:
            strcat(str, "_x4");
            break;
        case 0xf:
            strcat(str, "_d2");
            break;
        default:
            // Bad program!
            nvAssert(0);
        }

        if ((dstop & D3DSP_DSTMOD_MASK) == D3DSPDM_SATURATE) strcat(str, "_sat");

        sprintf(str2, " %s%d%s", 
            PSRegTypeToPrefix[(dstop & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT],
            dstop & D3DSP_REGNUM_MASK,
            PSWriteMask[(dstop & D3DSP_WRITEMASK_ALL) >> D3DSP_WRITEMASK_SHIFT]);
        strcat(str, str2);

        comma = 1;
    }

    for (DWORD i=0; i<PSNumSrcRegs[opcode]; i++) {
        mod = srcop[i] & D3DSP_SRCMOD_MASK;
        neg = (mod == D3DSPSM_NEG) || (mod == D3DSPSM_BIASNEG) || (mod == D3DSPSM_SIGNNEG) || (mod == D3DSPSM_COMP);
        bias = (mod == D3DSPSM_BIAS) || (mod == D3DSPSM_BIASNEG);
        scale = (mod == D3DSPSM_SIGN) || (mod == D3DSPSM_SIGNNEG);

        sprintf(str2, "%s %s%s%s%d%s%s",
            comma ? "," : "",
            (mod == D3DSPSM_COMP) ? "1" : "",
            neg ? "-" : "",
            PSRegTypeToPrefix[(srcop[i] & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT],
            srcop[i] & D3DSP_REGNUM_MASK,
            bias ? "_bias" : "",
            scale ? "_bx2" : "");
        strcat(str, str2);

        comma = 1;
    }

    DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, str);
}
#endif

///////////////////////////////////////////////////////////////////////

CPixelShader::create
(
    PNVD3DCONTEXT pContext,
    DWORD dwHandle,
    DWORD dwCodeSize,
    DWORD *lpCode
)
{
    DWORD i;
    DWORD dstop, srcop[3];

    // An instruction wants it's output saturated on this register.  
    // Unfortunately, we can only saturate on input
    bool bSaturateOutput[NV_REG_MAX][2]; 
                              
#ifdef DEBUG
    char str[256];
#endif

    m_dwCodeData = (DWORD *)AllocIPM (dwCodeSize);
    m_dwCodeSize = dwCodeSize;
    m_dwHandle = dwHandle;
    memcpy (m_dwCodeData,lpCode,dwCodeSize); //not really needed, but nice for debug

    DWORD maxcombiners = (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ? 8 : 2;

    for (i=0; i<PSHAD_MAX_COMBINER_STAGES; i++) {
        m_cw[i][0][0] = 0;
        m_cw[i][0][1] = 0;
        m_cw[i][1][0] = 0;
        m_cw[i][1][1] = 0;
    }

    for (i=0; i<PSHAD_MAX_CONSTANTS; i++) {
        m_dwPixelShaderConstFlags[i] = 0;
    }

    for (i=0; i<PSHAD_MAX_SHADER_STAGES; i++) {
        m_celsiusTexStageMapping[i] = PSHAD_UNUSED;
        m_shaderStageInput[i] = PSHAD_UNUSED;
        m_shaderStageInputInverse[i] = PSHAD_UNUSED;
        m_shaderStageProgram[i] = PSHAD_UNUSED;
    }

    for (i=0; i<NV_REG_MAX; i++) {
        bSaturateOutput[i][PSHAD_COLOR] = false;
        bSaturateOutput[i][PSHAD_ALPHA] = false;
    }

    m_texturesUsed = 0;
    m_textureStageSwap = 0;

    DWORD op, lastMask;
    DWORD opcode, shift, sat, mask;

    DWORD dst[PSHAD_MAX_DST_ARGS_PER_INSTRUCTION];
    DWORD lastDst[PSHAD_MAX_DST_ARGS_PER_INSTRUCTION];

    DWORD src[PSHAD_MAX_SRC_ARGS_PER_INSTRUCTION];
    DWORD rgbAlpha[PSHAD_MAX_SRC_ARGS_PER_INSTRUCTION];
    DWORD alphaAlpha[PSHAD_MAX_SRC_ARGS_PER_INSTRUCTION];
    DWORD map[PSHAD_MAX_SRC_ARGS_PER_INSTRUCTION];

    DWORD size, dstoffset, srcoffset, srcmap;

    DWORD version = *lpCode++;
    nvAssert((version & 0xffff0000) == 0xffff0000);

    m_dwStage = 0;
    m_dwTextureStage = 0;

    DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "Pixel shader creation: handle=%d", dwHandle);
    DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "ps. %d.%d", (version & 0xff00) >> 8, version & 0xff);

    for (i=0; i<dwCodeSize; i+=sizeof(DWORD)) {
        op = *lpCode++;
        opcode = op & D3DSI_OPCODE_MASK;

        if (m_dwStage >= maxcombiners) break; // too many instructions

        if (opcode == D3DSIO_END) {
            // End of tokens
            DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "end");
            break;
        } else if (opcode == D3DSIO_NOP) {
            // Nothing
            DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "nop");
        } else if (opcode == D3DSIO_COMMENT) {
            // Comment instruction...  or unconditional jump forward...
            size = (op & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
            if ((*lpCode == MAKEFOURCC('N', 'V', 'D', 'A')) &&
                (size == 49)) {
                // Load raw combiner state
            }
#ifdef DEBUG
            memset(str, 0, 256);
            strncpy(str, (const char *)lpCode, 4*size);
            DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "comment: %s", str);
#endif
            lpCode += size; // Skip past comment DWORDS.  There's sometimes a message string in here.
        } else if (PSInstructionType[opcode] == PSHAD_INS) {
            // It's a standard instruction

            if (op & D3DSI_COISSUE) {
                // They are issuing a color op and an alpha op in parallel.
                // Decrease stage count to match last instruction and check consistency.
                m_dwStage--; 
            }

            dstop = *lpCode++;
            for (i=0; i<PSNumDstRegs[opcode]; i++) {
	            if (!DSTdecode(&dst[i], &shift, &sat, &mask, dstop)) {
					nvAssert(0);
					return FALSE;
				}
			}

            nvAssert(PSNumSrcRegs[opcode] >= 0);
            for (i=0; i<PSNumSrcRegs[opcode]; i++) {
                srcop[i] = *lpCode++;
                DWORD signedconst = !((opcode == D3DSIO_LRP) && (i == 0));
                if (!SRCdecode(&src[i], &rgbAlpha[i], &alphaAlpha[i], &map[i], srcop[i], mask, signedconst)) {
                    nvAssert(0);
                    return FALSE;
                }

                bool bSatSrc;

                if (mask & D3DSP_WRITEMASK_RGB) {
                    bSatSrc = bSaturateOutput[src[i]][PSHAD_COLOR];
                } else if (mask & D3DSP_WRITEMASK_A) {
                    bSatSrc = bSaturateOutput[src[i]][PSHAD_ALPHA];
                }

                if (bSatSrc) {
                    if (map[i] == NV_MAPPING_SIGNED_IDENTITY) {
                        // Clamp input
                        map[i] = NV_MAPPING_UNSIGNED_IDENTITY;
                    } else if ((map[i] == NV_MAPPING_UNSIGNED_IDENTITY) ||
                               (map[i] == NV_MAPPING_UNSIGNED_INVERT)) {
                        // Do nothing; these are already clamped
                    } else {
                        // The runtime chouldn't allow these cases, but just in case, assert...
                        nvAssert(0);
                    }
                }
            }

            nvAssert(PSInstructionLUT[opcode] != 0);

            if ((op & D3DSI_COISSUE) && ((lastMask ^ mask) != D3DSP_WRITEMASK_RGBA)) {
                // They tried one of the following combinations: rgba/rgba, rgba/rgb, rgba/a, rgb/rgb, a/a
                // The only combo we support is rgb/a
                nvAssert(0);
                return FALSE;
            }

            if (mask & D3DSP_WRITEMASK_RGB) {
                bSaturateOutput[dst[0]][PSHAD_COLOR] = sat ? true : false;
                (this->*PSInstructionLUT[opcode])(m_dwStage, PSHAD_COLOR, shift, sat, dst, src, rgbAlpha, map);
            }
            if (mask & D3DSP_WRITEMASK_A) {
                bSaturateOutput[dst[0]][PSHAD_ALPHA] = sat ? true : false;
                (this->*PSInstructionLUT[opcode])(m_dwStage, PSHAD_ALPHA, shift, sat, dst, src, alphaAlpha, map);
            }

#ifdef DEBUG
            DBGPrintInstruction(op, dstop, srcop);
#endif

            m_dwStage++;
        } else if (PSInstructionType[opcode] == PSHAD_TEX) {
            nvAssert(opcode >= D3DSIO_TEXCOORD);
            nvAssert(opcode <= D3DSIO_TEXM3x3VSPEC);

            // Get destination register offset
            if (PSNumDstRegs[opcode] == 1) {
                dstop = *lpCode++;
                dstoffset = dstop & D3DSP_REGNUM_MASK;
            }

            // Grab texture input stage
            if (PSNumSrcRegs[opcode] >= 1) {
                srcop[0] = *lpCode++;
                srcoffset = srcop[0] & D3DSP_REGNUM_MASK;
                srcmap = srcop[0] & D3DSP_SRCMOD_MASK;
            } else {
                srcoffset = PSHAD_UNUSED;
                srcmap = D3DSPSM_NONE;
            }

            nvAssert((srcmap == D3DSPSM_NONE) || (srcmap == D3DSPSM_SIGN));

            // Skip rest of src arguments, they are useless to us
            for (i=1; (int)i<PSNumSrcRegs[opcode]; i++) {
                srcop[i] = *lpCode++;
            }


            if (dstoffset >= PSHAD_MAX_SHADER_STAGES) {
                // Oops, they've exceeeded the max number of texture stages.  Break so they know they screwed up.
                break;
            }

            // In Kelvin, there's a pretty direct mapping between D3D texture shader stages
            // and the hardware stages.
            // In Celsius, we only support a single texture addressing operation, which is a
            // simple 2D/cube map sampling.  Because of this, we actually ignore all addressing
            // instructions, because they can't give us any additional information.  Maybe we
            // should check these anyway, simply for debugging purposes.

            // At some point, we should at least put in some quick error checking on the stage state, since currently,
            // we save setting up the stage state until later on (in setKelvinState).
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                m_shaderStageProgram[dstoffset] = opcode - D3DSIO_TEXCOORD;
                m_shaderStageInput[dstoffset] = srcoffset;
                m_shaderStageInputMapping[dstoffset] = srcmap;
                m_dwTextureStage++;
            }

#ifdef DEBUG
            srcoffset = (srcoffset == PSHAD_UNUSED) ? 0 : srcoffset; // Fix up srcoffset after the fact, so it doesn't page fault things
            DBGPrintInstruction(op, dstop, srcop);
#endif
        } else if (PSInstructionType[opcode] == PSHAD_DEF) {
            // Accept a constant color definition
            DWORD dwIndex = (*lpCode++) & 0xffff;

            m_dwPixelShaderConstFlags[dwIndex] |= PSHAD_CONST_LOCAL;

            D3DCOLORVALUE c;
            c.r = *(D3DVALUE *)&(*lpCode++);
            c.g = *(D3DVALUE *)&(*lpCode++);
            c.b = *(D3DVALUE *)&(*lpCode++);
            c.a = *(D3DVALUE *)&(*lpCode++);

            m_pixelShaderConsts[dwIndex] = c;

            DPF_LEVEL(NVDBG_LEVEL_PIXEL_SHADER, "def c%d, %f, %f, %f, %f", dwIndex, c.a, c.r, c.g, c.b);
        } else {
            // Invalid opcode
            nvAssert(0);
        }

        lastMask = mask; // save for checking coissue ops
		for (i=0; i<PSHAD_MAX_DST_ARGS_PER_INSTRUCTION; i++) lastDst[i] = dst[i]; // for dependency checking
    }

    if (m_dwStage == 0) {
        // Make sure we have a valid combiner setup
        InstructionNOP(0, PSHAD_COLOR, 0, 0, 0, 0, 0, 0);
        InstructionNOP(0, PSHAD_ALPHA, 0, 0, 0, 0, 0, 0);
        m_dwStage = 1;
    }

    for (i=0; i<PSHAD_MAX_SHADER_STAGES; i++) {
        if (m_shaderStageInput[i] != PSHAD_UNUSED) {
            m_shaderStageInputInverse[m_shaderStageInput[i]] = i;
        } else {
            m_shaderStageInput[i] = 0;
        }
    }

    // Missing end opcode
    if (opcode != D3DSIO_END) {
        nvAssert(0);
        return FALSE;
    }


    return TRUE;
}

CPixelShader::~CPixelShader() {
    if (m_dwCodeData) FreeIPM(m_dwCodeData);
}

//////////////////////////////////////////////////////////////////////////

DWORD CPixelShader::AllocateConstRegister(DWORD dwStage, DWORD dwRegNum, BOOL bSigned, DWORD dwMap) {
    if ((m_dwConstMapping[dwStage][0].dwRegNum == dwRegNum) &&
        (m_dwConstMapping[dwStage][0].dwMap == dwMap) &&
        (m_dwConstMapping[dwStage][0].bSigned == bSigned)) {
        return 0;
    } else if ((m_dwConstMapping[dwStage][1].dwRegNum == dwRegNum) &&
               (m_dwConstMapping[dwStage][1].dwMap == dwMap) &&
               (m_dwConstMapping[dwStage][1].bSigned == bSigned)) {
        return 1;
    } else if (m_dwConstMapping[dwStage][0].dwRegNum == PSHAD_UNUSED) {
        m_dwConstMapping[dwStage][0].dwRegNum = dwRegNum;
        m_dwConstMapping[dwStage][0].dwMap = dwMap;
        m_dwConstMapping[dwStage][0].bSigned = bSigned;
        return 0;
    } else if (m_dwConstMapping[dwStage][1].dwRegNum == PSHAD_UNUSED) {
        m_dwConstMapping[dwStage][1].dwRegNum = dwRegNum;
        m_dwConstMapping[dwStage][1].dwMap = dwMap;
        m_dwConstMapping[dwStage][1].bSigned = bSigned;
        return 1;
    } else {
        // Return error code
        return 0xffffffff;
    }
}

#define Repeat4(f, a, r, g, b) do { f(a); f(r); f(g); f(b); } while(0)

__inline void Clamp0to1(D3DVALUE &x)    { x = (x < 0) ? 0 : (x > 1) ? 1 : x; }
__inline void ClampNeg1to1(D3DVALUE &x) { x = (x < -1) ? -1 : (x > 1) ? 1 : x; }
__inline void Complement(D3DVALUE &x)   { x = 1 - x; }
__inline void Expand(D3DVALUE &x)       { x = (x - 0.5f) * 2; }
__inline void Negate(D3DVALUE &x)       { x = -x; }
__inline void Bias(D3DVALUE &x)         { x = x - 0.5f; }

DWORD CPixelShader::ColorConvertWithMapping(D3DCOLORVALUE c, DWORD dwMod, BOOL *bSigned) {
    Repeat4(ClampNeg1to1, c.a, c.r, c.g, c.b);

    switch (dwMod) {
    case NV_MAPPING_UNSIGNED_IDENTITY:
        Repeat4(Clamp0to1, c.a, c.r, c.g, c.b);
        break;
    case NV_MAPPING_UNSIGNED_INVERT:
        Repeat4(Complement, c.a, c.r, c.g, c.b);
        Repeat4(Clamp0to1, c.a, c.r, c.g, c.b);
        break;
    case NV_MAPPING_EXPAND_NORMAL:
        Repeat4(Expand, c.a, c.r, c.g, c.b);
        Repeat4(ClampNeg1to1, c.a, c.r, c.g, c.b);
        break;
    case NV_MAPPING_EXPAND_NEGATE:
        Repeat4(Expand, c.a, c.r, c.g, c.b);
        Repeat4(Negate, c.a, c.r, c.g, c.b);
        Repeat4(ClampNeg1to1, c.a, c.r, c.g, c.b);
        break;
    case NV_MAPPING_HALF_BIAS_NORMAL:
        Repeat4(Bias, c.a, c.r, c.g, c.b);
        Repeat4(ClampNeg1to1, c.a, c.r, c.g, c.b);
        break;
    case NV_MAPPING_HALF_BIAS_NEGATE:
        Repeat4(Bias, c.a, c.r, c.g, c.b);
        Repeat4(Negate, c.a, c.r, c.g, c.b);
        Repeat4(ClampNeg1to1, c.a, c.r, c.g, c.b);
        break;
    case NV_MAPPING_SIGNED_IDENTITY:
        Repeat4(ClampNeg1to1, c.a, c.r, c.g, c.b);
        break;
    case NV_MAPPING_SIGNED_NEGATE:
        Repeat4(ClampNeg1to1, c.a, c.r, c.g, c.b);
        Repeat4(Negate, c.a, c.r, c.g, c.b);
        break;
    default:
        // Invalid mapping mode!
        nvAssert(0);
    }

    if (*bSigned) {
        return D3DCOLOR_ARGB(
            DWORD(c.a*127.0f + 128.0f),
            DWORD(c.r*127.0f + 128.0f),
            DWORD(c.g*127.0f + 128.0f),
            DWORD(c.b*127.0f + 128.0f));
    } else {
        return D3DCOLOR_ARGB(
            DWORD(c.a*255.0f),
            DWORD(c.r*255.0f),
            DWORD(c.g*255.0f),
            DWORD(c.b*255.0f));
    }
}

///////////////////////////////////////////////////////////////////////
static const char * PSTextureTypes[] = {"<null>", "2D", "3D", "cube map"};

DWORD CPixelShader::GetShaderProgram(PNVD3DCONTEXT pContext, DWORD dwStage) {
    DWORD dwType, dwProgram;

    DWORD dwTexture = pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP];
    CTexture *pTexture = dwTexture ? ((CNvObject *)dwTexture)->getTexture() : 0;

    dwType = (!pTexture) ? 0 : (pTexture->isCubeMap()) ? 3 : (pTexture->isVolumeMap()) ? 2 : 1;

    dwProgram = (m_shaderStageProgram[dwStage] == PSHAD_UNUSED) ? 
        NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE : 
        PSD3DTexToNVShader[m_shaderStageProgram[dwStage]][dwType];

    if (dwProgram == ~0) {
        DPF("Using program '%s', but texture is of type '%s' on stage %d", PSInstructionStrings[m_shaderStageProgram[dwStage]+D3DSIO_TEXCOORD], PSTextureTypes[dwType], dwStage);
        dbgD3DError();
        dwProgram = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PASS_THROUGH;
    }

    return dwProgram;
}


#if (NVARCH >= 0x020)
void CPixelShader::setKelvinState(PNVD3DCONTEXT pContext) {
    DWORD i, j;

    DWORD dwCombinerControl = 0;
    dwCombinerControl |= DRF_NUM(097, _SET_COMBINER_CONTROL, _ITERATION_COUNT, m_dwStage);
    dwCombinerControl |= DRF_DEF(097, _SET_COMBINER_CONTROL, _FACTOR0, _EACH_STAGE);
    dwCombinerControl |= DRF_DEF(097, _SET_COMBINER_CONTROL, _FACTOR1, _EACH_STAGE);
    dwCombinerControl |= DRF_DEF(097, _SET_COMBINER_CONTROL, _MUX_SELECT, _MSB);

    pContext->hwState.kelvin.set1(NV097_SET_COMBINER_CONTROL, dwCombinerControl);


    DWORD dwShaderProgram = 0;

    // Set up the shader programs based on the currently set textures
    // Unfortunately, there are different programs for different types of textures
    dwShaderProgram |= DRF_NUM(097, _SET_SHADER_STAGE_PROGRAM, _STAGE0, GetShaderProgram(pContext, 0));
    dwShaderProgram |= DRF_NUM(097, _SET_SHADER_STAGE_PROGRAM, _STAGE1, GetShaderProgram(pContext, 1));
    dwShaderProgram |= DRF_NUM(097, _SET_SHADER_STAGE_PROGRAM, _STAGE2, GetShaderProgram(pContext, 2));
    dwShaderProgram |= DRF_NUM(097, _SET_SHADER_STAGE_PROGRAM, _STAGE3, GetShaderProgram(pContext, 3));

    pContext->hwState.kelvin.set1(NV097_SET_SHADER_STAGE_PROGRAM, dwShaderProgram);

    // The RGB mapping for each stage is a function of the stage it uses as input, and the texture assigned to the unit.
    DWORD dwRGBMapping = 0;
    DWORD pTextureHandle;
    CTexture *pTexture;

    for (i=1; i<PSHAD_MAX_SHADER_STAGES; i++) {
        nvAssert(m_shaderStageInput[i] < PSHAD_MAX_SHADER_STAGES);

        pTextureHandle = pContext->tssState[m_shaderStageInput[i]].dwValue[D3DTSS_TEXTUREMAP];
        pTexture = pTextureHandle ? (((CNvObject *)pTextureHandle)->getTexture()) : 0;

//        nvAssert(!pTexture || (GetRGBMapping(pTexture) == NV097_SET_DOT_RGBMAPPING_STAGE1_ZERO_TO_1) || (m_shaderStageInputMapping[i] == D3DSPSM_NONE));

        dwRGBMapping |= (m_shaderStageInputMapping[i] == D3DSPSM_SIGN) ?
            (DRF_DEF(097, _SET_DOT_RGBMAPPING, _STAGE1, _MINUS_1_TO_1_MS) << (4*(i-1))) :
            pTexture ? 
                (DRF_NUM(097, _SET_DOT_RGBMAPPING, _STAGE1, GetRGBMapping(pTexture)) << (4*(i-1))) : 
                0;
    }

    pContext->hwState.kelvin.set1 (NV097_SET_DOT_RGBMAPPING, dwRGBMapping);


    // Set input stages
    pContext->hwState.kelvin.set1(NV097_SET_SHADER_OTHER_STAGE_INPUT,
                                 (DRF_NUM(097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE1, m_shaderStageInput[1]) |
                                  DRF_NUM(097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE2, m_shaderStageInput[2]) |
                                  DRF_NUM(097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE3, m_shaderStageInput[3])));

#define NV097_SET_COMBINER_FACTOR(stage, num) (NV097_SET_COMBINER_FACTOR0(stage) + ((NV097_SET_COMBINER_FACTOR1(0) - NV097_SET_COMBINER_FACTOR0(0)) * num))

    // Program the constants, based on the mapping table we generated
    for (i=0; i<PSHAD_MAX_COMBINER_STAGES; i++) {
        for (j=0; j<2; j++) {
            DWORD dwRegNum = m_dwConstMapping[i][j].dwRegNum;
            if (dwRegNum != PSHAD_UNUSED) {
                DWORD dwMap = m_dwConstMapping[i][j].dwMap;
                BOOL bSigned = m_dwConstMapping[i][j].bSigned;
                DWORD dwColor;

                if (m_dwPixelShaderConstFlags[dwRegNum] & PSHAD_CONST_LOCAL) {
                    dwColor = ColorConvertWithMapping(m_pixelShaderConsts[dwRegNum], dwMap, &bSigned);
                } else {
                    dwColor = ColorConvertWithMapping(pContext->pixelShaderConsts[dwRegNum], dwMap, &bSigned);
                }

                pContext->hwState.kelvin.set1(NV097_SET_COMBINER_FACTOR(i, j), dwColor);
            }
        }
    }


    // Set eye vector, which happens to be hard-coded to c0 in D3D
    D3DCOLORVALUE eye = 
        (m_dwPixelShaderConstFlags[0] & PSHAD_CONST_LOCAL) ?
        m_pixelShaderConsts[0] :
        pContext->pixelShaderConsts[0];

    pContext->hwState.kelvin.set3f(NV097_SET_EYE_VECTOR(0), eye.r, eye.g, eye.b);

    // Program up the combiners
    for (i=0; i<m_dwStage; i++) {
        pContext->hwState.kelvin.set1(NV097_SET_COMBINER_COLOR_ICW(i), m_cw[i][PSHAD_COLOR][PSHAD_ICW]);
        pContext->hwState.kelvin.set1(NV097_SET_COMBINER_COLOR_OCW(i), m_cw[i][PSHAD_COLOR][PSHAD_OCW]);
        pContext->hwState.kelvin.set1(NV097_SET_COMBINER_ALPHA_ICW(i), m_cw[i][PSHAD_ALPHA][PSHAD_ICW]);
        pContext->hwState.kelvin.set1(NV097_SET_COMBINER_ALPHA_OCW(i), m_cw[i][PSHAD_ALPHA][PSHAD_OCW]);
    }

    for (i=0; i<PSHAD_MAX_SHADER_STAGES; i++) {
        if (m_shaderStageProgram[i] == PSHAD_UNUSED) {
            pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_USERTEXCOORDSNEEDED(i);
            pContext->hwState.dwTexUnitToTexStageMapping[i] = KELVIN_UNUSED;
            pContext->hwState.dwTextureUnitStatus[i] = KELVIN_TEXUNITSTATUS_IDLE;
            pContext->hwState.pTextureInUse[i]     = NULL;
        } else {
            pContext->hwState.dwStateFlags |= KELVIN_FLAG_USERTEXCOORDSNEEDED(i);
            pContext->hwState.dwTexUnitToTexStageMapping[i] = i;
            pContext->hwState.dwTextureUnitStatus[i] = KELVIN_TEXUNITSTATUS_USER;

            if (!PSShaderUsesTexture[m_shaderStageProgram[i]]) {
                pContext->hwState.pTextureInUse[i]     = NULL;
            }
        }
    }

    // Optimize this later
    pContext->hwState.dwStateFlags |= KELVIN_FLAG_COMBINERSNEEDSPECULAR;
}
#endif


#if (NVARCH >= 0x010)
void CPixelShader::setCelsiusState(PNVD3DCONTEXT pContext) {

    pContext->hwState.celsius.set (NV056_SET_COMBINER_COLOR_ICW(0), m_cw[0][PSHAD_COLOR][PSHAD_ICW]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER0_COLOR_OCW,   m_cw[0][PSHAD_COLOR][PSHAD_OCW]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_ICW(0), m_cw[0][PSHAD_ALPHA][PSHAD_ICW]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_OCW(0), m_cw[0][PSHAD_ALPHA][PSHAD_OCW]);

    pContext->hwState.celsius.set (NV056_SET_COMBINER_COLOR_ICW(1), m_cw[1][PSHAD_COLOR][PSHAD_ICW]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER1_COLOR_OCW,   m_cw[1][PSHAD_COLOR][PSHAD_OCW] | DRF_NUM(056, _SET_COMBINER1_COLOR, _OCW_ITERATION_COUNT, m_dwStage));
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_ICW(1), m_cw[1][PSHAD_ALPHA][PSHAD_ICW]);
    pContext->hwState.celsius.set (NV056_SET_COMBINER_ALPHA_OCW(1), m_cw[1][PSHAD_ALPHA][PSHAD_OCW]);

    // We use the specular color in the general sense, as a simple secondary color.
    pContext->hwState.celsius.set (NV056_SET_SPECULAR_ENABLE, DRF_NUM(056, _SET_SPECULAR_ENABLE, _V, TRUE));
    pContext->hwState.dwStateFlags |= CELSIUS_FLAG_COMBINERSNEEDSPECULAR;

    // Change the texture stage mapping
    DWORD unit = 0;
    for (DWORD i = 0; i < 2; i++) {
        pContext->hwState.dwTexUnitToTexStageMapping[i] = CELSIUS_UNUSED;

        for (DWORD j=0; j<PSHAD_MAX_SHADER_STAGES; j++) {
            if (i == m_celsiusTexStageMapping[j]) {
                pContext->hwState.dwTexUnitToTexStageMapping[i] = j;
            }
        }
    }

    // Program the constants, based on the mapping table we generated
    // Note that Celsius combiners have a common constant table, so we only use the
    // '0' row of the table.
    for (i=0; i<2; i++) {
        DWORD dwRegNum = m_dwConstMapping[0][i].dwRegNum;
        if (dwRegNum != PSHAD_UNUSED) {
            DWORD dwMap = m_dwConstMapping[0][i].dwMap;
            BOOL bSigned = m_dwConstMapping[0][i].bSigned;
            DWORD dwColor;

            if (m_dwPixelShaderConstFlags[dwRegNum] & PSHAD_CONST_LOCAL) {
                dwColor = ColorConvertWithMapping(m_pixelShaderConsts[dwRegNum], dwMap, &bSigned);
            } else {
                dwColor = ColorConvertWithMapping(pContext->pixelShaderConsts[dwRegNum], dwMap, &bSigned);
            }

            pContext->hwState.kelvin.set1(NV056_SET_COMBINE_FACTOR(i), dwColor);
        }
    }
}
#endif

///////////////////////////////////////////////////////////////////////

DWORD CPixelShader::isBEM(DWORD dwStage) {
    nvAssert(dwStage < PSHAD_MAX_SHADER_STAGES);

    DWORD dwInput = m_shaderStageInputInverse[dwStage];

    return (dwInput == PSHAD_UNUSED) ?
        FALSE :
        ((m_shaderStageProgram[dwInput] == (D3DSIO_TEXBEM-D3DSIO_TEXCOORD)) ||
         (m_shaderStageProgram[dwInput] == (D3DSIO_TEXBEML-D3DSIO_TEXCOORD)));
}

DWORD CPixelShader::isBEML(DWORD dwStage) {
    nvAssert(dwStage < PSHAD_MAX_SHADER_STAGES);

    DWORD dwInput = m_shaderStageInputInverse[dwStage];

    return (dwInput == PSHAD_UNUSED) ?
        FALSE :
        (m_shaderStageProgram[dwInput] == (D3DSIO_TEXBEML-D3DSIO_TEXCOORD));
}

DWORD CPixelShader::GetRGBMapping(const CTexture *pTexture) {
    return (pTexture->getFormat() == NV_SURFACE_FORMAT_HILO_HEMI_V16U16) ?
                NV097_SET_DOT_RGBMAPPING_STAGE1_HILO_HEMISPHERE_NV :
                (pTexture->getFormat() == NV_SURFACE_FORMAT_HILO_1_V16U16) ?
                    NV097_SET_DOT_RGBMAPPING_STAGE1_HILO_1 :
                    (pTexture->getFourCC() == D3DFMT_Q8W8V8U8) ?
                        NV097_SET_DOT_RGBMAPPING_STAGE1_MINUS_1_TO_1_NV :
                        NV097_SET_DOT_RGBMAPPING_STAGE1_ZERO_TO_1;
}

DWORD CPixelShader::getNumTexCoords(PNVD3DCONTEXT pContext, DWORD dwStage) {
    CTexture *pTexture = (pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP]) ? 
        ((CNvObject *)(pContext->tssState[dwStage].dwValue[D3DTSS_TEXTUREMAP]))->getTexture() : 
        0;

    if (pTexture && PSIsProjective[GetShaderProgram(pContext, dwStage)]) {
        return pTexture->getNumTexCoords();
    } else {
        // we may need all 4 coords
        return 4;
    }
}

bool CPixelShader::stageUsesTexture(DWORD dwStage) {
    if (m_shaderStageProgram[dwStage] == PSHAD_UNUSED) {
        return false;
    } else {
        nvAssert(m_shaderStageProgram[dwStage] <= (D3DSIO_TEXM3x3VSPEC-D3DSIO_TEXCOORD));

        return PSShaderUsesTexture[m_shaderStageProgram[dwStage]];
    }
}
