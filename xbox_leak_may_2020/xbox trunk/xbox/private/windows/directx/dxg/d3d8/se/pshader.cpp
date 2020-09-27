/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pshader.cpp
 *  Content:    Pixel shader implementation.
 *
 ****************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// ---------------------------------------------------
// Pixel Shader register validation code
// ---------------------------------------------------

#if DBG
#define GETFIELD(value, upper, lower) \
    ((value >> lower) & ((2L << (upper-lower))-1))

#define ERRBADARG(err1, err2) \
    { \
        DbgPrint("%s: %s", err1, err2); \
        DPF_ERR(""); \
        return FALSE; \
    } 

#define CHECKZERO(value, upper, lower, err1, err2) \
    { \
        if(GETFIELD(value, upper, lower) != 0) \
            ERRBADARG(err1, err2) \
    }

#define CHECKENUM(value, upper, lower, valid, err1, err2) \
    { \
        if(!((valid >> GETFIELD(value, upper, lower)) & 0x1)) \
            ERRBADARG(err1, err2) \
    }

// The following constants define valid enumerants for a group of fields
// The lsb corresponds to enum 0, the next bit corresponds to enum 1, etc.
#define INPUT_SRC_REG      0x3f3fL // valid combiner input source registers
#define OUTPUT_MAPPING     0x005fL // valid output mapping enumerants
#define OUTPUT_DST_REG     0x3f31L // valid output destination registers
#define ABCD_INPUT_SRC_REG 0xff3fL // final combiner input source registers ABCD
#define EFG_INPUT_SRC_REG  0x3f3fL // final combiner input source register EFG
#define TEXTURE_MODE_STG0 0x0003fL // texture modes for stage 0
#define TEXTURE_MODE_STG1 0x380ffL // texture modes for stage 1
#define TEXTURE_MODE_STG2 0x38fffL // texture modes for stage 2
#define TEXTURE_MODE_STG3 0x5f7ffL // texture modes for stage 3
#define COMBINER_COUNT     0x01feL // valid combiner count values
#define TRUE_FALSE         0x0003L // only 0 and 1 are valid
#define DOT_MAPPING        0x00ffL // 0-7 are valid

    BOOL ValidCombinerInputs(DWORD val, char *err)
    {
        CHECKENUM(val, 27, 24, INPUT_SRC_REG, err, "Invalid A Register");
        CHECKENUM(val, 19, 16, INPUT_SRC_REG, err, "Invalid B Register");
        CHECKENUM(val, 11,  8, INPUT_SRC_REG, err, "Invalid C Register");
        CHECKENUM(val,  3,  0, INPUT_SRC_REG, err, "Invalid D Register");
        return TRUE;
    }

    BOOL ValidAlphaOutputs(DWORD val, char *err)
    {
        CHECKZERO(val, 31, 18, err, "31:18 must be 0");
        CHECKZERO(val, 13, 12, err, "13:12 must be 0");
        CHECKENUM(val, 17, 15, OUTPUT_MAPPING, err, "Invalid output mapping");
        CHECKENUM(val, 11,  8, OUTPUT_DST_REG, err, "Invalid Sum register");
        CHECKENUM(val,  7,  4, OUTPUT_DST_REG, err, "Invalid AB register");
        CHECKENUM(val,  3,  0, OUTPUT_DST_REG, err, "Invalid CD register");
        return TRUE;
    }

    BOOL ValidRGBOutputs(DWORD val, char *err)
    {
        CHECKZERO(val, 31, 20, err, "31:20 must be 0");
        CHECKENUM(val, 17, 15, OUTPUT_MAPPING, err, "Invalid output mapping");
        CHECKENUM(val, 11,  8, OUTPUT_DST_REG, err, "Invalid Sum register");
        CHECKENUM(val,  7,  4, OUTPUT_DST_REG, err, "Invalid AB register");
        CHECKENUM(val,  3,  0, OUTPUT_DST_REG, err, "Invalid CD register");
        return TRUE;
    }

    BOOL ValidFinalCombinerABCD(DWORD val, char *err)
    {
        DWORD src;
        // if A source i SUM or PROD, A_Alpha must be 0
        src = GETFIELD(val, 27, 24);
        if((src == 0xe) || (src == 0xf))
            CHECKZERO(val, 28, 28, err, "Alpha invalid for SUM or PROD on A");

        // if B source i SUM or PROD, B_Alpha must be 0
        src = GETFIELD(val, 19, 16);
        if((src == 0xe) || (src == 0xf))
            CHECKZERO(val, 20, 20, err, "Alpha invalid for SUM or PROD on B");

        // if C source i SUM or PROD, C_Alpha must be 0
        src = GETFIELD(val, 11,  8);
        if((src == 0xe) || (src == 0xf))
            CHECKZERO(val, 12, 12, err, "Alpha invalid for SUM or PROD on C");

        // if D source i SUM or PROD, D_Alpha must be 0
        src = GETFIELD(val,  3,  0);
        if((src == 0xe) || (src == 0xf))
            CHECKZERO(val,  4,  4, err, "Alpha invalid for SUM or PROD on D");

        CHECKENUM(val, 27, 24, ABCD_INPUT_SRC_REG, err, "Invalid A Register");
        CHECKENUM(val, 19, 16, ABCD_INPUT_SRC_REG, err, "Invalid B Register");
        CHECKENUM(val, 11,  8, ABCD_INPUT_SRC_REG, err, "Invalid C Register");
        CHECKENUM(val,  3,  0, ABCD_INPUT_SRC_REG, err, "Invalid D Register");
        CHECKENUM(val, 31, 29, TRUE_FALSE, err, "Invalid input mapping Reg A");
        CHECKENUM(val, 23, 21, TRUE_FALSE, err, "Invalid input mapping Reg B");
        CHECKENUM(val, 15, 13, TRUE_FALSE, err, "Invalid input mapping Reg C");
        CHECKENUM(val,  7,  5, TRUE_FALSE, err, "Invalid input mapping Reg D");
        return TRUE;
    }

    BOOL ValidFinalCombinerEFG(DWORD val, char *err)
    {
        CHECKENUM(val, 31, 29, TRUE_FALSE, err, "Invalid input mapping Reg E");
        CHECKENUM(val, 23, 21, TRUE_FALSE, err, "Invalid input mapping Reg F");
        CHECKENUM(val, 15, 13, TRUE_FALSE, err, "Invalid input mapping Reg G");
        CHECKZERO(val,  4,  0, err, "4:0 must be 0");
        CHECKENUM(val, 27, 24, EFG_INPUT_SRC_REG, err, "Invalid E Register");
        CHECKENUM(val, 19, 16, EFG_INPUT_SRC_REG, err, "Invalid F Register");
        CHECKENUM(val, 11,  8, EFG_INPUT_SRC_REG, err, "Invalid G Register");
        return TRUE;
    }

    BOOL ValidTextureModes(DWORD val, char *err)
    {
        CHECKENUM(val,  4,  0, TEXTURE_MODE_STG0, err, "Invalid Stg 0 Mode");
        CHECKENUM(val,  9,  5, TEXTURE_MODE_STG1, err, "Invalid Stg 1 Mode");
        CHECKENUM(val, 14, 10, TEXTURE_MODE_STG2, err, "Invalid Stg 2 Mode");
        CHECKENUM(val, 19, 15, TEXTURE_MODE_STG3, err, "Invalid Stg 3 Mode");
        CHECKZERO(val, 31, 20, err, "31:20 must be 0");
        
        // inter-field checks
        DWORD Stage0 = GETFIELD(val,  4,  0);
        DWORD Stage1 = GETFIELD(val,  9,  5);
        DWORD Stage2 = GETFIELD(val, 14, 10);
        DWORD Stage3 = GETFIELD(val, 19, 15);

        if(((Stage1 == PS_TEXTUREMODES_BUMPENVMAP) || 
            (Stage1 == PS_TEXTUREMODES_BUMPENVMAP_LUM) ||
            (Stage1 == PS_TEXTUREMODES_DPNDNT_AR) || 
            (Stage1 == PS_TEXTUREMODES_DPNDNT_GB) || 
            (Stage1 == PS_TEXTUREMODES_DOTPRODUCT)) &&
           ((Stage0 == PS_TEXTUREMODES_CLIPPLANE) ||
            (Stage0 == PS_TEXTUREMODES_NONE)))
            ERRBADARG(err, "Stg1 mode requires different Stg0 mode");

        if((Stage2 == PS_TEXTUREMODES_BRDF) && 
           ((Stage0 == PS_TEXTUREMODES_NONE) || 
            (Stage0 == PS_TEXTUREMODES_CLIPPLANE)))
            ERRBADARG(err, "Stg2 mode requires different Stg0 mode");

        if((Stage3 == PS_TEXTUREMODES_BRDF) && 
           ((Stage1 == PS_TEXTUREMODES_NONE) || 
            (Stage1 == PS_TEXTUREMODES_CLIPPLANE) || 
            (Stage1 == PS_TEXTUREMODES_DOTPRODUCT)))
            ERRBADARG(err, "Stg3 mode requires different Stg1 mode");

        if((Stage2 == PS_TEXTUREMODES_DOT_ST) && (Stage1 != PS_TEXTUREMODES_DOTPRODUCT))
            ERRBADARG(err, "Stg2 mode requires DOTPRODUCT in Stg1");

        if((Stage3 == PS_TEXTUREMODES_DOT_ST) && (Stage2 != PS_TEXTUREMODES_DOTPRODUCT))
            ERRBADARG(err, "Stg3 mode requires DOTPRODUCT in Stg2");

        if((Stage2 == PS_TEXTUREMODES_DOT_ZW) && (Stage1 != PS_TEXTUREMODES_DOTPRODUCT))
            ERRBADARG(err, "Stg2 mode requires DOTPRODUCT in Stg1");

        if((Stage3 == PS_TEXTUREMODES_DOT_ZW) && (Stage2 != PS_TEXTUREMODES_DOTPRODUCT))
            ERRBADARG(err, "Stg3 mode requires DOTPRODUCT in Stg2");

        if((Stage3 == PS_TEXTUREMODES_DOT_STR_3D) &&
           ((Stage2 != PS_TEXTUREMODES_DOTPRODUCT) || 
            (Stage1 != PS_TEXTUREMODES_DOTPRODUCT)))
            ERRBADARG(err, "Stg3 mode requires DOTPRODUCT in Stg2 or Stg1");

        if((Stage3 == PS_TEXTUREMODES_DOT_STR_CUBE) &&
           ((Stage2 != PS_TEXTUREMODES_DOTPRODUCT) || 
            (Stage1 != PS_TEXTUREMODES_DOTPRODUCT)))
            ERRBADARG(err, "Stg3 mode requires DOTPRODUCT in Stg2 or Stg1");

        if(((Stage3 == PS_TEXTUREMODES_DOT_RFLCT_SPEC) ||
            (Stage3 == PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST)) &&
           ((Stage2 != PS_TEXTUREMODES_DOTPRODUCT) &&
            (Stage2 != PS_TEXTUREMODES_DOT_RFLCT_DIFF)))
            ERRBADARG(err, "Stg3 mode requires DOTPRODUCT or DOT_RFLCT_DIFF in Stg2");

        if((Stage2 == PS_TEXTUREMODES_DOT_RFLCT_DIFF) &&
           ((Stage3 != PS_TEXTUREMODES_DOT_RFLCT_SPEC) &&
            (Stage3 != PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST)))
            ERRBADARG(err, "Stg2 mode requires DOT_RFLCT_SPEC or DOT_RFLCT_SPEC_CONST in Stg 3");

        if(((Stage3 == PS_TEXTUREMODES_DOT_RFLCT_SPEC) ||
            (Stage3 == PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST)) &&
           (Stage1 != PS_TEXTUREMODES_DOTPRODUCT))
            ERRBADARG(err, "Stg 3 mode requires DOTPRODUCT in Stg 1");

        return TRUE;
    }

    BOOL ValidInputTexture(DWORD val, char *err)
    {
        CHECKZERO(val, 15,  0, err, "15:0 must be 0");
        CHECKENUM(val, 19, 16, 0x3L, err, "Invalid input texture in stg 2"); // 0 and 1 valid
        CHECKENUM(val, 23, 20, 0x7L, err, "Invalid input texture in stg 3"); // 0, 1, and 2 valid
        CHECKZERO(val, 31, 24, err, "31:24 must be 0");
        return TRUE;
    }

    BOOL ValidCombinerCount(DWORD val, char *err)
    {
        CHECKENUM(val,  7,  0, COMBINER_COUNT, err, "Invalid Combiner Count");
        CHECKENUM(val, 11,  8, TRUE_FALSE, err, "Invalid Mux bit");
        CHECKENUM(val, 15, 12, TRUE_FALSE, err, "Invalid Separate C0 flag");
        CHECKZERO(val, 31, 17, err, "31:17 must be 0");
        return TRUE;
    }

    BOOL ValidCompareMode(DWORD val, char *err)
    {
        CHECKZERO(val, 31, 16, err, "31:16 must be 0\n");
        return TRUE;
    }

    BOOL ValidDotMapping(DWORD val, char *err)
    {
        CHECKENUM(val,  3,  0, DOT_MAPPING, err, "Stage 1 invalid");
        CHECKENUM(val,  7,  4, DOT_MAPPING, err, "Stage 2 invalid");
        CHECKENUM(val, 11,  8, DOT_MAPPING, err, "Stage 3 invalid");
        CHECKZERO(val, 31, 12, err, "31:12 must be 0");
        return TRUE;
    }

    BOOL ValidFinalCombinerConstants(DWORD val, char *err)
    {
        CHECKZERO(val, 31,  9, err, "31:9 must be 0");
        return TRUE;
    }

