// FILE:        PShader.cpp
// DESC:        pixel shader conformance tests
// AUTHOR:      Cliff Hudson

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "PShader.h"

//CD3DWindowFramework App;

#define UVe 0.99

#ifndef D3DRGBA
    #define D3DRGBA(r, g, b, a) \
        ((D3DCOLOR) \
         (   (((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) \
         |   (((long)((g) * 255)) << 8) | (long)((b) * 255) \
         ))
#endif

MYTLVERTEX g_FourColorVertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0 },
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0 },
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0 }
};

MYTLVERTEX g_FourColorVertices2[4] =
{
    { 0, 1, 0, 1, D3DRGBA(0.5f, 0,	  0,    1), D3DRGBA(0,    0,    0,    1), 0.0, 1.0 },
    { 0, 0, 0, 1, D3DRGBA(0,    0.5f, 0,    1), D3DRGBA(0,    0,    0,    1), 0.0, 0.0 },
    { 1, 1, 0, 1, D3DRGBA(0.5f, 0.5f, 0.5f, 1), D3DRGBA(0.5f, 0.5f, 0.5f, 1), 1.0, 1.0 },
    { 1, 0, 0, 1, D3DRGBA(0,    0,    0.5f, 1), D3DRGBA(0.5f, 0.5f, 0.5f, 1), 1.0, 0.0 }
};

MYTLVERTEX g_FourBrickVertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 4.0 },
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 4.0, 4.0 },
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 4.0, 0.0 }
};

MYTLVERTEX g_FourColorAlphaVertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 0.5), D3DRGBA(0, 0, 0, 1), 0.0, 1.0 },
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 0.5), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 0.5), D3DRGBA(1, 1, 1, 1), 1.0, 1.0 },
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 0.5), D3DRGBA(1, 1, 1, 1), 1.0, 0.0 }
};

MYTLVERTEX g_TexKillVertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1),  0.0,  0.5 },
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), -0.5, -0.5 },
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1),  0.5,  0.5 },
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1),  0.5,  0.0 }
};

MYTLVERTEX g_IndexARGBVertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(  0, UVe,   0, UVe), D3DRGBA(0, 0, 0, 1), 0.0, 1.0 },
    { 0, 0, 0, 1, D3DRGBA(UVe, UVe, UVe, UVe), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
    { 1, 1, 0, 1, D3DRGBA(  0,   0,   0,   0), D3DRGBA(1, 1, 1, 1), 1.0, 1.0 },
    { 1, 0, 0, 1, D3DRGBA(UVe,   0, UVe,   0), D3DRGBA(1, 1, 1, 1), 1.0, 0.0 }
};

MYCUBETLVERTEX2 g_Cube2Vertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1, 1, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0, 1, 0, 0, 0, 1, 0 },
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1, 1, 0, 0, 0, 1, 0 },
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0, 1, 0, 0, 0, 1, 0 }
};

MYCUBETLVERTEX3 g_Cube3Vertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}
};

MYCUBETLVERTEX2 g_Texm2Vertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 2,	1, 0, 0,	0, 1, 0 },
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0,	1, 0, 0,	0, 1, 0 },
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 2, 2,	1, 0, 0,	0, 1, 0 },
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 2, 0,	1, 0, 0,	0, 1, 0 }
};

MYCUBETLVERTEX3 g_Texm3Vertices[4] =
{
    { 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    { 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    { 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    { 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}
};

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

    CPShader*       pPShaderTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 6 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pPShaderTest = new CPSOps();
                break;
            case 1:
                pPShaderTest = new CPSAddress();
                break;
            case 2:
                pPShaderTest = new CPSRegister();
                break;
            case 3:
                pPShaderTest = new CPSSrcMod();
                break;
            case 4:
                pPShaderTest = new CPSDestMod();
                break;
            case 5:
                pPShaderTest = new CPSInstMod();
                break;
        }

        if (!pPShaderTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pPShaderTest->Create(pDisplay)) {
            pPShaderTest->Release();
            return FALSE;
        }

        bRet = pPShaderTest->Exhibit(pnExitCode);

        bQuit = pPShaderTest->AbortedExit();

        // Clean up the scene
        pPShaderTest->Release();
    }

    return bRet;
}

