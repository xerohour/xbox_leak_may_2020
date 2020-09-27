//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
//-------------------------------------------------------------
#include "std.h"
#include "font.h"
#include "FileUtil.h"
#include "XOConst.h"
#include "globals.h"
#include "utilities.h"



// Adjust the alpha value for verts whose x is between nStart and nEnd such that they fade
// from opaque to transparent (bRight==true) or transparent to opaque (bRight==false).
static void HorizontalFade(TEXTVERTEX* verts, int nVertexCount, float nStart, float nEnd, bool bRight)
{
	float nWidth = nEnd - nStart;

	for (int i = 0; i < nVertexCount; i += 1)
	{
		float a = (verts[i].x - nStart) / nWidth;
		a = smoothstep(0.0f, 1.0f, a);

		if (bRight)
			a = 1.0f - a;

		float a0 = (float)(verts[i].color >> 24);
		verts[i].color = (verts[i].color & 0x00ffffff) | (((DWORD)(a0 * a)) << 24);
	}
}

inline void FadeLeftEdge(TEXTVERTEX* verts, int nVertexCount, float nStart, float nEnd)
{
	HorizontalFade(verts, nVertexCount, nStart, nEnd, false);
}

inline void FadeRightEdge(TEXTVERTEX* verts, int nVertexCount, float nStart, float nEnd)
{
	HorizontalFade(verts, nVertexCount, nStart, nEnd, true);
}



CFont::CFont()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_pGlyphSet = NULL;
	m_rgGlyphObjects = NULL;
	m_rgGlyphLoaded = NULL;
    
}

CFont::~CFont()
{
	Close();
}

void CFont::Close()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwRes = CloseHandle(m_hFile);
		ASSERT(dwRes);
	}

	if (m_pGlyphSet != NULL)
	{
		for (UINT i = 0; i < m_pGlyphSet->cGlyphsSupported; i += 1)
		{
			if (m_rgGlyphLoaded[i])
			{
				CGlyphShape* pGlyphShape = m_rgGlyphObjects[i].m_pGlyphShape;
				delete [] pGlyphShape->m_indices;
				delete [] pGlyphShape->m_vertices;
				delete pGlyphShape;
			}
		}
	}

	delete [] (BYTE*)m_pGlyphSet;
	delete [] m_rgGlyphObjects;
	delete [] m_rgGlyphLoaded;

	m_hFile = INVALID_HANDLE_VALUE;
	m_pGlyphSet = NULL;
	m_rgGlyphObjects = NULL;
	m_rgGlyphLoaded = NULL;
}

bool CFont::Open(const TCHAR* szFile)
{
	ASSERT(m_hFile == INVALID_HANDLE_VALUE);

	m_hFile = XAppCreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwRead;

	DWORD dwMagic;
	if(!ReadFile(m_hFile, &dwMagic, 4, &dwRead, NULL))
	{
		ASSERT(false);
		DbgPrint("CFont::Open-Fail to open the font file\n");
		return false;
	}


	if (dwMagic != 0x30465458) // "XTF0"
	{
		DbgPrint("\001Invalid font file: %s\n", szFile);

		DWORD dwRes = CloseHandle(m_hFile);
		ASSERT(dwRes);
		m_hFile = INVALID_HANDLE_VALUE;

		return false;
	}

	// BLOCK: Skip header...
	{
		DWORD dwHeaderLen;
		DWORD dwRes = ReadFile(m_hFile, &dwHeaderLen, 4, &dwRead, NULL);
		ASSERT(dwRes);

		ASSERT(dwHeaderLen == LF_FACESIZE);

		char szFace [LF_FACESIZE];
		dwRes = ReadFile(m_hFile, szFace, LF_FACESIZE, &dwRead, NULL);
		ASSERT(dwRes);

	}

	DWORD cbGlyphSet=0;
	DWORD dwRes = ReadFile(m_hFile, &cbGlyphSet, 4, &dwRead, NULL);
	ASSERT(dwRes);

	m_pGlyphSet = (CGlyphSet*)new BYTE [cbGlyphSet];
	m_pGlyphSet->cbThis = cbGlyphSet;
	dwRes = ReadFile(m_hFile, ((BYTE*)m_pGlyphSet) + 4, cbGlyphSet - 4, &dwRead, NULL);
	ASSERT(dwRes);

	m_rgGlyphObjects = new CGlyphObject [m_pGlyphSet->cGlyphsSupported];
	dwRes = ReadFile(m_hFile, m_rgGlyphObjects, sizeof (CGlyphObject) * m_pGlyphSet->cGlyphsSupported, &dwRead, NULL);
	ASSERT(dwRes);

	m_rgGlyphLoaded = new bool [m_pGlyphSet->cGlyphsSupported];
	ZeroMemory(m_rgGlyphLoaded, sizeof (bool) * m_pGlyphSet->cGlyphsSupported);

	return true;
}

