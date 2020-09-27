#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "xip.h"

extern bool bQuiet;

#define MAKE_MESHID(fvfIndex, meshIndex) ((fvfIndex << 24) | (meshIndex))
#define INVALID_MESHID 0xffffffff

struct CMeshBuffer
{
	DWORD m_fvf;
	int m_nVertexStride;

	void* m_vertices;
	int m_nVertexCount;

	WORD* m_indices;
	int m_nIndexCount;
/*
	int* m_rgnMeshEnd;
	int m_nMeshCount;
*/
};

#define MAX_MESHBUFFER 10
CMeshBuffer rgMeshBuffer [MAX_MESHBUFFER];
int nMeshBufferCount = 0;

int FindMeshBuffer(DWORD fvf, int nVerts)
{
	for (int i = 0; i < nMeshBufferCount; i += 1)
	{
		if (rgMeshBuffer[i].m_fvf == fvf && rgMeshBuffer[i].m_nVertexCount + nVerts <= 65536)
			return i;
	}

	if (nMeshBufferCount >= MAX_MESHBUFFER)
	{
		fprintf(stderr, "ERROR: too many mesh buffers!\n");
		exit(1);
	}

	if (!bQuiet)
		printf("Adding buffer for FVF:%08x\n", fvf);

	nMeshBufferCount += 1;
	rgMeshBuffer[i].m_fvf = fvf;

	return i;
}

typedef enum _D3DPRIMITIVETYPE {
    D3DPT_POINTLIST             = 1,
    D3DPT_LINELIST              = 2,
    D3DPT_LINESTRIP             = 3,
    D3DPT_TRIANGLELIST          = 4,
    D3DPT_TRIANGLESTRIP         = 5,
    D3DPT_TRIANGLEFAN           = 6,

    D3DPT_FORCE_DWORD           = 0x7fffffff
} D3DPRIMITIVETYPE;


bool AddMesh(const char* szFileName, DWORD& dwMeshID, DWORD& dwPrimCount)
{
	D3DPRIMITIVETYPE m_primitiveType;
	int m_nFaceCount;
	DWORD m_fvf;
	int m_nVertexStride;
	int m_nVertexCount;
	int m_nIndexCount;

	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwValue, dwRead;

	ReadFile(hFile, &dwValue, 4, &dwRead, NULL);
	m_primitiveType = (D3DPRIMITIVETYPE)dwValue;

	ReadFile(hFile, &dwValue, 4, &dwRead, NULL);
	m_nFaceCount = dwValue;

	ReadFile(hFile, &dwValue, 4, &dwRead, NULL);
	m_fvf = dwValue;

	ReadFile(hFile, &dwValue, 4, &dwRead, NULL);
	m_nVertexStride = dwValue;

	ReadFile(hFile, &dwValue, 4, &dwRead, NULL);
	m_nVertexCount = dwValue;

	ReadFile(hFile, &dwValue, 4, &dwRead, NULL);
	m_nIndexCount = dwValue;


	int fvfIndex = FindMeshBuffer(m_fvf, m_nVertexCount);
	CMeshBuffer& mb = rgMeshBuffer[fvfIndex];
	int meshIndex = mb.m_nIndexCount;

	mb.m_nVertexStride = m_nVertexStride;

	BYTE* vertices = new BYTE [(mb.m_nVertexCount + m_nVertexCount) * mb.m_nVertexStride];
	CopyMemory(vertices, mb.m_vertices, mb.m_nVertexCount * mb.m_nVertexStride);
	ReadFile(hFile, vertices + mb.m_nVertexCount * mb.m_nVertexStride, m_nVertexCount * m_nVertexStride, &dwRead, NULL);
	delete [] mb.m_vertices;
	mb.m_vertices = vertices;
	mb.m_nVertexCount += m_nVertexCount;

	WORD* indices = new WORD [mb.m_nIndexCount + m_nIndexCount];
	CopyMemory(indices, mb.m_indices, mb.m_nIndexCount * sizeof (WORD));
	ReadFile(hFile, indices + mb.m_nIndexCount, m_nIndexCount * sizeof (WORD), &dwRead, NULL);
	for (int i = 0; i < m_nIndexCount; i += 1)
		indices[mb.m_nIndexCount + i] += mb.m_nVertexCount - m_nVertexCount;
	delete [] mb.m_indices;
	mb.m_indices = indices;
	mb.m_nIndexCount += m_nIndexCount;
/*
	int* rgnMeshEnd = new int [mb.m_nMeshCount + 1];
	CopyMemory(rgnMeshEnd, mb.m_rgnMeshEnd, mb.m_nMeshCount * sizeof (int));
	rgnMeshEnd[mb.m_nMeshCount] = mb.m_nIndexCount;
	delete [] mb.m_rgnMeshEnd;
	mb.m_rgnMeshEnd = rgnMeshEnd;
	mb.m_nMeshCount += 1;
*/
	CloseHandle(hFile);

	dwMeshID = MAKE_MESHID(fvfIndex, meshIndex);
	dwPrimCount = m_nIndexCount / 3;

	return true;
}


void WriteMergedMeshFiles(CXipCreator* pCreator)
{
	for (int i = 0; i < nMeshBufferCount; i += 1)
	{
		CMeshBuffer& mb = rgMeshBuffer[i];

		char szName [MAX_PATH];
		DWORD dwWrite;

		sprintf(szName, "~%d.ib", i);
		HANDLE hFile = CreateFile(szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		WriteFile(hFile, mb.m_indices, mb.m_nIndexCount * sizeof (WORD), &dwWrite, NULL);
		CloseHandle(hFile);
		pCreator->AddFile(szName);

		sprintf(szName, "~%d.vb", i);
		hFile = CreateFile(szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		WriteFile(hFile, &mb.m_nVertexCount, sizeof (int), &dwWrite, NULL);
		WriteFile(hFile, &mb.m_fvf, sizeof (DWORD), &dwWrite, NULL);
		WriteFile(hFile, mb.m_vertices, mb.m_nVertexCount * mb.m_nVertexStride, &dwWrite, NULL);
		CloseHandle(hFile);
		pCreator->AddFile(szName);
	}
}