CPShader::CPShader()
{
    m_szTestName = TEXT("PShader");
    m_szCommandKey = TEXT("PShader");

    m_nPass = m_nFail = 0;
    m_uNumShaders = 0;
    m_bStretch = true;
    m_bExit = FALSE;
}

void CPShader::CommandLineHelp()
{
}

UINT CPShader::TestInitialize()
{
    float x[4], y[4];

    if (!DoCapsCheck())
    {
        m_bSkipAllForCaps = true;
        return D3DTESTINIT_ABORT;
    }

    DWORD dwWidth = m_pDisplay->GetWidth();
    DWORD dwHeight = m_pDisplay->GetHeight();

    if (m_bStretch) {
        x[0] = (float)m_vpTest.X;
        y[0] = (float)(m_vpTest.Y + m_vpTest.Height);
        x[1] = (float)m_vpTest.X;
        y[1] = (float)m_vpTest.Y;
        x[2] = (float)(m_vpTest.X + m_vpTest.Width);
        y[2] = (float)(m_vpTest.Y + m_vpTest.Height);
        x[3] = (float)(m_vpTest.X + m_vpTest.Width);
        y[3] = (float)m_vpTest.Y;
    }
    else {
        x[0] = 0.0f;
        y[0] = 1.0f;
        x[1] = 0.0f;
        y[1] = 0.0f;
        x[2] = 1.0f;
        y[2] = 1.0f;
        x[3] = 1.0f;
        y[3] = 0.0f;
    }

    for (UINT i = 0; i < 4; i++)
    {
        g_FourColorVertices[i].x = x[i];
        g_FourColorVertices[i].y = y[i];

        g_FourColorVertices2[i].x = x[i];
        g_FourColorVertices2[i].y = y[i];

        g_FourBrickVertices[i].x = x[i];
        g_FourBrickVertices[i].y = y[i];

        g_FourColorAlphaVertices[i].x = x[i];
        g_FourColorAlphaVertices[i].y = y[i];

        g_TexKillVertices[i].x = x[i];
        g_TexKillVertices[i].y = y[i];

        g_IndexARGBVertices[i].x = x[i];
        g_IndexARGBVertices[i].y = y[i];

        g_Cube2Vertices[i].x = x[i];
        g_Cube2Vertices[i].y = y[i];

        g_Cube3Vertices[i].x = x[i];
        g_Cube3Vertices[i].y = y[i];

		g_Texm2Vertices[i].x = x[i];
		g_Texm2Vertices[i].y = y[i];

		g_Texm3Vertices[i].x = x[i];
		g_Texm3Vertices[i].y = y[i];
    }

    m_bSkipAllForCaps = false;
    return D3DTESTINIT_RUN;
}

bool CPShader::TestTerminate()
{
//    SHADERVECTOR::iterator iShader = m_vShaders.begin();
    UINT i;

//    while (iShader != m_vShaders.end())
    for (i = 0; i < m_uNumShaders; i++)
    {
        ReleaseVertexBuffer(m_Shaders[i].pSrcVB);
//        RELEASE(iShader->pRefVB);

        if (m_Shaders[i].dwHandle) {
            m_pDevice->SetPixelShader(NULL);
            m_pDevice->DeletePixelShader(m_Shaders[i].dwHandle);
        }

//        iShader++;
    }

	return true;
}


bool CPShader::ProcessFrame(void)
{
    bool bResult;
    char szBuffer[64];

    bResult = GetCompareResult(0.15f, 0.78f, 0);

    if (bResult)
    {
        (m_fIncrementForward) ? m_nPass++ : m_nPass--;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", m_nPass, m_nFail);
        Pass();
    }
    else
    {
        (m_fIncrementForward) ? m_nFail++ : m_nFail--;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", m_nPass, m_nFail);
        Fail();
    }

   WriteStatus("$gOverall Results",szBuffer);
   EndTestCase();
   return bResult;
}


