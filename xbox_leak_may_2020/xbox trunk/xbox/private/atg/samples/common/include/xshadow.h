#ifndef __XSHADOW_H
#define __XSHADOW_H

#include "xobj.h"
#include "xfvf.h"

class CXShadowVolume
{
	float m_fWidth;
	float m_fHeight;

	DWORD m_dwNumTriangles;
	DWORD m_dwNumVertices;
	DWORD m_dwNumEdges;

	D3DMATRIX m_Matrix;
	FVFT_XYZDIFF *m_Vert;
	WORD *m_Tris;

	void AddEdge(DWORD *edgelist, WORD v0, WORD v1);

public:
	CXShadowVolume(float w, float h);
	~CXShadowVolume();

	void AddObj(CXObject *obj, D3DXVECTOR3 *lightpos);
	void Render();
};

#endif