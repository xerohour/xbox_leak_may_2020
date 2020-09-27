//-----------------------------------------------------------------------------
// File: D3DApp.cpp
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define INITGUID

#include <stdio.h>
#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

#if defined(PCBUILD)
#include <d3d8.h>
#include <d3dx8core.h>
#include <dinput.h>
#endif 

#include "D3DApp.h"
#include "DXUtil.h"

#ifndef XBOX

#define INITGUID
#include <objbase.h>
#include <rmxfguid.h>
#endif

// Define PCBUILD if building to run on a PC - we need to create a window.

static ScenarioMenu s_MainMenu = 
{
	MT_STARTBUTTON1,
	L"Main Menu",
	{
		L"Display statistics",
		L"Run a 20-second test",
		L"Pause",
		L"Step",
		L"Quit",
		NULL
	},
	5
};

static ScenarioMenu s_ToggleMenu = 
{
	MT_STARTBUTTON2,
	L"Main Toggle Menu",
	{
		L"Cycle screen mode",
		L"Cycle antialias mode",
		NULL,
		NULL,
		NULL,
		NULL
	},
	2
};

static const WCHAR *s_rgszMenuButtons[] =
{
	L"Left top trigger",
	L"Right top trigger",
	L"Left underside trigger",
	L"Right underside trigger",
	L"Top start button",
	L"Bottom start button",
};

//-----------------------------------------------------------------------------
// Class constructor.
//-----------------------------------------------------------------------------

CD3DApplication::CD3DApplication()
{
    m_pD3D              = NULL;
    m_pDevice           = NULL;
    m_hWnd              = NULL;
    m_bActive           = FALSE;
    m_bReady            = FALSE;

#if defined(PCBUILD)
	m_pDirectInput		= NULL;
	m_pKeyboard			= NULL;
	m_pGamepad          = NULL;
#else
    m_hDevice           = 0;
#endif 

	m_fShowHelp         = FALSE;
	m_fShowStats		= TRUE;
	m_iActiveMenu		= MT_NONE;
	m_iActiveKey		= -1;

	ZeroMemory(m_rgpMenus, sizeof(m_rgpMenus));

	m_cMenus			= 0;
	m_dwSpaceHeight		= 0;
	m_dwHelpWidth		= 0;	
	m_dwColumn1X		= 0;	
	m_dwColumn2X		= 0;

    m_bFrameMoving      = TRUE;
    m_bSingleStep       = FALSE;
	m_bRunFullTest      = FALSE;

    m_fFPS              = 0.0f;
    m_strFrameStats[0]  = 0;
	m_szTestStats[0]    = 0;

    m_bUseDepthBuffer   = FALSE;
    m_DepthBufferFormat = D3DFMT_D16;
	m_dwCurrentMode     = 0;

	m_dwSampleMask		= 0;
	m_iSampleType		= (D3DMULTISAMPLE_TYPE)0;

	m_dwFrames			= 0;
}

#if PCBUILD

//-----------------------------------------------------------------------------
// Dummy message handler for the window that the PC requires us to create.
//-----------------------------------------------------------------------------

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Create the dummy window.
//-----------------------------------------------------------------------------

static HWND GetDummyWindow(HINSTANCE hInstance)
{
	HWND hWnd;

    // Register the windows class
    WNDCLASS wndClass = { 
						  CS_CLASSDC, 
		                  WndProc, 
						  0, 
						  0, 
						  hInstance,
                          NULL,
                          NULL, 
                          (HBRUSH)GetStockObject(WHITE_BRUSH),
                          NULL, 
						  L"Dummy Window" 
						};

    RegisterClass( &wndClass );

    // Create the render window
    hWnd = CreateWindow(L"Dummy Window", 
		                NULL,
                        WS_MINIMIZE,
                        0, 
						0,
                        1,
						1, 
						0,
                        NULL, 
                        hInstance, 
						0L);

    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowCursor(false);

	return hWnd;
}

#else !PCBUILD

//-----------------------------------------------------------------------------
// Stub so we won't try to create a window on the non-pc build.
//-----------------------------------------------------------------------------

inline HWND GetDummyWindow(HINSTANCE) { return NULL; }

#endif !PCBUILD

//-----------------------------------------------------------------------------
// Do the initial preparations for running the scenario.
//-----------------------------------------------------------------------------

void CD3DApplication::Create(HINSTANCE hInstance)
{

#if defined(XBOX)

#endif

    // Create the Direct3D object
    m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    if( m_pD3D == NULL )
    {
		DisplayError(L"FATAL: Unable to create Direct3D.", 0);
    }

    // Build a list of Direct3D adapters, modes and devices. The ConfirmDevice() 
	// callback is used to confirm that only devices that meet the app's 
	// requirements are considered.
	//
    BuildDeviceList();

#if defined(PCBUILD)

	// Create the DirectInput object.
	CheckHR(DirectInput8Create(hInstance, 
		                       DIRECTINPUT_VERSION, 
							   IID_IDirectInput8, 
							   (void **)&m_pDirectInput, 
							   NULL));

	// Hook up to the keyboard and the gamepad if they're available.
	GetInputDevices();

#else
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    if(XDEVICE_PORT0_MASK&XGetDevices(XDEVICE_TYPE_GAMEPAD))
    {
        m_hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
    }

#endif

    // Get the windows class.
	m_hWnd = GetDummyWindow(hInstance);

    // Initialize the app's custom scene stuff
    OneTimeSceneInit();

    // Initialize the 3D environment for the app
    Initialize3DEnvironment();

	// Prepare the help information.
	InitializeHelpInformation();

    // Setup the app so it can support single-stepping
	DXUtil_Timer::Reset();

    // The app is ready to go
    m_bReady = TRUE;
}