void CPShader::AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha, DWORD dwRequired)
{
    char szCode[1024];
    HRESULT hResult;
//    SHADERVECTOR::iterator iShader = m_vShaders.insert(m_vShaders.end());
    PMYSHADER pShader = &m_Shaders[m_uNumShaders++];

    memset(pShader, 0, sizeof(MYSHADER));

    pShader->bSetup = true;
    pShader->pName = pName;
    pShader->pCode = pCode;
    pShader->dwConstantsStart = 0;
    pShader->dwHandle = 0;
    pShader->pSrcVB = NULL;
    pShader->pRefVB = NULL;
    pShader->dwVertexFormat = dwFVF;
    pShader->dwVertexFormatSize = GetVertexSize(dwFVF);
    pShader->dwRequired = dwRequired;
    pShader->bAlpha = bAlpha;

#ifdef UNDER_XBOX
	if (D3DSHADER_VERSION_MAJOR(dwRequired) == 6 && D3DSHADER_VERSION_MINOR(dwRequired) == 9) {
        sprintf(szCode, "xps.1.1\n%s", pCode);
    }
    else {
#endif

	    // Reset 0.5 pixel shaders to 1.0
	    if (D3DSHADER_VERSION_MAJOR(dwRequired) == 0 && D3DSHADER_VERSION_MINOR(dwRequired) == 5)
		    dwRequired = PSVERSION(1,0);

    //    if (VERSIONMASK(m_dwSupported) < VERSIONMASK(pShader->dwRequired) &&
    //		D3DSHADER_VERSION_MAJOR(pShader->dwRequired) != 255)
    //        return;

        sprintf(szCode, "ps.%d.%02d\n%s", D3DSHADER_VERSION_MAJOR(dwRequired),
                                        D3DSHADER_VERSION_MINOR(dwRequired), pCode);
#ifdef UNDER_XBOX
    }
#endif

#ifndef UNDER_XBOX

    DWORD dwFlags = 0;
    LPD3DXBUFFER pXCode = NULL;
    LPD3DXBUFFER pXError = NULL;
    hResult = D3DXAssembleShader(szCode, strlen(szCode), dwFlags, NULL, &pXCode, &pXError);

    if (FAILED(hResult))
    {
        WriteToLog("(ERROR) Pixel shader assembly failed:  %08X\n", hResult);
        WriteToLog("        Shader Name:\n%s\n", pName);
        WriteToLog("        Shader Code:\n%s", szCode);
        WriteToLog("        Error:\n%s\n", pXError->GetBufferPointer());
        pShader->bSetup = false;
    }
    else
    {
        if (FAILED(m_pDevice->CreatePixelShader((DWORD *) pXCode->GetBufferPointer(), &(pShader->dwHandle))))
        {
            WriteToLog("(ERROR) CreatePixelShader failed for shader '%s' - '%X'\n",
                       pName, GetLastError());
            pShader->bSetup = false;
        }
    }

    RELEASE(pXCode);
    RELEASE(pXError);

#else

    if (FAILED(m_pDevice->CreatePixelShaderAsm(szCode, strlen(szCode), &(pShader->dwHandle))))
    {
        WriteToLog("(ERROR) CreatePixelShader failed for shader '%s' - '%X'\n",
                   pName, GetLastError());
        pShader->bSetup = false;
    }

#endif

    if (!CreateLoadedVertexBuffers(dwFVF, pVertices, &(pShader->pSrcVB), &(pShader->pRefVB)))
    {
        pShader->bSetup = false;
    }
}

void CPShader::AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices)
{
    AddShaderDef(pName, pCode, dwFVF, pVertices, false, DEFAULT_PSVERSION);
} 

void CPShader::AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha)
{
    AddShaderDef(pName, pCode, dwFVF, pVertices, bAlpha, DEFAULT_PSVERSION);
} 

void CPShader::AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, DWORD dwRequired)
{
    AddShaderDef(pName, pCode, dwFVF, pVertices, false, dwRequired);
} 

