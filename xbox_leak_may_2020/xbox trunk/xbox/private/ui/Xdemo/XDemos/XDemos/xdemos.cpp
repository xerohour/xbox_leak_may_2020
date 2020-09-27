//=============================================================================
// File: XDemos.cpp
//
// Desc: This is the main module for the XDemos project
// Created: 07/02/2001 by Michael Lyons (mlyons@microsoft.com)
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================
#include "XDemos.h"
#include "Image\XImage.h"
#include <xgraphics.h>
#ifdef _DEBUG
#include <xbdm.h>
#endif
#ifdef PROFILE
#include <xbdm.h>
#include <d3d8perf.h>
#pragma comment(lib, "xbdm.lib")
#endif

//=============================================================================
//=============================================================================
#define WIDTH		640
#define HEIGHT		480

#ifdef _DEBUG
#define VIDEO_DELAY	75000
#else
//#define VIDEO_DELAY	5000
#define VIDEO_DELAY	75000
#endif

#define MAX_PRIORITY	100

#define MAKE_RGBA(r,g,b,a) ((((a)<<24)&0xFF000000) | (((r)<<16)&0x00FF0000) | (((g)<<8)&0x0000FF00) | (((b)<<0)&0x000000FF))


#define TEXT_START_X			87//70
#define TEXT_START_Y			145//108
#define TEXT_SECOND_OFFSET		10
#define TEXT_LINE_SEP			24
#define TEXT_DEMO_SEP			49
#define FILL_START_X			82//68
#define FILL_START_Y			147//110
#define FILL_WIDTH				227//(291-67)
#define FILL_HEIGHT				50
#define FILL_SEP				49
#define TEASER_X				335//335
#define TEASER_Y				130//120
#define TEASER_W				(594-335)
#define TEASER_H				(311-120)

#define BUTTON_DELAY			500
#define BUTTON_REPEAT			175

#define ARROW_X					178//153
#define ARROW_UP				133//100
#define ARROW_DN				346//310
#define ARROW_W					32//46
#define ARROW_H					16//12


//=============================================================================
// function prototypes
//=============================================================================
void UpdateBackgroundMovie(void);

//=============================================================================
//=============================================================================
class DEMO
{
public:
	XImage *		m_Teaser;
	XImage *		m_PleaseWait;
	DIB *			m_pDib;
	WCHAR *			m_Title;
	WCHAR *			m_Type;
	char *			m_szFile;
};

//=============================================================================
// Global variables
//=============================================================================
LPDIRECT3D8				g_pD3D					= NULL;
LPDIRECT3DDEVICE8		g_pd3dDevice			= NULL;
LPDIRECT3DTEXTURE8		g_pTexture				= NULL;
CXBFont *				g_pFontN				= NULL;
CXBFont *				g_pFontB				= NULL;
XImage					g_Background;
XImage					g_PleaseWait;
XImage					g_UpArrow;
XImage					g_DnArrow;
XImage *				g_ReturnToGamePleaseWait = NULL;

int						g_iListChoice			= 0;
int						g_iListOffset			= 0;
XDIHEADER *				g_pXDIHeader;
DEMO *					g_pDemos				= NULL;
char *					g_pszDirectory			= NULL;

IDirect3DTexture8 *		g_pBkgndTexture			= NULL;
HBINK					g_hBkgndMovie			= NULL;
DWORD					g_dwNextButton			= 0;


// timing mode variables
bool					g_bTimingMode			= false;
int						g_iNextVideo			= 0;
DWORD					g_dwVideoTime			= 0;



