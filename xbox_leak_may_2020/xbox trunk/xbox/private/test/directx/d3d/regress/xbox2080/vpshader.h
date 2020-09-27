/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vpshader.h

Author:

    Matt Bronder

Description:

    Basic vertex and pixel shader test.

*******************************************************************************/

#ifndef __VPSHADR_H__
#define __VPSHADR_H__

//******************************************************************************
class CVPShader : public CScene {

protected:

    CTexture8*                      m_pd3dtSphere;
    CTexture8*                      m_pd3dtBack;
    CVertexBuffer8*                 m_pd3drSphere;
    CIndexBuffer8*                  m_pd3diSphere;
    PVERTEX                         m_prSphere;
    DWORD                           m_dwSphereVertices;
    LPWORD                          m_pwSphere;
    DWORD                           m_dwSphereIndices;
    D3DLIGHT8                       m_light;
    D3DMATERIAL8                    m_material;
    DWORD                           m_dwVShader1;
    DWORD                           m_dwPShader1;

public:

                                    CVPShader();
                                    ~CVPShader();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

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

#endif //__VPSHADR_H__
