/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    psgen.h

Author:

    Matt Bronder

Description:

    Direct3D shader generator.

*******************************************************************************/

#ifndef __PSGEN_H__
#define __PSGEN_H__

#define XVECTOR4    D3DXVECTOR4

// Pixel shader register flags
#define PSREG_ZERO          0x0001
#define PSREG_C0            0x0002
#define PSREG_C1            0x0004
#define PSREG_FOG           0x0008
#define PSREG_V0            0x0010
#define PSREG_V1            0x0020
#define PSREG_T0            0x0040
#define PSREG_T1            0x0080
#define PSREG_T2            0x0100
#define PSREG_T3            0x0200
#define PSREG_R0            0x0400
#define PSREG_R1            0x0800
#define PSREG_V1R0_SUM      0x1000
#define PSREG_EF_PROD       0x2000

//******************************************************************************
class CShaderGenerator {

protected:

    DWORD                           m_dwPSConstant[16];
    DWORD                           m_dwValidInputsRGB;
    DWORD                           m_dwValidInputsA;

    float                           m_fCascadeFrequency;

    LPSTR                           m_szAsm;
    UINT                            m_uAsmSize;
    UINT                            m_uAsmLength;

    LPSTR                           m_szDef;
    UINT                            m_uDefSize;
    UINT                            m_uDefLength;

public:

                                    CShaderGenerator();
                                    ~CShaderGenerator();

    BOOL                            GeneratePixelShader(D3DPIXELSHADERDEF* pd3dpsd, LPXGBUFFER* pxgbAsm = NULL, DWORD* pdwInputRegisters = NULL);

    void                            SetCascadeFrequency(float fFrequency);
    void                            Seed(DWORD dwSeed);

    void                            OutputPixelShaderDef(D3DPIXELSHADERDEF* pd3dpsd, LPXGBUFFER* pxgbDef);

protected:

    void                            GeneratePSConstants(D3DPIXELSHADERDEF* pd3dpsd);
    void                            GeneratePSTexAddress(D3DPIXELSHADERDEF* pd3dpsd);
    void                            GeneratePSCombiners(D3DPIXELSHADERDEF* pd3dpsd);
    void                            GeneratePSCombinersAsm(D3DPIXELSHADERDEF* pd3dpsd);

    void                            AppendCode(LPCSTR szCode, ...);
    void                            AppendDef(LPCSTR szDef, ...);

    inline void                     OutputError(LPCSTR szError);
};

#endif //__PSGEN_H__
