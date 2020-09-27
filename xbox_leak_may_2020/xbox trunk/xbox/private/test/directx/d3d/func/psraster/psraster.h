/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    psraster.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __PSRASTER_H__
#define __PSRASTER_H__

#define FVF_PSVERTEX            (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX4 | \
                                 D3DFVF_TEXCOORDSIZE4(0) | \
                                 D3DFVF_TEXCOORDSIZE4(1) | \
                                 D3DFVF_TEXCOORDSIZE4(2) | \
                                 D3DFVF_TEXCOORDSIZE4(3))

//******************************************************************************
typedef struct _PSVERTEX {
    D3DXVECTOR3 vPosition;
    float       fRHW;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;
    D3DXVECTOR4 pvTCoord[4];
//    D3DXVECTOR4 vTCoord0;
//    D3DXVECTOR4 vTCoord1;
//    D3DXVECTOR4 vTCoord2;
//    D3DXVECTOR4 vTCoord3;

    _PSVERTEX() {}
    _PSVERTEX(const D3DVECTOR& v, float _fRHW, D3DCOLOR _cDiffuse, 
              D3DCOLOR _cSpecular, D3DXVECTOR4& _vTCoord0,
              D3DXVECTOR4& _vTCoord1, D3DXVECTOR4& _vTCoord2,
              D3DXVECTOR4& _vTCoord3) 
    {
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
        cDiffuse = _cDiffuse; cSpecular = _cSpecular;
        pvTCoord[0] = _vTCoord0; pvTCoord[1] = _vTCoord1; 
        pvTCoord[2] = _vTCoord2; pvTCoord[3] = _vTCoord3;
    }
} PSVERTEX, *PPSVERTEX;

//******************************************************************************
typedef struct _PSREGISTERS {
    D3DXVECTOR4 v0;
    D3DXVECTOR4 v1;
    D3DXVECTOR4 t0;
    D3DXVECTOR4 t1;
    D3DXVECTOR4 t2;
    D3DXVECTOR4 t3;
    D3DXVECTOR4 r0;
    D3DXVECTOR4 r1;
    D3DXVECTOR4 c0[9];
    D3DXVECTOR4 c1[9];
    D3DXVECTOR4 fog;
    D3DXVECTOR4 zero;
    D3DXVECTOR4 prod;
    D3DXVECTOR4 sum;
} PSREGISTERS, *PPSREGISTERS;

//******************************************************************************
class CPSRasterizer : public CScene {

protected:

    CShaderGenerator*               m_pShaderGen;
    DWORD                           m_dwInputRegisters;

    UINT                            m_uPSGenerated;
    UINT                            m_uPSRasterSuccess;
    UINT                            m_uPSRasterFailure;

    TCHAR                           m_szVariation[256];

    CSurface8*                      m_pd3dsTarget;

    PSREGISTERS                     m_reg;
    D3DPIXELSHADERDEF               m_d3dpsd;
    D3DPIXELSHADERDEF               m_d3dpsdDiffuse;
    DWORD                           m_dwDiffuseShader;

    float                           m_fBumpEnvMat[4][4];
    float                           m_fLumOffset[4];
    float                           m_fLumScale[4];

    CTexture8*                      m_pd3dtBase;
    CTexture8*                      m_pd3dtBump;
    CTexture8*                      m_pd3dtDiffuse;
    CCubeTexture8*                  m_pd3dtcNormal;
    CVolumeTexture8*                m_pd3dtvCloud;

    PSVERTEX                        m_prTriangle[3];
    PSVERTEX                        m_prTriangleR[3];

    float                           m_fThreshold;
    float                           m_fMaxMagSq;

//    D3DXVECTOR4                     m_vZero;
//    D3DXVECTOR4                     m_vOne;
//    D3DXVECTOR4                     m_vHalf;
//    float                           m_fRegNegOne;

    BOOL                            m_bGenerateShader;

public:

                                    CPSRasterizer();
                                    ~CPSRasterizer();

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual BOOL                    InitPixelShader(D3DPIXELSHADERDEF* pd3dpsd, LPDWORD pdwInputRegisters);

    virtual void                    DrawTriangle(PSVERTEX* prTriangle);
    virtual void                    ShadePixel(D3DCOLOR* pcPixel, PSVERTEX* prIPixel);
    virtual void                    TexturePixel(PSVERTEX* prIPixel);
    virtual D3DCOLOR                CombinePixel(PSVERTEX* prIPixel);
    virtual BOOL                    ComparePixels(D3DCOLOR cSrc, D3DCOLOR cRef);
    virtual D3DCOLOR                SampleTexture(DWORD dwStage, D3DXVECTOR4* pvTCoord, BOOL bCube, DWORD dwDotMap = PS_DOTMAPPING_ZERO_TO_ONE);

    virtual D3DXVECTOR4*            GetCombinerRegister(UINT uStage, DWORD dwPSIOReg, DWORD dwShift);
    virtual void                    SelectedChannelToColor(DWORD dwPSInReg, DWORD dwShift, D3DXVECTOR4* pvDst, D3DXVECTOR4* pvSrc);
    virtual void                    SelectedChannelToAlpha(DWORD dwPSInReg, DWORD dwShift, D3DXVECTOR4* pvDst, D3DXVECTOR4* pvSrc);
    virtual void                    MapInputRange(DWORD dwPSRGBInReg, DWORD dwPSAInReg, DWORD dwShift, D3DXVECTOR4* pvReg);
    virtual void                    MapOutputRange(DWORD dwPSRGBOutReg, DWORD dwPSAOutReg, D3DXVECTOR4* pvReg);

    virtual D3DCOLOR                GetBlendColor(BOOL bSource, D3DCOLOR cDstPixel, D3DCOLOR cSrcPixel);

    virtual BOOL                    SetShaderStates(D3DPIXELSHADERDEF* pd3dpsd);

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__PSRASTER_H__
