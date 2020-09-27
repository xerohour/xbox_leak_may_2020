#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Lighting.h"


#ifdef TESTTYPE_LIGHTING

RENDERPRIMITIVEAPI Lighting::m_pDX6APIList[10] =
{
    RP_BE,                      // Begin/Vertex/End
    RP_BIE,                     // BeginIndexed/Index/End
    RP_DP,                      // DrawPrimitive
    RP_DIP,                     // DrawIndexedPrimitive
    RP_DPS,                     // DrawPrimitiveStrided
    RP_DIPS,                    // DrawIndexedPrimitiveStrided
    RP_DPVB,                    // DrawPrimitiveVB
    RP_DIPVB,                   // DrawIndexedPrimitiveVB
    RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
    RP_DIPVB_OPT                // DrawIndexedPrimitiveVB with optimized vertex buffer
};

RENDERPRIMITIVEAPI Lighting::m_pDX7APIList[8] =
{
    RP_DP,                      // DrawPrimitive
    RP_DIP,                     // DrawIndexedPrimitive
    RP_DPS,                     // DrawPrimitiveStrided
    RP_DIPS,                    // DrawIndexedPrimitiveStrided
    RP_DPVB,                    // DrawPrimitiveVB
    RP_DIPVB,                   // DrawIndexedPrimitiveVB
    RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
    RP_DIPVB_OPT                // DrawIndexedPrimitiveVB with optimized vertex buffer
};

RENDERPRIMITIVEAPI Lighting::m_pDX8APIList[6] =
{
    RP_DP,                      // DrawPrimitiveUp
    RP_DIP,                     // DrawIndexedPrimitive
    RP_DPVB,                    // DrawPrimitiveVB
    RP_DIPVB,                   // DrawIndexedPrimitiveVB
    RP_DPS,                     // DrawPrimitiveStrided
    RP_DIPS,                    // DrawIndexedPrimitiveStrided
};

DWORD Lighting::m_dwDX6APIListSize = 10;
DWORD Lighting::m_dwDX7APIListSize = 8;
DWORD Lighting::m_dwDX8APIListSize = 6;

#else

RENDERPRIMITIVEAPI Lighting::m_pDX6APIList[4] =
{
    RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
    RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
    RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DIPVB_OPT_PV             // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
};

RENDERPRIMITIVEAPI Lighting::m_pDX7APIList[6] =
{
    RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
    RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
    RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DPVB_PVS,                // DrawPrimitiveVB after ProcessVerticesStrided
    RP_DIPVB_PVS                // DrawIndexedPrimitiveVB after ProcessVerticesStrided
};

RENDERPRIMITIVEAPI Lighting::m_pDX8APIList[4] =
{
    RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
    RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
    RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
};

DWORD Lighting::m_dwDX6APIListSize = 4;
DWORD Lighting::m_dwDX7APIListSize = 6;
DWORD Lighting::m_dwDX8APIListSize = 4;

#endif

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

    Lighting*    pLighting;
    BOOL         bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pLighting = new Lighting();
    if (!pLighting) {
        return FALSE;
    }

    // Initialize the scene
    if (!pLighting->Create(pDisplay)) {
        pLighting->Release();
        return FALSE;
    }

    bRet = pLighting->Exhibit(pnExitCode);

    // Clean up the scene
    pLighting->Release();

    return bRet;
}

Lighting::Lighting() 
{
//	m_pMaterial = NULL;
	for (int i = 0; i < LT_LIGHTS; i++)
        memset(&m_Light[i], 0, sizeof(D3DLIGHT8));
//		m_pLight[i] = NULL;
	m_pSourceVertices = NULL;
	m_pReferenceVertices = NULL;
	m_pResultVertices = NULL;
#ifdef TESTTYPE_LIGHTING
	m_pPointVertices = NULL;
	m_pTriangleVertices = NULL;
    m_nPointVertices = 0;
    m_nTriangleVertices = 0;
#endif
    m_nVertices = 0;
    m_pSourceVertexBuffer = 0;
//    m_pOptimizedSourceVertexBuffer = 0;
//    m_pDestinationVertexBuffer = 0;

	m_bRasterize = false;
	m_dwRows = 0;
	m_dwColumns = 0;
	m_iStep = 5;
	m_iColorTollerance = 10;
	m_iVertexNumber = -1;
	m_dwNumFailed = 0;

	m_dwCurrentTestNumber = 0;

    m_ModeOptions.fTextures = false;
    m_ModeOptions.fZBuffer = false;
    m_ModeOptions.uMinDXVersion = 0x0600;

#ifdef TESTTYPE_LIGHTING
	m_szTestName = TEXT("Lighting");
	m_szCommandKey = TEXT("lighting");
#else
	m_szTestName = TEXT("PVLighting");
	m_szCommandKey = TEXT("pvlighting");
#endif
}

