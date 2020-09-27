#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "texgen.h"

//CD3DWindowFramework	theApp;
//TexGen  			TestOne;


#define COLOR	255

D3DTLVERTEX TexGen::m_pBackVertices[4] =
{
	{  0.f, 255.f, 0.999f, 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 0.f,    0.f},
	{  0.f,   0.f, 0.999f, 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 0.999f, 0.f},
	{255.f,   0.f, 0.999f, 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 0.999f, 0.999f},
	{255.f, 255.f, 0.999f, 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 0.f,    0.999f},
};

/*
RENDERPRIMITIVEAPI TexGen::m_pAPIList[6] =
{
    RP_DP,
    RP_DIP,
    RP_DPS,
    RP_DIPS,
    RP_DPVB,
    RP_DIPVB
};
*/
RENDERPRIMITIVEAPI TexGen::m_pAPIList[6] =
{
    RP_DPVB,
    RP_DIPVB,
    RP_DP,
    RP_DIP,
    RP_DPS,
    RP_DIPS
};
DWORD TexGen::m_dwAPIListSize = 4;

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

    // Check for texgen support
    return (pd3dcaps->VertexProcessingCaps & D3DVTXPCAPS_TEXGEN);
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

    TexGen*  TestOne;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(786432, 32768);
        pDisplay->CreateDevice();
    }
#endif

    // Create the scene
    TestOne = new TexGen();
    if (!TestOne) {
        return FALSE;
    }

    // Initialize the scene
    if (!TestOne->Create(pDisplay)) {
        TestOne->Release();
        return FALSE;
    }

    bRet = TestOne->Exhibit(pnExitCode);

    // Clean up the scene
    TestOne->Release();

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(524288, 32768);
        pDisplay->CreateDevice();
    }
#endif

    return bRet;
}

TexGen::TexGen()
{
	m_pImageTexture = NULL;
//    m_pLight = NULL;
//    m_pMaterial = NULL;
    m_dwTexgenType = 0;

	m_pSphereVertices = NULL;
    m_pLocalVertices = NULL;
    m_pNonLocalVertices = NULL;
	m_pSphereIndices = NULL;
	m_nSphereVertices = 0;
	m_nSphereIndices = 0;		

	m_dwCurrentTestNumber = 0;
    m_dwSameTestNumber = 0;

	m_szTestName = TEXT("TexGen");
	m_szCommandKey = TEXT("texgen");

}

TexGen::~TexGen()
{
	if (m_pSphereVertices)
		delete [] m_pSphereVertices;
	if (m_pLocalVertices)
		delete [] m_pLocalVertices;
	if (m_pNonLocalVertices)
		delete [] m_pNonLocalVertices;
	if (m_pSphereIndices)
        delete [] m_pSphereIndices;

	ReleaseTexture(m_pImageTexture);
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
}