//=============================================================================
// hard-coded file names
//=============================================================================
const char *			g_pszSettingsFile	= "t:\\XDemos.ini";
const char *			g_pszXDI1			= "d:\\XDemos\\XDemos.xdi";
const char *			g_pszXDI2			= "d:\\XDemos.xdi";
const char *			g_pszAmbientSound	= "media\\ambient.wma";
const char *			g_pszDn				= "media\\dn.png";
const char *			g_pszMenuSound		= "media\\menu.wav";
const char *			g_pszLaunchSound	= "media\\launch.wav";
const char *			g_pszBackground		= "media\\screen.png";
const char *			g_pszBackgroundT	= "media\\screenT.png";
const char *			g_pszWait			= "media\\wait.png";
const char *			g_pszUp				= "media\\up.png";
const char *			g_pszFontN			= "media\\fontN.xpr";
const char *			g_pszFontB			= "media\\fontB.xpr";
const char *			g_pszBackgroundMovie= "media\\bkgnd.bik";



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
bool ExecuteDemo(DEMO &demo, bool bTimingMode)
{
	// verify the file exists
	if (true)
	{
		HANDLE hFile;

		hFile=CreateFile(demo.m_szFile, 0, 0, 0, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

	 	CloseHandle(hFile);
	}


	if (demo.m_pDib->m_dwFlags & DIBFLAGS_MOVIE)
	{
		return PlayMovie(demo.m_szFile);
	}
	else
	{
		//
		// save the state
		//
		SaveState(g_iListChoice, g_iListOffset, g_iNextVideo);
		DEMO_LAUNCH_DATA ld;

		ld.dwID = 0;
		ld.dwTimeout = 60000;
		strcpy(ld.szLaunchedXBE, demo.m_szFile);

		if (g_bTimingMode)
			strcpy(ld.szLauncherXBE, "d:\\XDemos.xbe");
		else
			strcpy(ld.szLauncherXBE, "d:\\XDemos\\XDemos.xbe");

		if (bTimingMode)
			ld.dwRunmode = RUNMODE_KIOSKMODE;
		else
			ld.dwRunmode = RUNMODE_USERSELECTED;

		demo.m_PleaseWait->Blt();
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		g_pd3dDevice->PersistDisplay();

		XLaunchNewImage(demo.m_szFile, (LAUNCH_DATA *)&ld);
	}

	return false;
}

//=============================================================================
//=============================================================================
void Input(void)
{
	g_dwVideoTime = timeGetTime() + VIDEO_DELAY;
}

//=============================================================================
//=============================================================================
void ButtonUp(void)
{
	DWORD time=timeGetTime();

	if (g_dwNextButton == 0)
		g_dwNextButton = time + BUTTON_DELAY;
	else if (time > g_dwNextButton)
		g_dwNextButton += BUTTON_REPEAT;
	else
		return;

	g_iListChoice--;
	if (g_iListChoice < 0)
		g_iListChoice = 0;
	else
		PlayMenuSound();

	if ((g_iListChoice - g_iListOffset) > 2)
	{
		g_iListOffset = g_iListChoice - 2;
	}

	if ((g_iListChoice - g_iListOffset) < 1)
	{
		g_iListOffset = g_iListChoice - 1;
	}

	if (g_iListOffset == g_pXDIHeader->m_lNumDemos - 1)
		g_iListOffset = g_pXDIHeader->m_lNumDemos - 2;

	if (g_iListOffset < 0)
		g_iListOffset = 0;
}

//=============================================================================
//=============================================================================
void ButtonDn(void)
{
	DWORD time=timeGetTime();

	if (g_dwNextButton == 0)
		g_dwNextButton = time + BUTTON_DELAY;
	else if (time > g_dwNextButton)
		g_dwNextButton += BUTTON_REPEAT;
	else
		return;

	g_iListChoice++;
	if (g_iListChoice == g_pXDIHeader->m_lNumDemos)
		g_iListChoice = g_pXDIHeader->m_lNumDemos-1;
	else
		PlayMenuSound();

	if ((g_iListChoice - g_iListOffset) > 2)
	{
		if (g_iListChoice != g_pXDIHeader->m_lNumDemos - 1)
			g_iListOffset = g_iListChoice - 2;
	}

	if ((g_iListChoice - g_iListOffset) < 1)
	{
		g_iListOffset = g_iListChoice - 1;
	}

	if (g_iListOffset == g_pXDIHeader->m_lNumDemos - 1)
		g_iListOffset = g_pXDIHeader->m_lNumDemos - 2;

	if (g_iListOffset < 0)
		g_iListOffset = 0;
}

//=============================================================================
//=============================================================================
void ButtonMiddle(void)
{
	g_dwNextButton = 0;
	return;
}


//=============================================================================
//=============================================================================
void ButtonA(void)
{
	PlayLaunchSound();

	ExecuteDemo(g_pDemos[g_iListChoice], false);

}

//=============================================================================
//=============================================================================
void ButtonB(void)
{
	if (g_bTimingMode)
		return;

	g_ReturnToGamePleaseWait->Blt();
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	g_pd3dDevice->PersistDisplay();
	LAUNCH_DATA ld;

	ld.Data[0]='X';
	ld.Data[1]='D';
	ld.Data[2]='E';
	ld.Data[3]='M';
	ld.Data[4]='O';
	ld.Data[5]='S';
	ld.Data[6]=0;

	SaveState(0, 0, 0);

	PlayLaunchSound();
	XLaunchNewImage(g_pXDIHeader->m_szGameName, &ld);
	// exit!!
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
	if (g_bTimingMode)
		g_Background.Load(g_pd3dDevice, FindPath(g_pszBackgroundT));
	else
		g_Background.Load(g_pd3dDevice, FindPath(g_pszBackground));

	g_PleaseWait.Load(g_pd3dDevice, FindPath(g_pszWait));
	g_UpArrow.Load(g_pd3dDevice, FindPath(g_pszUp));
	g_DnArrow.Load(g_pd3dDevice, FindPath(g_pszDn));

	return S_OK;

}

//=============================================================================
//=============================================================================
bool PlayBinkFile(char *szFileName)
{
	bool bReturn = false;

	StopAudio();

	CheckForInput();

	IDirect3DTexture8 *pTexture = NULL;
	HBINK hBink = BinkOpen(szFileName, 0);
	if (!hBink)
		return false;
	
	g_pd3dDevice->CreateTexture(hBink->Width, hBink->Height, 1, 0, D3DFMT_LIN_X8R8G8B8, 0, &pTexture);

	int fs=0;

	while (true)
	{
		BinkDoFrame(hBink);
		if (CheckForInput())
		{
			bReturn = true;
			g_dwNextButton = (DWORD)-1;
			break;
		}

		if (true)
		{
			D3DLOCKED_RECT lr;

			if (true)
			{
				pTexture->LockRect(0, &lr, NULL, 0);
				BinkCopyToBuffer(hBink, lr.pBits, lr.Pitch, hBink->Height, 0, 0, BINKCOPYALL | BINKSURFACE32);
				pTexture->UnlockRect(0);
			}

			static struct {
				float	x,y,z,w;
				float	u,v;
			} vertices[4] = {
				{ 0,0,0,1,0,0 },
				{ 0,0,0,1,0,0 },
				{ 0,0,0,1,0,0 },
				{ 0,0,0,1,0,0 },
			};

			if (true)
			{
				if (true)
				{
					vertices[0].x	= 0.0f;
					vertices[0].y	= 0.0f;

					vertices[1].x	= WIDTH;
					vertices[1].y	= 0.0f;

					vertices[2].x	= 0.0f;
					vertices[2].y	= HEIGHT;

					vertices[3].x	= WIDTH;
					vertices[3].y	= HEIGHT;
				}

				if (true)
				{
					vertices[0].u	= -.5f + 0.0f;
					vertices[0].v	= -.5f + 0.0f;

					vertices[1].u	= -.5f + (float)hBink->Width-10;
					vertices[1].v	= -.5f + 0.0f;

					vertices[2].u	= -.5f + 0.0f;
					vertices[2].v	= -.5f + (float)hBink->Height-10;

					vertices[3].u	= -.5f + (float)hBink->Width-10;
					vertices[3].v	= -.5f + (float)hBink->Height-10;
				}
			}

			g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0L );
			g_pd3dDevice->BeginScene();
			g_pd3dDevice->SetTexture(0, pTexture);
			g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(vertices[0]));

			// End the scene
			g_pd3dDevice->EndScene();

		    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
		
		// blit the frame onto the screen here (platform specific)
		if (hBink->FrameNum == (hBink->Frames-1))
			break;	// done at the point
		else
			BinkNextFrame(hBink);  // skip to the next frame
		
		while (BinkWait(hBink));
	}

	pTexture->Release();

	BinkClose(hBink);

	StartAudio();

	return bReturn;
}

