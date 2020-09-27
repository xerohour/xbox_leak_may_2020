////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation, 1999.
//
// CCubemap.cpp
//
// CCubemap class - Conformance test for cubemap textures
//
// History: 12/20/98   Jeff Vezina   - Created
//
////////////////////////////////////////////////////////////////////////////////////////////
//
// Test Description:
//    Test consists of 3 parts.  First part rotates yaw by 10 degrees increments (default)
//    for each test.  Next part rotates pitch by 10 degrees increments (default) for each
//    test.  Final part creates a mipmapped cubemap and rotates pitch by 45 degrees
//    increments for 3 distances within each of the 3 mipmap levels.
//
////////////////////////////////////////////////////////////////////////////////////////////

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Cubemap.h"

// Sphere information
#define SPH_NUM 20
#define PI ((float)3.14159265358979)
#define D2R ((float)0.0174532925199433)
#define R2D ((float)57.2957795130823)
#define SPH_TRIS (2*(SPH_NUM*2)+(SPH_NUM-2)*(SPH_NUM*4))
#define SPH_VTXS (SPH_TRIS*3)
#define SPH_RADIUS (2.0f)

//CD3DWindowFramework TheApp;

#ifdef UNDER_XBOX
#define DEMO_HACK
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

    CCubemap*   pCubeTest;
    BOOL        bQuit = FALSE, bRet = TRUE;
    UINT        i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 6 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pCubeTest = new CYawRotation();
                break;
            case 1:
                pCubeTest = new CPitchRotation();
                break;
            case 2:
                pCubeTest = new CMipmaps();
                break;
            case 3:
                pCubeTest = new CTexGen();
                break;
            case 4:
                pCubeTest = new CCubeFilter();
                break;
            case 5:
                pCubeTest = new CCubeBlend();
                break;
        }

        if (!pCubeTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pCubeTest->Create(pDisplay)) {
            pCubeTest->Release();
            return FALSE;
        }

        bRet = pCubeTest->Exhibit(pnExitCode);

        bQuit = pCubeTest->AbortedExit();

        // Clean up the scene
        pCubeTest->Release();
    }

    return bRet;
}

CCubemap::CCubemap()
{
    D3DFORMAT fmt[] = {
        D3DFMT_A8R8G8B8,
        D3DFMT_X8R8G8B8,
        D3DFMT_R5G6B5,
        D3DFMT_A1R5G5B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_A4R4G4B4,
//        D3DFMT_DXT1,
//        D3DFMT_DXT2,
//        D3DFMT_DXT3,
//        D3DFMT_DXT4,
//        D3DFMT_DXT5,
    };

//	m_paMaterial=NULL;
	m_paVertex=NULL;
	m_paTGVertex=NULL;
	m_paTexture=NULL;
	m_paSysTexture=NULL;
	m_cVertexMax=0;

	m_flZDepthMin=3.0f;				// Min distance for yaw/pitch rotation and first mipmap level
	m_flZDepthMax=15.0f;			// Max distance for last mipmap level

	m_dTextureWidthMax=256;			// Maximum texture width (top mipmap level)
	m_dTextureHeightMax=256;		// Maximum texture height (top mipmap level)

    bTexGen = false;

    bD3DManage = false;
    bManage = false;
    bBlt = false;
    bCopy = false;

    bVideo = true;
    bLock = true;
    bUpdate = true;

    // Disable textures
    m_ModeOptions.fTextures = false;

    // Enable cubemaps
    m_ModeOptions.fCubemaps = true;

    m_uCommonCubeFormats = countof(fmt);
    memcpy(m_fmtCommonCube, fmt, m_uCommonCubeFormats * sizeof(D3DFORMAT));

    m_bExit = FALSE;
}

CCubemap::~CCubemap()
{
}

