#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Shape.h"
#include "Runner.h"
#include "Texture.h"
#include "xip.h"
#include "Mesh.h"
#include "FileUtil.h"

#ifdef COOL_XDASH
#include "Camera.h"
extern CCamera theCamera;
extern UINT g_uMesh;
#endif

extern BOOL g_bEdgeAntialiasOverride;

CMesh::CMesh()
{
//	m_bOwnMemory = true;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_nVertexStride = 0;
	m_nFaceCount = 0;
	m_nVertexCount = 0;
	m_nIndexCount = 0;
	m_fvf = 0;
	m_primitiveType = (D3DPRIMITIVETYPE)0;
}

CMesh::~CMesh()
{
//	if (m_bOwnMemory)
	{
		if (m_pVertexBuffer != NULL)
			m_pVertexBuffer->Release();

		if (m_pIndexBuffer != NULL)
			m_pIndexBuffer->Release();
	}
}

DWORD CMesh::GetFVF() const
{
    ASSERT(m_fvf != D3DFVF_RESERVED0);
	return m_fvf;
}

bool CMesh::Create(BYTE* pbData, DWORD dwData)
{
	MESHFILEHEADER* pHeader = (MESHFILEHEADER*)pbData;
	pbData += sizeof (MESHFILEHEADER);

#ifdef _XBOX
	switch (pHeader->dwPrimitiveType)
	{
	default:
		ASSERT(FALSE); // unsupported primitive type
		return false;

	case 4:
		m_primitiveType = D3DPT_TRIANGLELIST;
		break;

	case 5:
		m_primitiveType = D3DPT_TRIANGLESTRIP;
		break;
	}
#else
	m_primitiveType = (D3DPRIMITIVETYPE)pHeader->dwPrimitiveType;
#endif

	m_nFaceCount = pHeader->dwFaceCount;
	m_fvf = pHeader->dwFVF;
	m_nVertexStride = pHeader->dwVertexStride;
	m_nVertexCount = pHeader->dwVertexCount;
	m_nIndexCount = pHeader->dwIndexCount;

	XAppCreateVertexBuffer(m_nVertexCount * m_nVertexStride, D3DUSAGE_DYNAMIC, m_fvf, D3DPOOL_DEFAULT, &m_pVertexBuffer);

	BYTE* verts;
#ifdef _XBOX
	const DWORD dwLockFlags = D3DLOCK_DISCARD | D3DLOCK_NOFLUSH;
#else
	const DWORD dwLockFlags = D3DLOCK_DISCARD;
#endif
	VERIFYHR(m_pVertexBuffer->Lock(0, 0, &verts, dwLockFlags));
	CopyMemory(verts, pbData, m_nVertexCount * m_nVertexStride);
	pbData += m_nVertexCount * m_nVertexStride;
	VERIFYHR(m_pVertexBuffer->Unlock());

	XAppCreateIndexBuffer(m_nIndexCount * sizeof (WORD), D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer);

	BYTE* indices;
	VERIFYHR(m_pIndexBuffer->Lock(0, m_nIndexCount * sizeof (WORD), (BYTE**)&indices, dwLockFlags));
	CopyMemory(indices, pbData, m_nIndexCount * sizeof (WORD));
	VERIFYHR(m_pIndexBuffer->Unlock());

	return true;
}

bool CMesh::Create(HANDLE hFile)
{
	DWORD dwRead;
	MESHFILEHEADER header;

	VERIFY(ReadFile(hFile, &header, sizeof (header), &dwRead, NULL) && dwRead == sizeof (header));

#ifdef _XBOX
	switch (header.dwPrimitiveType)
	{
	default:
		ASSERT(FALSE); // unsupported primitive type
		return false;

	case 4:
		m_primitiveType = D3DPT_TRIANGLELIST;
		break;

	case 5:
		m_primitiveType = D3DPT_TRIANGLESTRIP;
		break;
	}
#else
	m_primitiveType = (D3DPRIMITIVETYPE)header.dwPrimitiveType;
#endif

	m_nFaceCount = header.dwFaceCount;
	m_fvf = header.dwFVF;
    ASSERT(m_fvf != D3DFVF_RESERVED0);
	m_nVertexStride = header.dwVertexStride;
	m_nVertexCount = header.dwVertexCount;
	m_nIndexCount = header.dwIndexCount;

    XAppCreateVertexBuffer(m_nVertexCount * m_nVertexStride, D3DUSAGE_DYNAMIC, m_fvf, D3DPOOL_DEFAULT, &m_pVertexBuffer);

	BYTE* verts;
#ifdef _XBOX
	const DWORD dwLockFlags = D3DLOCK_DISCARD | D3DLOCK_NOFLUSH;
#else
	const DWORD dwLockFlags = D3DLOCK_DISCARD;
#endif
	VERIFYHR(m_pVertexBuffer->Lock(0, 0, &verts, dwLockFlags));
	VERIFY(ReadFile(hFile, verts, m_nVertexCount * m_nVertexStride, &dwRead, NULL) && dwRead == (DWORD)(m_nVertexCount * m_nVertexStride));
	VERIFYHR(m_pVertexBuffer->Unlock());

	XAppCreateIndexBuffer(m_nIndexCount * sizeof (WORD), D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer);

	BYTE* indices;
	VERIFYHR(m_pIndexBuffer->Lock(0, m_nIndexCount * sizeof (WORD), (BYTE**)&indices, D3DLOCK_DISCARD));
	VERIFY(ReadFile(hFile, indices, m_nIndexCount * sizeof (WORD), &dwRead, NULL) && dwRead == m_nIndexCount * sizeof (WORD));
	VERIFYHR(m_pIndexBuffer->Unlock());

	return true;
}

