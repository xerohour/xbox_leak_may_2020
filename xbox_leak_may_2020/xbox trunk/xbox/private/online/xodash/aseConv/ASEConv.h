//-----------------------------------------------------------------------------
// File: XODash.h
//
// Container for XOnline's dash.  Stolen from ATG
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBAPP_H
#define XBAPP_H

#include "std.h"
#include "XBInput.h"
#include "translator.h"


class ASEScene;

//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
extern LPDIRECT3DDEVICE8 g_pd3dDevice;




//-----------------------------------------------------------------------------
// Name: class XOApplication
// Desc: holds all really high level objects, sets up D3D
//          Initialize()          - To initialize the device-dependant objects
//          FrameMove()           - To animate the scene
//          Render()              - To render the scene
//-----------------------------------------------------------------------------
class ASEConv
{
protected:
    // Main objects used for creating and rendering the 3D scene
    D3DPRESENT_PARAMETERS m_d3dpp;
    LPDIRECT3D8           m_pD3D;              // The D3D enumerator object
    LPDIRECT3DDEVICE8     m_pd3dDevice;        // The D3D rendering device
    LPDIRECT3DSURFACE8    m_pBackBuffer;       // The backbuffer
    LPDIRECT3DSURFACE8    m_pDepthBuffer;      // The depthbuffer

    // Variables for timing
    FLOAT      m_fTime;             // Current absolute time in seconds
    FLOAT      m_fElapsedTime;      // Elapsed absolute time since last frame
    FLOAT      m_fAppTime;          // Current app time in seconds
    FLOAT      m_fElapsedAppTime;   // Elapsed app time since last frame
    BOOL       m_bPaused;           // Whether app time is paused by user
    FLOAT      m_fFPS;              // Instanteous frame rate
    WCHAR      m_strFrameRate[20];  // Frame rate written to a string
    HANDLE     m_hFrameCounter;     // Handle to framerate perf counter

	ASEScene*			m_pScene;		// currently rendered scene


    // Members to init the XINPUT devices.
    XDEVICE_PREALLOC_TYPE* m_InputDeviceTypes;
    DWORD                  m_dwNumInputDeviceTypes;
    XBGAMEPAD*             m_Gamepad;
    XBGAMEPAD              m_DefaultGamepad;


    // Helper functions
    HRESULT RenderGradientBackground( DWORD dwTopColor, DWORD dwBottomColor );

    // Overridable functions for the 3D scene created by the app
    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT Cleanup();
	HRESULT Update();

public:

	TCHAR					m_szAppDir[MAX_PATH];

    ULONG                   curFile;

    // Functions to create, run, and clean up the application
    HRESULT Create();
    INT     Run();
    VOID    Destroy();

    // Internal constructor
    ASEConv();
    ~ASEConv();

	float	GetTime(){ return m_fAppTime; }

    void    GetFilesToOpen( const char* path, const char* pExtension );


private:

    struct  XBGFileData
    {
        char*   m_pAseName;
        char**   m_pTextButtonNames;
        long    m_nTextButtonCount;

    };

    XBGFileData*        m_ConvertedFiles;
    ULONG                m_nConvertedFileCount;
	
    HRESULT InitAudio();
};


extern ASEConv	theApp; // the single instance


#endif
