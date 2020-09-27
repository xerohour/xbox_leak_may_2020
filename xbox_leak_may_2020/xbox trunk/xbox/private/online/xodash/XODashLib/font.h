#pragma once 

struct TEXTVERTEX
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
};

struct CGlyphVertex
{
	float x, y;
};

struct CGlyphShape
{
	WORD m_nIndexCount;
	WORD m_nVertexCount;
	WORD* m_indices;
	CGlyphVertex* m_vertices;
	int m_nGlyphIndex;    // added to hold the index of the character
};

struct CGlyphMetrics
{
    float gmfBlackBoxX;
    float gmfBlackBoxY;
    CGlyphVertex gmfptGlyphOrigin;
    float gmfCellIncX;
    float gmfCellIncY;
};

struct CGlyphObject
{
	CGlyphMetrics m_metrics;
	CGlyphShape* m_pGlyphShape; // or DWORD m_dwFileOffsetOfGlyphShape
};

struct CWCRange
{
	WCHAR wcLow;
	USHORT cGlyphs;
};

struct CGlyphSet
{
	DWORD cbThis;
	DWORD flAccel;
	DWORD cGlyphsSupported;
	DWORD cRanges;
	CWCRange ranges [1];
};

inline float smoothstep(float a, float b, float x)
{
	if (x < a)
		return 0.0f;

	if (x >= b)
		return 1.0f;

	x = (x - a) / (b - a);

	return (x * x * (3 - 2 * x));
}

class CFont
{
public:
	CFont();
	~CFont();

	bool Open(const TCHAR* szFile);
	void Close();
	bool LoadGlyph(int nGlyphIndex);
	int FindGlyphIndex(WCHAR wch);
	HRESULT CreateTextMesh(const TCHAR* pchText, int nChars, LPD3DXMESH* ppMesh, D3DXVECTOR3* pMin, D3DXVECTOR3* pMax, float nFormatWidth, bool bDoNotBreak, int nCursorPos);
	void CreateCursorMesh(WORD*& indices, int& nCurIndex, TEXTVERTEX*& verts, int& nCurVertex, float x, float y, bool visible);
	HANDLE m_hFile;

private:
    bool IsBreakChar(TCHAR ch);


	CGlyphSet* m_pGlyphSet;
	CGlyphObject* m_rgGlyphObjects;
	bool* m_rgGlyphLoaded;
};
