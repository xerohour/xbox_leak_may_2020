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
// CD3DXAssignment ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define ASGN_STATEBLOCK     (1 << 0)
#define ASGN_LIGHT_INIT     (1 << 1)
#define ASGN_LIGHT_SET      (1 << 2)
#define ASGN_MATERIAL_INIT  (1 << 3)
#define ASGN_MATERIAL_SET   (1 << 4)


CD3DXAssignment::CD3DXAssignment()
{
    m_Type          = D3DXPT_DWORD;
    m_dwState       = 0;
    m_dwIndex       = 0;
    m_pParameter    = NULL;
    m_dwFlags       = 0;
    m_pLight        = NULL;
    m_pMaterial     = NULL;
}


CD3DXAssignment::~CD3DXAssignment()
{
    if(m_dwFlags & ASGN_LIGHT_INIT)
        delete m_pLight;

    if(m_dwFlags & ASGN_MATERIAL_INIT)
        delete m_pMaterial;
}


HRESULT 
CD3DXAssignment::Initialize(CD3DXEffect *pEffect, CEffectNode *pAssignment, 
                            CD3DXAssignment *pAsgns)
{
    HRESULT hr;

    D3DXASSERT((pEffect != NULL) && (pAssignment != NULL));


    //
    // Parse header
    //

    if(N_ASSIGNMENT != pAssignment->m_dwId)
        return E_FAIL;

    m_Type    = (D3DXPARAMETERTYPE) pAssignment->m_dwData[0];
    m_dwState = pAssignment->m_dwData[1];
    m_dwIndex = pAssignment->m_dwData[2];

    if(FAILED(hr = m_Value.Initialize(pEffect, m_Type)))
        return hr;


    //
    // Parse Value/Reference
    //

    CEffectNode *pNode;

    if(!(pNode = pAssignment->Children()))
        return E_FAIL;


    if(N_VALUE == pNode->m_dwId)
    {
        CD3DXValue Value;

        if(FAILED(hr = Value.Initialize(pEffect, pNode)))
            return hr;

        if(FAILED(hr = m_Value.Cast(&Value)))
            return hr;

        m_dwFlags |= ASGN_STATEBLOCK;
    }
    else if(N_REFERENCE == pNode->m_dwId)
    {
        if(!(m_pParameter = pEffect->FindParameter((D3DXPARAMETERTYPE) pNode->m_dwData[0], pNode->m_dwData[1])))
            return E_FAIL;
    }
    else
    {
        return E_FAIL;
    }


    // Allocate lights
    if((m_dwState >> 24) == 0x03)
    {
        for(CD3DXAssignment *pAsgn = pAsgns; pAsgn < this; pAsgn++)
        {
            if(((pAsgn->m_dwState >> 24) == 0x03) && (pAsgn->m_dwIndex == m_dwIndex))
            {
                m_pLight = pAsgn->m_pLight;
                pAsgn->m_dwFlags &= ~ASGN_LIGHT_SET;
                break;
            }

        }

        if(!m_pLight)
        {
            if(!(m_pLight = new D3DLIGHT8))
                return E_OUTOFMEMORY;

            m_dwFlags |= ASGN_LIGHT_INIT;
        }

        m_dwFlags |= ASGN_LIGHT_SET;
        m_dwFlags &= ~ASGN_STATEBLOCK;
    }


    // Allocate materials
    if((m_dwState >> 24) == 0x04)
    {
        for(CD3DXAssignment *pAsgn = pAsgns; pAsgn < this; pAsgn++)
        {
            if((pAsgn->m_dwState >> 24) == 0x04)
            {
                m_pMaterial = pAsgn->m_pMaterial;
                pAsgn->m_dwFlags &= ~ASGN_MATERIAL_SET;
                break;
            }

        }

        if(!m_pMaterial)
        {
            if(!(m_pMaterial = new D3DMATERIAL8))
                return E_OUTOFMEMORY;

            m_dwFlags |= ASGN_MATERIAL_INIT;
        }

        m_dwFlags |= ASGN_MATERIAL_SET;
        m_dwFlags &= ~ASGN_STATEBLOCK;
    }

    return S_OK;
}


