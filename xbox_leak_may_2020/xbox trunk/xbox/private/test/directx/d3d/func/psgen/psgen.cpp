/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    psgen.cpp

Author:

    Matt Bronder

Description:

    Direct3D shader generator.

*******************************************************************************/

#include <xtl.h>
#include <winsockx.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "psgen.h"

//******************************************************************************
// CShaderGenerator
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CShaderGenerator
//
// Description:
//
//     Initialize the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CShaderGenerator::CShaderGenerator() {

    m_szAsm = NULL;
    m_uAsmSize = 0;
    m_uAsmLength = 0;
    m_fCascadeFrequency = 0.5f;
    srand(GetTickCount());
}

//******************************************************************************
//
// Method:
//
//     ~CShaderGenerator
//
// Description:
//
//     Clean up the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CShaderGenerator::~CShaderGenerator() {
}

//******************************************************************************
//
// Method:
//
//     SetCascadeFrequency
//
// Description:
//
//     Set the frequency with which the outputs from one combiner stage will
//     be used as the inputs to the following combiner stage.  A frequency
//     of 0.0 indicates the inputs to a combiner will always be randomly chosen
//     from any valid (initialized) register in the pool.  A frequency of 1.0
//     indicates the combiner input registers can be chosen only from registers
//     that were written to as destination registers in the combiner stage 
//     immediately preceding the given stage.
//
// Arguments:
//
//     float fFrequency             - Cascade frequency.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CShaderGenerator::SetCascadeFrequency(float fFrequency) {

    m_fCascadeFrequency = fFrequency;
    if (m_fCascadeFrequency < 0.0f) {
        m_fCascadeFrequency = 0.0f;
    }
    else if (m_fCascadeFrequency > 1.0f) {
        m_fCascadeFrequency = 1.0f;
    }
}

//******************************************************************************
//
// Method:
//
//     Seed
//
// Description:
//
//     Seed the pseduo-random number generator with the given value.
//
// Arguments:
//
//     DWORD dwSeed                 - Seed value.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CShaderGenerator::Seed(DWORD dwSeed) {

    srand(dwSeed);
}

//******************************************************************************
//
// Method:
//
//     GeneratePixelShader
//
// Description:
//
//     Generate a random pixel shader and, optionally, the corresponding 
//     assembly for the shader.
//
// Arguments:
//
//     BOOL bCascade                - Indicates whether to cascade the outputs
//                                    of one combiner stage into the inputs
//                                    for a subsequent stage.  If cascade is not
//                                    specified the shader will be completely
//                                    random, composing stages that will do 
//                                    nothing to affect the output of the pixel.
//
//     D3DPIXELSHADERDEF* pd3dpsd   - Structure to hold the generated pixel shader.
//
//     LPXGBUFFER* pxgbAsm          - Optional pointer to hold the generated
//                                    pixel shader assembly buffer.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CShaderGenerator::GeneratePixelShader(D3DPIXELSHADERDEF* pd3dpsd, LPXGBUFFER* pxgbAsm, DWORD* pdwInputRegisters) {

// TODO: Spit out a list of what registers are referenced as input registers so a vertex shader will know what
// registers it must output to

    if (!pd3dpsd) { 
        return FALSE;
    }

    memset(pd3dpsd, 0, sizeof(D3DPIXELSHADERDEF));

    if (pxgbAsm) {
        m_szAsm = new char[1024];
        if (!m_szAsm) {
            OutputError("Failed to allocate temporary assembly buffer");
            return FALSE;
        }
        m_szAsm[0] = '\0';
        m_uAsmSize = 1024;
        m_uAsmLength = 1;
    }
    else {
        m_szAsm = NULL;
        m_uAsmSize = 0;
        m_uAsmLength = 0;
    }

    // Initialize the valid inputs
    m_dwValidInputsRGB = PSREG_ZERO | PSREG_C0 | PSREG_C1;
    if (rand() % 4) {
        m_dwValidInputsRGB |= PSREG_V0;
    }
    if (rand() % 2) {
        m_dwValidInputsRGB |= PSREG_V1;
    }
    m_dwValidInputsA = m_dwValidInputsRGB;
    m_dwValidInputsRGB |= PSREG_EF_PROD; // Alpha is not a valid channel for the sum or prod registers
    if (!(rand() % 4)) {
        m_dwValidInputsRGB |= PSREG_FOG;
    }

    // Shader version
    AppendCode("xps.1.1");
    AppendCode("");

    // Constant definitions
    GeneratePSConstants(pd3dpsd);

    // Texture addressing
    GeneratePSTexAddress(pd3dpsd);

    // Combiner stages
    if (pxgbAsm) {
        GeneratePSCombinersAsm(pd3dpsd);
    }
    else {
        GeneratePSCombiners(pd3dpsd);
    }

    if (pxgbAsm) {
        LPBYTE pAsm;
        XGBufferCreate(m_uAsmLength, pxgbAsm);
        if (!*pxgbAsm) {
            OutputError("XGBufferCreate failed to create assembly buffer");
        }
        pAsm = (LPBYTE)(*pxgbAsm)->GetBufferPointer();
        memcpy(pAsm, m_szAsm, m_uAsmLength);
        delete [] m_szAsm;
        m_szAsm = NULL;
        m_uAsmSize = 0;
        m_uAsmLength = 0;
    }

    if (pdwInputRegisters) {
        *pdwInputRegisters = m_dwValidInputsRGB | m_dwValidInputsA;
    }

    return TRUE;
}

