// xtf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "xtf.h"
#include "Glyph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

void Export(const TCHAR* szFilePath, const TCHAR* szFaceName, const TCHAR* szJFaceName, float nChordalDeviation);

void UsageExit()
{
	cerr << _T("Usage: XTF [/c <chordal deviation>] <output.xtf> <face name> [<japanese face name>]") << endl;
	exit(2);
}

int __cdecl _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	const TCHAR* szOutputFileName = NULL;
	const TCHAR* szFontFaceName = NULL;
	const TCHAR* szJFontFaceName = NULL;
	float nChordalDeviation = 0.001f;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return 1;
	}
	
	if (argc < 3)
		UsageExit();

	for (int i = 1; i < argc; i += 1)
	{
		if (argv[i][0] == '/' || argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			default:
				UsageExit();
				break;

			case 'c':
			case 'C':
				{
					const TCHAR* szValue = argv[i] + 2;
					if (*szValue == 0)
					{
						i += 1;
						if (i >= argc)
							UsageExit();

						szValue = argv[i];
					}

					nChordalDeviation = (float)_tcstod(szValue, NULL);
					if (nChordalDeviation <= 0.0f)
					{
						cout << _T("Error: chordal deviation is too small!") << endl;
						return 1;
					}
				}
				break;
			}
		}
		else
		{
			if (szOutputFileName == NULL)
				szOutputFileName = argv[i];
			else if (szFontFaceName == NULL)
				szFontFaceName = argv[i];
			else if (szJFontFaceName == NULL)
				szJFontFaceName = argv[i];
			else
				UsageExit();
		}
	}

	if (szOutputFileName == NULL || szFontFaceName == NULL)
		UsageExit();

	cout << _T("Creating ") << szOutputFileName << _T(" from ") << szFontFaceName;
	if (szJFontFaceName != NULL)
		cout << _T(" and ") << szJFontFaceName << endl;

	Export(szOutputFileName, szFontFaceName, szJFontFaceName, nChordalDeviation);

	return 0;
}


extern CFile* g_pFile;
extern CStdioFile* g_pWrlFile;



extern HRESULT WINAPI MyCreateTextW
( 
    HDC   hDC,
    LPCWSTR wszText,
    FLOAT chordalDeviation,
    FLOAT extrusion,
    CGlyphObject* pGlyphObject
);

