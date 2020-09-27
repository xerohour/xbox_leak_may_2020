#ifndef __VBWRAP_H__
#define __VBWRAP_H__

typedef enum _D3DXVERTEXCOMPONENT 
{
    D3DXVC_XYZ, 
    D3DXVC_XYZRHW, 
    D3DXVC_NORMAL,
    D3DXVC_DIFFUSE,
    D3DXVC_SPECULAR,
    D3DXVC_TEX2 
} D3DXVERTEXCOMPONENT;

class CVertexArray
{
public:
    CVertexArray();
    ~CVertexArray();

    void Init( LPDIRECT3D8 pD3D,
               LPDIRECT3DDEVICE8 pD3DDev,
               DWORD dwMaxVertexCount, 
               D3DXVERTEXCOMPONENT*, DWORD dwNumComponents, 
               DWORD dwFlags);

    void Begin();
    void End();

#ifdef __HAVE_DRAWING_MEMBERS
    void Draw(D3DPRIMITIVETYPE, DWORD dwFirstVertexIndex, 
                            DWORD dwNumVertices);
    void DrawIndexed(D3DPRIMITIVETYPE, LPWORD pwIndices, 
                            DWORD dwNumIndices);
#endif //__HAVE_DRAWING_MEMBERS

    void  SetCurrentVertex(DWORD dwIndex);
    DWORD GetCurrentVertexIndex();
    DWORD GetNumVertices();
    void  CopyVertex(DWORD dwSrcIndex, DWORD dwDestIndex);

    void Vertex(float x, float y, float z);
    void Vertex4(float x, float y, float z, float rhw);
    void Normal(float x, float y, float z);
    void Diffuse(DWORD color);
    void Specular(DWORD color);
    void Diffuse( float r, float g, float b );
    void Specular( float r, float g, float b );
    void Diffuse( float r, float g, float b, float a );
    void Specular( float r, float g, float b, float a );

    DWORD GetVertex(float*);
    void  GetNormal(float*);
    DWORD GetDiffuse();
    DWORD GetSpecular();

    void  GetTexture( int index, float*);
    void  GetTexture0(float*);
    void  GetTexture1(float*);
    void  GetTexture2(float*);
    void  GetTexture3(float*);
    void  GetTexture4(float*);
    void  GetTexture5(float*);
    void  GetTexture6(float*);
    void  GetTexture7(float*);

    void Texture2( int index, float, float);
    void Texture0_2(float, float);
    void Texture1_2(float, float);
    void Texture2_2(float, float);
    void Texture3_2(float, float);
    void Texture4_2(float, float);
    void Texture5_2(float, float);
    void Texture6_2(float, float);
    void Texture7_2(float, float);

    LPDIRECT3DVERTEXBUFFER8 m_vb;
protected:
    DWORD   m_FVF;
    DWORD   m_dwGrowSize;
    DWORD   m_dwMaxNumVertices;
    LPBYTE  m_pVertices;
    LPBYTE  m_pCurrentVertex;
    DWORD   m_dwVertexSize;
    DWORD   m_dwCurrentVertexIndex;
    DWORD   m_dwNormalOffset;
    DWORD   m_dwDiffuseOffset;
    DWORD   m_dwSpecularOffset;
    DWORD   m_dwNumTexCoord;
    DWORD   m_dwTextureOffset[8];
    BYTE    m_TextureSize[8];
    D3DVERTEXBUFFER_DESC m_vbDesc;

    LPDIRECT3D8 m_pD3D;
    LPDIRECT3DDEVICE8 m_pDevice;
};

#endif //__VBWRAP_H__
