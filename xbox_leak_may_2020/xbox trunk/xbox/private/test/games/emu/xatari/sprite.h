#ifndef __Sprite_H
#define __Sprite_H

struct Color
{
    BYTE red;
	BYTE green;
	BYTE blue;
};

struct Vertex
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DCOLOR    diffuse;
    D3DXVECTOR2 texcoord0;
};

class Sprite3D {
public:
	char textureName[20];
	
	Vertex			p[4];
	D3DXCOLOR       c;					//p0---p1
//	D3DXVECTOR3     p[4];				// |	|
    float           spriteWidth;     	//p2---p3
	float           alphaValue;

	int				atariWidth, atariHeight;
	Color			atariPalette[256];
    

	D3DSURFACE_DESC    d3dsd;
    D3DLOCKED_RECT     d3dlr;
	DWORD			   dwDstPitch;
	LPDIRECT3DTEXTURE8 texture;
    D3DXVECTOR4     translateVector;

    D3DXMATRIX holdMatrix;

	Sprite3D(void);
//	Sprite3D(float width, D3DXCOLOR col);
	void initialize(IDirect3DDevice8 *pD3DDevice, int width, int height, Color *palette);
	void translate(D3DXVECTOR4 v);
	void drawSprite(IDirect3DDevice8 *pD3DDevice, unsigned char *atariImage);
/*	void loadTexture(const char texName[20]);
	char* texName(void);*/
};


/*class SpriteTrail {
public:
	int      numSprites;
	Sprite3D leader;
	Sprite3D *spriteArray;
	Vector   *locArray;
	int      frameDelay;
	float    alphaIncrement;
	int      counter;

	SpriteTrail(float width, Color col, int number, int frmDelay);
	void translate(Vector v);
	void drawTrail(LPDIRECT3DDEVICE3 pd3dDevice,
		           Transformation &pipeline);
	void loadTexture(char texName[20]);
};*/

#endif //__Sprite_H
