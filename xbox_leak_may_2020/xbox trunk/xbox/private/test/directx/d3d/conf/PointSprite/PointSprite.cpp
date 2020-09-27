// FILE:        pointsprite.cpp
// DESC:        point sprite class methods for point sprite conformance test
// AUTHOR:      Todd M. Frost
// COMMENTS:

#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PointSprite.h"
//#include "testutils.h"
//#include "cimageloader.h"
//#include "ddraw.h"

#ifndef RGB_MAKE
#define RGB_MAKE(r, g, b) ((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#endif

//CD3DWindowFramework   App;

// NAME:        fRed()
// DESC:        red function
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      2*sqrt((u-0.5)*(u-0.5) + (v-0.5)*(v-0.5))

FLOAT fRed(FLOAT u, FLOAT v)
{
	u -= 0.5f;
	v -= 0.5f;
	return (FLOAT) (2.0*sqrt(u*u + v*v));
}

// NAME:        fGreen()
// DESC:        green function
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      1 - 2*sqrt((u-0.5)*(u-0.5) + (v-0.5)*(v-0.5))

FLOAT fGreen(FLOAT u, FLOAT v)
{
	u -= 0.5f;
	v -= 0.5f;
	return (FLOAT) (1.0 - 2.0*sqrt(u*u + v*v));
}

// NAME:        fBlue()
// DESC:        blue function
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      2*sqrt(u*u + v*v) - 1

FLOAT fBlue(FLOAT u, FLOAT v)
{
	return (FLOAT) (2.0*sqrt(u*u + v*v) - 1.0);
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

    CSprite*        pSprite;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 7 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pSprite = new CSpriteAttenuateFVFL();
                break;
            case 1:
                pSprite = new CSpriteAttenuateFVFLS();
                break;
            case 2:
                pSprite = new CSpriteScaleFVFL();
                break;
            case 3:
                pSprite = new CSpriteScaleFVFLS();
                break;
            case 4:
                pSprite = new CSpriteScaleFVFTL();
                break;
            case 5:
                pSprite = new CSpriteScaleFVFTLS();
                break;
            case 6:
                pSprite = new CSpriteBatch();
                break;
        }

        if (!pSprite) {
            return FALSE;
        }

        // Initialize the scene
        if (!pSprite->Create(pDisplay)) {
            pSprite->Release();
            return FALSE;
        }

        bRet = pSprite->Exhibit(pnExitCode);

        bQuit = pSprite->AbortedExit();

        // Clean up the scene
        pSprite->Release();
    }

    return bRet;
}


// NAME:        CSprite()
// DESC:        constructor for sprite class
// INPUT:       none
// OUTPUT:      none

CSprite::CSprite(VOID)
{
    m_dcBackground = RGB_MAKE(255, 255, 255);

    m_dvAngle = 55.0f*pi/180.0f;
    m_dvFar = 90.0f;
    m_dvNear = 1.0f;

    m_At.x = 0.0f;
    m_At.y = 0.0f;
    m_At.z = 0.0f;

    m_From.x = 0.0f;
    m_From.y = 0.0f;
    m_From.z = -4.0f;

    m_Up.x = 0.0f;
    m_Up.y = 1.0f;
    m_Up.z = 0.0f;

    m_iFormat = (INT) NOFORMAT;
    m_nFormats = (INT) 0;
    m_pFormats = (INT *) NULL;

    m_nVariationsPerFormat = (UINT) 0;

//    m_pImage = NULL;

    m_pTexture = NULL;

    m_Sprite.Ka.f = 1.0f;           // default = 1.0f
    m_Sprite.Kb.f = 0.0f;           // default = 0.0f
    m_Sprite.Kc.f = 0.0f;           // default = 0.0f
    m_Sprite.PointSize.f = 1.0f;    // default = 1.0f
    m_Sprite.PointSizeMax.f = 1.0f; // default = 1.0f, range >= 0.0f
    m_Sprite.PointSizeMin.f = 1.0f; // default = 1.0f, range >= 0.0f

	m_ModeOptions.uMinDXVersion = 0x0800;	

    m_bExit = FALSE;
}

// NAME:        ~CSprite()
// DESC:        destructor for sprite class
// INPUT:       none
// OUTPUT:      none

