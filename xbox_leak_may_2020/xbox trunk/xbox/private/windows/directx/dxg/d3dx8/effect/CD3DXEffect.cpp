//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       effect.h
//  Content:    D3DX effect types and functions
//
//////////////////////////////////////////////////////////////////////////////


#include "pcheffect.h"


EffectData::EffectData()
{
    m_uTextures = 0;
    m_uMatrices = 0;
    m_uTechniques = 0;

    m_uMaxLOD = 0;
    m_uMaxLOD = 0;

    m_pTechnique = NULL;
}

EffectData::~EffectData()
{
    delete m_pTechnique;
}



//////////////////////////////////////////////////////////////////////////////
// CD3DXEffect ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXEffect::CD3DXEffect()
{
    m_uRefCount  = 1;
    m_pData      = NULL;
    m_ppSurface  = NULL;
    m_pMatrix    = NULL;
    m_pDevice    = NULL;
}


CD3DXEffect::~CD3DXEffect()
{
    if(!m_pData)
        return;

    for(UINT u = 0; u < m_pData->m_uTextures; u++)
    {
        if(m_ppSurface[u])
            m_ppSurface[u]->Release();
    }

    delete m_ppSurface;
    delete m_pMatrix;
    delete m_pData;

    if(m_pDevice)
        m_pDevice->Release();
}


HRESULT
CD3DXEffect::Initialize(IDirect3DDevice8 *pDevice, EffectData *pData)
{
    // Setup data structures
    if(!(m_ppSurface = new IDirect3DBaseTexture8*[pData->m_uTextures]))
        return E_OUTOFMEMORY;

    if(!(m_pMatrix = new D3DXMATRIX[pData->m_uMatrices]))
        return E_OUTOFMEMORY;

    memset(m_ppSurface, 0x00, pData->m_uTextures * sizeof(IDirect3DBaseTexture8*));

    for(UINT u = 0; u < pData->m_uMatrices; u++)
        D3DXMatrixIdentity(&m_pMatrix[u]);

    m_pData = pData;
    m_pDevice = pDevice;
    m_pDevice->AddRef();


    return S_OK;
}


STDMETHODIMP
CD3DXEffect::QueryInterface(REFIID iid, LPVOID *ppv)
{
#if DBG
    if(!ppv)
        return E_INVALIDARG;
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
    if(--m_uRefCount)
        return m_uRefCount;

    delete this;
    return 0;
}


STDMETHODIMP
CD3DXEffect::GetDevice(IDirect3DDevice8 **ppDevice)
{
#if DBG
    if(!ppDevice)
        return E_INVALIDARG;
#endif

    m_pDevice->AddRef();
    *ppDevice = m_pDevice;

    return S_OK;
}


STDMETHODIMP_(D3DXEFFECT_DESC)
CD3DXEffect::GetEffectDesc()
{
    D3DXEFFECT_DESC desc;
    memset(&desc, 0x00, sizeof(desc));

    desc.Matrices   = m_pData->m_uMatrices;
    desc.Textures   = m_pData->m_uTextures;
    desc.Techniques = m_pData->m_uTechniques;
    desc.MinLOD     = m_pData->m_uMinLOD;
    desc.MaxLOD     = m_pData->m_uMaxLOD;

    return desc;
}


STDMETHODIMP
CD3DXEffect::SetDword(UINT uINdex, DWORD dw)
{
    return E_NOTIMPL;
}

STDMETHODIMP
CD3DXEffect::SetFloat(UINT uINdex, FLOAT f)
{
    return E_NOTIMPL;
}

STDMETHODIMP
CD3DXEffect::SetColor(UINT uINdex, D3DCOLOR c)
{
    return E_NOTIMPL;
}

STDMETHODIMP
CD3DXEffect::SetVector(UINT uINdex, D3DXVECTOR4 *pVector)
{
    return E_NOTIMPL;
}

STDMETHODIMP
CD3DXEffect::SetMatrix(UINT uIndex, D3DMATRIX *pMatrix)
{
#if DBG
    if(uIndex >= m_pData->m_uMatrices)
        return E_INVALIDARG;
    if(!pMatrix)
        return E_INVALIDARG;
#endif

    m_pMatrix[uIndex] = *pMatrix;
    return S_OK;
}


STDMETHODIMP
CD3DXEffect::SetTexture(UINT uIndex, IDirect3DBaseTexture8* pSurface)
{
#if DBG
    if(uIndex >= m_pData->m_uTextures)
        return E_INVALIDARG;
#endif

    if(pSurface)
        pSurface->AddRef();

    if(m_ppSurface[uIndex])
        m_ppSurface[uIndex]->Release();

    m_ppSurface[uIndex] = pSurface;
    return S_OK;
}


