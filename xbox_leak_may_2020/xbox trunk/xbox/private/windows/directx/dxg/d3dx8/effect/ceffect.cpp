//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       effect.h
//  Content:    D3DX effect types and functions
//
//////////////////////////////////////////////////////////////////////////////

#include "pcheffect.h"


//////////////////////////////////////////////////////////////////////////////
// CEffectNode ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


CEffectNode*
CEffectNode::Children()
{
    if(!m_cdwNodes)
        return NULL;

    return (CEffectNode *) &m_dwData[m_cdwData];
}


CEffectNode* 
CEffectNode::Sibling(CEffectNode *pChild)
{
    DWORD* pdw = &pChild->m_dwData[pChild->m_cdwData + pChild->m_cdwNodes];

    if((pdw < &m_dwData[0]) || (pdw >= &m_dwData[m_cdwData + m_cdwNodes]))
        return NULL;

    return (CEffectNode *) pdw;
}



//////////////////////////////////////////////////////////////////////////////
// CD3DXValue ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXValue::CD3DXValue()
{
    m_Type = D3DXPT_DWORD;
    m_Dword = 0;
}


CD3DXValue::~CD3DXValue()
{
    switch(m_Type)
    {
    case D3DXPT_VECTOR:
        delete m_pVector;
        break;

    case D3DXPT_MATRIX:
        delete m_pMatrix;
        break;

    case D3DXPT_CONSTANT:
        delete [] m_pConstant;
        break;       
    }
}


HRESULT 
CD3DXValue::Initialize(CD3DXEffect* pEffect, D3DXPARAMETERTYPE Type)
{
    m_Type = Type;

    // Default values
    switch(m_Type)
    {
    case D3DXPT_DWORD:
        m_Dword = 0;
        break;

    case D3DXPT_FLOAT:
        m_Float = 0.0f;
        break;

    case D3DXPT_VECTOR:
        if(!(m_pVector = new D3DXVECTOR4))
            return E_OUTOFMEMORY;

        m_pVector->x = 0.0f;
        m_pVector->y = 0.0f;
        m_pVector->z = 0.0f;
        m_pVector->w = 1.0f;
        break;

    case D3DXPT_MATRIX:
        if(!(m_pMatrix = new D3DXMATRIX))
            return E_OUTOFMEMORY;

        D3DXMatrixIdentity(m_pMatrix);
        break;

    case D3DXPT_TEXTURE:
        m_pTexture = NULL;
        break;

    case D3DXPT_VERTEXSHADER:
        m_VertexShader = 0;
        break;

    case D3DXPT_PIXELSHADER:
        m_PixelShader = 0;
        break;

    case D3DXPT_CONSTANT:
        m_pConstant = NULL;
        break;
    }

    return S_OK;
}