//******************************************************************************
void CShaderGenerator::GeneratePSConstants(D3DPIXELSHADERDEF* pd3dpsd) {

    UINT i, j;

    for (i = 0; i < 16; i++) {

        BYTE red, green, blue, alpha;
        red = rand() % 256;
        green = rand() % 256;
        blue = rand() % 256;
        alpha = rand() % 256;

        m_dwPSConstant[i] = D3DCOLOR_RGBA(red, green, blue, alpha);
/*
        if (i % 2) {
            pd3dpsd->PSConstant0[i / 2] = m_dwPSConstant[i];
        }
        else {
            pd3dpsd->PSConstant1[i / 2] = m_dwPSConstant[i];
        }
*/
        AppendCode("def c%d, %1.8f, %1.8f, %1.8f, %1.8f", i, (float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, (float)alpha / 255.0f);
    }

    for (i = 0; i < 8; i++) {
        j = rand() % 16;
        pd3dpsd->PSConstant0[i] = m_dwPSConstant[j];
        pd3dpsd->PSC0Mapping |= (j << (4 * i));
        j = rand() % 16;
        pd3dpsd->PSConstant1[i] = m_dwPSConstant[j];
        pd3dpsd->PSC1Mapping |= (j << (4 * i));
    }

    i = rand() % 16;
    j = rand() % 16;
    pd3dpsd->PSFinalCombinerConstant0 = m_dwPSConstant[i];
    pd3dpsd->PSFinalCombinerConstant1 = m_dwPSConstant[j];

    if (m_szAsm) {
        pd3dpsd->PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(i, j, PS_GLOBALFLAGS_TEXMODE_ADJUST);
    }
    else {
        pd3dpsd->PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(i, j, PS_GLOBALFLAGS_NO_TEXMODE_ADJUST);
    }

if (m_szAsm) {
for (i = 0; i < 8; i++) {
    pd3dpsd->PSConstant0[i] = i;
    pd3dpsd->PSConstant1[i] = i;
}
pd3dpsd->PSFinalCombinerConstant0 = 7;
pd3dpsd->PSFinalCombinerConstant1 = 7;
pd3dpsd->PSC0Mapping = 0;
pd3dpsd->PSC1Mapping = 0;
pd3dpsd->PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(0, 0, PS_GLOBALFLAGS_TEXMODE_ADJUST);
}

    AppendCode("");
}

//******************************************************************************
void CShaderGenerator::GeneratePSTexAddress(D3DPIXELSHADERDEF* pd3dpsd) {

    BYTE    DotMapping[3] = {0};
    BYTE    InputTexture[3] = {0};
    UINT    uStage;
    UINT    i, j;
    DWORD   dwTexAddress[4];
    DWORD   dwTextureMode[] = {
                PS_TEXTUREMODES_NONE,
                PS_TEXTUREMODES_PROJECT2D,
                PS_TEXTUREMODES_PROJECT3D,
                PS_TEXTUREMODES_CUBEMAP,
                PS_TEXTUREMODES_PASSTHRU,
                PS_TEXTUREMODES_CLIPPLANE,
                PS_TEXTUREMODES_BUMPENVMAP,
                PS_TEXTUREMODES_BUMPENVMAP_LUM,
                PS_TEXTUREMODES_DPNDNT_AR,
                PS_TEXTUREMODES_DPNDNT_GB,
                PS_TEXTUREMODES_DOT_ST,
                PS_TEXTUREMODES_DOT_ZW,
                PS_TEXTUREMODES_BRDF,
                PS_TEXTUREMODES_DOT_STR_3D,
                PS_TEXTUREMODES_DOT_STR_CUBE,
                PS_TEXTUREMODES_DOT_RFLCT_SPEC,
                PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST,
            };
    DWORD   dwDotMapping[] = {
                PS_DOTMAPPING_ZERO_TO_ONE,
                PS_DOTMAPPING_MINUS1_TO_1_D3D,
                PS_DOTMAPPING_MINUS1_TO_1_GL,
                PS_DOTMAPPING_MINUS1_TO_1,
                PS_DOTMAPPING_HILO_1,
                PS_DOTMAPPING_HILO_HEMISPHERE_D3D,
                PS_DOTMAPPING_HILO_HEMISPHERE_GL,
                PS_DOTMAPPING_HILO_HEMISPHERE,
            };
    DWORD   dwValidTexInput[] = {
                PSREG_T0,
                PSREG_T1,
                PSREG_T2,
                PSREG_T3,
            };

    // Randomly choose the texture address modes
    dwTexAddress[0] = dwTextureMode[rand() % 6];
    dwTexAddress[1] = dwTextureMode[rand() % 10];
    dwTexAddress[2] = dwTextureMode[rand() % 13];
    dwTexAddress[3] = dwTextureMode[rand() % 17];

    // Resolve conflicting modes
    for (i = 3; i > 0; i--) {
        switch (dwTexAddress[i]) {
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC:
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST:
                if (rand() % 2) {
                    dwTexAddress[2] = PS_TEXTUREMODES_DOT_RFLCT_DIFF;
                }
                else {
                    dwTexAddress[2] = PS_TEXTUREMODES_DOTPRODUCT;
                }
                dwTexAddress[1] = PS_TEXTUREMODES_DOTPRODUCT;
                break;
            case PS_TEXTUREMODES_DOT_STR_3D:
            case PS_TEXTUREMODES_DOT_STR_CUBE:
                dwTexAddress[2] = dwTexAddress[1] = PS_TEXTUREMODES_DOTPRODUCT;
                break;
            case PS_TEXTUREMODES_DOT_ST:
            case PS_TEXTUREMODES_DOT_ZW:
                dwTexAddress[i - 1] = PS_TEXTUREMODES_DOTPRODUCT;
                break;
            case PS_TEXTUREMODES_DOT_RFLCT_DIFF:
                dwTexAddress[1] = PS_TEXTUREMODES_DOTPRODUCT;
                break;
            case PS_TEXTUREMODES_DOTPRODUCT:
                if (i == 1 || dwTexAddress[1] == PS_TEXTUREMODES_DOTPRODUCT || rand() % 2) {
                    if (dwTexAddress[0] == PS_TEXTUREMODES_NONE || dwTexAddress[0] == PS_TEXTUREMODES_CLIPPLANE) {
                        dwTexAddress[0] = dwTextureMode[rand() % 4 + 1];
                    }
                }
                else {
                    while (dwTexAddress[1] == PS_TEXTUREMODES_NONE || dwTexAddress[1] == PS_TEXTUREMODES_CLIPPLANE || dwTexAddress[1] == PS_TEXTUREMODES_DOTPRODUCT) {
                        dwTexAddress[1] = dwTextureMode[rand() % 10];
                    }
                }
                break;

            case PS_TEXTUREMODES_BUMPENVMAP:
            case PS_TEXTUREMODES_BUMPENVMAP_LUM:
            case PS_TEXTUREMODES_DPNDNT_AR:
            case PS_TEXTUREMODES_DPNDNT_GB:
                uStage = i % 3;
                if (i == 3) {
                    if (dwTexAddress[2] == PS_TEXTUREMODES_DOT_ST || dwTexAddress[2] == PS_TEXTUREMODES_DOT_ZW || dwTexAddress[2] == PS_TEXTUREMODES_DOT_RFLCT_DIFF) {
                        uStage = 0;
                    }
                    else {
                        while (dwTexAddress[uStage] == PS_TEXTUREMODES_BUMPENVMAP || 
                               dwTexAddress[uStage] == PS_TEXTUREMODES_BUMPENVMAP_LUM ||
                               dwTexAddress[uStage] == PS_TEXTUREMODES_DPNDNT_AR ||
                               dwTexAddress[uStage] == PS_TEXTUREMODES_DPNDNT_GB ||
                               dwTexAddress[uStage] == PS_TEXTUREMODES_BRDF)
                        {
                            uStage = rand() % 3;
                        }
                    }
                }
                else {
                    if (dwTexAddress[3] == PS_TEXTUREMODES_BRDF) {
                        uStage = 0;
                    }
                    else {
                        while (dwTexAddress[uStage] == PS_TEXTUREMODES_BUMPENVMAP || 
                               dwTexAddress[uStage] == PS_TEXTUREMODES_BUMPENVMAP_LUM ||
                               dwTexAddress[uStage] == PS_TEXTUREMODES_DPNDNT_AR ||
                               dwTexAddress[uStage] == PS_TEXTUREMODES_DPNDNT_GB)
                        {
                            uStage = rand() % 2;
                        }
                    }
                }
                dwTexAddress[uStage] = dwTextureMode[rand() % 4 + 1];
                break;

            case PS_TEXTUREMODES_BRDF:
                if (i == 3) {
                    for (j = 1; j < 3; j++) {
                        while (dwTexAddress[j] == PS_TEXTUREMODES_NONE || dwTexAddress[j] == PS_TEXTUREMODES_CLIPPLANE || dwTexAddress[j] == PS_TEXTUREMODES_DOT_ST || dwTexAddress[j] == PS_TEXTUREMODES_DOT_ZW) {
                            dwTexAddress[j] = dwTextureMode[rand() % 10];
                        }
                    }
                }
                else {
                    dwTexAddress[0] = dwTextureMode[rand() % 4 + 1];
                    while (dwTexAddress[1] == PS_TEXTUREMODES_NONE || dwTexAddress[1] == PS_TEXTUREMODES_CLIPPLANE) {
                        dwTexAddress[1] = dwTextureMode[rand() % 10];
                    }
                }
                break;
        }
    }

    // Set the dot mapping
    for (i = 1; i < 4; i++) {
        switch (dwTexAddress[i]) {
            case PS_TEXTUREMODES_DOTPRODUCT:
            case PS_TEXTUREMODES_DOT_ST:
            case PS_TEXTUREMODES_DOT_ZW:
            case PS_TEXTUREMODES_DOT_RFLCT_DIFF:
            case PS_TEXTUREMODES_DOT_STR_3D:
            case PS_TEXTUREMODES_DOT_STR_CUBE:
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC:
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST:
                if (m_szAsm) {
                    DotMapping[i-1] = (BYTE)dwDotMapping[rand() % 2];
                }
                else {
                    DotMapping[i-1] = (BYTE)dwDotMapping[rand() % 8];
                }
                break;
        }
    }
    pd3dpsd->PSDotMapping = PS_DOTMAPPING(0, DotMapping[0], DotMapping[1], DotMapping[2]);

    // Set the compare modes
    if (!m_szAsm) {
        BYTE CompareMode[4] = {0};
        for (i = 0; i < 4; i++) {
            if (dwTexAddress[i] == PS_TEXTUREMODES_CLIPPLANE) {
                if (rand() % 2) {
                    CompareMode[i] |= PS_COMPAREMODE_S_GE;
                }
                if (rand() % 2) {
                    CompareMode[i] |= PS_COMPAREMODE_T_GE;
                }
                if (rand() % 2) {
                    CompareMode[i] |= PS_COMPAREMODE_R_GE;
                }
                if (rand() % 2) {
                    CompareMode[i] |= PS_COMPAREMODE_Q_GE;
                }
            }
        }
        pd3dpsd->PSCompareMode = PS_COMPAREMODE(CompareMode[0], CompareMode[1], CompareMode[2], CompareMode[3]);
    }

    // Set the input texture stages
    for (i = 2; i < 4; i++) {
        switch (dwTexAddress[i]) {
            case PS_TEXTUREMODES_BUMPENVMAP:
            case PS_TEXTUREMODES_BUMPENVMAP_LUM:
            case PS_TEXTUREMODES_DPNDNT_AR:
            case PS_TEXTUREMODES_DPNDNT_GB:
            case PS_TEXTUREMODES_DOTPRODUCT:
            case PS_TEXTUREMODES_DOT_ST:
            case PS_TEXTUREMODES_DOT_ZW:
            case PS_TEXTUREMODES_DOT_RFLCT_DIFF:
            case PS_TEXTUREMODES_DOT_STR_3D:
            case PS_TEXTUREMODES_DOT_STR_CUBE:
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC:
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST:

                do {
                    InputTexture[i-1] = rand() % i;
                } while (dwTexAddress[InputTexture[i-1]] == PS_TEXTUREMODES_NONE ||
                         dwTexAddress[InputTexture[i-1]] == PS_TEXTUREMODES_CLIPPLANE ||
                         dwTexAddress[InputTexture[i-1]] == PS_TEXTUREMODES_DOTPRODUCT ||
                         dwTexAddress[InputTexture[i-1]] == PS_TEXTUREMODES_DOT_ZW ||
                         dwTexAddress[InputTexture[i-1]] == PS_TEXTUREMODES_DOT_RFLCT_DIFF);
                break;
        }
    }
    pd3dpsd->PSInputTexture = PS_INPUTTEXTURE(0, 0, InputTexture[1], InputTexture[2]);

    // Set the texture modes
    pd3dpsd->PSTextureModes = PS_TEXTUREMODES(dwTexAddress[0], dwTexAddress[1], dwTexAddress[2], dwTexAddress[3]);

    for (i = 0; i < 4; i++) {
        switch (dwTexAddress[i]) {
            case PS_TEXTUREMODES_PROJECT2D:
            case PS_TEXTUREMODES_PROJECT3D:
            case PS_TEXTUREMODES_CUBEMAP:
                AppendCode("tex t%d", i);
                break;
            case PS_TEXTUREMODES_PASSTHRU:
                AppendCode("texcoord t%d", i);
                break;
            case PS_TEXTUREMODES_CLIPPLANE:
                AppendCode("texkill t%d", i);
                break;
            case PS_TEXTUREMODES_BUMPENVMAP:
                AppendCode("texbem t%d, t%d", i, InputTexture[i-1]);
                break;
            case PS_TEXTUREMODES_BUMPENVMAP_LUM:
                AppendCode("texbeml t%d, t%d", i, InputTexture[i-1]);
                break;
            case PS_TEXTUREMODES_DPNDNT_AR:
                AppendCode("texreg2ar t%d, t%d", i, InputTexture[i-1]);
                break;
            case PS_TEXTUREMODES_DPNDNT_GB:
                AppendCode("texreg2gb t%d, t%d", i, InputTexture[i-1]);
                break;
            case PS_TEXTUREMODES_BRDF:
                AppendCode("texbrdf t%d", i);
                break;
            case PS_TEXTUREMODES_DOTPRODUCT:
                if (dwTexAddress[i-1] == PS_TEXTUREMODES_DOTPRODUCT || dwTexAddress[i+1] == PS_TEXTUREMODES_DOTPRODUCT || dwTexAddress[i+1] == PS_TEXTUREMODES_DOT_RFLCT_DIFF) {
                    AppendCode("texm3x3pad t%d, t%d%s", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                }
                else {
                    AppendCode("texm3x2pad t%d, t%d%s", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                }
                break;
            case PS_TEXTUREMODES_DOT_ST:
                AppendCode("texm3x2tex t%d, t%d%s", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                break;
            case PS_TEXTUREMODES_DOT_ZW:
                AppendCode("texm3x2depth t%d, t%d%s", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                break;
            case PS_TEXTUREMODES_DOT_RFLCT_DIFF:
                AppendCode("texm3x3diff t%d, t%d%s", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                break;
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST:
                AppendCode("texm3x3spec t%d, t%d%s, c0", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                break;
            case PS_TEXTUREMODES_DOT_RFLCT_SPEC:
                AppendCode("texm3x3vspec t%d, t%d%s", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                break;
            case PS_TEXTUREMODES_DOT_STR_3D:
            case PS_TEXTUREMODES_DOT_STR_CUBE:
                AppendCode("texm3x3tex t%d, t%d%s", i, InputTexture[i-1], DotMapping[i-1] == PS_DOTMAPPING_MINUS1_TO_1_D3D ? "_bx2" : "");
                break;
        }
    }

    // Keep track of which texture registers will have data written to them during addressing
    for (i = 0; i < 4; i++) {
        switch (dwTexAddress[i]) {
            case PS_TEXTUREMODES_NONE:
            case PS_TEXTUREMODES_CLIPPLANE:
            case PS_TEXTUREMODES_DOT_ZW:
            case PS_TEXTUREMODES_DOTPRODUCT:
//            case PS_TEXTUREMODES_DOT_RFLCT_DIFF: // Reads can be done from t2 when using texm3x3diff instruction
                break;
            default:
                m_dwValidInputsRGB |= dwValidTexInput[i];
                m_dwValidInputsA |= dwValidTexInput[i];
                if (i == 0) {
                    m_dwValidInputsA |= PSREG_R0;
                }
                break;
        }
    }

    AppendCode("");
}

//******************************************************************************
void CShaderGenerator::GeneratePSCombiners(D3DPIXELSHADERDEF* pd3dpsd) {

    BYTE  CombinerCount;
    WORD  CountFlags = 0;
    UINT  i, j;
    UINT  ABCD[2][4], AB[2], CD[2], SUM[2];
    DWORD dwInputMap[4];
    DWORD InputChannel[2][4];
    DWORD dwValidInputs, dwCombinerOutputRGB = m_dwValidInputsRGB, dwCombinerOutputA = m_dwValidInputsA;
    DWORD dwOutputFlags[2];
    DWORD dwFinalCombinerSetting = 0;
    DWORD dwInputRegister[] = {
                PS_REGISTER_ZERO,
                PS_REGISTER_ZERO,
                PS_REGISTER_C0,
                PS_REGISTER_C1,
                PS_REGISTER_FOG,
                PS_REGISTER_V0,
                PS_REGISTER_V1,
                PS_REGISTER_T0,
                PS_REGISTER_T1,
                PS_REGISTER_T2,
                PS_REGISTER_T3,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_V1R0_SUM,
                PS_REGISTER_EF_PROD,
                PS_REGISTER_V1R0_SUM,
                PS_REGISTER_EF_PROD,
                PS_REGISTER_EF_PROD,
          };
    DWORD dwValidInput[] = {
                PSREG_ZERO,
                PSREG_ZERO,
                PSREG_C0,
                PSREG_C1,
                PSREG_FOG,
                PSREG_V0,
                PSREG_V1,
                PSREG_T0,
                PSREG_T1,
                PSREG_T2,
                PSREG_T3,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
                PSREG_V1R0_SUM,
                PSREG_EF_PROD,
                PSREG_V1R0_SUM,
                PSREG_EF_PROD,
                PSREG_EF_PROD
          };
    DWORD dwOutputRegister[] = {
                PS_REGISTER_ZERO,
                PS_REGISTER_ZERO,
                PS_REGISTER_V0,
                PS_REGISTER_V1,
                PS_REGISTER_T0,
                PS_REGISTER_T1,
                PS_REGISTER_T2,
                PS_REGISTER_T3,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
          };
    DWORD dwValidOutput[] = {
                PSREG_ZERO,
                PSREG_ZERO,
                PSREG_V0,
                PSREG_V1,
                PSREG_T0,
                PSREG_T1,
                PSREG_T2,
                PSREG_T3,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
          };
    DWORD dwInputMapping[] = {
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_INPUTMAPPING_SIGNED_NEGATE,
                PS_INPUTMAPPING_SIGNED_NEGATE,
                PS_INPUTMAPPING_EXPAND_NEGATE,
                PS_INPUTMAPPING_EXPAND_NEGATE,
                PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_INPUTMAPPING_EXPAND_NORMAL,
                PS_INPUTMAPPING_EXPAND_NORMAL,
                PS_INPUTMAPPING_HALFBIAS_NORMAL,
                PS_INPUTMAPPING_HALFBIAS_NORMAL,
                PS_INPUTMAPPING_HALFBIAS_NEGATE,
                PS_INPUTMAPPING_HALFBIAS_NEGATE,
          };
    DWORD dwFinalInputMapping[] = {
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_INVERT,
          };
    DWORD dwOutputShiftBias[] = {
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_BIAS,
                PS_COMBINEROUTPUT_SHIFTLEFT_1,
                PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS,
                PS_COMBINEROUTPUT_SHIFTLEFT_2,
                PS_COMBINEROUTPUT_SHIFTRIGHT_1,
          };

    // Set the number of combiners, the mux bit, and the constant sources
    CombinerCount = rand() % 8 + 1;
    if (rand() % 2) {
        CountFlags |= PS_COMBINERCOUNT_MUX_MSB;
    }
    if (rand() % 4) {
        CountFlags |= PS_COMBINERCOUNT_UNIQUE_C0;
    }
    if (rand() % 4) {
        CountFlags |= PS_COMBINERCOUNT_UNIQUE_C1;
    }
    pd3dpsd->PSCombinerCount = PS_COMBINERCOUNT(CombinerCount, CountFlags);

    for (i = 0; i < (UINT)CombinerCount; i++) {

        // Color
        for (j = 0; j < 4; j++) {
            if (rand() % 4) {
                InputChannel[0][j] = PS_CHANNEL_RGB;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsRGB;
                }
                else {
                    dwValidInputs = dwCombinerOutputRGB;
                }
            }
            else {
                InputChannel[0][j] = PS_CHANNEL_ALPHA;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsA;
                }
                else {
                    dwValidInputs = dwCombinerOutputA;
                }
            }
            do {
                ABCD[0][j] = rand() % 17;
            } while (!(dwValidInput[ABCD[0][j]] & dwValidInputs));
        }

        // To prevent an overabundance of multiplies by zero obliterating
        // the combiner results, weight the frequency of zero mappings
        // that result in zero appearing in a multiply with a combiner
        // register other than a constant or zero.
        for (j = 0; j < 3; j += 2) {

            if (dwInputRegister[ABCD[0][j  ]] == PS_REGISTER_ZERO ||
                dwInputRegister[ABCD[0][j+1]] == PS_REGISTER_ZERO) 
            {
                int nOdds;
                UINT k, l;

                if (dwInputRegister[ABCD[0][j]] == PS_REGISTER_ZERO) {
                    k = 0;
                    l = 1;
                }
                else {
                    k = 1;
                    l = 0;
                }

                switch (dwInputRegister[ABCD[0][j+l]]) {
                    case PS_REGISTER_ZERO:
                    case PS_REGISTER_C0:
                    case PS_REGISTER_C1:
                        nOdds = 6;
                        break;
                    default:
                        nOdds = 1;
                        break;
                }

                dwInputMap[j+l] = rand() % 18;
                dwInputMap[j+k] = rand() % 9;
                if (rand() % 8 > nOdds) {
                    dwInputMap[j+k] += 9;
                }
            }
        }

        pd3dpsd->PSRGBInputs[i] = PS_COMBINERINPUTS(dwInputRegister[ABCD[0][0]] | InputChannel[0][0] | dwInputMapping[dwInputMap[0]],
                                                    dwInputRegister[ABCD[0][1]] | InputChannel[0][1] | dwInputMapping[dwInputMap[1]],
                                                    dwInputRegister[ABCD[0][2]] | InputChannel[0][2] | dwInputMapping[dwInputMap[2]],
                                                    dwInputRegister[ABCD[0][3]] | InputChannel[0][3] | dwInputMapping[dwInputMap[3]]);

        AB[0] = rand() % 14;
        do {
            CD[0] = rand() % 14;
        } while (dwOutputRegister[CD[0]] == dwOutputRegister[AB[0]] && dwOutputRegister[CD[0]] != PS_REGISTER_ZERO);
        do {
            SUM[0] = rand() % 14;
        } while ((dwOutputRegister[SUM[0]] == dwOutputRegister[AB[0]] || dwOutputRegister[SUM[0]] == dwOutputRegister[CD[0]]) && dwOutputRegister[SUM[0]] != PS_REGISTER_ZERO);

        dwOutputFlags[0] = dwOutputShiftBias[rand() % 9];
        if (!(rand() % 5)) {
            dwOutputFlags[0] |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA;
        }
        if (!(rand() % 5)) {
            dwOutputFlags[0] |= PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA;
        }
        if (!(rand() % 3)) {
            dwOutputFlags[0] |= PS_COMBINEROUTPUT_AB_DOT_PRODUCT;
        }
        if (!(rand() % 3)) {
            dwOutputFlags[0] |= PS_COMBINEROUTPUT_CD_DOT_PRODUCT;
        }
        if (!(rand() % 6)) {
            dwOutputFlags[0] |= PS_COMBINEROUTPUT_AB_CD_MUX;
        }

        pd3dpsd->PSRGBOutputs[i] = PS_COMBINEROUTPUTS(dwOutputRegister[AB[0]], dwOutputRegister[CD[0]], dwOutputRegister[SUM[0]], dwOutputFlags[0]);

        // Alpha
        for (j = 0; j < 4; j++) {
            if (rand() % 4) {
                InputChannel[1][j] = PS_CHANNEL_ALPHA;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsA;
                }
                else {
                    dwValidInputs = dwCombinerOutputA;
                }
            }
            else {
                InputChannel[1][j] = PS_CHANNEL_BLUE;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsRGB;
                }
                else {
                    dwValidInputs = dwCombinerOutputRGB;
                }
            }
            do {
                ABCD[1][j] = rand() % 17;
            } while (!(dwValidInput[ABCD[1][j]] & dwValidInputs));
        }

        // To prevent an overabundance of multiplies by zero obliterating
        // the combiner results, weight the frequency of zero mappings
        // that result in zero appearing in a multiply with a combiner
        // register other than a constant or zero.
        for (j = 0; j < 3; j += 2) {

            if (dwInputRegister[ABCD[1][j  ]] == PS_REGISTER_ZERO ||
                dwInputRegister[ABCD[1][j+1]] == PS_REGISTER_ZERO) 
            {
                int nOdds;
                UINT k, l;

                if (dwInputRegister[ABCD[1][j]] == PS_REGISTER_ZERO) {
                    k = 0;
                    l = 1;
                }
                else {
                    k = 1;
                    l = 0;
                }

                switch (dwInputRegister[ABCD[1][j+l]]) {
                    case PS_REGISTER_ZERO:
                    case PS_REGISTER_C0:
                    case PS_REGISTER_C1:
                        nOdds = 6;
                        break;
                    default:
                        nOdds = 1;
                        break;
                }

                dwInputMap[j+l] = rand() % 18;
                dwInputMap[j+k] = rand() % 9;
                if (rand() % 8 > nOdds) {
                    dwInputMap[j+k] += 9;
                }
            }
        }

        pd3dpsd->PSAlphaInputs[i] = PS_COMBINERINPUTS(dwInputRegister[ABCD[1][0]] | InputChannel[1][0] | dwInputMapping[dwInputMap[0]],
                                                      dwInputRegister[ABCD[1][1]] | InputChannel[1][1] | dwInputMapping[dwInputMap[1]],
                                                      dwInputRegister[ABCD[1][2]] | InputChannel[1][2] | dwInputMapping[dwInputMap[2]],
                                                      dwInputRegister[ABCD[1][3]] | InputChannel[1][3] | dwInputMapping[dwInputMap[3]]);

        // Find different alpha (excluding discard) registers for the AB, CD, and SUM results that also differ from the 
        // AB and CD color registers if the *_BLUE_TO_ALPHA flags are present
        do {
            AB[1] = rand() % 14;
            do {
                CD[1] = rand() % 14;
            } while (dwOutputRegister[CD[1]] == dwOutputRegister[AB[1]] && dwOutputRegister[CD[1]] != PS_REGISTER_ZERO);
            do {
                SUM[1] = rand() % 14;
            } while ((dwOutputRegister[SUM[1]] == dwOutputRegister[AB[1]] || dwOutputRegister[SUM[1]] == dwOutputRegister[CD[1]]) && dwOutputRegister[SUM[1]] != PS_REGISTER_ZERO);
        } while (((dwOutputFlags[0] & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA) && (AB[0] > 1) && ((dwOutputRegister[AB[1]] == dwOutputRegister[AB[0]] && AB[1] > 1) || (dwOutputRegister[CD[1]] == dwOutputRegister[AB[0]] && CD[1] > 1) || (dwOutputRegister[SUM[1]] == dwOutputRegister[AB[0]] && SUM[1] > 1))) ||
                 ((dwOutputFlags[0] & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA) && (CD[0] > 1) && ((dwOutputRegister[AB[1]] == dwOutputRegister[CD[0]] && AB[1] > 1) || (dwOutputRegister[CD[1]] == dwOutputRegister[CD[0]] && CD[1] > 1) || (dwOutputRegister[SUM[1]] == dwOutputRegister[CD[0]] && SUM[1] > 1))));

        dwOutputFlags[1] = dwOutputShiftBias[rand() % 9];
        if (!(rand() % 6)) {
            dwOutputFlags[1] |= PS_COMBINEROUTPUT_AB_CD_MUX;
        }

        pd3dpsd->PSAlphaOutputs[i] = PS_COMBINEROUTPUTS(dwOutputRegister[AB[1]], dwOutputRegister[CD[1]], dwOutputRegister[SUM[1]], dwOutputFlags[1]);

        dwCombinerOutputRGB = dwValidOutput[AB[0]] | dwValidOutput[CD[0]] | dwValidOutput[SUM[0]] | PSREG_ZERO | PSREG_C0 | PSREG_C1;
        dwCombinerOutputA = dwValidOutput[AB[1]] | dwValidOutput[CD[1]] | dwValidOutput[SUM[1]] | PSREG_ZERO | PSREG_C0 | PSREG_C1;
        if (dwOutputFlags[0] & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA) {
            dwCombinerOutputA |= dwValidOutput[AB[0]];
        }
        if (dwOutputFlags[0] & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA) {
            dwCombinerOutputA |= dwValidOutput[CD[0]];
        }
        m_dwValidInputsRGB |= dwCombinerOutputRGB;
        m_dwValidInputsA |= dwCombinerOutputA;
    }

    dwCombinerOutputRGB |= PSREG_EF_PROD;

    if ((m_dwValidInputsRGB & PSREG_V1) && (m_dwValidInputsRGB & PSREG_R0)) {
        m_dwValidInputsRGB |= PSREG_V1R0_SUM;
        dwCombinerOutputRGB |= PSREG_V1R0_SUM;
    }

    // Final combiner
    for (i = 0; i < 4; i++) {
        if (rand() % 4) {
            InputChannel[0][i] = PS_CHANNEL_RGB;
            if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                dwValidInputs = m_dwValidInputsRGB;
            }
            else {
                dwValidInputs = dwCombinerOutputRGB;
            }
        }
        else {
            InputChannel[0][i] = PS_CHANNEL_ALPHA;
            if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                dwValidInputs = m_dwValidInputsA;
            }
            else {
                dwValidInputs = dwCombinerOutputA;
            }
        }
        do {
            ABCD[0][i] = rand() % 21 + 1;
        } while (!(dwValidInput[ABCD[0][i]] & dwValidInputs));
    }

    pd3dpsd->PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(dwInputRegister[ABCD[0][0]] | InputChannel[0][0] | dwFinalInputMapping[rand() % 4],
                                                           dwInputRegister[ABCD[0][1]] | InputChannel[0][1] | dwFinalInputMapping[rand() % 4],
                                                           dwInputRegister[ABCD[0][2]] | InputChannel[0][2] | dwFinalInputMapping[rand() % 4],
                                                           dwInputRegister[ABCD[0][3]] | InputChannel[0][3] | dwFinalInputMapping[rand() % 4]);

    if (ABCD[0][0] == PS_REGISTER_V1R0_SUM || ABCD[0][1] == PS_REGISTER_V1R0_SUM || ABCD[0][2] == PS_REGISTER_V1R0_SUM || ABCD[0][3] == PS_REGISTER_V1R0_SUM) {
        if (rand() % 2) {
            dwFinalCombinerSetting |= PS_FINALCOMBINERSETTING_CLAMP_SUM;
        }
        if (rand() % 2) {
            dwFinalCombinerSetting |= PS_FINALCOMBINERSETTING_COMPLEMENT_V1;
        }
        if (rand() % 2) {
            dwFinalCombinerSetting |= PS_FINALCOMBINERSETTING_COMPLEMENT_R0;
        }
    }

    for (i = 0; i < 3; i++) {

        if (i < 2) {
            j = rand() % 3;
        }
        else {
            j = (UINT)!(BOOL)(rand() % 3);
        }
        if (j) {
            InputChannel[0][i] = PS_CHANNEL_RGB;
            if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                dwValidInputs = m_dwValidInputsRGB;
            }
            else {
                dwValidInputs = dwCombinerOutputRGB;
            }
        }
        else {
            InputChannel[0][i] = PS_CHANNEL_ALPHA;
            if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                dwValidInputs = m_dwValidInputsA;
            }
            else {
                dwValidInputs = dwCombinerOutputA;
            }
        }
        do {
            ABCD[0][i] = rand() % 16 + 1;
        } while (!(dwValidInput[ABCD[0][i]] & dwValidInputs));
    }

    pd3dpsd->PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(dwInputRegister[ABCD[0][0]] | InputChannel[0][0] | dwFinalInputMapping[rand() % 4],
                                                          dwInputRegister[ABCD[0][1]] | InputChannel[0][1] | dwFinalInputMapping[rand() % 4],
                                                          dwInputRegister[ABCD[0][2]] | InputChannel[0][2] | dwFinalInputMapping[rand() % 4],
                                                          dwFinalCombinerSetting);
}