bool CFont::LoadGlyph(int nGlyphIndex)
{
	ASSERT(m_pGlyphSet != NULL);
	ASSERT(nGlyphIndex >= 0 && (UINT)nGlyphIndex < m_pGlyphSet->cGlyphsSupported);

	if (m_rgGlyphLoaded[nGlyphIndex])
		return true;

	ASSERT(SetFilePointer(m_hFile, (DWORD)m_rgGlyphObjects[nGlyphIndex].m_pGlyphShape, 0, FILE_BEGIN) != ~0);
	CGlyphShape* pGlyphShape = new CGlyphShape;
    ASSERT(pGlyphShape);

	DWORD dwRead;
	
	DWORD dwRes = ReadFile(m_hFile, &pGlyphShape->m_nIndexCount, 2, &dwRead, NULL);
	ASSERT(dwRes);
	pGlyphShape->m_indices = new WORD [pGlyphShape->m_nIndexCount];

	dwRes = ReadFile(m_hFile, &pGlyphShape->m_nVertexCount, 2, &dwRead, NULL);
	ASSERT(dwRes);
	pGlyphShape->m_vertices = new CGlyphVertex [pGlyphShape->m_nVertexCount];

	dwRes = ReadFile(m_hFile, pGlyphShape->m_indices, sizeof (WORD) * pGlyphShape->m_nIndexCount, &dwRead, NULL);
	ASSERT(dwRes);
	dwRes = ReadFile(m_hFile, pGlyphShape->m_vertices, sizeof (CGlyphVertex) * pGlyphShape->m_nVertexCount, &dwRead, NULL);
	ASSERT(dwRes);

	m_rgGlyphObjects[nGlyphIndex].m_pGlyphShape = pGlyphShape;
	m_rgGlyphLoaded[nGlyphIndex] = true;

	return true;
}

int CFont::FindGlyphIndex(WCHAR wch)
{
	int nIndex = 0;

	for (UINT i = 0; i < m_pGlyphSet->cRanges; i += 1)
	{
		if (wch >= m_pGlyphSet->ranges[i].wcLow && wch < m_pGlyphSet->ranges[i].wcLow + m_pGlyphSet->ranges[i].cGlyphs)
			return nIndex + wch - m_pGlyphSet->ranges[i].wcLow;

		nIndex += m_pGlyphSet->ranges[i].cGlyphs;
	}

	if (wch != 127)
		return FindGlyphIndex(127); // the invalid char box

	return 0; // "invalid char" glyph (it's a space)
}

bool CFont::IsBreakChar(TCHAR ch)
{
    // Basically, we will break after double byte or single byte Kana
    if (ch == ' ' || (ch >= 0x3040 && ch < 0xF000) || ch >= 0xFF66)
    {
        return true;
    }

    return false;
}