bool CMesh::Load(const TCHAR* szFilePath)
{
	HANDLE hFile = XAppCreateFile(szFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("\001Cannot load MeshNode: %s\n"), szFilePath);
		return false;
	}

	bool b = Create(hFile);

	CloseHandle(hFile);

	return b;
}

void CMesh::Render(bool bSetFVF)
{
	if (m_pVertexBuffer == NULL || m_pIndexBuffer == NULL)
		return;

	ASSERT(m_primitiveType != 0); // forget to set this?

	if (bSetFVF)
		XAppSetVertexShader(GetFixedFunctionShader(m_fvf));

	if (m_nFaceCount > 800 && !g_bEdgeAntialiasOverride) {
		XAppSetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
		XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		//XAppSetRenderState(D3DRS_MULTISAMPLETYPE, D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN);
	}

#ifdef COOL_XDASH

    D3DMATRIX matOldView;

    if (g_uMesh == 0) {
        // Turn off breathing for the background sphere in the ugliest way imaginable...
        D3DXMATRIX matPosition, matView;
	    D3DXMatrixAffineTransformation(&matPosition, 1.0f, NULL, &theCamera.m_orientation, &theCamera.m_position);
	    D3DXMatrixInverse(&matView, NULL, &matPosition);
	    matView._31 = -matView._31;
	    matView._32 = -matView._32;
	    matView._33 = -matView._33;
	    matView._34 = -matView._34;
        XAppGetTransform(D3DTS_VIEW, &matOldView);
	    XAppSetTransform(D3DTS_VIEW, &matView);
    }

#endif

	XAppSetStreamSource(0, m_pVertexBuffer, m_nVertexStride);
	XAppSetIndices(m_pIndexBuffer, 0);
	XAppDrawIndexedPrimitive(m_primitiveType, 0, m_nVertexCount, 0, m_nFaceCount);

#ifdef _WINDOWS
	theApp.m_nVertPerFrame += m_nVertexCount;
	theApp.m_nTriPerFrame += m_nFaceCount;
#endif

#ifdef COOL_XDASH
    if (g_uMesh == 0) {
        XAppSetTransform(D3DTS_VIEW, &matOldView);
    }

    g_uMesh++;
#endif
}

////////////////////////////////////////////////////////////////////////////

class CMeshNode* g_pRenderMeshNode = NULL;

CMeshNode* CMeshNode::c_pFirst;

IMPLEMENT_NODE("Mesh", CMeshNode, CNode)

START_NODE_PROPS(CMeshNode, CNode)
	NODE_PROP(pt_string, CMeshNode, url)
END_NODE_PROPS()

START_NODE_FUN(CMeshNode, CNode)
	NODE_FUN_VS(load)
END_NODE_FUN()

CMeshNode::CMeshNode() :
	m_url(NULL)
{
	m_pNext = c_pFirst;
	c_pFirst = this;
	m_renderTime = 0.0f;

	m_bDirty = true;
	m_pMesh = NULL;
	m_bOwnMesh = true;
}

CMeshNode::~CMeshNode()
{
	if (m_bOwnMesh)
		delete m_pMesh;

	delete [] m_url;

	for (CMeshNode** ppMeshNode = &c_pFirst; *ppMeshNode != this; ppMeshNode = &(*ppMeshNode)->m_pNext)
		ASSERT(*ppMeshNode != NULL);
	*ppMeshNode = m_pNext;
}

bool CMeshNode::Initialize()
{
	ASSERT(m_bDirty);

	if (m_url != NULL && m_url[0] != 0)
		load(m_url);

	Init();

	return m_pMesh != NULL;
}

void CMeshNode::Init()
{
	m_bDirty = false;
}

void CMeshNode::Render()
{
	m_renderTime = XAppGetNow();

	if (m_bDirty && !Initialize())
		return;

	if (m_pMesh != NULL)
		m_pMesh->Render(g_pRenderMeshNode != this); // REVIEW: Does this ever happen anymore?
}

void CMeshNode::load(const TCHAR* szFile)
{
	ASSERT(m_pMesh == NULL);

	TCHAR szFilePath [MAX_PATH];
	MakeAbsoluteURL(szFilePath, szFile);

	m_pMesh = (CMeshCore*)FindObjectInXIP(szFilePath, szFile);

	if (m_pMesh == NULL)
	{
		m_bOwnMesh = true;
		CMesh* pMesh = LoadMesh(szFilePath);
		m_pMesh = pMesh;
		if (pMesh->GetFVF() == 0)
		{
			delete pMesh;
			MakePath(szFilePath, theApp.m_szAppDir, szFile);
			pMesh = LoadMesh(szFilePath);
			m_pMesh = pMesh;
			if (pMesh->GetFVF() == 0)
				return;
		}
		TRACE(_T("\002Loaded %s from file\n"), szFilePath);
	}
	else
	{
		m_bOwnMesh = false;
//		TRACE(_T("Loaded %s from XIP\n"), szFilePath);
	}
}

