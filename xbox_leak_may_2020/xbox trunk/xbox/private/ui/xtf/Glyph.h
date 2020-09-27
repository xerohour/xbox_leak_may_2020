#pragma once

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
};

struct CGlyphObject
{
	GLYPHMETRICSFLOAT m_metrics;
	CGlyphShape* m_pGlyphShape; // or DWORD m_dwFileOffsetOfGlyphShape
};



