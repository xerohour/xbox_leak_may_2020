//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "TLVertexClip.h"

const D3DVALUE RHW = 1.0f;

const DWORD g_dwDX6APIListSize = 3;
const DWORD g_dwDX7APIListSize = 1;

RENDERPRIMITIVEAPI g_pDX6APIList[g_dwDX6APIListSize] =
{
    RP_BE,  // Begin/Vertex/End
    RP_DP,  // DrawPrimitive
    RP_DPS, // DrawPrimitiveStrided
};

RENDERPRIMITIVEAPI g_pDX7APIList[g_dwDX7APIListSize] =
{
    RP_DPVB, // DrawPrimitiveVB
};

//CD3DWindowFramework App;

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

    CTLVertexClip*  pTLVClipTest;
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
                pTLVClipTest = new CTLVertexClipEdge();
                break;
            case 1:
                pTLVClipTest = new CTLVertexClipQuad();
                break;
        }

        if (!pTLVClipTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pTLVClipTest->Create(pDisplay)) {
            pTLVClipTest->Release();
            return FALSE;
        }

        bRet = pTLVClipTest->Exhibit(pnExitCode);

        bQuit = pTLVClipTest->AbortedExit();

        // Clean up the scene
        pTLVClipTest->Release();
    }

    return bRet;
}

// NAME:        CTLVertexClip()
// DESC:        TL vertex clip class constructor
// INPUT:       none
// OUTPUT:      none

CTLVertexClip::CTLVertexClip() : m_Port(PORT_X, PORT_Y, PORT_WIDTH, PORT_HEIGHT)
{
	m_bShow = false;

	m_nTests = (UINT) 0;
	m_Type = D3DPT_LINELIST;

	m_dwLineColor = RGBA_MAKE(0, 255, 255, 255);
	m_dwPortColor = RGBA_MAKE(0,   0,   0,   0);

    m_ModeOptions.fReference = false;

    m_bExit = FALSE;
}

// NAME:        ~CTLVertexClip()
// DESC:        TL vertex clip class destructor
// INPUT:       none
// OUTPUT:      none

CTLVertexClip::~CTLVertexClip()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CTLVertexClip::CommandLineHelp(void)
{
//    WriteCommandLineHelp("$yline: $wline color $c(default(ARGB)=0x00FFFFFF)");
//    WriteCommandLineHelp("$yport: $wport color $c(default(ARGB)=0x00000000)");
//    WriteCommandLineHelp("$yshow: $wshow port $c($wOn$c/Off)");
}

// NAME:        SetupViewport()
// DESC:        set up viewport
// INPUT:       none
// OUTPUT:      none

bool CTLVertexClip::SetupViewport(void)
{
    return SetViewport(m_Port.Left, m_Port.Top, m_Port.Width(), m_Port.Height());
}

// NAME:        ClearFrame()
// DESC:        clear frame
// INPUT:       none
// OUTPUT:      Clear()

bool CTLVertexClip::ClearFrame(void)
{
    bool bResult;

    (void) SetViewport(0, 0, m_pDisplay->GetWidth(), m_pDisplay->GetHeight());
    bResult = Clear(RGBA_MAKE(0, 0, 0, 255));
    SetViewport(m_Port.Left, m_Port.Top, m_Port.Width(), m_Port.Height());

    return bResult;
}

// NAME:        TestInitialize()
// DESC:        test initialize
// INPUT:       none
// OUTPUT:      D3DTESTINIT_RUN

