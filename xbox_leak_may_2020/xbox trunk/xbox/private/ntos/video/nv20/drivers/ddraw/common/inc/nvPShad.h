
 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#ifndef _nvPShad_h
#define _nvPShad_h

#ifdef __cplusplus
extern "C"
{
#endif

// Some redefines for my convenience
#define NV_MAPPING_UNSIGNED_IDENTITY  NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY
#define NV_MAPPING_UNSIGNED_INVERT    NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_INVERT
#define NV_MAPPING_EXPAND_NORMAL      NV056_SET_COMBINER_COLOR_ICW_A_MAP_EXPAND_NORMAL
#define NV_MAPPING_EXPAND_NEGATE      NV056_SET_COMBINER_COLOR_ICW_A_MAP_EXPAND_NEGATE
#define NV_MAPPING_HALF_BIAS_NORMAL   NV056_SET_COMBINER_COLOR_ICW_A_MAP_HALFBIAS_NORMAL
#define NV_MAPPING_HALF_BIAS_NEGATE   NV056_SET_COMBINER_COLOR_ICW_A_MAP_HALFBIAS_NEGATE
#define NV_MAPPING_SIGNED_IDENTITY    NV056_SET_COMBINER_COLOR_ICW_A_MAP_SIGNED_IDENTITY
#define NV_MAPPING_SIGNED_NEGATE      NV056_SET_COMBINER_COLOR_ICW_A_MAP_SIGNED_NEGATE


#define NV_REG_ZERO              0x0
#define NV_REG_CONSTANT0         0x1
#define NV_REG_CONSTANT1         0x2
#define NV_REG_FOG               0x3
#define NV_REG_COLOR0            0x4
#define NV_REG_COLOR1            0x5
#define NV_REG_TEXTURE4          0x6
#define NV_REG_TEXTURE5          0x7
#define NV_REG_TEXTURE0          0x8
#define NV_REG_TEXTURE1          0x9
#define NV_REG_TEXTURE2          0xA
#define NV_REG_TEXTURE3          0xB
#define NV_REG_SPARE0            0xC
#define NV_REG_SPARE1            0xD
#define NV_REG_TEXTURE6          0xE
#define NV_REG_TEXTURE7          0xF
#define NV_REG_SPECLIT           0xE
#define NV_REG_EF_PROD           0xF
#define NV_REG_MAX              0x10

#define D3DSP_WRITEMASK_RGBA (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3)
#define D3DSP_WRITEMASK_RGB (D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2)
#define D3DSP_WRITEMASK_A (D3DSP_WRITEMASK_3)
#define D3DSP_WRITEMASK_SHIFT 16

#define DRF_SHIFTMASK(x) (DRF_MASK(x) << DRF_SHIFT(x))



// Random constants
#define CELSIUS_COMBINER_STAGES   2
#define KELVIN_COMBINER_STAGES    8
#define PSHAD_MAX_COMBINER_STAGES       8

#if (NVARCH >= 0x020)
#define PSHAD_MAX_SHADER_STAGES         KELVIN_NUM_TEXTURES
#else
#define PSHAD_MAX_SHADER_STAGES         2
#endif

#define PSHAD_MAX_CONSTANTS       NV_CAPS_MAX_PSHADER_CONSTS

#define PSHAD_INSTRUCTION_ARGS DWORD stage, DWORD color, DWORD shift, DWORD sat, DWORD *dst, DWORD *src, DWORD *alpha, DWORD *map

#define PSHAD_MAX_SRC_ARGS_PER_INSTRUCTION 7
#define PSHAD_MAX_DST_ARGS_PER_INSTRUCTION 2

#define PSHAD_COLOR 0
#define PSHAD_ALPHA 1
#define PSHAD_ICW   0
#define PSHAD_OCW   1

#define PSHAD_DEF   0   // Definition instruction
#define PSHAD_INS   1   // Regular instruction
#define PSHAD_TEX   2   // Texture addressing instruction

// For m_dwPixelShaderConstFlags
#define PSHAD_CONST_SIGNED 0x0001
#define PSHAD_CONST_LOCAL  0x0002
#define PSHAD_CONST_STAGE_SHIFT 16
#define PSHAD_CONST_STAGE_MASK  0xf
#define PSHAD_CONST_NUM_SHIFT   20
#define PSHAD_CONST_NUM_MASK  0xf


#define PSHAD_UNUSED 0xdeadbeef


#define D3DSIO_NV_MMA   59
#define D3DSIO_NV_DD    60
#define D3DSIO_NV_MD    61
#define D3DSIO_NV_MM    62
#define D3DSIO_NV_SPF   63


class CPixelShader {
protected:
    // Structure declarations

    struct SConstMapping {
        DWORD dwRegNum;
        DWORD dwMap;
        BOOL  bSigned;
        SConstMapping() { dwRegNum = PSHAD_UNUSED; }
    };

    // Data declarations

    CNvObject                   *m_pWrapperObject;
    DWORD                       m_dwHandle;
    DWORD                       m_dwCodeSize;
    DWORD                       *m_dwCodeData;

    DWORD                       m_dwStage;

	DWORD                       m_cw[PSHAD_MAX_COMBINER_STAGES][2][2];

    DWORD                       m_textureStageSwap;             // Texture stage 0 and 1 have been swapped for Celsius compatibility

    DWORD                       m_shaderStageProgram[PSHAD_MAX_SHADER_STAGES];
    DWORD                       m_shaderStageInput[PSHAD_MAX_SHADER_STAGES];
    DWORD                       m_shaderStageInputInverse[PSHAD_MAX_SHADER_STAGES];
    DWORD                       m_shaderStageInputMapping[PSHAD_MAX_SHADER_STAGES];

    DWORD                       m_dwTextureStage;
    DWORD                       m_celsiusTexStageMapping[PSHAD_MAX_SHADER_STAGES];
    DWORD                       m_texturesUsed;

    D3DCOLORVALUE               m_pixelShaderConsts[PSHAD_MAX_CONSTANTS];
    DWORD                       m_dwPixelShaderConstFlags[PSHAD_MAX_CONSTANTS];

    SConstMapping               m_dwConstMapping[8][2];

    // Method declarations

    DWORD AllocateConstRegister(DWORD dwStage, DWORD dwRegNum, BOOL bSigned, DWORD dwMap);
    DWORD ColorConvertWithMapping(D3DCOLORVALUE color, DWORD dwMap, BOOL *bSigned);

    DWORD DSTdecode(DWORD *dst, DWORD *shift, DWORD *bias, DWORD *mask, DWORD op);
    DWORD SRCdecode(DWORD *src, DWORD *rgbAlpha, DWORD *alphaAlpha, DWORD *map, DWORD op, DWORD mask, BOOL bSignedConst);

    void GetTextureRegMapping(DWORD *newoffset, DWORD offset);
    DWORD GetShaderProgram(PNVD3DCONTEXT pContext, DWORD dwStage);

    DWORD GetRGBMapping(const CTexture *);

    void DBGPrintInstruction(DWORD op, DWORD dstop, DWORD *srcop);

    void SetCombinerInput(DWORD stage, DWORD color, DWORD var, DWORD mapping, DWORD alpha, DWORD src);

    static const DWORD  PSD3DModToNVMap[];
    static const DWORD  NVMapToNVMapInvert[];
    static const DWORD  PSTypeOffsetToCombinerReg[][4];
    static const DWORD  PSInstructionType[];
    static const char * PSInstructionStrings[];
    static const char * PSProgramNames[];
    static const bool   PSShaderUsesTexture[];
    static const int    PSD3DTexToNVShader[][4];
    static const DWORD  PSNumDstRegs[];
    static const DWORD  PSNumSrcRegs[];
    static const bool   PSIsProjective[];
    static void  (CPixelShader::* const PSInstructionLUT[])(PSHAD_INSTRUCTION_ARGS);
    static const char * PSRegTypeToPrefix[];
    static const char * PSWriteMask[];

    // Instructions broken into two groups, pairable and not.  Pairable ones
    // can be doubled up in a single combiner if there are no dependencies, and
    // have an extra parameter ("side") that determines whether the AB or CD
    // half is used.
    // Note: currently, no optimizations are in place that make this distinction.
    // This should change over time.

	// Standard DX8
    void InstructionMOV(PSHAD_INSTRUCTION_ARGS);
    void InstructionMUL(PSHAD_INSTRUCTION_ARGS);
    void InstructionDP3(PSHAD_INSTRUCTION_ARGS);
    void InstructionNOP(PSHAD_INSTRUCTION_ARGS);
    void InstructionADD(PSHAD_INSTRUCTION_ARGS);
    void InstructionSUB(PSHAD_INSTRUCTION_ARGS);
    void InstructionMAD(PSHAD_INSTRUCTION_ARGS);
    void InstructionLRP(PSHAD_INSTRUCTION_ARGS);
    void InstructionCND(PSHAD_INSTRUCTION_ARGS);

	// NVIDIA internal
	void InstructionNV_MMA(PSHAD_INSTRUCTION_ARGS);
	void InstructionNV_DD (PSHAD_INSTRUCTION_ARGS);
	void InstructionNV_MD (PSHAD_INSTRUCTION_ARGS);
	void InstructionNV_MM (PSHAD_INSTRUCTION_ARGS);
    void InstructionNV_SPF(PSHAD_INSTRUCTION_ARGS);

public:

    BOOL create(PNVD3DCONTEXT pContext, DWORD dwHandle, DWORD dwCodeSize, DWORD *lpCode);
    inline DWORD getHandle() const { return m_dwHandle;  }

    BOOL needsTexCoords(DWORD dwStage) { return (m_shaderStageProgram[dwStage] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE); }

#if (NVARCH >= 0x020)
    void setKelvinState(PNVD3DCONTEXT);
#endif
#if (NVARCH >= 0x010)
    void setCelsiusState(PNVD3DCONTEXT);
#endif

    bool stageUsesTexture(DWORD);
    DWORD isBEM(DWORD);
    DWORD isBEML(DWORD);
    DWORD getNumTexCoords(PNVD3DCONTEXT, DWORD);

    ~CPixelShader(void);
};

#ifdef __cplusplus
}
#endif

#endif // _nvPShad_h