void CPShader::AddTextureToLastShaderDef(CTexture8 * pTexture)
{
//    MYSHADER &Shader = m_vShaders.back();

//    Shader.vTextures.push_back(pTexture);

    if (m_uNumShaders) {
        MYSHADER* pShader = &m_Shaders[m_uNumShaders-1];
        pShader->pd3dt[pShader->uNumTextures++] = pTexture;
    }
}

void CPShader::SetConstantsStart(DWORD dwStart)
{
//    MYSHADER &Shader = m_vShaders.back();

//    Shader.dwConstantsStart = dwStart;

    if (m_uNumShaders) {
        MYSHADER* pShader = &m_Shaders[m_uNumShaders-1];
        pShader->dwConstantsStart = dwStart;
    }
}

void CPShader::AddConstantsToLastShaderDef(float a, float b, float c, float d)
{
//    MYSHADER &Shader = m_vShaders.back();
    
//    Shader.vConstants.push_back(a);
//    Shader.vConstants.push_back(b);
//    Shader.vConstants.push_back(c);
//    Shader.vConstants.push_back(d);

    if (m_uNumShaders) {
        MYSHADER* pShader = &m_Shaders[m_uNumShaders-1];
        pShader->fConstants[pShader->uNumConstants++] = a;
        pShader->fConstants[pShader->uNumConstants++] = b;
        pShader->fConstants[pShader->uNumConstants++] = c;
        pShader->fConstants[pShader->uNumConstants++] = d;
    }
}

void CPShader::AddConstantsToLastShaderDef(float f[4])
{
//    MYSHADER &Shader = m_vShaders.back();
    
//    Shader.vConstants.push_back(f[0]);
//    Shader.vConstants.push_back(f[1]);
//    Shader.vConstants.push_back(f[2]);
//    Shader.vConstants.push_back(f[3]);

    if (m_uNumShaders) {
        MYSHADER* pShader = &m_Shaders[m_uNumShaders-1];
        memcpy(&pShader->fConstants[pShader->uNumConstants], f, 4 * sizeof(float));
        pShader->uNumConstants += 4;
    }
}

void CPShader::AddConstantsToLastShaderDef(float *f, DWORD dwCount)
{
//    MYSHADER &Shader = m_vShaders.back();

//    for (DWORD d = 0; d < dwCount; d++)
//        Shader.vConstants.push_back(f[d]);

    if (m_uNumShaders) {
        MYSHADER* pShader = &m_Shaders[m_uNumShaders-1];
        for (DWORD d = 0; d < dwCount; d++) {
            pShader->fConstants[pShader->uNumConstants++] = f[d];
        }
    }
}

bool CPShader::CreateLoadedVertexBuffers(DWORD dwFVFType, void *pData, CVertexBuffer8 **ppSource, CVertexBuffer8 **ppRef)
{
    CVertexBuffer8 *pSrc, *pRef;

//    pSrc = CreateVertexBuffer(dwFVFType, 4, NULL, false);
    pSrc = CreateVertexBuffer(m_pDevice, pData, 4 * GetVertexSize(dwFVFType), 0, dwFVFType);

    if (!pSrc)
    {
        WriteToLog("(ERROR) Failed to create source vertex buffer. (%08X)\n", GetLastError());
        return false;
    }
/*
    pRef = CreateVertexBuffer(dwFVFType, 4, NULL, true);

    if (!pRef)
    {
        WriteToLog("(ERROR) Failed to create reference vertex buffer. (%08X)\n", GetLastError());
        RELEASE(pSrc);
        return false;
    }

    void *pVertexTarget = NULL;

    if (pSrc->Lock((VOID **) &pVertexTarget))
    {
        memcpy(pVertexTarget, pData, 4*GetVertexSize(dwFVFType));
        pSrc->Unlock();
        pVertexTarget = NULL;
    }
    else
    {
        WriteToLog("(ERROR) Failed to load source vertex buffer.\n");
        RELEASE(pSrc);
        RELEASE(pRef);
        return false;
    }

    if (pRef->Lock((VOID **) &pVertexTarget))
    {
        memcpy(pVertexTarget, pData, 4*GetVertexSize(dwFVFType));
        pRef->Unlock();
        pVertexTarget = NULL;
    }
    else
    {
        WriteToLog("(ERROR) Failed to load reference vertex buffer.\n");
        RELEASE(pSrc);
        RELEASE(pRef);
        return false;
    }
*/
    *ppSource = pSrc;
    *ppRef = NULL;//pRef;
    return true;
}


