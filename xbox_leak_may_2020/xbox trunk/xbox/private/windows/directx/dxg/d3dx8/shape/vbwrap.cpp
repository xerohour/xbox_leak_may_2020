#include "pchshape.h"

CVertexArray::CVertexArray()
{
    m_vb = NULL;
    memset(&m_vbDesc, 0, sizeof(D3DVERTEXBUFFER_DESC));
    m_dwGrowSize = 128;     // Number of vertices to grow

    m_pDevice = NULL;
}

CVertexArray::~CVertexArray()
{
    if( m_pDevice )
        m_pDevice->Release();

    if( m_pD3D )
        m_pD3D->Release();

    if (m_vb)
        m_vb->Release();
}

void
CVertexArray::Init( LPDIRECT3D8 pD3D,
                    LPDIRECT3DDEVICE8 pD3DDev,
                    DWORD dwMaxVertexCount,
                    D3DXVERTEXCOMPONENT* pComponent, DWORD dwNumComponents,
                    DWORD dwFlags )
{
    HRESULT hr = S_OK;
    m_pDevice = pD3DDev;
    if( m_pDevice == NULL )
    {
        hr = D3DERR_INVALIDCALL;
        throw CD3DXException( hr,
                             "CVertexArray::Init - m_pDevice is NULL",
                             __LINE__);
    }
    m_pDevice->AddRef();

    if(pD3D)
    {
        m_pD3D = pD3D;
        m_pD3D->AddRef();
    }
    else if(pD3DDev)
    {
        if(FAILED(hr = pD3DDev->GetDirect3D(&m_pD3D)))
        {
            throw CD3DXException( hr,
                                 "CVertexArray::Init - Could not get D3D from device",
                                 __LINE__);
        }
    }
    else
    {
        //hr = D3DERR_INVALIDCALL;
        hr = D3DERR_INVALIDCALL;
        throw CD3DXException( hr,
                             "CVertexArray::Init - pD3D is NULL",
                             __LINE__);
    }

    m_dwMaxNumVertices = dwMaxVertexCount;
    m_dwNumTexCoord = 0;
    m_dwVertexSize = 0;
    memset(m_TextureSize, 0, sizeof(m_TextureSize));
    m_FVF = 0;
    for (DWORD i=0; i < dwNumComponents; i++)
    {
        switch (*pComponent++)
        {
        case D3DXVC_XYZ  :
            m_FVF |= D3DFVF_XYZ;
            m_dwVertexSize += 3*sizeof(D3DVALUE);
            break;
        case D3DXVC_XYZRHW  :
            m_FVF |= D3DFVF_XYZRHW;
            m_dwVertexSize += 4*sizeof(D3DVALUE);
            break;
        case D3DXVC_NORMAL:
            m_FVF |= D3DFVF_NORMAL;
            m_dwVertexSize += 3*sizeof(D3DVALUE);
            break;
        case D3DXVC_DIFFUSE:
            m_FVF |= D3DFVF_DIFFUSE;
            m_dwVertexSize += sizeof(DWORD);
            break;
        case D3DXVC_SPECULAR:
            m_FVF |= D3DFVF_SPECULAR;
            m_dwVertexSize += sizeof(DWORD);
            break;
        case D3DXVC_TEX2:
            m_TextureSize[m_dwNumTexCoord] = 2*sizeof(D3DVALUE);
            m_dwNumTexCoord++;
            m_dwVertexSize += 2*sizeof(D3DVALUE);
            break;
#ifdef __DX7_TEXTURES
        case D3DXVC_TEX1:
            m_TextureSize[m_dwNumTexCoord] = sizeof(D3DVALUE);
            m_dwNumTexCoord++;
            m_dwVertexSize += sizeof(D3DVALUE);
            break;
        case D3DXVC_TEX3:
            m_TextureSize[m_dwNumTexCoord] = 3*sizeof(D3DVALUE);
            m_dwNumTexCoord++;
            m_dwVertexSize += 3*sizeof(D3DVALUE);
            break;
        case D3DXVC_TEX4:
            m_TextureSize[m_dwNumTexCoord] = 4*sizeof(D3DVALUE);
            m_dwNumTexCoord++;
            m_dwVertexSize += 4*sizeof(D3DVALUE);
            break;
#endif //__DX7_TEXTURES
        default:
            throw CD3DXException( hr,
                                 "CVertexArray::Init - "\
                                 "invalid vtx component",
                                 __LINE__);
        }
    }

    m_FVF |= m_dwNumTexCoord << 8;

    DWORD dwOffset;
    if (m_FVF & D3DFVF_XYZ)
        dwOffset = 3*sizeof(D3DVALUE);
    else
        dwOffset = 4*sizeof(D3DVALUE);
    if (m_FVF & D3DFVF_NORMAL)
    {
        m_dwNormalOffset = dwOffset;
        dwOffset += 3*sizeof(D3DVALUE);
    }

    if (m_FVF & D3DFVF_DIFFUSE)
    {
        m_dwDiffuseOffset = dwOffset;
        dwOffset += sizeof(DWORD);
    }

    if (m_FVF & D3DFVF_SPECULAR)
    {
        m_dwSpecularOffset = dwOffset;
        dwOffset += sizeof(DWORD);
    }

    for (DWORD j=0; j < m_dwNumTexCoord; j++)
    {
        if (m_TextureSize[j] == 0)
            m_TextureSize[j] = 2*sizeof(D3DVALUE);
        m_dwTextureOffset[j] = dwOffset;
        dwOffset += m_TextureSize[j];
    }

    // need to create a vertex buffer
    hr = m_pDevice->CreateVertexBuffer(m_dwMaxNumVertices * dwOffset, D3DUSAGE_SOFTWAREPROCESSING, m_FVF, D3DPOOL_DEFAULT, &m_vb);
    if( FAILED( hr ) )
    {
        throw CD3DXException( hr,
                             "CVertexArray::Init - error creating VB",
                             __LINE__);
    }
}