CSprite::~CSprite(VOID)
{
//	delete m_pImage;
//	m_pImage = NULL;
    ReleaseTexture(m_pTexture);
    m_pTexture = NULL;
	free( m_pFormats );
	m_pFormats = NULL;
}


// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSprite::CommandLineHelp(void)
{
//	WriteCommandLineHelp("$yvariations: $wvariations per texture $c(default=%d)", NVDEF);
//    WriteCommandLineHelp("$yvalidate: $wvalidate $c(On/$wOff$c)");
//    WriteCommandLineHelp("$ysymmetric: $wsymmetric $c($wOn$c/Off)");
//	WriteCommandLineHelp("$ytexoff: $wtexoff $c($wYes$c/No)");
//	WriteCommandLineHelp("$yflatshade: $wflatshade $c($wOn$c/Off)");
}


// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSprite::TestInitialize(VOID)
{
//    CImageLoader Loader;
//    DWORD dwFlags = (DDPF_PALETTEINDEXED1 |
//                     DDPF_PALETTEINDEXED2 |
//                     DDPF_PALETTEINDEXEDTO8 |
//                     DDPF_PALETTEINDEXED8);
    DWORD dwWidth = (DWORD) TEXTURE_WIDTH, dwHeight = (DWORD) TEXTURE_HEIGHT;
    UINT i, j;

    if (0x0800 > m_dwVersion)
        return D3DTESTINIT_SKIPALL;

    ProcessArgs();

//    for (m_nFormats = 0, i = 0; i < m_uCommonTextureFormats; i++)
//        if ((m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & DDPF_RGB) &&
//            !(m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & dwFlags))
//            m_nFormats++;
    m_nFormats = m_uCommonTextureFormats;

    if (0 == m_nFormats)
    {
        WriteToLog("Device does NOT support RGB texture formats.\n");
        return D3DTESTINIT_SKIPALL;
    }

    m_pFormats = (INT *) malloc(m_nFormats*sizeof(INT));
	for (j = 0, i = 0; i < m_uCommonTextureFormats; i++)
//        if ((m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & DDPF_RGB) &&
//            !(m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & dwFlags))
            if (j < (UINT)m_nFormats)
            {
                m_pFormats[j] = (INT) i;
                j++;
            }

	D3DCAPS8 Src;//, Ref;
    m_pSrcDevice8->GetDeviceCaps(&Src);
//    m_pRefDevice8->GetDeviceCaps(&Ref);
//    m_Sprite.PointSizeMax.f = (Src.MaxPointSize < Ref.MaxPointSize) ? Src.MaxPointSize : Ref.MaxPointSize;
    m_Sprite.PointSizeMax.f = Src.MaxPointSize;
    m_Flags.vSet(VERTEXPOINTSIZE);

#ifndef UNDER_XBOX
    if (!(Src.FVFCaps & D3DFVFCAPS_PSIZE))
#else
    if (Src.FVFCaps & D3DFVFCAPS_PSIZE)
    {
        Log(LOG_FAIL, TEXT("FVFCaps contains D3DFVFCAPS_PSIZE when D3DFVF_PSIZE is unsupported"));
    }
#endif
    {
        WriteToLog("Src device does not support point size in vertex data.\n");
        m_Flags.vClear(VERTEXPOINTSIZE);
    }

//    if (!(Ref.FVFCaps & D3DFVFCAPS_PSIZE))
//    {
//        WriteToLog("Ref device does not support point size in vertex data.\n");
//        m_Flags.vClear(VERTEXPOINTSIZE);
//    }
    
    if (!(m_Sprite.PointSizeMax.f > 1.0f))
    {
        WriteToLog("Device does NOT support point sprites.\n");

        if (m_Sprite.PointSizeMax.f < 1.0f)
            WriteToLog("Note:  dvMaxPointSize = %f (expected 1.0).\n", m_Sprite.PointSizeMax.f);

        return D3DTESTINIT_SKIPALL;
    }

	// find max point that can be contained within the port
    m_dvPointSizeMax = (D3DVALUE) ((m_pDisplay->GetWidth() > m_pDisplay->GetHeight()) ? m_pDisplay->GetHeight() : m_pDisplay->GetWidth());
    m_dvPointSizeMax = (m_Sprite.PointSizeMax.f < m_dvPointSizeMax) ? m_Sprite.PointSizeMax.f : m_dvPointSizeMax;

    m_ViewportCenter.x = (D3DVALUE) (m_pDisplay->GetWidth() >> 1);
    m_ViewportCenter.y = (D3DVALUE) (m_pDisplay->GetHeight() >> 1);
    m_ViewportCenter.z = (D3DVALUE) 0.5;

//    CHANNELDATA Data;
    Data.dwAlpha = (DWORD) 0;
    Data.dwRed = (DWORD) CIL_UNSIGNED;
    Data.dwGreen = (DWORD) CIL_UNSIGNED;
    Data.dwBlue = (DWORD) CIL_UNSIGNED;
    Data.fAlpha = NULL;

    if (m_Flags.bCheck(SYMMETRIC))
    {
        Data.fRed = fRed;
        Data.fGreen = fGreen;
        Data.fBlue = fRed;
    }
    else
    {
        Data.fRed = fRed;
        Data.fGreen = fGreen;
        Data.fBlue = fBlue;
    }

//    if (!(m_pImage = Loader.LoadChannelData(dwWidth, dwHeight, &Data)))
//    {
//        WriteToLog("Unable to load image.\n");
//        return D3DTESTINIT_ABORT;
//    }

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSprite::ExecuteTest(UINT uTest)
{
    bool bFound = false, bValid = true;
    int iFormat, iStage = 0;
    CHAR szBuffer[MAXBUFFER];

	iFormat = uTest / m_nVariationsPerFormat;
	if (iFormat < m_nFormats)
		bFound = true;

    if (!bFound)
        return false;

	if (m_Flags.bCheck(TEXTUREON))
	{
		if (iFormat != m_iFormat)
		{
//			DWORD dwFlags = CDDS_TEXTURE_VIDEO;
			DWORD dwWidth = (DWORD) TEXTURE_WIDTH, dwHeight = (DWORD) TEXTURE_HEIGHT;
            int nTextureFormat;

			m_iFormat = iFormat;
			nTextureFormat = m_pFormats[m_iFormat];

			ReleaseTexture(m_pTexture);
//##REVIEW: PointSprite doesn't like D3DFMT_DXT1 -- it gets drawn black
			if (!(m_pTexture = CreateChannelDataTexture(m_pDevice, dwWidth, dwHeight, &Data, m_fmtCommon[nTextureFormat])))
//			if (!(m_pTexture = CreateChannelDataTexture(m_pDevice, dwWidth, dwHeight, &Data, m_fmtCommon[iFormat])))
			{
				WriteToLog("Unable to create texture.\n");
				return false;
			}
		}
	}
	else
	{
		ReleaseTexture(m_pTexture);
        m_pTexture = NULL;
	}
    sprintf(szBuffer, "$y%f", m_Sprite.PointSize.f);
    WriteStatus("$gSize", szBuffer);
    sprintf(szBuffer, "$y%f", m_Sprite.PointSizeMin.f);
    WriteStatus("$gMin", szBuffer);
    sprintf(szBuffer, "$y%f", m_Sprite.PointSizeMax.f);
    WriteStatus("$gMax", szBuffer);
    sprintf(szBuffer, "$y%f", m_Sprite.Ka.f);
    WriteStatus("$gKa", szBuffer);
    sprintf(szBuffer, "$y%f", m_Sprite.Kb.f);
    WriteStatus("$gKb", szBuffer);
    sprintf(szBuffer, "$y%f", m_Sprite.Kc.f);
    WriteStatus("$gKc", szBuffer);

    SetRenderState(D3DRS_POINTSIZE, m_Sprite.PointSize.dw);
    SetRenderState(D3DRS_POINTSIZE_MIN, m_Sprite.PointSizeMin.dw);
    SetRenderState(D3DRS_POINTSCALE_A, m_Sprite.Ka.dw);
    SetRenderState(D3DRS_POINTSCALE_B, m_Sprite.Kb.dw);
    SetRenderState(D3DRS_POINTSCALE_C, m_Sprite.Kc.dw);

#ifdef UNDER_XBOX
    iStage = 3;
#endif // UNDER_XBOX

	if (m_Flags.bCheck(TEXTUREON))
	{
		SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) D3DTADDRESS_CLAMP);
		SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) D3DTADDRESS_CLAMP);
		SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
		SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
		SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);
		SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
		SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
		SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_DIFFUSE);
		SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
		SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
		SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_DIFFUSE);
		SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
		SetTexture(iStage, m_pTexture);
		iStage++;