UINT CTLVertexClip::TestInitialize(void)
{
    CHAR szBuffer[32];
    UINT uTemp;
    CSurface8* pd3ds;
    D3DSURFACE_DESC d3dsd;

    if (0x0600 == m_dwVersion)
    {
        SetAPI(g_pDX6APIList, g_dwDX6APIListSize, 2);
    }
    else if (0x0700 == m_dwVersion)
    {
        SetAPI(g_pDX7APIList, g_dwDX7APIListSize, 2);
    }
    else
    {
    }

//    SetDisplayMode(DISPLAYMODE_WINDOW_SRCONLY);

    // line color
    // default:  white

//    lstrcpy(szBuffer, "");
//    ReadString("line", szBuffer);
//    m_dwLineColor = (DWORD) (sscanf(szBuffer, "%x", &uTemp) == 1) ? uTemp : 0x00FFFFFF;
//    m_dwLineColor &= 0x00ffffff;
    m_dwLineColor = 0x00ffffff;

    // port color
    // default:  black

//	lstrcpy(szBuffer, "");
//	ReadString("port", szBuffer);
//    m_dwPortColor = (DWORD) (sscanf(szBuffer, "%x", &uTemp) == 1) ? uTemp : 0x00000000;
//    m_dwPortColor &= 0x00ffffff;
    m_dwPortColor = 0;

    if (KeySet("show"))	
        m_bShow = true;

    if (FAILED(m_pDevice->GetRenderTarget(&pd3ds))) {
        return D3DTESTINIT_SKIPALL;
    }
    pd3ds->GetDesc(&d3dsd);
    pd3ds->Release();
    GetARGBPixelDesc(d3dsd.Format, &pixd);

//    WriteToLog("Alpha (%0x), Red (%0x), Green (%0x), Blue (%0x)\n",
//               m_pSrcTarget->m_dwAlphaMask,
//               m_pSrcTarget->m_dwRedMask,
//               m_pSrcTarget->m_dwGreenMask,
//               m_pSrcTarget->m_dwBlueMask);
    WriteToLog("Alpha (%0x), Red (%0x), Green (%0x), Blue (%0x)\n",
               pixd.dwAMask,
               pixd.dwRMask,
               pixd.dwGMask,
               pixd.dwBMask);

//    UINT n = m_pMode->nSrcDevice;

    // 2x, one for each primitive (D3DPT_LINELIST & D3DPT_LINESTRIP)
	SetTestRange((UINT) 1, (UINT) (2*m_nTests));

	DWORD dwDevCaps = m_d3dcaps.DevCaps;
	DWORD dwMinTextureWidth = 1;//m_d3dcaps.MinTextureWidth;
	DWORD dwMinTextureHeight = 1;//m_d3dcaps.MinTextureHeight;
	DWORD dwMaxTextureWidth = m_d3dcaps.MaxTextureWidth;
	DWORD dwMaxTextureHeight = m_d3dcaps.MaxTextureHeight;

    if (m_dwVersion >= 0x0700)
	    SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD) FALSE);

	// set up clip port

	D3DCOLOR dcDiffuse(m_dwPortColor);
	D3DCOLOR dcSpecular(0x00000000);
	D3DVECTOR A = cD3DVECTOR((float)m_Port.Left, (float)m_Port.Top, 0.0f);
	D3DVECTOR B = cD3DVECTOR((float)(m_Port.Right -1), (float)m_Port.Top, 0.0f);

	// since test line uses points [0] & [1], start clip port's line list at [2]

	m_Vertices[2] = cD3DTLVERTEX(A, RHW, dcDiffuse, dcSpecular, 0, 0);
	m_Vertices[3] = cD3DTLVERTEX(B, RHW, dcDiffuse, dcSpecular, 0, 0);
	A.x = (float)(m_Port.Right -1);
	A.y = (float)(m_Port.Bottom -1);
	m_Vertices[4] = cD3DTLVERTEX(B, RHW, dcDiffuse, dcSpecular, 0, 0);
	m_Vertices[5] = cD3DTLVERTEX(A, RHW, dcDiffuse, dcSpecular, 0, 0);
	B.x = (float)(m_Port.Left);
	B.y = (float)(m_Port.Bottom -1);
	m_Vertices[6] = cD3DTLVERTEX(B, RHW, dcDiffuse, dcSpecular, 0, 0);
	m_Vertices[7] = cD3DTLVERTEX(A, RHW, dcDiffuse, dcSpecular, 0, 0);
	A.x = (float)(m_Port.Left);
	A.y = (float)(m_Port.Top);
	m_Vertices[8] = cD3DTLVERTEX(A, RHW, dcDiffuse, dcSpecular, 0, 0);
	m_Vertices[9] = cD3DTLVERTEX(B, RHW, dcDiffuse, dcSpecular, 0, 0);

	WriteToLog("Using a clip rect of (Left, Top, Right, Bottom) (%ld, %ld, %ld, %ld).\n",
               m_Port.Left, m_Port.Top, m_Port.Right, m_Port.Bottom );

	return (UINT) D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test index
