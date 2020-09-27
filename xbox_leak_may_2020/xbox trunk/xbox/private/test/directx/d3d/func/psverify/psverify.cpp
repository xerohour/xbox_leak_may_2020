/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    psverify.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "psgen.h"
#include "psverify.h"

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CPSVerify*  pPSVerify;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pPSVerify = new CPSVerify();
    if (!pPSVerify) {
        return FALSE;
    }

    // Initialize the scene
    if (!pPSVerify->Create(pDisplay)) {
        pPSVerify->Release();
        return FALSE;
    }

    bRet = pPSVerify->Exhibit(pnExitCode);

    // Clean up the scene
    pPSVerify->Release();

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CPSVerify
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CPSVerify
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
CPSVerify::CPSVerify() {

    m_pShaderGen = NULL;
    m_pxgbAsm = NULL;
    m_uPSGenerated = 0;
    m_uPSAssemblySuccess = 0;
    m_uPSAssemblyFailure = 0;
    m_uPSAssemblyCorrect = 0;
    m_uPSCreatedSuccess = 0;
    m_uPSCreatedFailure = 0;
}

//******************************************************************************
//
// Method:
//
//     ~CPSVerify
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
CPSVerify::~CPSVerify() {

    if (m_pShaderGen) {
        delete m_pShaderGen;
    }
    if (m_pxgbAsm) {
        m_pxgbAsm->Release();
    }
}

//******************************************************************************
//
// Method:
//
//     Prepare
//
// Description:
//
//     Initialize all device-independent data to be used in the scene.  This
//     method is called only once at creation (as opposed to Setup and
//     Initialize, which get called each time the device is Reset).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared, FALSE if it was not.
//
//******************************************************************************
BOOL CPSVerify::Prepare() {

    // Increase the size and width of the console buffer
    if (!xSetDisplayWidth(400)) {
        return FALSE;
    }

    if (!xSetOutputBufferSize(10922)) {
        return FALSE;
    }

    // Create the generator
    m_pShaderGen = new CShaderGenerator();
    if (!m_pShaderGen) {
        return FALSE;
    }

    // Seed the generator
    m_pShaderGen->Seed(GetTickCount());

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Update the state of the scene to coincide with the given time.
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
void CPSVerify::Update() {

    VerifyShaderAssembly();
//    VerifyShaderCreation();
}

//******************************************************************************
//
// Method:
//
//     Render
//
// Description:
//
//     Render the test scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CPSVerify::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

    m_pDisplay->EnableConsoleVisibility(TRUE);

#ifndef UNDER_XBOX
    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return TRUE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) {
                return FALSE;
            }
        }
    }
#endif // !UNDER_XBOX

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(150, 150, 150), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
}