UINT TexGen::TestInitialize(void)
{
	CShapes*		pSphere = NULL;
//	CImageLoader    Loader;
//	CImageData*		pImageData;
	int				i, j, k;

    if (GetStartupContext() & TSTART_STRESS) {
        m_dwAPIListSize = 2;
    }

//	if (m_dwVersion == 0x0700)
    {
        SetAPI(m_pAPIList, m_dwAPIListSize, 3);
        m_dwSameTestNumber = m_dwAPIListSize * 3;
    }
    
    SetTestRange(1, m_dwSameTestNumber * 5 * 2 * 2);

    // Check for D3D version
    if (m_dwVersion < 0x0700)
    {   
        WriteToLog("TexGen: Test requires DX7 or later.\n");
        return D3DTESTINIT_SKIPALL;
    }

    // Check for texgen support
    if (NULL == (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
    {
        WriteToLog("TexGen: The driver does not support texture coordinate generation\n");
        return D3DTESTINIT_SKIPALL;
    }
    
	// Background vertices
	m_pBackVertices[2].sx = m_pBackVertices[3].sx = (float)m_pDisplay->GetWidth();
	m_pBackVertices[0].sy = m_pBackVertices[3].sy = (float)m_pDisplay->GetHeight();

	// Sphere
	pSphere = new CShapes;
	if (NULL == pSphere)
    {
		WriteToLog("TexGen: Not enough memory to create primitive\n");
        return D3DTESTINIT_ABORT;
    }
	if (!pSphere->NewShape(CS_SPHERE)) {
        return D3DTESTINIT_ABORT;
    }

//    pSphere->Scale(1.1f);

	// Sphere vertices
	m_nSphereVertices = pSphere->m_nVertices;
	m_pSphereVertices = new SPHEREVERTEX [m_nSphereVertices];
	if (NULL == m_pSphereVertices)
    {
		WriteToLog("TexGen: Not enough memory to create vertex array\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_nSphereVertices; i++)
	{
		m_pSphereVertices[i].x  = pSphere->m_pVertices[i].x;
		m_pSphereVertices[i].y  = pSphere->m_pVertices[i].y;
		m_pSphereVertices[i].z  = pSphere->m_pVertices[i].z;
		m_pSphereVertices[i].k  = (0.5f - (pSphere->m_pVertices[i].y));
		m_pSphereVertices[i].nx = pSphere->m_pVertices[i].nx;
		m_pSphereVertices[i].ny = pSphere->m_pVertices[i].ny;
		m_pSphereVertices[i].nz = pSphere->m_pVertices[i].nz;
		m_pSphereVertices[i].color = RGBA_MAKE(200, 200, 200, 255);
//      m_pSphereVertices[i].tu0 = 0.f;
//      m_pSphereVertices[i].tv0 = 0.f;
//      m_pSphereVertices[i].tu1 = 0.f;
//      m_pSphereVertices[i].tv1 = 0.f;
        m_pSphereVertices[i].tu0 = (pSphere->m_pVertices[i].y + 0.5f)*1.f;
        m_pSphereVertices[i].tv0 = (pSphere->m_pVertices[i].x + 0.5f)*1.f;
        m_pSphereVertices[i].tu1 = m_pSphereVertices[i].tu0;
        m_pSphereVertices[i].tv1 = (m_pSphereVertices[i].x*0.5f*(1.f + m_pSphereVertices[i].k) + 0.501f)*0.99f;
	}

    // Localviewer reflection vector vertices
	m_pLocalVertices = new SPHEREVERTEX [m_nSphereVertices];
	if (NULL == m_pLocalVertices)
    {
		WriteToLog("TexGen: Not enough memory to create vertex array\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_nSphereVertices; i++)
	{
		float rx, ry, rz;
		float vx, vy, vz;
        float nx, ny, nz;
        float norm;

        // Required reflection vector
        norm = 1.f / (float)sqrt(
            pSphere->m_pVertices[i].nx*pSphere->m_pVertices[i].nx + 
            pSphere->m_pVertices[i].ny*pSphere->m_pVertices[i].ny + 
            pSphere->m_pVertices[i].nz*pSphere->m_pVertices[i].nz);
        rx = pSphere->m_pVertices[i].nx * norm;
        ry = pSphere->m_pVertices[i].ny * norm;
        rz = pSphere->m_pVertices[i].nz * norm;

        // Calculate eye vector. We have identity model matrix and view point at (0, 0, -1)
        vx = -pSphere->m_pVertices[i].x;
        vy = -pSphere->m_pVertices[i].y;
        vz = -pSphere->m_pVertices[i].z - 1.f;
        norm = 1.f / (float)sqrt(vx*vx + vy*vy + vz*vz);
        vx *= norm; vy *= norm; vz *= norm;

        // Get new normal vector as normalized sum of rv and view
        nx = vx + rx;
        ny = vy + ry;
        nz = vz + rz;
        norm = 1.f / (float)sqrt(nx*nx + ny*ny + nz*nz);
        nx *= norm; ny *= norm; nz *= norm;
        
        m_pLocalVertices[i].x  = pSphere->m_pVertices[i].x;
		m_pLocalVertices[i].y  = pSphere->m_pVertices[i].y;
		m_pLocalVertices[i].z  = pSphere->m_pVertices[i].z;
		m_pLocalVertices[i].k  = (0.5f - (pSphere->m_pVertices[i].y/1.1f));
		m_pLocalVertices[i].nx = nx;
		m_pLocalVertices[i].ny = ny;
		m_pLocalVertices[i].nz = nz;
		m_pLocalVertices[i].color = RGBA_MAKE(200, 200, 200, 255);
//      m_pSphereVertices[i].tu0 = 0.f;
//      m_pSphereVertices[i].tv0 = 0.f;
//      m_pSphereVertices[i].tu1 = 0.f;
//      m_pSphereVertices[i].tv1 = 0.f;
        m_pLocalVertices[i].tu0 = (pSphere->m_pVertices[i].y + 0.501f)*0.99f;
        m_pLocalVertices[i].tv0 = (pSphere->m_pVertices[i].x + 0.501f)*0.99f;
        m_pLocalVertices[i].tu1 = m_pSphereVertices[i].tu0;
        m_pLocalVertices[i].tv1 = (m_pSphereVertices[i].x*0.5f*(1.f + m_pSphereVertices[i].k) + 0.501f)*0.99f;
	}

    // NonLocal viewer reflection vector vertices
	m_pNonLocalVertices = new SPHEREVERTEX [m_nSphereVertices];
	if (NULL == m_pNonLocalVertices)
    {
		WriteToLog("TexGen: Not enough memory to create vertex array\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_nSphereVertices; i++)
	{
		float rx, ry, rz;
        float nx, ny, nz;
        float norm;

        // Required reflection vector
        norm = 1.f / (float)sqrt(
            pSphere->m_pVertices[i].nx*pSphere->m_pVertices[i].nx + 
            pSphere->m_pVertices[i].ny*pSphere->m_pVertices[i].ny + 
            pSphere->m_pVertices[i].nz*pSphere->m_pVertices[i].nz);
        rx = pSphere->m_pVertices[i].nx * norm;
        ry = pSphere->m_pVertices[i].ny * norm;
        rz = pSphere->m_pVertices[i].nz * norm;

        // Get new normal vector as normalized sum of rv and view (0, 0, -1)
        nx = rx;
        ny = ry;
        nz = rz - 1.f;
        norm = 1.f / (float)sqrt(nx*nx + ny*ny + nz*nz);
        nx *= norm; ny *= norm; nz *= norm;
        
		m_pNonLocalVertices[i].x  = pSphere->m_pVertices[i].x;
		m_pNonLocalVertices[i].y  = pSphere->m_pVertices[i].y;
		m_pNonLocalVertices[i].z  = pSphere->m_pVertices[i].z;
		m_pNonLocalVertices[i].k  = (0.5f - (pSphere->m_pVertices[i].y/1.1f));
		m_pNonLocalVertices[i].nx = nx;
		m_pNonLocalVertices[i].ny = ny;
		m_pNonLocalVertices[i].nz = nz;
		m_pNonLocalVertices[i].color = RGBA_MAKE(200, 200, 200, 255);
//      m_pSphereVertices[i].tu0 = 0.f;
//      m_pSphereVertices[i].tv0 = 0.f;
//      m_pSphereVertices[i].tu1 = 0.f;
//      m_pSphereVertices[i].tv1 = 0.f;
        m_pNonLocalVertices[i].tu0 = (pSphere->m_pVertices[i].y + 0.501f)*0.99f;
        m_pNonLocalVertices[i].tv0 = (pSphere->m_pVertices[i].x + 0.501f)*0.99f;
        m_pNonLocalVertices[i].tu1 = m_pSphereVertices[i].tu0;
        m_pNonLocalVertices[i].tv1 = (m_pSphereVertices[i].x*0.5f*(1.f + m_pSphereVertices[i].k) + 0.501f)*0.99f;
	}


    // Sphere indices
    m_nSphereIndices = pSphere->m_nIndices;
	m_pSphereIndices = new WORD [m_nSphereIndices];
	if (NULL == m_pSphereIndices)
    {
		WriteToLog("TexGen: Not enough memory to create vertex array\n");
		return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_nSphereIndices; i++)
		m_pSphereIndices[i] = pSphere->m_pIndices[i];
    delete pSphere;


    // Background texture
    DWORD           dwColors[4] = {0x0000ff00, 0x00ff0000, 0x00ffffff, 0x000000ff};
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    HRESULT         hr;
//    pImageData = Loader.LoadGradient(256, 256, dwColors);
    m_pImageTexture = CreateGradientTexture(m_pDevice, 256, 256, dwColors);
	if (m_pImageTexture == NULL)
    {
		WriteToLog("TexGen: Error creating RGB texture\n");
        return D3DTESTINIT_ABORT;
    }

    hr = m_pImageTexture->LockRect(0, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        return D3DTESTINIT_ABORT;
    }

    pdwPixel = (LPDWORD)d3dlr.pBits;

    // Draw vertical black lines on the texture
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 256; j++)
        {
            for (k = 0; k < 16; k++)
            {
//                int index = 256*j + 32*i + k;
                int index = (d3dlr.Pitch / 4) * j + 32 * i + k;
//                (pImageData->GetData())[index].nRed = 255 - (pImageData->GetData())[index].nRed;
//                (pImageData->GetData())[index].nGreen = 255 - (pImageData->GetData())[index].nGreen;
//                (pImageData->GetData())[index].nBlue = 255 - (pImageData->GetData())[index].nBlue;
                pdwPixel[index] = (0xFF - ((pdwPixel[index] >> 16) & 0xFF)) << 16 |
                                  (0xFF - ((pdwPixel[index] >> 8 ) & 0xFF)) << 8  |
                                  (0xFF - ((pdwPixel[index]      ) & 0xFF));
            }
        }
    }

    hr = m_pImageTexture->UnlockRect(0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        return D3DTESTINIT_ABORT;
    }

	return D3DTESTINIT_RUN;
}


bool TexGen::ExecuteTest(UINT iTestNumber)
{
	m_dwCurrentTestNumber = (DWORD)iTestNumber;
	
    if (!SetProperties())
        return false;
	BeginTestCase("");

    return true;
}

void TexGen::SceneRefresh(void)
{
    
	if (BeginScene())
	{
        SPHEREVERTEX* pVertices = m_pSphereVertices;

        if (3 == m_dwTestType)
            pVertices = m_pLocalVertices;
        else if (4 == m_dwTestType)
            pVertices = m_pNonLocalVertices;
        
        SetTexture(0, m_pImageTexture);
        SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
        SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_COUNT2);
        RenderPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, m_pBackVertices, 4, 
            NULL, 0, D3DDP_DONOTUPDATEEXTENTS);
		
        // Set texgen
        SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, m_dwTexgenType);
		SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_COUNT2);
        RenderPrimitive(D3DPT_TRIANGLELIST, 
            D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2, 
            pVertices, m_nSphereVertices, 
            m_pSphereIndices, m_nSphereIndices, 0);

		EndScene();
	}
    WriteStatus("Texgen type", m_pszTestName);
}


bool TexGen::ProcessFrame(void)
{
	bool    bResult;

    // This range (0.00002) basically allows 1 pixel to be different
    bResult = GetCompareResult(0.15f, 0.78f, 0);

    if (!bResult) {
		WriteToLog("Test fails on frame %d\n", m_dwCurrentTestNumber);
		Fail();
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

bool TexGen::TestTerminate(void)
{
	if (m_pSphereVertices)
	{
		delete [] m_pSphereVertices;
		m_pSphereVertices = NULL;
		m_nSphereVertices = 0;
	}
	if (m_pLocalVertices)
    {
        delete [] m_pLocalVertices;
		m_pLocalVertices = NULL;
    }
	if (m_pNonLocalVertices)
    {
		delete [] m_pNonLocalVertices;
        m_pNonLocalVertices = NULL;
    }
	if (m_pSphereIndices)
	{
		delete [] m_pSphereIndices;
		m_pSphereIndices = NULL;
		m_nSphereIndices = 0;
	}

    ReleaseTexture(m_pImageTexture);
    m_pImageTexture = NULL;
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
	return true;
}