void
CVertexArray::SetCurrentVertex( DWORD dwIndex )
{
    m_dwCurrentVertexIndex = dwIndex;

    DWORD dwVertexOffset = dwIndex * m_dwVertexSize;
    if (m_dwCurrentVertexIndex > m_dwMaxNumVertices)
    {
        throw CD3DXException(E_FAIL, "CVertexArray::SetCurrentVertex - Tried adding too many vertices",
                            __LINE__);
    }
}

void
CVertexArray::Begin()
{
    m_dwCurrentVertexIndex = 0;
    if (m_vb)
    {
        HRESULT hr = m_vb->Lock(0,0, &m_pVertices, D3DLOCK_NOSYSLOCK);
        if( FAILED( hr ) )
            throw CD3DXException(hr, "CVertexArray::Begin - error in Lock",
                                __LINE__);
        m_pCurrentVertex = m_pVertices;
    }
}

void
CVertexArray::End()
{
    m_vb->Unlock();
}

void
CVertexArray::Vertex(D3DVALUE x, D3DVALUE y, D3DVALUE z)
{
    if (!(m_FVF & D3DFVF_XYZ))
    {
        throw CD3DXException( D3DERR_INVALIDCALL,
                             "CVertexArray::Vertex - invalid vertex type",
                             __LINE__);
    }

    if (m_dwCurrentVertexIndex > m_dwMaxNumVertices)
    {
        throw CD3DXException(E_FAIL, "CVertexArray::SetCurrentVertex - Tried adding too many vertices",
                            __LINE__);
    }

    ((D3DVALUE*)m_pCurrentVertex)[0] = x;
    ((D3DVALUE*)m_pCurrentVertex)[1] = y;
    ((D3DVALUE*)m_pCurrentVertex)[2] = z;

    m_dwCurrentVertexIndex++;
    m_pCurrentVertex += m_dwVertexSize;
}

void
CVertexArray::Vertex4(D3DVALUE x, D3DVALUE y, D3DVALUE z, D3DVALUE rhw)
{
    if (!(m_FVF & D3DFVF_XYZRHW))
    {
        throw CD3DXException(D3DERR_INVALIDCALL, "CVertexArray::Vertex - "\
                                        "Invalid vertex type", __LINE__);
    }

    if (m_dwCurrentVertexIndex > m_dwMaxNumVertices)
    {
        throw CD3DXException(E_FAIL, "CVertexArray::SetCurrentVertex - Tried adding too many vertices",
                            __LINE__);
    }

    ((D3DVALUE*)m_pCurrentVertex)[0] = x;
    ((D3DVALUE*)m_pCurrentVertex)[1] = y;
    ((D3DVALUE*)m_pCurrentVertex)[2] = z;
    ((D3DVALUE*)m_pCurrentVertex)[3] = rhw;

    m_dwCurrentVertexIndex++;
    m_pCurrentVertex += m_dwVertexSize;
}

