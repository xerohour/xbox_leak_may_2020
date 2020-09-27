/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    scene.h

Author:

    Matt Bronder

Description:

    Direct3D scene functions.

*******************************************************************************/

#ifndef __SCENE_H__
#define __SCENE_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define FLT_INFINITE                3.402823466e+38F //FLT_MAX

#ifdef UNDER_XBOX
#define D3D_SECTION_NAME            ".D3D$TEST"
#endif // UNDER_XBOX

//******************************************************************************
// Forward declarations
//******************************************************************************

class CScene;

//******************************************************************************
// Data types
//******************************************************************************

typedef void (CScene::* FADEPROC)(void);

#ifdef UNDER_XBOX

typedef BOOL (*VALIDATEPROC)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm);
typedef int  (*EXHIBITPROC)(CDisplay* pDisplay, int *pnExitCode);

typedef struct _D3DTESTFUNCTIONDESC {
    LPTSTR          szModule;
    VALIDATEPROC    pfnValidate;
    EXHIBITPROC     pfnExhibit;
} D3DTESTFUNCTIONDESC, *PD3DTESTFUNCTIONDESC;

extern D3DTESTFUNCTIONDESC D3D_BeginTestFunctionTable;
extern D3DTESTFUNCTIONDESC D3D_EndTestFunctionTable;

#endif // UNDER_XBOX

//******************************************************************************
class CScene : public CObject {

private:

    D3DPRESENT_PARAMETERS           m_d3dppInitial;
    BOOL                            m_bRestoreMode;

protected:

    CDisplay*                       m_pDisplay;
    CDevice8*                       m_pDevice;

    BOOL                            m_bDisplayOK;

    BOOL                            m_bPaused;
    BOOL                            m_bAdvance;
    static float                    m_fTimeDilation;
    float                           m_fFPS;
    static BOOL                     m_bShowFPS;
    static BOOL                     m_bShowMode;
    TCHAR                           m_szDisplayMode[64];

    DWORD                           m_dwInitialState;

    CAMERA                          m_camInitial;

    D3DPRESENT_PARAMETERS           m_d3dpp;

    float                           m_fFrameDuration;
    float                           m_fTimeDuration;
    float                           m_fTime;
    float                           m_fLastTime;
    float                           m_fTimeDelta;
    float                           m_fFrame;
    float                           m_fLastFrame;
    float                           m_fFrameDelta;

#ifdef UNDER_XBOX
    BOOL                            m_bQuit;
#endif // UNDER_XBOX

    JOYSTATE                        m_jsJoys;
    JOYSTATE                        m_jsLast;

    D3DXMATRIX                      m_mIdentity;

    TLVERTEX                        m_prBackground[4];

    BOOL                            m_bFade;
    FADEPROC                        m_pfnFade;
    TLVERTEX                        m_prFade[4];
    BYTE                            m_fadeAlpha;

    static UINT                     m_uCreated;
    BOOL                            m_bCreated;

public:

                                    CScene();
                                    ~CScene();

    virtual ULONG                   Release();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual BOOL                    Exhibit(int *pnExitCode);

    virtual void                    SetFrameDuration(float fNumFrames);
    virtual void                    SetTimeDuration(float fSeconds);

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();
    virtual BOOL                    Reset();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);

    virtual void                    ShowFrameRate();
    virtual void                    ShowDisplayMode();
    virtual void                    InitDisplayModeString();

    virtual void                    FadeOut();
    virtual void                    ApplyFade();

    friend BOOL                     InitScene(LPVOID pvParam);
    friend void                     EffaceScene(LPVOID pvParam);

    friend BOOL                     SceneWndProc(LPVOID pvParam, LRESULT* plr, 
                                             HWND hWnd, UINT uMsg, 
                                             WPARAM wParam, LPARAM lParam);
};

#endif //__SCENE_H__
