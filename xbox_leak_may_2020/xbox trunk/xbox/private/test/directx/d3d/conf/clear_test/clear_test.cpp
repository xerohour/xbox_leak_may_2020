//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// clear_test.cpp
//
// CClearTest class - Tests many possible IDirect3DDeviceX::Clear() parameter combinations
//
// History: 8/19/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////


#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "clear_test.h"

/*
// from d3d8types.h
typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN              =  0,

    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,

    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,

    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,

    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_W11V11U10            = 65,

    D3DFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D16                  = 80,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,


    D3DFMT_VERTEXDATA           =100,
    D3DFMT_INDEX16              =101,
    D3DFMT_INDEX32              =102,

    D3DFMT_FORCE_DWORD          =0x7fffffff
} D3DFORMAT;
*/




// Global Test and App instance
//CD3DWindowFramework	App;

const DWORD CClearTest::VertexFVF = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

const DWORD CClearTest::TexVertexFVF = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

const DWORD CClearTest::dwVertexCount = NUM_VERTICES;

const CClearTest::Vertex CClearTest::VertexList[]=
{
	{ 0.0f,  0.0f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ (float)IMGWIDTH,  (float)IMGHEIGHT, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ 0.0f,  (float)IMGHEIGHT, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },


	{ 0.0f,  0.0f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ (float)IMGWIDTH,  0.0f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ (float)IMGWIDTH,  (float)IMGHEIGHT, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
};

//This is the grid on which we apply the texture.
const CClearTest::TexVertex CClearTest::Grid[]=
{
	{ -0.5f,  -0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 0.0, 0.0
	 },
	{ (float)IMGWIDTH - 0.5f,  (float)IMGHEIGHT - 0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 1.0, 1.0
	 },
	{ -0.5f,  (float)IMGHEIGHT - 0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 0.0, 1.0
	 },


	{ -0.5f,  -0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 0.0, 0.0
	 },
	{ (float)IMGWIDTH - 0.5f,  -0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 1.0, 0.0
	 },
	{ (float)IMGWIDTH - 0.5f,  (float)IMGHEIGHT - 0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 1.0, 1.0
	 },
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

    CClearTest*     pClearTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 2 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pClearTest = new CTextureClearTest();
                break;
            case 1:
                pClearTest = new COffscreenClearTest();
                break;
        }

        if (!pClearTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pClearTest->Create(pDisplay)) {
            pClearTest->Release();
            return FALSE;
        }

        bRet = pClearTest->Exhibit(pnExitCode);

        bQuit = pClearTest->AbortedExit();

        // Clean up the scene
        pClearTest->Release();
    }

    return bRet;
}

CClearTest::CClearTest()
{
#ifndef UNDER_XBOX
    D3DFORMAT fmt[] = {
        D3DFMT_A8R8G8B8,
        D3DFMT_X8R8G8B8,
        D3DFMT_R5G6B5,
//        D3DFMT_A1R5G5B5,
        D3DFMT_X1R5G5B5,
    };
#else
    D3DFORMAT fmt[] = {
        D3DFMT_LIN_A8R8G8B8,
        D3DFMT_LIN_X8R8G8B8,
        D3DFMT_LIN_R5G6B5,
//        D3DFMT_LIN_A1R5G5B5,
        D3DFMT_LIN_X1R5G5B5,
    };
#endif // UNDER_XBOX

    m_bOffscreen = false;
    m_pOriginalSrcRT = NULL;
    m_pOriginalSrcZ = NULL;
    m_pSrcZ = NULL;
    m_pTextureRT = NULL;
    m_pOffSrcSurf = NULL;
    m_pOffRefSurf = NULL;
    m_bExit = FALSE;

    m_uCommonTextureFormats = 4;
    memcpy(m_fmtCommon, fmt, m_uCommonTextureFormats * sizeof(D3DFORMAT));
}

CClearTest::~CClearTest()
{
	RELEASE(m_pSrcZ);
	RELEASE(m_pOffSrcSurf);
    m_pDevice->SetTexture(0, NULL);
	ReleaseTexture(m_pTextureRT);
    m_pTextureRT = NULL;
    if (m_pOriginalSrcRT) {
        m_pOriginalSrcRT->Release();
        m_pOriginalSrcRT = NULL;
    }
    if (m_pOriginalSrcZ) {
        m_pOriginalSrcZ->Release();
        m_pOriginalSrcZ = NULL;
    }
}