//******************************************************************************
void CShaderGenerator::GeneratePSCombinersAsm(D3DPIXELSHADERDEF* pd3dpsd) {

    char  szAsm[2048], szTemp[1024], szConstant[4];
    char* szInReg;
    UINT  uNumConstants, uConstant[2];
    BYTE  CombinerCount;
    int   i;
    UINT  j, k, l, m;
    UINT  uInstr;
    UINT  uSrc[7], uDst[3];
    UINT  uInputMap[7], uOutputMap;
    DWORD dwOutputFlags;
    char* szInstruction[] = {
                "nop",
                "mov",
                "add",
                "sub",
                "mul",
                "mad",
                "lrp",
                "cnd",
                "xmma",
                "xmmc",
                "xdm",
                "xdd",
                "dp3",
//                "dp4",
          };
    UINT  uInstrSrc[] = {
                0,
                1,
                2,
                2,
                2,
                3,
                3,
                3,
                4,
                4,
                4,
                4,
                2,
          };
    UINT  uInstrDst[] = {
                0,
                1,
                1,
                1,
                1,
                1,
                1,
                1,
                3,
                3,
                2,
                2,
                1,
          };
    char* szIOChannel[] = {
                "",
                ".rgb",
                ".a",
                "",
          };
    enum  IOCHANNEL {
                IOCH_RGB = 1,
                IOCH_ALPHA = 2,
                IOCH_BOTH = 3,
          } OutputChannel, InputChannel[7];
    DWORD dwCombinerInput, dwCombinerOutput;
    DWORD dwInputChannel[4], dwInputChannelSet[4];
    DWORD dwIOChannel[] = {
                0,
                PS_CHANNEL_RGB,
                PS_CHANNEL_ALPHA,
                0
          };
    DWORD dwValidInputs, dwValidInputsRGB, dwValidInputsA, dwCombinerOutputRGB = m_dwValidInputsRGB, dwCombinerOutputA = m_dwValidInputsA, dwOutRGB, dwOutA;
    char* szInputRegister[] = {
                "zero",
                "c",
                "c",
                "fog",
                "v0",
                "v1",
                "t0",
                "t1",
                "t2",
                "t3",
                "r0",
                "r1",
                "r0",
                "r1",
                "r0",
                "r1",
                "sum",
                "prod",
                "sum",
                "prod",
                "prod",
          };
    DWORD dwInputRegister[] = {
                PS_REGISTER_ZERO,
                PS_REGISTER_C0,
                PS_REGISTER_C1,
                PS_REGISTER_FOG,
                PS_REGISTER_V0,
                PS_REGISTER_V1,
                PS_REGISTER_T0,
                PS_REGISTER_T1,
                PS_REGISTER_T2,
                PS_REGISTER_T3,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_V1R0_SUM,
                PS_REGISTER_EF_PROD,
                PS_REGISTER_V1R0_SUM,
                PS_REGISTER_EF_PROD,
                PS_REGISTER_EF_PROD,
          };
    DWORD dwValidInput[] = {
                PSREG_ZERO,
                PSREG_C0,
                PSREG_C1,
                PSREG_FOG,
                PSREG_V0,
                PSREG_V1,
                PSREG_T0,
                PSREG_T1,
                PSREG_T2,
                PSREG_T3,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
                PSREG_V1R0_SUM,
                PSREG_EF_PROD,
                PSREG_V1R0_SUM,
                PSREG_EF_PROD,
                PSREG_EF_PROD
          };
    char* szOutputRegister[] = {
                "discard",
                "v0",
                "v1",
                "t0",
                "t1",
                "t2",
                "t3",
                "r0",
                "r1",
                "r0",
                "r1",
                "r0",
                "r1",
          };
    DWORD dwOutputRegister[] = {
                PS_REGISTER_ZERO,
                PS_REGISTER_V0,
                PS_REGISTER_V1,
                PS_REGISTER_T0,
                PS_REGISTER_T1,
                PS_REGISTER_T2,
                PS_REGISTER_T3,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
                PS_REGISTER_R0,
                PS_REGISTER_R1,
          };
    DWORD dwValidOutput[] = {
                PSREG_ZERO,
                PSREG_V0,
                PSREG_V1,
                PSREG_T0,
                PSREG_T1,
                PSREG_T2,
                PSREG_T3,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
                PSREG_R0,
                PSREG_R1,
          };
    DWORD dwInputMapping[] = {
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_INPUTMAPPING_EXPAND_NORMAL,
                PS_INPUTMAPPING_EXPAND_NEGATE,
                PS_INPUTMAPPING_HALFBIAS_NORMAL,
                PS_INPUTMAPPING_HALFBIAS_NEGATE,
                PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_INPUTMAPPING_SIGNED_NEGATE,
                PS_INPUTMAPPING_SIGNED_NEGATE,
          };
    char* szInputMapping[] = {
                "%s_sat",
                "%s_sat",
                "%s_sat",
                "1-%s",
                "1-%s",
                "%s_bx2",
                "-%s_bx2",
                "%s_bias",
                "-%s_bias",
                "%s",
                "%s",
                "%s",
                "-%s",
                "-%s",
          };
    DWORD dwOutputShiftBias[] = {
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_IDENTITY,
                PS_COMBINEROUTPUT_BIAS,
                PS_COMBINEROUTPUT_SHIFTLEFT_1,
                PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS,
                PS_COMBINEROUTPUT_SHIFTLEFT_2,
                PS_COMBINEROUTPUT_SHIFTRIGHT_1,
          };
    char* szOutputShiftBias[] = {
                "",
                "",
                "",
                "",
                "_bias",
                "_x2",
                "_bx2",
                "_x4",
                "_d2",
          };

    CombinerCount = rand() % 8 + 1;

    for (i = 0; i < (int)CombinerCount; i++) {

        uNumConstants = 0;
        uConstant[0] = 0xF;
        uConstant[1] = 0xF;
        dwOutRGB = PSREG_ZERO | PSREG_C0 | PSREG_C1;
        dwOutA = PSREG_ZERO | PSREG_C0 | PSREG_C1;

        for (m = 0; m < 2; m++) {
            uConstant[m] = 0;
        }

        for (k = 0; k < 2; k++) {

            *szAsm = '\0';

            for (m = 0; m < 7; m++) {
                uSrc[m] = 0;
            }
            for (m = 0; m < 3; m++) {
                uDst[m] = 0;
            }
            for (m = 0; m < 7; m++) {
                uInputMap[m] = 0;
            }
            for (m = 0; m < 7; m++) {
                InputChannel[m] = IOCH_RGB;
            }

            if (k) {
                if (OutputChannel != IOCH_BOTH && rand() % 2) {
                    strcat(szAsm, "+");
                    if (OutputChannel == IOCH_RGB) {
                        OutputChannel = IOCH_ALPHA;
                    }
                    else {
                        OutputChannel = IOCH_RGB;
                    }
                }
                else {
                    break;
                }
            }
            else {
                OutputChannel = (IOCHANNEL)(rand() % 3 + 1);
            }

            do {
                if (OutputChannel == IOCH_ALPHA) { // dp3, xdm, and xdd are only valid for the rgb pipeline
                    uInstr = rand() % 9 + 1; //rand() % 10;
                }
                else if (OutputChannel == IOCH_RGB) {
                    uInstr = rand() % 12 + 1; //rand() % 13;
                }
                else {
                    uInstr = rand() % 13; // nop cannot be paired so it is only valid for both output channels
                }
            } while (uInstr == 7 && !(m_dwValidInputsA & PSREG_R0)); // cnd may only be chosen if r0 is a valid input

            uDst[0] = rand() % 13;
            do {
                uDst[1] = rand() % 13;
            } while (dwValidOutput[uDst[1]] == dwValidOutput[uDst[0]] && dwValidOutput[uDst[1]] != PSREG_ZERO);
            do {
                uDst[2] = rand() % 13;
            } while ((dwValidOutput[uDst[2]] == dwValidOutput[uDst[0]] || dwValidOutput[uDst[2]] == dwValidOutput[uDst[1]]) && dwValidOutput[uDst[2]] != PSREG_ZERO);

            uOutputMap = rand() % 9;

            for (j = 0; j < uInstrSrc[uInstr]; j++) {

                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputsRGB = m_dwValidInputsRGB;
                }
                else {
                    dwValidInputsRGB = dwCombinerOutputRGB;
                }
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputsA = m_dwValidInputsA;
                }
                else {
                    dwValidInputsA = dwCombinerOutputA;
                }
                if (!(dwValidInputsRGB & dwValidInputsA)) {
                    OutputError("The valid inputs specified for combiner stage is the null set");
                }

                if (!j && uInstr == 7) { // The alpha input channel must be specified for Src0 of cnd
                    InputChannel[j] = IOCH_ALPHA;
                }
                else {
                    InputChannel[j] = (IOCHANNEL)(rand() % 3 + 1);
                }

                switch (InputChannel[j]) {
                    case IOCH_RGB:
                        dwValidInputs = dwValidInputsRGB;
                        break;
                    case IOCH_ALPHA:
                        dwValidInputs = dwValidInputsA;
                        break;
                    case IOCH_BOTH:
                        switch (OutputChannel) {
                            case IOCH_RGB:
                                dwValidInputs = dwValidInputsRGB;
                                break;
                            case IOCH_ALPHA:
                                dwValidInputs = dwValidInputsA;
                                break;
                            case IOCH_BOTH:
                                dwValidInputs = dwValidInputsRGB & dwValidInputsA;
                                break;
                        }
                        break;
                }
                if (uInstr == 13) {
                    dwValidInputs = dwValidInputsRGB & dwValidInputsA;
                }

                if (!j && uInstr == 7) { // Src0 for cnd must be r0
                    uSrc[0] = 10;
                }
                else {
                    do {
                        uSrc[j] = rand() % 16;
                    } while (!(dwValidInput[uSrc[j]] & dwValidInputs) || (uNumConstants == 2 && (dwValidInput[uSrc[j]] == PSREG_C0 || dwValidInput[uSrc[j]] == PSREG_C1)));
                }

                if (dwValidInput[uSrc[j]] == PSREG_C0 || dwValidInput[uSrc[j]] == PSREG_C1) {
                    if (++uNumConstants == 1 || !(rand() % 8)) {
                        uSrc[j] = 1;
                    }
                    else {
                        uSrc[j] = 2;
                    }
                }

                if (!j) {
                    if (uInstr == 6) { // lrp only supports 2 input mappings on Src0
                        uInputMap[j] = rand() % 5;
                    }
                    else if (uInstr == 7) { // cnd only supports the signed identity mapping on Src0
                        uInputMap[j] = 9;
                    }
                    else {
                        uInputMap[j] = rand() % 14;
                    }
                }
                else {
                    uInputMap[j] = rand() % 14;
                }
            }

            strcat(szAsm, szInstruction[uInstr]);
            if (uInstr) {
                strcat(szAsm, szOutputShiftBias[uOutputMap]);
            }

            for (j = 0; j < uInstrDst[uInstr]; j++) {
                strcat(szAsm, " ");
                strcat(szAsm, szOutputRegister[uDst[j]]);
                strcat(szAsm, szIOChannel[OutputChannel]);
                strcat(szAsm, ",");
            }

            for (j = 0; j < uInstrSrc[uInstr]; j++) {
                strcat (szAsm, " ");
                if (dwValidInput[uSrc[j]] == PSREG_C0 || dwValidInput[uSrc[j]] == PSREG_C1) {
                    LPDWORD pdwConstant;
                    if (dwValidInput[uSrc[j]] == PSREG_C0) {
                        l = 0;
                        pdwConstant = &pd3dpsd->PSConstant0[i];
                    }
                    else {
                        l = 1;
                        pdwConstant = &pd3dpsd->PSConstant1[i];
                    }
                    if (l || uNumConstants < 2) {
                        do {
                            uConstant[l] = rand() % 16;
                        } while (l && uConstant[1] == uConstant[0]);
                    }
                    *pdwConstant = m_dwPSConstant[uConstant[l]];
                    sprintf(szConstant, "c%d", uConstant[l]);
                    szInReg = szConstant;
                }
                else {
                    szInReg = szInputRegister[uSrc[j]];
                }
                sprintf(szTemp, szInputMapping[uInputMap[j]], szInReg);
                strcat(szTemp, szIOChannel[InputChannel[j]]);
                strcat(szAsm, szTemp);
                if (j != uInstrSrc[uInstr] - 1) {
                    strcat(szAsm, ",");
                }
            }

            AppendCode(szAsm);
            
            dwOutputFlags = 0;

            if (!uInstr) {
                // If a nop was generated, don't write anything to the combiners 
                i--;
                continue;
            }

            switch (uInstr) {
//                case 0: // nop
//                    uDst[0] = 0;
//                    uDst[1] = 0;
//                    uDst[2] = 0;
//                    break;
                case 1: // mov
                    uDst[1] = 0;
                    uDst[2] = 0;
                    uSrc[1] = 0;
                    uInputMap[1] = 3;
                    InputChannel[1] = IOCH_BOTH;
                    break;
                case 2: // add
                    uDst[2] = uDst[0];
                    uDst[0] = 0;
                    uDst[1] = 0;
                    uSrc[2] = uSrc[1];
                    uSrc[1] = 0;
                    uSrc[3] = 0;
                    uInputMap[2] = uInputMap[1];
                    uInputMap[1] = 3;
                    uInputMap[3] = 3;
                    InputChannel[2] = InputChannel[1];
                    InputChannel[1] = IOCH_BOTH;
                    InputChannel[3] = IOCH_BOTH;
                    break;
                case 3: // sub
                    uDst[2] = uDst[0];
                    uDst[0] = 0;
                    uDst[1] = 0;
                    uSrc[2] = uSrc[1];
                    uSrc[1] = 0;
                    uSrc[3] = 0;
                    uInputMap[2] = uInputMap[1];
                    uInputMap[1] = 3;
                    uInputMap[3] = 5;
                    InputChannel[2] = InputChannel[1];
                    InputChannel[1] = IOCH_BOTH;
                    InputChannel[3] = IOCH_BOTH;
                    break;
                case 4: // mul
                    uDst[1] = 0;
                    uDst[2] = 0;
                    break;
                case 5: // mad
                    uDst[2] = uDst[0];
                    uDst[0] = 0;
                    uDst[1] = 0;
                    uSrc[3] = uSrc[2];
                    uSrc[2] = 0;
                    uInputMap[3] = uInputMap[2];
                    uInputMap[2] = 3;
                    InputChannel[3] = InputChannel[2];
                    InputChannel[2] = IOCH_BOTH;
                    break;
                case 6: // lrp
                    uDst[2] = uDst[0];
                    uDst[0] = 0;
                    uDst[1] = 0;
                    uSrc[3] = uSrc[0];
                    uInputMap[3] = uInputMap[0];
                    if (uInputMap[0] < 3) {
                        uInputMap[3] = 3;
                    }
                    else {
                        uInputMap[3] = 2;
                    }
                    InputChannel[3] = InputChannel[0];
                    break;
                case 7: // cnd
                    uDst[2] = uDst[0];
                    uDst[0] = 0;
                    uDst[1] = 0;
                    uSrc[0] = uSrc[2];
                    uSrc[2] = uSrc[1];
                    uSrc[1] = 0;
                    uSrc[3] = 0;
                    uInputMap[0] = uInputMap[2];
                    uInputMap[2] = uInputMap[1];
                    uInputMap[1] = 3;
                    uInputMap[3] = 3;
                    InputChannel[0] = InputChannel[2];
                    InputChannel[2] = InputChannel[1];
                    InputChannel[1] = IOCH_BOTH;
                    InputChannel[3] = IOCH_BOTH;
                    dwOutputFlags |= PS_COMBINEROUTPUT_AB_CD_MUX;
                    break;
                case 8: // xmma
                    // Registers are okay as specified
                    break;
                case 9: // xmmc
                    dwOutputFlags |= PS_COMBINEROUTPUT_AB_CD_MUX;
                    break;
                case 10: // xdm
                    uDst[2] = 0;
                    dwOutputFlags |= PS_COMBINEROUTPUT_AB_DOT_PRODUCT;
                    if (OutputChannel & IOCH_ALPHA) {
                        dwOutputFlags |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA;
                    }
                    break;
                case 11: // xdd
                    uDst[2] = 0;
                    dwOutputFlags |= PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT;
                    if (OutputChannel & IOCH_ALPHA) {
                        dwOutputFlags |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA | PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA;
                    }
                    break;
                case 12: // dp3
                    uDst[1] = 0;
                    uDst[2] = 0;
                    dwOutputFlags |= PS_COMBINEROUTPUT_AB_DOT_PRODUCT;
                    if (OutputChannel & IOCH_ALPHA) {
                        dwOutputFlags |= PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA;
                    }
                    break;
/*
                case 13: // dp4
                    uDst[2] = uDst[0];
                    uDst[0] = 0;
                    uDst[1] = 0;
                    uSrc[2] = uSrc[0];
                    uSrc[3] = uSrc[1];
                    InputChannel[2] = IOCH_ALPHA;
                    InputChannel[3] = IOCH_ALPHA;
                    dwOutputFlags |= PS_COMBINEROUTPUT_AB_DOT_PRODUCT;
                    break;
*/
            }

            dwCombinerOutput = PS_COMBINEROUTPUTS(dwOutputRegister[uDst[0]], dwOutputRegister[uDst[1]], dwOutputRegister[uDst[2]], dwOutputFlags | dwOutputShiftBias[uOutputMap]);

            if (OutputChannel & IOCH_RGB) {
                for (j = 0; j < 4; j++) {
                    if (InputChannel[j] == IOCH_ALPHA) {
                        dwInputChannelSet[j] = PS_CHANNEL_ALPHA;
                    }
                    else {
                        dwInputChannelSet[j] = PS_CHANNEL_RGB;
                    }
                }

                pd3dpsd->PSRGBInputs[i] = PS_COMBINERINPUTS(dwInputRegister[uSrc[0]] | dwInputChannelSet[0] | dwInputMapping[uInputMap[0]],
                                                            dwInputRegister[uSrc[1]] | dwInputChannelSet[1] | dwInputMapping[uInputMap[1]],
                                                            dwInputRegister[uSrc[2]] | dwInputChannelSet[2] | dwInputMapping[uInputMap[2]],
                                                            dwInputRegister[uSrc[3]] | dwInputChannelSet[3] | dwInputMapping[uInputMap[3]]);
                pd3dpsd->PSRGBOutputs[i] = dwCombinerOutput;

                dwOutRGB |= dwValidOutput[uDst[0]] | dwValidOutput[uDst[1]] | dwValidOutput[uDst[2]];

                if (dwOutputFlags & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA) {
                    dwOutA |= dwValidOutput[uDst[0]];
                }
                if (dwOutputFlags & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA) {
                    dwOutA |= dwValidOutput[uDst[1]];
                }
            }
            if ((OutputChannel & IOCH_ALPHA) && uInstr < 11) { // Only set the alpha inputs for non-dotproduct instructions
                for (j = 0; j < 4; j++) {
                    if (InputChannel[j] == IOCH_RGB) {
                        dwInputChannelSet[j] = PS_CHANNEL_RGB;
                    }
                    else {
                        dwInputChannelSet[j] = PS_CHANNEL_ALPHA;
                    }
                }
                if (uInstr < 10) {
                    pd3dpsd->PSAlphaInputs[i] = PS_COMBINERINPUTS(dwInputRegister[uSrc[0]] | dwInputChannelSet[0] | dwInputMapping[uInputMap[0]],
                                                                  dwInputRegister[uSrc[1]] | dwInputChannelSet[1] | dwInputMapping[uInputMap[1]],
                                                                  dwInputRegister[uSrc[2]] | dwInputChannelSet[2] | dwInputMapping[uInputMap[2]],
                                                                  dwInputRegister[uSrc[3]] | dwInputChannelSet[3] | dwInputMapping[uInputMap[3]]);
                    pd3dpsd->PSAlphaOutputs[i] = dwCombinerOutput;

                    dwOutA |= dwValidOutput[uDst[0]] | dwValidOutput[uDst[1]] | dwValidOutput[uDst[2]];
                }
                else {
                    pd3dpsd->PSAlphaInputs[i] = PS_COMBINERINPUTS(PS_REGISTER_ZERO,
                                                                  PS_REGISTER_ZERO,
                                                                  dwInputRegister[uSrc[2]] | dwInputChannelSet[2] | dwInputMapping[uInputMap[2]],
                                                                  dwInputRegister[uSrc[3]] | dwInputChannelSet[3] | dwInputMapping[uInputMap[3]]);
                    pd3dpsd->PSAlphaOutputs[i] = dwCombinerOutput & 0xFFFFF00F;

                    dwOutA |= dwValidOutput[uDst[1]];
                }
            }
        }

        if (uInstr) {

            dwCombinerOutputRGB = dwOutRGB;
            dwCombinerOutputA = dwOutA;

            m_dwValidInputsRGB |= dwOutRGB;
            m_dwValidInputsA |= dwOutA;

            pd3dpsd->PSC0Mapping |= (uConstant[0] << (4 * i));
            pd3dpsd->PSC1Mapping |= (uConstant[1] << (4 * i));
        }
    }

    for (; i < 8; i++) {
        pd3dpsd->PSC0Mapping |= (0xF << (4 * i));
        pd3dpsd->PSC1Mapping |= (0xF << (4 * i));
    }

    // Final combiner
    if (!((m_dwValidInputsRGB & PSREG_R0) && (m_dwValidInputsA & PSREG_R0)) || rand() % 2) {

        dwCombinerOutputRGB |= PSREG_EF_PROD;

        if ((m_dwValidInputsRGB & PSREG_V1) && (m_dwValidInputsRGB & PSREG_R0)) {
            m_dwValidInputsRGB |= PSREG_V1R0_SUM;
            dwCombinerOutputRGB |= PSREG_V1R0_SUM;
        }

        for (m = 0; m < 7; m++) {
            uSrc[m] = 0;
        }
        for (m = 0; m < 7; m++) {
            uInputMap[m] = 0;
        }
        for (m = 0; m < 7; m++) {
            InputChannel[m] = IOCH_RGB;
        }
        for (m = 0; m < 2; m++) {
            uConstant[m] = 0;
        }

        for (i = 0; i < 4; i++) {
            if (rand() % 4) {
                InputChannel[i] = IOCH_RGB;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsRGB;
                }
                else {
                    dwValidInputs = dwCombinerOutputRGB;
                }
            }
            else {
                InputChannel[i] = IOCH_ALPHA;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsA;
                }
                else {
                    dwValidInputs = dwCombinerOutputA;
                }
            }
            do {
                uSrc[i] = rand() % 21;
            } while (!(dwValidInput[uSrc[i]] & dwValidInputs));

            uInputMap[i] = rand() % 5;
        }

        for (i = 4; i < 7; i++) {

            if (i < 6) {
                j = rand() % 3;
            }
            else {
                j = (UINT)!(BOOL)(rand() % 3);
            }
            if (j) {
                InputChannel[i] = IOCH_RGB;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsRGB;
                }
                else {
                    dwValidInputs = dwCombinerOutputRGB;
                }
            }
            else {
                InputChannel[i] = IOCH_ALPHA;
                if (((float)rand() / (float)RAND_MAX) > m_fCascadeFrequency) {
                    dwValidInputs = m_dwValidInputsA;
                }
                else {
                    dwValidInputs = dwCombinerOutputA;
                }
            }
            do {
                uSrc[i] = rand() % 16;
            } while (!(dwValidInput[uSrc[i]] & dwValidInputs));

            uInputMap[i] = rand() % 5;
        }


        uConstant[0] = rand() % 16;
        do {
            uConstant[1] = rand() % 16;
        } while (uConstant[1] == uConstant[0]);
        uNumConstants = 0;

        strcpy(szAsm, "xfc ");

        for (i = 0; i < 7; i++) {
            if (dwValidInput[uSrc[i]] == PSREG_C0 || dwValidInput[uSrc[i]] == PSREG_C1) {
                if (++uNumConstants == 1) {
                    uSrc[i] = 1;
                }
                if (dwValidInput[uSrc[i]] == PSREG_C0) {
                    l = 0;
                    pd3dpsd->PSFinalCombinerConstant0 = m_dwPSConstant[uConstant[0]];
                }
                else {
                    l = 1;
                    pd3dpsd->PSFinalCombinerConstant1 = m_dwPSConstant[uConstant[1]];
                }
                sprintf(szConstant, "c%d", uConstant[l]);
                szInReg = szConstant;
            }
            else {
                szInReg = szInputRegister[uSrc[i]];
            }
            if (uInputMap[i] < 3 && rand() % 10) {
                strcpy(szTemp, szInReg);
            }
            else {
                sprintf(szTemp, szInputMapping[uInputMap[i]], szInReg);
            }
            if (!(InputChannel[i] == IOCH_RGB && rand() % 4)) {
                strcat(szTemp, szIOChannel[InputChannel[i]]);
            }
            strcat(szAsm, szTemp);
            if (i != 6) {
                strcat(szAsm, ", ");
            }
        }

        AppendCode(szAsm);

        pd3dpsd->PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(dwInputRegister[uSrc[0]] | dwIOChannel[InputChannel[0]] | dwInputMapping[uInputMap[0]],
                                                               dwInputRegister[uSrc[1]] | dwIOChannel[InputChannel[1]] | dwInputMapping[uInputMap[1]],
                                                               dwInputRegister[uSrc[2]] | dwIOChannel[InputChannel[2]] | dwInputMapping[uInputMap[2]],
                                                               dwInputRegister[uSrc[3]] | dwIOChannel[InputChannel[3]] | dwInputMapping[uInputMap[3]]);


        pd3dpsd->PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(dwInputRegister[uSrc[4]] | dwIOChannel[InputChannel[4]] | dwInputMapping[uInputMap[4]],
                                                              dwInputRegister[uSrc[5]] | dwIOChannel[InputChannel[5]] | dwInputMapping[uInputMap[5]],
                                                              dwInputRegister[uSrc[6]] | dwIOChannel[InputChannel[6]] | dwInputMapping[uInputMap[6]],
                                                              PS_FINALCOMBINERSETTING_CLAMP_SUM);