HRESULT 
CD3DXValue::Initialize(CD3DXEffect* pEffect, CEffectNode* pValue)
{
    D3DXASSERT((pEffect != NULL) && (pValue != NULL));

    if(N_VALUE != pValue->m_dwId)
        return D3DXERR_INVALIDDATA;

    m_Type = (D3DXPARAMETERTYPE) pValue->m_dwData[0];

    switch(m_Type)
    {
    case D3DXPT_DWORD:
        m_Dword = pValue->m_dwData[1];
        break;

    case D3DXPT_FLOAT:
        m_Float = *((FLOAT *) &pValue->m_dwData[1]);
        break;

    case D3DXPT_VECTOR:
        if(!(m_pVector = new D3DXVECTOR4))
            return E_OUTOFMEMORY;

        memcpy(m_pVector, &pValue->m_dwData[1], sizeof(D3DXVECTOR4));
        break;

    case D3DXPT_MATRIX:
        if(!(m_pMatrix = new D3DXMATRIX))
            return E_OUTOFMEMORY;

        memcpy(m_pMatrix, &pValue->m_dwData[1], sizeof(D3DXMATRIX));
        break;

    case D3DXPT_TEXTURE:
        DPF(0, "Cannot initialized a texture");
        return D3DXERR_INVALIDDATA;

    case D3DXPT_VERTEXSHADER:
        if(FAILED(pEffect->m_pDevice->CreateVertexShader(
            &pValue->m_dwData[2], (pValue->m_cdwData - pValue->m_dwData[1] - 2) ? 
            &pValue->m_dwData[2 + pValue->m_dwData[1]] : NULL, &m_VertexShader,
            pEffect->m_Desc.Usage)))
        {
            m_VertexShader = 0;
        }
        break;

    case D3DXPT_PIXELSHADER:
        /*** HACK - kylej - cast to D3DPIXELSHADERDEF to fix a build break.  This will have to be 
             fixed eventually if we decide to support effects on Xbox ***/
        
        if(FAILED(pEffect->m_pDevice->CreatePixelShader(
            (D3DPIXELSHADERDEF *)(&pValue->m_dwData[2 + pValue->m_dwData[1]]), &m_PixelShader)))
        {
            m_PixelShader = 0;
        }
        break;

    case D3DXPT_CONSTANT:
        m_pConstant = NULL;
        break;
    }

    return S_OK;
}


HRESULT 
CD3DXValue::Cast(CD3DXValue *pValue)
{
    switch(m_Type)
    {
    case D3DXPT_DWORD:
        switch(pValue->m_Type)
        {
        case D3DXPT_DWORD:
            m_Dword = pValue->m_Dword;
            return S_OK;

        case D3DXPT_VECTOR:
            m_Dword = (DWORD) D3DXCOLOR((FLOAT *) pValue->m_pVector);
            return S_OK;
        }
        break;

    case D3DXPT_FLOAT:
        switch(pValue->m_Type)
        {
        case D3DXPT_FLOAT:
            m_Float = pValue->m_Float;
            return S_OK;
        }
        break;

    case D3DXPT_VECTOR:
        switch(pValue->m_Type)
        {
        case D3DXPT_DWORD:
            *((D3DXCOLOR *) m_pVector) = D3DXCOLOR(pValue->m_Dword);
            return S_OK;

        case D3DXPT_FLOAT:
            m_pVector->x =  m_pVector->y = m_pVector->z = m_pVector->w = pValue->m_Float;
            return S_OK;

        case D3DXPT_VECTOR:
            *m_pVector = *pValue->m_pVector;
            return S_OK;
        }
        break;

    case D3DXPT_MATRIX:
        switch(pValue->m_Type)
        {
        case D3DXPT_MATRIX:
            *m_pMatrix = *pValue->m_pMatrix;
            return S_OK;
        }
        break;

    case D3DXPT_TEXTURE:
        switch(pValue->m_Type)
        {
        case D3DXPT_TEXTURE:
            m_pTexture = pValue->m_pTexture;
            return S_OK;
        }
        break;

    case D3DXPT_VERTEXSHADER:
        switch(pValue->m_Type)
        {
        case D3DXPT_VERTEXSHADER:
            m_VertexShader = pValue->m_VertexShader;
            return S_OK;
        }
        break;

    case D3DXPT_PIXELSHADER:
        switch(pValue->m_Type)
        {
        case D3DXPT_PIXELSHADER:
            m_PixelShader = pValue->m_PixelShader;
            return S_OK;
        }
        break;

    case D3DXPT_CONSTANT:
        if(m_pConstant)
        {
            delete [] m_pConstant;
            m_pConstant = NULL;
        }

        switch(pValue->m_Type)
        {
        case D3DXPT_DWORD:
        case D3DXPT_FLOAT:
        case D3DXPT_VECTOR:
            if(!(m_pConstant = new DWORD[5]))
                return E_OUTOFMEMORY;

            m_pConstant[0] = 1;
            break;

        case D3DXPT_MATRIX:
            if(!(m_pConstant = new DWORD[17]))
                return E_OUTOFMEMORY;

            m_pConstant[0] = 4;
        }

        switch(pValue->m_Type)
        {
        case D3DXPT_DWORD:
            *((D3DXCOLOR *) &m_pConstant[1]) = D3DXCOLOR(pValue->m_Dword);
            return S_OK;

        case D3DXPT_FLOAT:
            m_pConstant[1] = m_pConstant[2] = m_pConstant[3] = m_pConstant[4] = *((DWORD *) &pValue->m_Float);
            return S_OK;

        case D3DXPT_VECTOR:
            *((D3DXVECTOR4 *) &m_pConstant[1]) = *pValue->m_pVector;
            return S_OK;

        case D3DXPT_MATRIX:
            *((D3DXMATRIX *) &m_pConstant[1]) = *pValue->m_pMatrix;
            return S_OK;
        }
        
        break;
    }

    return D3DXERR_INVALIDDATA;
}



