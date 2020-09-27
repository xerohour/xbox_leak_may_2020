// xm.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include <stdio.h>
#include <math.h>
#include "TriStrip.h"

typedef enum _D3DPRIMITIVETYPE {
    D3DPT_POINTLIST             = 1,
    D3DPT_LINELIST              = 2,
    D3DPT_LINESTRIP             = 3,
    D3DPT_TRIANGLELIST          = 4,
    D3DPT_TRIANGLESTRIP         = 5,
    D3DPT_TRIANGLEFAN           = 6,

    D3DPT_FORCE_DWORD           = 0x7fffffff
} D3DPRIMITIVETYPE;

#define D3DFVF_XYZ              0x002
#define D3DFVF_NORMAL           0x010
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_TEX1             0x100
#define D3DFVF_NORMPACKED3		0x20000000

/*
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
*/


bool bStripify = false;
bool bClean = false;
bool bOptimize = false;
bool bCompress = false;
bool bDump = false;
bool bVerboseDump = false;

class CMeshFile
{
public:
	D3DPRIMITIVETYPE m_primitiveType;
	int m_nFaceCount;
	DWORD m_fvf;
	int m_nVertexStride;
	int m_nVertexCount;
	int m_nIndexCount;
	BYTE* m_pVertexBuffer;
	WORD* m_pIndexBuffer;

	bool LoadFile(const char* szFileName);
	bool SaveFile(const char* szFileName);
	bool Stripify();
	bool Clean();
	bool Optimize();
	bool Compress();
	void Dump();
};

bool CMeshFile::Optimize()
{
	// Remove duplicate vertices...
	int nStartVertexCount = m_nVertexCount;
	BYTE* pbSrcVert = m_pVertexBuffer;
	BYTE* pbDestVert = m_pVertexBuffer;
	for (int i = 0; i < m_nVertexCount - 1; i += 1)
	{
		for (int j = i + 1; j < m_nVertexCount; j += 1)
		{
			if (memcmp(m_pVertexBuffer + i * m_nVertexStride, m_pVertexBuffer + j * m_nVertexStride, m_nVertexStride) == 0)
			{
				// found a duplicate
//				printf("\tremoving vertex %d\n", j);
				for (int k = 0; k < m_nIndexCount; k += 1)
				{
					if (m_pIndexBuffer[k] == j)
						m_pIndexBuffer[k] = (WORD)i;
					else if (m_pIndexBuffer[k] > j)
						m_pIndexBuffer[k] -= 1;
				}

				MoveMemory(m_pVertexBuffer + j * m_nVertexStride, m_pVertexBuffer + (j + 1) * m_nVertexStride, (m_nVertexCount - j - 1) * m_nVertexStride);
				m_nVertexCount -= 1;
				j -= 1;
			}
		}
	}

	if (m_nVertexCount < nStartVertexCount)
	{
		printf("\tremoved %d vertices\n", nStartVertexCount - m_nVertexCount);
		return true;
	}

	return false;
}

bool CMeshFile::Clean()
{
	if ((m_fvf & D3DFVF_DIFFUSE) == 0)
	{
		printf("\tskipping; no diffuse color\n");
		return false;
	}

	BYTE* pbSrcVert = m_pVertexBuffer;
	BYTE* pbDestVert = m_pVertexBuffer;
	int nNewVertexStride = m_nVertexStride - 4;
	for (int i = 0; i < m_nVertexCount; i += 1)
	{
		BYTE* src = pbSrcVert;
		BYTE* dest = pbDestVert;

		if ((m_fvf & D3DFVF_XYZ) != 0)
		{
			float* pf = (float*)src;
			src += sizeof (float) * 3;
			*((float*&)dest)++ = pf[0];
			*((float*&)dest)++ = pf[1];
			*((float*&)dest)++ = pf[2];
		}

		if ((m_fvf & D3DFVF_NORMAL) != 0)
		{
			float* pf = (float*)src;
			src += sizeof (float) * 3;
			*((float*&)dest)++ = pf[0];
			*((float*&)dest)++ = pf[1];
			*((float*&)dest)++ = pf[2];
		}

		if ((m_fvf & D3DFVF_NORMPACKED3) != 0) {
			DWORD* pdw = (DWORD*)src;
			src += sizeof (DWORD);
			*((DWORD*&)dest)++ = *pdw;
		}

		// Skip the color...
		src += sizeof (DWORD);

		if ((m_fvf & D3DFVF_TEX1) != 0)
		{
			float* pf = (float*)src;
			src += sizeof (float) * 2;
			*((float*&)dest)++ = pf[0];
			*((float*&)dest)++ = pf[1];
		}

		pbSrcVert += m_nVertexStride;
		pbDestVert += nNewVertexStride;
	}

	m_nVertexStride = nNewVertexStride;
	m_fvf &= ~D3DFVF_DIFFUSE;

	return true;
}