//******************************************************************************
BOOL CPSVerify::VerifyShaderAssembly() {

    LPXGBUFFER  pxgbShader, pxgbErrors = NULL;
    DWORD       dwHandle;
    char        szTemp[81];
    UINT        i, uLen, uSize;
    HRESULT     hr;

    szTemp[80] = '\0';

    if (m_pxgbAsm) {
        m_pxgbAsm->Release();
        m_pxgbAsm = NULL;
    }

    // Generate a pixel shader
    if (!m_pShaderGen->GeneratePixelShader(&m_d3dpsdGen, &m_pxgbAsm)) {
        Log(LOG_ABORT, TEXT("Pixel shader generation failed for shader %d"), (UINT)m_fFrame);
        return FALSE;
    }

    m_uPSGenerated++;

    printf("\n\n********************************************************************************\n");
    for (i = 0, uSize = m_pxgbAsm->GetBufferSize(); i < uSize; i += 80) {
        uLen = i + 80 < uSize ? 80 : uSize - i;
        memcpy(szTemp, (LPBYTE)m_pxgbAsm->GetBufferPointer() + i, uLen);
//        printf(szTemp);
        if (GetStartupContext() & TSTART_STRESS) {
            xprintf(szTemp);
        }
        else {
            Log(LOG_TEXT, TEXT("%S"), szTemp);
        }
    }

    Render();

    // Assemble the shader
    hr = AssembleShader(NULL, m_pxgbAsm->GetBufferPointer(), m_pxgbAsm->GetBufferSize(), 0, NULL, 
                        &pxgbShader, &pxgbErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        UINT uErrorSize = pxgbErrors->GetBufferSize();
        LPSTR pszErrors = (LPSTR)MemAlloc(uErrorSize + 1);
        if (!pszErrors) {
            OutputDebugString(TEXT("Shader assembly failed (unable to allocate buffer to display errors)"));
            if ((GetStartupContext() & TSTART_STRESS) && hr != E_OUTOFMEMORY) {
                __asm int 3;
            }
            pxgbErrors->Release();
            return FALSE;
        }
        memcpy(pszErrors, pxgbErrors->GetBufferPointer(), uErrorSize);
        pxgbErrors->Release();
        pszErrors[uErrorSize] = '\0';
        DebugString(TEXT("\n"));
        Log(LOG_FAIL, TEXT("Shader assembly failed on shader %d:\n%S\n\nErrors:\n\n%S"), m_uPSGenerated, m_pxgbAsm->GetBufferPointer(), (LPSTR)pszErrors);
        if ((GetStartupContext() & TSTART_STRESS) && hr != E_OUTOFMEMORY) {
            __asm int 3;
        }
        MemFree(pszErrors);
        m_uPSAssemblyFailure++;
        return FALSE;
    }

    m_uPSAssemblySuccess++;

    if (pxgbErrors) {
        pxgbErrors->Release();
    }

    memcpy(&m_d3dpsdAsm, pxgbShader->GetBufferPointer(), sizeof(D3DPIXELSHADERDEF));

    pxgbShader->Release();

    // Verify the assembled shader
    if (!ComparePixelShaders(&m_d3dpsdAsm, &m_d3dpsdGen)) {
        LPXGBUFFER pxgb;
        Log(LOG_FAIL, TEXT("Pixel shader assembly failed to generate a correct pixel shader definition for shader %d"), m_uPSGenerated);
        m_pShaderGen->OutputPixelShaderDef(&m_d3dpsdAsm, &pxgb);
        Log(LOG_DETAIL, TEXT("Asssembled shader definition:\n\n%S"), pxgb->GetBufferPointer());
        pxgb->Release();
        m_pShaderGen->OutputPixelShaderDef(&m_d3dpsdGen, &pxgb);
        Log(LOG_DETAIL, TEXT("Expected shader definition:\n\n%S"), pxgb->GetBufferPointer());
        pxgb->Release();
        __asm int 3;
        return FALSE;
    }

    m_uPSAssemblyCorrect++;

    // Create a shader using the assembled definition
    hr = m_pDevice->CreatePixelShader(&m_d3dpsdAsm, &dwHandle);
    if (FAILED(hr)) {
        LPXGBUFFER pxgb;
        m_pShaderGen->OutputPixelShaderDef(&m_d3dpsdAsm, &pxgb);
        Log(LOG_FAIL, TEXT("Failed to create a pixel shader using the assembled D3DPIXELSHADERDEF for shader %d:\n%S"), m_uPSGenerated, pxgb->GetBufferPointer());
        __asm int 3;
        pxgb->Release();
        m_uPSCreatedFailure++;
        return FALSE;
    }
    m_pDevice->DeletePixelShader(dwHandle);

    m_uPSCreatedSuccess++;

    return TRUE;
}

//******************************************************************************
BOOL CPSVerify::VerifyShaderCreation() {

    LPXGBUFFER  pxgbShader;
    DWORD       dwHandle;
    char        szTemp[81];
    UINT        i, uLen, uSize;
    HRESULT     hr;

    szTemp[80] = '\0';

    // Generate a pixel shader
    if (!m_pShaderGen->GeneratePixelShader(&m_d3dpsdGen)) {
        Log(LOG_ABORT, TEXT("Pixel shader generation failed for shader %d"), (UINT)m_fFrame);
        return FALSE;
    }

    m_uPSGenerated++;

    m_pShaderGen->OutputPixelShaderDef(&m_d3dpsdGen, &pxgbShader);

    printf("\n\n********************************************************************************\n");
    for (i = 0, uSize = pxgbShader->GetBufferSize(); i < uSize; i += 80) {
        uLen = i + 80 < uSize ? 80 : uSize - i;
        memcpy(szTemp, (LPBYTE)pxgbShader->GetBufferPointer() + i, uLen);
        printf(szTemp);
        Render();
    }

    // Create a shader using the generated definition
    hr = m_pDevice->CreatePixelShader(&m_d3dpsdGen, &dwHandle);
    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("Failed to create a pixel shader using the generated D3DPIXELSHADERDEF for shader %d:\n%S"), m_uPSGenerated, pxgbShader->GetBufferPointer());
        pxgbShader->Release();
        m_uPSCreatedFailure++;
        return FALSE;
    }
    m_pDevice->DeletePixelShader(dwHandle);

    pxgbShader->Release();

    m_uPSCreatedSuccess++;

    return TRUE;
}

#define FIELD_NOT_EQUAL(val, mask, shift)   ((pd3dpsd1->val & ((mask) << (shift))) != (pd3dpsd2->val & ((mask) << (shift))))
#define OUTPUT_FIELDS(val, mask, shift)     ((pd3dpsd1->val >> (shift)) & (mask)), ((pd3dpsd2->val >> (shift)) & (mask))

