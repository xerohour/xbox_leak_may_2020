/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fsaa.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __FSAA_H__
#define __FSAA_H__

#define NUM_SPOKES  140

//******************************************************************************
class CMultiSuperSample : public CScene {

protected:

    LVERTEX                         m_prWhite[4];
    LVERTEX                         m_prWhiteBase[4];
    LVERTEX                         m_prSpokes[NUM_SPOKES+1];
    LVERTEX                         m_prSpokesBase[NUM_SPOKES+1];
    WORD                            m_pwSpokes[NUM_SPOKES*2];
    TLVERTEX                        m_prTextured[4];
    TLVERTEX                        m_prTexBase[4];

    CTexture8*                      m_pd3dt;

    D3DXVECTOR3                     m_vOffset;
    float                           m_fAngle;

    D3DPRESENT_PARAMETERS           m_d3dpp;

    UINT                            m_uRenderTarget;
    UINT                            m_uMultiSampleType;
    UINT                            m_uMultiSampleFormat;
    UINT                            m_uNumBackBuffers;
    UINT                            m_uPresentInterval;
    BOOL                            m_bReset;

    TCHAR                           m_szFSAADesc[2048];

public:

                                    CMultiSuperSample();
                                    ~CMultiSuperSample();

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__FSAA_H__