bool CMeshFile::Stripify()
{
	if (m_primitiveType != D3DPT_TRIANGLELIST)
	{
		printf("\tskipping; not a triangle list\n");
		return false;
	}

	int nStrippedIndexCount = 0;
	WORD* pwStrippedIndices = NULL;

	::Stripify(m_nFaceCount, m_pIndexBuffer, &nStrippedIndexCount, &pwStrippedIndices);

	delete [] m_pIndexBuffer;
	m_pIndexBuffer = pwStrippedIndices;
	m_nIndexCount = nStrippedIndexCount;
	m_nFaceCount = nStrippedIndexCount - 2;
	m_primitiveType = D3DPT_TRIANGLESTRIP;

	// BLOCK: Create a new vertex buffer and sort the vertices into it...
	{
		WORD* pwVertexPermutation = NULL;
		ComputeVertexPermutation(nStrippedIndexCount, pwStrippedIndices, &m_nVertexCount, &pwVertexPermutation);

		BYTE* pVertexBuffer = new BYTE [m_nVertexCount * m_nVertexStride];
		
		for (int i = 0; i < m_nVertexCount; i += 1)
		{
			BYTE* pbSrcVert = m_pVertexBuffer + pwVertexPermutation[i] * m_nVertexStride;
			BYTE* pbDestVert = pVertexBuffer + i * m_nVertexStride;

			CopyMemory(pbDestVert, pbSrcVert, m_nVertexStride);
		}

		delete [] pwVertexPermutation;

		delete [] m_pVertexBuffer;
		m_pVertexBuffer = pVertexBuffer;
	}

	return true;
}

bool CMeshFile::LoadFile(const char* szFileName)
{
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

	m_pVertexBuffer = new BYTE [m_nVertexCount * m_nVertexStride];
	ReadFile(hFile, m_pVertexBuffer, m_nVertexCount * m_nVertexStride, &dwRead, NULL);

	m_pIndexBuffer = new WORD [m_nIndexCount];
	ReadFile(hFile, m_pIndexBuffer, m_nIndexCount * sizeof (WORD), &dwRead, NULL);

	CloseHandle(hFile);

	return true;
}

bool CMeshFile::SaveFile(const char* szFileName)
{
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwValue, dwWritten;

	dwValue = (DWORD)m_primitiveType;
	WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

	dwValue = (DWORD)m_nFaceCount;
	WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

	dwValue = (DWORD)m_fvf;
	WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

	dwValue = (DWORD)m_nVertexStride;
	WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

	dwValue = (DWORD)m_nVertexCount;
	WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

	dwValue = (DWORD)m_nIndexCount;
	WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

	WriteFile(hFile, m_pVertexBuffer, m_nVertexCount * m_nVertexStride, &dwWritten, NULL);

	WriteFile(hFile, m_pIndexBuffer, m_nIndexCount * sizeof (WORD), &dwWritten, NULL);

	CloseHandle(hFile);

	return true;
}