Lighting::~Lighting()
{
//	RELEASE(m_pMaterial);
//	for (int i = 0; i < LT_LIGHTS; i++)
//		RELEASE(m_pLight[i]);
    ReleaseVertexBuffer(m_pSourceVertexBuffer);
//    ReleaseVertexBuffer(m_pOptimizedSourceVertexBuffer);
//    ReleaseVertexBuffer(m_pDestinationVertexBuffer);

	if (m_pSourceVertices)
		delete [] m_pSourceVertices;
	if (m_pReferenceVertices)
		delete [] m_pReferenceVertices;
	if (m_pResultVertices)
		delete [] m_pResultVertices;
#ifdef TESTTYPE_LIGHTING
    if (m_pPointVertices)
        delete [] m_pPointVertices;
    if (m_pTriangleVertices)
        delete [] m_pTriangleVertices;
#endif
}

UINT Lighting::TestInitialize(void) 
{
	ReadInteger("STEP", 5, &m_iStep);

	if (m_dwVersion > 0x0600)
    {
        SetTestRange(1, 97200);
        
        // Check lighting support 
        if (m_d3dcaps.MaxActiveLights == 0)
        {
            WriteToLog("LightTest: Device does not support lighting\n");
            return D3DTESTINIT_SKIPALL;
        }
    }
	else
		SetTestRange(1, 5120);

    // Setting API list
    if (m_dwVersion == 0x0600)
        SetAPI(m_pDX6APIList, m_dwDX6APIListSize, 3);
    else if (m_dwVersion == 0x0700)
        SetAPI(m_pDX7APIList, m_dwDX7APIListSize, 3);
    else if (m_dwVersion == 0x0800)
        SetAPI(m_pDX8APIList, m_dwDX8APIListSize, 3);
    else
    {
        WriteToLog("%s: Incorrect DirectX version", m_szTestName);
        return D3DTESTINIT_ABORT;
    }
    
    
    if (!GenerateVertices())
		return D3DTESTINIT_ABORT;

#ifdef TESTTYPE_PVLIGHTING
    if (!InitializeVertexProcessing())
		return D3DTESTINIT_ABORT;
#endif

	if (!SetInitialProperties())
		return D3DTESTINIT_ABORT;

	if (KeySet("SHOW"))
	{
		m_bRasterize = true;
//		SetDisplayMode(DISPLAYMODE_WINDOW_SRCREFCMP);
	}
	else
	{
		m_bRasterize = false;
//		SetDisplayMode(DISPLAYMODE_WINDOW_SRCONLY);
	}
	
	// Initialize mode dependent class members
	m_dwWidth = m_pDisplay->GetWidth();
	m_dwHeight = m_pDisplay->GetHeight();

	ReadInteger("VERTEX", -1, &m_iVertexNumber);
	return D3DTESTINIT_RUN;
}

bool Lighting::ClearFrame(void)
{
    return Clear(RGB_MAKE(0,0,0));
}