HRESULT CFont::CreateTextMesh(const TCHAR* pchText, int nChars, LPD3DXMESH* ppMesh, D3DXVECTOR3* pMin, D3DXVECTOR3* pMax, float nFormatWidth,bool bDoNotBreak, int nCursorPos)
{
	HRESULT hr = S_OK;
	bool bFade = bDoNotBreak;

	// do not call this funciton if no mesh is to be created
	ASSERT(pchText || nCursorPos >=0);
	if(!pchText)
	{
		return S_FALSE;
	}
	if (nChars == -1 && pchText)
		nChars = _tcslen(pchText);

	LPD3DXMESH pMesh = NULL;
	int nFaces = 0, nVerts = 0;

	int ich = 0;
	float nMaxCol1Width = 0.0f;
	float nMaxCol2Width = 0.0f;
	bool bSingleLine = true;
	bool bSingleColumn = true;
	bool bAsterisk = false;
	for (int nLine = 0; ich < nChars; nLine += 1)
	{
		int nColumn = 1;
		float nCol1Width = 0.0f;
		float nCol2Width = 0.0f;

		while (ich < nChars)
		{
			TCHAR ch = pchText[ich];
			ich += 1;

			if (bDoNotBreak == false && ch == '\r' && ich < nChars && pchText[ich] == '\n')
            {
				ich += 1;
            }

			if (bDoNotBreak == false && (ch == '\n' || ch == '\r'))
			{
				bSingleLine = false;
				break;
			}

			if (ch == '\t' && bDoNotBreak == false)
			{
				nColumn = 2;
				bSingleColumn = false;
				continue;
			}

			int nGlyphIndex = FindGlyphIndex(ch);
			if (nGlyphIndex == -1)
				continue;

			DWORD dwRes = LoadGlyph(nGlyphIndex);
			ASSERT(dwRes);

			CGlyphShape* pGlyphShape = m_rgGlyphObjects[nGlyphIndex].m_pGlyphShape;
			pGlyphShape->m_nGlyphIndex = nGlyphIndex;

			if (nVerts + pGlyphShape->m_nVertexCount > 65535)
			{
				DbgPrint("CreateTextMesh: too much text!\n");
				nChars = ich - 1; // truncate string
				break;
			}

			if (nColumn == 1)
				nCol1Width += m_rgGlyphObjects[nGlyphIndex].m_metrics.gmfCellIncX;
			else
				nCol2Width += m_rgGlyphObjects[nGlyphIndex].m_metrics.gmfCellIncX;

			nFaces += pGlyphShape->m_nIndexCount / 3;
			nVerts += pGlyphShape->m_nVertexCount;
		}

		if (nCol1Width > nMaxCol1Width)
			nMaxCol1Width = nCol1Width;

		if (nCol2Width > nMaxCol2Width)
			nMaxCol2Width = nCol2Width;
	}

	if (nCursorPos >= 0 && nCursorPos <= nChars)
	{
		// TBD handle cursor only 
		nFaces += 2;
		nVerts += 4;
	}

	if (nFaces == 0)
	{ 
		// check it out
		ASSERT(false);
		return S_FALSE;
	}

	if (nFormatWidth > 0.0f)
	{
		if (bSingleColumn)
		{
			nMaxCol1Width = nFormatWidth;
		}
		else if (nMaxCol1Width + nMaxCol2Width > nFormatWidth)
		{
			nMaxCol1Width = nFormatWidth - nMaxCol2Width;
			bFade = true;
		}
	}
	else
	{
		bFade = false;
	}

//	DbgPrint("Creating text mesh with %d indices and %d vertices (about %dKB)\n", nFaces * 3, nVerts, ((nFaces * 3 * sizeof (WORD)) + (nVerts * 6 * sizeof (float)) + 512) / 1024);

	// Nat TBD: handle out of memory case
	hr = D3DXCreateMeshFVF(nFaces, nVerts, D3DXMESH_MANAGED, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, g_pd3dDevice, &pMesh);
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		DbgPrint("CFont::CreateTextMesh - Fail to create Mesh, hr = 0x%x", hr);
		return hr;
	}

	*ppMesh = pMesh;

	WORD* indices;
	hr = pMesh->LockIndexBuffer(D3DLOCK_DISCARD, (BYTE**)&indices);
	VERIFYHR(hr);

	TEXTVERTEX* verts;
	hr = pMesh->LockVertexBuffer(D3DLOCK_DISCARD, (BYTE**)&verts);
	VERIFYHR(hr);

	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;  

	bool bStartPara = true;
	int nCurIndex = 0;
	int nCurVertex = 0;
	float x = 0.0f;
	float y = 0.0f;
	ich = 0;
	for (nLine = 0; ich < nChars; nLine += 1)
	{
		int nColumn = 1;
		int nFirstColVertex = nCurVertex;
		float nCol2Width = 0.0f;
		float nMaxColWidth = nMaxCol1Width;

		int nBreakChar = -1;
		int nBreakVertex = nCurVertex;
		int nBreakIndex = nCurIndex;

		for (;;)
		{
			TCHAR ch = '\r';
			if (ich < nChars)
			{
				ch = pchText[ich];
				ich += 1;

				if (ch == '\r' && ich < nChars && pchText[ich] == '\n')
					ich += 1;
			}

			if (ch == '\n' || ch == '\r')
			{
                if (bDoNotBreak && bFade)
                {
                    FadeRightEdge(verts + nFirstColVertex, nCurVertex - nFirstColVertex, nMaxColWidth - 1, nMaxColWidth);
                    break;
                }

				if (nColumn == 2)
				{
					// Right justify the second column...
					for (int i = nFirstColVertex; i < nCurVertex; i += 1)
						verts[i].x += nMaxCol2Width - nCol2Width;
				}
				else
				{
					if (bFade)
						FadeRightEdge(verts + nFirstColVertex, nCurVertex - nFirstColVertex, nMaxColWidth - 1, nMaxColWidth);
				}

				if (ich != nChars)
				{
					x = 0.0f;
					if (/*bTitle ||*/ ch == '\r' || !bSingleColumn)
						y -= 1.0f;
					else
						y -= 1.5f;
				}

				bStartPara = true;
				break;
			}

			if (ch == '\t' && bDoNotBreak == false)
			{
				if (bFade)
					FadeRightEdge(verts + nFirstColVertex, nCurVertex - nFirstColVertex, nMaxColWidth - 2, nMaxColWidth);

				nColumn = 2;
				x = nFormatWidth - nMaxCol2Width;
				nFirstColVertex = nCurVertex;
				nMaxColWidth = nMaxCol2Width;
				bStartPara = true;

				continue;
			}

			bStartPara = false;

			int nGlyphIndex = FindGlyphIndex(ch);
			if (nGlyphIndex == -1)
				continue;

			ASSERT(m_rgGlyphLoaded[nGlyphIndex]);

			if (nFormatWidth > 0.0f)
			{
				if (bDoNotBreak)
				{
					if (x > nFormatWidth)
					{
						// TODO: Don't bother placing these chars in the buffer...
//						continue; // messes up vert count from first pass!
					}
				}
				else
				{
					if (ch != ' ' && nColumn == 1 && x + m_rgGlyphObjects[nGlyphIndex].m_metrics.gmfCellIncX > nFormatWidth)
					{

						// break line

						if (nBreakChar > -1)
						{
							x = 0.0f;
							y -= 1.0f;

							ich = nBreakChar /*- 1*/;
							nCurVertex = nBreakVertex;
							nCurIndex = nBreakIndex;

							nBreakChar = -1;
							nBreakVertex = nCurVertex;
							nBreakIndex = nCurIndex;

							continue;
						}
						else
						{
							// one word was too long to fit, fade it!
							bFade = true;
						}
					}
				}
			}

			CGlyphShape* pGlyphShape = m_rgGlyphObjects[nGlyphIndex].m_pGlyphShape;

			for (int j = 0; j < pGlyphShape->m_nIndexCount; j += 1)
				indices[nCurIndex++] = nCurVertex + pGlyphShape->m_indices[j];

			float fontAdjust = 1.0f;

			if(pGlyphShape->m_nGlyphIndex > 190)   //character is in the japanese font set
			{
				fontAdjust = 0.9f;
			}

			for (j = 0; j < pGlyphShape->m_nVertexCount; j += 1)
			{
				verts[nCurVertex].x = x + (pGlyphShape->m_vertices[j].x * fontAdjust);
				verts[nCurVertex].y = y + (pGlyphShape->m_vertices[j].y * fontAdjust);
				verts[nCurVertex].z = 0.0f;

				verts[nCurVertex].nx = 0.0f;
				verts[nCurVertex].ny = 0.0f;
				verts[nCurVertex].nz = -1.0f;

				verts[nCurVertex].color = D3DCOLOR_COLORVALUE(r, g, b, 1.0f);

				nCurVertex += 1;
			}

			if (ich - 1 == nCursorPos)
			{
				CreateCursorMesh(indices, nCurIndex, verts, nCurVertex, x, y, true);
			}

			x += (m_rgGlyphObjects[nGlyphIndex].m_metrics.gmfCellIncX * fontAdjust);

			if (nColumn == 2)
				nCol2Width += m_rgGlyphObjects[nGlyphIndex].m_metrics.gmfCellIncX;

			if (IsBreakChar(ch))
			{
				nBreakChar = ich;
				nBreakVertex = nCurVertex;
				nBreakIndex = nCurIndex;
			}
		}
	}

	if (nCursorPos == nChars)
	{
		bool visible = false;
		if(x < 16.556152f)  //value to keep the cursor from extending past the keyboard blank
			visible = true;

		CreateCursorMesh(indices, nCurIndex, verts, nCurVertex, x, y, visible);
	}

	ASSERT(nCurVertex == nVerts);
	ASSERT(nCurIndex == nFaces * 3);

	
	if (pMin != NULL && pMax != NULL)
	{
		hr = D3DXComputeBoundingBox(verts, nCurVertex, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, pMin, pMax);
		VERIFYHR(hr);
	}

	hr = pMesh->UnlockIndexBuffer();
	VERIFYHR(hr);

	pMesh->UnlockVertexBuffer();
	VERIFYHR(hr);

	return hr;

}