//=============================================================================
//=============================================================================
bool PlayMovie(char *szFileName)
{
#ifdef WMV
	char *pExtension = strrchr(szFileName, '.');

	if (pExtension && _stricmp(pExtension, ".wmv") == 0)
	{
		return PlayWMVFile(szFileName);
	}
	else
	{
		return PlayBinkFile(szFileName);
	}
#else
	return PlayBinkFile(szFileName);
#endif
}

//=============================================================================
//=============================================================================
VOID Render()
{
	// Cycle through some colors to clear the screen (just to show some output)
	static FLOAT r = 0.0f; if( (r+=1.3f) > 255.0f ) r = 0.0f;
	static FLOAT g = 0.0f; if( (g+=1.7f) > 255.0f ) g = 0.0f;
	static FLOAT b = 0.0f; if( (b+=1.5f) > 255.0f ) b = 0.0f;

	// Clear the backbuffer to a changing color
	//g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( (int)r, (int)g, (int)b ), 1.0f, 0L );
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, MAKE_RGBA(0x30, 0xA0, 0x00, 0x00), 1.0f, 0L );

	// Begin the scene
	g_pd3dDevice->BeginScene();

	UpdateBackgroundMovie();

	g_Background.Blt();
	if (g_pDemos)
		g_pDemos[g_iListChoice].m_Teaser->Blt(TEASER_X, TEASER_Y, TEASER_W, TEASER_H);
	else
	{
		g_pFontN->DrawText(TEXT_START_X, TEXT_START_Y, MAKE_RGBA(0xC6, 0xFF, 0x00, 0xFF), L"Corrupt XDI file");
	}

	const float period=8.0f;
	const float twopi = 6.283185307179586476925286766559f;
	const float halfpi = 1.5707963267948966192313216916398f;
	float time = (float)(timeGetTime())/1000.0f;

	for (int i = g_iListOffset ; i < g_iListOffset + 4 ; i++)
	{
		int offset = i - g_iListOffset;
		//int opacity = (int)(((float)sin((twopi * time / period) + (-i * halfpi)) * 100.0f) + 255.0f - 60.0f);
		//if (opacity > 204)
		//	opacity=204;
		int opacity = 255;

		if (i == g_pXDIHeader->m_lNumDemos)
			break;


		DWORD dwTextColor;
		DWORD dwAltTextColor;

		if (i == g_iListChoice)
		{
			dwTextColor = MAKE_RGBA(37, 77, 16, opacity);
			dwAltTextColor = MAKE_RGBA(57, 97, 36, opacity);
			//dwTextColor = MAKE_RGBA(0xD6, 0xE1, 0x0B, opacity);

			//g_Background.FillArea(FILL_START_X, FILL_START_Y + FILL_SEP * offset, FILL_WIDTH, FILL_HEIGHT, MAKE_RGBA(0xC6, 0xFF, 0x00, opacity));
			g_Background.FillArea(FILL_START_X, FILL_START_Y + FILL_SEP * offset, FILL_WIDTH, FILL_HEIGHT, MAKE_RGBA(187, 213, 98, opacity));
		}
		else
		{
			dwTextColor = MAKE_RGBA(108, 203, 53, opacity);
			dwAltTextColor = MAKE_RGBA(68, 153, 13, opacity);
			//dwTextColor = MAKE_RGBA(0xC6, 0xFF, 0x00, opacity);
		}


		g_pFontB->DrawText(TEXT_START_X, (float)(TEXT_START_Y + TEXT_DEMO_SEP * offset), dwTextColor, g_pDemos[i].m_Title);
		g_pFontN->DrawText(TEXT_START_X+TEXT_SECOND_OFFSET, (float)(TEXT_START_Y + TEXT_LINE_SEP + TEXT_DEMO_SEP * offset), dwAltTextColor, g_pDemos[i].m_Type);
	}

	if (g_iListOffset > 0)
	{
		// show up arrow
		g_UpArrow.Blt(ARROW_X, ARROW_UP, ARROW_W, ARROW_H);
	}

	if (g_pXDIHeader->m_lNumDemos > (g_iListOffset+4))
	{
		// show down arrow
		g_DnArrow.Blt(ARROW_X, ARROW_DN, ARROW_W, ARROW_H);
	}

    // End the scene
    g_pd3dDevice->EndScene();