// OUTPUT:      true

bool CTLVertexClip::ExecuteTest(UINT uTest)
{
	D3DCOLOR dcDiffuse(m_dwLineColor);
	D3DCOLOR dcSpecular(0x00000000);
	D3DVECTOR A, B;

	uTest--; // test coords are zero based

	if (m_nTests <= uTest)
	{
		m_Type = D3DPT_LINESTRIP;
		sprintf(m_szBuffer, "D3DPT_LINESTRIP" );
		uTest -= m_nTests; // so that the range is still 0..m_nTests
	}
	else
	{
		m_Type = D3DPT_LINELIST;
		sprintf(m_szBuffer, "D3DPT_LINELIST");
	}

	SetEndPoints(&A, &B, uTest);
	m_Vertices[0] = cD3DTLVERTEX(A, RHW, dcDiffuse, dcSpecular, 0, 0);
	m_Vertices[1] = cD3DTLVERTEX(B, RHW, dcDiffuse, dcSpecular, 0, 0);

	sprintf(m_szBuffer, "%s from %ld, %ld to %ld, %ld ", m_szBuffer, A.x, A.y, B.x, B.y);
	BeginTestCase(m_szBuffer);

    return true;
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      true.....if any extra pixels are present
//              false....otherwise

bool CTLVertexClip::ProcessFrame(void)
{
    bool bResult;
    static int nPass = 0;
    static int nFail = 0;

    bResult = !AnyExtraPixels();

    if (bResult)
    {
        (m_fIncrementForward) ? nPass++ : nPass--;
        sprintf(m_szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else
    {
        (m_fIncrementForward) ? nFail++ : nFail--;
        sprintf(m_szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
    }

    WriteStatus("$gOverall Results", m_szBuffer);
    return bResult;
}

// NAME:        SceneRefresh()
// DESC:        scene refresh
// INPUT:       none
// OUTPUT:      none

void CTLVertexClip::SceneRefresh(void)
{
    if (BeginScene())
    {
        RenderPrimitive(m_Type, D3DFVF_TLVERTEX, m_Vertices, 2, NULL, 0, 0);

        if (m_bShow)
            RenderPrimitive(D3DPT_LINELIST, D3DFVF_TLVERTEX, &m_Vertices[2], 8, NULL, 0, 0);

        EndScene();
    }
}

// NAME:        PixelCheckAtDepth()
// DESC:        check for bad pixels
// INPUT:       pBuffer..pixel buffer
//              pdwDx....x error
//              pdwDy....y error
// OUTPUT:      nBad.....number of bad pixels

int CTLVertexClip::PixelCheckAtDepth(PWORD pBuffer, long *pdwDx, long *pdwDy)
{
    int nBad = 0;
    BYTE *pbTemp = (BYTE *) pBuffer;
//    DWORD dwFilter = (m_pSrcTarget->m_dwAlphaMask << m_pSrcTarget->m_nAlphaShift) | m_pSrcTarget
//->m_dwFilter;
    DWORD dwFilter = pixd.dwAMask;

    for (long y = 0; y < (long)m_pDisplay->GetHeight(); y++)
    {
        pBuffer = (PWORD) pbTemp;

        for (long x = 0; x < (long)m_pDisplay->GetWidth(); x++)
        {
            if (!m_Port.Inside(x, y))
            {
                if ((*pBuffer & dwFilter) != m_dwClearColor)
                {
                    nBad++;

                    if (x < m_Port.Left)
                    {
                        if (m_Port.Left - x > *pdwDx)
                            *pdwDx = m_Port.Left - x;
                    }
                    else
                    {
                        // we have to use -1 here because m_Port.Right is the first bad pixel,
                        // but we want the distance to the bad pixel to the last good one
                        if (x - (m_Port.Right-1) > *pdwDx)
                            *pdwDx = x - (m_Port.Right -1);
                    }

                    if (y < m_Port.Top)
                    {
                        if (m_Port.Top - y > *pdwDy)
                            *pdwDy = m_Port.Top - y;
                    }
                    else
                    {
                        // we have to use -1 here because m_Port.Bottom is the first bad pixel,
                        // but we want the distance to the bad pixel to the last good one
                        if (y - (m_Port.Bottom -1) > *pdwDy)
                            *pdwDy = y - (m_Port.Bottom -1);
                    }
                }
            }

            pBuffer++;
        }

        pbTemp += m_dwPitch;
    }

    return nBad;
}

// NAME:        PixelCheckAtDepth()
// DESC:        check for bad pixels
// INPUT:       pBuffer..pixel buffer
//              pdwDx....x error
//              pdwDy....y error
// OUTPUT:      nBad.....number of bad pixels

int CTLVertexClip::PixelCheckAtDepth(PDWORD pBuffer, long *pdwDx, long *pdwDy)
{
    int nBad = 0;
    BYTE *pbTemp = (BYTE *) pBuffer;
//    DWORD dwFilter = (m_pSrcTarget->m_dwAlphaMask << m_pSrcTarget->m_nAlphaShift) | m_pSrcTarget->m_dwFilter;
    DWORD dwFilter = pixd.dwAMask;

	for (long y = 0; y < (long)m_pDisplay->GetHeight(); y++)
	{
		pBuffer = (PDWORD) pbTemp;

		for (long x = 0; x < (long)m_pDisplay->GetWidth(); x++)
		{
			if (!m_Port.Inside(x, y))
			{
				if ((*pBuffer & dwFilter) != m_dwClearColor)
				{
					nBad++;

					if (x <= m_Port.Left)
					{
						if (m_Port.Left - x > *pdwDx)
							*pdwDx = m_Port.Left - x;
					}
					else
					{
						if (x - m_Port.Right > *pdwDx)
							*pdwDx = x - m_Port.Right;
					}

					if (y <= m_Port.Top)
					{
						if (m_Port.Top - y > *pdwDy)
							*pdwDy = m_Port.Top - y;
					}
					else
					{
						if (y - m_Port.Bottom > *pdwDy)
							*pdwDy = y - m_Port.Bottom;
					}
				}
			}

			pBuffer++;
		}

		pbTemp += m_dwPitch;
	}

	return nBad;
}

// NAME:        PixelCheckAtDepth()
// DESC:        check for bad pixels
// INPUT:       pBuffer..pixel buffer
//              pdwDx....x error
//              pdwDy....y error
// OUTPUT:      nBad.....number of bad pixels

int CTLVertexClip::PixelCheckAtDepth(PRGBSIZE pBuffer, long *pdwDx, long *pdwDy)
{
	int	nBad = 0;
	BYTE *pbTemp = (BYTE *) pBuffer;
	RGBSIZE	black= { 0, 0, 0 };

	for (long y = 0; y < (long)m_pDisplay->GetHeight(); y++)
	{
		pBuffer = (PRGBSIZE) pbTemp;

		for (long x = 0; x < (long)m_pDisplay->GetWidth(); x++)
		{
			if (!m_Port.Inside(x, y))
			{
				if (memcmp((void *) pBuffer, (void *) &black, sizeof(RGBSIZE)))
				{
					nBad++;

					if (x <= m_Port.Left)
					{
						if (m_Port.Left - x > *pdwDx)
							*pdwDx = m_Port.Left - x;
					}
					else
					{
						if (x - m_Port.Right > *pdwDx)
							*pdwDx = x - m_Port.Right;
					}

					if (y <= m_Port.Top)
					{
						if (m_Port.Top - y > *pdwDy)
							*pdwDy = m_Port.Top - y;
					}
					else
					{
						if (y - m_Port.Bottom > *pdwDy)
							*pdwDy = y - m_Port.Bottom;
					}
				}
			}

			pBuffer++;
		}

		pbTemp += m_dwPitch;
	}

	return nBad;
}

// NAME:        AnyExtraPixels()
// DESC:        extra pixel check
// INPUT:       none
// OUTPUT:      true.....if any extra pixels are present
//              false....otherwise

bool CTLVertexClip::AnyExtraPixels(void)
{
	int nBad = 0;
	long dwDx = 0, dwDy = 0;
	void *pSurfBuf;
    CSurface8* pd3ds;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT d3dlr;

    m_pDevice->GetRenderTarget(&pd3ds);
    pd3ds->GetDesc(&d3dsd);
//	pSurfBuf = m_pSrcTarget->Lock();
    pd3ds->LockRect(&d3dlr, NULL, 0);
    pSurfBuf = d3dlr.pBits;

	if (NULL == pSurfBuf)
	{
        pd3ds->Release();
		WriteToLog("AnyExtraPixels:  Could not lock image.\n");
		Fail();
		return false;
	}

//	m_dwPitch = m_pSrcTarget->m_lPitch;
    m_dwPitch = d3dlr.Pitch;

    switch(FormatToBitDepth(d3dsd.Format))
    {
        case 8:
        break;

        case 16:
            nBad = PixelCheckAtDepth((PWORD) pSurfBuf, &dwDx, &dwDy);
        break;

        case 24:
            nBad = PixelCheckAtDepth((PRGBSIZE) pSurfBuf, &dwDx, &dwDy);
        break;

        case 32:
            nBad = PixelCheckAtDepth((PDWORD) pSurfBuf, &dwDx, &dwDy);
        break;
    }

//	m_pSrcTarget->Unlock();
    pd3ds->UnlockRect();
    pd3ds->Release();

    if (0 != nBad)
        WriteToLog("\t%d bad pixels(s) with max distance of %ld, %ld\n", nBad, dwDx, dwDy);

	return (0 != nBad);
}

// NAME:        PxFromTest()
// DESC:        point x coord from test index
// INPUT:       nPoint...point index
// OUTPUT:      none

float CTLVertexClip::PxFromTest(UINT nPoint)
{
    switch(nPoint/POINTS_PER_SIDE)
    {
        case 0:
            return (float) ((500.0*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE);
        case 1:
            return 500.0f;
        case 2:
            return (float) (500.0 - ((500.0*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE));
        case 3:
            return 0.0f;
        default:
            return 325.0f;
    }

    return 0.0f;
}

// NAME:        PyFromTest()
// DESC:        point y from test index
// INPUT:       nPoint...point index
// OUTPUT:      none

float CTLVertexClip::PyFromTest(UINT nPoint)
{
    switch(nPoint/POINTS_PER_SIDE)
    {
        case 0:
            return 0.0f;
        case 1:
            return (float) ((500.0*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE);
        case 2:
            return 500.0f;
        case 3:
            return (float) (500.0 - ((500.0*(nPoint % POINTS_PER_SIDE)) / POINTS_PER_SIDE));
        default:
            return 325.0f;
    }

    return 0.0f;
}

//******************************************************************************
BOOL CTLVertexClip::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CTLVertexClip::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CTLVertexClip::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