void CFont::CreateCursorMesh(WORD*& indices, int& nCurIndex, TEXTVERTEX*& verts, int& nCurVertex, float x, float y, bool visible)
{
	float t = (float) (XAppGetNow()) * 2.0f;
	float a = fabsf(sinf(t * D3DX_PI));
	if(!visible)
		a = 0.0f;

	indices[nCurIndex++] = (WORD)nCurVertex;
	indices[nCurIndex++] = (WORD)nCurVertex + 1;
	indices[nCurIndex++] = (WORD)nCurVertex + 2;

	indices[nCurIndex++] = (WORD)nCurVertex;
	indices[nCurIndex++] = (WORD)nCurVertex + 2;
	indices[nCurIndex++] = (WORD)nCurVertex + 3;

	verts[nCurVertex].x = x;
	verts[nCurVertex].y = y - 0.1f;
	verts[nCurVertex].z = 0.0f;

	verts[nCurVertex].nx = 0.0f;
	verts[nCurVertex].ny = 0.0f;
	verts[nCurVertex].nz = 1.0f;

	verts[nCurVertex].color = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, a);

	nCurVertex += 1;

	verts[nCurVertex].x = x + 0.1f;
	verts[nCurVertex].y = y - 0.1f;
	verts[nCurVertex].z = 0.0f;

	verts[nCurVertex].nx = 0.0f;
	verts[nCurVertex].ny = 0.0f;
	verts[nCurVertex].nz = 1.0f;

	verts[nCurVertex].color = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, a);

	nCurVertex += 1;

	verts[nCurVertex].x = x + 0.1f;
	verts[nCurVertex].y = y + 0.75f;
	verts[nCurVertex].z = 0.0f;

	verts[nCurVertex].nx = 0.0f;
	verts[nCurVertex].ny = 0.0f;
	verts[nCurVertex].nz = 1.0f;

	verts[nCurVertex].color = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, a);

	nCurVertex += 1;

	verts[nCurVertex].x = x;
	verts[nCurVertex].y = y + 0.75f;
	verts[nCurVertex].z = 0.0f;

	verts[nCurVertex].nx = 0.0f;
	verts[nCurVertex].ny = 0.0f;
	verts[nCurVertex].nz = 1.0f;

	verts[nCurVertex].color = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, a);

	nCurVertex += 1;
}
