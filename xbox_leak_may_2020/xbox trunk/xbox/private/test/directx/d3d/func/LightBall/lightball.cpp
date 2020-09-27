#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "LightBall.h"

#define COLOR	255

TLVERTEX LightBall::m_pBackVertices[4] =
{
	TLVERTEX(D3DXVECTOR3(  0.f, 255.f, 0.999f), 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 0.f, 0.f),
	TLVERTEX(D3DXVECTOR3(  0.f,   0.f, 0.999f), 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 1.f, 0.f),
	TLVERTEX(D3DXVECTOR3(255.f,   0.f, 0.999f), 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 1.f, 1.f),
	TLVERTEX(D3DXVECTOR3(255.f, 255.f, 0.999f), 1.f, RGBA_MAKE(COLOR, COLOR, COLOR, 255), RGBA_MAKE(0,0,0,0), 0.f, 1.f),
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

    // Check vertex blending support
    return ((pd3dcaps->TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP) &&
            (pd3dcaps->TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE) &&
            (pd3dcaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED));
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

    LightBall*  pLightBall;
    BOOL        bRet;

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
    pLightBall = new LightBall();
    if (!pLightBall) {
        return FALSE;
    }

    // Initialize the scene
    if (!pLightBall->Create(pDisplay)) {
        pLightBall->Release();
        return FALSE;
    }

    bRet = pLightBall->Exhibit(pnExitCode);

    // Clean up the scene
    pLightBall->Release();

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(524288, 32768);
        pDisplay->CreateDevice();
    }
#endif

    return bRet;
}

LightBall::LightBall()
{
	m_pImageTexture = NULL;
	m_pBumpTexture = NULL;
	m_pLuminanceTexture = NULL;
    m_pSourceVertexBuffer = NULL;
    m_pReferenceVertexBuffer = NULL;
    m_pSrcDestVertexBuffer = NULL;
    m_pRefDestVertexBuffer = NULL;

	m_pSphereVertices = NULL;
	m_pSphereIndices = NULL;
	m_nSphereVertices = 0;
	m_nSphereIndices = 0;		


	m_dwCurrentTestNumber = 0;
	m_dwCurrentTransform = 0;
	m_dwCurrentScenario = 0;

	m_dwSameScenarioFrames = 100;
	m_bProjectedTexture = false;
	m_bBumpMapping = false;
	m_bLuminanceTexture = false;
    m_bUseVertexBuffer = false;

	m_dwNumFailed = 0;
	m_dwDriverStyle = 0;

	m_szTestName = TEXT("LightBall");
	m_szCommandKey = TEXT("lightball");

}

LightBall::~LightBall()
{
	if (m_pSphereVertices)
		delete [] m_pSphereVertices;
	if (m_pSphereIndices)
        delete [] m_pSphereIndices;

	ReleaseTexture(m_pImageTexture);
	ReleaseTexture(m_pBumpTexture);
	ReleaseTexture(m_pLuminanceTexture);
    ReleaseVertexBuffer(m_pSourceVertexBuffer);
    ReleaseVertexBuffer(m_pSrcDestVertexBuffer);
    ReleaseVertexBuffer(m_pRefDestVertexBuffer);
}


