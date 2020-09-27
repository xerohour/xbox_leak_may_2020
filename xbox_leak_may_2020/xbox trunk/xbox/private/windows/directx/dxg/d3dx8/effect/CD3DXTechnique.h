///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXTechnique.h
//  Content:    D3DX technique types and functions
//
///////////////////////////////////////////////////////////////////////////

#ifndef __CD3DXTECHNIQUE_H__
#define __CD3DXTECHNIQUE_H__


#define D3DRS_FVF 0x80000001 // For fixed-function pipe.. not really a D3DRS



class StateData
{
public:
    DWORD m_dwState;
    DWORD m_dwValue;
    StateData *m_pNext;

public:
    StateData();
   ~StateData();
};


class StageData
{
public:
    UINT m_uTexture;
    UINT m_uMatrix;

    StateData *m_pTSS;
    StageData *m_pNext;

public:
    StageData();
   ~StageData();
};


class VShaderData
{
public:
    DWORD m_pDeclaration[MAX_FVF_DECL_SIZE];
    ID3DXBuffer *m_pShader;

public:
    VShaderData();
   ~VShaderData();
};


class PShaderData
{
public:
    ID3DXBuffer *m_pDeclaration;
    ID3DXBuffer *m_pShader;

public:
    PShaderData();
   ~PShaderData();
};


class PassData
{
public:
    StateData   *m_pRS;
    StageData   *m_pStage;
    VShaderData *m_pVShader;
    PShaderData *m_pPShader;
    PassData    *m_pNext;

    DWORD m_dwState;
    DWORD m_dwVShader;
    DWORD m_dwPShader;

    IDirect3DDevice8 *m_pDevice;

public:
    PassData();
   ~PassData();
};


class TechniqueData
{
public:
    UINT   m_uPasses;
    UINT   m_uLOD;
    UINT   m_uRefCount;

    StateData     *m_pRS;
    PassData      *m_pPass;
    TechniqueData *m_pNext;

public:
    TechniqueData();
   ~TechniqueData();
};


class CD3DXTechnique;
class CD3DXEffect;


//////////////////////////////////////////////////////////////////////////////
// CD3DXTechnique ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CD3DXTechnique : public ID3DXTechnique
{
public:
    CD3DXTechnique();
   ~CD3DXTechnique();

    HRESULT Initialize(CD3DXEffect *pEffect, TechniqueData *pTechniqueData, 
                       DWORD dwClear, DWORD dwTextureUsed, DWORD dwMatrixUsed);

    HRESULT ValidateDX8(UINT uPass);


    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    // ID3DXTechnique
    STDMETHOD_(D3DXTECHNIQUE_DESC, GetTechniqueDesc)(THIS);
    STDMETHOD(GetDevice)(THIS_ IDirect3DDevice8 **ppDevice);

    STDMETHOD_(BOOL, IsDwordUsed)    (THIS_ UINT uIndex);
    STDMETHOD_(BOOL, IsFloatUsed)    (THIS_ UINT uIndex);
    STDMETHOD_(BOOL, IsColorUsed)    (THIS_ UINT uIndex);
    STDMETHOD_(BOOL, IsVectorUsed)   (THIS_ UINT uIndex);
    STDMETHOD_(BOOL, IsMatrixUsed)   (THIS_ UINT uIndex);
    STDMETHOD_(BOOL, IsTextureUsed)  (THIS_ UINT uIndex);

    STDMETHOD(Validate)(THIS);

    STDMETHOD(Begin)(THIS_ UINT *puPasses);
    STDMETHOD(Pass)(THIS_ UINT uPass);
    STDMETHOD(End)(THIS);


public:
    UINT m_uRefCount;
    DWORD m_dwClear;

    DWORD m_dwDwordUsed;
    DWORD m_dwFloatUsed;
    DWORD m_dwColorUsed;
    DWORD m_dwVectorUsed;
    DWORD m_dwMatrixUsed;
    DWORD m_dwTextureUsed;

    CD3DXEffect *m_pEffect;
    TechniqueData *m_pTechniqueData;
    IDirect3DDevice8 *m_pDevice;
};

#endif //__CD3DXTECHNIQUE_H__