bool CPShader::SetupBasicScene(UINT uiTest)
{
//    MYSHADER &Shader = m_vShaders[uiTest - 1];
    MYSHADER &Shader = m_Shaders[uiTest - 1];

    if (FAILED(m_pDevice->SetPixelShader(Shader.dwHandle)))
    {
        WriteToLog("(ERROR) Failed to set pixel shader. (%08X)\n", GetLastError());
        return false;
    }

//    DWORD dwSrcHandle, dwRefHandle;

//    if (!GetPixelShaderHandles(Shader.dwHandle, &dwSrcHandle, &dwRefHandle))
//    {
//        WriteToLog("(ERROR) Unable to get pixel shader handles. (%08X)\n", GetLastError());
//        return false;
//    }
  
    if (!SetVertexShader(Shader.dwVertexFormat, Shader.dwVertexFormat))
    {
        WriteToLog("(ERROR) SetVertexShader failed. (%08X)\n", GetLastError());
        return false;
    }

    if (!SetStreamSource((DWORD) 0, Shader.pSrcVB, Shader.pRefVB, Shader.dwVertexFormatSize, (DWORD) 0)) 
    {
        WriteToLog("(ERROR) SetStreamSource failed. %08X\n", GetLastError());
        return false;
    }

    DWORD dwCurrentTexIndex = 0;
//    TEXTUREVECTOR &TexVec = Shader.vTextures;
//    TEXTUREVECTOR::iterator iCurrentTexture = TexVec.begin();
    UINT iCurrentTexture;

//    while (iCurrentTexture != TexVec.end())
//    {
//        SetTexture(dwCurrentTexIndex, *iCurrentTexture);
//        dwCurrentTexIndex++;
//        iCurrentTexture++;
//    }

if (strcmp(Shader.pName, "Address: texbem") == 0) {
VerifyTexture(Shader.pd3dt[0]);
}
    for (iCurrentTexture = 0; iCurrentTexture < Shader.uNumTextures; iCurrentTexture++) {
        SetTexture(dwCurrentTexIndex, Shader.pd3dt[iCurrentTexture]);
        SetTextureStageState(dwCurrentTexIndex++, D3DTSS_TEXCOORDINDEX, 0);
    }

    if (dwCurrentTexIndex < 4) {
        SetTexture(dwCurrentTexIndex, NULL);
    }

//    if (Shader.vConstants.size() > 0)
    if (Shader.uNumConstants > 0)
    {
//        if (((Shader.vConstants.size() % 4) != 0) || (Shader.vConstants.size() > 24))
        if (((Shader.uNumConstants % 4) != 0) || (Shader.uNumConstants > 24))
        {
            WriteToLog("(ERROR) Invalid number of constants specified\n");
            return false;
        }
        if (FAILED(m_pDevice->SetPixelShaderConstant(Shader.dwConstantsStart, &(Shader.fConstants[0]), Shader.uNumConstants/4)))
        {
            WriteToLog("(ERROR) SetPixelShaderConstant failed. (%08X)\n", GetLastError());
            return false;
        }
    }

    if (Shader.bAlpha)
        SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) TRUE);
    else
        SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) FALSE);

    SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);
#ifndef UNDER_XBOX
    SetRenderState(D3DRS_CLIPPING, (DWORD) FALSE);
#endif // !UNDER_XBOX
    SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);
    SetRenderState(D3DRS_ZENABLE, (DWORD) FALSE);
    SetRenderState(D3DRS_SRCBLEND, (DWORD) D3DBLEND_SRCALPHA);
    SetRenderState(D3DRS_DESTBLEND, (DWORD) D3DBLEND_INVSRCALPHA);

    SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    return true;
}