UINT LightBall::TestInitialize(void)
{
//	CImageLoader    Loader;
//	CImageData*		pImageData;
	CShapes*		pSphere = NULL;
//	IMAGEDATA*		pImage = new IMAGEDATA [LIGHTBALL_TEXTURESIZE*LIGHTBALL_TEXTURESIZE];
    SPHEREVERTEX*   pVBVertices;
	int				i, j, k;
    D3DLOCKED_RECT  d3dlr;
    LPWORD          pwPixel;
    LPBYTE          pPixel;

#ifdef UNDER_XBOX
    if (GetStartupContext() & TSTART_STRESS) {
        SetAPI(RP_DIPVB);
    }
#endif

	SetTestRange(1, 16*m_dwSameScenarioFrames);

    // Check for D3D version
    if (m_dwVersion < 0x0700)
    {   
        WriteToLog("LightBall: Test requires DX7 or later.\n");
        return D3DTESTINIT_SKIPALL;
    }

    // Check for pImage memory
//    if (NULL == pImage)
//    {
//		WriteToLog("LightBall: Not enough memory to create image buffer\n");
//        return D3DTESTINIT_ABORT;
//    }
	// Background vertices
	m_pBackVertices[2].vPosition.x = m_pBackVertices[3].vPosition.x = (float)m_pDisplay->GetWidth();
	m_pBackVertices[0].vPosition.y = m_pBackVertices[3].vPosition.y = (float)m_pDisplay->GetHeight();

	// Sphere vertices
	pSphere = new CShapes;
	if (NULL == pSphere)
    {
		WriteToLog("LightBall: Not enough memory to create primitive\n");
        return D3DTESTINIT_ABORT;
    }
	if (!pSphere->NewShape(CS_SPHERE)) {
        return D3DTESTINIT_ABORT;
    }

	pSphere->Scale(20.f);

	m_nSphereVertices = pSphere->m_nVertices;
	m_pSphereVertices = new SPHEREVERTEX [m_nSphereVertices];
	if (NULL == m_pSphereVertices)
    {
		WriteToLog("LightBall: Not enough memory to create vertex array\n");
        return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_nSphereVertices; i++)
	{
		m_pSphereVertices[i].x  = pSphere->m_pVertices[i].x;
		m_pSphereVertices[i].y  = pSphere->m_pVertices[i].y;
		m_pSphereVertices[i].z  = pSphere->m_pVertices[i].z;
		m_pSphereVertices[i].color = RGBA_MAKE(255, 255, 255, 255);
		m_pSphereVertices[i].tx = pSphere->m_pVertices[i].x;
		m_pSphereVertices[i].ty = pSphere->m_pVertices[i].y;
		m_pSphereVertices[i].tz = pSphere->m_pVertices[i].z;
	}

	m_nSphereIndices = pSphere->m_nIndices;
	m_pSphereIndices = new WORD [m_nSphereIndices];
	if (NULL == m_pSphereIndices)
    {
		WriteToLog("LightBall: Not enough memory to create vertex array\n");
		return D3DTESTINIT_ABORT;
    }
	for (i = 0; i < m_nSphereIndices; i++)
		m_pSphereIndices[i] = pSphere->m_pIndices[i];
    delete pSphere;

    // Source vertex buffer
    m_pSourceVertexBuffer = CreateVertexBuffer(m_pDevice, m_pSphereVertices, m_nSphereVertices * sizeof(SPHEREVERTEX),
        0, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0));