bool Lighting::ExecuteTest(UINT uTestNumber) 
{
    D3DMATRIX           Matrix;
	int					iTest;

	ReadInteger("FRAME", uTestNumber, &iTest);
	m_dwCurrentTestNumber = iTest;
	
	// Set test parameters
    if (m_dwVersion > 0x0600)
	{
		if (!ResolveTest7(m_dwCurrentTestNumber))
			return false;
        if (!ValidateTest7(iTest))
            return false;
    }
	else
	{
//		if (!ResolveTest6(m_dwCurrentTestNumber))
//			return false;
	}

    // Write lights status
    {
        char*    pStatusString[2];
        for (int i = 0; i < 2; i++)
        {
            if (m_LightProperties[i].bLightActive && m_LightProperties[i].dwLightType == D3DLIGHT_DIRECTIONAL)
                pStatusString[i] = "Directional";
            else if (m_LightProperties[i].bLightActive && m_LightProperties[i].dwLightType == D3DLIGHT_POINT)
                pStatusString[i] = "Point";
//            else if (m_LightProperties[i].bLightActive && m_LightProperties[i].dwLightType == D3DLIGHT_PARALLELPOINT)
//                pStatusString[i] = "Parallelpoint";
            else if (m_LightProperties[i].bLightActive && m_LightProperties[i].dwLightType == D3DLIGHT_SPOT)
                pStatusString[i] = "Spot";
            else
                pStatusString[i] = "Not active";

        }
        WriteStatus("First light", pStatusString[0]);
        WriteStatus("Second light", pStatusString[1]);
    }
	
    BeginTestCase("General lighting test");

	if (!SetSceneProperties())
	{
		Fail();
		EndTestCase();
		m_dwNumFailed++;
		return false;
	}

	if (!SetModelProperties())
	{
		Fail();
		EndTestCase();
		m_dwNumFailed++;
		return false;
	}

	if (!SetLightProperties())
	{
		Fail();
		EndTestCase();
		m_dwNumFailed++;
		return false;
	}

#ifdef TESTTYPE_PVLIGHTING
    if (!ProcessSourceVertices())
    {
        Fail();
        EndTestCase();
        m_dwNumFailed++;
        return false;
	}
#endif

	if (m_dwVersion > 0x0600)
	{
		if (!ProcessReferenceVertices7())
		{
			Fail();
			EndTestCase();
			m_dwNumFailed++;
			return false;
		}
	}
	else
	{
		if (!ProcessReferenceVertices6())
		{
			Fail();
			EndTestCase();
			m_dwNumFailed++;
			return false;
		}
	}

	return true;
}

void Lighting::SceneRefresh(void) {
    if (BeginScene())
    {
#ifdef TESTTYPE_LIGHTING
        if (m_iVertexNumber >= 0)
            RenderPrimitive(D3DPT_POINTLIST, D3DFVF_NDSVERTEX, 
                &((NDSVERTEX*)m_pSourceVertices)[m_iVertexNumber], 1, NULL, 0, CD3D_SRC);
        else
        {
            RenderPrimitive(D3DPT_POINTLIST, D3DFVF_NDSVERTEX,
                m_pPointVertices, m_nPointVertices, NULL, 0, CD3D_SRC);
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_NDSVERTEX,
                m_pTriangleVertices, m_nTriangleVertices, NULL, 0, CD3D_SRC);
        }
#else
//        if (m_iVertexNumber >= 0)
//            DrawPrimitiveVB(D3DPT_POINTLIST, m_pDestinationVertexBuffer, NULL,
//                m_iVertexNumber, 1, NULL, 0, CD3D_SRC);
//        else
//            DrawPrimitiveVB(D3DPT_POINTLIST, m_pDestinationVertexBuffer, NULL,
//                0, m_nVertices, NULL, 0, CD3D_SRC);
#endif
        if (m_bRasterize)
        {
            if (m_iVertexNumber >= 0)
                RenderPrimitive(D3DPT_POINTLIST, D3DFVF_TLVERTEX, 
                    &((NDSVERTEX*)m_pReferenceVertices)[m_iVertexNumber], 1, NULL, 0, CD3D_REF);
            else
                RenderPrimitive(D3DPT_POINTLIST, D3DFVF_TLVERTEX,
                    m_pReferenceVertices, m_nVertices, NULL, 0, CD3D_REF);
        }

        EndScene();
    }
}

bool Lighting::CompareImages()
{
//	if (m_bRasterize && !CDirect3D::CompareImages(pSourceSurface, pRefSurface, pResultSurface) )
//		return false;

#ifdef TESTTYPE_LIGHTING
	if (!GetResultVertices())
		return false;
#endif
	
	return true;
}


bool Lighting::ProcessFrame(void) {
	
//	if (GetCompareResult(0.01f))
		Pass();
//	else
//	{
//		Fail();
//		m_dwNumFailed++;
//	}
	EndTestCase();
	return true;
}

bool Lighting::TestTerminate(void) {
//	RELEASE(m_pMaterial);
//	for (int i = 0; i < LT_LIGHTS; i++)
//		RELEASE(m_pLight[i]);
    ReleaseVertexBuffer(m_pSourceVertexBuffer);
    m_pSourceVertexBuffer = NULL;
//    RELEASE(m_pOptimizedSourceVertexBuffer);
//    RELEASE(m_pDestinationVertexBuffer);

	return true;
}


bool Lighting::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, -200.0f); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float               fZn= 1.f;
	float               fZf= 300.f;

    
	// Projection matrix
