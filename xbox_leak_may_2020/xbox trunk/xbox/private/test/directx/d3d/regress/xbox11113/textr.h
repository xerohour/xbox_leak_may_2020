/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    textr.h

Author:

    Matt Bronder

*******************************************************************************/

#ifndef __TEXTR_H__
#define __TEXTR_H__

//******************************************************************************
class CSample {

protected:

    LPDIRECT3D8                     m_pd3d;
    D3DSurface                      m_d3dsColorBuffer[3];
    D3DSurface                      m_d3dsDepthBuffer;
    LPVOID                          m_pvColorBuffer[3];
    LPVOID                          m_pvDepthBuffer;
    LPDIRECT3DDEVICE8               m_pDevice;

    LPDIRECT3DTEXTURE8              m_pd3dt;
    VERTEX                          m_prVertices[4];
    TLVERTEX                        m_prBackground[4];

    float                           m_fAspect;
    float                           m_fFieldOfView;

public:

                                    CSample();
                                    ~CSample();

    BOOL                            Create();
    BOOL                            Render();

protected:

	BOOL                            StartGraphics();
};

inline DWORD F2DW(float f) {return *((LPDWORD)&f);}

#endif //__TEXTR_H__