bool CPShader::ExecuteTest(UINT uiTest)
{
    char szBuffer[256];

	// Check if we need to skip 255 shader
#if 0
	if (D3DSHADER_VERSION_MAJOR(m_Shaders[uiTest-1].dwRequired) == 255 && !(m_Shaders[uiTest-1].bSetup))
	{
		SkipTests(1);
		return false;
	}
#endif

    sprintf(szBuffer, "%s", m_Shaders[uiTest - 1].pName);
    BeginTestCase(szBuffer);
    WriteStatus("Name:", m_Shaders[uiTest - 1].pName);

#if 0
	if (D3DSHADER_VERSION_MAJOR(m_Shaders[uiTest-1].dwRequired) != 255)
	{
        if (VERSIONMASK(m_Shaders[uiTest-1].dwRequired) > VERSIONMASK(m_dwSupported))
        {
            WriteToLog("(SKIP) Shader version %d.%d required, %d.%d supported.\n",
                       D3DSHADER_VERSION_MAJOR(m_Shaders[uiTest-1].dwRequired),
                       D3DSHADER_VERSION_MINOR(m_Shaders[uiTest-1].dwRequired),
                       D3DSHADER_VERSION_MAJOR(m_dwSupported),
                       D3DSHADER_VERSION_MINOR(m_dwSupported));
            SkipTests(1);
            return false;
        }

    //	WriteToLog("Code:\nps %d,%d\n%s",	D3DSHADER_VERSION_MAJOR(m_vShaders[uiTest-1].dwRequired),
    //										D3DSHADER_VERSION_MINOR(m_vShaders[uiTest-1].dwRequired),
    //										m_vShaders[uiTest - 1].pCode);

        if (!(m_Shaders[uiTest-1].bSetup))
        {
            WriteToLog("(FAIL) Shader setup failed.\n");
            Fail();
            return false;
        }
    }
#else
    if (!(m_Shaders[uiTest-1].bSetup))
    {
        WriteToLog("(FAIL) Shader setup failed.\n");
        Fail();
        return false;
    }
#endif

    if (!SetupBasicScene(uiTest))
    {
        WriteToLog("(FAIL) Basic scene setup failed.\n");
        Fail();
        return false;
    }

    return true;
}

void CPShader::SceneRefresh()
{
    if (BeginScene())
    {
        HRESULT hResult = m_pSrcDevice8->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

        if (D3D_OK != hResult)
        { 
            WriteToLog("(ERROR) DrawPrimitive on the source failed. %08X\n", hResult);
            Fail();
        }
/*
        hResult = m_pRefDevice8->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

        if (D3D_OK != hResult)
        {
            WriteToLog("(ERROR) DrawPrimitive on the reference failed. %08X\n", hResult);
            Fail();
        }
*/
        if (!EndScene())
        {
            WriteToLog("(ERROR) EndScene() failed: %08X\n", GetLastError());
            Fail();
        }
    }
    else
    {
        WriteToLog("(ERROR) BeginScene() failed: %08X\n", GetLastError());
        Fail();
    }
}

