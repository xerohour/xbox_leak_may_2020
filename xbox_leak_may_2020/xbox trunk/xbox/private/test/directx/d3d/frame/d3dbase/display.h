/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    display.h

Author:

    Matt Bronder

Description:

    Direct3D initialization routines.

*******************************************************************************/

#ifndef __INIT_H__
#define __INIT_H__

#include "enum.h"
#include "input.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#if defined(_DEBUG) || defined(DBG)
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifndef UNDER_XBOX
#define POOL_DEFAULT    D3DPOOL_MANAGED
#else
#define POOL_DEFAULT    D3DPOOL_DEFAULT
#endif

#define KEY_LEFT                0x0001
#define KEY_RIGHT               0x0002
#define KEY_UP                  0x0004
#define KEY_DOWN                0x0008
#define KEY_ADD                 0x0010
#define KEY_SUBTRACT            0x0020
#define KEY_HOME                0x0040
#define KEY_END                 0x0080
#define KEY_INSERT              0x0100
#define KEY_DELETE              0x0200
#define KEY_PAGEUP              0x0400
#define KEY_PAGEDOWN            0x0800
#define KEY_SHIFT               0x1000

//******************************************************************************
// Data types
//******************************************************************************

typedef DWORD                   KEYSDOWN;

typedef BOOL                    (* SETDEVICEPROC)(LPVOID);
typedef void                    (* CLEARDEVICEPROC)(LPVOID);
typedef BOOL                    (* SUBWNDPROC)(LPVOID, LRESULT*, HWND, UINT, WPARAM, LPARAM);

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
typedef struct _DISPLAYDESC {
    PADAPTERDESC        padpd;
    PDEVICEDESC         pdevd;
    PDISPLAYMODE        pdm;
    UINT                uNumBackBuffers;
    BOOL                bWindowed;
    BOOL                bDepthBuffer;
    BOOL                bAntialias;
} DISPLAYDESC, *PDISPLAYDESC;

//******************************************************************************
typedef struct _D3DCREATIONDESC {
    D3DDEVTYPE          d3ddt;
    BOOL                bWindowed;
    D3DDISPLAYMODE      d3ddm;
    RECT                rectWnd;
    BOOL                bDepthBuffer;
    D3DFORMAT           fmtd;
    BOOL                bAntialias;
    D3DMULTISAMPLE_TYPE mst;
    UINT                uNumBackBuffers;
    UINT                uPresentInterval;
    DWORD               dwBehavior;
} D3DCREATIONDESC, *PD3DCREATIONDESC;

//******************************************************************************
typedef struct _CAMERA {
	D3DXVECTOR3         vPosition;
	D3DXVECTOR3         vInterest;
	float               fRoll;
	float               fFieldOfView;
	float               fNearPlane;
	float               fFarPlane;
} CAMERA, *PCAMERA;

//******************************************************************************
// CDisplay
//******************************************************************************

//******************************************************************************
class CDisplay : public CObject {

public:

    BOOL                        m_bTimeSync;

protected:

    LPDIRECT3D8                 m_pDirect3D;
    LPDIRECT3DDEVICE8           m_pDevice;

    PADAPTERDESC                m_padpdList;
    DISPLAYDESC                 m_disdCurrent;

    HWND                        m_hWnd;
    RECT                        m_rectWnd;
    D3DPRESENT_PARAMETERS       m_d3dpp;
    D3DSURFACE_DESC             m_d3dsdBack;

    BOOL                        m_bActive;

    KEYSDOWN                    m_kdKeys;
    JOYSTATE                    m_jsJoys;
    JOYSTATE                    m_jsLast;

    CAMERA                      m_cam;
    float                       m_fCameraTDelta;
    float                       m_fCameraRDelta;

    BOOL                        m_bShowConsole;

    RECT                        m_rectBorder;

    BOOL                        m_bUseInput;
    BOOL                        m_bSwapSticks;

    DISPLAYDESC                 m_disdSelect;

    SETDEVICEPROC               m_pfnSetDevice;
    CLEARDEVICEPROC             m_pfnClearDevice;
    SUBWNDPROC                  m_pfnSubWndProc;

    LPVOID                      m_pvSetDeviceParam;
    LPVOID                      m_pvClearDeviceParam;
    LPVOID                      m_pvSubWndProcParam;

    static UINT                 m_uCreated;
    BOOL                        m_bCreated;

public:

                                CDisplay();
                                ~CDisplay();

    virtual BOOL                Create(PD3DCREATIONDESC pd3dcd = NULL);
    virtual BOOL                Reset(D3DPRESENT_PARAMETERS* pd3dpp);
    virtual BOOL                ResetDevice();

    virtual BOOL                Update(PDISPLAYMODE pdm, BOOL bWindowed, 
                                    BOOL bDepthBuffer, BOOL bAntialias);
    virtual void                Select();

    virtual void                EnableInput(BOOL bEnable);

    virtual void                ProcessInput();

    virtual void                EnableConsoleVisibility(BOOL bEnable);
    virtual void                ShowConsole();

    virtual BOOL                SetView(PCAMERA pcam = NULL);

    virtual void                InitSetDeviceProc(SETDEVICEPROC pfnSetDevice, LPVOID pvParam);
    virtual void                InitClearDeviceProc(CLEARDEVICEPROC pfnClearDevice, LPVOID pvParam);
    virtual void                InitSubWindowProc(SUBWNDPROC pfnSubWndProc, LPVOID pvParam);

    virtual LPDIRECT3D8         GetDirect3D8();
    virtual LPDIRECT3DDEVICE8   GetDevice8();
    virtual void                GetCurrentDisplay(PDISPLAYDESC pdisd);
    virtual PADAPTERDESC        GetDisplayList();
    virtual BOOL                IsActive();
    virtual HWND                GetWindow();
    virtual void                GetPresentParameters(D3DPRESENT_PARAMETERS* pd3dpp);
    virtual UINT                GetWidth();
    virtual UINT                GetHeight();
    virtual KEYSDOWN            GetKeyState();
    virtual void                GetJoyState(PJOYSTATE pjsCurrent, PJOYSTATE pjsLast);
    virtual void                GetCamera(PCAMERA pcam);
    virtual void                SetCamera(PCAMERA pcam);
    virtual void                SetCameraOffsets(float fTranslation, float fRotation);
    virtual BOOL                ConsoleVisible();
    virtual BOOL                IsWindowed();
    virtual BOOL                IsDepthBuffered();

protected:

    LRESULT                     WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL                        DisplayDlgProc(HWND hDlg, UINT message, WPARAM wParam, 
                                    LPARAM lParam);

    BOOL                        CreateDevice(PADAPTERDESC padpd, PDEVICEDESC pdevd, 
                                    PDISPLAYMODE pdm, UINT uNumBackBuffers, BOOL bWindowed, 
                                    BOOL bDepthBuffer, BOOL bAntialias);

    BOOL                        InitDeviceState();
    BOOL                        SetViewport(DWORD dwWidth, DWORD dwHeight);

    void                        UpdateCamera(PCAMERA pcam);

    BOOL                        MoveWindowTarget(int x, int y);
    BOOL                        SizeWindowTarget(int width, int height);

    HMENU                       CreateMainMenu();

    void                        AddDlgItemTemplate(LPWORD* ppw, WORD wClass, 
                                    LPWSTR wszText, WORD wId, short x, short y, 
                                    short cx, short cy, DWORD dwStyle);

    friend LRESULT CALLBACK     WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    friend BOOL CALLBACK        DisplayDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, 
                                    LPARAM lParam);
};

#endif //__INIT_H__