void CMeshFile::Dump()
{
	const char* rgszPrimitiveType [] =
	{
		"unknown",
		"D3DPT_POINTLIST",
		"D3DPT_LINELIST",
		"D3DPT_LINESTRIP",
		"D3DPT_TRIANGLELIST",
		"D3DPT_TRIANGLESTRIP",
		"D3DPT_TRIANGLEFAN"
	};

	printf("\ttype: %s\n", rgszPrimitiveType[(int)m_primitiveType]);

	printf("\tfvf: ");
	if ((m_fvf & D3DFVF_XYZ) != 0)
		printf("D3DFVF_XYZ ");
	if ((m_fvf & D3DFVF_NORMAL) != 0)
		printf("D3DFVF_NORMAL ");
	if ((m_fvf & D3DFVF_NORMPACKED3) != 0)
		printf("D3DFVF_NORMPACKED3 ");
	if ((m_fvf & D3DFVF_DIFFUSE) != 0)
		printf("D3DFVF_DIFFUSE ");
	if ((m_fvf & D3DFVF_TEX1) != 0)
		printf("D3DFVF_TEX1 ");
	printf("\n");

	printf("\tface count: %d\n", m_nFaceCount);
	printf("\tvertex count: %d\n", m_nVertexCount);
	printf("\tindex count: %d\n", m_nIndexCount);

	if (bVerboseDump)
	{
		printf("\tvertices:\n");
		for (int i = 0; i < m_nVertexCount; i += 1)
		{
			printf("\t%6d: ", i);

			BYTE* vert = m_pVertexBuffer + m_nVertexStride * i;
			if ((m_fvf & D3DFVF_XYZ) != 0)
			{
				float* pf = (float*)vert;
				printf("xyz(%f,%f,%f)\n\t\t", pf[0], pf[1], pf[2]);
				vert += sizeof (float) * 3;
			}

			if ((m_fvf & D3DFVF_NORMAL) != 0)
			{
				float* pf = (float*)vert;
				printf("normal(%f,%f,%f)\n\t\t", pf[0], pf[1], pf[2]);
				vert += sizeof (float) * 3;
			}

			if ((m_fvf & D3DFVF_NORMPACKED3) != 0)
			{
				DWORD* pdw = (DWORD*)vert;
				printf("normal(0x%X)\n\t\t", *pdw);
				vert += sizeof (DWORD);
			}

			if ((m_fvf & D3DFVF_DIFFUSE) != 0)
			{
				vert += sizeof (DWORD);
			}

			if ((m_fvf & D3DFVF_TEX1) != 0)
			{
				float* pf = (float*)vert;
				printf("tex1(%f,%f)\n\t\t", pf[0], pf[1]);
				vert += sizeof (float) * 2;
			}
			printf("\n");
		}

		printf("\tindices:\n");
		for (i = 0; i < m_nIndexCount; i += 3)
		{
			printf("\t\t%d %d %d\n", m_pIndexBuffer[i + 0], m_pIndexBuffer[i + 1], m_pIndexBuffer[i + 2]);
		}
	}
}

DWORD CompressNormal(float* pvNormal) {

    float vNormal[3];
    float fLength;

    fLength = (float)sqrt(pvNormal[0] * pvNormal[0] + pvNormal[1] * pvNormal[1] + pvNormal[2] * pvNormal[2]);

    vNormal[0] = pvNormal[0] / fLength;
    vNormal[1] = pvNormal[1] / fLength;
    vNormal[2] = pvNormal[2] / fLength;

    return ((((DWORD)(vNormal[0] * 1023.0f) & 0x7FF) << 0)  |
            (((DWORD)(vNormal[1] * 1023.0f) & 0x7FF) << 11) |
            (((DWORD)(vNormal[2] *  511.0f) & 0x3FF) << 22));
}

