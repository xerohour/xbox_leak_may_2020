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


//////////////////////////////////////////////////////////////////////////////
// CD3DXAssignment ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CD3DXAssignment
{
public:
    CD3DXAssignment();
   ~CD3DXAssignment();

    HRESULT Initialize(CD3DXEffect *pEffect, CEffectNode *pAssignment, 
        CD3DXAssignment *pAsgns);

    BOOL IsParameterUsed(DWORD Name);

    HRESULT Validate(D3DCAPS8 *pCaps);
    HRESULT Apply(LPDIRECT3DDEVICE8 pDevice);

public:
    D3DXPARAMETERTYPE   m_Type;
    DWORD               m_dwState;
    DWORD               m_dwIndex;
    CD3DXValue          m_Value;
    CD3DXParameter*     m_pParameter;
    DWORD               m_dwFlags;

    union
    {
        D3DLIGHT8*      m_pLight;
        D3DMATERIAL8*   m_pMaterial;
    };
};


//////////////////////////////////////////////////////////////////////////////
// CD3DXPass /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CD3DXPass
{
public:
    CD3DXPass();
   ~CD3DXPass();    
    
    HRESULT Initialize(CD3DXEffect *pEffect, CEffectNode *pPass);

    BOOL IsParameterUsed(DWORD Name);

    HRESULT Validate(D3DCAPS8 *pCaps);
    HRESULT Apply(BOOL bUseStateBlocks);

public:
    CD3DXEffect*      m_pEffect;
    LPDIRECT3DDEVICE8 m_pDevice;
    D3DXPASS_DESC     m_Desc;
    DWORD             m_dwState;

    UINT              m_cAssignment;
    CD3DXAssignment*  m_pAssignment;
};


//////////////////////////////////////////////////////////////////////////////
// CD3DXTechnique ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CD3DXTechnique : public ID3DXTechnique
{
public:
    CD3DXTechnique();
   ~CD3DXTechnique();

    HRESULT Initialize(CD3DXEffect *pEffect, CEffectNode *pTechnique);


    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    // ID3DXTechnique
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice);
    STDMETHOD(GetDesc)(THIS_ D3DXTECHNIQUE_DESC* pDesc);
    STDMETHOD(GetPassDesc)(THIS_ UINT Index, D3DXPASS_DESC* pDesc);

    STDMETHOD_(BOOL, IsParameterUsed)(THIS_ DWORD Name);

    STDMETHOD(Validate)(THIS);
    STDMETHOD(Begin)(THIS_ UINT *pPasses);
    STDMETHOD(Pass)(THIS_ UINT Index);
    STDMETHOD(End)(THIS);


public:
    UINT                m_uRefCount;
    CD3DXEffect*        m_pEffect;
    LPDIRECT3DDEVICE8   m_pDevice;
    CEffectNode*        m_pTechnique;
    D3DXTECHNIQUE_DESC  m_Desc;
    CD3DXPass*          m_pPass;
    DWORD               m_dwClear;
};

#endif //__CD3DXTECHNIQUE_H__