bool CPShader::DoCapsCheck()
{
	// Check if caps from 2 GetDeviceCaps() functions are in agreement
	if (m_dwVersion == 0x0800)
	{
		HRESULT		hr;
		D3DCAPS8	D3DCaps;
		
		// Source device
		hr = m_pSrcDevice8->GetDeviceCaps(&D3DCaps);
		if (FAILED(hr))
		{
			WriteToLog("PShader: GetDeviceCaps() call failed\n");
			return false;
		}
		else
		{
			if (D3DCaps.PixelShaderVersion != m_d3dcaps.PixelShaderVersion)
			{
				WriteToLog("PShader: Caps are not consistent on source device\n");
				return false;
			}
		}

		// Reference device
//		hr = m_pRefDevice8->GetDeviceCaps(&D3DCaps);
//		if (FAILED(hr))
//		{
//			WriteToLog("PShader: GetDeviceCaps() call failed\n");
//			return false;
//		}
//		else
//		{
//			if (D3DCaps.PixelShaderVersion != m_pAdapter->Devices[m_pMode->nRefDevice].Desc.dwPixelShaderVersion)
//			{
//				WriteToLog("PShader: Caps are not consistent on reference device\n");
//				return false;
//			}
//		}
	}

    bool bPass = false;
//    DEVICEDESC SrcDesc = m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;;

    if (m_dwVersion < 0x0800)
    {
        WriteToLog("ERROR: Test requires at least DirectX 8.0.\n");
    }
    else
    {
        m_dwSupported = m_d3dcaps.PixelShaderVersion;
        WriteToLog("Pixel shader version:  %8X\n", m_dwSupported);
        WriteToLog("Pixel shader version:  %d.%d\n", D3DSHADER_VERSION_MAJOR(m_dwSupported),
                                                     D3DSHADER_VERSION_MINOR(m_dwSupported));
        WriteToLog("Pixel shader maximum:  %f\n", m_d3dcaps.MaxPixelShaderValue);

        if (0xFFFF0000 != (m_dwSupported & 0xFFFF0000))
            WriteToLog("ERROR: High word of version (%4X) should be FFFF.\n",
                       (m_dwSupported >> 16));

        if ((D3DSHADER_VERSION_MAJOR(m_dwSupported) < 1) &&
            (D3DSHADER_VERSION_MINOR(m_dwSupported) < 5))
        {
            WriteToLog("Minimum pixel shader version of 0.5 required for this test.\n");
        }
        else
            bPass = true;
    }

    return bPass;
}

bool CPShader::UpdateCubicTexture8(CTexture8 *pSrc, CTexture8 *pDst)
{
    HRESULT hResult;

    hResult = m_pSrcDevice8->UpdateTexture(pSrc, pDst);

    if (FAILED(hResult))
    {
        WriteToLog("Source UpdateTexture() failed with HResult = %X.\n",
                   hResult);
        return false;
    }
/*
    if (NULL != m_pRefTarget)
    {
        hResult = m_pRefDevice8->UpdateTexture(pSrc->m_pRefCubeTexture8,
                                               pDst->m_pRefCubeTexture8);

        if (FAILED(hResult))
        {
            WriteToLog("Reference UpdateTexture() failed with HResult = %X.\n",
                       GetHResultString(hResult).c_str());
            return false;
        }
    }
*/
    return true;
}

CTexture8* CPShader::GenerateBrickTexture()
{
//	IMAGEDATA	*Data = new IMAGEDATA[64 * 64];
	int			i, j;
    D3DLOCKED_RECT d3dlr;
    LPDWORD Data;

#ifndef UNDER_XBOX
    CTexture8* pd3dt = (CTexture8*)CreateTexture(m_pDevice, 64, 64, D3DFMT_A8R8G8B8);
#else
    CTexture8* pd3dt = (CTexture8*)CreateTexture(m_pDevice, 64, 64, D3DFMT_LIN_A8R8G8B8);
#endif // UNDER_XBOX
    if (!pd3dt) {
        return NULL;
    }

    if (FAILED(pd3dt->LockRect(0, &d3dlr, NULL, 0))) {
        return NULL;
    }

	// Fill with (0, 0, -1) - (128, 128, 0)
//	for (i = 0; i < (64*64); i++)
//		Data[i].dwColor = (256 << 24) | (128 << 16) | (128 << 8) | (0 << 0);
    for (i = 0, Data = (LPDWORD)d3dlr.pBits; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            Data[j] = (256 << 24) | (128 << 16) | (128 << 8) | (0 << 0);
        }
        Data += d3dlr.Pitch / 4;
    }

	// Make "up" lines (0, 0.5, -0.866025) - (128, 191, 17)
