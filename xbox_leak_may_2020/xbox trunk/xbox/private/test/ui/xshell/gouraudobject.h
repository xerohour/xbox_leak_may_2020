#ifndef _GOURAUDOBJECT_H_
#define _GOURAUDOBJECT_H_

#include "3dobject.h"

typedef struct _XRAYVERTEX 
{
    D3DXVECTOR3 p;         // position
	D3DXVECTOR3 n;	       // normal
	DWORD       diffuse;   // diffuse color
} XRAYVERTEX;

class GouraudObject : public C3DObject
{
public:
    // Constructors and Destructors
    GouraudObject(void);
    ~GouraudObject(void);

    // Public Properties
	IDirect3DVertexBuffer8* m_pVB;
	IDirect3DIndexBuffer8*  m_pIB;

    // Initial Object description
	WORD        m_wNumVertices;
	XRAYVERTEX* m_pVertex;
	WORD        m_wNumFaces;
	Face*	    m_pFace;

    // Tri-stripped object
	DWORD		m_dwPrimitiveCount;
	DWORD       m_dwNumVertices;
	DWORD		m_dwStrippedIndexCount;  // Tristrip count
    WORD*       m_pwStrippedIndices;     // Tristrip indices

	D3DXMATRIX  m_holdMatrix;

    // Public Methods
	void render( IDirect3DDevice8* pD3DDevice );
	void renderStrip( IDirect3DDevice8* pD3DDevice );
	void loadMDL( FILE* pfInput, IDirect3DDevice8* pD3DDevice );
	void loadMDL( char* pszFilename, IDirect3DDevice8* pD3DDevice );
	void loadStrippedMDL( FILE* pfInput, IDirect3DDevice8* pD3DDevice );
};

#endif // _GOURAUDOBJECT_H_