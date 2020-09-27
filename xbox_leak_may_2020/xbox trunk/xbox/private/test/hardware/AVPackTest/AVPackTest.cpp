#include <xtl.h>
#include <xgraphics.h>
#include <xbfont.h>
#include <xbinput.h>
#include <av.h>

extern "C" extern DWORD D3D__AvInfo;

extern "C"
{
void    WINAPI D3DDevice_EnableCC(BOOL Enable);
void    WINAPI D3DDevice_SendCC(BOOL Field, BYTE cc1, BYTE cc2);
void    WINAPI D3DDevice_GetCCStatus(BOOL *pField1, BOOL *pField2);
};

class CXBD3DDevice
{
protected:
    // Main objects used for creating and rendering the 3D scene
	UINT				  m_Adapter;			// Adapter for this device
	UINT				  m_AdapterMode;		// Display mode for this apapter
	D3DDISPLAYMODE		  m_DisplayMode;		// Adapter display mode

    D3DPRESENT_PARAMETERS m_d3dpp;				// D3D presentation parameters
    LPDIRECT3D8           m_pD3D;              // The D3D enumerator object
    LPDIRECT3DDEVICE8     m_pd3dDevice;        // The D3D rendering device
    LPDIRECT3DSURFACE8    m_pBackBuffer;       // The backbuffer

	// AV Pack Info
	DWORD					m_VideoStandard;
	DWORD					m_VideoFlags;
	WCHAR 					m_szAvPackLine1[64];
	WCHAR 					m_szAvPackLine2[64];
	WCHAR 					m_szAvPackLine3[64];

	// Font
	CXBFont                 m_Font;

	// Misc
	CHAR	szText[64];

	// Functions
	VOID AVPackDescription();
	VOID SetDisplayMode();
	VOID SetD3DPresentationParams();
	DWORD IREtoDW(double dfIre);
	BOOL SmpteBars(void);
	BOOL ButtonMap(void);

public:
	// Variables
	DWORD					m_AvPack;

	// Display mode values, to change display modes set one or all of these values
	// and call Reset()
	UINT	m_DisplayWidth;
	UINT	m_DisplayHeight;
	BOOL	m_Interlaced;
	BOOL	m_Widescreen;
	BOOL	m_ClosedCaptioning;
	BOOL	m_DisplayButtonMap;

    // Functions to create, run, and clean up the application
	HRESULT Init();					//
	HRESULT Create();
	HRESULT	Reset();
	HRESULT Destroy();
	HRESULT Render();

	// Constructor
	CXBD3DDevice();
};


