//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       effect.h
//  Content:    D3DX effect types and functions
//
//////////////////////////////////////////////////////////////////////////////


#include "pcheffect.h"




StateData::StateData()
{
    m_dwState = NULL;
    m_dwValue = NULL;
    m_pNext = NULL;
}

StateData::~StateData()
{
    delete m_pNext;
}


StageData::StageData()
{
    m_uTexture = (UINT) -1;
    m_uMatrix  = (UINT) -1;

    m_pTSS = NULL;
    m_pNext = NULL;
}

StageData::~StageData()
{
    delete m_pTSS;
    delete m_pNext;
}


VShaderData::VShaderData()
{
    m_pShader = NULL;
}

VShaderData::~VShaderData()
{
    RELEASE(m_pShader);
}


PShaderData::PShaderData()
{
    m_pDeclaration = NULL;
    m_pShader = NULL;
}

PShaderData::~PShaderData()
{
    RELEASE(m_pDeclaration);
    RELEASE(m_pShader);
}


PassData::PassData()
{
    m_pRS = NULL;
    m_pStage = NULL;
    m_pVShader = NULL;
    m_pPShader = NULL;
    m_pNext = NULL;

    m_dwState = 0;
    m_dwVShader = 0;
    m_dwPShader = 0;

    m_pDevice = NULL;
}

PassData::~PassData()
{
    delete m_pRS;
    delete m_pStage;
    delete m_pVShader;
    delete m_pPShader;
    delete m_pNext;

    if(m_pDevice)
    {
        if(m_dwState)
            m_pDevice->DeleteStateBlock(m_dwState);

        if(m_dwVShader)
            m_pDevice->DeleteVertexShader(m_dwVShader);

        if(m_dwPShader)
            m_pDevice->DeletePixelShader(m_dwPShader);
    }
}


TechniqueData::TechniqueData()
{
    m_uLOD       = 0;
    m_uPasses    = 0;
    m_uRefCount  = 0;

    m_pRS   = NULL;
    m_pPass = NULL;
    m_pNext = NULL;
}

TechniqueData::~TechniqueData()
{
    delete m_pRS;
    delete m_pPass;
    delete m_pNext;
}





//////////////////////////////////////////////////////////////////////////////
// CD3DXTechnique ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXTechnique::CD3DXTechnique()
{
    m_uRefCount = 1;
    m_dwClear = 0;
    m_pEffect = NULL;
    m_pTechniqueData = NULL;
    m_pDevice = NULL;
}


CD3DXTechnique::~CD3DXTechnique()
{
    if(!m_pEffect)
        return;

    m_pTechniqueData->m_uRefCount--;

    if(!m_pTechniqueData->m_uRefCount)
    {
        PassData *pPassData = m_pTechniqueData->m_pPass;

        while(pPassData)
        {
            m_pDevice->DeleteStateBlock(pPassData->m_dwState);
            pPassData->m_dwState = 0;

            if(pPassData->m_dwVShader)
            {
                m_pDevice->DeleteVertexShader(pPassData->m_dwVShader);
                pPassData->m_dwVShader = 0;
            }

            if(pPassData->m_dwPShader)
            {
                m_pDevice->DeletePixelShader(pPassData->m_dwPShader);
                pPassData->m_dwPShader = 0;
            }

            pPassData = pPassData->m_pNext;
        }
    }

    m_pDevice->DeleteStateBlock(m_dwClear);

    m_pEffect->Release();
    m_pDevice->Release();
}


HRESULT
CD3DXTechnique::Initialize(CD3DXEffect *pEffect, TechniqueData *pTechniqueData,
                           DWORD dwClear, DWORD dwTextureUsed, DWORD dwMatrixUsed)
{
    m_dwClear = dwClear;

    m_pEffect = pEffect;
    m_pEffect->AddRef();

    m_pTechniqueData = pTechniqueData;
    m_pTechniqueData->m_uRefCount++;

    m_pDevice = m_pEffect->m_pDevice;
    m_pDevice->AddRef();

    m_dwDwordUsed   = 0;
    m_dwFloatUsed   = 0;
    m_dwColorUsed   = 0;
    m_dwVectorUsed  = 0;
    m_dwMatrixUsed  = dwMatrixUsed;
    m_dwTextureUsed = dwTextureUsed;

    return S_OK;
}



HRESULT
CD3DXTechnique::ValidateDX8(UINT uPass)
{
    PassData *pPass = m_pTechniqueData->m_pPass;

    while(uPass && pPass)
    {
        pPass = pPass->m_pNext;
        uPass--;
    }

    if(!pPass)
        return E_FAIL;


    D3DCAPS8 caps;
    m_pDevice->GetDeviceCaps(&caps);

    UINT uStages = 0;
    UINT uTextures = 0;

    StageData *pStage = pPass->m_pStage;

    while(pStage)
    {
        if(pStage->m_uTexture < m_pEffect->m_pData->m_uTextures)
            uTextures++;

        StateData *pState = pStage->m_pTSS;

        while(pState)
        {
            if(pState->m_dwState == D3DTSS_COLOROP ||
               pState->m_dwState == D3DTSS_ALPHAOP)
            {
                if(0 == (((1 << pState->m_dwValue) >> 1) & caps.TextureOpCaps))
                    return E_FAIL;
            }


            if(pState->m_dwState == D3DTSS_TEXTURETRANSFORMFLAGS)
            {
                UINT uCoords = pState->m_dwValue & 0xff;

                if(pState->m_dwValue & D3DTTFF_PROJECTED)
                {
                    uCoords--;

                    if(!(caps.TextureCaps & D3DPTEXTURECAPS_PROJECTED))
                        return E_FAIL;
                }

                if(3 == uCoords)
                {
                    if(!(caps.TextureCaps & (D3DPTEXTURECAPS_CUBEMAP | D3DPTEXTURECAPS_VOLUMEMAP)))
                        return E_FAIL;
                }

                if(4 == uCoords)
                    return E_FAIL;
            }

            pState = pState->m_pNext;
        }

        uStages++;
        pStage = pStage->m_pNext;
    }

    if(uStages > caps.MaxTextureBlendStages)
        return E_FAIL;

    if(uTextures > caps.MaxSimultaneousTextures)
        return E_FAIL;

    return S_OK;
}