/*
bool CClearTest::AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize)
{
    if (IsConformance())
    {
		pMode->D3DMode.dwWidth = IMGWIDTH;
		pMode->D3DMode.dwHeight = IMGHEIGHT;
	}

	CD3DTest::AddModeList(pMode,pData,dwDataSize);

	return true;
}
*/


UINT CClearTest::TestInitialize(void)
{
    UINT i;

	// Request only RGB texture formats
//	if (!RequestTextureFormats(PF_RGB))
//    {
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

    if (m_pDisplay->IsDepthBuffered())
        SetTestRange(1, m_uCommonTextureFormats * m_uCommonZBufferFormats * TEST_CASES);
    else
        SetTestRange(1, m_uCommonTextureFormats * TEST_CASES);


//    m_pOriginalSrcRT = m_pSrcTarget;
//    m_pOriginalRefRT = m_pRefTarget;

//    m_pOriginalSrcRT->AddRef();
//    m_pOriginalRefRT->AddRef();

    m_pDevice->GetRenderTarget(&m_pOriginalSrcRT);

//    m_pOriginalSrcZ = m_pSrcZBuffer;
//    m_pOriginalRefZ = m_pRefZBuffer;

//    if (m_ModeOptions.fZBuffer)
//    {
//        m_pOriginalSrcZ->AddRef();
//        m_pOriginalRefZ->AddRef();
//    }

    if (m_pDisplay->IsDepthBuffered()) {
        m_pDevice->GetDepthStencilSurface(&m_pOriginalSrcZ);
    }

//    m_nOriginalZFormat = m_pMode->nZBufferFormat;
    
    memcpy(VPGrid, Grid, NUM_VERTICES * sizeof(TexVertex));
    for (i = 0; i < NUM_VERTICES; i++) {
        VPGrid[i].x += m_vpTest.X;
        VPGrid[i].y += m_vpTest.Y;
    }

#ifdef UNDER_XBOX
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif

    return D3DTESTINIT_RUN;
}




bool CClearTest::ClearFrame(void)
{
    bool bResult;

//    m_pMode->nZBufferFormat = m_nOriginalZFormat;

	bResult =  Clear(RGB_MAKE(0,0,0));

//    if (m_ModeOptions.fZBuffer)
//        m_pMode->nZBufferFormat = m_nZFormat;

    return bResult;
}