bool CCubemap::SetDefaultMaterials(void)
{
	D3DMATERIAL8 Material;
    HRESULT      hr;

//	m_paMaterial=CreateMaterial();
//	if (m_paMaterial==NULL)
//	{
//		WriteToLog("CreateMaterial() failed with HResult = %X.\n",GetLastError());
//        return false;
//	}

	ZeroMemory(&Material,sizeof(D3DMATERIAL8));
//	Material.dwSize=sizeof(D3DMATERIAL);

    // No alpha
	Material.Diffuse.a=1.0f;

    // White 
    Material.Ambient.r=1.0f;
	Material.Ambient.g=1.0f;
	Material.Ambient.b=1.0f;

//	if (!m_paMaterial->SetMaterial(&Material))
//	{
//		WriteToLog("SetMaterial() failed with HResult = %X.\n",GetLastError());
//        return false;
//	}

    // Now that the material data has been set,
    // re-set the material so the changes get picked up.
//	if (!SetMaterial(m_paMaterial))
//	{
//		WriteToLog("SetMaterial() failed with HResult = %X.\n",GetLastError());
//        return false;
//	}

//	return true;

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}

bool CCubemap::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRENDERSTATE_AMBIENT,(DWORD)0xFFFFFFFF);

    if (bTexGen)
        SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS,(DWORD)TRUE);

	return true;
}

bool CCubemap::SetDefaultMatrices(void)
{
	D3DMATRIX Matrix;

#ifndef UNDER_XBOX
	Matrix=ProjectionMatrix(pi/2.0f,320.0f/280.0f,1.0f,50.0f);
#else
	Matrix=ProjectionMatrix(pi/2.0f,320.0f/280.0f,0.999f,50.0f);
#endif // UNDER_XBOX
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

	Matrix=IdentityMatrix();
	if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

	Matrix=IdentityMatrix();
	if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
	{
		WriteToLog("SetTransform(ViewMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    if (bTexGen)
    {
	    Matrix=IdentityMatrix();
	    if (!SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&Matrix))
	    {
		    WriteToLog("SetTransform(Texture0) failed with HResult = %X.\n",GetLastError());
            return false;
	    }
    }

	return true;
}

UINT CCubemap::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Make sure we are on at lease DX7
	if (m_dwVersion <= 0x0600)
	{
		WriteToLog("This test requires at least Dx7.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the caps we are interested in
	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;
	DWORD dwMaxTextureWidth  = m_d3dcaps.MaxTextureWidth;
	DWORD dwMaxTextureHeight = m_d3dcaps.MaxTextureHeight;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		WriteToLog("Device capability not supported: Texture Cubemap.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check max texture width & height caps
	if (dwMaxTextureWidth < (UINT)m_dTextureWidthMax)
	{
		WriteToLog("Texture width (%d) > MaxTextureWidth (%d)\n",m_dTextureWidthMax, dwMaxTextureWidth);
		Result = D3DTESTINIT_SKIPALL;
	}

	if (dwMaxTextureHeight < (UINT)m_dTextureHeightMax) 
	{
		WriteToLog("Texture height (%d) > MaxTextureHeight (%d)\n",m_dTextureHeightMax, dwMaxTextureHeight);
		Result = D3DTESTINIT_SKIPALL;
	}

    // Load sphere vertices
	if (!LoadSphere()) {
        return D3DTESTINIT_SKIPALL;
    }

    // Setup the "special" Pool variables
    if (KeySet("Manage"))
    {
        bD3DManage = false;
        bVideo = false;
        bManage = true;
    }

    if (KeySet("Video"))
    {
        bD3DManage = false;
        bVideo = true;
        bManage = false;
    }

    if (KeySet("D3DManage"))
    {
        bD3DManage = true;
        bManage = false;
        bVideo = false;
    }

    // Tell the log what we are going to do
#ifndef UNDER_XBOX
    if (bVideo)
        WriteToLog("Creation: Video\n");

    if (bManage)
        WriteToLog("Creation: Manage\n");

    if (bD3DManage)
        WriteToLog("Creation: D3DManage\n");
#else
        WriteToLog("Creation: UMA\n");
#endif // UNDER_XBOX

    // Setup the "special" loading variables
    if (m_dwVersion <= 0x0700)
    {
/*
        if (KeySet("Blt") || (m_pMode->dwTexLoad == TEXLOAD_COPY_RECTS))
        {
            bBlt = true;
            bLock = false;
        }

        if (KeySet("Lock") || (m_pMode->dwTexLoad == TEXLOAD_UPDATE_TEXTURE))
        {
            bBlt = false;
            bLock = true;
        }

        // Tell the log what we are going to use
        if (bBlt)
            WriteToLog("Loading: Blt\n");

        if (bLock)
            WriteToLog("Loading: Lock & Copy\n");
*/
    }
    else // m_dwVersion >= 0x0800
    {
/*
        if (KeySet("Copy") || (m_pMode->dwTexLoad == TEXLOAD_COPY_RECTS))
        {
            bUpdate = false;
            bCopy = true;
        }

        if (KeySet("Update") || (m_pMode->dwTexLoad == TEXLOAD_UPDATE_TEXTURE))
        {
            bUpdate = true;
            bCopy = false;
        }
*/
        if (rand() % 2) {
            bUpdate = false;
            bCopy = true;
        }
        else {
            bUpdate = true;
            bCopy = false;
        }

        // Tell the log what we are going to use
        if (bCopy)
            WriteToLog("Loading: CopyRects\n");

        if (bUpdate)
            WriteToLog("Loading: UpdateTexure\n");
    }

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return Result;
}

void CCubemap::SceneRefresh()
{
	if (BeginScene()) 
    {
        if (bTexGen)
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL,m_paTGVertex,m_cVertexMax,NULL,0,NULL);
        else     
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0),m_paVertex,m_cVertexMax,NULL,0,NULL);

		EndScene();
	}
}