//-----------------------------------------------------------------------------
// Name: CXBD3DDevice()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBD3DDevice::CXBD3DDevice() {
    // Direct3D variables
	m_Adapter		  = D3DADAPTER_DEFAULT;
	m_AdapterMode	  = D3DCURRENT_DISPLAY_MODE;

	ZeroMemory(&m_DisplayMode, sizeof(m_DisplayMode));

	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
    m_pD3D            = NULL;
    m_pd3dDevice      = NULL;
    m_pBackBuffer     = NULL;

	// Av Pack Info
	m_AvPack = XGetAVPack();
	m_VideoStandard = XGetVideoStandard();
	m_VideoFlags = XGetVideoFlags();
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Init the D3D interface and HDTV modes
//-----------------------------------------------------------------------------
HRESULT CXBD3DDevice::Init() {
	// Create the Direct3D object
    OutputDebugStringA("XBApp: Initializing Direct3D...\n");
    if(NULL == (m_pD3D = Direct3DCreate8(D3D_SDK_VERSION))) {
        OutputDebugStringA("XBApp: Unable to create Direct3D!\n");
        return E_FAIL;
    }
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the app
//-----------------------------------------------------------------------------
HRESULT CXBD3DDevice::Create() {
	HRESULT hr;

	OutputDebugStringA("XBApp: Creating the D3D device...\n");

	// Set desired display mode parameters
	m_DisplayWidth		= 640;
	m_DisplayHeight		= 480;
	m_Interlaced		= TRUE;
	m_Widescreen		= FALSE;
	m_ClosedCaptioning	= TRUE;
	m_DisplayButtonMap	= FALSE;

	// Get Adapter Mode
	SetDisplayMode();

	// Set up the presentation parameters
	SetD3DPresentationParams();	
    
    // Create the device
    if(FAILED(hr = m_pD3D->CreateDevice(m_Adapter, D3DDEVTYPE_HAL, NULL, 
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                           &m_d3dpp, &m_pd3dDevice))) {
        OutputDebugStringA("XBApp: Could not create D3D device!\n");
        return hr;
    }

	// Enable everything in D3D_AvInfo
    D3D__AvInfo |= AV_FLAGS_WIDESCREEN
                    | AV_FLAGS_HDTV_480p
                    | AV_FLAGS_HDTV_720p
                    | AV_FLAGS_HDTV_1080i
                    | AV_FLAGS_HDTV_480i
                    | AV_FLAGS_60Hz;

	// Reset with new modes
	SetDisplayMode();
	SetD3DPresentationParams();	
	if(FAILED(hr = m_pd3dDevice->Reset(&m_d3dpp))) {
        OutputDebugStringA("XBApp: Could not reset D3D device!\n");
        return hr;
    }

	wsprintfA(szText, "Width = %u Height = %u\n", m_DisplayMode.Width, m_DisplayMode.Height);
	OutputDebugStringA(szText);

	// Create description string
	AVPackDescription();

    // Store pointers to the depth and back buffers
    m_pd3dDevice->GetBackBuffer(0, 0, &m_pBackBuffer);
	
	// Create a font
	if(FAILED(m_Font.Create( m_pd3dDevice, "d:\\Media\\Font.xpr"))) {
		OutputDebugStringA("m_Font.Create() FAILED\n");
        return E_FAIL;
	}

	// Set Closed Captioning
    D3DDevice_EnableCC(m_ClosedCaptioning);
#if 0
	if(FAILED(D3DDevice_EnableCC(m_ClosedCaptioning))) {
		wsprintfA(szText, "m_pd3dDevice->EnableCC(%s) FAILED\n", m_ClosedCaptioning ? "ON" : "OFF");
		OutputDebugStringA(szText);
		return E_FAIL;
	}
#endif 

    // Clear the backbuffer
    m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L);
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: Reset D3D Device to specified mode
//-----------------------------------------------------------------------------
HRESULT CXBD3DDevice::Reset() {
    HRESULT hr;

	OutputDebugStringA("XBApp: Reseting the D3D device...\n");

	// Release back buffer
	m_pBackBuffer->Release();

	// Set adapter and display mode
	SetDisplayMode();

	// Set up the presentation parameters
	SetD3DPresentationParams();	

    // Reset the device
    if(FAILED(hr = m_pd3dDevice->Reset(&m_d3dpp))) {
        OutputDebugStringA("XBApp: Could not reset D3D device!\n");
        return hr;
    }

	m_pD3D->GetAdapterDisplayMode(m_Adapter, &m_DisplayMode);

	// Get back buffer
	m_pd3dDevice->GetBackBuffer(0, 0, &m_pBackBuffer);

	// Set Closed Captioning
    D3DDevice_EnableCC(m_ClosedCaptioning);
#if 0
	if(FAILED(D3DDevice_EnableCC(m_ClosedCaptioning))) {
		wsprintfA(szText, "m_pd3dDevice->EnableCC(%s) FAILED\n", m_ClosedCaptioning ? "ON" : "OFF");
		OutputDebugStringA(szText);
		return E_FAIL;
	}
#endif 0

	// Create description string
	AVPackDescription();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleanup objects
//-----------------------------------------------------------------------------
HRESULT CXBD3DDevice::Destroy() {
    // Release display objects
	if(m_pBackBuffer) {
		m_pBackBuffer->Release();
        m_pBackBuffer = NULL;
    }
	
	if(m_pd3dDevice) {
		m_pd3dDevice->Release();
		m_pd3dDevice = NULL;
    }
    
	if(m_pD3D) {
		m_pD3D->Release();
		m_pD3D = NULL;
    }

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetDisplayMode()
// Desc: Set m_AdapterMode and m_DisplayMode from: m_DisplayWidth, m_DisplayHeight,
//			m_Interlaced, and m_Widscreen;
//-----------------------------------------------------------------------------
VOID CXBD3DDevice::SetDisplayMode() {
	UINT AdapterMode, AdapterModeCount;

	AdapterModeCount = m_pD3D->GetAdapterModeCount(m_Adapter);
	
	for(AdapterMode = 0; AdapterMode < AdapterModeCount; AdapterMode++) {
		m_pD3D->EnumAdapterModes(m_Adapter, AdapterMode, &m_DisplayMode);

		if((m_DisplayMode.Width == m_DisplayWidth) &&
				(m_DisplayMode.Height == m_DisplayHeight) &&
				(((m_DisplayMode.Flags & D3DPRESENTFLAG_INTERLACED) == D3DPRESENTFLAG_INTERLACED) == m_Interlaced) &&
				(((m_DisplayMode.Flags & D3DPRESENTFLAG_PROGRESSIVE) == D3DPRESENTFLAG_PROGRESSIVE) == !m_Interlaced) &&
				(((m_DisplayMode.Flags & D3DPRESENTFLAG_WIDESCREEN) ==  D3DPRESENTFLAG_WIDESCREEN) == m_Widescreen) &&
				((m_DisplayMode.Flags & D3DPRESENTFLAG_FIELD) != D3DPRESENTFLAG_FIELD)) {
			m_AdapterMode = AdapterMode;
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// Name: SetD3DPresentationParams()
// Desc: Populate m_d3dpp with info from m_DisplayMode
//-----------------------------------------------------------------------------
VOID CXBD3DDevice::SetD3DPresentationParams() {
	// Valid formats for AutoDepthStencilFormat
	D3DFORMAT Formats[] = {
        D3DFMT_D24S8, 
        D3DFMT_D16, 
        D3DFMT_F24S8, 
        D3DFMT_F16, 
        D3DFMT_D16_LOCKABLE 
    };
    UINT numFormats = sizeof(Formats) / sizeof(Formats[0]);
	UINT i;

	m_d3dpp.BackBufferWidth					= m_DisplayMode.Width;
    m_d3dpp.BackBufferHeight				= m_DisplayMode.Height;
    m_d3dpp.BackBufferFormat				= m_DisplayMode.Format;
    m_d3dpp.BackBufferCount					= 1;
	m_d3dpp.Flags							= m_DisplayMode.Flags;
    m_d3dpp.MultiSampleType					= D3DMULTISAMPLE_NONE;
	m_d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow					= NULL;
    m_d3dpp.Windowed						= FALSE;
    m_d3dpp.EnableAutoDepthStencil			= TRUE;

	// Find valid format for AutoDepthStencilFormat
	for(i = 0; i < numFormats; i++) {

        if(m_pD3D->CheckDepthStencilMatch(m_Adapter, D3DDEVTYPE_HAL, m_DisplayMode.Format,
											m_DisplayMode.Format, Formats[i]) == D3D_OK) {
			m_d3dpp.AutoDepthStencilFormat = Formats[i];
			break;
		}
	}

	m_d3dpp.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
    m_d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
}

//-----------------------------------------------------------------------------
// Name: AVPackDescription()
// Desc: Populate m_szAvPack with AV Pack and Mode info
//-----------------------------------------------------------------------------
VOID CXBD3DDevice::AVPackDescription() {
	// wsprintfW(m_szAvPackLine1, L"AV Pack(%i,%i,%#x) ", m_AvPack, m_VideoStandard, m_VideoFlags);
	wsprintfW(m_szAvPackLine1, L"AV Pack ");
	// AV Pack
	switch(m_AvPack) {
		case XC_AV_PACK_SCART:
			lstrcatW(m_szAvPackLine1, L"SCART ");
			break;

		case XC_AV_PACK_HDTV:
			lstrcatW(m_szAvPackLine1, L"HDTV ");
			break;

		case XC_AV_PACK_VGA:
			lstrcatW(m_szAvPackLine1, L"VGA ");
			break;

		case XC_AV_PACK_RFU:
			lstrcatW(m_szAvPackLine1, L"RFU ");
			break;

		case XC_AV_PACK_SVIDEO:
			lstrcatW(m_szAvPackLine1, L"SVIDEO ");
			break;

		case XC_AV_PACK_STANDARD:
			lstrcatW(m_szAvPackLine1, L"STANDARD ");
			break;

		case 7:
			lstrcatW(m_szAvPackLine1, L"NONE ");
			break;

		default:
			lstrcatW(m_szAvPackLine1, L"UNKNOWN ");
			break;
	}

	// Video Standard
	switch(m_VideoStandard) {
		case XC_VIDEO_STANDARD_NTSC_M:
			lstrcatW(m_szAvPackLine1, L"NTSC_M ");
			break;

		case XC_VIDEO_STANDARD_NTSC_J:
			lstrcatW(m_szAvPackLine1, L"NTSC_J ");
			break;

		case XC_VIDEO_STANDARD_PAL_I:
			lstrcatW(m_szAvPackLine1, L"PAL_I ");
			break;

		default:
			lstrcatW(m_szAvPackLine1, L"UNKNOWN ");
			break;
	}

	// Video Flags
	/*
	if((m_VideoFlags & XC_VIDEO_FLAGS_WIDESCREEN) == XC_VIDEO_FLAGS_WIDESCREEN) {
		lstrcatW(m_szAvPackLine1, L"WIDESCREEN ");
	}
		

	if((m_VideoFlags & XC_VIDEO_FLAGS_HDTV_720p) == XC_VIDEO_FLAGS_HDTV_720p) {
		lstrcatW(m_szAvPackLine1, L"HDTV 720p ");
	}

	if((m_VideoFlags & XC_VIDEO_FLAGS_HDTV_1080i) == XC_VIDEO_FLAGS_HDTV_1080i) {
		lstrcatW(m_szAvPackLine1, L"HDTV 1080i ");
	}

	if((m_VideoFlags & XC_VIDEO_FLAGS_LETTERBOX) == XC_VIDEO_FLAGS_LETTERBOX) {
		lstrcatW(m_szAvPackLine1, L"LETTERBOX ");
	}

	if((m_VideoFlags & XC_VIDEO_FLAGS_PAL_60Hz) == XC_VIDEO_FLAGS_PAL_60Hz) {
		lstrcatW(m_szAvPackLine1, L"PAL 60Hz ");
	}
	*/

	// Dimension
	wsprintfW(m_szAvPackLine2, L"%i x %i ", m_DisplayMode.Width, m_DisplayMode.Height);

	// Interlaced or Progressive
	if((m_DisplayMode.Flags & D3DPRESENTFLAG_INTERLACED) == D3DPRESENTFLAG_INTERLACED) {
		lstrcatW(m_szAvPackLine2, L"INTERLACED ");
	}

	if((m_DisplayMode.Flags & D3DPRESENTFLAG_PROGRESSIVE) == D3DPRESENTFLAG_PROGRESSIVE) {
		lstrcatW(m_szAvPackLine2, L"PROGRESSIVE ");
	}

	// Widescreen
	if((m_DisplayMode.Flags & D3DPRESENTFLAG_WIDESCREEN) == D3DPRESENTFLAG_WIDESCREEN) {
		lstrcatW(m_szAvPackLine2, L"WIDESCREEN ");
	}

	// Closed Captioning
	wsprintfW(m_szAvPackLine3, L"Closed Captioning %s", m_ClosedCaptioning ? L"ON" : L"OFF");

	OutputDebugStringW(m_szAvPackLine1);
	OutputDebugStringA("\n");
	OutputDebugStringW(m_szAvPackLine2);
	OutputDebugStringA("\n");
	OutputDebugStringW(m_szAvPackLine3);
	OutputDebugStringA("\n");
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
HRESULT CXBD3DDevice::Render() {
    
    m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L);

	// Display graphic
	if(m_DisplayButtonMap) {
		ButtonMap();
	} else {
		SmpteBars();

		// Write AV Pack info to screen
		m_pd3dDevice->BeginScene();
		m_Font.DrawText((float)m_DisplayMode.Width*2/10 + (float)m_Font.m_dwFontHeight,
						(float)m_DisplayMode.Height*2/10 + (float)m_Font.m_dwFontHeight,
						0xffffffff, m_szAvPackLine1);
		m_Font.DrawText((float)m_DisplayMode.Width*2/10 + (float)m_Font.m_dwFontHeight,
						(float)m_DisplayMode.Height*2/10 + (float)m_Font.m_dwFontHeight*2,
						0xffffffff, m_szAvPackLine2);
		m_Font.DrawText((float)m_DisplayMode.Width*2/10 + (float)m_Font.m_dwFontHeight,
						(float)m_DisplayMode.Height*2/10 + (float)m_Font.m_dwFontHeight*3,
						0xffffffff, m_szAvPackLine3);
		m_pd3dDevice->EndScene();
	}
    
	// Finally, show the frame (swaps the backbuffer to the front)
	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: IREtoDW()
// Desc: Helper function for color value converstions
//-----------------------------------------------------------------------------
DWORD CXBD3DDevice::IREtoDW (double dfIre) {
	return ((DWORD)(dfIre*255.0/100.0));
}


//-----------------------------------------------------------------------------
// Name: SmpteBars()
// Desc: Displays SMPTE color bars, plus concentric rectangles, center target
//-----------------------------------------------------------------------------
BOOL CXBD3DDevice::SmpteBars(void) {
    D3DLOCKED_RECT  dr;
	D3DRECT			Rect;
    LPDWORD         pData;
    DWORD           dwR, dwG, dwB;
    D3DSURFACE_DESC	SurfDesc;
	DWORD			x, y, z;
	DWORD			WINDOW_WIDTH, WINDOW_HEIGHT, PitchAdjust;
	DWORD			Black, White, Width, Height;
	WCHAR			szwText[64];


    m_pBackBuffer->GetDesc(&SurfDesc);

	WINDOW_WIDTH = SurfDesc.Width;
	WINDOW_HEIGHT = SurfDesc.Height;

    ZeroMemory(&dr, sizeof(dr));
    m_pBackBuffer->LockRect(&dr, NULL, D3DLOCK_TILED) ;
	PitchAdjust = dr.Pitch/4 - WINDOW_WIDTH;
    pData = (LPDWORD)dr.pBits;

    ZeroMemory(pData, WINDOW_WIDTH * WINDOW_HEIGHT * 4);

    for (y = 0; y < WINDOW_HEIGHT; y++)
	{
		if (y < WINDOW_HEIGHT*3/4)
		{
			for (x = 0; x < 1*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(75.0);
				dwG = IREtoDW(75.0);
				dwB = IREtoDW(75.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 2*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(75.0);
				dwG = IREtoDW(75.0);
				dwB = IREtoDW(0.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 3*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(75.0);
				dwB = IREtoDW(75.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 4*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(75.0);
				dwB = IREtoDW(0.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 5*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(75.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(75.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 6*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(75.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(0.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 7*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(75.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 8*WINDOW_WIDTH/8; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(0.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
		}
		else if (y < WINDOW_HEIGHT)
		{
			for (x = 0; x < 1*WINDOW_WIDTH/6; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(10.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 2*WINDOW_WIDTH/6; x++)
			{
				dwR = IREtoDW(100.0);
				dwG = IREtoDW(100.0);
				dwB = IREtoDW(100.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 3*WINDOW_WIDTH/6; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(10.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 4*WINDOW_WIDTH/6; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(0.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 5*WINDOW_WIDTH/6; x++)
			{
				dwR = IREtoDW(4.2);
				dwG = IREtoDW(4.2);
				dwB = IREtoDW(4.2);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
			for (; x < 6*WINDOW_WIDTH/6; x++)
			{
				dwR = IREtoDW(0.0);
				dwG = IREtoDW(0.0);
				dwB = IREtoDW(0.0);
				*pData++ = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
			}
		}

		pData += PitchAdjust;
	}

	m_pBackBuffer->UnlockRect(); 
	
	// Center squares
	dwR = IREtoDW(0.0);
	dwG = IREtoDW(0.0);
	dwB = IREtoDW(0.0);
	Black = D3DCOLOR_ARGB(0, dwR, dwG, dwB);

	dwR = IREtoDW(100.0);
	dwG = IREtoDW(100.0);
	dwB = IREtoDW(100.0);
	White = D3DCOLOR_ARGB(0, dwR, dwG, dwB);
	
	Rect.x1 = WINDOW_WIDTH/2 - 30;
	Rect.y1 = WINDOW_HEIGHT/2 - 30;
	Rect.x2 = WINDOW_WIDTH/2 + 30;
	Rect.y2 = WINDOW_HEIGHT/2 + 30;
	
	ZeroMemory(&dr, sizeof(dr));
	m_pBackBuffer->LockRect(&dr, (const RECT *)&Rect, D3DLOCK_TILED) ;
	pData = (LPDWORD)dr.pBits;
	PitchAdjust = dr.Pitch/4 - 60;

	for(y = 0; y < 30; y++) {
		for(x = 0; x < 30; x++) {
			*pData++ = Black;
		}

		for(x = 30 ; x < 60; x++) {
			*pData++ = White;
		}

		pData += PitchAdjust;
	}

	for(y = 30; y < 60; y++) {
		for(x = 0; x < 30; x++) {
			*pData++ = White;
		}

		for(x = 30 ; x < 60; x++) {
			*pData++ = Black;
		}

		pData += PitchAdjust;
	}

	m_pBackBuffer->UnlockRect(); 

	// Concentric Rectangles
	for(z = 0; z <= 20; z += 5) {
		Width = WINDOW_WIDTH*(100 - z)/100;
		Height = WINDOW_HEIGHT*(100 - z)/100;

		Rect.x1 = (WINDOW_WIDTH - Width)/2;
		Rect.y1 = (WINDOW_HEIGHT - Height)/2;
		Rect.x2 = WINDOW_WIDTH - Rect.x1;
		Rect.y2 = WINDOW_HEIGHT - Rect.y1;
		
		ZeroMemory(&dr, sizeof(dr));
		m_pBackBuffer->LockRect(&dr, (const RECT *)&Rect, D3DLOCK_TILED) ;
		pData = (LPDWORD)dr.pBits;
		PitchAdjust = dr.Pitch/4 - Width;

		// Top two lines
		for(y = 0; y < 2; y++) {
			for(x = 0; x < Width; x++) {
				if(*pData == White) {
					*pData++ = Black;
				} else {
					*pData++ = White;
				}
			}
			pData += PitchAdjust;
		}

		// Sides
		for(y = 2; y < Height - 2; y++) {
			for(x = 0; x < Width; x++) {
				if((x < 2) || (x > Width - 3)) {
					if(*pData == White) {
						*pData = Black;
					} else {
						*pData = White;
					}
				}
				pData++;
			}
			pData += PitchAdjust;
		}

		// Bottom two lines
		for(y = Height - 2; y < Height; y++) {
			for(x = 0; x < Width; x++) {
				if(*pData == White) {
					*pData++ = Black;
				} else {
					*pData++ = White;
				}
			}
			pData += PitchAdjust;
		}

		m_pBackBuffer->UnlockRect(); 

		// Display %
		wsprintfW(szwText, L"%i%%", z);
		m_Font.DrawText((FLOAT)Rect.x1, (FLOAT)Rect.y2 - (float)m_Font.m_dwFontHeight - (FLOAT)z*4, 0xffffffff, szwText);
	}
    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: ButtonMap()
// Desc: Displays controller button mapping
//-----------------------------------------------------------------------------
BOOL CXBD3DDevice::ButtonMap(void) {
	FLOAT x, y;
	WCHAR			szwText[64];

	// Starting position of text (20% corner)
	x = (float)m_DisplayMode.Width*2/10;
	y = (float)m_DisplayMode.Height*2/10;

	m_pd3dDevice->BeginScene();
	wsprintfW(szwText, L"Controller Button Map");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"Start = Toggle Widescreen");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"Back = Toggle Closed Captioning");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"Black = Toggle Display Button Map");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"A = HDTV Only: 640 x 480p");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"B = HDTV Only: 720 x 480p");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"X = HDTV Only: 640 x 480i");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"Y = HDTV Only: 1920 x 1080i");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	wsprintfW(szwText, L"Both Triggers + Black = Reboot");
	m_Font.DrawText(x, y, 0xffffffff, szwText);
	y += m_Font.m_dwFontHeight;

	m_pd3dDevice->EndScene();
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: main routine of application
//-----------------------------------------------------------------------------
void __cdecl main() {
	HRESULT hr;
	XBGAMEPAD *pGamePad, DefaultGamePad;
	DWORD i, b;
	CXBD3DDevice xbApp;

	// Init game pads
	XInitDevices(0, NULL);

	// Create the gamepad devices
    OutputDebugStringA("XBApp: Creating gamepad devices...\n");
    if(FAILED(hr = XBInput_CreateGamepads(&pGamePad))) {
        OutputDebugStringA("XBApp: Call to CreateGamepads() FAILED!\n");
        Sleep(INFINITE);
    }

	if(FAILED(xbApp.Init())) {
        OutputDebugStringA("XBApp: Init() FAILED!\n");
        Sleep(INFINITE);
    }

	if(FAILED(xbApp.Create())) {
        OutputDebugStringA("XBApp: Create() FAILED!\n");
        Sleep(INFINITE);
    }

	if(FAILED(xbApp.Render())) {
        OutputDebugStringA("XBApp: Render() FAILED!\n");
        Sleep(INFINITE);
    }

	while(TRUE) {
		// Get Input
		XBInput_GetInput(pGamePad);

		// Combine input from all attached controllers
		 ZeroMemory(&DefaultGamePad, sizeof(DefaultGamePad));
        for(i = 0; i < 4; i++) {
            if(pGamePad[i].hDevice) {
				DefaultGamePad.sThumbLX = DefaultGamePad.sThumbLX + pGamePad[i].sThumbLX;
                DefaultGamePad.sThumbLY = DefaultGamePad.sThumbLY + pGamePad[i].sThumbLY;
                DefaultGamePad.sThumbRX = DefaultGamePad.sThumbRX + pGamePad[i].sThumbRX;
                DefaultGamePad.sThumbRY = DefaultGamePad.sThumbRY + pGamePad[i].sThumbRY;
                DefaultGamePad.fX1      += pGamePad[i].fX1;
                DefaultGamePad.fY1      += pGamePad[i].fY1;
                DefaultGamePad.fX2      += pGamePad[i].fX2;
                DefaultGamePad.fY2      += pGamePad[i].fY2;
                DefaultGamePad.wButtons        |= pGamePad[i].wButtons;
                DefaultGamePad.wPressedButtons |= pGamePad[i].wPressedButtons;
                DefaultGamePad.wLastButtons    |= pGamePad[i].wLastButtons;

                for(b =0 ; b < 8; b++) {
                    DefaultGamePad.bAnalogButtons[b]        |= pGamePad[i].bAnalogButtons[b];
                    DefaultGamePad.bPressedAnalogButtons[b] |= pGamePad[i].bPressedAnalogButtons[b];
                    DefaultGamePad.bLastAnalogButtons[b]    |= pGamePad[i].bLastAnalogButtons[b];
                }
            }
        }

		// Reboot
		if(DefaultGamePad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0) {
            if(DefaultGamePad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0) {
                if(DefaultGamePad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]) {
                    XLaunchNewImage( NULL, NULL );
				}
			}
		}

		// Check for Widescreen toggle (only for 640 wide)
		if(xbApp.m_DisplayWidth	== 640) {
			if((DefaultGamePad.wPressedButtons & XINPUT_GAMEPAD_START) == XINPUT_GAMEPAD_START) {
				xbApp.m_Widescreen = !(xbApp.m_Widescreen);
				xbApp.Reset();
			}
		}

		// Check for Closed Captioning toggle
		if((DefaultGamePad.wPressedButtons & XINPUT_GAMEPAD_BACK) == XINPUT_GAMEPAD_BACK) {
			xbApp.m_ClosedCaptioning = !(xbApp.m_ClosedCaptioning);
			xbApp.Reset();
		}


		// Check for button mapping display toggle
		if(DefaultGamePad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]) {
		// if((DefaultGamePad.wPressedButtons & XINPUT_GAMEPAD_BLACK) == XINPUT_GAMEPAD_BLACK) {
			xbApp.m_DisplayButtonMap = !(xbApp.m_DisplayButtonMap);
		}
	
		// Check for mode changes
		if(xbApp.m_AvPack == XC_AV_PACK_HDTV) {
			// If HDTV then check for mode changes
			if(DefaultGamePad.bPressedAnalogButtons[XINPUT_GAMEPAD_A]) {
				// A Button = 640x480p
				xbApp.m_DisplayWidth	= 640;
				xbApp.m_DisplayHeight	= 480;
				xbApp.m_Interlaced	= FALSE;
				xbApp.Reset();
			}

			if(DefaultGamePad.bPressedAnalogButtons[XINPUT_GAMEPAD_B]) {
				// B Button = 720x480p
				xbApp.m_DisplayWidth	= 720;
				xbApp.m_DisplayHeight	= 480;
				xbApp.m_Interlaced	= FALSE;
				xbApp.m_Widescreen	= TRUE;
				xbApp.Reset();
			}

			if(DefaultGamePad.bPressedAnalogButtons[XINPUT_GAMEPAD_X]) {
				// X Button = 640x480i
				xbApp.m_DisplayWidth	= 640;
				xbApp.m_DisplayHeight	= 480;
				xbApp.m_Interlaced	= TRUE;
				xbApp.Reset();
			}

			if(DefaultGamePad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y]) {
				// Y Button = 1920x1080i
				xbApp.m_DisplayWidth	= 1920;
				xbApp.m_DisplayHeight	= 1080;
				xbApp.m_Interlaced	= TRUE;
				xbApp.m_Widescreen	= TRUE;
				xbApp.Reset();
			}
		}

		if(FAILED(xbApp.Render())) {
			OutputDebugStringA("XBApp: Render() FAILED!\n");
			Sleep(INFINITE);
		}
	}
}