bool CMeshFile::Compress()
{
	DWORD dwNormal;
	BYTE* pCompressedVertexBuffer;
	int nCompressedVertexStride = m_nVertexStride - 2 * sizeof (float);

	if ((m_fvf & D3DFVF_NORMPACKED3) || !(m_fvf & D3DFVF_NORMAL)) {
		return false;
	}

	pCompressedVertexBuffer = new BYTE [m_nVertexCount * nCompressedVertexStride];

	for (int i = 0; i < m_nVertexCount; i += 1)
	{
		BYTE* vert = m_pVertexBuffer + m_nVertexStride * i;
		BYTE* cvert = pCompressedVertexBuffer + nCompressedVertexStride * i;

		if ((m_fvf & D3DFVF_XYZ) != 0)
		{
			memcpy(cvert, vert, sizeof (float) * 3);
			vert += sizeof (float) * 3;
			cvert += sizeof (float) * 3;
		}

		if ((m_fvf & D3DFVF_NORMAL) != 0) // Should always be TRUE
		{
			dwNormal = CompressNormal((float*)vert);
			memcpy(cvert, &dwNormal, sizeof (DWORD));
			vert += sizeof (float) * 3;
			cvert += sizeof (DWORD);
		}

		if ((m_fvf & D3DFVF_DIFFUSE) != 0)
		{
			memcpy(cvert, vert, sizeof(DWORD));
			vert += sizeof (DWORD);
			cvert += sizeof (DWORD);
		}

		if ((m_fvf & D3DFVF_TEX1) != 0)
		{
			memcpy(cvert, vert, sizeof(float) * 2);
			vert += sizeof (float) * 2;
			cvert += sizeof (float) * 2;
		}
	}

	delete [] m_pVertexBuffer;
	m_pVertexBuffer = pCompressedVertexBuffer;
	m_nVertexStride = nCompressedVertexStride;

	m_fvf &= ~D3DFVF_NORMAL;
	m_fvf |= D3DFVF_NORMPACKED3;

	return true;
}

void ProcessFile(const char* szFileName)
{
	CMeshFile file;
	bool bModified = false;

	if (!file.LoadFile(szFileName))
	{
		fprintf(stderr, "Cannot load file '%s'\n", szFileName);
		return;
	}

	if (bClean)
	{
		printf("Cleaning %s...\n", szFileName);
		bModified = file.Clean();
	}

	if (bOptimize)
	{
		printf("Optimizing %s...\n", szFileName);
		bModified = file.Optimize();
	}

	if (bStripify)
	{
		printf("Stripifying %s...\n", szFileName);
		bModified = file.Stripify();
	}

	if (bCompress)
	{
		printf("Compressing %s...\n", szFileName);
		bModified = file.Compress();
	}

	if (bDump)
	{
		printf("Dump of %s\n", szFileName);
		file.Dump();
	}

	if (bModified)
	{
		if (!file.SaveFile(szFileName))
		{
			fprintf(stderr, "Cannot write file '%s'\n", szFileName);
			return;
		}
	}
}

void ProcessFiles(const char* szWild)
{
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(szWild, &fd);
	if (h == INVALID_HANDLE_VALUE)
	{
		printf("Cannot find %s\n", szWild);
		return;
	}

	do
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
			continue;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			continue;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
			continue;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		ProcessFile(fd.cFileName);
	}
	while (FindNextFile(h, &fd));

	FindClose(h);
}

int __cdecl main(int argc, char* argv[])
{
	bool bNeedHelp = true;

	for (int i = 1; i < argc; i += 1)
	{
		const char* sz = argv[i];

		if (sz[0] == '-' || sz[0] == '/')
		{
			sz += 1;
			while (*sz != '\0')
			{
				switch (*sz++)
				{
				case '?':
					goto LNeedHelp;

				case 's':
				case 'S':
					bStripify = true;
					break;

				case 'c':
				case 'C':
					bClean = true;
					break;

				case 'o':
				case 'O':
					bOptimize = true;
					break;

				case 'n':
				case 'N':
					bCompress = true;
					break;

				case 'd':
				case 'D':
					bDump = true;
					break;

				case 'v':
				case 'V':
					bDump = true;
					bVerboseDump = true;
					break;
				}
			}
		}
		else
		{
			bNeedHelp = false;
			ProcessFiles(sz);
		}
	}

	if (bNeedHelp)
	{
LNeedHelp:
		printf("This program processes XDash style 3D mesh files in various ways.\n");
		printf("Usage:\n\txm [-options] <file> ...\n");
		printf("Options:\n");
		printf("\t-d\tDump\n");
		printf("\t-v\tVerbose Dump\n");
		printf("\t-s\tStripify\n");
		printf("\t-c\tClean (remove diffuse color)\n");
	}

	return 0;
}