#ifndef UNDER_XBOX
		SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
		SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
#endif // !UNDER_XBOX
	}
	else
	{
		SetTexture(iStage, NULL);
		iStage--;
	}
	
	if (m_Flags.bCheck(FLATSHADE))
	{
		SetRenderState(D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT);
	}
	else
	{
		SetRenderState(D3DRS_SHADEMODE, (DWORD)D3DSHADE_GOURAUD);
	}

#ifndef UNDER_XBOX

    if (m_Flags.bCheck(VALIDATE))
    {
        DWORD dwPasses;
        HRESULT hResult;

        if (m_pSrcDevice8)
            if (D3D_OK != (hResult = m_pSrcDevice8->ValidateDevice(&dwPasses)))
            {
                WriteToLog("Src requires %d passes:  %X.\n",
                           dwPasses, hResult);
                bValid = false;
            }
    }

#endif // !UNDER_XBOX

    if (!bValid)
        SkipTests((UINT) 1);

    return true;
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      bResult..result of src:ref image comparison

bool CSprite::ProcessFrame(void)
{
    bool bResult;
    CHAR szBuffer[MAXBUFFER];
    static int nPass = 0;
    static int nFail = 0;

    bResult = GetCompareResult(0.15f, 0.78f, (int) 0);

    if (m_Flags.bCheck(INVALID))
    {
        (m_fIncrementForward) ? nPass++ : nPass--;
        sprintf(szBuffer, "$yPass (invalid parameters): %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else if (bResult)
    {
        (m_fIncrementForward) ? nPass++ : nPass--;
        sprintf(szBuffer, "$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else
    {
        (m_fIncrementForward) ? nFail++ : nFail--;
        sprintf(szBuffer, "$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
    }

    WriteStatus("$gSummary", szBuffer);
    return bResult;
}

// NAME:        TestTerminate()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true

bool CSprite::TestTerminate(VOID)
{
    return true;
}

// NAME:        ClearFrame()
// DESC:        clear frame
// INPUT:       none
// OUTPUT:      m_dcBackround

bool CSprite::ClearFrame(VOID)
{
	bool bResult = Clear(m_dcBackground);
    //SetViewport(20, 20, 280, 240);
    return bResult;
}

// NAME:        SetDefaultMatrices()
// DESC:        setup default matrices
// INPUT:       none
// OUTPUT:      none

bool CSprite::SetDefaultMatrices(VOID)
{
    D3DMATRIX Matrix;

    Matrix = ProjectionMatrix(m_dvNear, m_dvFar, m_dvAngle);
    Matrix = MatrixMult(Matrix, (float) (1.0/Matrix.m[2][3]));

    if (!SetTransform(D3DTS_PROJECTION, &Matrix))
        return false;

    Matrix = ViewMatrix(m_From, m_At, m_Up);

    if (!SetTransform(D3DTS_VIEW, &Matrix))
        return false;

    return true;
}

// NAME:        SetDefaultRenderStates()
// DESC:        set default render states
// INPUT:       none
// OUTPUT:      true.....if SetRenderState() succeeds
//              false....otherwise

bool CSprite::SetDefaultRenderStates(VOID)
{
    SetRenderState(D3DRS_POINTSCALEENABLE, (DWORD) FALSE);
    SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);
    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) TRUE);
	return true;
}

// NAME:        ProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CSprite::ProcessArgs(VOID)
{
    int iDefault, iResult;

    if (KeySet("INVALID"))
        m_Flags.vSet(INVALID);
    else
        m_Flags.vClear(INVALID);

	if (KeySet("TEXOFF"))
		m_Flags.vClear(TEXTUREON);
	else
		m_Flags.vSet(TEXTUREON);

	if (KeySet("FLATSHADE"))
		m_Flags.vSet(FLATSHADE);
	else
		m_Flags.vClear(FLATSHADE);

    if (KeySet("WHQL"))
    {
        m_Flags.vSet(VALIDATE);
        m_nVariationsPerFormat = (UINT) NVDEF;
        return;
    }

    if (KeySet("SYMMETRIC"))
        m_Flags.vSet(SYMMETRIC);
    else
        m_Flags.vClear(SYMMETRIC);

    // variations
    // default:  NVDEF 

    iDefault = (int) NVDEF;
    ReadInteger("variations", iDefault, &iResult);
    m_nVariationsPerFormat = ((iResult < NVMIN) ? NVMIN : ((iResult >= NVMAX) ? NVMAX: iResult));

    // validate
    // default:  true

    if (!KeySet("!validate"))
        m_Flags.vSet(VALIDATE);
}

// NAME:        SetPosition()
// DESC:        set position (x,y)
// INPUT:       uPosition....position index
//              pfX..........x coordinate
//              pfY..........y coordinate
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise
// all the values in SetPosition should be in pixels.

VOID CSprite::SetPosition(UINT nPosition, float *pfX, float *pfY)
{
	float fHalfPointSize = m_Sprite.PointSize.f/2.0f;
//    DWORD dwWidth = m_pDisplay->GetWidth();
//    DWORD dwHeight = m_pDisplay->GetHeight();
    DWORD dwWidth = m_vpTest.X + m_vpTest.Width;
    DWORD dwHeight = m_vpTest.Y + m_vpTest.Height;
    float fPosition[NCLIPTESTS][2] =
    {
        { (float) (	m_vpTest.X + fHalfPointSize),				(float) ( m_vpTest.Y + fHalfPointSize ) },
        { (float) (	m_ViewportCenter.x ),						(float) ( m_vpTest.Y + fHalfPointSize ) },
        { (float) (	dwWidth - fHalfPointSize ),	                (float) ( m_vpTest.Y + fHalfPointSize ) },
        { (float) ( dwWidth - fHalfPointSize ),	                (float) ( m_ViewportCenter.y ) },
        { (float) ( dwWidth - fHalfPointSize ),	                (float) ( dwHeight-fHalfPointSize ) },
        { (float) ( m_ViewportCenter.x ),						(float) ( dwHeight-fHalfPointSize ) },
        { (float) ( m_vpTest.X + fHalfPointSize ),				(float) ( dwHeight-fHalfPointSize ) },
        { (float) ( m_vpTest.X + fHalfPointSize ),				(float) ( m_ViewportCenter.y ) },
        { (float) ( m_vpTest.X ),								(float) ( m_vpTest.Y ) },
        { (float) ( m_ViewportCenter.x ),						(float) ( m_vpTest.Y ) },
        { (float) ( dwWidth ),					                (float) ( m_vpTest.Y ) },
        { (float) ( dwWidth ),					                (float) ( m_ViewportCenter.y ) },
        { (float) ( dwWidth ),					                (float) ( dwHeight ) },
        { (float) ( m_ViewportCenter.x ),						(float) ( dwHeight ) },
        { (float) ( m_vpTest.X ),								(float) ( dwHeight ) },
		{ (float) ( m_vpTest.X ),								(float) ( m_ViewportCenter.y ) },
        { (float) ( m_vpTest.X - fHalfPointSize ),				(float) ( m_vpTest.Y - fHalfPointSize ) },		
		{ (float) ( m_ViewportCenter.x ),						(float) ( m_vpTest.Y - fHalfPointSize ) },
		{ (float) ( dwWidth + fHalfPointSize ),	                (float) ( m_vpTest.Y - fHalfPointSize ) },
		{ (float) ( dwWidth + fHalfPointSize ),	                (float) ( m_ViewportCenter.y ) },
		{ (float) ( dwWidth + fHalfPointSize ),	                (float) ( dwHeight + fHalfPointSize ) },
		{ (float) ( m_ViewportCenter.x ),						(float) ( dwHeight + fHalfPointSize ) },
		{ (float) ( m_vpTest.X - fHalfPointSize ),				(float) ( dwHeight + fHalfPointSize ) },
		{ (float) ( m_vpTest.X - fHalfPointSize ),				(float) ( m_ViewportCenter.y ) },        
	};
    UINT n = (nPosition < NCLIPTESTS) ? nPosition : 0;

    *pfX = fPosition[n][0];
    *pfY = fPosition[n][1];
}

//******************************************************************************
BOOL CSprite::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CSprite::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CSprite::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
