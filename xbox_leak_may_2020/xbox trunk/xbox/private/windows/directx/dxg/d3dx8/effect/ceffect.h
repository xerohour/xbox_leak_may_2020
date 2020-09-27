///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXEffect.h
//  Content:    D3DX effect types and functions
//
///////////////////////////////////////////////////////////////////////////

#ifndef __CD3DXEFFECT_H__
#define __CD3DXEFFECT_H__



//////////////////////////////////////////////////////////////////////////////
// CEffectNode ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CEffectNode
{
public:
    DWORD m_dwId;
    DWORD m_cdwData;
    DWORD m_cdwNodes;
    DWORD m_dwData[1];

public:
    CEffectNode* Children();
    CEffectNode* Sibling(CEffectNode *pChild);
};



///////////////////////////////////////////////////////////////////////////
// CD3DXValue /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXValue
{
public:
    D3DXPARAMETERTYPE m_Type;

    union
    {
        DWORD                   m_Dword;
        FLOAT                   m_Float;
        D3DXVECTOR4*            m_pVector;
        D3DXMATRIX*             m_pMatrix;
        LPDIRECT3DBASETEXTURE8  m_pTexture;
        DWORD                   m_VertexShader;
        DWORD                   m_PixelShader;
        DWORD*                  m_pConstant;
    };

public:
    CD3DXValue();
   ~CD3DXValue();

    HRESULT Initialize(CD3DXEffect* pEffect, D3DXPARAMETERTYPE Type);
    HRESULT Initialize(CD3DXEffect* pEffect, CEffectNode* pValue);

    HRESULT Cast(CD3DXValue *pValue);
};



///////////////////////////////////////////////////////////////////////////
// CD3DXParameter /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXParameter
{
public:
    DWORD       m_dwName;
    CD3DXValue  m_Value;

public:
    CD3DXParameter();
   ~CD3DXParameter();

    HRESULT Initialize(CD3DXEffect* pEffect, CEffectNode* pParameter);
};



//////////////////////////////////////////////////////////////////////////////
// CD3DXEffect ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CD3DXEffect : public ID3DXEffect
{
public:
    CD3DXEffect();
   ~CD3DXEffect();

    HRESULT Initialize(LPDIRECT3DDEVICE8 pDevice, LPCVOID pData, UINT cbData, DWORD Usage);
    CD3DXParameter* FindParameter(D3DXPARAMETERTYPE Type, DWORD dwName);

    ULONG AddRefTechnique();
    ULONG ReleaseTechnique();


    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID* ppv);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    // ID3DXEffect
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice);
    STDMETHOD(GetDesc)(THIS_ D3DXEFFECT_DESC* pDesc);

    STDMETHOD(GetParameterDesc)(THIS_ UINT Index, D3DXPARAMETER_DESC* pDesc);
    STDMETHOD(GetTechniqueDesc)(THIS_ UINT Index, D3DXTECHNIQUE_DESC* pDesc);

    STDMETHOD(SetDword)(THIS_ DWORD Name, DWORD dw);
    STDMETHOD(GetDword)(THIS_ DWORD Name, DWORD* pdw); 
    STDMETHOD(SetFloat)(THIS_ DWORD Name, FLOAT f);
    STDMETHOD(GetFloat)(THIS_ DWORD Name, FLOAT* pf);
    STDMETHOD(SetVector)(THIS_ DWORD Name, D3DXVECTOR4* pVector);
    STDMETHOD(GetVector)(THIS_ DWORD Name, D3DXVECTOR4* pVector);
    STDMETHOD(SetMatrix)(THIS_ DWORD Name, D3DXMATRIX* pMatrix);
    STDMETHOD(GetMatrix)(THIS_ DWORD Name, D3DXMATRIX* pMatrix);
    STDMETHOD(SetTexture)(THIS_ DWORD Name, LPDIRECT3DBASETEXTURE8 pTexture);
    STDMETHOD(GetTexture)(THIS_ DWORD Name, LPDIRECT3DBASETEXTURE8 *ppTexture);
    STDMETHOD(SetVertexShader)(THIS_ DWORD Name, DWORD Handle);
    STDMETHOD(GetVertexShader)(THIS_ DWORD Name, DWORD* pHandle);
    STDMETHOD(SetPixelShader)(THIS_ DWORD Name, DWORD Handle);
    STDMETHOD(GetPixelShader)(THIS_ DWORD Name, DWORD* pHandle);

    STDMETHOD(GetTechnique)(THIS_ UINT Index, LPD3DXTECHNIQUE* ppTechnique);
    STDMETHOD(CloneEffect)(THIS_ LPDIRECT3DDEVICE8 pDevice, DWORD Usage, LPD3DXEFFECT* ppEffect);


public:
    UINT m_uRefCount;
    UINT m_uRefTechnique;

    LPDIRECT3DDEVICE8 m_pDevice;
    DWORD*            m_pData;
    D3DXEFFECT_DESC   m_Desc;
    CD3DXParameter*   m_pParameter;
    CD3DXTechnique*   m_pTechnique;
};

#endif //__CD3DXEFFECT_H__

