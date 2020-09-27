//=============================================================================
//=============================================================================
#include <xtl.h>
#include <assert.h>
#include "..\xbfont.h"
#include "..\Image\XImage.h"
#include <xgraphics.h>

//=============================================================================
//=============================================================================
struct DEMO_LAUNCH_DATA
{
	DWORD	dwID;				// I don't know what I'll use this for, yet...
	DWORD	dwRunmode;
	DWORD	dwTimeout;
	char	szLauncherXBE[64];	// name of XBE to launch to return
	char	szLaunchedXBE[64];	// name of your XBE, useful to get path info from
	char	szDemoInfo[MAX_LAUNCH_DATA_SIZE - (128 * sizeof(char)) - (3 * sizeof(DWORD))];
								// set in XDI file, additional info/parameters/whatever
};

#define RUNMODE_KIOSKMODE		0x01
#define RUNMODE_USERSELECTED	0x02

//=============================================================================
//=============================================================================
#define WIDTH		640
#define HEIGHT		480
#define MAKE_RGBA(r,g,b,a) ((((a)<<24)&0xFF000000) | (((r)<<16)&0x00FF0000) | (((g)<<8)&0x0000FF00) | (((b)<<0)&0x000000FF))

//=============================================================================
// function prototypes
//=============================================================================
extern HRESULT InitInput(void);
extern void HandleInput(void);

//=============================================================================
// Global variables
//=============================================================================
LPDIRECT3D8				g_pD3D					= NULL;
LPDIRECT3DDEVICE8		g_pd3dDevice			= NULL;
LPDIRECT3DTEXTURE8		g_pTexture				= NULL;
CXBFont *				g_pFont					= NULL;
XImage					g_Background;
char *					g_pszDirectory;
DEMO_LAUNCH_DATA		g_ld;
DWORD					g_dwTimeout				= 0;

//=============================================================================
// hard-coded file names
//=============================================================================


#define STICK_THRESHHOLD	15000
//=============================================================================
DWORD					g_dwDeviceState	= 0;
HANDLE					g_hInput[12]	= {0};

//=============================================================================
//=============================================================================
void HandleDeviceChanges(XPP_DEVICE_TYPE *pxdt, DWORD dwInsert, DWORD dwRemove)
{

	return;
}

//=============================================================================
//=============================================================================
char *FindPath(const char *szRelPath)
{
	static char szPath[256];

	strcpy(szPath, g_pszDirectory);
	strcat(szPath, szRelPath);

	return szPath;
}


//=============================================================================
//=============================================================================
HANDLE GetController(bool bFirst, int *pnum=NULL)
{
	static int i=0;

	if (bFirst)
		i=0;

	for ( ; i<12 ; i++)
	{
		if (g_dwDeviceState & (1<<i))
		{
			if (pnum)
			{
				*pnum = i;
			}
			return g_hInput[i++];
		}
	}

	return NULL;
}

//=============================================================================
//=============================================================================
void Input(void)
{
	XLaunchNewImage(g_ld.szLauncherXBE, (LAUNCH_DATA *)&g_ld);
}