STDMETHODIMP
CD3DXTechnique::QueryInterface(REFIID iid, LPVOID *ppv)
{
#if DBG
    if(!ppv)
        return E_INVALIDARG;
#endif

    if(IsEqualIID(iid, IID_ID3DXTechnique))
        *ppv = (ID3DXTechnique *) this;
    else if(IsEqualIID(iid, IID_IUnknown))
        *ppv = (IUnknown *) this;
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}


STDMETHODIMP_(ULONG)
CD3DXTechnique::AddRef()
{
    return ++m_uRefCount;
}


STDMETHODIMP_(ULONG)
CD3DXTechnique::Release()
{
    if(--m_uRefCount)
        return m_uRefCount;

    delete this;
    return 0;
}


STDMETHODIMP_(D3DXTECHNIQUE_DESC)
CD3DXTechnique::GetTechniqueDesc()
{
    D3DXTECHNIQUE_DESC desc;
    memset(&desc, 0x00, sizeof(desc));

    desc.Passes = m_pTechniqueData->m_uPasses;
    desc.LOD    = m_pTechniqueData->m_uLOD;

    return desc;
}


STDMETHODIMP
CD3DXTechnique::GetDevice(IDirect3DDevice8 **ppDevice)
{
#if DBG
    if(!ppDevice)
        return E_INVALIDARG;
#endif

    m_pDevice->AddRef();
    *ppDevice = m_pDevice;

    return S_OK;
}


STDMETHODIMP_(BOOL)
CD3DXTechnique::IsDwordUsed(UINT uIndex)
{
    return m_dwDwordUsed & (1 << uIndex);
}


STDMETHODIMP_(BOOL)
CD3DXTechnique::IsFloatUsed(UINT uIndex)
{
    return m_dwFloatUsed & (1 << uIndex);
}


STDMETHODIMP_(BOOL)
CD3DXTechnique::IsColorUsed(UINT uIndex)
{
    return m_dwColorUsed & (1 << uIndex);
}


STDMETHODIMP_(BOOL)
CD3DXTechnique::IsVectorUsed(UINT uIndex)
{
    return m_dwVectorUsed & (1 << uIndex);
}


STDMETHODIMP_(BOOL)
CD3DXTechnique::IsMatrixUsed(UINT uIndex)
{
    return m_dwMatrixUsed & (1 << uIndex);
}

STDMETHODIMP_(BOOL)
CD3DXTechnique::IsTextureUsed(UINT uIndex)
{
    return m_dwTextureUsed & (1 << uIndex);
}


STDMETHODIMP
CD3DXTechnique::Validate()
{
    HRESULT hr;
    UINT uPass;
    UINT uPasses;
    DWORD dw;

    if(FAILED(hr = Begin(&uPasses)))
        return hr;

    for(uPass = 0; uPass < uPasses; uPass++)
    {
        if(FAILED(hr = Pass(uPass)) ||
           FAILED(hr = ValidateDX8(uPass)) ||
           FAILED(hr = m_pDevice->ValidateDevice(&dw)))
        {
            End();
            return hr;
        }
    }

    if(FAILED(hr = End()))
        return hr;

    return S_OK;
}


STDMETHODIMP
CD3DXTechnique::Begin(UINT *puPasses)
{
    if(puPasses)
        *puPasses = m_pTechniqueData->m_uPasses;

    m_pDevice->CaptureStateBlock(m_dwClear);

    return S_OK;
}


STDMETHODIMP
CD3DXTechnique::Pass(UINT uPass)
{
#if DBG
    if(uPass >= m_pTechniqueData->m_uPasses)
        return E_INVALIDARG;
#endif

    PassData *pPass = m_pTechniqueData->m_pPass;

    while(uPass && pPass)
    {
        pPass = pPass->m_pNext;
        uPass--;
    }

    if(!pPass)
        return E_FAIL;

    m_pDevice->ApplyStateBlock(pPass->m_dwState);

    if(pPass->m_dwVShader)
        m_pDevice->SetVertexShader(pPass->m_dwVShader);

    if(pPass->m_dwPShader)
        m_pDevice->SetPixelShader(pPass->m_dwPShader);


    // Setup textures and matrices
    UINT uStage = 0;
    StageData *pStage = pPass->m_pStage;

    while(pStage)
    {
        if(pStage->m_uTexture < m_pEffect->m_pData->m_uTextures)
            m_pDevice->SetTexture(uStage, m_pEffect->m_ppSurface[pStage->m_uTexture]);

        if(pStage->m_uMatrix < m_pEffect->m_pData->m_uMatrices)
            m_pDevice->SetTransform((_D3DTRANSFORMSTATETYPE) (uStage + D3DTS_TEXTURE0), &m_pEffect->m_pMatrix[pStage->m_uMatrix]);

        uStage++;
        pStage = pStage->m_pNext;
    }

    return S_OK;
}


STDMETHODIMP
CD3DXTechnique::End()
{
    m_pDevice->ApplyStateBlock(m_dwClear);
    return S_OK;
}