#ifdef PROFILE
	if (true)
	{
		static DWORD dwLastTime=0;
		DWORD dwTime=timeGetTime();
		WORD string[64];

		wsprintfW(string, L"%d ms/f", dwTime-dwLastTime);
		g_pFontB->DrawText(400, 30, 0xFFFFFFFF, string);

		float fps=1000.0f / (dwTime-dwLastTime);

		wsprintfW(string, L"%.2f fps", fps);
		g_pFontB->DrawText(400, 55, 0xFFFFFFFF, string);

		dwLastTime=dwTime;
	}
#endif
}


//=============================================================================
// InitXDI
//
// This function reads in the XDI file, sorts the demos according to priority,
// and loads in each demo's teaser image.
//=============================================================================
HRESULT InitXDI(void)
{
	bool bRoot=false;

	DWORD dwSize;

	// try to read the XDI file in the XDemos directory (as if we're launched from a game
    dwSize = XDIReadFile(g_pszXDI1, NULL, 0);
	if (dwSize == -1)
	{
		// okay, then try to load it in the root directory, as if we're launched in Kiosk
		// mode
	    dwSize = XDIReadFile(g_pszXDI2, NULL, 0);
		g_pXDIHeader = (XDIHEADER *)calloc(dwSize, 1);
		if (XDIReadFile(g_pszXDI2, g_pXDIHeader, dwSize) == -1)
		{
			// error reading XDI file!!
			g_pXDIHeader->m_fKiosk			= true;
			g_pXDIHeader->m_lNumDemos		= 0;
			g_pXDIHeader->m_szGameName[0]	= 0;
			g_pXDIHeader->m_szPersist[0]	= 0;
			g_pXDIHeader->m_lVerMajor		= VERMAJOR;
			g_pXDIHeader->m_lVerMinor		= VERMINOR;

			if (true)
			{
				HANDLE hFile;

				hFile=CreateFile("d:\\XDemos.xdi", 0, 0, 0, OPEN_EXISTING, 0, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					g_pszDirectory = "d:\\XDemos\\";
				}
				else
				{
					bRoot=true;
					g_pszDirectory = "d:\\";
					CloseHandle(hFile);
				}
			}
		}
		else
			bRoot=true;
	}
	else
	{
		g_pXDIHeader = (XDIHEADER *)calloc(dwSize, 1);
		if (XDIReadFile(g_pszXDI1, g_pXDIHeader, dwSize) == -1)
		{
			// error reading XDI file!!
			g_pXDIHeader->m_fKiosk			= true;
			g_pXDIHeader->m_lNumDemos		= 0;
			g_pXDIHeader->m_szGameName[0]	= 0;
			g_pXDIHeader->m_szPersist[0]	= 0;
			g_pXDIHeader->m_lVerMajor		= VERMAJOR;
			g_pXDIHeader->m_lVerMinor		= VERMINOR;

			if (true)
			{
				HANDLE hFile;

				hFile=CreateFile("d:\\XDemos.xdi", 0, 0, 0, OPEN_EXISTING, 0, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					g_pszDirectory = "d:\\XDemos\\";
				}
				else
				{
					bRoot=true;
					g_pszDirectory = "d:\\";
					CloseHandle(hFile);
				}
			}
		}
	}

	if (!g_pszDirectory)
	{
		if (g_pXDIHeader->m_szGameName[0])
		{
			g_bTimingMode = false;
			if (bRoot)
				g_pszDirectory = "d:\\";
			else
				g_pszDirectory = "d:\\XDemos\\";
		}
		else
		{
			g_bTimingMode = true;
			g_dwVideoTime = timeGetTime() + VIDEO_DELAY;
			g_pszDirectory = "d:\\";
		}
	}

	if (g_pXDIHeader->m_lNumDemos)
		g_pDemos = (DEMO *)malloc(sizeof(DEMO) * g_pXDIHeader->m_lNumDemos);

	g_ReturnToGamePleaseWait = &g_PleaseWait;
	if (g_pXDIHeader->m_szPersist && g_pXDIHeader->m_szPersist[0])
	{
		g_ReturnToGamePleaseWait = new XImage;
		g_ReturnToGamePleaseWait->Load(g_pd3dDevice, g_pXDIHeader->m_szPersist);
	}

	int pri;
	int i=0;

	for (pri=0 ; pri<MAX_PRIORITY ; pri++)
	{
		DIB *pdib = (DIB *)((int)g_pXDIHeader + sizeof(XDIHEADER));

		for (int n=0 ; n<g_pXDIHeader->m_lNumDemos ; n++)
		{
			if (pdib->m_dwPriority == pri)
			{
				g_pDemos[i].m_pDib = pdib;

				//
				// do images
				//
				g_pDemos[i].m_Teaser = new XImage;
				g_pDemos[i].m_Teaser->Load(g_pd3dDevice, FindPath(pdib->m_pszTeaser));

				if (pdib->m_pszPersist)
				{
					g_pDemos[i].m_PleaseWait = new XImage;
					g_pDemos[i].m_PleaseWait->Load(g_pd3dDevice, pdib->m_pszPersist);
				}
				else
				{
					g_pDemos[i].m_PleaseWait = &g_PleaseWait;
				}

				//
				// do text
				//
				if (true)
				{
					g_pDemos[i].m_Title = DuplicateAsciiToUnicode(pdib->m_pszTitle);
				}

				if (true)
				{
					if ((pdib->m_pszDemoType) && (pdib->m_pszDemoType[0]))
						g_pDemos[i].m_Type = DuplicateAsciiToUnicode(pdib->m_pszDemoType);
					else
						g_pDemos[i].m_Type = DuplicateAsciiToUnicode((pdib->m_dwFlags & DIBFLAGS_MOVIE) ? "Non-Interactive Movie" : "Interactive Demo");
				}

				if (true)
				{
					char *p = FindPath(pdib->m_pszXBE);

					g_pDemos[i].m_szFile = new char[strlen(p) + 1];
					strcpy(g_pDemos[i].m_szFile, p);
				}

				i++;
			}

			pdib = pdib->m_pNext;
		}
	}

	return S_OK;
}