/*
    m_pSourceVertexBuffer = CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0),
        m_nSphereVertices, D3DVBCAPS_SYSTEMMEMORY, false);
    if (m_pSourceVertexBuffer && m_pSourceVertexBuffer->Lock((void**)&pVBVertices))
    {
        memcpy(pVBVertices, m_pSphereVertices, m_nSphereVertices*sizeof(SPHEREVERTEX));
        m_pSourceVertexBuffer->Unlock();
    }
    else
    {
        WriteToLog("LightBall: Can't lock source vertex buffer\n");
		return D3DTESTINIT_ABORT;
    }
    
    // Reference vertex buffer
    m_pReferenceVertexBuffer = CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0),
        m_nSphereVertices, D3DVBCAPS_SYSTEMMEMORY, true);
    if (m_pReferenceVertexBuffer && m_pReferenceVertexBuffer->Lock((void**)&pVBVertices))
    {
        memcpy(pVBVertices, m_pSphereVertices, m_nSphereVertices*sizeof(SPHEREVERTEX));
        m_pReferenceVertexBuffer->Unlock();
    }
    else
    {
        WriteToLog("LightBall: Can't lock reference vertex buffer\n");
		return D3DTESTINIT_ABORT;
    }
*/

	// Check for projected texture support
	if (!CheckProjectedTextures())
	{
		WriteToLog("LightBall: Device does NOT support projected textures.\n");
		m_bProjectedTexture = false;
	}

	// BumpMap texture
    m_pBumpTexture = (CTexture8*)CreateTexture(m_pDevice, LIGHTBALL_TEXTURESIZE, LIGHTBALL_TEXTURESIZE, D3DFMT_V8U8);
    if (!m_pBumpTexture) {
        m_bBumpMapping = false;
			WriteToLog("LightBall: Unable to create bumpmap texture\n");
			return D3DTESTINIT_ABORT;
    }

    m_pBumpTexture->LockRect(0, &d3dlr, NULL, 0);
    pwPixel = (LPWORD)d3dlr.pBits;

	srand(777);
	for (i = 0; i < LIGHTBALL_TEXTURESIZE; i++)
	{
		for (j = 0; j < LIGHTBALL_TEXTURESIZE; j++)
		{
            pwPixel[j] = ((BYTE)(float(rand()) * 30.f / RAND_MAX) << 8) |
                         ((BYTE)(float(rand()) * 30.f / RAND_MAX));
        }

        pwPixel += d3dlr.Pitch / 2;
    }

    m_pBumpTexture->UnlockRect(0);
/*
	if (FindBumpmapTextureFormat())
	{
		pImageData = new CImageData;
        if (NULL == pImageData)
        {
            WriteToLog("LightBall: Not enough memory to create bumpmap image\n");
            return D3DTESTINIT_ABORT;
        }
		ZeroMemory(pImage, sizeof(IMAGEDATA)*LIGHTBALL_TEXTURESIZE*LIGHTBALL_TEXTURESIZE);
		srand(777);
		for (i = 0; i < LIGHTBALL_TEXTURESIZE; i++)
		{
			for (j = 0; j < LIGHTBALL_TEXTURESIZE; j++)
			{
				k = i*LIGHTBALL_TEXTURESIZE + j;
				pImage[k].bBumpDu = (BYTE)(float(rand()) * 30.f / RAND_MAX);
				pImage[k].bBumpDv = (BYTE)(float(rand()) * 30.f / RAND_MAX);
				pImage[k].bBumpLuminance = 255;
			}
		}
		pImageData->SetData(LIGHTBALL_TEXTURESIZE, LIGHTBALL_TEXTURESIZE, pImage);
		if (!(m_pBumpTexture = CreateTexture(LIGHTBALL_TEXTURESIZE, LIGHTBALL_TEXTURESIZE, NULL, pImageData)))
		{
			WriteToLog("LightBall: Unable to create bumpmap texture.\n");
			return D3DTESTINIT_ABORT;
		}
		RELEASE(pImageData);
	}
	else
	{
        WriteToLog("LightBall: Device does NOT support bumpmapping textures.\n");
		m_bBumpMapping = false;
	}
*/

	// Luminance texture
    m_pLuminanceTexture = (CTexture8*)CreateTexture(m_pDevice, LIGHTBALL_TEXTURESIZE, LIGHTBALL_TEXTURESIZE, D3DFMT_L8);
    if (!m_pLuminanceTexture) {
			WriteToLog("LightBall: Unable to create luminance texture\n");
			return D3DTESTINIT_ABORT;
    }

    m_pLuminanceTexture->LockRect(0, &d3dlr, NULL, 0);
    pPixel = (LPBYTE)d3dlr.pBits;

	for (i = 0; i < LIGHTBALL_TEXTURESIZE; i++)
	{
		for (j = 0; j < LIGHTBALL_TEXTURESIZE; j++)
		{
			float center = LIGHTBALL_TEXTURESIZE >> 1;
			float dx = float(abs(i - (int)center)) / (float)LIGHTBALL_TEXTURESIZE * 2.0f;
			float dy = float(abs(j - (int)center)) / (float)LIGHTBALL_TEXTURESIZE * 2.0f;
			float dist = (float)sqrt(dx*dx + dy*dy);
			if (dist < 1.f)
				pPixel[j] = (BYTE)(255.f - 255.f*dist);
			else
				pPixel[j] = 0;
        }

        pPixel += d3dlr.Pitch;
    }

    m_pLuminanceTexture->UnlockRect(0);
