/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    obelisk.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __OBELISK_H__
#define __OBELISK_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

// Amount to be added to the camera rotation angle at each render
#define ROTATION_DELTA    (double)(M_PI / 100.0)

// Amount to shift the cloud texture on the reflection
#define CLOUD_DRIFT_DELTA     0.005f;

// Maximum amount to blend the reflection
#define MAX_ALPHA             0.8f

//******************************************************************************
class CObelisk : public CScene {

protected:

    D3DMATERIAL8                    m_matObeliskTip;
    D3DMATERIAL8                    m_matObeliskTrunk;
    D3DMATERIAL8                    m_pmatReflection[4];
    CTexture8*                      m_pd3dtGround;
    CTexture8*                      m_pd3dtClouds;
    CTexture8*                      m_pd3dtObelisk;
    VERTEX                          m_prObeliskTip[12];
    VERTEX                          m_prObeliskTrunk[16];
    VERTEX                          m_prReflection[16];
    VERTEX                          m_prGround[4];
    VERTEX                          m_prClouds[4];
    float                           m_fReflectionAlpha[4];
    float                           m_fAlpha;
    float                           m_fOffset;
    double                          m_dAngleOfRotation;

public:

                                    CObelisk();
                                    ~CObelisk();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__OBELISK_H__