//=============================================================================
// HandleTimingMode()
//
// If we're in timing mode, this function checks to see if enough time
// has elapsed since the last user input, and if so, plays the next movie
// that it's supposed to.
//=============================================================================
void HandleTimingMode(void)
{
	if (g_bTimingMode && g_pDemos)
	{
		// is it time to play a movie?
		if (timeGetTime() >= g_dwVideoTime)
		{
			while (true)
			{
				int iDemo=g_iNextVideo;
				g_iNextVideo++;

				if (g_iNextVideo >= g_pXDIHeader->m_lNumDemos)
					g_iNextVideo = 0;

				// make sure we found a movie
				if (g_pDemos[iDemo].m_pDib->m_dwFlags & DIBFLAGS_KIOSK)
				{
					if (ExecuteDemo(g_pDemos[iDemo], true))
					{
						g_dwVideoTime = timeGetTime() + VIDEO_DELAY;
						return;
					}

					// reset the timer
					Input();
				}

				if (g_iNextVideo==0)
					break;
			}

			g_dwVideoTime = timeGetTime() + VIDEO_DELAY;
		}
	}
}

//=============================================================================
//=============================================================================
HRESULT InitBackgroundMovie(void)
{
	g_hBkgndMovie = BinkOpen(FindPath(g_pszBackgroundMovie), 0);
	if (!g_hBkgndMovie)
		return S_OK;
	
	g_pd3dDevice->CreateTexture(g_hBkgndMovie->Width, g_hBkgndMovie->Height, 1, 0, D3DFMT_LIN_X8R8G8B8, 0, &g_pBkgndTexture);

	return S_OK;

}