void
CVertexArray::Normal(D3DVALUE x, D3DVALUE y, D3DVALUE z)
{
    if (!(m_FVF & D3DFVF_NORMAL))
    {
        throw CD3DXException(D3DERR_INVALIDCALL, "CVertexArray::Normal - "\
                                        "Normal is not allowed", __LINE__);
    }

    *(D3DVALUE*)&m_pCurrentVertex[m_dwNormalOffset] = x;
    *(D3DVALUE*)&m_pCurrentVertex[m_dwNormalOffset+sizeof(D3DVALUE)] = y;
    *(D3DVALUE*)&m_pCurrentVertex[m_dwNormalOffset+2*sizeof(D3DVALUE)] = z;
}

void
CVertexArray::Diffuse(DWORD color)
{
    if (!(m_FVF & D3DFVF_DIFFUSE))
    {
        throw CD3DXException(D3DERR_INVALIDCALL, "CVertexArray::Diffuse - "\
                                        "Diffuse is not allowed", __LINE__);
    }

    *(DWORD*)&m_pCurrentVertex[m_dwDiffuseOffset] = color;
}

void
CVertexArray::Specular(DWORD color)
{
    if (!(m_FVF & D3DFVF_SPECULAR))
    {
        throw CD3DXException(D3DERR_INVALIDCALL, "CVertexArray::Specular - "\
                                        "Specular is not allowed", __LINE__);
    }

    *(DWORD*)&m_pCurrentVertex[m_dwSpecularOffset] = color;
}


void
CVertexArray::Texture2( int index, float s, float t)
{
    switch( index )
    {
    case 0:
        Texture0_2(s, t);
        return;
    case 1:
        Texture1_2(s, t);
        return;
    case 2:
        Texture2_2(s, t);
        return;
    case 3:
        Texture3_2(s, t);
        return;
    case 4:
        Texture4_2(s, t);
        return;
    case 5:
        Texture5_2(s, t);
        return;
    case 6:
        Texture6_2(s, t);
        return;
    case 7:
        Texture7_2(s, t);
        return;
    default:
        throw CD3DXException( D3DERR_INVALIDCALL,
                             "CVertexArray::Texture2 - bad tex-coord index",
                             __LINE__);
    }
}


#define TEXTURE1(Name, TextureIndex)                            \
void CVertexArray::Name(D3DVALUE v)                             \
{                                                               \
    if (m_TextureSize[TextureIndex] != sizeof(D3DVALUE))    \
    {                                                       \
        throw CD3DXException(D3DERR_INVALIDCALL,            \
                            "CVertexArray::Name - invalid texture size", \
                             __LINE__);                     \
    }                                                       \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[0] = v; \
                                                                \
}

#define TEXTURE2(Name, TextureIndex)                            \
void CVertexArray::Name(D3DVALUE v1, D3DVALUE v2)               \
{                                                               \
    if (m_TextureSize[TextureIndex] != 2*sizeof(D3DVALUE))  \
    {                                                       \
        throw CD3DXException(D3DERR_INVALIDCALL,            \
                            "CVertexArray::Name - invalid texture size", \
                             __LINE__);                     \
    }                                                       \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[0] = v1; \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[1] = v2; \
}

#define TEXTURE3(Name, TextureIndex)                            \
void CVertexArray::Name(D3DVALUE v1, D3DVALUE v2, D3DVALUE v3)  \
{                                                               \
    if (m_TextureSize[TextureIndex] != 3*sizeof(D3DVALUE))  \
    {                                                       \
        throw CD3DXException(D3DERR_INVALIDCALL,            \
                            "CVertexArray::Name - invalid texture size", \
                             __LINE__);                     \
    }                                                       \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[0] = v1; \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[1] = v2; \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[2] = v3; \
}