//-----------------------------------------------------------------------------
// Callback function for sorting display modes.
//-----------------------------------------------------------------------------

int SortModesCallback( const VOID* arg1, const VOID* arg2 )
{
    D3DDISPLAYMODE* p1 = (D3DDISPLAYMODE*)arg1;
    D3DDISPLAYMODE* p2 = (D3DDISPLAYMODE*)arg2;

    if( p1->Format > p2->Format )   return -1;
    if( p1->Format < p2->Format )   return +1;
    if( p1->Width  < p2->Width )    return -1;
    if( p1->Width  > p2->Width )    return +1;
    if( p1->Height < p2->Height )   return -1;
    if( p1->Height > p2->Height )   return +1;

    return 0;
}

//-----------------------------------------------------------------------------
// Gather information about the adapter and what modes it supports.  This
// method assumes that we only have one adapter and that is supports the
// modes an capacities that we need.
//
// The method returns 'true' if it succeeds.
//-----------------------------------------------------------------------------

void CD3DApplication::BuildDeviceList()
{	
	// We only support one adapter.
	const unsigned iAdapter = 0;

	if (m_pD3D->GetAdapterCount() != 1)
	{
		DisplayError(L"FATAL: Multiple adapters found, only one adapter is supported.", 0);
	}

    // Get the adapter attributes
    CheckHR(m_pD3D->GetAdapterIdentifier(iAdapter, 0L, &m_Adapter.AdapterIdentifier));

    // Enumerate display modes
    D3DDISPLAYMODE modes[100];
    D3DFORMAT      formats[20];
    DWORD dwNumFormats      = 0;
    DWORD dwNumModes        = 0;
    DWORD dwNumAdapterModes = m_pD3D->GetAdapterModeCount(iAdapter);

    // Add the current desktop format to list of formats
    D3DDISPLAYMODE DesktopMode;
    CheckHR(m_pD3D->GetAdapterDisplayMode(iAdapter, &DesktopMode));
    if (DesktopMode.Format != 0)
    {
        formats[dwNumFormats++] = DesktopMode.Format;
    }

    for (UINT iMode = 0; iMode < dwNumAdapterModes; iMode++)
    {
        // Get the display mode attributes
        D3DDISPLAYMODE DisplayMode;

        CheckHR(m_pD3D->EnumAdapterModes(iAdapter, iMode, &DisplayMode));

        // Filter out unknown modes
        if (DisplayMode.Format == D3DFMT_UNKNOWN)
		{
            continue;
		}

        // Filter out low-resolution modes
        if (DisplayMode.Width  < 640 || DisplayMode.Height < 400) 
		{
            continue;
		}

        // Check if the mode already exists (to filter out refresh rates)
        for (DWORD m = 0; m < dwNumModes; m++)
        {
            if ((modes[m].Width  == DisplayMode.Width ) &&
                (modes[m].Height == DisplayMode.Height) &&
                (modes[m].Format == DisplayMode.Format)) 
			{
                break;
			}
        }

        // If we found a new mode, add it to the list
        if (m == dwNumModes)
        {
            modes[dwNumModes].Width       = DisplayMode.Width;
            modes[dwNumModes].Height      = DisplayMode.Height;
            modes[dwNumModes].Format      = DisplayMode.Format;
            modes[dwNumModes].RefreshRate = 0;
            dwNumModes++;

            // Check if the mode's format already exists
            for (DWORD f = 0; f < dwNumFormats; f++)
            {
                if (DisplayMode.Format == formats[f])
				{
                    break;
				}
            }

            // If the format is new, add it to the list
            if (f == dwNumFormats)
			{
                formats[dwNumFormats++] = DisplayMode.Format;
			}
        }
    }

    // Sort the list of display modes (by format, then width, then height)

    // Get the device attributes
    CheckHR(m_pD3D->GetDeviceCaps(iAdapter, D3DDEVTYPE_HAL, &m_Adapter.Caps));

    // Call the app's ConfirmDevice() callback to see if the device
    // caps and the enumerated formats meet the app's requirements.
	//
    BOOL  bFormatConfirmed[20];
    DWORD dwBehavior[20];

    for (DWORD f = 0; f < dwNumFormats; f++)
    {
		HRESULT hr;

        bFormatConfirmed[f] = FALSE;

        hr = m_pD3D->CheckDeviceFormat(iAdapter, 
			                           D3DDEVTYPE_HAL, 
                                       formats[f], 
									   D3DUSAGE_RENDERTARGET, 
                                       D3DRTYPE_SURFACE, 
									   formats[f]);

		if (FAILED(hr))
		{
            continue;
		}

        // Confirm the device for HW vertex processing
        if (m_Adapter.Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

            if (ConfirmDevice(&m_Adapter.Caps, dwBehavior[f], formats[f]))
			{
                bFormatConfirmed[f] = TRUE;
			}
        }

        // Confirm the device for SW vertex processing
        if (FALSE == bFormatConfirmed[f])
        {
            dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

            if (ConfirmDevice(&m_Adapter.Caps, dwBehavior[f], formats[f]))
			{
                bFormatConfirmed[f] = TRUE;
			}
        }
    }

    // Add all enumerated display modes with confirmed formats to the
    // device's list of valid modes
	//
    for (DWORD m = 0; m < dwNumModes; m++)
    {
        for (DWORD f = 0; f < dwNumFormats; f++)
        {
            if (modes[m].Format == formats[f])
            {
                if (bFormatConfirmed[f] == TRUE)
                {
                    // Add this mode to the device's list of valid modes
                    m_Adapter.modes[m_Adapter.dwNumModes].Width      = modes[m].Width;
                    m_Adapter.modes[m_Adapter.dwNumModes].Height     = modes[m].Height;
                    m_Adapter.modes[m_Adapter.dwNumModes].Format     = modes[m].Format;
                    m_Adapter.modes[m_Adapter.dwNumModes].dwBehavior = dwBehavior[f];
                    m_Adapter.dwNumModes++;
                }
            }
        }
    }

    // Select 640x480, 16-bit mode for default
    for (m = 0; m < m_Adapter.dwNumModes; m++)
    {
        if (m_Adapter.modes[m].Width==640 && 
			m_Adapter.modes[m].Height==480 
#ifdef XBOX
			 && m_Adapter.modes[m].Format == D3DFMT_LIN_R5G6B5
#endif
            )
        {
            m_dwCurrentMode = m;
            break;
        }
    }

	// Generate an error if we didn't find the default mode.
	if (m == m_Adapter.dwNumModes)
	{
		DisplayError(L"FATAL: Unable to obtain 640x480x16 mode.", 0);
	}
}

