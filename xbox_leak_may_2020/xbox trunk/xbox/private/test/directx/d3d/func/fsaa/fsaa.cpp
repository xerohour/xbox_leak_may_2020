/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fsaa.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "fsaa.h"

#define LDST (22.0f)
#define SDST (20.0f)

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

    CMultiSuperSample* pFSAA;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pFSAA = new CMultiSuperSample();
    if (!pFSAA) {
        return FALSE;
    }

    // Initialize the scene
    if (!pFSAA->Create(pDisplay)) {
        pFSAA->Release();
        return FALSE;
    }

    bRet = pFSAA->Exhibit(pnExitCode);

    // Clean up the scene
    pFSAA->Release();

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
// CMultiSuperSample
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CMultiSuperSample
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
CMultiSuperSample::CMultiSuperSample() {

    m_pd3dt = NULL;
    m_fAngle = M_5PIDIV4;
    m_uRenderTarget = 0;
    m_uMultiSampleType = -1;
    m_uMultiSampleFormat = 0;
    m_uNumBackBuffers = 1;
    if (GetStartupContext() & TSTART_HARNESS) {
        m_uPresentInterval = 1;
    }
    else {
        m_uPresentInterval = 0;
    }
    m_bReset = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CMultiSuperSample
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
CMultiSuperSample::~CMultiSuperSample() {

    if (m_pd3dt) {
        m_pd3dt->Release();
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
BOOL CMultiSuperSample::Prepare() {

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Initialize
//
// Description:
//
//     Initialize the device and all device objects to be used in the test (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the test was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CMultiSuperSample::Initialize() {

    float fAngle;
    UINT  i;

    m_prWhiteBase[0] = LVERTEX(D3DXVECTOR3(-LDST, -LDST, 50.0f), RGBA_MAKE(255, 255, 255, 255), 0, 0.0f, 0.0f);
    m_prWhiteBase[1] = LVERTEX(D3DXVECTOR3(-LDST,  LDST, 50.0f), RGBA_MAKE(255, 255, 255, 255), 0, 0.0f, 0.0f);
    m_prWhiteBase[2] = LVERTEX(D3DXVECTOR3( LDST,  LDST, 50.0f), RGBA_MAKE(255, 255, 255, 255), 0, 0.0f, 0.0f);
    m_prWhiteBase[3] = LVERTEX(D3DXVECTOR3( LDST, -LDST, 50.0f), RGBA_MAKE(255, 255, 255, 255), 0, 0.0f, 0.0f);

    m_prSpokesBase[0] = LVERTEX(D3DXVECTOR3(0.0f, 0.0f, 39.5f), 0, 0, 0.0f, 0.0f);

    for (i = 1, fAngle = 0.0f; i <= NUM_SPOKES; i++, fAngle += (M_2PI / NUM_SPOKES)) {
        m_prSpokesBase[i] = LVERTEX(D3DXVECTOR3((float)cos(fAngle) * LDST * 10.0f, (float)sin(fAngle) * LDST * 10.0f, 39.5f), 0, 0, 0.0f, 0.0f);
    }

    for (i = 0; i < NUM_SPOKES; i++) {
        m_pwSpokes[i*2] = (WORD)0;
        m_pwSpokes[i*2+1] = (WORD)(i + 1);
    }

    m_pDisplay->GetPresentParameters(&m_d3dpp);
    return CScene::Initialize();
}

//******************************************************************************
//
// Method:
//
//     Efface
//
// Description:
//
//     Release all device resource objects (or at least those objects created
//     in video memory, non-local video memory, or the default memory pools)
//     and restore the device to its initial state.
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
void CMultiSuperSample::Efface() {

    CScene::Efface();
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
void CMultiSuperSample::Update() {

    CSurface8* pd3ds;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT d3dlr;
    WORD wSurface;
    D3DXMATRIX mWorld;
    float fAngle = M_PI / 500.0f;
    BOOL bReset = m_bReset;
    D3DFORMAT fmtPrefilter;
    HRESULT hr;
    UINT i;
    static D3DFORMAT fmtr[] = {
        D3DFMT_LIN_X1R5G5B5,
        D3DFMT_LIN_R5G6B5,
        D3DFMT_LIN_X8R8G8B8,
        D3DFMT_LIN_A8R8G8B8
    };
    static D3DFORMAT fmtd[] = {
        D3DFMT_LIN_D16,
        D3DFMT_LIN_F16,
        D3DFMT_LIN_D24S8,
        D3DFMT_LIN_F24S8
    };
    static D3DMULTISAMPLE_TYPE mstType[] = {
	    D3DMULTISAMPLE_NONE,
	    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,
	    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,
	    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR,
	    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,
	    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,
	    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,
	    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,
	    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,
	    D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN,
	    D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN,
    };
    static D3DMULTISAMPLE_TYPE mstFormat[] = {
	    D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT,
	    D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5,
	    D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5,
	    D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8,
	    D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8
    };
    static UINT uPresentInterval[] = {
        D3DPRESENT_INTERVAL_ONE,
        D3DPRESENT_INTERVAL_IMMEDIATE
    };
    static UINT uWidthScale[] = {
        1,
        2,
        2,
        2,
        1,
        2,
        2,
        2,
        2,
        3,
        3
    };
    static UINT uHeightScale[] = {
        1, 
        1,
        1,
        1,
        2,
        2,
        2,
        2,
        2,
        3,
        3,
    };
    static LPTSTR szFmtr[] = {
        {TEXT("D3DFMT_LIN_X1R5G5B5")},
        {TEXT("D3DFMT_LIN_R5G6B5")},
        {TEXT("D3DFMT_LIN_X8R8G8B8")},
        {TEXT("D3DFMT_LIN_A8R8G8B")}
    };
    static LPTSTR szFmtd[] = {
        {TEXT("D3DFMT_LIN_D16")},
        {TEXT("D3DFMT_LIN_F16")},
        {TEXT("D3DFMT_LIN_D24S8")},
        {TEXT("D3DFMT_LIN_F24S8")}
    };
    static LPTSTR szMstType[] = {
        {TEXT("D3DMULTISAMPLE_NONE")},
        {TEXT("D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR")},
        {TEXT("D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX")},
        {TEXT("D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR")},
        {TEXT("D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR")},
        {TEXT("D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR")},
        {TEXT("D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN")},
        {TEXT("D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR")},
        {TEXT("D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN")},
        {TEXT("D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN")},
        {TEXT("D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN")}
    };
    static LPTSTR szMstFormat[] = {
        {TEXT("D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT")},
        {TEXT("D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5")},
        {TEXT("D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5")},
        {TEXT("D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8")},
        {TEXT("D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8")}
    };
    static LPTSTR szPresentInterval[] = {
        {TEXT("D3DPRESENT_INTERVAL_ONE")},
        {TEXT("D3DPRESENT_INTERVAL_IMMEDIATE")}
    };

    if (m_fAngle >= M_5PIDIV4) {

        D3DSURFACE_DESC d3dsd;
        D3DLOCKED_RECT  d3dlr;
        LPDWORD         pdwTexel;
        UINT            uLength;
        float           fLength, fX, fY;
        UINT            j;

        if (++m_uMultiSampleType >= countof(mstType)) {
            m_uMultiSampleType = 0;
            if (++m_uMultiSampleFormat >= countof(mstFormat)) {
                m_uMultiSampleFormat = 0;
                if (++m_uRenderTarget >= countof(fmtr)) {
                    m_uRenderTarget = 0;
                    if (++m_uNumBackBuffers >= 3) {
                        m_uNumBackBuffers = 1;
                        if (GetStartupContext() & TSTART_HARNESS) {
//                            m_bReset = !m_bReset;
//                            if (m_bReset) {
                                if (GetStartupContext() & TSTART_SEQUENTIAL) {
                                    SetFrameDuration(m_fFrame);
                                }
//                            }
                        }
                        else {
                            if (++m_uPresentInterval > countof(uPresentInterval)) {
                                m_uPresentInterval = 0;
//                                m_bReset = !m_bReset;
//                                if (m_bReset) {
                                    if (GetStartupContext() & TSTART_SEQUENTIAL) {
                                        SetFrameDuration(m_fFrame);
                                    }
//                                }
                            }
                        }
                    }
                }
            }
        }

        if (!bReset) {

            // D3DRS_MULTISAMPLETYPE is dependent on the prefilter buffer being compatible
            // with the types switched from and to.  Therefore changing the render state
            // using this switch statement depends on the ordering of mstType remaining static
            // as it is declared above.
            switch (mstType[m_uMultiSampleType]) {

	            case D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX:
	            case D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR:
	            case D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN:
	            case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR:
	            case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN:
	            case D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN:
//                    m_pDevice->SetRenderState(D3DRS_MULTISAMPLETYPE, mstType[m_uMultiSampleType]);
                    break;

                default:
                    bReset = TRUE;
                    break;
            }
        }

        if (bReset) {

            // Reset the device to a new sample type (cycle through aliased, all multisample types, all supersample types, and double/triple buffering)
            m_d3dpp.BackBufferFormat = fmtr[m_uRenderTarget];
            m_d3dpp.AutoDepthStencilFormat = fmtd[m_uRenderTarget];
            if (mstType[m_uMultiSampleType] == D3DMULTISAMPLE_NONE)
                m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
            else
                m_d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)(mstType[m_uMultiSampleType] | mstFormat[m_uMultiSampleFormat]);
            m_d3dpp.BackBufferCount = m_uNumBackBuffers;
            m_d3dpp.FullScreen_PresentationInterval = uPresentInterval[m_uPresentInterval];

//            Log(LOG_COMMENT, TEXT("Resetting device"));
            Log(LOG_COMMENT, TEXT("Multisample type:      %s"), szMstType[m_uMultiSampleType]);
            Log(LOG_COMMENT, TEXT("Multisample format:    %s"), szMstFormat[m_uMultiSampleFormat]);
            Log(LOG_COMMENT, TEXT("Target format:         %s"), szFmtr[m_uRenderTarget]);
            Log(LOG_COMMENT, TEXT("Depth format:          %s"), szFmtd[m_uRenderTarget]);
            Log(LOG_COMMENT, TEXT("Back buffer count:     %d;"), m_uNumBackBuffers);
            Log(LOG_COMMENT, TEXT("Presentation interval: %s\r\n"), szPresentInterval[m_uPresentInterval]);

            if (!m_pDisplay->Reset(&m_d3dpp)) {
                m_bQuit = TRUE;
                return;
            }
        }

        _stprintf(m_szFSAADesc, TEXT("Multisample type: %s\n")
                                TEXT("Prefilter format: %s\n")
                                TEXT("Target format:    %s\n")
                                TEXT("Depth format:     %s\n")
                                TEXT("Back buffers:     %d\n")
                                TEXT("Present interval: %s\n"),
                                szMstType[m_uMultiSampleType],
                                szMstFormat[m_uMultiSampleFormat],
                                szFmtr[m_uRenderTarget], szFmtd[m_uRenderTarget],
                                m_uNumBackBuffers, 
                                szPresentInterval[m_uPresentInterval]);

#if 0

        D3DGAMMARAMP d3dgr;

        for (i = 0; i < 256; i++) {
            d3dgr.red[i] = (BYTE)i;
            d3dgr.green[i] = (BYTE)i;
            d3dgr.blue[i] = (BYTE)i;
        }

        m_pDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION | D3DSGR_IMMEDIATE, &d3dgr);
#endif

        if (bReset) {

            // Verify the pre-filter buffer
            hr = m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
                Log(LOG_FAIL, TEXT("GetBackBuffer failed on prefilter buffer"));
            }
            else {

                // Verify the surface description of the pre-filter buffer
                hr = pd3ds->GetDesc(&d3dsd);
                if (!ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
                
                    if (d3dsd.Width != m_pDisplay->GetWidth() * uWidthScale[m_uMultiSampleType]) {
                        Log(LOG_FAIL, TEXT("Surface width of %d incorrectly returned for prefilter buffer (expected %d)"), d3dsd.Width, m_pDisplay->GetWidth() * uWidthScale[m_uMultiSampleType]);
                    }

                    if (d3dsd.Height != m_pDisplay->GetHeight() * uHeightScale[m_uMultiSampleType]) {
                        Log(LOG_FAIL, TEXT("Surface height of %d incorrectly returned for prefilter buffer (expected %d)"), d3dsd.Height, m_pDisplay->GetHeight() * uHeightScale[m_uMultiSampleType]);
                    }

                    if (mstFormat[m_uMultiSampleFormat] == D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT) {
                        fmtPrefilter = fmtr[m_uRenderTarget];
                    }
                    else {
                        fmtPrefilter = fmtr[m_uMultiSampleFormat - 1];
                    }
                    if (d3dsd.Format != fmtPrefilter) {
                        Log(LOG_FAIL, TEXT("Surface format of 0x%X incorrectly returned for prefilter buffer (expected 0x%X)"), d3dsd.Format, fmtPrefilter);
                    }

                    if (d3dsd.Type != D3DRTYPE_SURFACE) {
                        Log(LOG_FAIL, TEXT("Surface type of %d incorrectly returned for prefilter buffer (expected D3DRTYPE_SURFACE)"), d3dsd.Type);
                    }

                    if (d3dsd.Usage != D3DUSAGE_RENDERTARGET) {
                        Log(LOG_FAIL, TEXT("Surface usage of %d incorrectly returned for prefilter buffer (expected D3DUSAGE_RENDERTARGET)"), d3dsd.Usage);
                    }

                    if (d3dsd.Size < m_pDisplay->GetWidth() * uWidthScale[m_uMultiSampleType] * m_pDisplay->GetHeight() * uHeightScale[m_uMultiSampleType] * (FormatToBitDepth(fmtPrefilter) / 8)) {
                        Log(LOG_FAIL, TEXT("Surface size of %d incorrectly returned for prefilter buffer (expected at least %d)"), d3dsd.Size, m_pDisplay->GetWidth() * uWidthScale[m_uMultiSampleType] * m_pDisplay->GetHeight() * uHeightScale[m_uMultiSampleType] * (FormatToBitDepth(fmtPrefilter) / 8));
                    }

                    if (d3dsd.MultiSampleType != mstType[m_uMultiSampleType]) {
                        Log(LOG_FAIL, TEXT("Surface type of %d incorrectly returned for prefilter buffer (expected %d)"), d3dsd.MultiSampleType, mstType[m_uMultiSampleType]);
                    }

                    // Verify the pre-filter buffer can be locked
                    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
                    if (!ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {

                        wSurface = *(LPWORD)d3dlr.pBits;

                        if (d3dlr.Pitch < (int)(m_pDisplay->GetWidth() * uWidthScale[m_uMultiSampleType] * (FormatToBitDepth(fmtPrefilter) / 8))) {
                            Log(LOG_FAIL, TEXT("Surface pitch of %d incorrectly returned for locked prefilter buffer (expected at least %d)"), d3dlr.Pitch, m_pDisplay->GetWidth() * uWidthScale[m_uMultiSampleType] * (FormatToBitDepth(fmtPrefilter) / 8));
                        }

                        pd3ds->UnlockRect();
                    }
                }

                pd3ds->Release();
            }
        }

        d3dsd.Width = m_pDisplay->GetWidth();
        d3dsd.Height = m_pDisplay->GetHeight();

        // Set the white quad vertices
        memcpy(m_prWhite, m_prWhiteBase, 4 * sizeof(LVERTEX));

        // Set the spokes
        memcpy(m_prSpokes, m_prSpokesBase, (NUM_SPOKES + 1) * sizeof(LVERTEX));

        // Set the textured quad vertices
        uLength = d3dsd.Width > d3dsd.Height ? d3dsd.Height : d3dsd.Width;
        fLength = (float)(uLength - (uLength / 2)) / (float)sqrt(2.0f);
        fX = ((float)d3dsd.Width - fLength) / 2.0f;
        fY = ((float)d3dsd.Height - fLength) / 2.0f;
        uLength = (UINT)(fLength + 0.5f);

        m_vOffset = D3DXVECTOR3((float)(d3dsd.Width / 2), (float)(d3dsd.Height / 2), 0.0f);

        m_prTexBase[0] = TLVERTEX(D3DXVECTOR3(fX,           fY + fLength, 0.1f), 0.9f, RGBA_MAKE(255, 255, 255, 255), 0, 0.0f, fLength);
        m_prTexBase[1] = TLVERTEX(D3DXVECTOR3(fX,           fY,           0.1f), 0.9f, RGBA_MAKE(255, 255, 255, 255), 0, 0.0f, 0.0f);
        m_prTexBase[2] = TLVERTEX(D3DXVECTOR3(fX + fLength, fY,           0.1f), 0.9f, RGBA_MAKE(255, 255, 255, 255), 0, fLength, 0.0f);
        m_prTexBase[3] = TLVERTEX(D3DXVECTOR3(fX + fLength, fY + fLength, 0.1f), 0.9f, RGBA_MAKE(255, 255, 255, 255), 0, fLength, fLength);
        memcpy(m_prTextured, m_prTexBase, 4 * sizeof(TLVERTEX));

        // Create a linear texture whose dimensions match the textured quad dimensions
        if (m_pd3dt) {
            m_pd3dt->Release();
        }
        m_pDevice->CreateTexture(uLength, uLength, 1, 0, D3DFMT_LIN_X8R8G8B8, POOL_DEFAULT, &m_pd3dt);

        m_pd3dt->LockRect(0, &d3dlr, NULL, 0);
        pdwTexel = (LPDWORD)d3dlr.pBits;
        for (i = 0; i < uLength; i++) {
            for (j = 0; j < uLength; j++) {
                pdwTexel[j] = D3DCOLOR_RGBA(rand() % 255, rand() % 255, rand() % 255, 255);
            }
            pdwTexel += d3dlr.Pitch >> 2;
        }
        m_pd3dt->UnlockRect(0);

        m_fAngle -= M_PI;
    }

    m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (BOOL)(m_fAngle < M_3PIDIV4));

    InitMatrix(&mWorld,
                (float)cos(-m_fAngle), (float)sin(-m_fAngle), 0.0f, 0.0f,
                -(float)sin(-m_fAngle), (float)cos(-m_fAngle), 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
    );

    for (i = 0; i < 4; i++) {
        D3DXVec3TransformCoord(&m_prWhite[i].vPosition, &m_prWhiteBase[i].vPosition, &mWorld);
    }

    for (i = 1; i <= NUM_SPOKES; i++) {
        D3DXVec3TransformCoord(&m_prSpokes[i].vPosition, &m_prSpokesBase[i].vPosition, &mWorld);
    }

    InitMatrix(&mWorld,
                (float)cos(m_fAngle), (float)sin(m_fAngle), 0.0f, 0.0f,
                -(float)sin(m_fAngle), (float)cos(m_fAngle), 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
    );

    for (i = 0; i < 4; i++) {
        D3DXVec3TransformCoord(&m_prTextured[i].vPosition, &(m_prTexBase[i].vPosition - m_vOffset), &mWorld);
        m_prTextured[i].vPosition += m_vOffset;
    }

    m_fAngle += fAngle;
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
BOOL CMultiSuperSample::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

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
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView(&m_camInitial);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

    m_pDevice->SetTexture(0, NULL);

    m_pDevice->SetVertexShader(FVF_LVERTEX);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, NUM_SPOKES + 1, NUM_SPOKES, m_pwSpokes, D3DFMT_INDEX16, m_prSpokes, sizeof(LVERTEX));

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prWhite, sizeof(LVERTEX));

    m_pDevice->SetTexture(0, m_pd3dt);

    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prTextured, sizeof(TLVERTEX));

    m_pDevice->SetTexture(0, NULL);

    xTextOut(20.0f, 380.0f, 8.0f, 15.0f, RGBA_XCOLOR(255, 0, 255, 255), m_szFSAADesc, _tcslen(m_szFSAADesc));

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
    if (m_pfnFade) {
        (this->*m_pfnFade)();
    }

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
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
void CMultiSuperSample::ProcessInput() {

    CScene::ProcessInput();
}
//******************************************************************************
//
// Method:
//
//     InitView
//
// Description:
//
//     Initialize the camera view in the scene.
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
BOOL CMultiSuperSample::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -50.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = M_PI / 4.0f;
    m_camInitial.fNearPlane    = 0.1f;
    m_camInitial.fFarPlane     = 1000.0f;
    m_pDisplay->SetCamera(&m_camInitial);

    return m_pDisplay->SetView(&m_camInitial);
}

//******************************************************************************
// Scene window procedure (pseudo-subclassed off the main window procedure)
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Scene window procedure to process messages received by the main 
//     application window.
//
// Arguments:
//
//     LRESULT* plr             - Result of the message processing
//
//     HWND hWnd                - Application window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     TRUE if the message was handled, FALSE otherwise.
//
//******************************************************************************
BOOL CMultiSuperSample::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_F12: // Toggle pause state
                    m_bPaused = !m_bPaused;
                    return TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