//=============================================================================
//=============================================================================
void HandleInput(void)
{
	DWORD				dwInsert;
	DWORD				dwRemove;
	XINPUT_STATE		xis;
	static XINPUT_STATE	xis_old[12] = {0};

	if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsert, &dwRemove))
	{
		HandleDeviceChanges(XDEVICE_TYPE_GAMEPAD, dwInsert, dwRemove);

		g_dwDeviceState &= ~dwRemove;
		g_dwDeviceState |= dwInsert;

		for( DWORD i=0; i < XGetPortCount(); i++ )
		{
			// Handle removed devices.
			if( dwRemove & (1<<i) )
			{
				if (g_hInput[i])
				{
					XInputClose( g_hInput[i] );
					g_hInput[i] = NULL;
				}
			}

			if( dwInsert & (1<<i) )
			{
				if (!g_hInput[i])
					g_hInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );

				int err;
				if (g_hInput[i] == NULL)
				{
					err=GetLastError();
				}
				assert(g_hInput[i]);
			}

		}

	}


	HANDLE hPad;

	int n;
	hPad=GetController(true, &n);

	while (hPad)
	{
		XInputGetState(hPad, &xis);

		// now, make the analog thumb sticks either all or nothing
		if (xis.Gamepad.sThumbLY > STICK_THRESHHOLD)
			xis.Gamepad.sThumbLY = STICK_THRESHHOLD;
		else if (xis.Gamepad.sThumbLY < -STICK_THRESHHOLD)
			xis.Gamepad.sThumbLY = -STICK_THRESHHOLD;
		else
			xis.Gamepad.sThumbLY = 0;

		if (xis.Gamepad.sThumbRY > STICK_THRESHHOLD)
			xis.Gamepad.sThumbRY = STICK_THRESHHOLD;
		else if (xis.Gamepad.sThumbRY < -STICK_THRESHHOLD)
			xis.Gamepad.sThumbRY = -STICK_THRESHHOLD;
		else
			xis.Gamepad.sThumbRY = 0;

		if (
			((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]))
			||
			((xis.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !((xis_old[n].Gamepad.wButtons & XINPUT_GAMEPAD_START)))
			)
		{
			Input();
		}

		if ((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]))
		{
			Input();
		}

		if ((xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && !((xis_old[n].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)))
		{
			Input();
		}

		if ((xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && !((xis_old[n].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)))
		{
			Input();
		}

		if ((xis.Gamepad.sThumbLY == STICK_THRESHHOLD) && ((xis_old[n].Gamepad.sThumbLY != STICK_THRESHHOLD)))
		{
			Input();
		}

		if ((xis.Gamepad.sThumbLY == -STICK_THRESHHOLD) && ((xis_old[n].Gamepad.sThumbLY != -STICK_THRESHHOLD)))
		{
			Input();
		}

		if ((xis.Gamepad.sThumbRY == STICK_THRESHHOLD) && ((xis_old[n].Gamepad.sThumbRY != STICK_THRESHHOLD)))
		{
			Input();
		}

		if ((xis.Gamepad.sThumbRY == -STICK_THRESHHOLD) && ((xis_old[n].Gamepad.sThumbRY != -STICK_THRESHHOLD)))
		{
			Input();
		}



		xis_old[n] = xis;

		hPad=GetController(false, &n);
	}

	if (g_ld.dwRunmode & RUNMODE_KIOSKMODE)
	{
		if (g_dwTimeout < timeGetTime())
			Input();
	}
}

//=============================================================================
//=============================================================================
HRESULT InitInput(void)
{
	XDEVICE_PREALLOC_TYPE xdpt[] = {
		{XDEVICE_TYPE_GAMEPAD, 4},
	};

	XInitDevices(sizeof(xdpt)/sizeof(xdpt[0]), xdpt);

	g_dwDeviceState = XGetDevices(XDEVICE_TYPE_GAMEPAD);

	for( DWORD i=0; i<4 ; i++ )
	{
		if( g_dwDeviceState & (1<<i) )
		{
			if (!g_hInput[i])
				g_hInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );
		}
	}

	return S_OK;
}