//////////////////////////////////////////////////////////////////////////////
// CD3DXParameter ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


CD3DXParameter::CD3DXParameter()
{
    m_dwName = 0;
}


CD3DXParameter::~CD3DXParameter()
{
    if(D3DXPT_TEXTURE == m_Value.m_Type)
    {
        RELEASE(m_Value.m_pTexture);
    }
}


HRESULT
CD3DXParameter::Initialize(CD3DXEffect *pEffect, CEffectNode* pParameter)
{
    HRESULT hr;

    D3DXASSERT((pEffect != NULL) && (pParameter != NULL));


    // Parse header
    if(N_PARAMETER != pParameter->m_dwId)
        return D3DXERR_INVALIDDATA;

    m_dwName = pParameter->m_dwData[1];

    if(FAILED(hr = m_Value.Initialize(pEffect, (D3DXPARAMETERTYPE) pParameter->m_dwData[0])))
        return hr;


    // Initial value present?
    CEffectNode *pValue = pParameter->Children();

    if(pValue)
    {
        CD3DXValue Value;

        if(FAILED(hr = Value.Initialize(pEffect, pValue)))
            return hr;

        if(FAILED(hr = m_Value.Cast(&Value)))
            return hr;

        return S_OK;
    }

    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CD3DXEffect ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXEffect::CD3DXEffect()
{
    m_uRefCount     = 1;
    m_uRefTechnique = 0;

    m_pDevice       = NULL;
    m_pData         = NULL;
    m_pParameter    = NULL;
    m_pTechnique    = NULL;

    memset(&m_Desc, 0x00, sizeof(D3DXEFFECT_DESC));
}


CD3DXEffect::~CD3DXEffect()
{
    delete [] m_pData;
    delete [] m_pParameter;
    delete [] m_pTechnique;

    RELEASE(m_pDevice);
}


HRESULT
CD3DXEffect::Initialize(IDirect3DDevice8 *pDevice, LPCVOID pData, UINT cbData, DWORD Usage)
{
    HRESULT hr;

    // Validate args.
    D3DXASSERT(pDevice != NULL);
    D3DXASSERT(pData != NULL);
    D3DXASSERT(cbData  != 0);

    if(Usage != 0)
    {
        DPF(0, "Usage parameter is invalid");
        return D3DERR_INVALIDCALL;
    }
    m_pDevice = pDevice;
    m_pDevice->AddRef();


    // Create our own buffer and copy effect data into it.
    if(!(m_pData = new DWORD[(cbData + 3) >> 2]))
        return E_OUTOFMEMORY;

    memcpy(m_pData, pData, cbData);

    DWORD *pdw = m_pData;
    UINT cdw = (cbData + 3) >> 2;



    //
    // Parse header
    //

    CEffectNode *pEffect = (CEffectNode *) pdw;

    if(cdw < 3)
    {
        return D3DXERR_INVALIDDATA;
    }

    if(N_EFFECT != pEffect->m_dwId)
    {
        return D3DXERR_INVALIDDATA;
    }

    if(cbData != (3 + pEffect->m_cdwData + pEffect->m_cdwNodes) * sizeof(DWORD))
    {
        return D3DXERR_INVALIDDATA;
    }

    CEffectNode *pParameters = pEffect->Children();
    CEffectNode *pTechniques = pEffect->Sibling(pParameters);

    memset(&m_Desc, 0x00, sizeof(D3DXEFFECT_DESC));
    m_Desc.Usage = Usage;


    //
    // Parameters
    //

    UINT iParameter;
    CEffectNode* pParameter;

    // Count parameters
    pParameter = pParameters->Children();

    while(pParameter)
    {
        m_Desc.Parameters++;
        pParameter = pParameters->Sibling(pParameter);
    }

    if(!(m_pParameter = new CD3DXParameter[m_Desc.Parameters]))
        return E_OUTOFMEMORY;

    // Initialize parameters
    iParameter = 0;
    pParameter = pParameters->Children();

    while(pParameter)
    {
        if(FAILED(hr = m_pParameter[iParameter].Initialize(this, pParameter)))
            return hr;

        iParameter++;
        pParameter = pParameters->Sibling(pParameter);
    }


    //
    // Techniques
    //

    UINT iTechnique;
    CEffectNode* pTechnique;

    // Count techniques
    pTechnique = pTechniques->Children();

    while(pTechnique)
    {
        m_Desc.Techniques++;
        pTechnique = pTechniques->Sibling(pTechnique);
    }

    if(!(m_pTechnique = new CD3DXTechnique[m_Desc.Techniques]))
        return E_OUTOFMEMORY;

    // Initialize techniques
    iTechnique = 0;
    pTechnique = pTechniques->Children();

    while(pTechnique)
    {
        if(FAILED(hr = m_pTechnique[iTechnique].Initialize(this, pTechnique)))
            return hr;

        iTechnique++;
        pTechnique = pTechniques->Sibling(pTechnique);
    }


    return S_OK;
}



CD3DXParameter* 
CD3DXEffect::FindParameter(D3DXPARAMETERTYPE Type, DWORD dwName)
{
#if DBG
    static const char* szType[] =
        { "DWORD", "FLOAT", "VECTOR", "MATRIX", "TEXTURE", "VERTEXSHADER", "PIXELSHADER" };
#endif

    for(UINT iParameter = 0; iParameter < m_Desc.Parameters; iParameter++)
    {
        if(m_pParameter[iParameter].m_dwName == dwName)
        {
            if(m_pParameter[iParameter].m_Value.m_Type == Type)
                return &m_pParameter[iParameter];

            DPF(0, "Type mismatch: '%.4s' is a %s (not a %s)", &dwName, 
                szType[m_pParameter[iParameter].m_Value.m_Type], szType[Type]);

            return NULL;
        }
    }

    DPF(0, "Parameter not found: '%.4s'", &dwName);
    return NULL;
}


ULONG
CD3DXEffect::AddRefTechnique()
{
    return ++m_uRefTechnique;
}


ULONG
CD3DXEffect::ReleaseTechnique()
{
    if(m_uRefTechnique)
        m_uRefTechnique--;

    if(m_uRefTechnique || m_uRefCount)
        return m_uRefTechnique;

    delete this;
    return 0;
}


STDMETHODIMP
CD3DXEffect::QueryInterface(REFIID iid, LPVOID *ppv)
{
#if DBG
    if(!ppv)
    {
        DPF(0, "ppv pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
#endif

    if(IsEqualIID(iid, IID_ID3DXEffect))
        *ppv = (ID3DXEffect *) this;
    else if(IsEqualIID(iid, IID_IUnknown))
        *ppv = (IUnknown *) this;
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}


STDMETHODIMP_(ULONG)
CD3DXEffect::AddRef()
{
    return ++m_uRefCount;
}


STDMETHODIMP_(ULONG)
CD3DXEffect::Release()
{
    if(m_uRefCount)
        m_uRefCount--;

    if(m_uRefCount || m_uRefTechnique)
        return m_uRefCount;

    delete this;
    return 0;
}


STDMETHODIMP
CD3DXEffect::GetDevice(IDirect3DDevice8 **ppDevice)
{
    if(!ppDevice)
    {
        DPF(0, "ppDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    m_pDevice->AddRef();
    *ppDevice = m_pDevice;

    return S_OK;
}


STDMETHODIMP
CD3DXEffect::GetDesc(D3DXEFFECT_DESC* pDesc)
{
    if(!pDesc)
    {
        DPF(0, "pDesc pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    *pDesc = m_Desc;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetParameterDesc(UINT Index, D3DXPARAMETER_DESC* pDesc)
{
    if(Index >= m_Desc.Parameters)
    {
        DPF(0, "Invalid parameter index");
        return D3DERR_INVALIDCALL;
    }

    if(!pDesc)
    {
        DPF(0, "pDesc pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    pDesc->Name = m_pParameter[Index].m_dwName;
    pDesc->Type = m_pParameter[Index].m_Value.m_Type;

    return S_OK;
}


HRESULT 
CD3DXEffect::GetTechniqueDesc(UINT Index, D3DXTECHNIQUE_DESC* pDesc)
{
    if(Index >= m_Desc.Techniques)
    {
        DPF(0, "Invalid technique index");
        return D3DERR_INVALIDCALL;
    }

    if(!pDesc)
    {
        DPF(0, "pDesc pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    *pDesc = m_pTechnique[Index].m_Desc;
    return S_OK;
}


HRESULT 
CD3DXEffect::SetDword(DWORD Name, DWORD dw)
{
    CD3DXParameter *pParameter;

    if(!(pParameter = FindParameter(D3DXPT_DWORD, Name)))
    {
        return D3DERR_INVALIDCALL;
    }
    pParameter->m_Value.m_Dword = dw;
    return S_OK;
}



HRESULT 
CD3DXEffect::GetDword(DWORD Name, DWORD* pdw)
{
    CD3DXParameter *pParameter;

    if(!pdw)
    {
        DPF(0, "pdw pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_DWORD, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pdw = pParameter->m_Value.m_Dword;
    return S_OK;
}


HRESULT 
CD3DXEffect::SetFloat(DWORD Name, FLOAT f)
{
    CD3DXParameter *pParameter;

    if(!(pParameter = FindParameter(D3DXPT_FLOAT, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    pParameter->m_Value.m_Float = f;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetFloat(DWORD Name, FLOAT* pf)
{
    CD3DXParameter *pParameter;

    if(!pf)
    {
        DPF(0, "pf pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_FLOAT, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pf = pParameter->m_Value.m_Float;
    return S_OK;
}


HRESULT 
CD3DXEffect::SetVector(DWORD Name, D3DXVECTOR4* pVec)
{
    CD3DXParameter *pParameter;

    if(!pVec)
    {
        DPF(0, "pVec pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_VECTOR, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pParameter->m_Value.m_pVector = *pVec;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetVector(DWORD Name, D3DXVECTOR4* pVec)
{
    CD3DXParameter *pParameter;

    if(!pVec)
    {
        DPF(0, "pVec pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_VECTOR, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pVec = *pParameter->m_Value.m_pVector;
    return S_OK;
}


HRESULT 
CD3DXEffect::SetMatrix(DWORD Name, D3DXMATRIX* pMat)
{
    CD3DXParameter *pParameter;

    if(!pMat)
    {
        DPF(0, "pMat pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_MATRIX, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pParameter->m_Value.m_pMatrix = *pMat;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetMatrix(DWORD Name, D3DXMATRIX* pMat)
{
    CD3DXParameter *pParameter;

    if(!pMat)
    {
        DPF(0, "pMat pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_MATRIX, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pMat = *pParameter->m_Value.m_pMatrix;
    return S_OK;
}


HRESULT 
CD3DXEffect::SetTexture(DWORD Name, LPDIRECT3DBASETEXTURE8 pTexture)
{
    CD3DXParameter *pParameter;

    if(!(pParameter = FindParameter(D3DXPT_TEXTURE, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    if(pTexture)
        pTexture->AddRef();

    if(pParameter->m_Value.m_pTexture)
        pParameter->m_Value.m_pTexture->Release();

    pParameter->m_Value.m_pTexture = pTexture;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetTexture(DWORD Name, LPDIRECT3DBASETEXTURE8 *ppTex)
{
    CD3DXParameter *pParameter;

    if(!ppTex)
    {
        DPF(0, "ppTex pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_TEXTURE, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    if(pParameter->m_Value.m_pTexture)
        pParameter->m_Value.m_pTexture->AddRef();

    *ppTex = pParameter->m_Value.m_pTexture;
    return S_OK;
}


HRESULT 
CD3DXEffect::SetVertexShader(DWORD Name, DWORD Handle)
{
    CD3DXParameter *pParameter;

    if(!(pParameter = FindParameter(D3DXPT_VERTEXSHADER, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    pParameter->m_Value.m_VertexShader = Handle;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetVertexShader(DWORD Name, DWORD* pHandle)
{
    CD3DXParameter *pParameter;

    if(!pHandle)
    {
        DPF(0, "pHandle pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_VERTEXSHADER, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pHandle = pParameter->m_Value.m_VertexShader;
    return S_OK;
}


HRESULT 
CD3DXEffect::SetPixelShader(DWORD Name, DWORD Handle)
{
    CD3DXParameter *pParameter;

    if(!(pParameter = FindParameter(D3DXPT_PIXELSHADER, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    pParameter->m_Value.m_PixelShader = Handle;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetPixelShader(DWORD Name, DWORD* pHandle)
{
    CD3DXParameter *pParameter;

    if(!pHandle)
    {
        DPF(0, "pHandle pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pParameter = FindParameter(D3DXPT_PIXELSHADER, Name)))
    {
        return D3DERR_INVALIDCALL;
    }

    *pHandle = pParameter->m_Value.m_PixelShader;
    return S_OK;
}


HRESULT 
CD3DXEffect::GetTechnique(UINT Index, LPD3DXTECHNIQUE* ppTechnique)
{
    if(Index >= m_Desc.Techniques)
    {
        DPF(0, "Invalid technique index");
        return D3DERR_INVALIDCALL;
    }

    if(!ppTechnique)
    {
        DPF(0, "ppTechnique pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    m_uRefTechnique++;
    *ppTechnique = (LPD3DXTECHNIQUE) &m_pTechnique[Index];
    return S_OK;
}


STDMETHODIMP
CD3DXEffect::CloneEffect(LPDIRECT3DDEVICE8 pDevice, DWORD Usage, LPD3DXEFFECT* ppEffect)
{
    HRESULT hr;

    // Validate args
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!ppEffect)
    {
        DPF(0, "ppEffect pointer is invalid");
        return D3DERR_INVALIDCALL;
    }


    // Compute data size
    CEffectNode *pNode = (CEffectNode *) m_pData;
    UINT cbData = (3 + pNode->m_cdwData + pNode->m_cdwNodes) * sizeof(DWORD);


    // Create new effect, similar to this one
    CD3DXEffect *pEffect;

    if(!(pEffect = new CD3DXEffect))
        return E_OUTOFMEMORY;

    if(FAILED(hr = pEffect->Initialize(pDevice, m_pData, cbData, Usage)))
    {
        if (hr == D3DXERR_INVALIDDATA || hr == E_FAIL)
        {
            DPF(0, "Could not parse compiled effect");
            hr = D3DXERR_INVALIDDATA;
        }
        delete pEffect;
        return hr;
    }


    // Share stateblocks and shaders
    // XXXlorenmcq - UNDONE


    *ppEffect = (LPD3DXEFFECT) pEffect;
    return S_OK;
}