DWORD CMeshNode::GetFVF()
{
	if (m_pMesh == NULL)
		return 0;

	return m_pMesh->GetFVF();
}

////////////////////////////////////////////////////////////////////////////

CMesh* LoadMesh(const TCHAR* szFilePath)
{
	CMesh* pMesh = new CMesh;
	pMesh->Load(szFilePath);
	return pMesh;
}

CMesh* CreateMesh(HANDLE hFile)
{
	CMesh* pMesh = new CMesh;
	pMesh->Create(hFile);
	return pMesh;
}

CMesh* CreateMesh(BYTE* pbContent, DWORD cbContent)
{
	CMesh* pMesh = new CMesh;
	pMesh->Create(pbContent, cbContent);
	return pMesh;
}

CMesh* MakeSphere(float nRadius, int nSlices, int nStacks)
{
    HRESULT hr;
	
	CMesh* pMesh = new CMesh;

	pMesh->m_primitiveType = D3DPT_TRIANGLELIST;

	pMesh->m_fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE /*| D3DFVF_TEX1*/;
	pMesh->m_nVertexStride = 3 * sizeof (float) + 3 * sizeof (float) + sizeof (DWORD) /*+ 2 * sizeof (float)*/;

	// REVIEW: Tacky (and inefficient) way to create a sphere!

	LPD3DXMESH pSphere = NULL;
	LPD3DXMESH pClone = NULL;

	hr = D3DXCreateSphere(XAppGetD3DDev(), nRadius, nSlices, nStacks, &pSphere, NULL);

    if (SUCCEEDED(hr))
    {
        ASSERT(pSphere);
	    hr = pSphere->CloneMeshFVF(D3DXMESH_MANAGED, pMesh->m_fvf, XAppGetD3DDev(), &pClone);
        pSphere->Release();
    }

    if (SUCCEEDED(hr))
    {
        ASSERT(pClone);
	    hr = D3DXComputeNormals(pClone);
    }

    if (SUCCEEDED(hr))
    {
    	hr = pClone->GetVertexBuffer(&pMesh->m_pVertexBuffer);
    }

    if (SUCCEEDED(hr))
    {
	    hr = pClone->GetIndexBuffer(&pMesh->m_pIndexBuffer);
    }

    if (SUCCEEDED(hr))
    {
        pMesh->m_nIndexCount = pClone->GetNumFaces() * 3;
        pMesh->m_nFaceCount = pClone->GetNumFaces();
        pMesh->m_nVertexCount = pClone->GetNumVertices();
    }
    else
    {
        if (pMesh->m_pVertexBuffer)
        {
            pMesh->m_pVertexBuffer->Release();
        }
        if (pMesh->m_pIndexBuffer)
        {
            pMesh->m_pIndexBuffer->Release();
        }
        delete pMesh;
        pMesh = NULL;
    }

	if (pClone)
    {
        pClone->Release();
    }

	// Compress the vertices
	LPDIRECT3DVERTEXBUFFER8 pCompressedVertexBuffer;
	BYTE *pSrc, *pDst;
	DWORD dwNormal;

	XAppCreateVertexBuffer(pMesh->m_nVertexCount * (3 * sizeof(float) + 2 * sizeof(DWORD)), D3DUSAGE_DYNAMIC, 0, D3DPOOL_MANAGED, &pCompressedVertexBuffer);

	pMesh->m_pVertexBuffer->Lock(0, 0, &pSrc, 0);
	pCompressedVertexBuffer->Lock(0, 0, &pDst, 0);

	for (int i = 0; i < pMesh->m_nVertexCount; i++) 
	{
		memcpy(pDst, pSrc, 3 * sizeof(float));
		pSrc += 3 * sizeof(float);
		pDst += 3 * sizeof(float);
		dwNormal = CompressNormal((float*)pSrc);
		memcpy(pDst, &dwNormal, sizeof(DWORD));
		pSrc += 3 * sizeof(float);
		pDst += sizeof(DWORD);
		memcpy(pDst, pSrc, sizeof(DWORD));
		pSrc += sizeof(DWORD);
		pDst += sizeof(DWORD);
	}

	pCompressedVertexBuffer->Unlock();
	pMesh->m_pVertexBuffer->Unlock();

	pMesh->m_pVertexBuffer->Release();
	pMesh->m_pVertexBuffer = pCompressedVertexBuffer;
	pMesh->m_fvf = D3DFVF_XYZ | D3DFVF_NORMPACKED3 | D3DFVF_DIFFUSE /*| D3DFVF_TEX1*/;
	pMesh->m_nVertexStride = 3 * sizeof (float) + sizeof (DWORD) + sizeof (DWORD) /*+ 2 * sizeof (float)*/;

	return pMesh;
}