/*
	if (FindLuminanceTextureFormat())
	{
		pImageData = new CImageData;
        if (NULL == pImageData)
        {
            WriteToLog("LightBall: Not enough memory to create luminance image\n");
            return D3DTESTINIT_ABORT;
        }
		ZeroMemory(pImage, sizeof(IMAGEDATA)*LIGHTBALL_TEXTURESIZE*LIGHTBALL_TEXTURESIZE);
		for (i = 0; i < LIGHTBALL_TEXTURESIZE; i++)
		{
			for (j = 0; j < LIGHTBALL_TEXTURESIZE; j++)
			{
				k = i*LIGHTBALL_TEXTURESIZE + j;
				float center = LIGHTBALL_TEXTURESIZE >> 1;
				float dx = float(abs(i - center)) / LIGHTBALL_TEXTURESIZE * 2;
				float dy = float(abs(j - center)) / LIGHTBALL_TEXTURESIZE * 2;
				float dist = sqrt(dx*dx + dy*dy);
				if (dist < 1.f)
					pImage[k].bLuminance = (BYTE)(255.f - 255.f*dist);
				else
					pImage[k].bLuminance = 0;
			}
		}
		pImageData->SetData(LIGHTBALL_TEXTURESIZE, LIGHTBALL_TEXTURESIZE, pImage);
		if (!(m_pLuminanceTexture = CreateTexture(LIGHTBALL_TEXTURESIZE, LIGHTBALL_TEXTURESIZE, NULL, pImageData)))
		{
			WriteToLog("LightBall: Unable to create luminance texture\n");
			return D3DTESTINIT_ABORT;
		}
		RELEASE(pImageData);
	}
	else
	{
		WriteToLog("LightBall: Device does NOT support luminance textures.\n");
		m_bLuminanceTexture = false;
	}

    // Do not need memory at pImage any more
    delete [] pImage;
*/	
	// Image texture
    m_pImageTexture = (CTexture8*)CreateTexture(m_pDevice, TEXT("win95.bmp"), D3DFMT_R5G6B5);
    if (!m_pImageTexture) {
		WriteToLog("LightBall: Error creating RGB texture\n");
        return D3DTESTINIT_ABORT;
    }

/*
	if (!FindRGBTextureFormat())
    {
		WriteToLog("LightBall: Device does not support 16 bit RGB textures\n");
        return D3DTESTINIT_SKIPALL;
    }
	pImageData = Loader.Load(CIL_BMP,"WIN95.BMP");
	m_pImageTexture = CreateTexture(LIGHTBALL_TEXTURESIZE,LIGHTBALL_TEXTURESIZE,NULL,pImageData);
	RELEASE(pImageData);
	if (m_pImageTexture == NULL)
    {
		WriteToLog("LightBall: Error creating RGB texture\n");
        return D3DTESTINIT_ABORT;
    }
*/
	return D3DTESTINIT_RUN;
}


bool LightBall::ExecuteTest(UINT iTestNumber)
{
	HRESULT		hr;

	hr = GetLastError();
	if (FAILED(hr))
		WriteToLog("LightBall: Error before ExecuteTest()\n");
	
	m_dwCurrentTestNumber = (DWORD)iTestNumber;
	
	if (ResolveTestNumber())
	{
		BeginTestCase("RGB texture");
		SetCurrentTransform();
		SetTextureTransform();	

		hr = GetLastError();
		if (FAILED(hr))
			WriteToLog("LightBall: Error in ExecuteTest()\n");
	}
	else
	{
		SkipTests(m_dwSameScenarioFrames - m_dwCurrentTransform);
		return false;
	}

	return true;
}