bool CClearTest::ExecuteTest(UINT uiTest)
{
	D3DRECT rectList[2] = {{IMGWIDTH/4,IMGHEIGHT/4,IMGWIDTH/2,IMGHEIGHT/2},{IMGWIDTH/2,IMGHEIGHT/2,IMGWIDTH*3/4,IMGHEIGHT*3/4}};
    CSurface8 *pRendTgtSrc = NULL, *pRendTgtRef = NULL;

    if (m_pDisplay->IsDepthBuffered())
    {
        m_nTexFormat = (uiTest -1) / (m_uCommonZBufferFormats * TEST_CASES);
        m_nZFormat = ( (uiTest -1) % (m_uCommonZBufferFormats * TEST_CASES) ) / TEST_CASES;
		m_nTestCase = ( (uiTest -1) % (m_uCommonZBufferFormats * TEST_CASES) ) % TEST_CASES;
    }
    else
    {
        m_nTexFormat = (uiTest-1) / TEST_CASES;
        m_nZFormat = 0;
		m_nTestCase = (uiTest-1) % TEST_CASES;
    }


//	m_pMode->nTextureFormat = m_nTexFormat;
//	m_pMode->nZBufferFormat = m_nZFormat;

	// do we need to change render-target or ZStencil formats now?
	if (m_nTestCase == 0)
	{
		//must change ZStencil format now.

		// Cleanup previous ZStencil
		RELEASE(m_pSrcZ);
//		RELEASE(m_pRefZ);

		if (m_nZFormat == 0)
		{
			//must change render-target format now.

			// Cleanup previous render-target
			SetTexture(0, NULL);
			ReleaseTexture(m_pTextureRT);
            m_pTextureRT = NULL;
			RELEASE(m_pOffSrcSurf);
//			RELEASE(m_pOffRefSurf);

			//create render target with the new format.
			if (m_bOffscreen)
			{
				if (!CreateOffScrRendTrgts(m_nTexFormat))
				{
					WriteToLog("(INFO) CreateOffScrRendTrgts failed.\n");
					SkipTests(m_pDisplay->IsDepthBuffered() ? (m_uCommonZBufferFormats * TEST_CASES) : TEST_CASES);
					return false;
				}

//				m_pTextureRT = CreateTexture(IMGWIDTH, IMGHEIGHT, CDDS_TEXTURE_VIDEO, NULL);
                m_pTextureRT = (CTexture8*)CreateTexture(m_pDevice, IMGWIDTH, IMGHEIGHT, m_fmtCommon[m_nTexFormat]);

			}
			else
			{
//				m_pTextureRT = CreateTextureAsRenderTarget(IMGWIDTH, IMGHEIGHT, CDDS_TEXTURE_VIDEO, FALSE, nullpZStencil, nullpZStencil);
                m_pTextureRT = (CTexture8*)CreateTexture(m_pDevice, IMGWIDTH, IMGHEIGHT, m_fmtCommon[m_nTexFormat], TTYPE_TARGET);
			}
 
			if (NULL == m_pTextureRT)
			{
				WriteToLog("(INFO) CreateTexture failed. (%X)\n", GetLastError());
				SkipTests(m_pDisplay->IsDepthBuffered() ? (m_uCommonZBufferFormats * TEST_CASES) : TEST_CASES);
				return false;
			}

			// update m_bAlphaPresent
			if (m_dwVersion <= 0x0700)
			{
//				m_bAlphaPresent = (m_pCommonTextureFormats[m_nTexFormat].ddpfPixelFormat.dwRGBAlphaBitMask != 0);
			}
			else
			{
//				switch (m_pCommonTextureFormats[m_nTexFormat].d3dfFormat)
                switch (m_fmtCommon[m_nTexFormat])
				{
					case D3DFMT_A8R8G8B8:
					case D3DFMT_A1R5G5B5:
					case D3DFMT_A4R4G4B4:
					case D3DFMT_A8R3G3B2:
					case D3DFMT_A8P8:
					case D3DFMT_A8L8:
					case D3DFMT_A4L4:
						m_bAlphaPresent = TRUE;
						break;
					default:
						m_bAlphaPresent = FALSE;
						break;
				}
			}

		}




		//create ZStencil with the new format.
/*
		if (m_bOffscreen)
		{
			pRendTgtSrc = m_pOffSrcSurf;
			pRendTgtRef = m_pOffRefSurf;
		}
		else
		{
//			pRendTgtSrc = m_pTextureRT->m_pSrcSurface;
            m_pTextureRT->GetSurfaceLevel(0, &pRendTgtSrc);
            pRendTgtSrc->Release();
//			pRendTgtRef = m_pTextureRT->m_pRefSurface;
		}
*/
		if (m_pDisplay->IsDepthBuffered())
		{
			if (!CreateZStencil(m_nZFormat))
			{
				WriteToLog("(INFO) CreateZStencil failed.\n");
    			SkipTests(TEST_CASES);
				return false;
			}
/*
			// Attach ZStencil to render-target.
			if (m_dwVersion <= 0x0700)
			{

				// Attach current ZBuffer to src surface
				if (!pRendTgtSrc->AttachSurface(m_pSrcZ))
				{
					WriteToLog("(Info) Failed Src AttachSurface(). (%X)\n", GetLastError());
    				SkipTests(TEST_CASES);
					return false;
				}

				// Attach current ZBuffer to ref surface
				if (!pRendTgtRef->AttachSurface(m_pRefZ))
				{
					WriteToLog("(Info) Failed Ref AttachSurface(). (%X)\n", GetLastError());
    				SkipTests(TEST_CASES);
					return false;
				}

			}

			// update m_bStencilPresent
			if (m_dwVersion <= 0x0700)
			{
				m_bStencilPresent = (m_pCommonZBufferFormats[m_nZFormat].ddpfPixelFormat.dwStencilBitDepth > 0);
			}
			else
			{
*/
//				switch (m_pCommonZBufferFormats[m_nZFormat].d3dfFormat)
                switch (m_fmtdCommon[m_nZFormat])
				{
					case D3DFMT_D15S1:
					case D3DFMT_D24S8:
						m_bStencilPresent = TRUE;
						break;
					default:
						m_bStencilPresent = FALSE;
						break;
				}
//			}
		}
		else
		{
			m_pSrcZ = NULL;
			m_pRefZ = NULL;
		}
	}


	//Change RenderTarget to texture/offscreen

	if (m_bOffscreen)
	{
		pRendTgtSrc = m_pOffSrcSurf;
		pRendTgtRef = m_pOffRefSurf;
	}
	else
	{
//		pRendTgtSrc = m_pTextureRT->m_pSrcSurface;
        if (FAILED(m_pTextureRT->GetSurfaceLevel(0, &pRendTgtSrc))) {
		    SkipTests(TEST_CASES);
		    return false;
        }
//		pRendTgtRef = m_pTextureRT->m_pRefSurface;
	}

	// Set the texture/offscreen src surface as the src render target
//	if (!SetRenderTarget(pRendTgtSrc, false, m_pSrcZ))
    if (FAILED(m_pDevice->SetRenderTarget(pRendTgtSrc, m_pSrcZ)))
	{
		WriteToLog("(Info) Failed Src SetRenderTarget(). (%X)\n", GetLastError());
		SkipTests(TEST_CASES);
        pRendTgtSrc->Release();
		return false;
	}

    if (!m_bOffscreen) {
        pRendTgtSrc->Release();
    }
/*
	// Set the texture/offscreen ref surface as the ref render target
	if (!SetRenderTarget(pRendTgtRef, true, m_pRefZ))
	{
		WriteToLog("(Info) Failed Ref SetRenderTarget(). (%X)\n", GetLastError());
		SkipTests(TEST_CASES);
		return false;
	}
*/

//	SetupViewport();
    SetViewport(0,0,IMGWIDTH,IMGHEIGHT);
	SetTexture(0, NULL);


	// clear the texture with black.
	if (BeginScene())
	{
		Clear((CLEAR_COLOR_0), 1.0, 1, D3DCLEAR_TARGET, 0, NULL);
		EndScene();
	}


	//Now we run the different clear() test cases and the results
	// go into the texture.
	switch(m_nTestCase)
	{

	// Clear Full Screen tests.
	//--------------------------
	case 0: // clear RGB full screen test.
		//Buffer must contain CLEAR_COLOR_FULL(red) after rendering grid. 
		if (BeginScene())
		{
			Clear((CLEAR_COLOR_FULL), 1.0, 1, D3DCLEAR_TARGET, 0, NULL);
			EndScene();
		}
		break;
	case 1: // clear Alpha full screen test
		//Buffer must contain CLEAR_COLOR_FULL_ALPHA1(red) after rendering grid.
		if (m_bAlphaPresent)
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL_ALPHA1), 1.0, 1, D3DCLEAR_TARGET, 0, NULL);
				EndScene();
			}
		}
		break;
	case 2: // clear Alpha full screen test
		//Buffer must contain black(0 color) after rendering grid.
		if (m_bAlphaPresent)
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL_ALPHA0), 1.0, 1, D3DCLEAR_TARGET, 0, NULL);
				EndScene();
			}
		}
		break;
	case 3: // clear Z full screen test
		// Triangles must NOT be rendered. So buffer must contain CLEAR_COLOR_FULL(red) after rendering grid.
		if (m_pDisplay->IsDepthBuffered())
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), LOWER_Z, 1, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, NULL);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)FALSE );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	case 4: // clear Z full screen test
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if (m_pDisplay->IsDepthBuffered())
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), HIGHER_Z, 1, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, NULL);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)FALSE );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	case 5: // clear Stencil full screen test.
		// Triangles must NOT be rendered. So buffer must contain CLEAR_COLOR_FULL(red) after rendering grid.
		if ( (m_pDisplay->IsDepthBuffered()) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), 1.0f, STENCIL_0, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, 0, NULL);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRENDERSTATE_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	case 6: // clear Stencil full screen test.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_pDisplay->IsDepthBuffered()) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), 1.0f, STENCIL_1, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, 0, NULL);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRENDERSTATE_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	case 7: // test to ensure ClearZ and ClearStencil does not affect each other.
		// Testing if clear Z affect stencil.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_pDisplay->IsDepthBuffered()) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), 0.0f, STENCIL_1, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0, NULL);
				Clear((CLEAR_COLOR_FULL), 0.0f, STENCIL_0, D3DCLEAR_ZBUFFER, 0, NULL);
				Clear((CLEAR_COLOR_FULL), 1.0f, STENCIL_0, D3DCLEAR_ZBUFFER, 0, NULL);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRENDERSTATE_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	case 8: // test to ensure ClearZ and ClearStencil does not affect each other.
		// Testing if clear stencil affect Z.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_pDisplay->IsDepthBuffered()) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), 1.0f, STENCIL_0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0, NULL);
				Clear((CLEAR_COLOR_FULL), 0.0f, STENCIL_0, D3DCLEAR_STENCIL, 0, NULL);
				Clear((CLEAR_COLOR_FULL), 0.0f, STENCIL_1, D3DCLEAR_STENCIL, 0, NULL);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRENDERSTATE_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	case 9: //test to ensure Clear render-target alone does not affect Z or stencil.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_pDisplay->IsDepthBuffered()) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), 1.0f, STENCIL_1, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0, NULL);
				Clear((CLEAR_COLOR_0), 0.0f, STENCIL_0, D3DCLEAR_TARGET, 0, NULL);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRENDERSTATE_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;


	// Clear List of Rects.
	//--------------------------

	case 10: // clear RGB - list of rects.
		//rects will be CLEAR_COLOR_FULL_2(blue), rest CLEAR_COLOR_FULL(red) after rendering grid. 
		if (BeginScene())
		{
			Clear((CLEAR_COLOR_FULL), 1.0, 1, D3DCLEAR_TARGET, 0, NULL);
			Clear((CLEAR_COLOR_FULL_2), 1.0, 1, D3DCLEAR_TARGET, 2, rectList);
			EndScene();
		}
		break;
	case 11: // clear alpha test - list of rects.
		//rects will be CLEAR_COLOR_FULL_ALPHA1(red), rest black after rendering grid. 
		if (m_bAlphaPresent)
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL_ALPHA0), 1.0, 1, D3DCLEAR_TARGET, 0, NULL);
				Clear((CLEAR_COLOR_FULL_ALPHA1), 1.0, 1, D3DCLEAR_TARGET, 2, rectList);
				EndScene();
			}
		}
		break;
	case 12: // clear Z test - list of rects.
		//rects will be RENDERING_COLOR(green), rest CLEAR_COLOR_FULL(red) after rendering grid. 
		if (m_pDisplay->IsDepthBuffered())
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), LOWER_Z, 1, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, NULL);
				Clear((CLEAR_COLOR_FULL), HIGHER_Z, 1, D3DCLEAR_ZBUFFER, 2, rectList);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)FALSE );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	case 13: // clear Stencil test - list of rects.
		//rects will be RENDERING_COLOR(green), rest CLEAR_COLOR_FULL(red) after rendering grid. 
		if ( (m_pDisplay->IsDepthBuffered()) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear((CLEAR_COLOR_FULL), 1.0f, STENCIL_0, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, 0, NULL);
				Clear((CLEAR_COLOR_FULL), 1.0f, STENCIL_1, D3DCLEAR_STENCIL, 2, rectList);

				SetRenderState( D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE );
#ifndef UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_CLIPPING, (DWORD)FALSE );
#endif // !UNDER_XBOX
				SetRenderState( D3DRENDERSTATE_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRENDERSTATE_ZENABLE, (DWORD)FALSE );
				SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRENDERSTATE_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRENDERSTATE_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount))
					WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
			}
		}
		break;
	default:
		break;
	}

	//if m_bOffscreen, copy offscreen rendered image to texture.
    if (m_bOffscreen)
    {
        if (!CopyToTexture())
            WriteToLog("(ERROR): CopyToTexture failed.\n");
    }

    // Reset to the original render target