//	Matrix = ProjectionMatrix(pi/2.f, ((float)m_pSrcTarget->m_dwWidth / (float)m_pSrcTarget->m_dwHeight), 0.1f, 1000.f);
	Matrix = IdentityMatrix();
	Matrix._11 = 2.0f / 320;
	Matrix._22 = 2.0f / 280;
	Matrix._33 = 1.0f / (fZf - fZn);
	Matrix._43 = -fZn / (fZf - fZn);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
		return false;

    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;

	Matrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    return true;
}

bool Lighting::SetDefaultMaterials(void) {
    D3DMATERIAL8         Material;
    HRESULT              hr;

//    m_pMaterial = CreateMaterial();

//    if (NULL == m_pMaterial)
//        return false;

    ZeroMemory(&Material,sizeof(D3DMATERIAL8));
//    Material.dwSize=sizeof(D3DMATERIAL);
    Material.Diffuse.r  = 1.0f;
    Material.Diffuse.g  = 1.0f;
    Material.Diffuse.b  = 1.0f;
    Material.Diffuse.a  = 1.0f;

    Material.Ambient.r  = 1.0f;
    Material.Ambient.g  = 1.0f;
    Material.Ambient.b  = 1.0f;
    Material.Ambient.a  = 1.0f;

    Material.Specular.r = 1.0f;
    Material.Specular.g = 1.0f;
    Material.Specular.b = 1.0f;
    Material.Specular.a = 1.0f;


    Material.Emissive.r = 0.0f;
    Material.Emissive.g = 0.0f;
    Material.Emissive.b = 0.0f;
    Material.Emissive.a = 0.0f;

    Material.Power      = 20.0f;
//    Material.dwRampSize = 32;
//	Material.hTexture	= 0;

	memcpy(&m_ModelProperties.Material, &Material, sizeof(m_ModelProperties.Material));

//    if (!m_pMaterial->SetMaterial(&Material))
//        return false;

    // Now that the material data has been set,
    // re-set the material so the changes get picked up.

//    if (!SetMaterial(m_pMaterial))
//        return false;

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}


bool Lighting::SetDefaultLights(void) {
    D3DLIGHT8   Light;
    HRESULT     hr;

	ZeroMemory(&Light,sizeof(D3DLIGHT8));
//	Light.dwSize = sizeof(D3DLIGHT2);

	Light.Type           = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r        = 1.0f;
	Light.Diffuse.g        = 1.0f;
	Light.Diffuse.b        = 1.0f;
	Light.Diffuse.a        = 1.0f;
	Light.Position.x      = 0.0f;
	Light.Position.y      = 0.0f;
	Light.Position.z      = 0.0f;
	Light.Direction.x     = 0.0f;
	Light.Direction.y     = -1.0f / (float)sqrt(2);
	Light.Direction.z     = 1.0f / (float)sqrt(2);
	Light.Range           = D3DLIGHT_RANGE_MAX;
	Light.Attenuation0    = 0.f;
	Light.Attenuation1    = 0.f;
	Light.Attenuation2    = 0.f;

	for (int i = 0; i < LT_LIGHTS; i++)
	{
//		m_pLight[i] = CreateLight();

//		if (NULL == m_pLight[i])
//			return false;

//		m_pLight[i]->SetLight(&Light);

//		if (!SetLight(i,m_pLight[i]))
//			return false;

        memcpy(&m_Light[i], &Light, sizeof(D3DLIGHT8));

        hr = m_pDevice->SetLight(i, &Light);
        if (FAILED(hr)) {
            return false;
        }

        hr = m_pDevice->LightEnable(i, TRUE);
        if (FAILED(hr)) {
            return false;
        }
	}
	
	return true;
}


bool Lighting::SetDefaultLightStates(void) {
    if (m_dwVersion >= 0x0700) 
	{
		SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(128,128,128));
		SetRenderState(D3DRENDERSTATE_LOCALVIEWER,(DWORD)FALSE);
#ifndef UNDER_XBOX
		SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)(KeySet("CLIP") ? TRUE : FALSE));
#endif
	}
	else 
	{
//	    SetLightState(D3DLIGHTSTATE_AMBIENT,RGB_MAKE(50,50,50));
	}
    return true;
}

bool Lighting::SetDefaultRenderStates(void) {
#ifndef UNDER_XBOX
	SetRenderState(D3DRENDERSTATE_LASTPIXEL, (DWORD)FALSE);
#endif
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);
    if (m_dwVersion >= 0x0700) {
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);
	}
	return true;
}