void Export(const TCHAR* szFilePath, const TCHAR* szFaceName, const TCHAR* szJFaceName, float nChordalDeviation)
{
	CFont font, fontJ;
	if (!font.CreatePointFont(100, szFaceName))
	{
		cout << _T("Error: Cannot load font: ") << szFaceName << endl;
		return;
	}

	if (szJFaceName != NULL && !fontJ.CreatePointFont(100, szJFaceName))
	{
		cout << _T("Error: Cannot load font: ") << szJFaceName << endl;
		return;
	}

	int cGlyphsSupportedJ = 0;
	int cRangesJ = 0;
	int nFirstJGlyphSet = -1;
	LPGLYPHSET pGlyphSetJ = NULL;

	if (szJFaceName != NULL)
	{
		// We need to adjust the GlyphSet to include the J characters as well...
		CClientDC dcJ(NULL);
		CFont* pOldFont = dcJ.SelectObject(&fontJ);
		
		DWORD dwGlyphSetSizeJ = GetFontUnicodeRanges(dcJ.m_hDC, NULL);
		pGlyphSetJ = (LPGLYPHSET)malloc(dwGlyphSetSizeJ);
		pGlyphSetJ->cbThis = dwGlyphSetSizeJ;
		GetFontUnicodeRanges(dcJ.m_hDC, pGlyphSetJ);

		for (int i = 0; i < pGlyphSetJ->cRanges; i += 1)
		{
			if (pGlyphSetJ->ranges[i].wcLow >= 0x3000)
			{
				if (nFirstJGlyphSet < 0)
					nFirstJGlyphSet = i;
				cGlyphsSupportedJ += pGlyphSetJ->ranges[i].cGlyphs;
				cRangesJ += 1;
			}
		}

		dcJ.SelectObject(pOldFont);
	}

	CClientDC dc(NULL);
	CFont* pOldFont = dc.SelectObject(&font);
	
	DWORD dwGlyphSetSize = GetFontUnicodeRanges(dc.m_hDC, NULL);
	dwGlyphSetSize += sizeof (WCRANGE) * cRangesJ;
	LPGLYPHSET pGlyphSet = (LPGLYPHSET)malloc(dwGlyphSetSize);
	pGlyphSet->cbThis = dwGlyphSetSize;
	GetFontUnicodeRanges(dc.m_hDC, pGlyphSet);
	pGlyphSet->cbThis = dwGlyphSetSize; // GetFontUnicodeRanges just trached this for us...

	if (nFirstJGlyphSet >= 0)
	{
		CopyMemory(&pGlyphSet->ranges[pGlyphSet->cRanges], &pGlyphSetJ->ranges[nFirstJGlyphSet], sizeof (WCRANGE) * cRangesJ);

		nFirstJGlyphSet = pGlyphSet->cRanges;
		pGlyphSet->cGlyphsSupported += cGlyphsSupportedJ;
		pGlyphSet->cRanges += cRangesJ;
	}

	CFile file(szFilePath, CFile::modeWrite | CFile::modeCreate);
	g_pFile = &file;


	file.Write("XTF0", 4); // magic number

	// BLOCK: Write face name
	{
		DWORD dwHeaderLen = LF_FACESIZE;
		file.Write(&dwHeaderLen, 4);

		char szFaceNameA [LF_FACESIZE];
		ZeroMemory(szFaceNameA, sizeof (szFaceNameA));

#ifdef _UNICODE
		WideCharToMultiByte(CP_ACP, 0, szFaceName, -1, szFaceNameA, LF_FACESIZE, NULL, NULL);
#else
		strncpy(szFaceNameA, szFaceName, LF_FACESIZE);
#endif

		file.Write(szFaceNameA, LF_FACESIZE);
	}

	file.Write(pGlyphSet, dwGlyphSetSize);

	CGlyphObject* rgGlyphObjects = new CGlyphObject [pGlyphSet->cGlyphsSupported];
	ZeroMemory(rgGlyphObjects, sizeof (CGlyphObject) * pGlyphSet->cGlyphsSupported);

	DWORD dwIndexPos = file.GetPosition();
	file.Seek(sizeof (CGlyphObject) * pGlyphSet->cGlyphsSupported, CFile::current);

	WCHAR wsz [2];
	wsz[1] = 0;

	int nGlyphIndex = 0;
	for (UINT i = 0; i < pGlyphSet->cRanges; i += 1)
	{
		if (i == nFirstJGlyphSet)
			dc.SelectObject(&fontJ);

		WCHAR wcHigh = pGlyphSet->ranges[i].wcLow + pGlyphSet->ranges[i].cGlyphs;
		for (wsz[0] = pGlyphSet->ranges[i].wcLow; wsz[0] < wcHigh; wsz[0] += 1)
		{
			VERIFY(MyCreateTextW(dc.m_hDC, wsz, nChordalDeviation, 0.0f, &rgGlyphObjects[nGlyphIndex]) == D3D_OK);

			CGlyphShape* pShape = rgGlyphObjects[nGlyphIndex].m_pGlyphShape;
			ASSERT(pShape != NULL);

			// Change pointer to file offset...
			rgGlyphObjects[nGlyphIndex].m_pGlyphShape = (CGlyphShape*)file.GetPosition();

			file.Write(&pShape->m_nIndexCount, sizeof (WORD));
			file.Write(&pShape->m_nVertexCount, sizeof (WORD));
			file.Write(pShape->m_indices, sizeof (WORD) * pShape->m_nIndexCount);
			file.Write(pShape->m_vertices, sizeof (CGlyphVertex) * pShape->m_nVertexCount);

			delete [] pShape->m_indices;
			delete [] pShape->m_vertices;
			delete pShape;

			nGlyphIndex += 1;
		}
	}

	// Backup and write the index...
	file.Seek(dwIndexPos, CFile::begin);
	file.Write(rgGlyphObjects, sizeof (CGlyphObject) * pGlyphSet->cGlyphsSupported);

	if (pGlyphSetJ != NULL)
		free(pGlyphSetJ);

	free(pGlyphSet);
	dc.SelectObject(pOldFont);

	g_pFile = NULL;
	g_pWrlFile = NULL;

	delete [] rgGlyphObjects;
}