#endif

#define PSDEFCHECKS(pPSDef) \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[0], "PSAlphaInputs[0]") || \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[1], "PSAlphaInputs[1]") || \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[2], "PSAlphaInputs[2]") || \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[3], "PSAlphaInputs[3]") || \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[4], "PSAlphaInputs[4]") || \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[5], "PSAlphaInputs[5]") || \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[6], "PSAlphaInputs[6]") || \
    !ValidCombinerInputs(pPSDef->PSAlphaInputs[7], "PSAlphaInputs[7]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[0], "PSRGBInputs[0]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[1], "PSRGBInputs[1]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[2], "PSRGBInputs[2]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[3], "PSRGBInputs[3]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[4], "PSRGBInputs[4]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[5], "PSRGBInputs[5]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[6], "PSRGBInputs[6]") || \
    !ValidCombinerInputs(pPSDef->PSRGBInputs[7], "PSRGBInputs[7]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[0], "PSAlphaOutputs[0]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[1], "PSAlphaOutputs[1]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[2], "PSAlphaOutputs[2]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[3], "PSAlphaOutputs[3]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[4], "PSAlphaOutputs[4]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[5], "PSAlphaOutputs[5]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[6], "PSAlphaOutputs[6]") || \
    !ValidAlphaOutputs(pPSDef->PSAlphaOutputs[7], "PSAlphaOutputs[7]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[0], "PSRGBOutputs[0]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[1], "PSRGBOutputs[1]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[2], "PSRGBOutputs[2]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[3], "PSRGBOutputs[3]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[4], "PSRGBOutputs[4]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[5], "PSRGBOutputs[5]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[6], "PSRGBOutputs[6]") || \
    !ValidRGBOutputs(pPSDef->PSRGBOutputs[7], "PSRGBOutputs[7]") || \
    !ValidFinalCombinerABCD(pPSDef->PSFinalCombinerInputsABCD, "PSFinalCombinerInputsABCD") || \
    !ValidFinalCombinerEFG(pPSDef->PSFinalCombinerInputsEFG, "PSFinalCombinerInputsEFG") || \
    !ValidTextureModes(pPSDef->PSTextureModes, "PSTextureModes") || \
    !ValidInputTexture(pPSDef->PSInputTexture, "PSInputTexture") || \
    !ValidCombinerCount(pPSDef->PSCombinerCount, "PSCombinerCount") || \
    !ValidDotMapping(pPSDef->PSDotMapping, "PSDotMapping") || \
    !ValidCompareMode(pPSDef->PSCompareMode, "PSCompareMode") || \
    !ValidFinalCombinerConstants(pPSDef->PSFinalCombinerConstants, "PSFinalCombinerConstants")
           
//------------------------------------------------------------------------------
// D3DDevice_CreatePixelShader

extern "C"
HRESULT WINAPI D3DDevice_CreatePixelShader(
    CONST D3DPIXELSHADERDEF *pPSDef,
    DWORD* pHandle) 
{ 
    COUNT_API(API_D3DDEVICE_CREATEPIXELSHADER);

    if (DBG_CHECK(TRUE))
    {
        #if DBG
        if(PSDEFCHECKS(pPSDef))
        {
            DXGRIP("D3DDevice_CreatePixelShader - Invalid pixel shader definition.");
        }
        #endif
    }
    
    // Allocate PixelShader struct with a D3DPIXELSHADERDEF struct appended to the end
    PixelShader* pPixelShader = (PixelShader*) MemAllocNoZero(sizeof(*pPixelShader) +
                                                              sizeof(D3DPIXELSHADERDEF));
    if (pPixelShader == NULL)
    {
        return E_OUTOFMEMORY;
    }

#if DBG

    pPixelShader->Signature = 'Pshd';

#endif

    pPixelShader->RefCount = 1;
    pPixelShader->D3DOwned = 1;
    // make pPSDef point to the D3DPIXELSHADERDEF struct appended to pPixelShader
    pPixelShader->pPSDef = (D3DPIXELSHADERDEF *)(pPixelShader + 1);

    // keep a copy of the pixel shader definition
    memcpy(pPixelShader->pPSDef, pPSDef, sizeof(D3DPIXELSHADERDEF));
    
    *pHandle = (DWORD) pPixelShader;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_DeletePixelShader

extern "C"
void WINAPI D3DDevice_DeletePixelShader(
    DWORD Handle) 
{ 
    COUNT_API(API_D3DDEVICE_DELETEPIXELSHADER);

    CDevice* pDevice = g_pDevice;

    PixelShader* pPixelShader = (PixelShader*) Handle;

    if (DBG_CHECK(TRUE))
    {
        if (pPixelShader == NULL)
        {
            DPF_ERR("Invalid pixel shader handle value");
        }
        if ((pDevice->m_pPixelShader == pPixelShader) &&
            (pPixelShader->RefCount == 1))
        {
            DPF_ERR("Can't delete a pixel shader that's currently active");
        }

        #if DBG

            if (pPixelShader->Signature != 'Pshd')
            {
                DPF_ERR("Invalid pixel shader object (already deleted?)");
            }

        #endif
    }

    // We have to employ a reference count because of state blocks:

    if ((--pPixelShader->RefCount == 0) && (pPixelShader->D3DOwned))
    {
    
    #if DBG

        pPixelShader->Signature = 'xxxx';

    #endif

        // only free the memory if it is owned by D3D
        MemFree(pPixelShader);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetUserPixelShader
//
// Same functionality as D3DDevice_SetPixelShader but this method does not keep
// a copy of the PIXELSHADERDEF.

extern "C"
void WINAPI D3DDevice_SetPixelShaderProgram(
    CONST D3DPIXELSHADERDEF *pPSDef) 
{ 
    COUNT_API(API_D3DDEVICE_SETPIXELSHADERPROGRAM);

    CDevice* pDevice = g_pDevice;
    
    if (DBG_CHECK(TRUE))
    {
        #if DBG
        if(pPSDef && (PSDEFCHECKS(pPSDef)))
        {
            DXGRIP("D3DDevice_SetUserPixelShader - Invalid pixel shader definition.");
        }
        #endif
    }
    
    if(pPSDef)
    {
#if DBG
        pDevice->m_UserPixelShader.Signature = 'Pshd';
#endif

        pDevice->m_UserPixelShader.RefCount = 1;
        pDevice->m_UserPixelShader.D3DOwned = 0;
        pDevice->m_UserPixelShader.pPSDef = (D3DPIXELSHADERDEF*)pPSDef;
        D3DDevice_SetPixelShader((DWORD)(&(pDevice->m_UserPixelShader)));
    }
    else
    {
        // uninstall pixel shader
        D3DDevice_SetPixelShader((DWORD)(NULL));
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetPixelShader

extern "C"
void WINAPI D3DDevice_SetPixelShader(
    DWORD Handle) 
{ 
    COUNT_API(API_D3DDEVICE_SETPIXELSHADER);

    PixelShader* pPixelShader = (PixelShader*) Handle;

    CDevice* pDevice = g_pDevice;
    
    if (DBG_CHECK(TRUE))
    {
        #if DBG
            // allow Handle to be NULL
            if (pPixelShader && (pPixelShader->Signature != 'Pshd'))
            {
                DPF_ERR("Invalid pixel shader object (already deleted?)");
            }
            // The pixel shader was validated at create time.
        #endif
    }

    PixelShader* pOldShader = pDevice->m_pPixelShader;

    // set current pixel shader
    pDevice->m_pPixelShader = pPixelShader;  
    
    if(!pPixelShader)
    {
        // removing pixel shader, make sure that the combiners get 
        // set up for the fixed-function pipeline lazily
        D3D__DirtyFlags |= (D3DDIRTYFLAG_COMBINERS | D3DDIRTYFLAG_SHADER_STAGE_PROGRAM);
        if(pDevice->m_ShaderUsesSpecFog != 0)
            D3D__DirtyFlags |= D3DDIRTYFLAG_SPECFOG_COMBINER;

        // D3DRS_TEXTUREFACTOR writes to the SetCombinerFactor registers, but
        // it's not handled lazily with the above, so we do the restore now
        D3DDevice_SetRenderState_TextureFactor(D3D__RenderState[D3DRS_TEXTUREFACTOR]);

        PPUSH pPush = pDevice->StartPush();

        // The bump-env registers map to different stages when a pixel shader
        // is no longer active
        pPush = CommonSetTextureBumpEnv(pDevice, pPush);

        // The fixed function pipeline always expects dependent texture 
        // lookups to run between stages i and i+1, so reset that now
        Push1(pPush, NV097_SET_SHADER_OTHER_STAGE_INPUT,
            (DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE1, _INSTAGE_0) |
             DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE2, _INSTAGE_1) |
             DRF_DEF (097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE3, _INSTAGE_2)));

        pDevice->EndPush(pPush + 2);
        
        // finished removing pixel shader
        return;
    }

    // at this point, pPixelShader != NULL
    // determine whether pixel shader uses the final combiner
    pDevice->m_ShaderUsesSpecFog = pPixelShader->pPSDef->PSFinalCombinerInputsABCD | 
                                   pPixelShader->pPSDef->PSFinalCombinerInputsEFG;
    
    // determine whether the pixel shade wants texture modes adjusted
    pDevice->m_ShaderAdjustsTexMode = pPixelShader->pPSDef->PSFinalCombinerConstants & 
                                                    (PS_GLOBALFLAGS_TEXMODE_ADJUST << 8);
    
    // If we made it here then we are installing a new pixel shader
    // or replacing a previous pixel shader with a new one.
    // Slam the bits into hardware.
    //
    // Is there anything to be gained by checking CombinerCount and only
    // downloading registers for active stages?  It seems like the data is
    // interleaved so much that this might require more pushbuffer data
    // than just dumping the whole structure.
    //
    
    D3DPIXELSHADERDEF *pPSDef = pPixelShader->pPSDef;
    
    // update texture modes according to currently set textures
    pDevice->m_PSShaderStageProgram = pPSDef->PSTextureModes;

    D3D__DirtyFlags |= D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;

    PPUSH pPush = pDevice->StartPush();

    // The bump-env registers map to different stages when a pixel shader
    // becomes active
    if (pOldShader == NULL)
        pPush = CommonSetTextureBumpEnv(pDevice, pPush);

    PushCount(pPush, NV097_SET_COMBINER_ALPHA_ICW(0), 8);
        //pushbuffer[1] = PSAlphaInputs0 (0x0260)
        //pushbuffer[2] = PSAlphaInputs1 (0x0264)
        //pushbuffer[3] = PSAlphaInputs2 (0x0268)
        //pushbuffer[4] = PSAlphaInputs3 (0x026c)
        //pushbuffer[5] = PSAlphaInputs4 (0x0270)
        //pushbuffer[6] = PSAlphaInputs5 (0x0274)
        //pushbuffer[7] = PSAlphaInputs6 (0x0278)
        //pushbuffer[8] = PSAlphaInputs7 (0x027c)
    memcpy((void*) (pPush + 1), &(pPSDef->PSAlphaInputs[0]), 8 * sizeof(DWORD));

    PushCount(pPush+9, NV097_SET_COMBINER_FACTOR0(0), 32);
        //pushbuffer[10] = PSConstant0 (0x0a60)
        //pushbuffer[11] = PSConstant2 (0x0a64)
        //pushbuffer[12] = PSConstant4 (0x0a68)
        //pushbuffer[13] = PSConstant6 (0x0a6c)
        //pushbuffer[14] = PSConstant8 (0x0a70)
        //pushbuffer[15] = PSConstant10 (0x0a74)
        //pushbuffer[16] = PSConstant12 (0x0a78)
        //pushbuffer[17] = PSConstant14 (0x0a7c)
        //pushbuffer[18] = PSConstant1 (0x0a80)
        //pushbuffer[19] = PSConstant3 (0x0a84)
        //pushbuffer[20] = PSConstant5 (0x0a88)
        //pushbuffer[21] = PSConstant7 (0x0a8c)
        //pushbuffer[22] = PSConstant9 (0x0a90)
        //pushbuffer[23] = PSConstant11 (0x0a94)
        //pushbuffer[24] = PSConstant13 (0x0a98)
        //pushbuffer[25] = PSConstant15 (0x0a9c)
        //pushbuffer[26] = PSAlphaOutputs0 (0x0aa0)
        //pushbuffer[27] = PSAlphaOutputs1 (0x0aa4)
        //pushbuffer[28] = PSAlphaOutputs2 (0x0aa8)
        //pushbuffer[29] = PSAlphaOutputs3 (0x0aac)
        //pushbuffer[30] = PSAlphaOutputs4 (0x0ab0)
        //pushbuffer[31] = PSAlphaOutputs5 (0x0ab4)
        //pushbuffer[32] = PSAlphaOutputs6 (0x0ab8)
        //pushbuffer[33] = PSAlphaOutputs7 (0x0abc)
        //pushbuffer[34] = PSRGBInputs0 (0x0ac0)
        //pushbuffer[35] = PSRGBInputs1 (0x0ac4)
        //pushbuffer[36] = PSRGBInputs2 (0x0ac8)
        //pushbuffer[37] = PSRGBInputs3 (0x0acc)
        //pushbuffer[38] = PSRGBInputs4 (0x0ad0)
        //pushbuffer[39] = PSRGBInputs5 (0x0ad4)
        //pushbuffer[40] = PSRGBInputs6 (0x0ad8)
        //pushbuffer[41] = PSRGBInputs7 (0x0adc)
    memcpy((void*) (pPush + 10), &(pPSDef->PSConstant0), 32 * sizeof(DWORD));
    
    Push1(pPush+42, NV097_SET_SHADER_CLIP_PLANE_MODE, pPSDef->PSCompareMode);

    PushCount(pPush+44, NV097_SET_SPECULAR_FOG_FACTOR(0), 2);
        //pushbuffer[45] = PSConstant16 (0x1e20)
        //pushbuffer[46] = PSConstant17 (0x1e24)
    memcpy((void*) (pPush + 45), &(pPSDef->PSFinalCombinerConstant0), 2 * sizeof(DWORD));
    
    PushCount(pPush+47, NV097_SET_COMBINER_COLOR_OCW(0), 9);
        //pushbuffer[48] = PSRGBOutputs0 (0x1e40)
        //pushbuffer[49] = PSRGBOutputs1 (0x1e44)
        //pushbuffer[50] = PSRGBOutputs2 (0x1e48)
        //pushbuffer[51] = PSRGBOutputs3 (0x1e4c)
        //pushbuffer[52] = PSRGBOutputs4 (0x1e50)
        //pushbuffer[53] = PSRGBOutputs5 (0x1e54)
        //pushbuffer[54] = PSRGBOutputs6 (0x1e58)
        //pushbuffer[55] = PSRGBOutputs7 (0x1e5c)
        //pushbuffer[56] = PSCombinerCount (0x1e60)
    memcpy((void*) (pPush + 48), &(pPSDef->PSRGBOutputs[0]), 9 * sizeof(DWORD));
    
    PushCount(pPush+57, NV097_SET_DOT_RGBMAPPING, 2);
        //pushbuffer[58] = PSDotMapping (0x1e74)
        //pushbuffer[59] = PSInputTexture (0x1e78)
    memcpy((void*) (pPush + 58), &(pPSDef->PSDotMapping), 2 * sizeof(DWORD));
    
        //PSTextureModes (0x1e70) is handled by D3DDIRTYFLAG_SHADER_STAGE_PROGRAM

    if(pDevice->m_ShaderUsesSpecFog != 0)
    {
        PushCount(pPush+60, NV097_SET_COMBINER_SPECULAR_FOG_CW0, 2);
            //pushbuffer[61] = PSFinalCombinerInputsABCD (0x0288)
            //pushbuffer[62] = PSFinalCombinerInputsEFG  (0x028c)
        memcpy((void*) (pPush + 61), &(pPSDef->PSFinalCombinerInputsABCD), 2 * sizeof(DWORD));
        
        pDevice->EndPush(pPush+63);
    }
    else
        pDevice->EndPush(pPush+60); // count without specfog registers
    
    // copy the new pixel shader state to the renderstate shadow if necessary
    if (!(pDevice->m_StateFlags & STATE_PUREDEVICE))
    {
        // copy all but last three DWORDS to renderstate shadow
        memcpy(&(D3D__RenderState[D3DRS_PSALPHAINPUTS0]), 
               pPSDef, 
               sizeof(D3DPIXELSHADERDEF) - 3*sizeof(DWORD));

        // D3DRS_PSTEXTUREMODES isn't grouped with the others
        D3D__RenderState[D3DRS_PSTEXTUREMODES] = pPSDef->PSTextureModes;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetPixelShader

extern "C"
void WINAPI D3DDevice_GetPixelShader(
    DWORD* pHandle) 
{ 
    COUNT_API(API_D3DDEVICE_GETPIXELSHADER);

    if (DBG_CHECK(TRUE))
    {
        if (pHandle == NULL)
        {
            DPF_ERR("Pixel shader handle pointer is NULL");
        }
    }
    
    *pHandle = (DWORD) g_pDevice->m_pPixelShader;
}

//------------------------------------------------------------------------------
// Float2UChar
//
// maps [0.0-1.0] to [0-255]

FORCEINLINE UCHAR Float2UChar(float in)
{
    if(in > 1.0f)
        in = 1.0f;
    else if(in < 0.0f)
        in = 0.0f;

    return (UCHAR)FloatToLong(in * 255.0f + 0.5f);
}

//------------------------------------------------------------------------------
// D3DDevice_SetPixelShaderConstant

extern "C"
void WINAPI D3DDevice_SetPixelShaderConstant(
    DWORD Register,
    CONST void* pConstantData,
    DWORD ConstantCount) 
{ 
    COUNT_API(API_D3DDEVICE_SETPIXELSHADERCONSTANT);
    
    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (pConstantData == NULL)
        {
            DPF_ERR("Invalid constant data pointer in SetPixelShaderConstant");
        }
        if ((Register + ConstantCount) > 16)  // maximum of 16 D3D constants
        {
            DPF_ERR("Too many constants specified in SetPixelShaderConstant");
        }
        if(pDevice->m_pPixelShader == NULL)
        {
            DPF_ERR("Attempt to set pixel shader constant with no active pixel shader.");
        }
    }
    
    // get the currently set pixel shader
    D3DPIXELSHADERDEF *pPsd = pDevice->m_pPixelShader->pPSDef;

    float *pFloatData = (float *)pConstantData;
    for (UINT i=0; i<ConstantCount; i++, Register++, pFloatData += 4) 
    {
        DWORD dVal = (((DWORD)Float2UChar(pFloatData[3])) << 24) |
                     (((DWORD)Float2UChar(pFloatData[0])) << 16) |
                     (((DWORD)Float2UChar(pFloatData[1])) <<  8) |
                     (((DWORD)Float2UChar(pFloatData[2]))      );

        // keep a copy of this constant
        pDevice->m_pPixelShaderConstants[Register] = dVal;

        // The pixelshaderdef struct has three dwords that map D3D constants 
        // to nVidia registers.  Scan the mapping to determine which registers
        // to write with this D3D constant.
        for(UINT r=0; r<8; r++) // check c0 registers
            if(((pPsd->PSC0Mapping >> (4*r)) & 0xf) == Register)
                pDevice->SetRenderState((D3DRENDERSTATETYPE)(D3DRS_PSCONSTANT0_0 + r), dVal);

        for(r=0; r<8; r++) // check c1 registers
            if((pPsd->PSC1Mapping >> (4*r) & 0xf) == Register)
                pDevice->SetRenderState((D3DRENDERSTATETYPE)(D3DRS_PSCONSTANT1_0 + r), dVal);
    
        for(r=0; r<2; r++) // check final combiner registers
            if((pPsd->PSFinalCombinerConstants >> (4*r) & 0xf) == Register)
                pDevice->SetRenderState((D3DRENDERSTATETYPE)(D3DRS_PSFINALCOMBINERCONSTANT0 + r), dVal);
        
        // if c0 is being set, also set the static eye vector
        if(Register == 0)
        {
            PPUSH pPush = pDevice->StartPush();
            // (c0.r, c0.g, c0.b) => (eye.x, eye.y, eye.z)
            PushCount(pPush, NV097_SET_EYE_VECTOR(0), 3);
                //pushbuffer[1] = eye.x
                //pushbuffer[2] = eye.y
                //pushbuffer[3] = eye.z
            memcpy((void*) (pPush + 1), &(pFloatData[0]), 3 * sizeof(DWORD));

            pDevice->EndPush(pPush + 4);
        }
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetPixelShaderConstant

extern "C"
void WINAPI D3DDevice_GetPixelShaderConstant(
    DWORD Register,
    void* pConstantData,
    DWORD ConstantCount) 
{ 
    COUNT_API(API_D3DDEVICE_GETPIXELSHADERCONSTANT);

    if (DBG_CHECK(TRUE))
    {
        if (pConstantData == NULL)
        {
            DPF_ERR("Invalid constant data pointer in GetPixelShaderConstant");
        }
        if ((Register + ConstantCount) > 16)
        {
            DPF_ERR("Too many constants specified in GetPixelShaderConstant");
        }
    }

    float *pFloatData = (float *)pConstantData;
    for (UINT i=0; i<ConstantCount; i++, Register++, pFloatData += 4) 
    {
        DWORD dVal = g_pDevice->m_pPixelShaderConstants[Register];
        // map [0-255] to [0.0-1.0] for each packed byte
        pFloatData[3] = ((FLOAT)((dVal >> 24) & 0xff))/255.0F;
        pFloatData[0] = ((FLOAT)((dVal >> 16) & 0xff))/255.0F;
        pFloatData[1] = ((FLOAT)((dVal >>  8) & 0xff))/255.0F;
        pFloatData[2] = ((FLOAT)((dVal      ) & 0xff))/255.0F;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetPixelShaderFunction

extern "C"
void WINAPI D3DDevice_GetPixelShaderFunction(
    DWORD Handle,
    D3DPIXELSHADERDEF* pPSDef)
{ 
    COUNT_API(API_D3DDEVICE_GETPIXELSHADERFUNCTION);

    PixelShader* pPixelShader = (PixelShader*) Handle;
    
    if (DBG_CHECK(TRUE))
    {
        if (pPixelShader == NULL)
        {
            DPF_ERR("Invalid pixel shader handle value");
        }

        #if DBG

            if (pPixelShader->Signature != 'Pshd')
            {
                DPF_ERR("Invalid pixel shader object (already deleted?)");
            }

        #endif
        
        if (pPSDef == NULL) 
        {
            DPF_ERR("Invalid pixel shader definition pointer");
        }
    }
    
    memcpy(pPSDef, pPixelShader->pPSDef, sizeof(D3DPIXELSHADERDEF));
}

} // end of namespace
