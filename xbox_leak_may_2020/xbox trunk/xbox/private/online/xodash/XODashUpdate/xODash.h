//-----------------------------------------------------------------------------
// File: XODash.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBAPP_H
#define XBAPP_H

#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include "XBInput.h"
#include "XBUtil.h"
#include  "MsgDialog.h"



//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
extern LPDIRECT3DDEVICE8 g_pd3dDevice;


//
// Define the dash title ID
//
#define AUTOUPD_DASH_TITLE_ID            ((DWORD)0xfffe0000)

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)    (sizeof(a)/sizeof((a)[0]))
#endif

//-----------------------------------------------------------------------------
// Name: class XOApplication
// Desc: holds all really high level objects, sets up D3D
//          Initialize()          - To initialize the device-dependant objects
//          FrameMove()           - To animate the scene
//          Render()              - To render the scene
//-----------------------------------------------------------------------------
class XOApplication
{
protected:
    // Main objects used for creating and rendering the 3D scene
    D3DPRESENT_PARAMETERS m_d3dpp;
    LPDIRECT3D8           m_pD3D;              // The D3D enumerator object
    LPDIRECT3DDEVICE8     m_pd3dDevice;        // The D3D rendering device
    LPDIRECT3DSURFACE8    m_pBackBuffer;       // The backbuffer
    LPDIRECT3DSURFACE8    m_pDepthBuffer;      // The depthbuffer

    // Variables for timing
    float      m_fTime;             // Current absolute time in seconds
    float      m_fElapsedTime;      // Elapsed absolute time since last frame
	float      m_fStartTime;        // absolute start time in seconds
	float      m_fSecsPerTick;

	DWORD      m_dwLastUpdate;

    // Members to init the XINPUT devices.
    XDEVICE_PREALLOC_TYPE* m_InputDeviceTypes;
    DWORD                  m_dwNumInputDeviceTypes;
    XBGAMEPAD*             m_Gamepad;
    XBGAMEPAD              m_DefaultGamepad;

    // Overridable functions for the 3D scene created by the app
    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT Cleanup();

public:

    // Functions to create, run, and clean up the application
    HRESULT Create();
    INT     Run();
    VOID    Destroy();

    // Internal constructor
    XOApplication();

	float	GetTime(){ return m_fTime; }


private:
	HRESULT InitAudio();
	TCHAR					m_szAppDir[MAX_PATH];
	CProgressDialog m_ProgressBar;
	HRESULT CreateProgressBar();

	DWORD RebootWhilePreservingDDrive(
			    LPCSTR lpTitlePath,
    		    PLD_FROM_TITLE_UPDATE pldftu
			    );
};


extern XOApplication	theApp; // the single instance


#endif