//    if (!SetRenderTarget(m_pOriginalSrcRT, false, m_pOriginalSrcZ))
    if (FAILED(m_pDevice->SetRenderTarget(m_pOriginalSrcRT, m_pOriginalSrcZ)))
        WriteToLog("(ERROR): Couldn't reset Src RenderTarget. (%X)\n", GetLastError());

//    if (!SetRenderTarget(m_pOriginalRefRT, true, m_pOriginalRefZ))
//        WriteToLog("(ERROR): Couldn't reset Ref RenderTarget. (%X)\n", GetLastError());

//    SetupViewport();
    m_pDevice->SetViewport(&m_vpTest);

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);


	switch(m_nTestCase)
	{
	case 1:
	case 2:
	case 11:
		// for test cases testing alpha clear.
		SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
		SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);
		break;
	default:
		SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
		break;
	}


    SetTexture(0, m_pTextureRT);



	return true;
}



void CClearTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)FALSE);
		SetRenderState( D3DRENDERSTATE_STENCILENABLE, (DWORD)FALSE );

        if (!RenderPrimitive(D3DPT_TRIANGLELIST, TexVertexFVF, (void *)VPGrid, dwVertexCount))
            WriteToLog("(ERROR): RenderPrimitive failed. (%X)\n", GetLastError());

        if (!EndScene())
            WriteToLog("(ERROR): EndScene failed. (%X)\n", GetLastError());
	}

	return;
}