void LightBall::SceneRefresh(void)
{
	HRESULT		hr;
	int			iStage = 0;
	
	hr = GetLastError();
	if (FAILED(hr))
		WriteToLog("LightBall: Error before ScreenRefresh()\n");

	if (BeginScene())
	{
		// Background
		SetTexture(0, m_pImageTexture);
		SetTextureStageState(0, D3DTSS_COLOROP,		(DWORD)D3DTOP_SELECTARG1);
		SetTextureStageState(0, D3DTSS_COLORARG1,	(DWORD)D3DTA_TEXTURE);
		SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_DISABLE) );
		SetTextureStageState(1, D3DTSS_COLOROP,		(DWORD)D3DTOP_DISABLE);
		SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_DISABLE) );
		RenderPrimitive(D3DPT_TRIANGLEFAN, FVF_TLVERTEX, m_pBackVertices, 4, 
            NULL, 0, D3DDP_DONOTUPDATEEXTENTS);
		
		// LightBall
		if (m_bLuminanceTexture && m_bBumpMapping)
		{
			SetTexture(0, m_pBumpTexture);
		    SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	(DWORD)0);
		    SetTextureStageState(0, D3DTSS_COLOROP,			(DWORD)D3DTOP_BUMPENVMAP);
			SetTextureStageState(0, D3DTSS_COLORARG1,		(DWORD)D3DTA_TEXTURE);
			SetTextureStageState(0, D3DTSS_COLORARG2,		(DWORD)D3DTA_DIFFUSE);
		    SetTextureStageState(0, D3DTSS_BUMPENVMAT00, 	F2DW(0.1f));
			SetTextureStageState(0, D3DTSS_BUMPENVMAT01, 	F2DW(0.0f));
			SetTextureStageState(0, D3DTSS_BUMPENVMAT10, 	F2DW(0.0f));
			SetTextureStageState(0, D3DTSS_BUMPENVMAT11, 	F2DW(0.1f));
			SetTextureStageState(0, D3DTSS_BUMPENVLSCALE,	F2DW(1.f));
			SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET,	F2DW(1.f));
			SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &m_mProjectionTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );

			SetTexture(1, m_pImageTexture);
		    SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	(DWORD)0);
			SetTextureStageState(1, D3DTSS_COLOROP,			(DWORD)D3DTOP_SELECTARG1);
			SetTextureStageState(1, D3DTSS_COLORARG1,		(DWORD)D3DTA_TEXTURE);
		    SetTextureStageState(1, D3DTSS_COLORARG2,		(DWORD)D3DTA_CURRENT);
			SetTransform(D3DTRANSFORMSTATE_TEXTURE1, &m_mProjectionTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );

			SetTexture(2, m_pLuminanceTexture);
		    SetTextureStageState(2, D3DTSS_TEXCOORDINDEX,	(DWORD)0);
			SetTextureStageState(2, D3DTSS_COLOROP,			(DWORD)D3DTOP_MODULATE);
			SetTextureStageState(2, D3DTSS_COLORARG1,		(DWORD)D3DTA_TEXTURE);
			SetTextureStageState(2, D3DTSS_COLORARG2,		(DWORD)D3DTA_CURRENT);
			SetTransform(D3DTRANSFORMSTATE_TEXTURE2, &m_mSphereTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );
		}
		else if (m_bLuminanceTexture)
		{
			SetTexture(0, m_pImageTexture);
		    SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	(DWORD)0);
			SetTextureStageState(0, D3DTSS_COLOROP,			(DWORD)D3DTOP_SELECTARG1);
			SetTextureStageState(0, D3DTSS_COLORARG1,		(DWORD)D3DTA_TEXTURE);
			SetTextureStageState(0, D3DTSS_COLORARG2,		(DWORD)D3DTA_DIFFUSE);
			SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &m_mProjectionTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );

			SetTexture(1, m_pLuminanceTexture);
		    SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	(DWORD)0);
			SetTextureStageState(1, D3DTSS_COLOROP,			(DWORD)D3DTOP_MODULATE);
			SetTextureStageState(1, D3DTSS_COLORARG1,		(DWORD)D3DTA_TEXTURE);
			SetTextureStageState(1, D3DTSS_COLORARG2,		(DWORD)D3DTA_CURRENT);
			SetTransform(D3DTRANSFORMSTATE_TEXTURE1, &m_mSphereTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );

            SetTextureStageState(2, D3DTSS_COLOROP,		(DWORD)D3DTOP_DISABLE);
		}
		else if (m_bBumpMapping)
		{
			SetTexture(0, m_pBumpTexture);
		    SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,	(DWORD)0);
		    SetTextureStageState(0, D3DTSS_COLOROP,			(DWORD)D3DTOP_BUMPENVMAP);
			SetTextureStageState(0, D3DTSS_COLORARG1,		(DWORD)D3DTA_TEXTURE);
			SetTextureStageState(0, D3DTSS_COLORARG2,		(DWORD)D3DTA_DIFFUSE);
		    SetTextureStageState(0, D3DTSS_BUMPENVMAT00, 	F2DW(0.1f));
			SetTextureStageState(0, D3DTSS_BUMPENVMAT01, 	F2DW(0.0f));
			SetTextureStageState(0, D3DTSS_BUMPENVMAT10, 	F2DW(0.0f));
			SetTextureStageState(0, D3DTSS_BUMPENVMAT11, 	F2DW(0.1f));
			SetTextureStageState(0, D3DTSS_BUMPENVLSCALE,	F2DW(1.f));
			SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET,	F2DW(1.f));
			SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &m_mProjectionTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );

			SetTexture(1, m_pImageTexture);
		    SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,	(DWORD)0);
			SetTextureStageState(1, D3DTSS_COLOROP,			(DWORD)D3DTOP_SELECTARG1);
			SetTextureStageState(1, D3DTSS_COLORARG1,		(DWORD)D3DTA_TEXTURE);
		    SetTextureStageState(1, D3DTSS_COLORARG2,		(DWORD)D3DTA_CURRENT);
			SetTransform(D3DTRANSFORMSTATE_TEXTURE1, &m_mProjectionTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );

            SetTextureStageState(2, D3DTSS_COLOROP,		(DWORD)D3DTOP_DISABLE);
        }
		else
		{
			SetTexture(0, m_pImageTexture);
			SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &m_mProjectionTransform);
			if (m_bProjectedTexture)
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT3|D3DTTFF_PROJECTED) );
			else
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2) );
		}
		