BOOL 
CD3DXAssignment::IsParameterUsed(DWORD Name)
{
    if(!m_pParameter)
        return FALSE;

    return (m_pParameter->m_dwName == Name);
}


HRESULT 
CD3DXAssignment::Validate(D3DCAPS8 *pCaps)
{
    HRESULT hr;

    // If this is a reference, get current value as set on effect
    if(m_pParameter)
    {
        if(FAILED(hr = m_Value.Cast(&m_pParameter->m_Value)))
            return hr;
    }


    // Validate state
    switch(m_dwState >> 24)
    {
    case 0x01: // RenderState
        switch(m_dwState & 0x00ffffff)
        {
        case D3DRS_SRCBLEND:
            if(!m_Value.m_Dword || !(pCaps->SrcBlendCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DRS_DESTBLEND:
            if(!m_Value.m_Dword || !(pCaps->DestBlendCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DRS_BLENDOP:
            if(D3DBLENDOP_ADD == m_Value.m_Dword)
                break;

            if(!m_Value.m_Dword || !(pCaps->PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP))
                return E_FAIL;

            break;

        case D3DRS_ZFUNC:
            if(!m_Value.m_Dword || !(pCaps->ZCmpCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DRS_ALPHAFUNC:
            if(!m_Value.m_Dword || !(pCaps->AlphaCmpCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DRS_STENCILFAIL:
        case D3DRS_STENCILZFAIL:
        case D3DRS_STENCILPASS:
            if(!m_Value.m_Dword || !(pCaps->StencilCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;            
        }

        break;

    case 0x02: // TextureStageState
        if(m_dwIndex >= 8)
            return E_FAIL;

        switch(m_dwState & 0x00ffffff)
        {
        case D3DTSS_COLOROP:
        case D3DTSS_ALPHAOP:
            if((m_Value.m_Dword != D3DTOP_DISABLE) && (m_dwIndex >= pCaps->MaxTextureBlendStages))
                return E_FAIL;

            if(!m_Value.m_Dword || !(pCaps->TextureOpCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DTSS_COLORARG0:
        case D3DTSS_COLORARG1:
        case D3DTSS_COLORARG2:
        case D3DTSS_ALPHAARG0:
        case D3DTSS_ALPHAARG1:
        case D3DTSS_ALPHAARG2:
            break;

        case D3DTSS_RESULTARG:
            if(D3DTA_CURRENT == m_Value.m_Dword)
                break;

            if((D3DTA_TEMP == m_Value.m_Dword) && (pCaps->PrimitiveMiscCaps & D3DPMISCCAPS_TSSARGTEMP))
                break;

            return E_FAIL;

        case D3DTSS_TEXCOORDINDEX:
            if((m_Value.m_Dword != D3DTSS_TCI_PASSTHRU) && !(pCaps->VertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
                return E_FAIL;

            break;

        case D3DTSS_ADDRESSU:
        case D3DTSS_ADDRESSV:            
            if(!m_Value.m_Dword || !(pCaps->TextureAddressCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DTSS_ADDRESSW:
            if(!m_Value.m_Dword || !(pCaps->VolumeTextureAddressCaps & (1 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DTSS_MAGFILTER:
            if(!m_Value.m_Dword || !(pCaps->TextureFilterCaps & (0x01000000 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DTSS_MINFILTER:
            if(!m_Value.m_Dword || !(pCaps->TextureFilterCaps & (0x00000100 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;

        case D3DTSS_MIPFILTER:
            if( m_Value.m_Dword && !(pCaps->TextureFilterCaps & (0x00010000 << (m_Value.m_Dword - 1))))
                return E_FAIL;

            break;


        case D3DTSS_TEXTURETRANSFORMFLAGS:
            if((m_Value.m_Dword & D3DTTFF_PROJECTED) && !(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
                return E_FAIL;

            switch((m_Value.m_Dword & 0xff) - ((m_Value.m_Dword & D3DTTFF_PROJECTED) ? 1 : 0))
            {
            case 0:
                return E_FAIL;

            case 1:
            case 2:
                break;

            case 3:
                if(!(pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP) && !(pCaps->TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
                    return E_FAIL;

                break;

            default:
                return E_FAIL;
            }

            break;
        }
        break;


    case 0x03: // Light
        break;

    case 0x04: // Material
        break;

    case 0x05: // Texture
        if(m_dwIndex >= 8)
            return E_FAIL;
        break;

    case 0x06: // Shader
        switch(m_dwState)
        {
        case D3DXES_VERTEXSHADER:
            if(!m_Value.m_VertexShader)
                return E_FAIL;

            break;

        case D3DXES_PIXELSHADER:
            if(!m_Value.m_PixelShader)
                return E_FAIL;

            break;

        case D3DXES_VERTEXSHADERCONSTANT:
            if((m_dwIndex >= pCaps->MaxVertexShaderConst))
                return E_FAIL;

            if(!m_Value.m_pConstant)
                return E_FAIL;

            if((m_dwIndex + m_Value.m_pConstant[0] > pCaps->MaxVertexShaderConst))
                return E_FAIL;

            break;

        case D3DXES_PIXELSHADERCONSTANT:
            if((m_dwIndex >= D3DPS_CONSTREG_MAX_DX8))
                return E_FAIL;

            if(!m_Value.m_pConstant)
                return E_FAIL;

            if((m_dwIndex + m_Value.m_pConstant[0] > D3DPS_CONSTREG_MAX_DX8))
                return E_FAIL;

            break;
        }
        break;

    case 0x07: // Transform
        switch(m_dwState)
        {
        case D3DXES_PROJECTIONTRANSFORM:
        case D3DXES_VIEWTRANSFORM:
            break;

        case D3DXES_WORLDTRANSFORM:
            if(m_dwIndex >= pCaps->MaxVertexBlendMatrixIndex)
                return E_FAIL;

            break;

        case D3DXES_TEXTURETRANSFORM:
            if(m_dwIndex >= 8)
                return E_FAIL;
        }
        break;
    }

    return S_OK;
}


HRESULT 
CD3DXAssignment::Apply(LPDIRECT3DDEVICE8 pDevice)
{
    HRESULT hr;

    // If this is a reference, get current value as set on effect
    if(m_pParameter)
    {
        if(FAILED(hr = m_Value.Cast(&m_pParameter->m_Value)))
            return hr;
    }


    // Apply state
    switch(m_dwState >> 24)
    {
    case 0x01: // RenderState
        pDevice->SetRenderState((D3DRENDERSTATETYPE) (m_dwState & 0x00ffffff), m_Value.m_Dword);
        break;

    case 0x02: // TextureStageState
        pDevice->SetTextureStageState(m_dwIndex, (D3DTEXTURESTAGESTATETYPE) (m_dwState & 0x00ffffff), m_Value.m_Dword);
        break;

    case 0x03: // Light
        if(m_dwFlags & ASGN_LIGHT_INIT)
        {
            // Set default value to the light
            memset(m_pLight, 0x00, sizeof(D3DLIGHT8));

            m_pLight->Type = D3DLIGHT_DIRECTIONAL;
            m_pLight->Direction.x = 0.0f;
            m_pLight->Direction.y = 0.0f;
            m_pLight->Direction.z = 1.0f;
            m_pLight->Diffuse.r = 1.0f;
            m_pLight->Diffuse.g = 1.0f;
            m_pLight->Diffuse.b = 1.0f;
        }

        switch(m_dwState)
        {
        case D3DXES_LIGHTTYPE:
            m_pLight->Type = (D3DLIGHTTYPE) m_Value.m_Dword;
            break;

        case D3DXES_LIGHTDIFFUSE:
            m_pLight->Diffuse.r = m_Value.m_pVector->x;
            m_pLight->Diffuse.g = m_Value.m_pVector->y;
            m_pLight->Diffuse.b = m_Value.m_pVector->z;
            m_pLight->Diffuse.a = m_Value.m_pVector->w;
            break;

        case D3DXES_LIGHTSPECULAR:
            m_pLight->Specular.r = m_Value.m_pVector->x;
            m_pLight->Specular.g = m_Value.m_pVector->y;
            m_pLight->Specular.b = m_Value.m_pVector->z;
            m_pLight->Specular.a = m_Value.m_pVector->w;
            break;

        case D3DXES_LIGHTAMBIENT:
            m_pLight->Ambient.r = m_Value.m_pVector->x;
            m_pLight->Ambient.g = m_Value.m_pVector->y;
            m_pLight->Ambient.b = m_Value.m_pVector->z;
            m_pLight->Ambient.a = m_Value.m_pVector->w;
            break;

        case D3DXES_LIGHTPOSITION:
            m_pLight->Position.x = m_Value.m_pVector->x;
            m_pLight->Position.y = m_Value.m_pVector->y;
            m_pLight->Position.z = m_Value.m_pVector->z;
            break;

        case D3DXES_LIGHTDIRECTION:
            m_pLight->Direction.x = m_Value.m_pVector->x;
            m_pLight->Direction.y = m_Value.m_pVector->y;
            m_pLight->Direction.z = m_Value.m_pVector->z;
            break;

        case D3DXES_LIGHTRANGE:
            m_pLight->Range = m_Value.m_Float;
            break;

        case D3DXES_LIGHTFALLOFF:
            m_pLight->Falloff = m_Value.m_Float;
            break;

        case D3DXES_LIGHTATTENUATION0:
            m_pLight->Attenuation0 = m_Value.m_Float;
            break;

        case D3DXES_LIGHTATTENUATION1:
            m_pLight->Attenuation1 = m_Value.m_Float;
            break;

        case D3DXES_LIGHTATTENUATION2:
            m_pLight->Attenuation2 = m_Value.m_Float;
            break;

        case D3DXES_LIGHTTHETA:
            m_pLight->Theta = m_Value.m_Float;
            break;

        case D3DXES_LIGHTPHI:
            m_pLight->Phi = m_Value.m_Float;
            break;

        case D3DXES_LIGHTENABLE:
            pDevice->LightEnable(m_dwIndex, m_Value.m_Dword);
            break;
        }

        if(m_dwFlags & ASGN_LIGHT_SET)
            pDevice->SetLight(m_dwIndex, m_pLight);

        break;

    case 0x04: // Material
        if(m_dwFlags & ASGN_MATERIAL_INIT)
            memset(m_pMaterial, 0x00, sizeof(D3DMATERIAL8));

        switch(m_dwState)
        {
        case D3DXES_MATERIALDIFFUSE:
            m_pMaterial->Diffuse.r = m_Value.m_pVector->x;
            m_pMaterial->Diffuse.g = m_Value.m_pVector->y;
            m_pMaterial->Diffuse.b = m_Value.m_pVector->z;
            m_pMaterial->Diffuse.a = m_Value.m_pVector->w;
            break;

        case D3DXES_MATERIALAMBIENT:
            m_pMaterial->Ambient.r = m_Value.m_pVector->x;
            m_pMaterial->Ambient.g = m_Value.m_pVector->y;
            m_pMaterial->Ambient.b = m_Value.m_pVector->z;
            m_pMaterial->Ambient.a = m_Value.m_pVector->w;
            break;

        case D3DXES_MATERIALSPECULAR:
            m_pMaterial->Specular.r = m_Value.m_pVector->x;
            m_pMaterial->Specular.g = m_Value.m_pVector->y;
            m_pMaterial->Specular.b = m_Value.m_pVector->z;
            m_pMaterial->Specular.a = m_Value.m_pVector->w;
            break;

        case D3DXES_MATERIALEMISSIVE:
            m_pMaterial->Emissive.r = m_Value.m_pVector->x;
            m_pMaterial->Emissive.g = m_Value.m_pVector->y;
            m_pMaterial->Emissive.b = m_Value.m_pVector->z;
            m_pMaterial->Emissive.a = m_Value.m_pVector->w;
            break;

        case D3DXES_MATERIALPOWER:
            m_pMaterial->Power = m_Value.m_Float;
            break;
        }

        if(m_dwFlags & ASGN_MATERIAL_SET)
            pDevice->SetMaterial(m_pMaterial);

        break;

    case 0x05: // Texture
        pDevice->SetTexture(m_dwIndex, m_Value.m_pTexture);
        break;

    case 0x06: // Shader
        switch(m_dwState)
        {
        case D3DXES_VERTEXSHADER:
            if(m_Value.m_VertexShader)
                pDevice->SetVertexShader(m_Value.m_VertexShader);
            break;

        case D3DXES_PIXELSHADER:
            if(m_Value.m_PixelShader)
                pDevice->SetPixelShader(m_Value.m_PixelShader);
            break;

        case D3DXES_VERTEXSHADERCONSTANT:
            if(m_Value.m_pConstant)
                pDevice->SetVertexShaderConstant(m_dwIndex, &m_Value.m_pConstant[1], m_Value.m_pConstant[0]);
            break;

        case D3DXES_PIXELSHADERCONSTANT:
            if(m_Value.m_pConstant)
                pDevice->SetPixelShaderConstant(m_dwIndex, &m_Value.m_pConstant[1], m_Value.m_pConstant[0]);
            break;
        }
        break;

    case 0x07: // Transform
        switch(m_dwState)
        {
        case D3DXES_PROJECTIONTRANSFORM:
            pDevice->SetTransform(D3DTS_PROJECTION, m_Value.m_pMatrix);
            break;

        case D3DXES_VIEWTRANSFORM:
            pDevice->SetTransform(D3DTS_VIEW, m_Value.m_pMatrix);
            break;

        case D3DXES_WORLDTRANSFORM:
            pDevice->SetTransform(D3DTS_WORLDMATRIX(m_dwIndex), m_Value.m_pMatrix);
            break;

        case D3DXES_TEXTURETRANSFORM:
            pDevice->SetTransform((D3DTRANSFORMSTATETYPE) (D3DTS_TEXTURE0 + m_dwIndex), m_Value.m_pMatrix);
            break;
        }
        break;
    }

    return S_OK;
}



//////////////////////////////////////////////////////////////////////////////
// CD3DXPass /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXPass::CD3DXPass()
{
    m_pEffect = NULL;
    m_pDevice = NULL;
    m_dwState = NULL;

    m_cAssignment = 0;
    m_pAssignment = 0;

    memset(&m_Desc, 0x00, sizeof(D3DXPASS_DESC));
}


CD3DXPass::~CD3DXPass()
{
    if(m_pDevice && m_dwState)
        m_pDevice->DeleteStateBlock(m_dwState);

    delete [] m_pAssignment;
}


HRESULT 
CD3DXPass::Initialize(CD3DXEffect *pEffect, CEffectNode *pPass)
{
    HRESULT hr;

    D3DXASSERT((pEffect != NULL) && (pPass != NULL));

    m_pEffect = pEffect;
    m_pDevice = pEffect->m_pDevice;


    //
    // Parse header
    //

    if(N_PASS != pPass->m_dwId)
        return E_FAIL;

    m_Desc.Name = pPass->m_dwData[0];


    //
    // Parse assignments
    //

    UINT iAssignment;
    CEffectNode *pAssignment;

    pAssignment = pPass->Children();

    while(pAssignment)
    {
        m_cAssignment++;
        pAssignment = pPass->Sibling(pAssignment);
    }

    if(!(m_pAssignment = new CD3DXAssignment[m_cAssignment]))
        return E_OUTOFMEMORY;

    iAssignment = 0;
    pAssignment = pPass->Children();

    while(pAssignment)
    {
        if(FAILED(hr = m_pAssignment[iAssignment].Initialize(pEffect, pAssignment, m_pAssignment)))
            return hr;

        iAssignment++;
        pAssignment = pPass->Sibling(pAssignment);
    }

    return S_OK;
}


BOOL
CD3DXPass::IsParameterUsed(DWORD Name)
{
    for(UINT iAssignment = 0; iAssignment < m_cAssignment; iAssignment++)
    {
        if(m_pAssignment[iAssignment].IsParameterUsed(Name))
            return TRUE;
    }

    return FALSE;
}


HRESULT 
CD3DXPass::Validate(D3DCAPS8 *pCaps)
{
    HRESULT hr;

    for(UINT iAssignment = 0; iAssignment < m_cAssignment; iAssignment++)
    {
        if(FAILED(hr = m_pAssignment[iAssignment].Validate(pCaps)))
            return hr;
    }

    return S_OK;
}


HRESULT 
CD3DXPass::Apply(BOOL bUseStateBlocks)
{
    HRESULT hr;

    if(bUseStateBlocks)
    {
        if(!m_dwState)
        {
            m_pDevice->BeginStateBlock();

            for(UINT iAssignment = 0; iAssignment < m_cAssignment; iAssignment++)
            {
                if(m_pAssignment[iAssignment].m_dwFlags & ASGN_STATEBLOCK)
                    m_pAssignment[iAssignment].Apply(m_pDevice);
            }

            m_pDevice->EndStateBlock(&m_dwState);
        }

        m_pDevice->ApplyStateBlock(m_dwState);
    }

    for(UINT iAssignment = 0; iAssignment < m_cAssignment; iAssignment++)
    {
        if(!bUseStateBlocks || !(m_pAssignment[iAssignment].m_dwFlags & ASGN_STATEBLOCK))
            m_pAssignment[iAssignment].Apply(m_pDevice);
    }

    return S_OK;
}



//////////////////////////////////////////////////////////////////////////////
// CD3DXTechnique ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXTechnique::CD3DXTechnique()
{
    m_uRefCount  = 1;
    m_pDevice    = NULL;
    m_pTechnique = NULL;
    m_pPass      = NULL;
    m_dwClear    = 0;

    memset(&m_Desc, 0x00, sizeof(D3DXTECHNIQUE_DESC));
}


CD3DXTechnique::~CD3DXTechnique()
{
    delete [] m_pPass;

    if(m_pDevice && m_dwClear)
        m_pDevice->DeleteStateBlock(m_dwClear);
}


HRESULT
CD3DXTechnique::Initialize(CD3DXEffect *pEffect, CEffectNode *pTechnique)
{
    HRESULT hr;

    D3DXASSERT((pEffect !=NULL) && (pTechnique != NULL));

    m_pEffect = pEffect;
    m_pDevice = pEffect->m_pDevice;


    //
    // Parse header
    //

    if(N_TECHNIQUE != pTechnique->m_dwId)
        return E_FAIL;

    m_Desc.Name = pTechnique->m_dwData[0];


    //
    // Parse passes
    //

    UINT iPass;
    CEffectNode *pPass;

    pPass = pTechnique->Children();

    while(pPass)
    {
        m_Desc.Passes++;
        pPass = pTechnique->Sibling(pPass);
    }

    if(!(m_pPass = new CD3DXPass[m_Desc.Passes]))
        return E_OUTOFMEMORY;

    iPass = 0;
    pPass = pTechnique->Children();

    while(pPass)
    {
        if(FAILED(hr = m_pPass[iPass].Initialize(pEffect, pPass)))
            return hr;

        iPass++;
        pPass = pTechnique->Sibling(pPass);
    }

    return S_OK;
}





STDMETHODIMP
CD3DXTechnique::QueryInterface(REFIID iid, LPVOID *ppv)
{
#if DBG
    if(!ppv)
    {
        DPF(0, "ppv pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
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
    if(m_pEffect)
        m_pEffect->AddRefTechnique();

    return ++m_uRefCount;
}


STDMETHODIMP_(ULONG)
CD3DXTechnique::Release()
{
    UINT uRefCount = --m_uRefCount;

    if(m_pEffect)
        m_pEffect->ReleaseTechnique();

    return uRefCount;
}


STDMETHODIMP
CD3DXTechnique::GetDesc(D3DXTECHNIQUE_DESC* pDesc)
{
    if(!pDesc)
    {
        DPF(0, "pDesc pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    *pDesc = m_Desc;
    return S_OK;
}


STDMETHODIMP
CD3DXTechnique::GetPassDesc(UINT Index, D3DXPASS_DESC* pDesc)
{
    if(!pDesc)
    {
        DPF(0, "pDesc pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(Index >= m_Desc.Passes)
    {
        DPF(0, "Invalid pass index");
        return D3DERR_INVALIDCALL;
    }

    *pDesc = m_pPass[Index].m_Desc;
    return S_OK;
}



STDMETHODIMP
CD3DXTechnique::GetDevice(IDirect3DDevice8 **ppDevice)
{
    if(!ppDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    m_pDevice->AddRef();
    *ppDevice = m_pDevice;

    return S_OK;
}


STDMETHODIMP_(BOOL)
CD3DXTechnique::IsParameterUsed(DWORD Name)
{
    for(UINT iPass = 0; iPass < m_Desc.Passes; iPass++)
    {
        if(m_pPass[iPass].IsParameterUsed(Name))
            return TRUE;
    }

    return FALSE;
}


STDMETHODIMP
CD3DXTechnique::Validate()
{
    HRESULT hr;
    UINT iPass;
    DWORD dw;
    D3DCAPS8 caps;


    // Do some preliminary validation against device caps, since drivers
    // don't generally tend to do a very good job of ValidateDevice.
    m_pDevice->GetDeviceCaps(&caps);

#if 0
    if(m_pEffect->m_Desc.Usage & D3DUSAGE_SOFTWAREPROCESSING)
    {
        caps.VertexProcessingCaps = D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_MATERIALSOURCE7 |
            D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_LOCALVIEWER;

        caps.MaxActiveLights           = 0xffffffff;
        caps.MaxUserClipPlanes         = 6;
        caps.MaxVertexBlendMatrices    = 4;
        caps.MaxVertexBlendMatrixIndex = 255;

        if(0.0f == caps.MaxPointSize)
            caps.MaxPointSize = 64.0f;

        caps.MaxPrimitiveCount         = 0xffffffff;
        caps.MaxVertexIndex            = 0xffffffff;
        caps.MaxStreams                = 16;
        caps.MaxStreamStride           = 0xffffffff;
        caps.VertexShaderVersion       = D3DVS_VERSION(1, 0);
        caps.MaxVertexShaderConst      = D3DVS_CONSTREG_MAX_V1_0;
    }
#endif 0

    for(iPass = 0; iPass < m_Desc.Passes; iPass++)
    {
        if(FAILED(hr = m_pPass[iPass].Validate(&caps)))
            return hr;
    }


    // Actually setup the device state for each pass and call ValidateDevice.
    if(FAILED(hr = Begin(NULL)))
        return hr;

    for(iPass = 0; iPass < m_Desc.Passes; iPass++)
    {
        if(FAILED(hr = Pass(iPass)) /* ||
           FAILED(hr = m_pDevice->ValidateDevice(&dw)) */ )
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
CD3DXTechnique::Begin(UINT *pPasses)
{
    HRESULT hr;

    if(!m_dwClear)
    {
        m_pDevice->BeginStateBlock();

        for(UINT iPass = 0; iPass < m_Desc.Passes; iPass++)
            m_pPass[iPass].Apply(FALSE);

        m_pDevice->EndStateBlock(&m_dwClear);
    }

    m_pDevice->CaptureStateBlock(m_dwClear);

    if(pPasses)
        *pPasses = m_Desc.Passes;

    return S_OK;
}


STDMETHODIMP
CD3DXTechnique::Pass(UINT Index)
{
    HRESULT hr;

    if(Index >= m_Desc.Passes)
    {
        DPF(0, "Invalid pass index");
        return D3DERR_INVALIDCALL;
    }

    if(FAILED(hr = m_pPass[Index].Apply(TRUE)))
        return hr;

    return S_OK;
}


STDMETHODIMP
CD3DXTechnique::End()
{
    if(m_dwClear)
        m_pDevice->ApplyStateBlock(m_dwClear);

    return S_OK;
}