#if defined(PCBUILD)

//-----------------------------------------------------------------------------
// Callback to enumerate the devices.
//-----------------------------------------------------------------------------

BOOL _stdcall CALLBACK InputEnumCallback(const DIDEVICEINSTANCE *pdi, void *pvRef) 
{ 
	DIDEVICEINSTANCE *rgdi = (DIDEVICEINSTANCE *)pvRef;

	if (GET_DIDEVICE_TYPE(pdi->dwDevType) == DI8DEVTYPE_KEYBOARD)
	{
		// Only 'see' the first one.
		if (!rgdi[0].dwDevType)
		{
			rgdi[0] = *pdi;
		}
	}

	else if (GET_DIDEVICE_TYPE(pdi->dwDevType) == DI8DEVTYPE_JOYSTICK)
	{
		// Only 'see' the first one.
		if (!rgdi[1].dwDevType)
		{
			rgdi[1] = *pdi;
		}
	}

	if (rgdi[0].dwDevType && rgdi[1].dwDevType)
	{
		return DIENUM_STOP; 
	}
	else
	{
		return DIENUM_CONTINUE; 
	}
}

#endif

//-----------------------------------------------------------------------------
// Hook up to the keyboard and the gamepad if they're available.  If there
// aren't any devices then the scenario cannot be controlled and will 
// run forever.
//-----------------------------------------------------------------------------

#if defined(PCBUILD)

void CD3DApplication::GetInputDevices()
{

	DIDEVICEINSTANCE rgdi[2];

	// Zero the array we're going to fill in the enumeration.
	ZeroMemory(rgdi, sizeof(rgdi));

	// Enumerate all connected devices.
	CheckHR(m_pDirectInput->EnumDevices(DI8DEVCLASS_ALL, 
					                    InputEnumCallback, 
										(void *)&rgdi, 
										DIEDFL_ATTACHEDONLY));	

	// Initialize the keyboard device, if installed.
	if (rgdi[0].dwDevType)
	{
		CheckHR(m_pDirectInput->CreateDevice(rgdi[0].guidInstance, &m_pKeyboard, NULL));

		CheckHR(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard))
		CheckHR(m_pKeyboard->Acquire());
	}

	// Initialize the gamepad device, if installed.
	if (rgdi[1].dwDevType)
	{
		CheckHR(m_pDirectInput->CreateDevice(rgdi[1].guidInstance, &m_pGamepad, NULL));

		// What should I set the format to?
		CheckHR(m_pGamepad->SetDataFormat(&c_dfDIJoystick ))
		CheckHR(m_pGamepad->Acquire());
	}

}

#endif

//-----------------------------------------------------------------------------
// Create the rendering device and set up the back buffers.
//-----------------------------------------------------------------------------

