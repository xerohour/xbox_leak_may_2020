/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

//******************************************************************************
// Function prototypes
//******************************************************************************

int         ExhibitScene(CDisplay* pDisplay);
BOOL        ValidateDisplay(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, 
                            D3DDISPLAYMODE* pd3ddm);

//******************************************************************************
class CTest : public CScene {

protected:

    CVertexBuffer8*                 m_pd3dr;
    TLVERTEX                        m_prTriangle[3];

public:

                                    CTest();
                                    ~CTest();

protected:

    virtual BOOL                    Initialize();
    virtual BOOL                    Render();
};

#endif //__TEST_H__