//=============================================================================
//=============================================================================
void UpdateBackgroundMovie(void)
{
	static bool bFirst=true;

	if (bFirst)
	{
		bFirst=false;
		return;
	}

	if (g_hBkgndMovie)
	{
		if (BinkWait(g_hBkgndMovie))
		{
			// we don't need a new frame yet
		}
		else
		{
			// we need a new frame!!
			BinkDoFrame(g_hBkgndMovie);

			D3DLOCKED_RECT lr;

			if (true)
			{
				g_pBkgndTexture->LockRect(0, &lr, NULL, 0);
				BinkCopyToBuffer(g_hBkgndMovie, lr.pBits, lr.Pitch, g_hBkgndMovie->Height, 0, 0, BINKCOPYALL | BINKSURFACE32);
				g_pBkgndTexture->UnlockRect(0);
			}

			if (g_hBkgndMovie->FrameNum == (g_hBkgndMovie->Frames-1))
			{
				BinkGoto(g_hBkgndMovie, 1, 0);
			}
			else
				BinkNextFrame(g_hBkgndMovie);  // skip to the next frame
		}

		// Blt the image

		static struct {
			float	x,y,z,w;
			float	u,v;
		} vertices[4] = {
			{ 0,0,0,1,0,0 },
			{ 0,0,0,1,0,0 },
			{ 0,0,0,1,0,0 },
			{ 0,0,0,1,0,0 },
		};

		if (true)
		{
			if (true)
			{
				vertices[0].x	= 0.0f;
				vertices[0].y	= 0.0f;

				vertices[1].x	= WIDTH;
				vertices[1].y	= 0.0f;

				vertices[2].x	= 0.0f;
				vertices[2].y	= HEIGHT;

				vertices[3].x	= WIDTH;
				vertices[3].y	= HEIGHT;
			}

			if (true)
			{
				vertices[0].u	= -.5f + 0.0f;
				vertices[0].v	= -.5f + 0.0f;

				vertices[1].u	= -.5f + (float)g_hBkgndMovie->Width-10;
				vertices[1].v	= -.5f + 0.0f;

				vertices[2].u	= -.5f + 0.0f;
				vertices[2].v	= -.5f + (float)g_hBkgndMovie->Height-10;

				vertices[3].u	= -.5f + (float)g_hBkgndMovie->Width-10;
				vertices[3].v	= -.5f + (float)g_hBkgndMovie->Height-10;
			}
		}

		g_pd3dDevice->SetTexture(0, g_pBkgndTexture);
		g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(vertices[0]));
	}
}