void CD3DApplication::Initialize3DEnvironment()
{
    Adapter::Mode *pModeInfo = &m_Adapter.modes[m_dwCurrentMode];

    // Set up the presentation parameters
    ZeroMemory(&m_Presentation, sizeof(m_Presentation));

    switch(pModeInfo->Format)
    {
#ifdef XBOX
    case D3DFMT_LIN_A8R8G8B8:
    case D3DFMT_LIN_X8R8G8B8:
#endif
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        m_DepthBufferFormat = D3DFMT_D24S8;
        break;

#ifdef XBOX
    case D3DFMT_LIN_R5G6B5:
    case D3DFMT_LIN_X1R5G5B5:
#endif
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
        m_DepthBufferFormat = D3DFMT_D16;
        break;
    }

    m_Presentation.Windowed               = FALSE;
    m_Presentation.BackBufferCount        = 2;
    m_Presentation.MultiSampleType        = m_iSampleType;
    m_Presentation.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    m_Presentation.EnableAutoDepthStencil = m_bUseDepthBuffer;
    m_Presentation.AutoDepthStencilFormat = m_DepthBufferFormat;
    m_Presentation.hDeviceWindow          = m_hWnd;
    m_Presentation.BackBufferWidth        = pModeInfo->Width;
    m_Presentation.BackBufferHeight       = pModeInfo->Height;
    m_Presentation.BackBufferFormat       = pModeInfo->Format;
    m_Presentation.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    m_Presentation.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the device
    CheckHR(m_pD3D->CreateDevice(0, 
		                     D3DDEVTYPE_HAL, 
                             m_hWnd, 
						     pModeInfo->dwBehavior, 
							 &m_Presentation, 
                             &m_pDevice));

    // Store device Caps
    CheckHR(m_pDevice->GetDeviceCaps(&m_Caps));

	// Figure out the FSAA modes that are supported by the device.
	m_dwSampleMask = 0;
	m_iSampleType = (D3DMULTISAMPLE_TYPE)0;

	for (unsigned i = 2; i < 32; i++)
	{
		HRESULT hr = m_pD3D->CheckDeviceMultiSampleType(0, 
			                                            D3DDEVTYPE_HAL, 
												        pModeInfo->Format, 
														FALSE, 
														(D3DMULTISAMPLE_TYPE)i);

		if (SUCCEEDED(hr))
		{
            m_dwSampleMask |= 1 << i;
		}
	}

    // Store render target surface descriptor

    CheckHR(m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer));

    m_pBackBuffer->GetDesc(&m_BackBufferDesc);

    // Initialize the app's device-dependant objects
	InitDeviceObjects();
    RestoreDeviceObjects();

	m_bActive = TRUE;
}

//-----------------------------------------------------------------------------
// Helper to draw text on the screen.
//-----------------------------------------------------------------------------
void CD3DApplication::DisplayText(const WCHAR *sz, int iY, DWORD dwColor)
{
    m_Font.DrawText(m_pBackBuffer, sz, 0, iY, DRAWTEXT_TRANSPARENTBKGND, dwColor, 0);
}

//-----------------------------------------------------------------------------
// Handles driver, device, and/or mode changes for the app.
//-----------------------------------------------------------------------------

void CD3DApplication::Change3DEnvironment()
{
    // Release all scene objects that will be re-created for the new device
	InvalidateDeviceObjects();
    DeleteDeviceObjects();

    if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}

    // Release display objects, so a new device can be created
	//
	// UNDONE andrewso: Make sure this is a safe assumption.
	//
    if (m_pDevice->Release() > 0)
    {
		DisplayError(L"FATAL: Device still has an outstanding reference.", 0);
    }

	// Just to be safe.
	m_pDevice = NULL;

    // Inform the display class of the driver change. It will internally
    // re-create valid surfaces, a d3ddevice, etc.
	//
    Initialize3DEnvironment();

    // If the app is paused, trigger the rendering of the current frame
    if (FALSE == m_bFrameMoving)
    {
        m_bSingleStep = TRUE;

		DXUtil_Timer::Start();
		DXUtil_Timer::Stop();
    }
}

//-----------------------------------------------------------------------------
// Handle regaining the device.
//-----------------------------------------------------------------------------

void CD3DApplication::Resize3DEnvironment()
{
    // Release all vidmem objects
	InvalidateDeviceObjects();

    // Resize the device
    CheckHR(m_pDevice->Reset(&m_Presentation));

    // Store render target surface desc
    LPDIRECT3DSURFACE8 pBackBuffer;

    CheckHR(m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer));

    pBackBuffer->GetDesc(&m_BackBufferDesc);
    pBackBuffer->Release();

    // Initialize the app's device-dependant objects
    RestoreDeviceObjects();

    // If the app is paused, trigger the rendering of the current frame
    if (FALSE == m_bFrameMoving)
    {
        m_bSingleStep = TRUE;

		DXUtil_Timer::Start();
		DXUtil_Timer::Stop();
    }
}

//-----------------------------------------------------------------------------
// Cleanup scene objects
//-----------------------------------------------------------------------------

void CD3DApplication::Cleanup3DEnvironment()
{
    m_bActive = FALSE;
    m_bReady  = FALSE;

    if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}

    if (m_pDevice)
    {
        InvalidateDeviceObjects();
        DeleteDeviceObjects();

        m_pDevice->Release();
        m_pD3D->Release();
        
        m_pDevice = NULL;
        m_pD3D = NULL;
    }