bool CCubemap::ProcessFrame(void)
{
	char	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;
    bool	bResult;

     // Use the standard 15%
    bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how the compare went
    if (bResult)
    {
		(m_fIncrementForward) ? nPass++ : nPass--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
	else
	{
		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
	}

	WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}

bool CCubemap::TestTerminate()
{
    // Cleanup vertex data
	if (m_paVertex!=NULL) 
		delete []m_paVertex;

    if (m_paTGVertex!=NULL)
		delete []m_paTGVertex;

    // Cleanup material
//	RELEASE(m_paMaterial);

	// Cleanup textures
	SetTexture(0, NULL);
    ReleaseTexture(m_paTexture);
	ReleaseTexture(m_paSysTexture);

    return true;
}

bool CCubemap::LoadSphere()
{
    int iSph = 0;
    FLOAT fDAngY = ((180.0F/(FLOAT)SPH_NUM)*D2R);
    FLOAT fDAngX = ((360.0F/(FLOAT)(SPH_NUM*2))*D2R);
    FLOAT fDAngY0 = fDAngY;
    FLOAT fDAngY1 = fDAngY0 + fDAngY;

	m_cVertexMax=SPH_VTXS;
    m_paVertex=new(TVertex[m_cVertexMax]);
    if (!m_paVertex) {
        return false;
    }

    if (bTexGen) {
	    m_paTGVertex=new(TGVertex[m_cVertexMax]);
        if (!m_paTGVertex) {
            delete [] m_paVertex;
            m_paVertex = NULL;
            return false;
        }
    }

    // make middle
    int x, y;
    for(y = 0; y < (SPH_NUM-2); y++) {
        FLOAT y0 = (FLOAT)cos(fDAngY0);
        FLOAT y1 = (FLOAT)cos(fDAngY1);
        FLOAT Rad0 = (FLOAT)sin(fDAngY0);
        FLOAT Rad1 = (FLOAT)sin(fDAngY1);

        for(x = 0; x < (SPH_NUM*2); x++) {
            FLOAT fDAngX0 = (FLOAT)x*fDAngX;
            FLOAT fDAngX1;
            if (x == (SPH_NUM*2-1))
                fDAngX1 = 0.0;
            else
                fDAngX1 = (FLOAT)(x+1)*fDAngX;

            FLOAT x00 = Rad0*(FLOAT)sin(fDAngX0);
            FLOAT x01 = Rad0*(FLOAT)sin(fDAngX1);
            FLOAT x10 = Rad1*(FLOAT)sin(fDAngX0);
            FLOAT x11 = Rad1*(FLOAT)sin(fDAngX1);

            FLOAT z00 = Rad0*(FLOAT)cos(fDAngX0);
            FLOAT z01 = Rad0*(FLOAT)cos(fDAngX1);
            FLOAT z10 = Rad1*(FLOAT)cos(fDAngX0);
            FLOAT z11 = Rad1*(FLOAT)cos(fDAngX1);

            m_paVertex[iSph].v.x = SPH_RADIUS*x00;  m_paVertex[iSph].v.y = SPH_RADIUS*y0;  m_paVertex[iSph].v.z = SPH_RADIUS*z00;
            m_paVertex[iSph].n.x = x00; m_paVertex[iSph].n.y = y0; m_paVertex[iSph].n.z = z00;
            iSph++;

            m_paVertex[iSph].v.x = SPH_RADIUS*x10;  m_paVertex[iSph].v.y = SPH_RADIUS*y1;  m_paVertex[iSph].v.z = SPH_RADIUS*z10;
            m_paVertex[iSph].n.x = x10; m_paVertex[iSph].n.y = y1; m_paVertex[iSph].n.z = z10;
            iSph++;

            m_paVertex[iSph].v.x = SPH_RADIUS*x11;  m_paVertex[iSph].v.y = SPH_RADIUS*y1;  m_paVertex[iSph].v.z = SPH_RADIUS*z11;
            m_paVertex[iSph].n.x = x11; m_paVertex[iSph].n.y = y1; m_paVertex[iSph].n.z = z11;
            iSph++;


            m_paVertex[iSph].v.x = SPH_RADIUS*x00;  m_paVertex[iSph].v.y = SPH_RADIUS*y0;  m_paVertex[iSph].v.z = SPH_RADIUS*z00;
            m_paVertex[iSph].n.x = x00; m_paVertex[iSph].n.y = y0; m_paVertex[iSph].n.z = z00;
            iSph++;

            m_paVertex[iSph].v.x = SPH_RADIUS*x11;  m_paVertex[iSph].v.y = SPH_RADIUS*y1;  m_paVertex[iSph].v.z = SPH_RADIUS*z11;
            m_paVertex[iSph].n.x = x11; m_paVertex[iSph].n.y = y1; m_paVertex[iSph].n.z = z11;
            iSph++;

            m_paVertex[iSph].v.x = SPH_RADIUS*x01;  m_paVertex[iSph].v.y = SPH_RADIUS*y0;  m_paVertex[iSph].v.z = SPH_RADIUS*z01;
            m_paVertex[iSph].n.x = x01; m_paVertex[iSph].n.y = y0; m_paVertex[iSph].n.z = z01;
            iSph++;

        }
        fDAngY0 = fDAngY1;
        fDAngY1 += fDAngY;
    }

    // make top
    fDAngY1 = fDAngY;
    FLOAT y1 = (FLOAT)cos(fDAngY1);
    FLOAT Rad1 = (FLOAT)sin(fDAngY1);
    for(x = 0; x < (SPH_NUM*2); x++) {
        FLOAT fDAngX0 = (FLOAT)x*fDAngX;
        FLOAT fDAngX1;
        if (x == (SPH_NUM*2-1))
            fDAngX1 = 0.0F;
        else
            fDAngX1 = (FLOAT)(x+1)*fDAngX;

        FLOAT x10 = Rad1*(FLOAT)sin(fDAngX0);
        FLOAT x11 = Rad1*(FLOAT)sin(fDAngX1);

        FLOAT z10 = Rad1*(FLOAT)cos(fDAngX0);
        FLOAT z11 = Rad1*(FLOAT)cos(fDAngX1);

        m_paVertex[iSph].v.x = 0.0;  m_paVertex[iSph].v.y = SPH_RADIUS;  m_paVertex[iSph].v.z = 0.0;
        m_paVertex[iSph].n.x = 0.0; m_paVertex[iSph].n.y = 1.0; m_paVertex[iSph].n.z = 0.0;
        iSph++;

        m_paVertex[iSph].v.x = SPH_RADIUS*x10;  m_paVertex[iSph].v.y = SPH_RADIUS*y1;  m_paVertex[iSph].v.z = SPH_RADIUS*z10;
        m_paVertex[iSph].n.x = x10; m_paVertex[iSph].n.y = y1; m_paVertex[iSph].n.z = z10;
        iSph++;

        m_paVertex[iSph].v.x = SPH_RADIUS*x11;  m_paVertex[iSph].v.y = SPH_RADIUS*y1;  m_paVertex[iSph].v.z = SPH_RADIUS*z11;
        m_paVertex[iSph].n.x = x11; m_paVertex[iSph].n.y = y1; m_paVertex[iSph].n.z = z11;
        iSph++;
    }

    // make bottom
    fDAngY1 = fDAngY0;          // remember last value used, so there are no cracks
    y1 = (FLOAT)cos(fDAngY1);
    Rad1 = (FLOAT)sin(fDAngY1);
    for(x = 0; x < (SPH_NUM*2); x++) {
        FLOAT fDAngX0 = (FLOAT)x*fDAngX;
        FLOAT fDAngX1;
        if (x == (SPH_NUM*2-1))
            fDAngX1 = 0.0F;
        else
            fDAngX1 = (FLOAT)(x+1)*fDAngX;

        // to keep the same orientation
        FLOAT x11 = Rad1*(FLOAT)sin(fDAngX0);
        FLOAT x10 = Rad1*(FLOAT)sin(fDAngX1);

        FLOAT z11 = Rad1*(FLOAT)cos(fDAngX0);
        FLOAT z10 = Rad1*(FLOAT)cos(fDAngX1);

        m_paVertex[iSph].v.x = 0.0;  m_paVertex[iSph].v.y = -SPH_RADIUS;  m_paVertex[iSph].v.z = 0.0;
        m_paVertex[iSph].n.x = 0.0; m_paVertex[iSph].n.y = -1.0; m_paVertex[iSph].n.z = 0.0;
        iSph++;

        m_paVertex[iSph].v.x = SPH_RADIUS*x10;  m_paVertex[iSph].v.y = SPH_RADIUS*y1;  m_paVertex[iSph].v.z = SPH_RADIUS*z10;
        m_paVertex[iSph].n.x = x10; m_paVertex[iSph].n.y = y1; m_paVertex[iSph].n.z = z10;
        iSph++;

        m_paVertex[iSph].v.x = SPH_RADIUS*x11;  m_paVertex[iSph].v.y = SPH_RADIUS*y1;  m_paVertex[iSph].v.z = SPH_RADIUS*z11;
        m_paVertex[iSph].n.x = x11; m_paVertex[iSph].n.y = y1; m_paVertex[iSph].n.z = z11;
        iSph++;
    }

    if (bTexGen)
    {
    	for (int i=0;i<m_cVertexMax;i++)
        {
	    	m_paTGVertex[i].v = m_paVertex[i].v;
	    	m_paTGVertex[i].n = m_paVertex[i].n;
        }
    }
    else
    {
    	for (int i=0;i<m_cVertexMax;i++)
	    	m_paVertex[i].t = m_paVertex[i].n;
    }

    return true;
}

//******************************************************************************
BOOL CCubemap::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CCubemap::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CCubemap::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
