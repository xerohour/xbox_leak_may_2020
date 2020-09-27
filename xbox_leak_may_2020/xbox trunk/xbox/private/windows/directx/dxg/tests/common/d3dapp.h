//-----------------------------------------------------------------------------
// File: D3DApp.h
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DAPP_H
#define D3DAPP_H

#include "bitfont.h"

interface IDirectInput8 ;
interface IDirectInputDevice8;

//-----------------------------------------------------------------------------
// Structure for holding information about an adapter, and its display modes.
//-----------------------------------------------------------------------------

struct Adapter
{
	struct Mode
	{
		// Mode data
		DWORD      Width;
		DWORD      Height;
		D3DFORMAT  Format;
		DWORD      dwBehavior;
	};
	
	// Adapter data
	D3DADAPTER_IDENTIFIER8 AdapterIdentifier;
	D3DCAPS8   Caps;

	// Mode data
	DWORD      dwNumModes;
	Mode       modes[150];

	Adapter()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

//-----------------------------------------------------------------------------
// Structure used to define menus.
//-----------------------------------------------------------------------------

enum MenuTypes
{
	MT_NONE = 0,			// no menu button is pressed
	MT_BUTTON7 = 1,			// top left
	MT_BUTTON8 = 2,			// top right
	MT_BUTTON9 = 3,			// under left
	MT_BUTTON10 = 4,		// under right
	MT_STARTBUTTON1 = 5,	// reserved for the framework menu
	MT_STARTBUTTON2 = 6,	// reserved for the framework menu
	MT_MAX
};

struct ScenarioMenu
{

	MenuTypes m_MenuType;

	const WCHAR *m_szMenuName;

	const WCHAR *m_szActions[6]; // for A, B, C, D, E, F
	unsigned m_cActions;        // Number of filled actions.
};

//-----------------------------------------------------------------------------
// Name: class CD3DApplication8
// Desc: A base class for creating sample D3D8 applications. To create a simple
//       Direct3D application, simply derive this class into a class (such as
//       class CMyD3DApplication) and override the following functions, as 
//       needed:
//          OneTimeSceneInit()    - To initialize app data (alloc mem, etc.)
//          InitDeviceObjects()   - To initialize the 3D scene objects
//          FrameMove()           - To animate the scene
//          Render()              - To render the scene
//          DeleteDeviceObjects() - To cleanup the 3D scene objects
//          FinalCleanup()        - To cleanup app data (for exitting the app)
//-----------------------------------------------------------------------------

class CD3DApplication
{
	// Internal variables for the state of the app
    BOOL    m_bActive;
    BOOL    m_bReady;

    // Internal variables used for timing
    BOOL    m_bFrameMoving;
    BOOL    m_bSingleStep;
	BOOL    m_bRunFullTest;

	// FPS stats.
    FLOAT             m_fFPS;              // Instanteous frame rate
	WCHAR  m_strFrameStats[80];			   // String to hold frame rate stats
    DWORD m_dwFrames;

#if defined(PCBUILD)
	// Internal members used to manage input.
	IDirectInput8 *m_pDirectInput;
	IDirectInputDevice8 *m_pKeyboard;
	IDirectInputDevice8 *m_pGamepad;
#else
    HANDLE  m_hDevice;
#endif 

	BOOL    m_fShowHelp;				// Show a help menu.
	BOOL    m_fShowStats;               // Show statistics?
	DWORD	m_iActiveMenu;				// Which menu is currently active
	DWORD   m_iActiveKey;				// Which of ABCDEF is currently pressed?
	
	// Internal members for the menus.
	unsigned m_cMenus;					// How many menus are installed?

	ScenarioMenu *m_rgpMenus[MT_MAX];	// Descriptor for each menu.

	DWORD m_dwSpaceHeight;				// The height of a blank.
	DWORD m_dwHelpWidth;				// Width of the help box.
	DWORD m_dwColumn1X;					// Relative X for the first column
	DWORD m_dwColumn2X;                 // Relative X for the second column

	// Member used for output.  This variable is only valid for a short
	// period of time.
	//
	IDirect3DSurface8 *m_pBackBuffer;
	
    // Internal functions to manage and render the 3D scene
	void BuildDeviceList();

#if defined(PCBUILD)
	void GetInputDevices();
#endif 

    void UserChangeDevice();
    void Initialize3DEnvironment();
	void Resize3DEnvironment();
	void ToggleFullscreen();
    void Cleanup3DEnvironment();
    void Render3DEnvironment();
	bool ProcessInput();
	void InitializeHelpInformation();
	void ShowHelp(IDirect3DSurface8 *pSurface);
	void UpdateAllStats();

protected:

	// The adapter.
	Adapter          m_Adapter;

    // Main objects used for creating and rendering the 3D scene
	D3DPRESENT_PARAMETERS	m_Presentation;
    LPDIRECT3D8				m_pD3D;              // The D3D enumerator object
    LPDIRECT3DDEVICE8		m_pDevice;           // The D3D rendering device
    D3DCAPS8				m_Caps;              // Caps for the device
    D3DSURFACE_DESC			m_BackBufferDesc;    // Surface desc of the backbuffer
    HWND					m_hWnd;              // The main app window

	DWORD					m_dwSampleMask;      // Mask of available FSAA modes.
	D3DMULTISAMPLE_TYPE		m_iSampleType;       // Current sample mode.

    BitFont                 m_Font;

	// Variables for timing
    FLOAT             m_fTime;             // Current time in seconds
    FLOAT             m_fElapsedTime;      // Time elapsed since last frame

    // Overridable variables for the app
    BOOL      m_bUseDepthBuffer ;  // Whether to autocreate depthbuffer
    D3DFORMAT m_DepthBufferFormat; // Format used to create depthbuffer
	DWORD	  m_dwCurrentMode;	   // The current screen mode.
	WCHAR	  m_szTestStats[256];   // String to hold the test stats.

    // Overridable functions for the 3D scene created by the app
	virtual bool ConfirmDevice(D3DCAPS8*, DWORD, D3DFORMAT) { return true; }
    virtual void OneTimeSceneInit()                         { }
    virtual void InitDeviceObjects()                        { }
	virtual void RestoreDeviceObjects()                     { }
    virtual void FrameMove()                                { }
    virtual void Render()                                   { }
	virtual void InvalidateDeviceObjects()                  { }
    virtual void DeleteDeviceObjects()                      { }
    virtual void FinalCleanup()                             { }
	virtual void UpdateStats(float fElapsedTime)			{ }
	virtual void ShowInfo(int iY, int cHeight)              { }

	// Override this method to handle input. 
	virtual void ProcessKeyPress(DWORD iMenu, DWORD iKey)   { }

	// Override this method to set additional menus.  The
	// passed-in array has 10 elements, return the number 
	// of menus put into the array.
	//
	virtual unsigned GetMenus(ScenarioMenu **rgMenus)		{ return 0; }

	// Helper to draw the text on the screen.
    void DisplayText(const WCHAR *sz, int iY, DWORD dwColor);

	// Helper to reinitialize the 3d engine.
    void Change3DEnvironment();

public:

    // Functions to create, run, pause, and clean up the application
    virtual void Create(HINSTANCE hInstance);
    virtual void Run();

    // Internal constructor
    CD3DApplication();
};




#endif