//	for (i = 0; i < 8; i++)
//		for (j = 0; j < 64; j++)
//			Data[i*8*64 + j].dwColor = (256 << 24) | (128 << 16) | (191 << 8) | (17 << 0);
    for (i = 0, Data = (LPDWORD)d3dlr.pBits; i < 8; i++) {
        for (j = 0; j < 64; j++) {
            Data[j] = (256 << 24) | (128 << 16) | (191 << 8) | (17 << 0);
        }
        Data += d3dlr.Pitch / 4 * 8;
    }

	// Make "down" lines (0, -0.5, -0.866025) - (0, 64, 17)
//	for (i = 0; i < 8; i++)
//		for (j = 0; j < 64; j++)
//			Data[(i*8+7)*64 + j].dwColor = (256 << 24) | (128 << 16) | (64 << 8) | (17 << 0);
    for (i = 0, Data = (LPDWORD)d3dlr.pBits + (d3dlr.Pitch / 4 * 7); i < 8; i++) {
        for (j = 0; j < 64; j++) {
			Data[j] = (256 << 24) | (128 << 16) | (64 << 8) | (17 << 0);
        }
        Data += d3dlr.Pitch / 4 * 8;
    }

	// Make "left" lines (-0.5, 0, -0.866025) - (64, 128, 17)
	for (i = 0, Data = (LPDWORD)d3dlr.pBits; i < 8; i++)
		for (j = 0; j < 64; j++)
			if ( ((i & 0x01) && j & 0x08) || ( 0 == (i & 0x01) && 0 == (j & 0x08)))
//				Data[i*8 + j*64].dwColor = (256 << 24) | (64 << 16) | (128 << 8) | (17 << 0);
				Data[i*8 + j*(d3dlr.Pitch/4)] = (256 << 24) | (64 << 16) | (128 << 8) | (17 << 0);


	// Make "right" lines (0.5, 0, -0.866025) - (191, 128, 17)
	for (i = 0, Data = (LPDWORD)d3dlr.pBits; i < 8; i++)
		for (j = 0; j < 64; j++)
			if ( ((i & 0x01) && 0 == (j & 0x08)) || ( 0 == (i & 0x01) && (j & 0x08)))
//				Data[(i*8+7) + j*64].dwColor = (256 << 24) | (191 << 16) | (128 << 8) | (17 << 0);
				Data[(i*8+7) + j*(d3dlr.Pitch/4)] = (256 << 24) | (191 << 16) | (128 << 8) | (17 << 0);

//				Data[(i*8 + j*64) / 64 * (d3dlr.Pitch/4) + (i*8 + j*64) % 64] = (256 << 24) | (64 << 16) | (128 << 8) | (17 << 0);
//				Data[((i*8+7) + j*64) / 64 * (d3dlr.Pitch/4) + ((i*8+7) + j*64) % 64] = (256 << 24) | (191 << 16) | (128 << 8) | (17 << 0);


    if (FAILED(pd3dt->UnlockRect(0))) {
        return NULL;
    }

//	CImageData *pImageData = new CImageData();
//	if(false == pImageData->SetData(64, 64, Data))
//	{
//		delete pImageData;
//		delete [] Data;
//		return NULL;
//	}
//	delete [] Data;
//	return pImageData;

#ifndef UNDER_XBOX
    return pd3dt;
#else
    CTexture8* pd3dt2 = (CTexture8*)CreateTexture(m_pDevice, 64, 64, D3DFMT_A8R8G8B8);
    CopyTexture(pd3dt2, NULL, pd3dt, NULL, D3DX_FILTER_POINT);
    ReleaseTexture(pd3dt);
    return pd3dt2;
#endif // UNDER_XBOX
}

//******************************************************************************
BOOL CPShader::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CPShader::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CPShader::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    m_bExit = TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CD3DTest::WndProc(plr, hWnd, uMsg, wParam, lParam);
}

//******************************************************************************
void CPShader::VerifyTexture(CTexture8* pd3dt) {

    if (pd3dt == NULL) {
        OutputDebugString(TEXT("CPShader: The given texture pointer is NULL\r\n"));
        __asm int 3;
    }
    pd3dt->AddRef();
    pd3dt->Release();
}
