#pragma once

struct MESHFILEHEADER
{
	DWORD dwPrimitiveType;
	DWORD dwFaceCount;
	DWORD dwFVF;
	DWORD dwVertexStride;
	DWORD dwVertexCount;
	DWORD dwIndexCount;
};

class CMeshCore
{
public:
	virtual void Render(bool bSetFVF = true) = 0;
	virtual DWORD GetFVF() const = 0;
};

class CMesh : public CMeshCore
{
public:
	CMesh();
	virtual ~CMesh();

	bool Load(const TCHAR* szFilePath);
	bool Create(HANDLE hFile);
	bool Create(BYTE* pbContent, DWORD cbContent);
	void Render(bool bSetFVF = true);
	DWORD GetFVF() const;

	int m_nFaceCount;
	int m_nVertexCount;
	int m_nIndexCount;
	D3DPRIMITIVETYPE m_primitiveType;
	DWORD m_fvf;
	int m_nVertexStride;
	IDirect3DVertexBuffer8* m_pVertexBuffer;
	IDirect3DIndexBuffer8* m_pIndexBuffer;

	friend class CIndexedFaceSet;
};

class CMeshRef : public CMeshCore
{
public:
	void Render(bool bSetFVF = true);
	DWORD GetFVF() const;

	class CXipFile* m_pXipFile;
	int m_nMeshBuffer;

	int m_nFirstIndex;
	int m_nPrimitiveCount;
};

extern CMesh* LoadMesh(const TCHAR* szFilePath);
extern CMesh* CreateMesh(HANDLE hFile);
extern CMesh* CreateMesh(BYTE* pbContent, DWORD cbContent);
extern CMesh* MakeSphere(float nRadius, int nSlices, int nStacks);

