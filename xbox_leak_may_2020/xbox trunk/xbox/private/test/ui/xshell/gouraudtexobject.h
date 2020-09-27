#ifndef _GOURAUDTEXOBJECT_H_
#define _GOURAUDTEXOBJECT_H_

#include "3dobject.h"

typedef struct _TEXVERTEX 
{
    D3DXVECTOR3 p;         // position
	D3DXVECTOR3 n;	       // normal
	DWORD       diffuse;   // diffuse color
	FLOAT       tu, tv;    // The texture coordinates
} TEXVERTEX;

class TexFace : public Face
{

public:
    WORD m_wTexPoint[3];

};

class TextureVert
{

public:
	FLOAT m_tu, m_tv;

};

class GouraudTexObject : public C3DObject
{

public:
    // Constructors and Destructors
    GouraudTexObject(void);
    ~GouraudTexObject(void);

    // Public Properties
	char		            m_pszTextureName[30];
	WORD                    m_wNumVertices;
	TEXVERTEX*              m_pVertex;
	WORD                    m_wNumFaces;
	TexFace*                m_pFace;

    WORD                    m_wNumTexVertices;
    TextureVert*            m_pTextureVertex;

	IDirect3DVertexBuffer8* m_pVB;
	LPDIRECT3DTEXTURE8      m_pTexture;

	D3DXMATRIX              m_HoldMatrix;

	// Public Methods
	void loadTexture( char texture[30], IDirect3DDevice8* pd3dDevice );
	void render( IDirect3DDevice8* pd3dDevice );
	void loadMDL( FILE *input, IDirect3DDevice8* pd3dDevice );
	void loadMDL( char *filename, IDirect3DDevice8* pd3dDevice );
};

#endif // _GOURAUDTEXOBJECT_H_