#define TEXTURE4(Name, TextureIndex)                            \
void CVertexArray::Name(D3DVALUE v1, D3DVALUE v2, D3DVALUE v3, D3DVALUE v4) \
{                                                               \
    if (m_TextureSize[TextureIndex] != 4*sizeof(D3DVALUE))  \
    {                                                       \
        throw CD3DXException(D3DERR_INVALIDCALL,            \
                            "CVertexArray::Name - invalid texture size", \
                             __LINE__);                     \
    }                                                       \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[0] = v1; \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[1] = v2; \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[2] = v3; \
    ((D3DVALUE*)&m_pCurrentVertex[m_dwTextureOffset[TextureIndex]])[3] = v4; \
}


TEXTURE2(Texture0_2, 0)
TEXTURE2(Texture1_2, 1)
TEXTURE2(Texture2_2, 2)
TEXTURE2(Texture3_2, 3)
TEXTURE2(Texture4_2, 4)
TEXTURE2(Texture5_2, 5)
TEXTURE2(Texture6_2, 6)
TEXTURE2(Texture7_2, 7)

#ifdef __DX7_TEXTURES
TEXTURE1(Texture0_1, 0)
TEXTURE1(Texture1_1, 1)
TEXTURE1(Texture2_1, 2)
TEXTURE1(Texture3_1, 3)
TEXTURE1(Texture4_1, 4)
TEXTURE1(Texture5_1, 5)
TEXTURE1(Texture6_1, 6)
TEXTURE1(Texture7_1, 7)

TEXTURE3(Texture0_3, 0)
TEXTURE3(Texture1_3, 1)
TEXTURE3(Texture2_3, 2)
TEXTURE3(Texture3_3, 3)
TEXTURE3(Texture4_3, 4)
TEXTURE3(Texture5_3, 5)
TEXTURE3(Texture6_3, 6)
TEXTURE3(Texture7_3, 7)

TEXTURE4(Texture0_4, 0)
TEXTURE4(Texture1_4, 1)
TEXTURE4(Texture2_4, 2)
TEXTURE4(Texture3_4, 3)
TEXTURE4(Texture4_4, 4)
TEXTURE4(Texture5_4, 5)
TEXTURE4(Texture6_4, 6)
TEXTURE4(Texture7_4, 7)
#endif //__DX7_TEXTURES

#ifdef __HAVE_DRAWING_MEMBERS
void CVertexArray::Draw(D3DPRIMITIVETYPE  primType,
                        DWORD dwFirstVertexIndex, DWORD dwNumVertices)
{
    HRESULT hr = S_OK;

    m_vb->Unlock();
    hr = m_pDevice->DrawPrimitiveVB(primType, m_vb, dwFirstVertexIndex,
                                    dwNumVertices, 0);
    if ( hr != D3D_OK)
    {
        throw CD3DXException( hr, "CVertexArrayIM::Draw", __LINE__);
    }
    hr = m_vb->Lock(DDLOCK_WAIT, (LPVOID*)&m_pVertices, 0);
    if ( hr != D3D_OK)
        throw CD3DXException( hr, "CVertexArray::Vertex - error in Lock",
                            __LINE__);
    m_pCurrentVertex = m_pVertices;
}

void CVertexArray::DrawIndexed(D3DPRIMITIVETYPE primType,
                               WORD * pwIndices,
                               DWORD dwNumIndices)
{
    HRESULT hr = S_OK;

    m_vb->Unlock();
     hr = m_pDevice->DrawIndexedPrimitiveVB(primType,
                                            m_vb,
                                            0,
                                            m_dwMaxNumVertices,
                                            pwIndices,
                                            dwNumIndices,
                                            0);
    if ( hr != D3D_OK)
    {
        throw CD3DXException( hr, "CVertexArrayIM::Draw", __LINE__);
    }
    hr = m_vb->Lock(DDLOCK_WAIT, (LPVOID*)&m_pVertices, 0);
    if ( hr != D3D_OK)
        throw CD3DXException( hr, "CVertexArray::Vertex - "\
                                  "Error in Lock", __LINE__);
    m_pCurrentVertex = m_pVertices;
}
#endif //__HAVE_DRAWING_MEMBERS