void CClearTest::DisplaySetup(void)
{
//	g_pD3DFramework->SetClientSize(IMGWIDTH * 2, IMGHEIGHT);
	return;
}


bool CClearTest::ProcessFrame()
{
    char    szBuffer[80];
    char    szTestcase[150];
    char	szFormat[256];
    static  int nPass = 0;
    static  int nFail = 0;
    bool    bResult = true;

	// Get Render-target format and ZStencil-buffer description.
	DescribeFormat(szFormat, m_nTexFormat, m_nZFormat);
	// Get test case description.
	DescribeTestcase(szTestcase, m_nTestCase);

	//Show test information in the status window.
    WriteStatus("$gCurrent Format",szFormat);
    WriteStatus("$gCurrent Testcase",szTestcase);

    // Use the standard 15% for non Invalid cases
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

		//log the failure result.
		WriteToLog("TEST FAILED!");
		WriteToLog(szFormat);
		WriteToLog(szTestcase);
    }

    WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}



bool CClearTest::TestTerminate(void)
{
	RELEASE(m_pSrcZ);
	RELEASE(m_pOffSrcSurf);
    m_pDevice->SetTexture(0, NULL);
	ReleaseTexture(m_pTextureRT);
    m_pTextureRT = NULL;

    if (m_pOriginalSrcRT) {
        m_pOriginalSrcRT->Release();
        m_pOriginalSrcRT = NULL;
    }
    if (m_pOriginalSrcZ) {
        m_pOriginalSrcZ->Release();
        m_pOriginalSrcZ = NULL;
    }

	return true;
}

/*
bool CClearTest::IsStencilPresent(void) {

    CSurface8*      pd3ds;
    D3DSURFACE_DESC d3dsd;
    HRESULT         hr;

    hr = m_pDevice->GetDepthStencilSurface(&pd3ds);
    if (FAILED(hr)) {
        return false;
    }

    hr = pd3ds->GetDesc(&d3dsd);
    pd3ds->Release();
    if (FAILED(hr)) {
        return false;
    }

    switch (d3dsd.Format) {
        case D3DFMT_D15S1:
        case D3DFMT_D24S8:
        case D3DFMT_D24X4S4:
            return true;
        case D3DFMT_D16:
        case D3DFMT_D16_LOCKABLE:
        case D3DFMT_D32:
        case D3DFMT_D24X8:
        default:
            return false;
    }

    return false;
}
*/

//******************************************************************************
BOOL CClearTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CClearTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CClearTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