STDMETHODIMP
CD3DXEffect::GetTechnique(UINT uIndex, LPD3DXTECHNIQUE *ppTechnique)
{
#if DBG
    if(uIndex >= m_pData->m_uTechniques)
        return E_INVALIDARG;
    if(!ppTechnique)
        return E_INVALIDARG;
#endif

    HRESULT hr;

    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);

    DWORD dwClear;
    DWORD dwTextureUsed = 0;
    DWORD dwMatrixUsed = 0;

    CD3DXTechnique *pTechnique = NULL;


    TechniqueData *pTechniqueData = m_pData->m_pTechnique;

    while(pTechniqueData && uIndex)
    {
        pTechniqueData = pTechniqueData->m_pNext;
        uIndex--;
    }

    if(!pTechniqueData)
        return E_INVALIDARG;



    // Create stateblocks
    for(BOOL bClear = 0; bClear <= 1; bClear++)
    {
        if(bClear)
            m_pDevice->BeginStateBlock();

        PassData *pPassData = pTechniqueData->m_pPass;

        while(pPassData)
        {
            if(bClear || !pPassData->m_dwState)
            {
                if(!bClear)
                    m_pDevice->BeginStateBlock();

                // Set technique renderstates
                StateData *pStateData = pTechniqueData->m_pRS;

                while(pStateData)
                {
                    switch(pStateData->m_dwState)
                    {
                    case D3DRS_FVF:
                        m_pDevice->SetVertexShader(pStateData->m_dwValue);
                        break;

                    default:
                        m_pDevice->SetRenderState((_D3DRENDERSTATETYPE) pStateData->m_dwState, pStateData->m_dwValue);
                        break;
                    }

                    pStateData = pStateData->m_pNext;
                }

                // Set pass renderstates
                pStateData = pPassData->m_pRS;

                while(pStateData)
                {
                    switch(pStateData->m_dwState)
                    {
                    case D3DRS_FVF:
                        m_pDevice->SetVertexShader(pStateData->m_dwValue);
                        break;

                    default:
                        m_pDevice->SetRenderState((_D3DRENDERSTATETYPE) pStateData->m_dwState, pStateData->m_dwValue);
                        break;
                    }

                    pStateData = pStateData->m_pNext;
                }

                // Set pass texturestagestates
                UINT uStage = 0;
                StageData *pStageData = pPassData->m_pStage;

                while(pStageData)
                {
                    if(bClear)
                    {
                        if(pStageData->m_uTexture < m_pData->m_uTextures)
                        {
                            dwTextureUsed |= (1 << pStageData->m_uTexture);
                            m_pDevice->SetTexture(uStage, NULL);
                        }

                        if(pStageData->m_uMatrix < m_pData->m_uMatrices)
                        {
                            dwMatrixUsed |= (1 << pStageData->m_uMatrix);
                            m_pDevice->SetTransform((_D3DTRANSFORMSTATETYPE) (uStage + D3DTS_TEXTURE0), &mat);
                        }
                    }

                    m_pDevice->SetTextureStageState(uStage, D3DTSS_COLOROP, D3DTOP_DISABLE);
                    m_pDevice->SetTextureStageState(uStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

                    StateData *pStateData = pStageData->m_pTSS;

                    while(pStateData)
                    {
                        m_pDevice->SetTextureStageState(uStage, (_D3DTEXTURESTAGESTATETYPE) pStateData->m_dwState, pStateData->m_dwValue);
                        pStateData = pStateData->m_pNext;
                    }

                    uStage++;
                    pStageData = pStageData->m_pNext;
                }

                if(uStage < 8)
                {
                    m_pDevice->SetTextureStageState(uStage, D3DTSS_COLOROP, D3DTOP_DISABLE);
                    m_pDevice->SetTextureStageState(uStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                }

                if(!bClear)
                    m_pDevice->EndStateBlock(&pPassData->m_dwState);
            }


            // Vertex shader
            if(!bClear && pPassData->m_pVShader && !pPassData->m_dwVShader)
            {
                DWORD *pdwShader = (DWORD *) pPassData->m_pVShader->m_pShader->GetBufferPointer();

                if(FAILED(hr = m_pDevice->CreateVertexShader(pPassData->m_pVShader->m_pDeclaration, pdwShader, &pPassData->m_dwVShader, 0)))
                    return hr;
            }


            // Pixel shader
            if(!bClear && pPassData->m_pPShader && !pPassData->m_dwPShader)
            {
                DWORD *pdwShader = (DWORD *) pPassData->m_pPShader->m_pDeclaration->GetBufferPointer();

                if(FAILED(hr = m_pDevice->CreatePixelShader(pdwShader, &pPassData->m_dwVShader)))
                    return hr;
            }


            pPassData = pPassData->m_pNext;
        }

        if(bClear)
            m_pDevice->EndStateBlock(&dwClear);
    }


    // Create technique
    if(!(pTechnique = new CD3DXTechnique))
        return E_OUTOFMEMORY;


    if(FAILED(hr = pTechnique->Initialize(this, pTechniqueData, dwClear, dwTextureUsed, dwMatrixUsed)))
    {
        pTechnique->Release();
        return hr;
    }

    *ppTechnique = (LPD3DXTECHNIQUE) pTechnique;
    return S_OK;
}


STDMETHODIMP
CD3DXEffect::PickTechnique(UINT uMinLOD, UINT uMaxLOD, LPD3DXTECHNIQUE *ppTechnique)
{
#if DBG
    if(!ppTechnique)
        return E_INVALIDARG;
#endif

    UINT uTechnique = 0;
    TechniqueData *pTechniqueData = m_pData->m_pTechnique;

    while(pTechniqueData)
    {
        if(pTechniqueData->m_uLOD >= uMinLOD &&
           pTechniqueData->m_uLOD <= uMaxLOD)
        {
            LPD3DXTECHNIQUE pTechnique;

            if(SUCCEEDED(GetTechnique(uTechnique, &pTechnique)))
            {
                if(SUCCEEDED(pTechnique->Validate()))
                {
                    *ppTechnique = pTechnique;
                    return S_OK;
                }

                pTechnique->Release();
            }
        }

        uTechnique++;
        pTechniqueData = pTechniqueData->m_pNext;
    }

    return E_FAIL;
}