#if defined(PCBUILD)
	if (m_pKeyboard)
	{
		m_pKeyboard->Release();
		m_pKeyboard = NULL;
	}

	if (m_pGamepad)
	{
		m_pGamepad->Release();
		m_pGamepad = NULL;
	}

	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
		m_pDirectInput = NULL;
	}
#endif

    FinalCleanup();
}

//-----------------------------------------------------------------------------
// Run the scenario.
//-----------------------------------------------------------------------------

void CD3DApplication::Run()
{
	do
	{
		// Run a 'test'.  This disables everything, resets the stats,
		// runs the test for about 5 seconds, pauses and shows
		// the stats from the result.
		//
		if (m_bRunFullTest)
		{
			// Turn off everything.
			m_bFrameMoving = TRUE;
			m_bSingleStep = FALSE;
			m_fShowHelp = FALSE;
			m_fShowStats = FALSE;
			
			// Clear the stats.  This won't clear any 'lifetime' stats.
			UpdateAllStats();

			// Reset the timer and run for 5 seconds.
			DXUtil_Timer::Start();
			DXUtil_Timer::Reset();

			do
			{
				Render3DEnvironment();
			}
			while (m_fTime < 20.0f);

			// Update the stats.
			UpdateAllStats();

			// Pause.
			m_bFrameMoving = FALSE;
			DXUtil_Timer::Stop();

			// Show the stats.
			m_fShowStats = TRUE;

			// Don't do this agian.
			m_bRunFullTest = FALSE;
		}

        // Render a frame.
        if (m_bActive && m_bReady)
        {
            Render3DEnvironment();
        }
	}
	while(ProcessInput());
}

//-----------------------------------------------------------------------------
// Draws the scene.
//-----------------------------------------------------------------------------

void CD3DApplication::Render3DEnvironment()
{       
    // Get the app's time, in seconds.
    static float fLastAppTime = 0.0f;

    float fAppTime        = DXUtil_Timer::GetAppTime();
    float fElapsedAppTime = fAppTime - fLastAppTime;
    fLastAppTime          = fAppTime;

    // FrameMove (animate) the scene
    if (m_bFrameMoving || m_bSingleStep)
    {
        // Store the time for the app
        m_fTime        = fAppTime;
        m_fElapsedTime = fElapsedAppTime;

        // Frame move the scene
        FrameMove();

        m_bSingleStep = FALSE;
    }

    // Render the scene as normal
    Render();

	if (m_fShowStats)
	{
		int iY = 0;

		if (m_strFrameStats[0])
		{
			DisplayText(m_strFrameStats, 0, 0xffffff00);
			iY += m_dwSpaceHeight;
		}

		if (m_szTestStats[0])
		{
			DisplayText(m_szTestStats, iY, 0xffffff00);
			iY += m_dwSpaceHeight;
		}

		ShowInfo(iY, m_dwSpaceHeight);
	}

	if (m_fShowHelp)
	{
		ShowHelp(m_pBackBuffer);
	}

	// Keep track of the frame rate.
	m_dwFrames++;

	// Only update the stats if the scene is moving.
	if (m_fShowStats && m_bFrameMoving)
    {
		UpdateAllStats();
    }

    // Show the frame on the primary surface.
    CheckHR(m_pDevice->Present(NULL, NULL, NULL, NULL));
}

//-----------------------------------------------------------------------------
// Gets a string for an antialias mode.
//-----------------------------------------------------------------------------

static WCHAR *AntialiasModeName(D3DMULTISAMPLE_TYPE iType)
{
	switch(iType)
	{
    case D3DMULTISAMPLE_NONE:
		return L"D3DMULTISAMPLE_NONE";

	default:
		return L"unknown mode";
	}
}

//-----------------------------------------------------------------------------
// Update and reset all of the statistics.
//-----------------------------------------------------------------------------

void CD3DApplication::UpdateAllStats()
{
    static float fLastTime = 0.0f;

    float fTime = DXUtil_Timer::GetAbsoluteTime();

    // Update the scene stats once per second
    if (fTime - fLastTime > 1.0f)
    {
        m_fFPS     = m_dwFrames / (fTime - fLastTime);

        swprintf(m_strFrameStats, 
			     L"%.2f fps (%dx%dx%d) antialiasmode: %s", 
				 m_fFPS,
                 m_BackBufferDesc.Width, 
				 m_BackBufferDesc.Height, 
#ifdef XBOX
                 m_BackBufferDesc.Format == D3DFMT_LIN_X8R8G8B8 ? 32 : 16,
#else // XBOX
                 16,
#endif // XBOX
				 m_dwSampleMask ? AntialiasModeName(m_iSampleType) : L"not supported");

		// Let the subclass update its stats.
		UpdateStats(fTime - fLastTime);

		// Reset.
        fLastTime  = fTime;
        m_dwFrames = 0L;
    }
}

//-----------------------------------------------------------------------------
// Do all input processing.
//-----------------------------------------------------------------------------

