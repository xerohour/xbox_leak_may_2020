//-----------------------------------------------------------------------------
// File: Mesh.h
//
// Desc: Mesh object
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_MESH_H
#define TECH_CERT_GAME_MESH_H

#include "Common.h"
#include "Shader.h"




//-----------------------------------------------------------------------------
// Name: class MeshElement
// Desc: Mesh element
//-----------------------------------------------------------------------------
class MeshElement
{
    Shader*                 m_pShader;
    UINT                    m_uNumPrimitives;
    D3DPRIMITIVETYPE        m_PrimType;
    UINT                    m_uIndexBufferCount;
    LPDIRECT3DINDEXBUFFER8  m_pIndexBuffer;
    DWORD                   m_dwFVF;
    INT                     m_iBasisVectorOffset;
    UINT                    m_uNumVertices;
    UINT                    m_uVertexSize;
    LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

    friend class Box;
    friend class Sphere;
	friend class Mesh;
	friend class HeightField;

public:

    MeshElement();
    MeshElement( DWORD dwFVF, UINT uNumVerts, UINT uNumIndices );
    virtual ~MeshElement();

    virtual HRESULT Save( const File& );
    virtual HRESULT Load( const File& );
    virtual VOID    Render();
    VOID            AddBasisVectors();

    inline UINT GetNumVerts() const
    {
        return m_uNumVertices;
    }

    inline D3DPRIMITIVETYPE GetPrimType() const
    {
        return m_PrimType;
    }

    inline UINT GetIndexBufferCount() const
    {
        return m_uIndexBufferCount;
    }

    inline const LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer() const
    {
        return m_pVertexBuffer;
    }

    inline UINT GetVertexSize() const
    {
        return m_uVertexSize;
    }

    inline const LPDIRECT3DINDEXBUFFER8 GetIndexBuffer() const
    {
        return m_pIndexBuffer;
    }


private:

	HRESULT AllocateVertices(DWORD dwFVF, UINT uNumVerts);
	HRESULT AllocateIndices(UINT uNumIndices);

    VOID CalculateBasisVectors( INT iUVOffset, INT iBVOffset );
    static DWORD ConstantToIndex( DWORD dwConst, const DWORD* pTable );
    static DWORD IndexToConstant( DWORD dwIndex, const DWORD* pTable );
    static DWORD ComputeFVFOffset( DWORD dwComponentFVF, DWORD dwFVF );

};




//-----------------------------------------------------------------------------
// Name: class Mesh
// Desc: List of MeshElements
//-----------------------------------------------------------------------------
class Mesh
{

protected:

    INT          m_iNumMeshElements;
    MeshElement* m_Elements;

    D3DXVECTOR3  m_v3BoxMin, m_v3BoxMax;

	friend class HeightField;

public:

    Mesh();
    Mesh( INT iNumElements );
    virtual ~Mesh();
    
    virtual HRESULT Save( const File& );
    virtual HRESULT Load( const File& );
    VOID            Render();
    VOID            AddBasisVectors();

    inline INT GetNumElements() const
    {
        return m_iNumMeshElements;
    }

    inline MeshElement* GetElement( INT i )
    {
        assert( i < m_iNumMeshElements );
        return m_Elements + i;
    }

    inline const MeshElement* GetElement( INT i ) const
    {
        assert( i < m_iNumMeshElements );
        return m_Elements + i;
    }

	inline const D3DXVECTOR3& GetBoxMin()
	{
		return m_v3BoxMin;
	}

	inline const D3DXVECTOR3& GetBoxMax()
	{
		return m_v3BoxMax;
	}

#if !defined(_XBOX)
	static Mesh* CreateFromXFile(LPDIRECT3DDEVICE8 pd3dDev, char* sz_fname);
#endif
};




//-----------------------------------------------------------------------------
// Name: class Box
// Desc: Box mesh
//-----------------------------------------------------------------------------
class Box : public Mesh
{

public:

    Box( FLOAT width, FLOAT height, FLOAT depth, const D3DMATERIAL8& d3dMat );

private:

    // Disabled
    Box();

};




//-----------------------------------------------------------------------------
// Name: class Sphere
// Desc: Sphere mesh
//-----------------------------------------------------------------------------
class Sphere : public Mesh
{

public:

    Sphere( FLOAT radius, INT numMajor, INT numMinor, const D3DMATERIAL8& d3dMat );

private:

    // Disabled
    Sphere();

};




#endif // TECH_CERT_GAME_MESH_H