//=============================================================================
//=============================================================================
HRESULT InitD3D(void)
{
    // Create the D3D object, which is used to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    // Set fullscreen 640x480x32 mode
    d3dpp.BackBufferWidth        = WIDTH;
    d3dpp.BackBufferHeight       = HEIGHT;
    d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;

    // Create one backbuffer and a zbuffer
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Set up how the backbuffer is "presented" to the frontbuffer each frame
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    // Create the Direct3D device. Hardware vertex processing is specified
    // since all vertex processing takes place on Xbox hardware.
    if( FAILED( g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
        return E_FAIL;

    // After creating the device, initial state would normally be set

    return S_OK;
}



//=============================================================================
//=============================================================================
HRESULT InitBackground(void)
{
//#ifdef _DEBUG
//	g_Background.Load(g_pd3dDevice, "d:\\media\\screen.png");
//#else
	g_Background.Load(g_pd3dDevice, FindPath("screen.png"));
//#endif

	return S_OK;
}

//=============================================================================
//=============================================================================
#pragma optimize("", off)
void AsciiToUnicode(char *s, WCHAR *w)
{
	while (*s)
	{
		((char *)w)[0] = *s;
		((char *)w)[1] = 0;

		w++;
		s++;

		//*w++ = (WCHAR)*s++;
	}
	*w = 0;
}

//=============================================================================
//=============================================================================
VOID Render()
{
	// Cycle through some colors to clear the screen (just to show some output)
	static FLOAT r = 0.0f; if( (r+=1.3f) > 255.0f ) r = 0.0f;
	static FLOAT g = 0.0f; if( (g+=1.7f) > 255.0f ) g = 0.0f;
	static FLOAT b = 0.0f; if( (b+=1.5f) > 255.0f ) b = 0.0f;

	WCHAR wstring[64];
	float x;

	if (true)
	{
		FILETIME ft;
		FILETIME ft2;
		GetSystemTimeAsFileTime(&ft);

		SYSTEMTIME st;

		st.wYear			= 2001;
		st.wMonth			= 11;
		st.wDay				= 8;

		st.wHour			= 0;
		st.wMinute			= 0;
		st.wSecond			= 0;
		st.wMilliseconds	= 0;

		SystemTimeToFileTime(&st, &ft2);

		char string[64];

		__int64 diff = (*(__int64 *)&ft2)-(*(__int64 *)&ft);

		assert((diff/10000000) > 10);

		int ms	= (int)((diff/10000) % 1000);
		int sec	= (int)((diff/10000000) % 60);
		int min	= (int)((diff/600000000) % 60);
		int hr	= (int)((diff/36000000000) % 24);
		int days= (int)((diff/(36000000000*24)));

		wsprintf(string, "%d days, %3d:%02d:%02d", days, hr, min, sec);
	//	wsprintf(string, "%d", (unsigned int)(diff/10000000));

		//strcpy(string, "2503:30:12.523");

		//OutputDebugString(string);
		//OutputDebugString("   ");

		AsciiToUnicode(string, wstring);
		//UnicodeToAscii(wstring, string);

		//OutputDebugString(string);
		//OutputDebugString("\n");

		float width, height;
		g_pFont->GetTextExtent(wstring, &width, &height);

		x=320 - (width/2);

	}

	// Clear the backbuffer to a changing color
	//g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( (int)r, (int)g, (int)b ), 1.0f, 0L );
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );

	// Begin the scene
	g_pd3dDevice->BeginScene();

	g_Background.Blt();
	//g_pFont->DrawText( x, 60, MAKE_RGBA(112, 188, 31, 0xFF), wstring);
	g_pFont->DrawText( x, 60, MAKE_RGBA(56, 94, 16, 0xFF), wstring);
    g_pd3dDevice->EndScene();
}
#pragma optimize("", on)


//=============================================================================
// main()
//
// The main function that runs the program
//=============================================================================
void __cdecl main()
{
#ifdef _DEBUG
//	__asm int 3;
#endif

	if (true)
	{
		static char string[256];
		DWORD dwLaunchInfo;

		if ((XGetLaunchInfo(&dwLaunchInfo, (LAUNCH_DATA *)&g_ld)==ERROR_SUCCESS) && ((dwLaunchInfo&3) == LDT_TITLE))
		{
			// we were probably launched from XDemos, so get the path from
			// it
			strcpy(string, g_ld.szLaunchedXBE);
			char *p=string;
			for ( ; *p ; p++);
			for ( ; *p != '\\' && p>string ; p--);
			strcpy(p, "\\media\\");

			g_pszDirectory = string;

			if (g_ld.dwRunmode & RUNMODE_KIOSKMODE)
			{
#ifdef _DEBUG
				g_dwTimeout = timeGetTime() + 15000;
#else
				g_dwTimeout = timeGetTime() + g_ld.dwTimeout;
#endif
			}
		}
		else
		{
			g_ld.szLauncherXBE[0]=0;
			g_pszDirectory = "d:\\Media\\";
		}
	}

	// various initialization
    if (FAILED(InitD3D()))
        return;

	if (FAILED(InitBackground()))
		return;

	if (FAILED(InitInput()))
		return;

	// create the fonts that we will use
	g_pFont = new CXBFont;
	g_pFont->Create(g_pd3dDevice, FindPath("Font.xpr"));

	// the main loop
    while (true)
    {
		HandleInput();
        Render();
        g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    }
}