//        if (m_bUseVertexBuffer)
        if (FALSE)
        {
            ProcessVertices();
		    
            // Change texture stages to after-transformation status
            if (m_bLuminanceTexture || m_bBumpMapping)
                SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, (DWORD)1, CD3D_SRC);
		    if (m_bLuminanceTexture && m_bBumpMapping)
    		    SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, (DWORD)2, CD3D_SRC);

//            DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pSrcDestVertexBuffer, m_pReferenceVertexBuffer,
//                0, m_nSphereVertices, m_pSphereIndices, m_nSphereIndices, D3DDP_DONOTUPDATEEXTENTS);
            ClearVertexBuffers();
        }
        else
        {
            RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0), 
                m_pSphereVertices, m_nSphereVertices, 
                m_pSphereIndices, m_nSphereIndices, D3DDP_DONOTUPDATEEXTENTS);
        }
		
		SetTexture(0, 0);
		SetTexture(1, 0);
		SetTexture(2, 0);
		EndScene();
	}

	hr = GetLastError();
	if (FAILED(hr))
		WriteToLog("LightBall: Error in ScreenRefresh()\n");
}

bool LightBall::ProcessVertices(void)
{
    DWORD   dwSrcFlags = 0;
//  DWORD   dwRefFlags = 0;
    DWORD   dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
    DWORD   dwTexturesNumber = 1;

/*
    // Calculate FVF of output vertices
    if (m_bLuminanceTexture && m_bBumpMapping)
    {
        dwFVF |= D3DFVF_TEX3;
        if (m_bProjectedTexture)
            dwFVF |= D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2);
    }
    else if (m_bLuminanceTexture || m_bBumpMapping)
    {
        dwFVF |= D3DFVF_TEX2;
        if (m_bProjectedTexture)
            dwFVF |= D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1);
    }
    else
    {
        dwFVF |= D3DFVF_TEX1;
        if (m_bProjectedTexture)
            dwFVF |= D3DFVF_TEXCOORDSIZE3(0);
    }

    // See if D3DVBCAPS_SYSTEMMEMORY is required
    if (!m_pAdapter->Devices[m_pMode->nSrcDevice].fHardware)
        dwSrcFlags |= D3DVBCAPS_SYSTEMMEMORY;
    
    // Create destination vertex buffers
    m_pSrcDestVertexBuffer = CreateVertexBuffer(dwFVF, m_nSphereVertices, dwSrcFlags, false);
    if (NULL == m_pSrcDestVertexBuffer)
    {
        WriteToLog("LightBall: Can't create destination vertex buffer for source device\n");
        return false;
    }

    if (m_dwVersion >= 0x0800)
    {
        DWORD dwStride = GetVertexSize(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0));
        SetVertexShader((D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0)), 0);
        SetStreamSource(0, m_pSourceVertexBuffer, NULL, dwStride, CD3D_SRC);
        CDirect3D::ProcessVertices(0, 0, m_nSphereVertices, m_pSrcDestVertexBuffer, 0);
    }
    else
    {
        // Process vertices from m_pSourceVertexBuffer to destinaton vertex buffers
        if (!m_pSrcDestVertexBuffer->ProcessVertices(D3DVOP_TRANSFORM | D3DVOP_CLIP, 0, m_nSphereVertices, m_pSourceVertexBuffer, 0, false, 0))
            return false;
    }
*/
    return true;
}

