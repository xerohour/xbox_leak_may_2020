//=============================================================================
// File: XDemos.h
//
// Desc: This is the main header for the XDemos project
// Created: 07/02/2001 by Michael Lyons (mlyons@microsoft.com)
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================

#pragma once


//=============================================================================
// include files
//=============================================================================
#include <xtl.h>
#include <assert.h>
#include "xbfont.h"
#include "bink\bink.h"
#include "wmv\wmvplay.h"
#include "XDIFile\XDIFile.h"

//=============================================================================
// Global variables
//=============================================================================
extern LPDIRECT3D8			g_pD3D;
extern LPDIRECT3DDEVICE8	g_pd3dDevice;


//=============================================================================
// Global Functions
//=============================================================================
extern HRESULT InitAudio(void);
extern void UpdateAudio(void);
extern void StopAudio(void);
extern void StartAudio(void);
extern char *FindPath(const char *szRelPath);
extern void PlayLaunchSound(void);
extern void PlayMenuSound(void);
extern HRESULT InitInput(void);
extern void SaveState(int dw1, int dw2, int dw3);
extern void GetState(int &dw1, int &dw2, int &dw3);
extern void HandleInput(void);
extern bool PlayMovie(char *szFileName);

extern void ButtonUp(void);
extern void ButtonDn(void);
extern void ButtonMiddle(void);
extern void ButtonA(void);
extern void ButtonB(void);
extern void Input(void);
extern bool CheckForInput(void);


//=============================================================================
//=============================================================================
extern const char *			g_pszMenuSound;
extern const char *			g_pszLaunchSound;
extern const char *			g_pszAmbientSound;
extern const char *			g_pszSettingsFile;
extern const char *			g_pszBackground;
extern const char *			g_pszWait;
extern const char *			g_pszUp;
extern const char *			g_pszDn;
extern const char *			g_pszXDI1;
extern const char *			g_pszXDI2;
extern const char *			g_pszFontN;
extern const char *			g_pszFontB;
extern DWORD                g_dwNextButton;

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
typedef struct tagCUSTOMVERTEX
{
    float x,y,z,w;
    float u,v;
} CUSTOMVERTEX;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)



//========================================================================
//========================================================================
template<class T> __forceinline void AllocString(T * &ptr, size_t len)
{
	ptr = (T *)malloc(len * sizeof(T));
}

//========================================================================
//========================================================================
__forceinline WCHAR * DuplicateAsciiToUnicode(const char *src)
{
	WCHAR *dst;

	//
	// now, get the size of the buffer required, and allocate it
	//
	int iSize;

	iSize=(strlen(src) + 1)*2;

	if (!iSize)
		return NULL;
	
	AllocString(dst, iSize);
	if (!dst)
		return NULL;

	WCHAR *d = dst;
	while (*src)
	{
		*d++ = (WCHAR)*src++;
	}
	*d = 0;

	return dst;
}