bool CD3DApplication::ProcessInput()
{
	bool retval = true;

	DWORD rgActiveMenu[MT_MAX];
	BOOL rgKeyDown[6];

	unsigned cMenus = 0;

	DWORD iActiveMenu;
	DWORD iActiveKey;

	ZeroMemory(rgKeyDown, sizeof(rgKeyDown));
    ZeroMemory(rgActiveMenu, sizeof(rgActiveMenu));

	//
	// Get the state.
	//

#if defined(PCBUILD)

	// Get the state from the keyboard.
	if (m_pKeyboard)
	{
		BYTE rgKeys[256];

		CheckHR(m_pKeyboard->GetDeviceState(sizeof(rgKeys), &rgKeys));

		// Check the menu keys.
		if (rgKeys[DIK_LSHIFT] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON7;
		}

		if (rgKeys[DIK_RSHIFT] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON8;
		}
		
		if (rgKeys[DIK_LCONTROL] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON9;
		}
		
		if (rgKeys[DIK_RCONTROL] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON10;
		}
		
		if ((rgKeys[DIK_LMENU] & 0x80))
		{
			rgActiveMenu[cMenus++] = MT_STARTBUTTON1;
		}
	
		if ((rgKeys[DIK_RMENU] & 0x80))
		{
			rgActiveMenu[cMenus++] = MT_STARTBUTTON2;
		}

		// Check the action keys.
		if (rgKeys[DIK_A] & 0x80)
		{
			rgKeyDown[0] = TRUE;
		}

		if (rgKeys[DIK_B] & 0x80)
		{
			rgKeyDown[1] = TRUE;
		}

		if (rgKeys[DIK_C] & 0x80)
		{
			rgKeyDown[2] = TRUE;
		}

		if (rgKeys[DIK_D] & 0x80)
		{
			rgKeyDown[3] = TRUE;
		}

		if (rgKeys[DIK_E] & 0x80)
		{
			rgKeyDown[4] = TRUE;
		}

		if (rgKeys[DIK_F] & 0x80)
		{
			rgKeyDown[5] = TRUE;
		}

		// A couple of cheaters.  F1 brings up the main help menu
		// and ESC quits the app.
		//
		if (rgKeys[DIK_F1] & 0x80)
		{
			// This forces the main help menu.
			cMenus = 2;
		}

		if (rgKeys[DIK_ESCAPE] & 0x80)
		{
			// This forces a 'quit'.
			cMenus = 1;
			rgActiveMenu[0] = MT_STARTBUTTON1;

			ZeroMemory(rgKeyDown, sizeof(rgKeyDown));
			rgKeyDown[4] = TRUE;
		}
	}

	// Get the state from the gamepad.
	if (m_pGamepad)
	{
		DIJOYSTATE state;

		CheckHR(m_pGamepad->GetDeviceState(sizeof(state), &state));

		// Check the menu keys.
		if (state.rgbButtons[6] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON7;
		}

		if (state.rgbButtons[7] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON8;
		}
		
		if (state.rgbButtons[8] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON9;
		}
		
		if (state.rgbButtons[9] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON10;
		}
		
		if (state.rgbButtons[10] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_STARTBUTTON1;
		}
	
		if (state.rgbButtons[11] & 0x80)
		{
			rgActiveMenu[cMenus++] = MT_STARTBUTTON2;
		}

		// Check the action keys.
		for (unsigned i = 0; i < 6; i++)
		{
			if (state.rgbButtons[i] & 0x80)
			{
				rgKeyDown[i] = TRUE;
			}
		}
	}

#else // !PCBUILD

    //
    //  Check to see if a port 0 gamepad has come or gone.
    //
    DWORD dwInsertions, dwRemovals;
    if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
    {
        if(dwRemovals&XDEVICE_PORT0_MASK)
        {
            if(m_hDevice)
            {
                XInputClose(m_hDevice);
                m_hDevice = NULL;
            }
        }
        if(dwInsertions&XDEVICE_PORT0_MASK)
        {
            m_hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
        }
    }

    // If we have a device, get its state.
    if (m_hDevice)
    {
        XINPUT_STATE InputState;

        XInputGetState(m_hDevice, &InputState);

		// Check the menu keys.
		if (InputState.Gamepad.bAnalogButtons[6])
		{
			rgActiveMenu[cMenus++] = MT_BUTTON7;
		}

		if (InputState.Gamepad.bAnalogButtons[7])
		{
			rgActiveMenu[cMenus++] = MT_BUTTON8;
		}
		
		if (InputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON9;
		}
		
		if (InputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
		{
			rgActiveMenu[cMenus++] = MT_BUTTON10;
		}
		
		if (InputState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
		{
			rgActiveMenu[cMenus++] = MT_STARTBUTTON1;
		}
	
		if (InputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
		{
			rgActiveMenu[cMenus++] = MT_STARTBUTTON2;
		}

		// Check the action keys.
		for (unsigned i = 0; i < 6; i++)
		{
			if (InputState.Gamepad.bAnalogButtons[i])
			{
				rgKeyDown[i] = TRUE;
			}
		}
    }


#endif // !PCBUILD

	//
	// Process the inputs.
	//

	// We need to show help if at least one menu button is
	// pressed.
	//
	m_fShowHelp = cMenus;

	// Select the active menu.
	if (cMenus != 1 || !m_rgpMenus[rgActiveMenu[0]])
	{
		iActiveMenu = MT_NONE;
	}
	else
	{
		iActiveMenu = rgActiveMenu[0];
	}

	// Find the active key, if any (set to -1 if there is no key).  
	// Don't set the key if there are multiple menu buttons
	// pressed.
	//
	iActiveKey = -1;

	if (cMenus < 2) 
	{
		for (unsigned i = 0; i < 6; i++)
		{
			if (rgKeyDown[i])
			{
				if (iActiveKey == -1)
				{
					iActiveKey = i;
				}
				else
				{
					iActiveKey = -1;
					break;
				}
			}
		}
	}

	// Part of the above ESC hack, if ESC was pressed and is
	// no more, then remember the old menu.
	//
	if (iActiveKey == -1 && m_iActiveKey == 4 && m_iActiveMenu == MT_STARTBUTTON1)
	{
		iActiveMenu = m_iActiveMenu;;
	}

	// Figure out which key was actually pressed and
	// process it.
	//
	if (iActiveKey == -1 && m_iActiveKey != -1)
	{
		if (iActiveMenu == MT_STARTBUTTON1)
		{
			switch(m_iActiveKey)
			{
			// Display stats.
			case 0:
				m_fShowStats = !m_fShowStats;
				break;

			// Run a full test.
			case 1:
				m_bRunFullTest = TRUE;
				break;

			// Pause the test.
			case 2:
				// Toggle frame movement
				m_bFrameMoving = !m_bFrameMoving;

				if (m_bFrameMoving)
				{
					DXUtil_Timer::Start();
				}
				else
				{
					DXUtil_Timer::Stop();
				}
				break;

			// Single-step.
			case 3:
				// Single-step frame movement
				if (FALSE == m_bFrameMoving)
				{
					DXUtil_Timer::Advance();
				}

				DXUtil_Timer::Stop();
				m_bFrameMoving = FALSE;
				m_bSingleStep  = TRUE;

				break;

			// Quit?
			case 4:
				retval = false;
				break;
			}
		}	
		else if (iActiveMenu == MT_STARTBUTTON2)
		{
			switch(m_iActiveKey)
			{
			// Cycle the screen mode.
			case 0:
				m_dwCurrentMode = (m_dwCurrentMode + 1) % m_Adapter.dwNumModes;

				Change3DEnvironment();
				break;

			// Cycle to the next antialias mode.
			case 1:
				{
					unsigned i, iNewSampleType;

					// If no bits are set,then iNewSampleType will wind
					// up with the same value as m_iSampleType;
					//
					for (i = 1; i <= 32; i++)
					{
						iNewSampleType = (m_iSampleType + i) % 32;

						if (m_dwSampleMask & (1 << iNewSampleType ))
						{
							break;
						}
					}
							
					if (m_iSampleType != (D3DMULTISAMPLE_TYPE)iNewSampleType)
					{
						m_iSampleType = (D3DMULTISAMPLE_TYPE)iNewSampleType;						

						Change3DEnvironment();
					}
				}
				break;
			}
		}
		else
		{
			// Send it to the host.
			ProcessKeyPress(m_iActiveMenu, m_iActiveKey);
		}
	}

	// Save the existing state.
	m_iActiveMenu = iActiveMenu;
	m_iActiveKey = iActiveKey;

	return retval;
}

//-----------------------------------------------------------------------------
// Set up the per-app help information.  This pre-computes everything needed
// to draw the help box.
//-----------------------------------------------------------------------------

void CD3DApplication::InitializeHelpInformation()
{
	const unsigned cVertical = 3;		// Spaces for the vertial border
	const unsigned cBetween = 2;        // Space between the two columns.

	unsigned i;

	// Set up the default menu.
	m_rgpMenus[MT_STARTBUTTON1] = &s_MainMenu;
	m_rgpMenus[MT_STARTBUTTON2] = &s_ToggleMenu;
	m_cMenus = 2;

	// Collect menus from our subclass.
	ScenarioMenu *rgOtherMenus[10];
	unsigned iMenu, cMenus;

	cMenus = GetMenus(rgOtherMenus);

	for (iMenu = 0; iMenu < cMenus; iMenu++)
	{
		m_rgpMenus[rgOtherMenus[iMenu]->m_MenuType] = rgOtherMenus[iMenu];
	}

	m_cMenus += cMenus;

	DWORD dwSpaceWidth;
	DWORD dwColumn1Size = 0, dwColumn2Size = 0;

	// Use a big character (such as 'O') to get the height/width that
	// we'll use to create spaces.
	//
    m_Font.GetTextStringLength(&dwSpaceWidth, &m_dwSpaceHeight, L"O");

	// Decide on the size of the menu buttons...these are in column 1
	for (i = 0; i < sizeof(s_rgszMenuButtons) / sizeof(WCHAR *); i++)
	{
		DWORD dwWidth;

        m_Font.GetTextStringLength(&dwWidth, NULL, s_rgszMenuButtons[i]);

		if (dwWidth > dwColumn1Size)
		{
			dwColumn1Size = dwWidth;
		}
	}

	// Figure out the size of the menu names and menu items.
	for (i = 0; i < MT_MAX; i++)
	{
		DWORD dwWidth;

		if (m_rgpMenus[i])
		{
			// The first menu does not have a name.
			if (i != 0)
			{
                m_Font.GetTextStringLength(&dwWidth, NULL, m_rgpMenus[i]->m_szMenuName);

				if (dwWidth > dwColumn2Size)
				{
					dwColumn2Size = dwWidth;
				}
			}

			for (unsigned iItem = 0; iItem < 6; iItem++)
			{
				if (m_rgpMenus[i]->m_szActions[iItem])
				{
                    m_Font.GetTextStringLength(&dwWidth, NULL, m_rgpMenus[i]->m_szActions[iItem]);

					if (dwWidth > dwColumn2Size)
					{
						dwColumn2Size = dwWidth;
					}
				}
			}
		}
	}

	// Compute the values of each column and the total width of the box.  The height
	// can't be calculated until we know which menu is being displayed.
	//
	m_dwColumn1X = dwSpaceWidth * cVertical;
	m_dwColumn2X = m_dwColumn1X + dwColumn1Size + dwSpaceWidth * cBetween;
	m_dwHelpWidth = m_dwColumn2X + dwColumn2Size + dwSpaceWidth * cVertical;
}

//-----------------------------------------------------------------------------
// Draw a help screen on the display.  Make it look pretty.
//-----------------------------------------------------------------------------

void CD3DApplication::ShowHelp(IDirect3DSurface8 *pSurface)
{		
	const unsigned cHorizontal = 2;		// Spaces for the horizontal border

	// Which menu is being displayed?
	const ScenarioMenu *pMenu = m_rgpMenus[m_iActiveMenu];

	// How big is the box?
	unsigned dwHelpHeight = m_dwSpaceHeight * cHorizontal * 2 + m_cMenus * m_dwSpaceHeight;
	
	if (pMenu)
	{
		dwHelpHeight += m_dwSpaceHeight + pMenu->m_cActions * m_dwSpaceHeight;
	}

	// Is the box too big?
	DWORD dwBoxX, dwBoxY;

	// Box too big?
	if (m_Adapter.modes[m_dwCurrentMode].Height <= dwHelpHeight || 
		m_Adapter.modes[m_dwCurrentMode].Width <= m_dwHelpWidth)
	{
		DisplayError(L"FATAL: Help box is too big for this display mode.", 0);
	}

	// Center the box.
	dwBoxX = (m_Adapter.modes[m_dwCurrentMode].Width  - m_dwHelpWidth) / 2;
	dwBoxY = (m_Adapter.modes[m_dwCurrentMode].Height - dwHelpHeight) / 2;

	// Clear the box.
	D3DRECT rect = { dwBoxX, dwBoxY, dwBoxX + m_dwHelpWidth, dwBoxY + dwHelpHeight };
	CheckHR(m_pDevice->Clear(1, &rect, D3DCLEAR_TARGET, 0x00000000, 0.0f, 0L));

	// Paint the text.
	DWORD dwY = dwBoxY + m_dwSpaceHeight * cHorizontal;
	unsigned i, c;

	// Start with the menus.
	c = MT_MAX;

	for (i = 1; i < c; i++)
	{
		if (m_rgpMenus[i])
		{
            m_Font.DrawText(m_pBackBuffer, s_rgszMenuButtons[i - 1], m_dwColumn1X + dwBoxX, dwY, DRAWTEXT_TRANSPARENTBKGND, 0x00FFFFFF, 0);
            m_Font.DrawText(m_pBackBuffer, m_rgpMenus[i]->m_szMenuName, m_dwColumn2X + dwBoxX, dwY, DRAWTEXT_TRANSPARENTBKGND, 0x00FFFFFF, 0);

			dwY += m_dwSpaceHeight;
		}
	}

	// Menu items.
	if (pMenu)
	{
		dwY += m_dwSpaceHeight;

		for (i = 0; i < 6; i++)
		{
			if (pMenu->m_szActions[i])
			{
				WCHAR sz[2] = L"A";
				sz[0] += (WCHAR) i;

                m_Font.DrawText(m_pBackBuffer, sz, m_dwColumn1X + dwBoxX, dwY, DRAWTEXT_TRANSPARENTBKGND, 0x00FFFFFF, 0);
                m_Font.DrawText(m_pBackBuffer, pMenu->m_szActions[i], m_dwColumn2X + dwBoxX, dwY, DRAWTEXT_TRANSPARENTBKGND, 0x00FFFFFF, 0);

				dwY += m_dwSpaceHeight;
			}
		}
	}
}

//=========================================================================
// Map our 'new' directly to LocalAlloc
//=========================================================================

static void* __cdecl operator new(size_t size)
{
    return LocalAlloc(0, size);
}

//=========================================================================
// Map our 'delete' directly to LocalFree   
//=========================================================================

static VOID __cdecl operator delete(VOID* alloc)
{
    LocalFree(alloc);
}