//******************************************************************************
BOOL CPSVerify::ComparePixelShaders(D3DPIXELSHADERDEF* pd3dpsd1, D3DPIXELSHADERDEF* pd3dpsd2) {

    UINT i, uCount;
    BOOL bTexAdjust;
    BOOL bMatch = TRUE;

    bTexAdjust = (BOOL)(((pd3dpsd1->PSFinalCombinerConstants >> 8) & PS_GLOBALFLAGS_TEXMODE_ADJUST) || ((pd3dpsd1->PSFinalCombinerConstants >> 8) & PS_GLOBALFLAGS_TEXMODE_ADJUST));

    if (pd3dpsd1->PSTextureModes != pd3dpsd2->PSTextureModes) {
        for (i = 0; i < 4; i++) {
            if (FIELD_NOT_EQUAL(PSTextureModes, 0x1F, i * 5)) {
                DWORD dwMode1 = (pd3dpsd1->PSTextureModes >> (i * 5)) & 0x1F;
                DWORD dwMode2 = (pd3dpsd2->PSTextureModes >> (i * 5)) & 0x1F;
                if (!(bTexAdjust && (
                    ((dwMode1 == PS_TEXTUREMODES_PROJECT2D || dwMode1 == PS_TEXTUREMODES_PROJECT3D || dwMode1 == PS_TEXTUREMODES_CUBEMAP) &&
                     (dwMode2 == PS_TEXTUREMODES_PROJECT2D || dwMode2 == PS_TEXTUREMODES_PROJECT3D || dwMode2 == PS_TEXTUREMODES_CUBEMAP)) ||
                    ((dwMode1 == PS_TEXTUREMODES_DOT_STR_3D || dwMode1 == PS_TEXTUREMODES_DOT_STR_CUBE) &&
                     (dwMode2 == PS_TEXTUREMODES_DOT_STR_3D || dwMode2 == PS_TEXTUREMODES_DOT_STR_CUBE)))))
                {
                    Log(LOG_FAIL, TEXT("PSTextureModes: stage %d modes do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSTextureModes, 0x1F, i * 5));
                    bMatch = FALSE;
                }
            }
        }
    }

    if (pd3dpsd1->PSDotMapping != pd3dpsd2->PSDotMapping) {
        for (i = 0; i < 3; i++) {
            if (FIELD_NOT_EQUAL(PSDotMapping, 0x7, i * 4)) {
                DWORD dwMode1 = (pd3dpsd1->PSTextureModes >> ((i + 1) * 5)) & 0x1F;
                DWORD dwMode2 = (pd3dpsd2->PSTextureModes >> ((i + 1) * 5)) & 0x1F;
                if (dwMode1 == PS_TEXTUREMODES_DOT_ST || dwMode2 == PS_TEXTUREMODES_DOT_ST ||
                    dwMode1 == PS_TEXTUREMODES_DOT_ZW || dwMode2 == PS_TEXTUREMODES_DOT_ZW ||
                    dwMode1 == PS_TEXTUREMODES_DOT_RFLCT_DIFF || dwMode2 == PS_TEXTUREMODES_DOT_RFLCT_DIFF ||
                    dwMode1 == PS_TEXTUREMODES_DOT_RFLCT_SPEC || dwMode2 == PS_TEXTUREMODES_DOT_RFLCT_SPEC ||
                    dwMode1 == PS_TEXTUREMODES_DOT_STR_3D || dwMode2 == PS_TEXTUREMODES_DOT_STR_3D ||
                    dwMode1 == PS_TEXTUREMODES_DOT_STR_CUBE || dwMode2 == PS_TEXTUREMODES_DOT_STR_CUBE ||
                    dwMode1 == PS_TEXTUREMODES_DOTPRODUCT || dwMode2 == PS_TEXTUREMODES_DOTPRODUCT ||
                    dwMode1 == PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST || dwMode2 == PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST)
                {
                    Log(LOG_FAIL, TEXT("PSDotMapping: stage %d mappings do not match: 0x%X, 0x%X"), i + 1, OUTPUT_FIELDS(PSDotMapping, 0x7, i * 4));
                    bMatch = FALSE;
                }
            }
        }
    }

    if (pd3dpsd1->PSCompareMode != pd3dpsd2->PSCompareMode) {
        for (i = 0; i < 4; i++) {
            if (FIELD_NOT_EQUAL(PSCompareMode, 0xF, i * 4)) {
                Log(LOG_FAIL, TEXT("PSCompareMode: stage %d modes do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSCompareMode, 0xF, i * 4));
                bMatch = FALSE;
            }
        }
    }

    if (pd3dpsd1->PSInputTexture != pd3dpsd2->PSInputTexture) {
        if (FIELD_NOT_EQUAL(PSInputTexture, 0x1, 16)) {
            Log(LOG_FAIL, TEXT("PSInputTexture: stage 2 inputs do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSInputTexture, 0x1, 16));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSInputTexture, 0x2, 20)) {
            Log(LOG_FAIL, TEXT("PSInputTexture: stage 3 inputs do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSInputTexture, 0x2, 20));
            bMatch = FALSE;
        }
    }

    if (pd3dpsd1->PSCombinerCount != pd3dpsd2->PSCombinerCount) {
        if (FIELD_NOT_EQUAL(PSCombinerCount, 0xF, 0)) {
            Log(LOG_FAIL, TEXT("PSCombinerCount: Number of combiners do not match: %d, %d"), OUTPUT_FIELDS(PSCombinerCount, 0xF, 0));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSCombinerCount, PS_COMBINERCOUNT_MUX_MSB, 8)) {
            Log(LOG_FAIL, TEXT("PSCombinerCount: MUX_MSB count flags do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSCombinerCount, PS_COMBINERCOUNT_MUX_MSB, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSCombinerCount, PS_COMBINERCOUNT_UNIQUE_C0, 8)) {
            Log(LOG_FAIL, TEXT("PSCombinerCount: UNIQUE_C0 count flags do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSCombinerCount, PS_COMBINERCOUNT_UNIQUE_C0, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSCombinerCount, PS_COMBINERCOUNT_UNIQUE_C1, 8)) {
            Log(LOG_FAIL, TEXT("PSCombinerCount: UNIQUE_C1 count flags do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSCombinerCount, PS_COMBINERCOUNT_UNIQUE_C1, 8));
            bMatch = FALSE;
        }

        uCount = (pd3dpsd1->PSCombinerCount & 0xF) < (pd3dpsd2->PSCombinerCount & 0xF) ? (pd3dpsd1->PSCombinerCount & 0xF) : (pd3dpsd2->PSCombinerCount & 0xF);
    }
    else {
        uCount = pd3dpsd1->PSCombinerCount & 0xF;
    }

    for (i = 0; i < uCount; i++) {

        // RGB Inputs
        if (FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 4) || 
            FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 8) ||
            ((pd3dpsd1->PSRGBOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || 
            ((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD)
        {
            // AB and/or the SUM output register is set to something other than discard so compare the A and B register inputs
            if (pd3dpsd1->PSRGBInputs[i] != pd3dpsd2->PSRGBInputs[i]) {
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xF, 24)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: A registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xF, 24));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], PS_CHANNEL_ALPHA, 24)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: A channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], PS_CHANNEL_ALPHA, 24));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xE0, 24)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: A mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xE0, 24));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xF, 16)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: B registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xF, 16));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], PS_CHANNEL_ALPHA, 16)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: B channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], PS_CHANNEL_ALPHA, 16));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xE0, 16)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: B mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xE0, 16));
                    bMatch = FALSE;
                }
            }
        }

        if (FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 0) || 
            FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 8) ||
            ((pd3dpsd1->PSRGBOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || 
            ((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD)
        {
            // CD and/or the SUM output register is set to something other than discard so compare the C and D register inputs
            // AB and/or the SUM output register is set to something other than discard so compare the A and B register inputs
            if (pd3dpsd1->PSRGBInputs[i] != pd3dpsd2->PSRGBInputs[i]) {
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xF, 8)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: C registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xF, 8));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], PS_CHANNEL_ALPHA, 8)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: C channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], PS_CHANNEL_ALPHA, 8));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xE0, 8)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: C mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xE0, 8));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xF, 0)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: D registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xF, 0));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], PS_CHANNEL_ALPHA, 0)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: D channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], PS_CHANNEL_ALPHA, 0));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBInputs[i], 0xE0, 0)) {
                    Log(LOG_FAIL, TEXT("PSRGBInputs[%d]: D mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBInputs[i], 0xE0, 0));
                    bMatch = FALSE;
                }
            }
        }

        // RGB outputs
        if (pd3dpsd1->PSRGBOutputs[i] != pd3dpsd2->PSRGBOutputs[i]) {
            if (FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 4)) {
                Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: AB registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], 0xF, 4));
                bMatch = FALSE;
            }
            if (((pd3dpsd1->PSRGBOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd2->PSRGBOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD) {
                if (FIELD_NOT_EQUAL(PSRGBOutputs[i], PS_COMBINEROUTPUT_AB_DOT_PRODUCT, 12)) {
                    Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: AB dot product selects do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], PS_COMBINEROUTPUT_AB_DOT_PRODUCT, 12));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBOutputs[i], PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA, 12)) {
                    Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: AB alpha copy selects do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA, 12));
                    bMatch = FALSE;
                }
            }
            if (FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 0)) {
                Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: CD registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], 0xF, 0));
                bMatch = FALSE;
            }
            if (((pd3dpsd1->PSRGBOutputs[i]) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd2->PSRGBOutputs[i]) & 0xF) != PS_REGISTER_DISCARD) {
                if (FIELD_NOT_EQUAL(PSRGBOutputs[i], PS_COMBINEROUTPUT_CD_DOT_PRODUCT, 12)) {
                    Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: CD dot product selects do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], PS_COMBINEROUTPUT_CD_DOT_PRODUCT, 12));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSRGBOutputs[i], PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA, 12)) {
                    Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: CD alpha copy selects do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA, 12));
                    bMatch = FALSE;
                }
            }
            if (FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 8)) {
                Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: SUM registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], 0xF, 8));
                bMatch = FALSE;
            }
            if (((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd2->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) {
                if (FIELD_NOT_EQUAL(PSRGBOutputs[i], PS_COMBINEROUTPUT_AB_CD_MUX, 12)) {
                    Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: mux/sum selects do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], PS_COMBINEROUTPUT_AB_CD_MUX, 12));
                    bMatch = FALSE;
                }
            }
            if (FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 0) || 
                FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 4) || 
                FIELD_NOT_EQUAL(PSRGBOutputs[i], 0xF, 8) ||
                ((pd3dpsd1->PSRGBOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || 
                ((pd3dpsd1->PSRGBOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || 
                ((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD)
            {
                if (FIELD_NOT_EQUAL(PSRGBOutputs[i], 0x38, 12)) {
                    Log(LOG_FAIL, TEXT("PSRGBOutputs[%d]: mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSRGBOutputs[i], 0x38, 12));
                    bMatch = FALSE;
                }
            }
        }

        // Alpha inputs
        if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 4) || 
            FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 8) ||
            ((pd3dpsd1->PSAlphaOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || 
            ((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD)
        {
            // AB and/or the SUM output register is set to something other than discard so compare the A and B register inputs
            if (pd3dpsd1->PSAlphaInputs[i] != pd3dpsd2->PSAlphaInputs[i]) {
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xF, 24)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: A registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xF, 24));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 24)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: A channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 24));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xE0, 24)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: A mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xE0, 24));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xF, 16)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: B registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xF, 16));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 16)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: B channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 16));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xE0, 16)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: B mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xE0, 16));
                    bMatch = FALSE;
                }
            }
        }

        if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 0) || 
            FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 8) ||
            ((pd3dpsd1->PSAlphaOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || 
            ((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD)
        {
            // CD and/or the SUM output register is set to something other than discard so compare the C and D register inputs
            // AB and/or the SUM output register is set to something other than discard so compare the A and B register inputs
            if (pd3dpsd1->PSAlphaInputs[i] != pd3dpsd2->PSAlphaInputs[i]) {
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xF, 8)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: C registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xF, 8));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 8)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: C channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 8));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xE0, 8)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: C mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xE0, 8));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xF, 0)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: D registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xF, 0));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 0)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: D channels do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], PS_CHANNEL_ALPHA, 0));
                    bMatch = FALSE;
                }
                if (FIELD_NOT_EQUAL(PSAlphaInputs[i], 0xE0, 0)) {
                    Log(LOG_FAIL, TEXT("PSAlphaInputs[%d]: D mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaInputs[i], 0xE0, 0));
                    bMatch = FALSE;
                }
            }
        }

        // Alpha outputs
        if (pd3dpsd1->PSAlphaOutputs[i] != pd3dpsd2->PSAlphaOutputs[i]) {
            if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 4)) {
                Log(LOG_FAIL, TEXT("PSAlphaOutputs[%d]: AB registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaOutputs[i], 0xF, 4));
                bMatch = FALSE;
            }
            if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 0)) {
                Log(LOG_FAIL, TEXT("PSAlphaOutputs[%d]: CD registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaOutputs[i], 0xF, 0));
                bMatch = FALSE;
            }
            if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 8)) {
                Log(LOG_FAIL, TEXT("PSAlphaOutputs[%d]: SUM registers do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaOutputs[i], 0xF, 8));
                bMatch = FALSE;
            }
            if (((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd2->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) {
                if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], PS_COMBINEROUTPUT_AB_CD_MUX, 12)) {
                    Log(LOG_FAIL, TEXT("PSAlphaOutputs[%d]: mux/sum selects do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaOutputs[i], PS_COMBINEROUTPUT_AB_CD_MUX, 12));
                    bMatch = FALSE;
                }
            }
            if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 0) || 
                FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 4) || 
                FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0xF, 8) ||
                ((pd3dpsd1->PSAlphaOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || 
                ((pd3dpsd1->PSAlphaOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || 
                ((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD)
            {
                if (FIELD_NOT_EQUAL(PSAlphaOutputs[i], 0x38, 12)) {
                    Log(LOG_FAIL, TEXT("PSAlphaOutputs[%d]: mappings do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSAlphaOutputs[i], 0x38, 12));
                    bMatch = FALSE;
                }
            }
        }

        // Combiner constants
        if (((((pd3dpsd1->PSRGBOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSRGBInputs[i] >> 16) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd1->PSRGBInputs[i] >> 24) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSRGBInputs[i] >> 16) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSRGBInputs[i] >> 24) & 0xF) == PS_REGISTER_C0)) ||
            ((((pd3dpsd1->PSRGBOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSRGBInputs[i] >> 0) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd1->PSRGBInputs[i] >> 8) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSRGBInputs[i] >> 0) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSRGBInputs[i] >> 8) & 0xF) == PS_REGISTER_C0)) ||
            ((((pd3dpsd1->PSAlphaOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSAlphaInputs[i] >> 16) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd1->PSAlphaInputs[i] >> 24) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSAlphaInputs[i] >> 16) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSAlphaInputs[i] >> 24) & 0xF) == PS_REGISTER_C0)) ||
            ((((pd3dpsd1->PSAlphaOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSAlphaInputs[i] >> 0) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd1->PSAlphaInputs[i] >> 8) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSAlphaInputs[i] >> 0) & 0xF) == PS_REGISTER_C0 || ((pd3dpsd2->PSAlphaInputs[i] >> 8) & 0xF) == PS_REGISTER_C0)))
        {
            if (pd3dpsd1->PSConstant0[i] != pd3dpsd2->PSConstant0[i]) {
                Log(LOG_FAIL, TEXT("PSConstant0[%d]: constants do not match: 0x%X, 0x%X"), i, pd3dpsd1->PSConstant0[i], pd3dpsd2->PSConstant0[i]);
                bMatch = FALSE;
            }
            if (FIELD_NOT_EQUAL(PSC0Mapping, 0xF, i * 4)) {
                Log(LOG_FAIL, TEXT("PSC0Mapping: mappings for stage %d do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSC0Mapping, 0xF, i * 4));
                bMatch = FALSE;
            }
        }
        if (((((pd3dpsd1->PSRGBOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSRGBInputs[i] >> 16) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd1->PSRGBInputs[i] >> 24) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSRGBInputs[i] >> 16) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSRGBInputs[i] >> 24) & 0xF) == PS_REGISTER_C1)) ||
            ((((pd3dpsd1->PSRGBOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSRGBOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSRGBInputs[i] >> 0) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd1->PSRGBInputs[i] >> 8) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSRGBInputs[i] >> 0) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSRGBInputs[i] >> 8) & 0xF) == PS_REGISTER_C1)) ||
            ((((pd3dpsd1->PSAlphaOutputs[i] >> 4) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSAlphaInputs[i] >> 16) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd1->PSAlphaInputs[i] >> 24) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSAlphaInputs[i] >> 16) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSAlphaInputs[i] >> 24) & 0xF) == PS_REGISTER_C1)) ||
            ((((pd3dpsd1->PSAlphaOutputs[i] >> 0) & 0xF) != PS_REGISTER_DISCARD || ((pd3dpsd1->PSAlphaOutputs[i] >> 8) & 0xF) != PS_REGISTER_DISCARD) && (((pd3dpsd1->PSAlphaInputs[i] >> 0) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd1->PSAlphaInputs[i] >> 8) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSAlphaInputs[i] >> 0) & 0xF) == PS_REGISTER_C1 || ((pd3dpsd2->PSAlphaInputs[i] >> 8) & 0xF) == PS_REGISTER_C1)))
        {
            if (pd3dpsd1->PSConstant1[i] != pd3dpsd2->PSConstant1[i]) {
                Log(LOG_FAIL, TEXT("PSConstant1[%d]: constants do not match: 0x%X, 0x%X"), i, pd3dpsd1->PSConstant1[i], pd3dpsd2->PSConstant1[i]);
                bMatch = FALSE;
            }
            if (FIELD_NOT_EQUAL(PSC1Mapping, 0xF, i * 4)) {
                Log(LOG_FAIL, TEXT("PSC1Mapping: mappings for stage %d do not match: 0x%X, 0x%X"), i, OUTPUT_FIELDS(PSC1Mapping, 0xF, i * 4));
                bMatch = FALSE;
            }
        }
    }

    // Final combiner
    if (pd3dpsd1->PSFinalCombinerInputsABCD != pd3dpsd2->PSFinalCombinerInputsABCD) {
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xF, 24)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: A registers do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xF, 24));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 24)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: A channels do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 24));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xE0, 24)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: A mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xE0, 24));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xF, 16)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: B registers do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xF, 16));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 16)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: B channels do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 16));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xE0, 16)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: B mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xE0, 16));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xF, 8)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: C registers do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xF, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 8)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: C channels do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xE0, 8)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: C mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xE0, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xF, 0)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: D registers do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xF, 0));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 0)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: D channels do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, PS_CHANNEL_ALPHA, 0));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsABCD, 0xE0, 0)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsABCD: D mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsABCD, 0xE0, 0));
            bMatch = FALSE;
        }
    }

    if (pd3dpsd1->PSFinalCombinerInputsEFG != pd3dpsd2->PSFinalCombinerInputsEFG) {
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, 0xF, 24)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: E registers do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, 0xF, 24));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, PS_CHANNEL_ALPHA, 24)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: E channels do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, PS_CHANNEL_ALPHA, 24));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, 0xE0, 24)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: E mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, 0xE0, 24));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, 0xF, 16)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: F registers do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, 0xF, 16));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, PS_CHANNEL_ALPHA, 16)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: F channels do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, PS_CHANNEL_ALPHA, 16));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, 0xE0, 16)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: F mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, 0xE0, 16));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, 0xF, 8)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: G registers do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, 0xF, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, PS_CHANNEL_ALPHA, 8)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: G channels do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, PS_CHANNEL_ALPHA, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, 0xE0, 8)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: G mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, 0xE0, 8));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, PS_FINALCOMBINERSETTING_CLAMP_SUM, 0)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: clamp sum selects do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, PS_FINALCOMBINERSETTING_CLAMP_SUM, 0));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, PS_FINALCOMBINERSETTING_COMPLEMENT_V1, 0)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: complement v1 selects do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, PS_FINALCOMBINERSETTING_COMPLEMENT_V1, 0));
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerInputsEFG, PS_FINALCOMBINERSETTING_COMPLEMENT_R0, 0)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerInputsEFG: complement r0 selects do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerInputsEFG, PS_FINALCOMBINERSETTING_COMPLEMENT_R0, 0));
            bMatch = FALSE;
        }
    }

    // Final combiner constants and mappings
    if (((pd3dpsd1->PSFinalCombinerInputsABCD >> 0) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd1->PSFinalCombinerInputsABCD >> 8) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd1->PSFinalCombinerInputsABCD >> 16) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd1->PSFinalCombinerInputsABCD >> 24) & 0xF) == PS_REGISTER_C0 ||
        ((pd3dpsd1->PSFinalCombinerInputsEFG >> 8) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd1->PSFinalCombinerInputsEFG >> 16) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd1->PSFinalCombinerInputsEFG >> 24) & 0xF) == PS_REGISTER_C0 ||
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 0) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 8) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 16) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 24) & 0xF) == PS_REGISTER_C0 ||
        ((pd3dpsd2->PSFinalCombinerInputsEFG >> 8) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd2->PSFinalCombinerInputsEFG >> 16) & 0xF) == PS_REGISTER_C0 || 
        ((pd3dpsd2->PSFinalCombinerInputsEFG >> 24) & 0xF) == PS_REGISTER_C0)
    {
        if (pd3dpsd1->PSFinalCombinerConstant0 != pd3dpsd2->PSFinalCombinerConstant0) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerConstant0: constants do not match: 0x%X, 0x%X"), pd3dpsd1->PSFinalCombinerConstant0, pd3dpsd2->PSFinalCombinerConstant0);
            bMatch = FALSE;
        }
        if (FIELD_NOT_EQUAL(PSFinalCombinerConstants, 0xF, 0)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerConstants: C0 mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerConstants, 0xF, 0));
            bMatch = FALSE;
        }
    }

    if (((pd3dpsd1->PSFinalCombinerInputsABCD >> 0) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd1->PSFinalCombinerInputsABCD >> 8) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd1->PSFinalCombinerInputsABCD >> 16) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd1->PSFinalCombinerInputsABCD >> 24) & 0xF) == PS_REGISTER_C1 ||
        ((pd3dpsd1->PSFinalCombinerInputsEFG >> 8) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd1->PSFinalCombinerInputsEFG >> 16) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd1->PSFinalCombinerInputsEFG >> 24) & 0xF) == PS_REGISTER_C1 ||
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 0) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 8) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 16) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd2->PSFinalCombinerInputsABCD >> 24) & 0xF) == PS_REGISTER_C1 ||
        ((pd3dpsd2->PSFinalCombinerInputsEFG >> 8) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd2->PSFinalCombinerInputsEFG >> 16) & 0xF) == PS_REGISTER_C1 || 
        ((pd3dpsd2->PSFinalCombinerInputsEFG >> 24) & 0xF) == PS_REGISTER_C1)
    {
        if (pd3dpsd1->PSFinalCombinerConstant1 != pd3dpsd2->PSFinalCombinerConstant1) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerConstant1: constants do not match: 0x%X, 0x%X"), pd3dpsd1->PSFinalCombinerConstant1, pd3dpsd2->PSFinalCombinerConstant1);
            bMatch = FALSE;
        }

        if (FIELD_NOT_EQUAL(PSFinalCombinerConstants, 0xF, 4)) {
            Log(LOG_FAIL, TEXT("PSFinalCombinerConstants: C1 mappings do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerConstants, 0xF, 4));
            bMatch = FALSE;
        }
    }

    if (FIELD_NOT_EQUAL(PSFinalCombinerConstants, PS_GLOBALFLAGS_TEXMODE_ADJUST, 8)) {
        Log(LOG_FAIL, TEXT("PSFinalCombinerConstants: adjust texture flags do not match: 0x%X, 0x%X"), OUTPUT_FIELDS(PSFinalCombinerConstants, PS_GLOBALFLAGS_TEXMODE_ADJUST, 8));
        bMatch = FALSE;
    }

    return bMatch;
}