//=============================================================================
//=============================================================================
LONG XDemosUnhandledExceptionFilter(LPEXCEPTION_POINTERS*)
{
	XLaunchNewImage("d:\\default.xbe", NULL);
	return 0;
}

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

#ifdef PROFILE
	DmEnableGPUCounter(TRUE);
#endif

	// various initialization
    if (FAILED(InitD3D()))
        return;

	if (FAILED(InitXDI()))
		return;

	if (FAILED(InitBackground()))
		return;

	if (FAILED(InitInput()))
		return;

	if (FAILED(InitAudio()))
		return;

	if (FAILED(InitBackgroundMovie()))
		return;

	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)XDemosUnhandledExceptionFilter);
#ifdef WMV
	TextureBufferQueue::InitializeD3D();
#endif

	// default 64K cache size is too low, set to 1MB
	XSetFileCacheSize(1024*1024);

	// get the state of the listbox (i.e., the last choice the user made
	GetState(g_iListChoice, g_iListOffset, g_iNextVideo);

	// error checking (this should never be necessary, as the XDI file
	// can't change on the DVD!
	if (g_iListChoice >= g_pXDIHeader->m_lNumDemos)
	{
		g_iListChoice = 0;
		g_iListOffset = 0;
	}

	// create the fonts that we will use
	g_pFontN = new CXBFont;
	g_pFontN->Create(g_pd3dDevice, FindPath(g_pszFontN));
	g_pFontB = new CXBFont;
	g_pFontB->Create(g_pd3dDevice, FindPath(g_pszFontB));

	// the main loop
    while (true)
    {
		HandleTimingMode();

		UpdateAudio();
		HandleInput();
        Render();
        g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    }
}