bool LightBall::ClearVertexBuffers(void)
{
    ReleaseVertexBuffer(m_pSrcDestVertexBuffer);
    ReleaseVertexBuffer(m_pRefDestVertexBuffer);
    return true;
}


bool LightBall::ProcessFrame(void)
{
	bool    bResult;

    // This range (0.00002) basically allows 1 pixel to be different
    bResult = GetCompareResult(0.015f, 0.78f, 0);

    if (!bResult) {
		WriteToLog("$rLightBall: Test fails on frame %d\n", m_dwCurrentTestNumber);
		Fail();
		m_dwNumFailed++;
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

bool LightBall::TestTerminate(void)
{
	if (m_pSphereVertices)
	{
		delete [] m_pSphereVertices;
		m_pSphereVertices = NULL;
		m_nSphereVertices = 0;
	}
	if (m_pSphereIndices)
	{
		delete [] m_pSphereIndices;
		m_pSphereIndices = NULL;
		m_nSphereIndices = 0;
	}

	ReleaseTexture(m_pImageTexture);
    m_pImageTexture = NULL;
	ReleaseTexture(m_pBumpTexture);
    m_pBumpTexture = NULL;
	ReleaseTexture(m_pLuminanceTexture);
    m_pLuminanceTexture = NULL;
    ReleaseVertexBuffer(m_pSourceVertexBuffer);
    m_pSourceVertexBuffer = NULL;
    ReleaseVertexBuffer(m_pReferenceVertexBuffer);
    m_pReferenceVertexBuffer = NULL;
    ReleaseVertexBuffer(m_pSrcDestVertexBuffer);
    m_pSrcDestVertexBuffer = NULL;
    ReleaseVertexBuffer(m_pRefDestVertexBuffer);
    m_pRefDestVertexBuffer = NULL;
	return true;
}