//******************************************************************************
//
// Method:
//
//     ProcessInput
//
// Description:
//
//     Process user input for the scene.
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
void CPSVerify::ProcessInput() {

    m_pDisplay->GetJoyState(&m_jsJoys, &m_jsLast);

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_BACK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
            m_bQuit = TRUE;
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_A)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_A)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                m_bPaused = !m_bPaused;
            }
            else {
                m_bPaused = TRUE;
                m_bAdvance = TRUE;
            }
        }
    }
}

//******************************************************************************
//
// Method:
//
//     Exhibit
//
// Description:
//
//     Display the scene.
//
// Arguments:
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
BOOL CPSVerify::Exhibit(int* pnExitCode) {

    BOOL            bMsgReady;
    float           fTime, fLastTime, fPrevTime, fTimeFreq;
    LARGE_INTEGER   qwCounter;
    DWORD           dwNumFrames = 0;
#ifndef UNDER_XBOX
    MSG             msg;
#endif // !UNDER_XBOX

    if (!m_bCreated) {
        return TRUE;
    }

    QueryPerformanceFrequency(&qwCounter);
    fTimeFreq = 1.0f / (float)qwCounter.QuadPart;
    QueryPerformanceCounter(&qwCounter);
    fLastTime = (float)qwCounter.QuadPart * fTimeFreq;
    fPrevTime = fLastTime;

    do {

#ifndef UNDER_XBOX
        // Pump messages
        if (m_pDisplay->IsActive()) {

            // When the application is active, check for new
            // messages without blocking to wait on them
            bMsgReady = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        }
        else {

            // The application has lost the focus and is suspended
            // so we can afford to block here until a new message
            // has arrived (and conserve CPU usage in the process)
            bMsgReady = GetMessage(&msg, NULL, 0, 0);
        }

        if (bMsgReady) {

            // If a message is ready, process it and proceed to
            // check for another message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (msg.message != WM_QUIT) {
#endif // !UNDER_XBOX

            // Otherwise process user input, update the next frame,
            // and draw it
            QueryPerformanceCounter(&qwCounter);
            fTime = (float)qwCounter.QuadPart * fTimeFreq;

            // Process user input
            m_pDisplay->ProcessInput();
            ProcessInput();

            if (m_pDisplay->m_bTimeSync) {
                m_pDisplay->m_bTimeSync = FALSE;
            }
            else if (!m_bPaused || m_bAdvance) {
                m_fTime += (fTime - fLastTime) * m_fTimeDilation;
                m_fFrame += 1.0f;//(1.0f * m_fTimeDilation); // ##REVIEW
                if (m_fFrameDuration != FLT_INFINITE && m_fFrame > m_fFrameDuration) {
                    break;
                }
                if (m_fTimeDuration != FLT_INFINITE && m_fTime > m_fTimeDuration) {
                    break;
                }
                m_fTimeDelta = m_fTime - m_fLastTime;
                m_fFrameDelta = m_fFrame - m_fLastFrame;
                Update();
                m_fLastTime = m_fTime;
                m_fLastFrame = m_fFrame;
                m_bAdvance = FALSE;
            }

            fLastTime = fTime;

            if (!Render()) {
                break;
            }

            dwNumFrames++;

            if (fTime - fPrevTime > 1.0f) {
                m_fFPS = (float)dwNumFrames / (fTime - fPrevTime);
                fPrevTime = fTime;
                dwNumFrames = 0;
            }
#ifndef UNDER_XBOX
        }

    } while (msg.message != WM_QUIT);
#else
    } while (!m_bQuit);
#endif // UNDER_XBOX

    if (pnExitCode) {
#ifndef UNDER_XBOX
        *pnExitCode = msg.wParam;
#else
        *pnExitCode = 0;
#endif // UNDER_XBOX
    }

    return m_bDisplayOK;
}
