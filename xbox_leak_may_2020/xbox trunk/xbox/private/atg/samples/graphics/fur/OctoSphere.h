//-----------------------------------------------------------------------------
//  
//  File: OctoSphere.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//-----------------------------------------------------------------------------
#ifdef _XBOX
#include <xtl.h>
#endif
#include <d3dx8.h>
 
extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//-----------------------------------------------------------------------------
// Name: FillOctoSphere
// Desc: Create a sphere by sampling the faces of an octohedron and renormalizing.
//       Pass NULL pointers to aVertex or aIndex to return needed sizes of buffers.
//-----------------------------------------------------------------------------
HRESULT FillOctoSphere(UINT nSplit, // number of splits on each edge, 0 = octohedron
					  UINT *pnVertex, D3DXVECTOR3 *rVertex, // vertices
					  UINT *pnIndex, WORD *rIndex); // triangle indices

//-----------------------------------------------------------------------------
// Name: OctoSphere
// Desc: Wrapper to handle creation of vertex and index buffers.
//-----------------------------------------------------------------------------
struct OctoSphere { // octohedron sphere
	UINT m_nVertex;
	LPDIRECT3DVERTEXBUFFER8  m_pVB;
	UINT m_nIndex;
	LPDIRECT3DINDEXBUFFER8  m_pIB;

	~OctoSphere()
	{
		Cleanup();
	}
	
	void Cleanup()
	{
		if (m_pIB)
		{
			m_pIB->Release();
			m_pIB = NULL;
		}
		if (m_pVB)
		{
			m_pVB->Release();
			m_pVB = NULL;
		}
	}
	
	HRESULT Initialize(UINT nSplit)
	{
		HRESULT hr;
		DWORD dwUsage = 0;
		hr = FillOctoSphere(nSplit, &m_nVertex, NULL, &m_nIndex, NULL); // get needed size of buffers
		if (FAILED(hr))
			return hr;
		hr = g_pd3dDevice->CreateVertexBuffer(m_nVertex * sizeof(float) * 3, dwUsage, D3DFVF_XYZ, D3DPOOL_MANAGED, &m_pVB);
		if (FAILED(hr))
			return hr;
		hr = g_pd3dDevice->CreateIndexBuffer(m_nIndex * sizeof(WORD), dwUsage, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB);
		if (FAILED(hr))
			return hr;
		D3DXVECTOR3 *pVertex;
		hr = m_pVB->Lock(0, m_nVertex * sizeof(float) * 3, (BYTE **)&pVertex, 0);
		if (FAILED(hr))
			return hr;
		WORD *pIndex;
		hr = m_pIB->Lock(0, m_nIndex * sizeof(WORD), (BYTE **)&pIndex, 0);
		if (FAILED(hr))
		{
			m_pVB->Unlock();
			return hr;
		}
		hr = FillOctoSphere(nSplit, &m_nVertex, pVertex, &m_nIndex, pIndex); // fill in buffers
		m_pIB->Unlock();
		m_pVB->Unlock();
		if (FAILED(hr))
			return hr;
		return S_OK;
	}

	HRESULT Draw()
	{
		g_pd3dDevice->SetVertexShader(D3DFVF_XYZ);
		g_pd3dDevice->SetStreamSource(0, m_pVB, sizeof(float) * 3);
		g_pd3dDevice->SetIndices(m_pIB, 0);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_nVertex, 0, m_nIndex/3);
		return S_OK;
	}
};
