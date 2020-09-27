/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    psverify.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __PSVERIFY_H__
#define __PSVERIFY_H__

//******************************************************************************
class CPSVerify : public CScene {

protected:

    CShaderGenerator*               m_pShaderGen;

    D3DPIXELSHADERDEF               m_d3dpsdGen;
    D3DPIXELSHADERDEF               m_d3dpsdAsm;
    LPXGBUFFER                      m_pxgbAsm;

    UINT                            m_uPSGenerated;
    UINT                            m_uPSAssemblySuccess;
    UINT                            m_uPSAssemblyFailure;
    UINT                            m_uPSAssemblyCorrect;
    UINT                            m_uPSCreatedSuccess;
    UINT                            m_uPSCreatedFailure;

public:

                                    CPSVerify();
                                    ~CPSVerify();

    virtual BOOL                    Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Prepare();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual BOOL                    VerifyShaderAssembly();
    virtual BOOL                    VerifyShaderCreation();

    virtual BOOL                    ComparePixelShaders(D3DPIXELSHADERDEF* pd3dpsd1, D3DPIXELSHADERDEF* pd3dpsd2);

    virtual void                    ProcessInput();
};

#endif //__PSVERIFY_H__
