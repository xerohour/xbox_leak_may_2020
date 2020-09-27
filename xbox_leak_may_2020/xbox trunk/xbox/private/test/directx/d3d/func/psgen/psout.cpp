/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    psout.cpp

Author:

    Matt Bronder

Description:

    Pixel shader description output.

*******************************************************************************/

#include <xtl.h>
#include <winsockx.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "psgen.h"

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

//******************************************************************************
void CShaderGenerator::OutputPixelShaderDef(D3DPIXELSHADERDEF* pd3dpsd, LPXGBUFFER* pxgbDef) {

    LPBYTE pDef;

    if (!pd3dpsd || !pxgbDef) {
        return;
    }

    m_szDef = new char[1024];
    if (!m_szDef) {
        OutputError("Failed to allocate temporary definition buffer");
        return;
    }
    m_szDef[0] = '\0';
    m_uDefSize = 1024;
    m_uDefLength = 1;

    AppendDef("D3DPIXELSHADERDEF psd;\n");
    AppendDef("ZeroMemory(&psd, sizeof(psd));\n");
    AppendDef("\npsd.PSCombinerCount=PS_COMBINERCOUNT(\n");
    DWORD stagecount = ENUMVAL(pd3dpsd->PSCombinerCount, 3, 0);
    AppendDef("    %d,\n", stagecount);
    AppendDef("    %s | %s | %s);\n", ENUMNAME(pd3dpsd->PSCombinerCount,  8,  8, ccf0),
                                   ENUMNAME(pd3dpsd->PSCombinerCount, 12, 12, ccf1),
                                   ENUMNAME(pd3dpsd->PSCombinerCount, 16, 16, ccf2));
    
    AppendDef("\npsd.PSTextureModes=PS_TEXTUREMODES(\n");
    AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSTextureModes,  4,  0, tm));
    AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSTextureModes,  9,  5, tm));
    AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSTextureModes, 14, 10, tm));
    AppendDef("    %s);\n", ENUMNAME(pd3dpsd->PSTextureModes, 19, 15, tm));

    AppendDef("\npsd.PSInputTexture=PS_INPUTTEXTURE(%d,%d,%d,%d);\n", 
           0,
           0,
           ENUMVAL(pd3dpsd->PSInputTexture, 16, 16),
           ENUMVAL(pd3dpsd->PSInputTexture, 21, 20));

    AppendDef("\npsd.PSDotMapping=PS_DOTMAPPING(\n");
    AppendDef("    0,\n");
    AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSDotMapping,  2,  0, dm));
    AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSDotMapping,  6,  4, dm));
    AppendDef("    %s);\n", ENUMNAME(pd3dpsd->PSDotMapping, 10,  8, dm));

    AppendDef("\npsd.PSCompareMode=PS_COMPAREMODE(\n");
    AppendDef("    %s | %s | %s | %s,\n",
           ENUMNAME(pd3dpsd->PSCompareMode,  0,  0, cms),
           ENUMNAME(pd3dpsd->PSCompareMode,  1,  1, cmt),
           ENUMNAME(pd3dpsd->PSCompareMode,  2,  2, cmr),
           ENUMNAME(pd3dpsd->PSCompareMode,  3,  3, cmq));
    AppendDef("    %s | %s | %s | %s,\n",
           ENUMNAME(pd3dpsd->PSCompareMode,  4,  4, cms),
           ENUMNAME(pd3dpsd->PSCompareMode,  5,  5, cmt),
           ENUMNAME(pd3dpsd->PSCompareMode,  6,  6, cmr),
           ENUMNAME(pd3dpsd->PSCompareMode,  7,  7, cmq));
    AppendDef("    %s | %s | %s | %s,\n",
           ENUMNAME(pd3dpsd->PSCompareMode,  8,  8, cms),
           ENUMNAME(pd3dpsd->PSCompareMode,  9,  9, cmt),
           ENUMNAME(pd3dpsd->PSCompareMode, 10, 10, cmr),
           ENUMNAME(pd3dpsd->PSCompareMode, 11, 11, cmq));
    AppendDef("    %s | %s | %s | %s);\n",
           ENUMNAME(pd3dpsd->PSCompareMode, 12, 12, cms),
           ENUMNAME(pd3dpsd->PSCompareMode, 13, 13, cmt),
           ENUMNAME(pd3dpsd->PSCompareMode, 14, 14, cmr),
           ENUMNAME(pd3dpsd->PSCompareMode, 15, 15, cmq));

    for(UINT i=0; i<stagecount; i++)
    {
        AppendDef("\n//------------- Stage %d -------------\n", i);
        AppendDef("\npsd.PSRGBInputs[%d]=PS_COMBINERINPUTS(\n", i);
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 27, 24, inreg),
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 28, 28, chanrgb),
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 31, 29, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 19, 16, inreg),
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 20, 20, chanrgb),
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 23, 21, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 11,  8, inreg),
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 12, 12, chanrgb),
            ENUMNAME(pd3dpsd->PSRGBInputs[i], 15, 13, inmap));
        AppendDef("    %s | %s | %s);\n", 
            ENUMNAME(pd3dpsd->PSRGBInputs[i],  3,  0, inreg),
            ENUMNAME(pd3dpsd->PSRGBInputs[i],  4,  4, chanrgb),
            ENUMNAME(pd3dpsd->PSRGBInputs[i],  7,  5, inmap));
        
        AppendDef("\npsd.PSAlphaInputs[%d]=PS_COMBINERINPUTS(\n", i);
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 27, 24, inreg),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 28, 28, chanalpha),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 31, 29, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 19, 16, inreg),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 20, 20, chanalpha),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 23, 21, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 11,  8, inreg),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 12, 12, chanalpha),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i], 15, 13, inmap));
        AppendDef("    %s | %s | %s);\n", 
            ENUMNAME(pd3dpsd->PSAlphaInputs[i],  3,  0, inreg),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i],  4,  4, chanalpha),
            ENUMNAME(pd3dpsd->PSAlphaInputs[i],  7,  5, inmap));

        AppendDef("\npsd.PSRGBOutputs[%d]=PS_COMBINEROUTPUTS(\n", i);
        AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSRGBOutputs[i],  7,  4, outreg));
        AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSRGBOutputs[i],  3,  0, outreg));
        AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSRGBOutputs[i], 11,  8, outreg));
        AppendDef("    %s | %s | %s | %s",
               ENUMNAME(pd3dpsd->PSRGBOutputs[i], 17, 15, outmap),
               ENUMNAME(pd3dpsd->PSRGBOutputs[i], 13, 13, abmult),
               ENUMNAME(pd3dpsd->PSRGBOutputs[i], 12, 12, cdmult),
               ENUMNAME(pd3dpsd->PSRGBOutputs[i], 14, 14, muxsum));
        if(ENUMVAL(pd3dpsd->PSRGBOutputs[i], 19, 19))
            AppendDef(" | PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA");
        if(ENUMVAL(pd3dpsd->PSRGBOutputs[i], 18, 18))
            AppendDef(" | PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA");
        AppendDef(");\n");

        AppendDef("\npsd.PSAlphaOutputs[%d]=PS_COMBINEROUTPUTS(\n", i);
        AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSAlphaOutputs[i],  7,  4, outreg));
        AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSAlphaOutputs[i],  3,  0, outreg));
        AppendDef("    %s,\n", ENUMNAME(pd3dpsd->PSAlphaOutputs[i], 11,  8, outreg));
        AppendDef("    %s | %s | %s | %s);\n",
               ENUMNAME(pd3dpsd->PSAlphaOutputs[i], 17, 15, outmap),
               ENUMNAME(pd3dpsd->PSAlphaOutputs[i], 13, 13, abmult),
               ENUMNAME(pd3dpsd->PSAlphaOutputs[i], 12, 12, cdmult),
               ENUMNAME(pd3dpsd->PSAlphaOutputs[i], 14, 14, muxsum));
        AppendDef("\npsd.PSConstant0[%d] = 0x%08X;\n", i, pd3dpsd->PSConstant0[i]);
        AppendDef("psd.PSConstant1[%d] = 0x%08X;\n", i, pd3dpsd->PSConstant1[i]);
    }

    // only print info on final combiner if it has been set
    if((pd3dpsd->PSFinalCombinerInputsABCD | pd3dpsd->PSFinalCombinerInputsEFG) != 0)
    {
        AppendDef("\n//------------- FinalCombiner -------------\n", i);
        AppendDef("\npsd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(\n");
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 27, 24, inreg),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 28, 28, chanrgb),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 31, 29, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 19, 16, inreg),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 20, 20, chanrgb),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 23, 21, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 11,  8, inreg),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 12, 12, chanrgb),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD, 15, 13, inmap));
        AppendDef("    %s | %s | %s);\n", 
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD,  3,  0, inreg),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD,  4,  4, chanrgb),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsABCD,  7,  5, inmap));
        
        AppendDef("\npsd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(\n");
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 27, 24, inreg),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 28, 28, chanrgb),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 31, 29, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 19, 16, inreg),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 20, 20, chanrgb),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 23, 21, inmap));
        AppendDef("    %s | %s | %s,\n", 
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 11,  8, inreg),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 12, 12, chanrgb),
            ENUMNAME(pd3dpsd->PSFinalCombinerInputsEFG, 15, 13, inmap));
        
        if(ENUMVAL(pd3dpsd->PSFinalCombinerInputsEFG, 7, 7))
            AppendDef("    PS_FINALCOMBINERSETTING_CLAMP_SUM");
        else
            AppendDef("    0");
        
        if(ENUMVAL(pd3dpsd->PSFinalCombinerInputsEFG, 6, 6))
            AppendDef(" | PS_FINALCOMBINERSETTING_COMPLEMENT_V1");
        else
            AppendDef(" | 0");
        
        if(ENUMVAL(pd3dpsd->PSFinalCombinerInputsEFG, 5, 5))
            AppendDef(" | PS_FINALCOMBINERSETTING_COMPLEMENT_R0);");
        else
            AppendDef(" | 0);\n");
    }
        
    AppendDef("\n");
    AppendDef("psd.PSFinalCombinerConstant0 = 0x%08X;\n", pd3dpsd->PSFinalCombinerConstant0);
    AppendDef("psd.PSFinalCombinerConstant1 = 0x%08X;\n", pd3dpsd->PSFinalCombinerConstant1);
    AppendDef("\npsd.PSC0Mapping = 0x%08X;\n", pd3dpsd->PSC0Mapping);
    AppendDef("\npsd.PSC1Mapping = 0x%08X;\n", pd3dpsd->PSC1Mapping);
    AppendDef("\npsd.PSFinalCombinerConstants = 0x%08X;\n", pd3dpsd->PSFinalCombinerConstants);

    XGBufferCreate(m_uDefLength, pxgbDef);
    if (!*pxgbDef) {
        OutputError("XGBufferCreate failed to create assembly buffer");
    }
    pDef = (LPBYTE)(*pxgbDef)->GetBufferPointer();
    memcpy(pDef, m_szDef, m_uDefLength);
    delete [] m_szDef;
    m_szDef = NULL;
    m_uDefSize = 0;
    m_uDefLength = 0;
}

//******************************************************************************
void CShaderGenerator::AppendDef(LPCSTR szDef, ...) {

    if (m_szDef) {

        va_list vl;
        UINT    uLen;
        LPSTR   szBuffer = new char[1024];

        if (!szBuffer) {
            OutputError("Insufficient memory for AppendDef buffer allocation");
            return;
        }

        va_start(vl, szDef);
        vsprintf(szBuffer, szDef, vl);
        va_end(vl);

//        strcat(szBuffer, "\n");

        uLen = strlen(szBuffer);

        if (uLen + m_uDefLength > m_uDefSize) {
            LPSTR szDef = new char[m_uDefSize + uLen + 1024];
            if (!szDef) {
                OutputError("Failed to grow temporary definition buffer");
                delete [] szBuffer;
                return;
            }
            memcpy(szDef, m_szDef, m_uDefLength);
            delete [] m_szDef;
            m_szDef = szDef;
            m_uDefSize += uLen + 1024;
        }

        strcat(m_szDef, szBuffer);
        m_uDefLength += uLen;

        delete [] szBuffer;
    }
}