//        pd3dpsd->PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(uConstant[0], uConstant[1], PS_GLOBALFLAGS_TEXMODE_ADJUST);
        pd3dpsd->PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(0, 0, PS_GLOBALFLAGS_TEXMODE_ADJUST);

//        if (uNumConstants) { // Only set the constant mappings if c0 or c1 was actually specified in final combiner
        for (i = 0; i < 7; i++) {
            if (dwValidInput[uSrc[i]] == PSREG_C0) {
                pd3dpsd->PSFinalCombinerConstants |= uConstant[0];
                break;
            }
        }
        if (i == 7) {
            pd3dpsd->PSFinalCombinerConstants |= 0xF;
        }
        for (i = 0; i < 7; i++) {
            if (dwValidInput[uSrc[i]] == PSREG_C1) {
                pd3dpsd->PSFinalCombinerConstants |= uConstant[1] << 4;
                break;
            }
        }
        if (i == 7) {
            pd3dpsd->PSFinalCombinerConstants |= 0xF0;
        }
    }
    else {
        // Default final combiner
    }

    // Set the number of combiners, the mux bit, and the constant sources
    pd3dpsd->PSCombinerCount = PS_COMBINERCOUNT(CombinerCount, PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
}

//******************************************************************************
void CShaderGenerator::AppendCode(LPCSTR szCode, ...) {

    if (m_szAsm) {

        va_list vl;
        UINT    uLen;
        LPSTR   szBuffer = new char[1024];

        if (!szBuffer) {
            OutputError("Insufficient memory for AppendCode buffer allocation");
            return;
        }

        va_start(vl, szCode);
        vsprintf(szBuffer, szCode, vl);
        va_end(vl);

        strcat(szBuffer, "\n");

        uLen = strlen(szBuffer);

        if (uLen + m_uAsmLength > m_uAsmSize) {
            LPSTR szAsm = new char[m_uAsmSize + uLen + 1024];
            if (!szAsm) {
                OutputError("Failed to grow temporary assembly buffer");
                delete [] szBuffer;
                return;
            }
            memcpy(szAsm, m_szAsm, m_uAsmLength);
            delete [] m_szAsm;
            m_szAsm = szAsm;
            m_uAsmSize += uLen + 1024;
        }

        strcat(m_szAsm, szBuffer);
        m_uAsmLength += uLen;

        delete [] szBuffer;
    }
}

//******************************************************************************
void CShaderGenerator::OutputError(LPCSTR szError) {

    __asm int 3;
}

