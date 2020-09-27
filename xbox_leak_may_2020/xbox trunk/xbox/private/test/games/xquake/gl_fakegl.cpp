/*
Copyright (C) 2000 Jack Palevich.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// gl_fakegl.cpp -- Uses Direct3D to implement a subset of OpenGL.

#ifdef _XBOX
#include "xtl.h"
#include "xgraphics.h"
#else
#include "windows.h"
#endif

#include "gl/gl.h"
#include "stdio.h"

extern "C" void Con_Printf (char *fmt, ...);

#pragma warning( disable : 4244 )
#pragma warning( disable : 4820 )

#define     D3D_OVERLOADS
#define     RELEASENULL(object) if (object) {object->Release();}

#ifndef _XBOX
#include	"d3d8.h"
#include	"d3dx8.h"
#endif // ! XBOX

#define DX_DIRECT3D IDirect3D8
#define DX_TEXTURE IDirect3DTexture8*
#define LPDIRECT3DDEVICE IDirect3DDevice8*
#define LPDIRECT3D IDirect3D8*

// Some DX7 helper functions

#ifdef D3DRGBA
#undef D3DRGBA
#endif
#define D3DRGBA                                 D3DCOLOR_COLORVALUE

#define D3DRGB(_r,_g,_b)                        D3DCOLOR_COLORVALUE(_r,_g,_b,1.f)

#define D3DRGBFAST3(_r,_g,_b) \
    ((0xff << 24) | \
     (Truncate((_r)*255.f) << 16) | \
     (Truncate((_g)*255.f) << 8) | \
     (Truncate((_b)*255.f)))

#define D3DRGBFAST(_r,_g,_b,_a) \
    ((Truncate((_a)*255.f) << 24) | \
     (Truncate((_r)*255.f) << 16) | \
     (Truncate((_g)*255.f) << 8) | \
     (Truncate((_b)*255.f)))

#define RGBA_MAKE                               D3DCOLOR_RGBA

#define    TEXTURE0_SGIS    0x835E
#define    TEXTURE1_SGIS				0x835F
#define    D3D_TEXTURE_MAXANISOTROPY 0xf70001

extern "C" int			DIBWidth, DIBHeight;

#ifdef _DEBUG
void LocalDebugBreak(){
	DebugBreak();
}
#else
void LocalDebugBreak(){
}
#endif

// Globals
bool g_force16bitTextures;
bool gFullScreen = true;
DWORD gWidth = 640;
DWORD gHeight = 480;
DWORD gBpp = 16;
DWORD gZbpp = 16;
class FakeGL;
static FakeGL* gFakeGL;

class TextureEntry {
public:
	TextureEntry(){
		m_id = 0;
		m_mipMap = 0;
		m_format = D3DFMT_UNKNOWN;
		m_internalFormat = 0;

		m_glTexParameter2DMinFilter = GL_NEAREST_MIPMAP_LINEAR;
		m_glTexParameter2DMagFilter = GL_LINEAR;
		m_glTexParameter2DWrapS = GL_REPEAT;
		m_glTexParameter2DWrapT = GL_REPEAT;
		m_maxAnisotropy = 1.0;
	}
	~TextureEntry(){
	}

	void Release() {
		RELEASENULL(m_mipMap);
	}
	GLuint m_id;
	IDirect3DTexture8* m_mipMap;
	D3DFORMAT m_format;
	GLint m_internalFormat;

	GLint m_glTexParameter2DMinFilter;
	GLint m_glTexParameter2DMagFilter;
	GLint m_glTexParameter2DWrapS;
	GLint m_glTexParameter2DWrapT;
	float m_maxAnisotropy;
};


#define TASIZE 2000

class TextureTable {
public:
	TextureTable(){
		m_count = 0;
		m_size = 0;
		m_textures = 0;
		m_currentTexture = 0;
		m_currentID = 0;
		BindTexture(0);
	}
	~TextureTable(){
		DWORD i;
		for(i = 0; i < m_count; i++) {
			m_textures[i].Release();
		}
		for(i = 0; i < TASIZE; i++) {
			m_textureArray[i].Release();
		}

		delete [] m_textures;
	}

	void BindTexture(GLuint id){
		TextureEntry* oldEntry = m_currentTexture;
		m_currentID = id;

		if ( id < TASIZE ) {
			m_currentTexture = m_textureArray + id;
			if ( m_currentTexture->m_id ) {
				return;
			}
		}
		else {
			// Check overflow table.
			// Really ought to be a hash table.
			for(DWORD i = 0; i < m_count; i++){
				if ( id == m_textures[i].m_id ) {
					m_currentTexture =  m_textures + i;
					return;
				}
			}
			// It's a new ID.
			// Ensure space in the table
			if ( m_count >= m_size ) {
				int newSize = m_size * 2 + 10;
				TextureEntry* newTextures = new TextureEntry[newSize];
				for(DWORD i = 0; i < m_count; i++ ) {
					newTextures[i] = m_textures[i];
				}
				delete[] m_textures;
				m_textures = newTextures;
				m_size = newSize;
			}
			// Put new entry in table
			oldEntry = m_currentTexture;
			m_currentTexture = m_textures + m_count;
			m_count++;
		}
		if ( oldEntry ) {
			*m_currentTexture = *oldEntry;
		}
		m_currentTexture->m_id = id;
		m_currentTexture->m_mipMap = NULL;		
	}

	int GetCurrentID() {
		return m_currentID;
	}

	TextureEntry* GetCurrentEntry() {
		return m_currentTexture;
	}

	TextureEntry* GetEntry(GLuint id){
		if ( m_currentID == id && m_currentTexture ) {
			return m_currentTexture;
		}
		if ( id < TASIZE ) {
			return &m_textureArray[id];
		}
		else {
			// Check overflow table.
			// Really ought to be a hash table.
			for(DWORD i = 0; i < m_count; i++){
				if ( id == m_textures[i].m_id ) {
					return  &m_textures[i];
				}
			}
		}
		return 0;
	}

	IDirect3DTexture8*  GetMipMap(){
		if ( m_currentTexture ) {
			return m_currentTexture->m_mipMap;
		}
		return 0;
	}

	IDirect3DTexture8*  GetMipMap(int id){
		TextureEntry* entry = GetEntry(id);
		if ( entry ) {
			return entry->m_mipMap;
		}
		return 0;
	}

	D3DFORMAT GetSurfaceFormat() {
		if ( m_currentTexture ) {
			return m_currentTexture->m_format;
		}
		return D3DFMT_UNKNOWN;
	}
	void SetTexture(IDirect3DTexture8* mipMap, D3DFORMAT d3dFormat, GLint internalFormat){
		if ( !m_currentTexture ) {
			BindTexture(0);
		}
		RELEASENULL ( m_currentTexture->m_mipMap );
		m_currentTexture->m_mipMap = mipMap;
		m_currentTexture->m_format = d3dFormat;
		m_currentTexture->m_internalFormat = internalFormat;
	}

	GLint GetInternalFormat() {
		if ( m_currentTexture ) {
			return m_currentTexture->m_internalFormat;
		}
		return 0;
	}
private:
	GLuint m_currentID;
	DWORD m_count;
	DWORD m_size;
	TextureEntry m_textureArray[TASIZE]; // IDs 0..TASIZE-1
	TextureEntry* m_textures;			  // Overflow

	TextureEntry* m_currentTexture;
};


#if 1
#define Clamp(x) (x) // No clamping -- we've made sure the inputs are in the range 0..1
#else
float Clamp(float x) {
	if ( x < 0 ) {
		x = 0;
		LocalDebugBreak();
	}
	else if ( x > 1 ) {
		x = 1;
		LocalDebugBreak();
	}
	return x;
}
#endif

// Converts a floating point value to a long.
_declspec(naked) long Truncate(float f)
{
    _asm
    {
        cvttss2si eax, [esp+4]
        ret 4
    }
}

static D3DBLEND GLToDXSBlend(GLenum glBlend){
	D3DBLEND result = D3DBLEND_ONE;
	switch ( glBlend ) {
	case GL_ZERO: result = D3DBLEND_ZERO; break;
	case GL_ONE: result = D3DBLEND_ONE; break;
	case GL_DST_COLOR: result = D3DBLEND_DESTCOLOR; break;
	case GL_ONE_MINUS_DST_COLOR: result = D3DBLEND_INVDESTCOLOR; break;
	case GL_SRC_ALPHA: result = D3DBLEND_SRCALPHA; break;
	case GL_ONE_MINUS_SRC_ALPHA: result = D3DBLEND_INVSRCALPHA; break;
	case GL_DST_ALPHA: result = D3DBLEND_DESTALPHA; break;
	case GL_ONE_MINUS_DST_ALPHA: result = D3DBLEND_INVDESTALPHA; break;
	case GL_SRC_ALPHA_SATURATE: result = D3DBLEND_SRCALPHASAT; break;
	default: LocalDebugBreak(); break;
	}
	return result;
}

static D3DBLEND GLToDXDBlend(GLenum glBlend){
	D3DBLEND result = D3DBLEND_ONE;
	switch ( glBlend ) {
	case GL_ZERO: result = D3DBLEND_ZERO; break;
	case GL_ONE: result = D3DBLEND_ONE; break;
	case GL_SRC_COLOR: result = D3DBLEND_SRCCOLOR; break;
	case GL_ONE_MINUS_SRC_COLOR: result = D3DBLEND_INVSRCCOLOR; break;
	case GL_SRC_ALPHA: result = D3DBLEND_SRCALPHA; break;
	case GL_ONE_MINUS_SRC_ALPHA: result = D3DBLEND_INVSRCALPHA; break;
	case GL_DST_ALPHA: result = D3DBLEND_DESTALPHA; break;
	case GL_ONE_MINUS_DST_ALPHA: result = D3DBLEND_INVDESTALPHA; break;
	default: LocalDebugBreak(); break;
	}
	return result;
}

static D3DCMPFUNC GLToDXCompare(GLenum func){
	D3DCMPFUNC result = D3DCMP_ALWAYS;
	switch ( func ) {
	case GL_NEVER: result = D3DCMP_NEVER; break;
	case GL_LESS: result = D3DCMP_LESS; break;
	case GL_EQUAL: result = D3DCMP_EQUAL; break;
	case GL_LEQUAL: result = D3DCMP_LESSEQUAL; break;
	case GL_GREATER: result = D3DCMP_GREATER; break;
	case GL_NOTEQUAL: result = D3DCMP_NOTEQUAL; break;
	case GL_GEQUAL: result = D3DCMP_GREATEREQUAL; break;
	case GL_ALWAYS: result = D3DCMP_ALWAYS; break;
	default: break;
	}
	return result;
}

/*
   OpenGL                      MinFilter           MipFilter       Comments
   GL_NEAREST                  D3DTFN_POINT        D3DTFP_NONE
   GL_LINEAR                   D3DTFN_LINEAR       D3DTFP_NONE
   GL_NEAREST_MIPMAP_NEAREST   D3DTFN_POINT        D3DTFP_POINT
   GL_LINEAR_MIPMAP_NEAREST    D3DTFN_LINEAR       D3DTFP_POINT    bilinear
   GL_NEAREST_MIPMAP_LINEAR    D3DTFN_POINT        D3DTFP_LINEAR
   GL_LINEAR_MIPMAP_LINEAR     D3DTFN_LINEAR       D3DTFP_LINEAR   trilinear
*/
static D3DTEXTUREFILTERTYPE GLToDXMinFilter(GLint filter){
	D3DTEXTUREFILTERTYPE result = D3DTEXF_LINEAR;
	switch ( filter ) {
	case GL_NEAREST: result = D3DTEXF_POINT; break;
	case GL_LINEAR: result = D3DTEXF_LINEAR; break;
	case GL_NEAREST_MIPMAP_NEAREST: result = D3DTEXF_POINT; break;
	case GL_LINEAR_MIPMAP_NEAREST: result = D3DTEXF_LINEAR; break;
	case GL_NEAREST_MIPMAP_LINEAR: result = D3DTEXF_POINT; break;
	case GL_LINEAR_MIPMAP_LINEAR: result = D3DTEXF_LINEAR; break;
	default:
		LocalDebugBreak();
		break;
	}
	return result;
}

static D3DTEXTUREFILTERTYPE GLToDXMipFilter(GLint filter){
	D3DTEXTUREFILTERTYPE result = D3DTEXF_LINEAR;
	switch ( filter ) {
	case GL_NEAREST: result = D3DTEXF_NONE; break;
	case GL_LINEAR: result = D3DTEXF_NONE; break;
	case GL_NEAREST_MIPMAP_NEAREST: result = D3DTEXF_POINT; break;
	case GL_LINEAR_MIPMAP_NEAREST: result = D3DTEXF_POINT; break;
	case GL_NEAREST_MIPMAP_LINEAR: result = D3DTEXF_LINEAR; break;
	case GL_LINEAR_MIPMAP_LINEAR: result = D3DTEXF_LINEAR; break;
	default:
		LocalDebugBreak();
		break;
	}
	return result;
}

static D3DTEXTUREFILTERTYPE GLToDXMagFilter(GLint filter){
	D3DTEXTUREFILTERTYPE result = D3DTEXF_POINT;
	switch ( filter ) {
	case GL_NEAREST: result = D3DTEXF_POINT; break;
	case GL_LINEAR: result = D3DTEXF_LINEAR; break;
	default:
		LocalDebugBreak();
		break;
	}
	return result;
}

static D3DTEXTUREOP GLToDXTextEnvMode(GLint mode){
	D3DTEXTUREOP result = D3DTOP_MODULATE;
	switch ( mode ) {
	case GL_MODULATE: result = D3DTOP_MODULATE; break;
	case GL_DECAL: result = D3DTOP_SELECTARG1; break; // Fix this
	case GL_BLEND: result = D3DTOP_BLENDTEXTUREALPHA; break;
	case GL_REPLACE: result = D3DTOP_SELECTARG1; break;
	default: break;
	}
	return result;
}

#define MAXSTATES 8

class TextureStageState {
public:
	TextureStageState() {
		m_currentTexture = 0;
		m_glTextEnvMode = GL_MODULATE;
		m_glTexture2D = false;
		m_dirty = true;
	}

	bool GetDirty() { return m_dirty; }
	void SetDirty(bool dirty) { m_dirty = dirty; }

	void DirtyTexture(GLuint textureID) {
		if ( textureID == m_currentTexture ) {
			m_dirty = true;
		}
	}

	GLuint GetCurrentTexture() { return m_currentTexture; }
	void SetCurrentTexture(GLuint texture) { m_dirty = true; m_currentTexture = texture; }

	GLfloat GetTextEnvMode() { return m_glTextEnvMode; }
	void SetTextEnvMode(GLfloat mode) { m_dirty = true; m_glTextEnvMode = mode; }

	bool GetTexture2D() { return m_glTexture2D; }
	void SetTexture2D(bool texture2D) { m_dirty = true; m_glTexture2D = texture2D; }

private:
	
	GLuint m_currentTexture;
	GLfloat m_glTextEnvMode;
	bool m_glTexture2D;
	bool m_dirty;
};

class TextureState {
public:
	TextureState(){
		m_currentStage = 0;
		memset(&m_stage, 0, sizeof(m_stage));
		m_dirty = false;
		m_mainBlend = false;
	}

	void SetMaxStages(int maxStages){
		m_maxStages = maxStages;
		for(int i = 0; i < m_maxStages;i++){
			m_stage[i].SetDirty(true);
		}
		m_dirty = true;
	}

	// Keep track of changes to texture stage state
	void SetCurrentStage(int index){
		m_currentStage = index;
	}

	int GetMaxStages() { return m_maxStages; }
	bool GetDirty() { return m_dirty; }
	void DirtyTexture(int textureID){
		for(int i = 0; i < m_maxStages;i++){
			m_stage[i].DirtyTexture(textureID);
		}
		m_dirty = true;
	}

	void SetMainBlend(bool mainBlend){
		m_mainBlend = mainBlend;
		m_stage[0].SetDirty(true);
		m_dirty = true;
	}

	// These methods apply to the current stage

	GLuint GetCurrentTexture() { return Get()->GetCurrentTexture(); }
	void SetCurrentTexture(GLuint texture) { m_dirty = true; Get()->SetCurrentTexture(texture); }

	GLfloat GetTextEnvMode() { return Get()->GetTextEnvMode(); }
	void SetTextEnvMode(GLfloat mode) { m_dirty = true; Get()->SetTextEnvMode(mode); }

	bool GetTexture2D() { return Get()->GetTexture2D(); }
	void SetTexture2D(bool texture2D) { m_dirty = true; Get()->SetTexture2D(texture2D); }

	void SetTextureStageState(LPDIRECT3DDEVICE pD3DDev, TextureTable* textures){
		if ( ! m_dirty ) {
			return;
		}
		static bool firstTime = true;
		if ( firstTime ) {
			firstTime = false;
			for(int i = 0; i < m_maxStages; i++ ) {
				pD3DDev->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
			}
		}

		m_dirty = false;

		for(int i = 0; i < m_maxStages; i++ ) {
			if ( ! m_stage[i].GetDirty() ) {
				continue;
			}
			m_stage[i].SetDirty(false);
			if ( m_stage[i].GetTexture2D() ) {
				DWORD color1 = D3DTA_TEXTURE;
				int textEnvMode =  m_stage[i].GetTextEnvMode();
				DWORD colorOp = GLToDXTextEnvMode(textEnvMode);
				if ( i > 0 && textEnvMode == GL_BLEND ) {
					// Assume we're doing multi-texture light mapping.
					// I don't think this is the right way to do this
					// but it works for D3DQuake.
					colorOp = D3DTOP_MODULATE;
					color1 |= D3DTA_COMPLEMENT;
				}
				pD3DDev->SetTextureStageState( i, D3DTSS_COLORARG1, color1);
				pD3DDev->SetTextureStageState( i, D3DTSS_COLORARG2, i == 0 ? D3DTA_DIFFUSE :  D3DTA_CURRENT);
				pD3DDev->SetTextureStageState( i, D3DTSS_COLOROP, colorOp);
				DWORD alpha1 = D3DTA_TEXTURE;
				DWORD alpha2 = D3DTA_DIFFUSE;
				DWORD alphaOp;
				alphaOp = GLToDXTextEnvMode(textEnvMode);
				if (i == 0 && m_mainBlend ) {
					alphaOp = D3DTOP_MODULATE;	// Otherwise the console is never transparent
				}
				pD3DDev->SetTextureStageState( i, D3DTSS_ALPHAARG1, alpha1);
				pD3DDev->SetTextureStageState( i, D3DTSS_ALPHAARG2, alpha2);
				pD3DDev->SetTextureStageState( i, D3DTSS_ALPHAOP,   alphaOp);

				TextureEntry* entry = textures->GetEntry(m_stage[i].GetCurrentTexture());
				if ( entry ) {
					int minFilter = entry->m_glTexParameter2DMinFilter;
					DWORD dxMinFilter = GLToDXMinFilter(minFilter);
					DWORD dxMipFilter = GLToDXMipFilter(minFilter);
					DWORD dxMagFilter = GLToDXMagFilter(entry->m_glTexParameter2DMagFilter);

					// Avoid setting anisotropic if the user doesn't request it.
					static bool bSetMaxAnisotropy = false;
					if ( entry->m_maxAnisotropy != 1.0f ) {
						bSetMaxAnisotropy = true;
						if ( dxMagFilter == D3DTEXF_LINEAR) {
							dxMagFilter = D3DTEXF_ANISOTROPIC;
						}
						if ( dxMinFilter == D3DTEXF_LINEAR) {
							dxMinFilter = D3DTEXF_ANISOTROPIC;
						}
					}
					if ( bSetMaxAnisotropy ) {
						pD3DDev->SetTextureStageState( i, D3DTSS_MAXANISOTROPY, entry->m_maxAnisotropy);
					}
					pD3DDev->SetTextureStageState( i, D3DTSS_MINFILTER, dxMinFilter );
					pD3DDev->SetTextureStageState( i, D3DTSS_MIPFILTER, dxMipFilter );
					pD3DDev->SetTextureStageState( i, D3DTSS_MAGFILTER,  dxMagFilter);
					IDirect3DTexture8* pTexture = entry->m_mipMap;
					// char buf[100];
					// sprintf(buf,"SetTexture 0x%08x\n", pTexture);
					// OutputDebugString(buf);
					if ( pTexture ) {
						pD3DDev->SetTexture( i, pTexture);
					}
					else {
						LocalDebugBreak();
					}
				}
			}
			else {
				pD3DDev->SetTexture( i, NULL);
				pD3DDev->SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pD3DDev->SetTextureStageState( i, D3DTSS_COLORARG2, i == 0 ? D3DTA_DIFFUSE :  D3DTA_CURRENT);
				pD3DDev->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_DISABLE);
			}
		}
	}

private:
	TextureStageState* Get() {
		return m_stage + m_currentStage;
	}

	bool m_dirty;
	bool m_mainBlend;
	int m_maxStages;
	int m_currentStage;
	TextureStageState m_stage[MAXSTATES];
};

// This class buffers up all the glVertex calls between
// glBegin and glEnd.
//
// Choose one of these three
// USE_DRAWINDEXEDPRIMITIVE seems slightly faster (54 fps vs 53 fps) than USE_DRAWPRIMITIVE.
// USE_DRAWINDEXEDPRIMITIVEVB is much slower (30fps vs 54fps), at least on GeForce Win9x 3.75.

// DrawPrimitive works for DX8, the other ones don't work right yet.

#ifdef _XBOX
#define USE_BEGINEND
#else
#define USE_DRAWPRIMITIVE
#endif

// #define USE_DRAWINDEXEDPRIMITIVE
// #define USE_DRAWINDEXEDPRIMITIVEVB

#if defined(USE_DRAWINDEXEDPRIMITIVE) || defined(USE_DRAWINDEXEDPRIMITIVEVB)
#define USE_INDECIES
#endif

#ifdef USE_DRAWINDEXEDPRIMITIVEVB
// The DX 7 docs suggest that you can get away with just one
// vertex buffer. But drivers (NVIDIA 3.75 on Win2K) don't seem to like that.

#endif

#ifdef USE_INDECIES
#define VERTSUSED 400
#define VERTSSLOP 100
#endif

#ifdef USE_INDECIES

class OGLPrimitiveVertexBuffer {
public:
	OGLPrimitiveVertexBuffer(){
		m_drawMode = (GLuint) -1;
		m_size = 0;
		m_count = 0;
		m_OGLPrimitiveVertexBuffer = 0;
		m_vertexCount = 0;
		m_vertexTypeDesc = 0;
		memset(m_textureCoords, 0, sizeof(m_textureCoords));

		m_pD3DDev = 0;
#ifdef USE_DRAWINDEXEDPRIMITIVEVB
		m_buffer = 0;
#else
		m_buffer = 0;
#endif
		m_color = (DWORD) D3DRGBA(0.0,0.0,0.0,1.0); // Don't know if this is correct
		m_indecies = 0;
		m_indexBuffer = 0;
		m_indexBase = 0;
		m_indexCount = 0;
	}

	~OGLPrimitiveVertexBuffer(){
		RELEASENULL(m_indexBuffer);
#ifdef USE_DRAWINDEXEDPRIMITIVEVB
			RELEASENULL(m_buffer);
#else
		delete[] m_buffer;
#endif
	}

	HRESULT Initialize(LPDIRECT3DDEVICE pD3DDev, DX_DIRECT3D* pD3D, bool hardwareTandL, DWORD typeDesc){
		m_pD3DDev = pD3DDev;

		int numVerts = VERTSUSED + VERTSSLOP;

		m_vertexTypeDesc = typeDesc;
		m_vertexSize = 0;
		if ( m_vertexTypeDesc & D3DFVF_XYZ ) {
			m_vertexSize += 3 * sizeof(float);
		}
		if ( m_vertexTypeDesc & D3DFVF_DIFFUSE ) {
			m_vertexSize += 4;
		}
		int textureStages = (m_vertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
		m_vertexSize += 2 * sizeof(float) * textureStages;

		m_indexSize = numVerts * 3;
		{
			HRESULT hr;
			RELEASENULL(m_indexBuffer);
			hr = pD3DDev->CreateIndexBuffer(m_indexSize*sizeof(WORD), D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX_16, D3DPOOL_DEFAULT, &m_indexBuffer);
			if ( FAILED(hr) ) {
				return hr;
			}
			hr = m_indexBuffer->Lock(0, m_indexSize*sizeof(WORD), (unsigned char**) & m_indecies, D3DLOCK_DISCARD);
			if ( FAILED(hr) ) {
				return hr;
			}
		}

#ifdef USE_DRAWINDEXEDPRIMITIVEVB
		RELEASENULL(m_buffer);
		m_size = m_vertexSize * numVerts;
		HRESULT hr = pD3DDev->CreateVertexBuffer(m_size, typeDesc, D3DUSAGE_WRITEONLY,
			D3DPOOL_DEFAULT, &m_buffer);
		if ( FAILED(hr) ) {
			return hr;
		}
		m_pD3DDev->SetStreamSource( 0, m_buffer, m_vertexSize );
		m_pD3DDev->SetVertexShader( m_vertexTypeDesc );
		m_pD3DDev->SetIndices( m_indexBuffer, 0);
		m_vertexBase = 0;
#else
		m_size = (VERTSUSED + VERTSSLOP) * m_vertexSize;
		delete[] m_buffer;
		m_buffer = new char[m_size];
#endif
		
		return S_OK;
	}

	DWORD GetVertexTypeDesc(){
		return m_vertexTypeDesc;
	}

	LPVOID GetOGLPrimitiveVertexBuffer(){
		return m_OGLPrimitiveVertexBuffer;
	}

	DWORD GetVertexCount(){
		return m_vertexCount;
	}

	inline void SetColor(D3DCOLOR color){
		m_color = color;
	}
	
	inline void SetTextureCoord0(float u, float v){
		DWORD* pCoords = (DWORD*) m_textureCoords;
		pCoords[0] = *(DWORD*)& u;
		pCoords[1] = *(DWORD*)& v;
	}

	inline void SetTextureCoord(int textStage, float u, float v){
		DWORD* pCoords = (DWORD*) m_textureCoords + (textStage << 1);
		pCoords[0] = *(DWORD*)& u;
		pCoords[1] = *(DWORD*)& v;
	}

	void CheckFlush() {
		if ( m_size && m_indexCount &&
			((m_count + m_vertexSize * VERTSSLOP > m_size )
			|| (m_indexCount + VERTSSLOP*6 > m_indexSize) ) ) {
			Flush();
		}
	}

	static void DumpBuffer(const void* vertexBuffer, int vbBaseIndex, int vertexCount, const WORD* indexBuffer, int indexCount){
		char buf[100];
		const float* vf = (const float*) vertexBuffer;
		const unsigned char* vb = (const unsigned char*) vertexBuffer;
		const int vertexSizeBytes = 3*4 + 4 + 2*2*4; // assumes 2 textures / vertex.
		int vbEndIndex = vbBaseIndex + vertexCount;
		static int serialNumber;
		++serialNumber;
		sprintf(buf, "\nBuffer %d vertexCount: %4d indexCount: %d (%d).\n", serialNumber, vertexCount, indexCount, indexCount / 3);
		OutputDebugString(buf);
		if ( indexCount < vertexCount ) {
			OutputDebugString("indexCount too small for vertexCount");
			LocalDebugBreak();
		}
		{
			for(int i = 0; i < vertexCount; i++){
				sprintf(buf, "%3d (%6g,%6g,%6g) (%02x%02x%02x%02x) (%6g %6g) (%6g %6g)\n",
					vbBaseIndex + i, vf[0], vf[1], vf[2], vb[12], vb[13], vb[14], vb[15],
					vf[4], vf[5],
					vf[6], vf[7]);
				OutputDebugString(buf);
				Sleep(1);
				vb += vertexSizeBytes;
				vf = (const float*) vb;
			}
		}
		{
			for(int i = 0; i < indexCount; i += 3){
				sprintf(buf, "%4d (%d,%d,%d)\n", i, indexBuffer[i], indexBuffer[i+1], indexBuffer[i+2]);
				OutputDebugString(buf);
				Sleep(1);
				for(int j = 0; j < 3; j++) {
					if ( indexBuffer[i+j] < vbBaseIndex || indexBuffer[i+j] >= vbEndIndex ) {
						sprintf(buf, "Warning: index out of range: %d\n", indexBuffer[i+j]);
						OutputDebugString(buf);
					}
				}
			}
		}
#if 0
		if ( serialNumber == 5 ) {
			Sleep(30);
			LocalDebugBreak();
		}
#endif
	}

	void Flush() {
		if ( m_indexCount > 0 ) {
#ifdef USE_DRAWINDEXEDPRIMITIVEVB
			DumpBuffer(m_OGLPrimitiveVertexBuffer + m_vertexBase * m_vertexSize,
				m_vertexBase, m_vertexCount - m_vertexBase, m_indecies, m_indexCount);
			m_OGLPrimitiveVertexBuffer = 0;
			m_buffer->Unlock();
			m_indexBuffer->Unlock();
			HRESULT hr =
				m_pD3DDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
				m_vertexBase, m_vertexCount - m_vertexBase,
					m_indexBase, m_indexCount / 3);
			if ( FAILED(hr) ) {
				LocalDebugBreak();
			}
			DWORD lockFlag;
			m_indexBase += m_indexCount;
			if ( m_indexBase < VERTSSLOP * 3) {
				lockFlag = D3DLOCK_NO_OVERWRITE;
			}
			else {
				m_indexBase = 0;
				lockFlag = D3DLOCK_DISCARD;
			}
			hr = m_indexBuffer->Lock(m_indexBase*sizeof(WORD), (m_indexSize-m_indexBase)*sizeof(WORD),
				(unsigned char**) & m_indecies, lockFlag);

			if ( FAILED(hr) ) {
				// LocalDebugBreak(); // ? NVidia driver sometimes says it's out of memory
			}
#else
			m_OGLPrimitiveVertexBuffer = 0;
     		HRESULT hr = m_pD3DDev->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST, m_vertexTypeDesc, m_buffer,
				m_vertexCount, m_indecies, m_indexCount, 0);
			if ( FAILED(hr) ) {
				// LocalDebugBreak(); // ? NVidia driver sometimes says it's out of memory
			}
#endif
		}
		else {
			LocalDebugBreak();
		}
		m_indexCount = 0;
		m_vertexState = 0;
	}

	void SetVertex(float x, float y, float z){
		bool bCheckFlush = false;
		if (m_count + m_vertexSize > m_size) {
			Ensure(m_vertexSize);
		}
		if ( ! m_OGLPrimitiveVertexBuffer ) {
			LockBuffer();
		}
		DWORD* pFloat = (DWORD*) (m_OGLPrimitiveVertexBuffer + m_count);
		pFloat[0] = *(DWORD*)& x;
		pFloat[1] = *(DWORD*)& y;
		pFloat[2] = *(DWORD*)& z;
		const DWORD* pCoords = (DWORD*) m_textureCoords;
		switch(m_vertexTypeDesc){
		case (D3DFVF_XYZ | D3DFVF_DIFFUSE | (1 << D3DFVF_TEXCOUNT_SHIFT)):
			pFloat[3] = m_color;
			pFloat[4] = pCoords[0];
			pFloat[5] = pCoords[1];
			break;
		case (D3DFVF_XYZ | D3DFVF_DIFFUSE | (2 << D3DFVF_TEXCOUNT_SHIFT)):
			pFloat[3] = m_color;
			pFloat[4] = pCoords[0];
			pFloat[5] = pCoords[1];
			pFloat[6] = pCoords[2];
			pFloat[7] = pCoords[3];
			break;
		default:
			{
				if ( m_vertexTypeDesc & D3DFVF_DIFFUSE ) {
					*pFloat++ = m_color;
				}
				int textureStages = (m_vertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
				for ( int i = 0; i < textureStages; i++ ) {
					*pFloat++ = *pCoords++;
					*pFloat++ = *pCoords++;
				}
			}
			break;
		}

		if( m_indexCount < m_indexSize - 5){
			// Convert quads to double triangles
			switch ( m_drawMode ) {
			default:
				LocalDebugBreak();
				break;
			case GL_TRIANGLES:
				m_indecies[m_indexCount++] = m_vertexCount;
				if ( m_vertexState++ == 2 ) {
					m_vertexState = 0;
					bCheckFlush = true; // Flush for long sequences of triangles.
				}
				break;
			case GL_QUADS:
				{
					if ( m_vertexState++ < 3) {
						m_indecies[m_indexCount++] = m_vertexCount;
					}
					else {
						// We've already done triangle (0 , 1, 2), now draw (2, 3, 0)
						m_indecies[m_indexCount++] = m_vertexCount-1;
						m_indecies[m_indexCount++] = m_vertexCount;
						m_indecies[m_indexCount++] = m_vertexCount-3;
						m_vertexState = 0;
						bCheckFlush = true; // Flush for long sequences of quads.
					}
				}
				break;
			case GL_TRIANGLE_STRIP:
				{
					if ( m_vertexState > VERTSSLOP ) {
						// This is a strip that's too big for us to buffer.
						// (We can't just flush the buffer because we have to keep
						// track of the last two vertices.
						LocalDebugBreak();
					}
					if ( m_vertexState++ < 3) {
						m_indecies[m_indexCount++] = m_vertexCount;
					}
					else {
						// Flip triangles between clockwise and counter clockwise
						if (m_vertexState & 1) {
							// draw triangle [n-2 n-1 n]
							m_indecies[m_indexCount++] = m_vertexCount-2;
							m_indecies[m_indexCount++] = m_vertexCount-1;
							m_indecies[m_indexCount++] = m_vertexCount;
						}
						else {
							// draw triangle [n-1 n-2 n]
							m_indecies[m_indexCount++] = m_vertexCount-1;
							m_indecies[m_indexCount++] = m_vertexCount-2;
							m_indecies[m_indexCount++] = m_vertexCount;
						}
					}
				}
				break;
			case GL_TRIANGLE_FAN:
			case GL_POLYGON:
				{
					if ( m_vertexState > VERTSSLOP ) {
						// This is a polygon or fan that's too big for us to buffer.
						// (We can't just flush the buffer because we have to keep
						// track of the starting vertex.
						LocalDebugBreak();
					}
					if ( m_vertexState++ < 3) {
						m_indecies[m_indexCount++] = m_vertexCount;
					}
					else {
						// Draw triangle [0 n-1 n]
						m_indecies[m_indexCount++] = m_vertexCount-(m_vertexState-1);
						m_indecies[m_indexCount++] = m_vertexCount-1;
						m_indecies[m_indexCount++] = m_vertexCount;
					}
				}
				break;
			}
		}
		else {
			LocalDebugBreak();
		}

		m_count += m_vertexSize;
		m_vertexCount++;
		if ( bCheckFlush ) {
			CheckFlush();
		}
	}

	inline IsMergableMode(GLenum /* mode */){
		CheckFlush();
		return true;
	}

	void Begin(GLuint drawMode){
		m_drawMode = drawMode;
		CheckFlush();
		if ( ! m_OGLPrimitiveVertexBuffer ) {
			LockBuffer();
		}
		m_vertexState = 0;
	}

	void Append(GLuint drawMode){
		m_drawMode = drawMode;
		CheckFlush();
		m_vertexState = 0;
	}

	void LockBuffer(){
		if ( ! m_OGLPrimitiveVertexBuffer ) {
#ifdef USE_DRAWINDEXEDPRIMITIVEVB
			void* memory = 0;
			// If there's room in the buffer, we try to append to what's already there.
			DWORD dwFlags = 0;
			if ( m_vertexCount > 0 && m_vertexCount < VERTSUSED ){
				dwFlags |= D3DLOCK_NO_OVERWRITE;
			}
			else {
				m_vertexCount = 0;
				m_count = 0;
				dwFlags |= D3DLOCK_DISCARD;
			}
			m_vertexBase = m_vertexCount;
			HRESULT hr = m_buffer->Lock(m_count, m_size - m_count,
				(unsigned char**) & memory, dwFlags);
			if ( FAILED(hr) || ! memory) {
				char errStr[100];
				D3DXGetErrorString(hr, errStr, sizeof(errStr) / sizeof(errStr[0]) );
				MessageBox(NULL,errStr,"D3DX Error",MB_OK);
				LocalDebugBreak();
			}
			m_OGLPrimitiveVertexBuffer = (char*) memory;
#else
			m_OGLPrimitiveVertexBuffer = (char*) m_buffer;
			m_vertexCount = 0;
			m_count = 0;
#endif
			m_indexCount = 0;
		}
	}

	void End(){
		if ( m_indexCount == 0 ) { // Startup
			return;
		}
		Flush();
	}
private:
	void Ensure(int size){
		if (( m_count + size ) > m_size ) {
			LocalDebugBreak();
		}
	}

	GLuint m_drawMode;
	DWORD  m_vertexTypeDesc;
	int m_vertexSize; // in bytes

	LPDIRECT3DDEVICE m_pD3DDev;
#ifdef USE_DRAWINDEXEDPRIMITIVEVB
	IDirect3DVertexBuffer8* m_buffer;
	DWORD m_vertexBase; // First vertex in the buffer that we're using during this call to DrawIndexedPrimitive;
#else
	char* m_buffer;
#endif
	char* m_OGLPrimitiveVertexBuffer;
	DWORD m_size; // total vertex buffer size in bytes
	DWORD m_count; // used ammount of vertex buffer, in bytes
	DWORD m_vertexCount;
	DWORD m_indexCount;
	int m_vertexState; // Cycles from 0..n-1 where n is the number of verticies in a primitive.
	DWORD m_indexSize; // size in WORDs, not bytes
	WORD* m_indecies;
	IDirect3DIndexBuffer8* m_indexBuffer;
	DWORD m_indexBase; // size in WORDS, not bytes
	D3DCOLOR m_color;
	float m_textureCoords[MAXSTATES*2];
};
#endif

#ifdef USE_DRAWPRIMITIVE
class OGLPrimitiveVertexBuffer {
public:
	OGLPrimitiveVertexBuffer(){
		m_drawMode = -1;
		m_size = 0;
		m_count = 0;
		m_OGLPrimitiveVertexBuffer = 0;
		m_vertexCount = 0;
		m_vertexTypeDesc = 0;
        m_bShaderTypeDirty = true;
		memset(m_textureCoords, 0, sizeof(m_textureCoords));

		m_pD3DDev = 0;
		m_color = 0xff000000; // Don't know if this is correct
	}

	~OGLPrimitiveVertexBuffer(){
		delete [] m_OGLPrimitiveVertexBuffer;
	}

	HRESULT Initialize(LPDIRECT3DDEVICE pD3DDev, DX_DIRECT3D* pD3D, bool hardwareTandL, DWORD typeDesc){
		m_pD3DDev = pD3DDev;
		if (m_vertexTypeDesc != typeDesc) {
			m_vertexTypeDesc = typeDesc;
            m_bShaderTypeDirty = true;
			m_vertexSize = 0;
			if ( m_vertexTypeDesc & D3DFVF_XYZ ) {
				m_vertexSize += 3 * sizeof(float);
			}
			if ( m_vertexTypeDesc & D3DFVF_DIFFUSE ) {
				m_vertexSize += 4;
			}
			int textureStages = (m_vertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
			m_vertexSize += 2 * sizeof(float) * textureStages;
		}
		return S_OK;
	}

	DWORD GetVertexTypeDesc(){
		return m_vertexTypeDesc;
	}

	LPVOID GetOGLPrimitiveVertexBuffer(){
		return m_OGLPrimitiveVertexBuffer;
	}

	DWORD GetVertexCount(){
		return m_vertexCount;
	}

	inline void SetColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a){
		m_color = (a << 24) | (r << 16) | (g << 8) | b;
	}

	inline void SetColor(GLubyte r, GLubyte g, GLubyte b){
		m_color = (0xff << 24) | (r << 16) | (g << 8) | b;
	}

    inline void SetColor(float r, float g, float b, float a){
		// Note: On x86 architectures this function will chew up a lot of time
		// converting floating point to integer by calling _ftol
		// unless the /QIfist flag is specified.

        m_color = D3DRGBFAST(Clamp(r), Clamp(g), Clamp(b), Clamp(a));
	}

    inline void SetColor(float r, float g, float b){
		// Note: On x86 architectures this function will chew up a lot of time
		// converting floating point to integer by calling _ftol
		// unless the /QIfist flag is specified.

        m_color = D3DRGBFAST3(Clamp(r), Clamp(g), Clamp(b));
	}

	inline void SetTextureCoord0(float u, float v){
		DWORD* pCoords = (DWORD*) m_textureCoords;
		pCoords[0] = *(DWORD*)& u;
		pCoords[1] = *(DWORD*)& v;
	}

	inline void SetTextureCoord(int textStage, float u, float v){
		DWORD* pCoords = (DWORD*) m_textureCoords + (textStage << 1);
		pCoords[0] = *(DWORD*)& u;
		pCoords[1] = *(DWORD*)& v;
	}

	void SetVertex(float x, float y, float z){
		int newCount = m_count + m_vertexSize;
		if (newCount > m_size) {
			Ensure(m_vertexSize);
		}
		DWORD* pFloat = (DWORD*) (m_OGLPrimitiveVertexBuffer + m_count);
		pFloat[0] = *(DWORD*)& x;
		pFloat[1] = *(DWORD*)& y;
		pFloat[2] = *(DWORD*)& z;
		const DWORD* pCoords = (DWORD*) m_textureCoords;
		switch(m_vertexTypeDesc){
		case (D3DFVF_XYZ | D3DFVF_DIFFUSE | (1 << D3DFVF_TEXCOUNT_SHIFT)):
			pFloat[3] = m_color;
			pFloat[4] = pCoords[0];
			pFloat[5] = pCoords[1];
			break;
		case (D3DFVF_XYZ | D3DFVF_DIFFUSE | (2 << D3DFVF_TEXCOUNT_SHIFT)):
			pFloat[3] = m_color;
			pFloat[4] = pCoords[0];
			pFloat[5] = pCoords[1];
			pFloat[6] = pCoords[2];
			pFloat[7] = pCoords[3];
			break;
		default:
			{
				if ( m_vertexTypeDesc & D3DFVF_DIFFUSE ) {
					*pFloat++ = m_color;
				}
				int textureStages = (m_vertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
				for ( int i = 0; i < textureStages; i++ ) {
					*pFloat++ = *pCoords++;
					*pFloat++ = *pCoords++;
				}
			}
			break;
		}
		m_count = newCount;
		m_vertexCount++;

		// TO DO: Flush vertex buffer if larger than 1000 vertexes.
		// Have to do this modulo vertexes-per-primitive
	}

	inline IsMergableMode(GLenum mode){
		return ( mode == m_drawMode ) && ( mode == GL_QUADS || mode == GL_TRIANGLES );
	}

	void Begin(GLuint drawMode){
		m_drawMode = drawMode;
	}

	void Append(GLuint drawMode){
	}

	void End(){
		if ( m_vertexCount == 0 ) { // Startup
			return;
		}
		D3DPRIMITIVETYPE dptPrimitiveType;
		switch ( m_drawMode ) {
		case GL_POINTS: dptPrimitiveType = D3DPT_POINTLIST; break;
		case GL_LINES: dptPrimitiveType = D3DPT_LINELIST; break;
		case GL_LINE_STRIP: dptPrimitiveType = D3DPT_LINESTRIP; break;
		case GL_LINE_LOOP:
			dptPrimitiveType = D3DPT_LINESTRIP;
			LocalDebugBreak();  // Need to add one more point
			break;
		case GL_TRIANGLES: dptPrimitiveType = D3DPT_TRIANGLELIST; break;
		case GL_TRIANGLE_STRIP: dptPrimitiveType = D3DPT_TRIANGLESTRIP; break;
		case GL_TRIANGLE_FAN: dptPrimitiveType = D3DPT_TRIANGLEFAN; break;
		case GL_QUADS:
			if ( m_vertexCount <= 4 ) {
				dptPrimitiveType = D3DPT_TRIANGLEFAN;
			}
			else {
				dptPrimitiveType = D3DPT_TRIANGLELIST;
				ConvertQuadsToTriangles();
			}
			break;
		case GL_QUAD_STRIP:
			if ( m_vertexCount <= 4 ) {
				dptPrimitiveType = D3DPT_TRIANGLEFAN;
			}
			else {
				dptPrimitiveType = D3DPT_TRIANGLESTRIP;
				ConvertQuadStripToTriangleStrip();
			}
			break;

		case GL_POLYGON:
			dptPrimitiveType = D3DPT_TRIANGLEFAN;
			if ( m_vertexCount < 3) {
				goto exit;
			}
			// How is this different from GL_TRIANGLE_FAN, other than
			// that polygons are planar?
			break;
		default:
			LocalDebugBreak();
			goto exit;
		}
		{
			DWORD primCount;
			switch ( dptPrimitiveType ) {
			default:
			case D3DPT_TRIANGLESTRIP: primCount = m_vertexCount - 2; break;
			case D3DPT_TRIANGLEFAN: primCount = m_vertexCount - 2; break;
			case D3DPT_TRIANGLELIST: primCount = m_vertexCount / 3; break;
			}

            HRESULT hr;
            if(m_bShaderTypeDirty){
                m_bShaderTypeDirty = false;
                hr = m_pD3DDev->SetVertexShader(m_vertexTypeDesc);
            }
     		hr = m_pD3DDev->DrawPrimitiveUP(
				dptPrimitiveType,
				primCount, m_OGLPrimitiveVertexBuffer, m_vertexSize);
			if ( FAILED(hr) ) {
				// LocalDebugBreak();
			}
		}
exit:
		m_vertexCount = 0;
		m_count = 0;
	}

private:
	void ConvertQuadsToTriangles(){
		int quadCount = m_vertexCount / 4;
		int addedVerticies = 2 * quadCount;
		int addedDataSize = addedVerticies * m_vertexSize;
		Ensure( addedDataSize );

		// A quad is v0, v1, v2, v3
		// The corresponding triangle pair is v0 v1 v2 , v0 v2 v3
		for(int i = quadCount-1; i >= 0; i--) {
			int startOfQuad = i * m_vertexSize * 4;
			int startOfTrianglePair = i * m_vertexSize * 6;
			// Copy the last two verticies of the second triangle
			memcpy(m_OGLPrimitiveVertexBuffer + startOfTrianglePair + 4 * m_vertexSize,
				m_OGLPrimitiveVertexBuffer + startOfQuad + m_vertexSize * 2, m_vertexSize * 2);
			// Copy the first vertex of the second triangle
			memcpy(m_OGLPrimitiveVertexBuffer + startOfTrianglePair + 3 * m_vertexSize,
				m_OGLPrimitiveVertexBuffer + startOfQuad, m_vertexSize);
			// Copy the first triangle
			if ( i > 0 ) {
				memcpy(m_OGLPrimitiveVertexBuffer + startOfTrianglePair, m_OGLPrimitiveVertexBuffer + startOfQuad, 3 * m_vertexSize);
			}
		}
		m_count += addedDataSize;
		m_vertexCount += addedVerticies;
	}

	void ConvertQuadStripToTriangleStrip(){
		int vertexPairCount = m_vertexCount / 2;

		// Doesn't add any points, but does reorder the verticies.
		// Swap each pair of verticies.

		for(int i = 0; i < vertexPairCount; i++) {
			int startOfPair = i * m_vertexSize * 2;
			int middleOfPair = startOfPair + m_vertexSize;
			for(int j = 0; j < m_vertexSize; j++) {
				int c = m_OGLPrimitiveVertexBuffer[startOfPair + j];
				m_OGLPrimitiveVertexBuffer[startOfPair + j] = m_OGLPrimitiveVertexBuffer[middleOfPair + j];
				m_OGLPrimitiveVertexBuffer[middleOfPair + j] = (char)c;
			}
		}
	}

	void Ensure(int size){
		if (( m_count + size ) > m_size ) {
			int newSize = m_size * 2;
			if ( newSize < m_count + size ) newSize = m_count + size;
			char* newVB = new char[newSize];
			if ( m_OGLPrimitiveVertexBuffer ) {
				memcpy(newVB, m_OGLPrimitiveVertexBuffer, m_count);
			}
			delete[] m_OGLPrimitiveVertexBuffer;
			m_OGLPrimitiveVertexBuffer = newVB;
			m_size = newSize;
		}
	}

	GLuint m_drawMode;
	DWORD  m_vertexTypeDesc;
    bool m_bShaderTypeDirty;
	int m_vertexSize; // in bytes

	LPDIRECT3DDEVICE m_pD3DDev;
	char* m_OGLPrimitiveVertexBuffer;
	int m_size;  // bytes size of buffer
	int m_count; // bytes used
	DWORD m_vertexCount;
	D3DCOLOR m_color;
	float m_textureCoords[MAXSTATES*2];
};

#endif // USE_DRAWPRIMITIVE

#ifdef USE_BEGINEND
class OGLPrimitiveVertexBuffer {
public:
	OGLPrimitiveVertexBuffer(){
		m_pD3DDev = 0;
        m_needEnd = false;
	}

	~OGLPrimitiveVertexBuffer(){
	}

	HRESULT Initialize(LPDIRECT3DDEVICE pD3DDev, DX_DIRECT3D* pD3D, bool hardwareTandL, DWORD typeDesc){
		m_pD3DDev = pD3DDev;
        m_vertexTypeDesc = typeDesc;
        m_pD3DDev->SetVertexShader(m_vertexTypeDesc);
		return S_OK;
	}

    DWORD GetVertexTypeDesc(){
    	return m_vertexTypeDesc;
    }

	inline bool IsMergableMode(GLenum mode){
		return false;
	}

    inline void Append(GLuint drawMode) {
    }

	inline void SetColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a){
		m_pD3DDev->SetVertexData4ub(D3DVSDE_DIFFUSE, r, g, b, a);
	}

	inline void SetColor(GLubyte r, GLubyte g, GLubyte b){
		m_pD3DDev->SetVertexData4ub(D3DVSDE_DIFFUSE, r, g, b, 0xff);
	}

    inline void SetColor(float r, float g, float b, float a){
		m_pD3DDev->SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, a);
	}

    inline void SetColor(float r, float g, float b){
		m_pD3DDev->SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, 1.0f);
	}

	inline void SetTextureCoord0(float u, float v){
        m_pD3DDev->SetVertexData2f(9, u, v);
	}

	inline void SetTextureCoord(int textStage, float u, float v){
        m_pD3DDev->SetVertexData2f(9+textStage, u, v);
	}

	inline void SetVertex(float x, float y, float z){
        m_pD3DDev->SetVertexData4f(-1, x, y, z, 1.0f);
	}

	inline void Begin(GLuint drawMode){
		m_pD3DDev->Begin((D3DPRIMITIVETYPE)(drawMode+1));
        m_needEnd = true;
	}

	inline void End(){
        if (m_needEnd)
        {
            m_pD3DDev->End();
            m_needEnd = false;
        }
	}

private:
    LPDIRECT3DDEVICE m_pD3DDev;
    DWORD m_vertexTypeDesc;
    bool m_needEnd;
};

#endif // USE_BEGINEND

class FakeGL {
private:
	HWND					m_hwndMain;
	LPDIRECT3DDEVICE        m_pD3DDev;
    D3DSURFACE_DESC			m_d3dsdBackBuffer;   // Surface desc of the backbuffer

	LPDIRECT3D				m_pD3D;
	DX_TEXTURE    m_pPrimary;
	bool m_hardwareTandL;

    BOOL                    m_bD3DXReady;

	bool m_glRenderStateDirty;

	bool m_glAlphaStateDirty;
	GLenum m_glAlphaFunc;
	GLclampf m_glAlphaFuncRef;
	bool m_glAlphaTest;

	bool m_glBlendStateDirty;
	bool m_glBlend;
	GLenum m_glBlendFuncSFactor;
	GLenum m_glBlendFuncDFactor;

	bool m_glCullStateDirty;
	bool m_glCullFace;
	GLenum m_glCullFaceMode;

	bool m_glDepthStateDirty;
	bool m_glDepthTest;
	GLenum m_glDepthFunc;
	bool m_glDepthMask;

	GLclampd m_glDepthRangeNear;
	GLclampd m_glDepthRangeFar;

	GLenum m_glMatrixMode;

	GLenum m_glPolygonModeFront;
	GLenum m_glPolygonModeBack;

	bool m_glShadeModelStateDirty;
	GLenum m_glShadeModel;

	bool m_bViewPortDirty;
	GLint m_glViewPortX;
	GLint m_glViewPortY;
	GLsizei m_glViewPortWidth;
	GLsizei m_glViewPortHeight;

	TextureState m_textureState;
	TextureTable m_textures;

	bool m_modelViewMatrixStateDirty;
	bool m_projectionMatrixStateDirty;
	bool m_textureMatrixStateDirty;
	bool* m_currentMatrixStateDirty; // an alias to one of the preceeding stacks

	ID3DXMatrixStack* m_modelViewMatrixStack;
	ID3DXMatrixStack* m_projectionMatrixStack;
	ID3DXMatrixStack* m_textureMatrixStack;
	ID3DXMatrixStack* m_currentMatrixStack; // an alias to one of the preceeding stacks

	bool m_viewMatrixStateDirty;
	D3DXMATRIX m_d3dViewMatrix;

	OGLPrimitiveVertexBuffer m_OGLPrimitiveVertexBuffer;

	bool m_needBeginScene;

	const char* m_vendor;
	const char* m_renderer;
	char m_version[64];
	const char* m_extensions;
	D3DADAPTER_IDENTIFIER8 m_dddi;

	char* m_stickyAlloc;
	DWORD m_stickyAllocSize;

	bool m_hintGenerateMipMaps;

	HRESULT ReleaseD3DX()
	{
		m_bD3DXReady = FALSE;
		return S_OK;
	}

	HRESULT InitD3DX()
	{
		HRESULT hr;
		m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

#ifndef _XBOX
		BOOL windowed = gFullScreen ? FALSE : TRUE;
		D3DFORMAT displayFormat = (gBpp <= 16 ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8);

		if ( windowed ) {
			// format has to match current screen depth
			D3DDISPLAYMODE mode;
			if ( SUCCEEDED(hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode))) {
				displayFormat = mode.Format;
			}
		}
		D3DFORMAT backBufferFormat = displayFormat;

		if ( FAILED(hr = m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			displayFormat, backBufferFormat, windowed))) {
			if ( gBpp <= 16 ) {
				displayFormat = D3DFMT_X1R5G5B5;
				backBufferFormat = displayFormat;
				if ( FAILED(hr = m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
					displayFormat, backBufferFormat, windowed))) {
					displayFormat = D3DFMT_X8R8G8B8;
					backBufferFormat = displayFormat;
					if ( FAILED(hr = m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
						displayFormat, backBufferFormat, windowed))) {
					}
				}
			}
		}
		if ( FAILED(hr) ) {
			return hr;
		}


		D3DPRESENT_PARAMETERS params = {0};
		params.Windowed               = windowed;
		params.EnableAutoDepthStencil = TRUE;
		params.AutoDepthStencilFormat = D3DFMT_D16;
		params.SwapEffect             = D3DSWAPEFFECT_FLIP; // D3DSWAPEFFECT_COPY_VSYNC;
		params.BackBufferWidth        = gFullScreen ? gWidth : 0;
		params.BackBufferHeight       = gFullScreen ? gHeight : 0;
		params.BackBufferFormat       = backBufferFormat;
        params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	/*
		params.BackBufferCount        = 1;
		params.MultiSampleType        = D3DMULTISAMPLE_NONE;
		params.SwapEffect             = D3DSWAPEFFECT_FLIP;
		*/
		params.hDeviceWindow          = m_hwndMain;
#else

		D3DPRESENT_PARAMETERS params = {0};
		params.BackBufferWidth           = gWidth;
		params.BackBufferHeight          = gHeight;
		params.BackBufferFormat          = D3DFMT_X8R8G8B8;
		params.BackBufferCount           = 1;
		params.Windowed                  = false;   // Must be false for Xbox.
		params.EnableAutoDepthStencil    = true;
		params.AutoDepthStencilFormat    = D3DFMT_D24S8;
		params.SwapEffect                = D3DSWAPEFFECT_DISCARD;
		params.FullScreen_RefreshRateInHz= 60;
		params.hDeviceWindow             = m_hwndMain;
        params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

#endif

		hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			0,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
			  & params,
			  & m_pD3DDev
			);

		if( FAILED(hr) )
			return hr;

		// Store render target surface desc
		{
			LPDIRECT3DSURFACE8 pBackBuffer;
			m_pD3DDev->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
			hr = pBackBuffer->GetDesc(&m_d3dsdBackBuffer);
			pBackBuffer->Release();
			if( FAILED(hr) )
				return hr;
		}

		m_bD3DXReady = TRUE;

		return hr;
	}

	void InterpretError(HRESULT hr)
	{
		char errStr[100];
		D3DXGetErrorStringA(hr, errStr, sizeof(errStr) / sizeof(errStr[0]) );
        Con_Printf("D3D Error: %s", errStr);
		LocalDebugBreak();
	}

public:
	FakeGL(HWND hwndMain){
		m_bD3DXReady = TRUE;

		m_pD3DDev = 0;
		m_pD3D = 0;
		m_pPrimary = 0;
		m_hardwareTandL = false;

		m_glRenderStateDirty = true;

		m_glAlphaStateDirty = true;
		m_glAlphaFunc = GL_ALWAYS;
		m_glAlphaFuncRef = 0;
		m_glAlphaTest = false;

		m_glBlendStateDirty = true;
		m_glBlend = false;
		m_glBlendFuncSFactor = GL_ONE; // Not sure this is the default
		m_glBlendFuncDFactor = GL_ZERO; // Not sure this is the default

		m_glCullStateDirty = true;
		m_glCullFace = false;
		m_glCullFaceMode = GL_BACK;

		m_glDepthStateDirty = true;
		m_glDepthTest = false;
		m_glDepthMask = true;
		m_glDepthFunc = GL_ALWAYS; // not sure if this is the default

		m_glDepthRangeNear = 0; // not sure if this is the default
		m_glDepthRangeFar = 1.0; // not sure if this is the default

		m_glMatrixMode = GL_MODELVIEW; // Not sure this is the default

		m_glPolygonModeFront = GL_FILL;
		m_glPolygonModeBack = GL_FILL;

		m_glShadeModelStateDirty = true;
		m_glShadeModel = GL_SMOOTH;


		m_bViewPortDirty = true;
		m_glViewPortX = 0;
		m_glViewPortY = 0;
		m_glViewPortWidth = DIBWidth;
		m_glViewPortHeight = DIBHeight;

		m_vendor = 0;
		m_renderer = 0;
		m_extensions = 0;

		m_hintGenerateMipMaps = true;

		m_hwndMain = hwndMain;

		HRESULT hr = InitD3DX();
		if ( FAILED(hr) ) {
			InterpretError(hr);
		}

		hr = D3DXCreateMatrixStack(0, &m_modelViewMatrixStack);
		hr = D3DXCreateMatrixStack(0, &m_projectionMatrixStack);
		hr = D3DXCreateMatrixStack(0, &m_textureMatrixStack);
		m_currentMatrixStack = m_modelViewMatrixStack;
		m_modelViewMatrixStack->LoadIdentity(); // Not sure this is correct
		m_projectionMatrixStack->LoadIdentity();
		m_textureMatrixStack->LoadIdentity();
		m_modelViewMatrixStateDirty = true;
		m_projectionMatrixStateDirty = true;
		m_textureMatrixStateDirty = true;
		m_currentMatrixStateDirty = &m_modelViewMatrixStateDirty;
		m_viewMatrixStateDirty = true;

		D3DXMatrixIdentity(&m_d3dViewMatrix);

		m_needBeginScene = true;

		m_stickyAlloc = 0;
		m_stickyAllocSize = 0;

		{
			// Check for multitexture.
			D3DCAPS8 deviceCaps;
			HRESULT hr = m_pD3DDev->GetDeviceCaps(&deviceCaps);
			if ( ! FAILED(hr)) {
				// Clamp texture blend stages to 2. Some cards can do eight, but that's more
				// than we need.
				int maxStages = deviceCaps.MaxTextureBlendStages;
				if ( maxStages > 2 ){
					maxStages = 2;
				}
				m_textureState.SetMaxStages(maxStages);

				m_hardwareTandL = (deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
			}
		}

		// One-time render state initialization

		m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x00000000 );
		m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE, TRUE );
		m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
		// ? Not in DX8? m_pD3DDev->SetRenderState( D3DRS_TEXTUREPERSPECTIVE, TRUE );
		m_pD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE);
	}

	~FakeGL(){
		delete [] m_stickyAlloc;
		ReleaseD3DX();
		RELEASENULL(m_modelViewMatrixStack);
		RELEASENULL(m_projectionMatrixStack);
		RELEASENULL(m_textureMatrixStack);
	}

	void glAlphaFunc (GLenum func, GLclampf ref){
		if ( m_glAlphaFunc != func || m_glAlphaFuncRef != ref ) {
			SetRenderStateDirty();
			m_glAlphaFunc = func;
			m_glAlphaFuncRef = ref;
			m_glAlphaStateDirty = true;
		}
	}

	void glBegin (GLenum mode){
		if ( m_needBeginScene ){
			m_needBeginScene = false;
			HRESULT hr = m_pD3DDev->BeginScene();
			if ( FAILED(hr) ) {
				InterpretError(hr);
			}
		}

#if 0
		// statistics
		static int beginCount;
		static int stateChangeCount;
		static int primitivesCount;
		beginCount++;
		if ( m_glRenderStateDirty )
			stateChangeCount++;
		if ( m_glRenderStateDirty || ! m_OGLPrimitiveVertexBuffer.IsMergableMode(mode) )
			primitivesCount++;
#endif

		if ( m_glRenderStateDirty || ! m_OGLPrimitiveVertexBuffer.IsMergableMode(mode) ) {
			internalEnd();
			SetGLRenderState();
			DWORD typeDesc;
			typeDesc = D3DFVF_XYZ | D3DFVF_DIFFUSE;
			typeDesc |= (m_textureState.GetMaxStages() << D3DFVF_TEXCOUNT_SHIFT);

			if ( typeDesc != m_OGLPrimitiveVertexBuffer.GetVertexTypeDesc()) {
				m_OGLPrimitiveVertexBuffer.Initialize(m_pD3DDev, m_pD3D, m_hardwareTandL, typeDesc);
			}
			m_OGLPrimitiveVertexBuffer.Begin(mode);
		}
		else {
			m_OGLPrimitiveVertexBuffer.Append(mode);
		}
	}

	void glBindTexture(GLenum target, GLuint texture){
		if ( target != GL_TEXTURE_2D ) {
			LocalDebugBreak();
			return;
		}
		if ( m_textureState.GetCurrentTexture() != texture ) {
			SetRenderStateDirty();
			m_textureState.SetCurrentTexture(texture);
			m_textures.BindTexture(texture);
		}
	}

	inline void glMTexCoord2fSGIS(GLenum target, GLfloat s, GLfloat t){
		int textStage = target - TEXTURE0_SGIS;
		m_OGLPrimitiveVertexBuffer.SetTextureCoord(textStage, s, t);
	}
	
	void glSelectTextureSGIS(GLenum target){
		int textStage = target - TEXTURE0_SGIS;
		m_textureState.SetCurrentStage(textStage);
		m_textures.BindTexture(m_textureState.GetCurrentTexture());
		// Does not, by itself, dirty the render state
	}

	void glBlendFunc (GLenum sfactor, GLenum dfactor){
		if ( m_glBlendFuncSFactor != sfactor || m_glBlendFuncDFactor != dfactor ) {
			SetRenderStateDirty();
			m_glBlendFuncSFactor = sfactor;
			m_glBlendFuncDFactor = dfactor;
			m_glBlendStateDirty = true;
		}
	}

	void glClear (GLbitfield mask){
		HRESULT hr;
		internalEnd();
		SetGLRenderState();
		DWORD clearMask = 0;

        // for NV20 it's better to always clear everything
        mask |= GL_STENCIL_BUFFER_BIT;

		if ( mask & GL_STENCIL_BUFFER_BIT ) {
			clearMask |= D3DCLEAR_STENCIL;
		}

		if ( mask & GL_COLOR_BUFFER_BIT ) {
			clearMask |= D3DCLEAR_TARGET;
		}

		if ( mask & GL_DEPTH_BUFFER_BIT ) {
			clearMask |= D3DCLEAR_ZBUFFER;
		}
		hr = m_pD3DDev->Clear(0, NULL, clearMask,
			0, 1.0f, 0 );

		if ( FAILED(hr) ){
			InterpretError(hr);
		}
	}

	void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha){
		D3DCOLOR clearColor = D3DRGBA(Clamp(red), Clamp(green), Clamp(blue), Clamp(alpha));
	}

	inline void glColor3f (GLfloat red, GLfloat green, GLfloat blue){
		m_OGLPrimitiveVertexBuffer.SetColor(red, green, blue);
	}

	inline void glColor3ubv (const GLubyte *v){
		m_OGLPrimitiveVertexBuffer.SetColor(v[0], v[1], v[2]);
	}

	inline void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
		m_OGLPrimitiveVertexBuffer.SetColor(red, green, blue, alpha);
	}

	inline void glColor4fv (const GLfloat *v){
		m_OGLPrimitiveVertexBuffer.SetColor(v[0], v[1], v[2], v[3]);
	}

	void glCullFace (GLenum mode){
		if ( m_glCullFaceMode != mode ) {
			SetRenderStateDirty();
			m_glCullFaceMode = mode;
			m_glCullStateDirty = true;
		}
	}

	void glDepthFunc (GLenum func){
		if ( m_glDepthFunc != func ) {
			SetRenderStateDirty();
			m_glDepthFunc = func;
			m_glDepthStateDirty = true;
		}
	}

	void glDepthMask (GLboolean flag){
		if ( m_glDepthMask != (flag != 0) ) {
			SetRenderStateDirty();
			m_glDepthMask = flag != 0 ? true : false;
			m_glDepthStateDirty = true;
		}
	}

	void glDepthRange (GLclampd zNear, GLclampd zFar){
		if ( m_glDepthRangeNear != zNear || m_glDepthRangeFar != zFar ) {
			SetRenderStateDirty();
			m_glDepthRangeNear = zNear;
			m_glDepthRangeFar = zFar;
			m_bViewPortDirty = true;
		}
	}

	void glDisable (GLenum cap){
		glEnableDisableSet(cap, false);
	}

	void glDrawBuffer (GLenum /* mode */){
		// Do nothing. (Can DirectX render to the front buffer at all?)
	}

	void glEnable (GLenum cap){
		glEnableDisableSet(cap, true);
	}

	void glEnableDisableSet(GLenum cap, bool value){
		switch ( cap ) {
		case GL_ALPHA_TEST:
			if ( m_glAlphaTest != value ) {
				SetRenderStateDirty();
				m_glAlphaTest = value;
				m_glAlphaStateDirty = true;
			}
			break;
		case GL_BLEND:
			if ( m_glBlend != value ) {
				SetRenderStateDirty();
				m_textureState.SetMainBlend(value);
				m_glBlend = value;
				m_glBlendStateDirty = true;
			}
			break;
		case GL_CULL_FACE:
			if ( m_glCullFace != value ) {
				SetRenderStateDirty();
				m_glCullFace = value;
				m_glCullStateDirty = true;
			}
			break;
		case GL_DEPTH_TEST:
			if ( m_glDepthTest != value ) {
				SetRenderStateDirty();
				m_glDepthTest = value;
				m_glDepthStateDirty = true;
			}
			break;
		case GL_TEXTURE_2D:
			if ( m_textureState.GetTexture2D() != value ) {
				SetRenderStateDirty();
				m_textureState.SetTexture2D(value);
			}
			break;
		default:
			LocalDebugBreak();
			break;
		}
	}

	void glEnd (void){
		// internalEnd();
	}

	void internalEnd(){
		m_OGLPrimitiveVertexBuffer.End();
	}

	void glFinish (void){
		// To Do: This is supposed to flush all pending commands
		internalEnd();
	}

	void glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){
		SetRenderStateDirty();
		D3DXMATRIX m;
		// Note that D3D takes top, bottom arguments in opposite order
		D3DXMatrixPerspectiveOffCenterRH(&m, left, right, bottom, top, zNear, zFar);
		m_currentMatrixStack->MultMatrixLocal(&m);
		*m_currentMatrixStateDirty = true;
	}

	void glGetFloatv (GLenum pname, GLfloat *params){
		switch(pname){
		case GL_MODELVIEW_MATRIX:
			memcpy(params,m_modelViewMatrixStack->GetTop(), sizeof(D3DMATRIX));
			break;
		default:
			LocalDebugBreak();
			break;
		}
	}

	const GLubyte * glGetString (GLenum name){
		const char* result = "";
		EnsureDriverInfo();
		switch ( name ) {
		case GL_VENDOR:
			result = m_vendor;
			break;
		case GL_RENDERER:
			result = m_renderer;
			break;
		case GL_VERSION:
			result = m_version;
			break;
		case GL_EXTENSIONS:
			result = m_extensions;
			break;
		default:
			break;
		}
		return (const GLubyte *) result;
	}

	void glHint (GLenum /* target */, GLenum /* mode */){
		LocalDebugBreak();
	}

	void glLoadIdentity (void){
		SetRenderStateDirty();
		m_currentMatrixStack->LoadIdentity();
		*m_currentMatrixStateDirty = true;
	}

	void glLoadMatrixf (const GLfloat *m){
		SetRenderStateDirty();
		m_currentMatrixStack->LoadMatrix((D3DXMATRIX*) m);
		*m_currentMatrixStateDirty = true;
	}

	void glMatrixMode (GLenum mode){
		m_glMatrixMode = mode;
		switch ( mode ) {
		case GL_MODELVIEW:
			m_currentMatrixStack = m_modelViewMatrixStack;
			m_currentMatrixStateDirty = &m_modelViewMatrixStateDirty;
			break;
		case GL_PROJECTION:
			m_currentMatrixStack = m_projectionMatrixStack;
			m_currentMatrixStateDirty = &m_projectionMatrixStateDirty;
			break;
		case GL_TEXTURE:
			m_currentMatrixStack = m_textureMatrixStack;
			m_currentMatrixStateDirty = &m_textureMatrixStateDirty;
			break;
		default:
			LocalDebugBreak();
			break;
		}
	}

	void glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){
		SetRenderStateDirty();
		D3DXMATRIX m;
		D3DXMatrixOrthoOffCenterRH(&m, left, right, top, bottom, zNear, zFar);
		m_currentMatrixStack->MultMatrixLocal(&m);
		*m_currentMatrixStateDirty = true;
	}

	void glPolygonMode (GLenum face, GLenum mode){
		SetRenderStateDirty();
		switch ( face ) {
		case GL_FRONT:
			m_glPolygonModeFront = mode;
			break;
		case GL_BACK:
			m_glPolygonModeBack = mode;
			break;
		case GL_FRONT_AND_BACK:
			m_glPolygonModeFront = mode;
			m_glPolygonModeBack = mode;
			break;
		default:
			LocalDebugBreak();
			break;
		}
	}

	void glPopMatrix (void){
		SetRenderStateDirty();
		m_currentMatrixStack->Pop();
		*m_currentMatrixStateDirty = true;
	}

	void glPushMatrix (void){
		m_currentMatrixStack->Push();
		// Doesn't dirty matrix state
	}

	void glReadBuffer (GLenum /* mode */){
		// Not that we allow reading from various buffers anyway.
	}

	void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels){
		if ( format != GL_RGB || type != GL_UNSIGNED_BYTE) {
			LocalDebugBreak();
			return;
		}
		internalEnd();

		// Temporarily disable, because I don't want to port DDSURFACEDESC2 to DX8
	}

	static WORD GetNumberOfBits( DWORD dwMask )
	{
		WORD wBits = 0;
		while( dwMask )
		{
			dwMask = dwMask & ( dwMask - 1 );
			wBits++;
		}
		return wBits;
	}

	static WORD GetShift( DWORD dwMask )
	{
		for(WORD i = 0; i < 32; i++ ) {
			if ( (1 << i) & dwMask ) {
				return i;
			}
		}
		return 0; // no bits in mask.
	}

	// Extract the bits and replicate out to an eight bit value
	static DWORD ExtractAndNormalize(DWORD rgba, DWORD shift, DWORD bits, DWORD mask){
		DWORD v = (rgba & mask) >> shift;
		// Assume bits >= 4
		v = (v | (v << bits));
		v = v >> (bits*2 - 8);
		return v;
	}

#if 0 // Temporarily disable
	void CopyBitsToRGB(void* pixels, DWORD sx, DWORD sy, DWORD width, DWORD height, LPDDSURFACEDESC2 pDesc){
		if ( ! (pDesc->ddpfPixelFormat.dwFlags & DDPF_RGB) ) {
			return; // Can't handle non-RGB surfaces
		}
		// We have to flip the Y axis to convert from D3D to openGL
		long destEndOfLineSkip = -2 * (width * 3);
		unsigned char* pDest = ((unsigned char*) pixels) + (height - 1) * width * 3 ;
		switch ( pDesc->ddpfPixelFormat.dwRGBBitCount ) {
		default:
			return;
		case 16:
			{
				unsigned short* pSource = (unsigned short*)
					(((unsigned char*) pDesc->lpSurface) + sx * sizeof(unsigned short) + sy * pDesc->lPitch);
				DWORD endOfLineSkip = pDesc->lPitch / sizeof(unsigned short) - pDesc->dwWidth;
				DWORD rMask = pDesc->ddpfPixelFormat.dwRBitMask;
				DWORD gMask = pDesc->ddpfPixelFormat.dwGBitMask;
				DWORD bMask = pDesc->ddpfPixelFormat.dwBBitMask;
				DWORD rShift = GetShift(rMask);
				DWORD rBits = GetNumberOfBits(rMask);
				DWORD gShift = GetShift(gMask);
				DWORD gBits = GetNumberOfBits(gMask);
				DWORD bShift = GetShift(bMask);
				DWORD bBits = GetNumberOfBits(bMask);
				for(DWORD y = 0; y < height; y++ ) {
					for (DWORD x = 0; x < width; x++ ) {
						unsigned short rgba = *pSource++;
						*pDest++ = ExtractAndNormalize(rgba, rShift, rBits, rMask);
						*pDest++ = ExtractAndNormalize(rgba, gShift, gBits, gMask);
						*pDest++ = ExtractAndNormalize(rgba, bShift, bBits, bMask);
					}
					pSource += endOfLineSkip;
					pDest += destEndOfLineSkip;
				}
			}
			break;
		case 32:
			{
				unsigned long* pSource = (unsigned long*)
					(((unsigned char*) pDesc->lpSurface) + sx * sizeof(unsigned long) + sy * pDesc->lPitch);
				DWORD endOfLineSkip = pDesc->lPitch / sizeof(unsigned long) - pDesc->dwWidth;
				for(DWORD y = 0; y < height; y++ ) {
					for (DWORD x = 0; x < width; x++ ) {
						unsigned long rgba = *pSource++;
						*pDest++ = RGBA_GETRED(rgba);
						*pDest++ = RGBA_GETGREEN(rgba);
						*pDest++ = RGBA_GETBLUE(rgba);
					}
					pSource += endOfLineSkip;
					pDest += destEndOfLineSkip;
				}
			}
			break;
		}
	}

#endif // Temporarily disable

	void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
		SetRenderStateDirty();
		D3DXMATRIX m;
		D3DXVECTOR3 v;
		v.x = x;
		v.y = y;
		v.z = z;
		// GL uses counterclockwise degrees, DX uses clockwise radians
		float dxAngle = angle * 3.14159 / 180;
		m_currentMatrixStack->RotateAxisLocal(&v, dxAngle);
		*m_currentMatrixStateDirty = true;
	}

	void glScalef (GLfloat x, GLfloat y, GLfloat z){
		SetRenderStateDirty();
		D3DXMATRIX m;
		D3DXMatrixScaling(&m, x, y, z);
		m_currentMatrixStack->MultMatrixLocal(&m);
		*m_currentMatrixStateDirty = true;
	}

	void glShadeModel (GLenum mode){
		if ( m_glShadeModel != mode ) {
			SetRenderStateDirty();
			m_glShadeModel = mode;
			m_glShadeModelStateDirty = true;
		}
	}

	inline void glTexCoord2f (GLfloat s, GLfloat t){
		m_OGLPrimitiveVertexBuffer.SetTextureCoord0(s, t);
	}

	void glTexEnvf (GLenum /* target */, GLenum /* pname */, GLfloat param){
		// ignore target, which must be GL_TEXTURE_ENV
		// ignore pname, which must be GL_TEXTURE_ENV_MODE
		if ( m_textureState.GetTextEnvMode() != param ) {
			SetRenderStateDirty();
			m_textureState.SetTextEnvMode(param);
		}
	}

	static int MipMapSize(DWORD width, DWORD height){
		DWORD n = width < height? width : height;
		DWORD result = 1;
		while (n > (DWORD) (1 << result) ) {
			result++;
		}
		return result;
	}

#define LOAD_OURSELVES

	void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width,
		GLsizei height, GLint /* border */, GLenum format, GLenum type, const GLvoid *pixels){
		HRESULT hr;
		if ( target != GL_TEXTURE_2D || type != GL_UNSIGNED_BYTE) {
			InterpretError(E_FAIL);
			return;
		}

		bool isDynamic = format == GL_LUMINANCE; // Lightmaps use this format.

		DWORD dxWidth = width;
		DWORD dxHeight = height;

		D3DFORMAT srcPixelFormat = GLToDXPixelFormat(internalformat, format);
		D3DFORMAT destPixelFormat = srcPixelFormat;
		// Can the surface handle that format?
		hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3dsdBackBuffer.Format,
			0, D3DRTYPE_TEXTURE, destPixelFormat);
		if ( FAILED(hr) ) {
			if ( g_force16bitTextures ) {
				destPixelFormat = D3DFMT_A4R4G4B4;
				hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3dsdBackBuffer.Format,
					0, D3DRTYPE_TEXTURE, destPixelFormat);
				if ( FAILED(hr) ) {
					// Don't know what to do.
					InterpretError(E_FAIL);
					return;
				}
			}
			else {
				destPixelFormat = D3DFMT_A8R8G8B8;
				hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3dsdBackBuffer.Format,
					0, D3DRTYPE_TEXTURE, destPixelFormat);
				if ( FAILED(hr) ) {
					// The card can't handle this pixel format. Switch to D3DX_SF_A4R4G4B4
					destPixelFormat = D3DFMT_A4R4G4B4;
					hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3dsdBackBuffer.Format,
						0, D3DRTYPE_TEXTURE, destPixelFormat);
					if ( FAILED(hr) ) {
						// Don't know what to do.
						InterpretError(E_FAIL);
						return;
					}
				}
			}
		}

#ifdef LOAD_OURSELVES

		char* goodSizeBits = (char*) pixels;
		if ( dxWidth != (DWORD) width || dxHeight != (DWORD) height ) {
			// Most likely this is because there is a 256 x 256 limit on the texture size.
			goodSizeBits = new char[sizeof(DWORD) * dxWidth * dxHeight];
			DWORD* dest = ((DWORD*) goodSizeBits);
			for ( DWORD y = 0; y < dxHeight; y++) {
				DWORD sy = y * height / dxHeight;
				for(DWORD x = 0; x < dxWidth; x++) {
					DWORD sx = x * width / dxWidth;
					DWORD* source = ((DWORD*) pixels) + sy * dxWidth + sx;
					*dest++ = *source;
				}
			}
			width = dxWidth;
			height = dxHeight;
		}
		// To do: Convert the pixels on the fly while copying into the DX texture.
		char* compatablePixels;
		DWORD compatablePixelsPitch;

		hr = ConvertToCompatablePixels(internalformat, width, height, format,
				type, destPixelFormat, goodSizeBits, &compatablePixels, &compatablePixelsPitch);

		if ( goodSizeBits != pixels ) {
			delete [] goodSizeBits;
		}
		if ( FAILED(hr)) {
			InterpretError(hr);
			return;
		}

#endif

		IDirect3DTexture8* pMipMap = m_textures.GetMipMap();
		if ( pMipMap ) {
			// DX8 textures don't know much. Always reset texture for level zero.
			if ( level == 0 ) {
				m_textures.SetTexture(NULL, D3DFMT_UNKNOWN, 0);
				pMipMap = 0;
			}
			// For non-square textures, OpenGL uses more MIPMAP levels than DirectX does.
			else if ( level >= (GLint)pMipMap->GetLevelCount() ) {
				return;
			}

		}

		if( ! pMipMap) {
			int levels = 1;
			if ( m_hintGenerateMipMaps ) {
				levels = MipMapSize(width, height);
			}

			hr = m_pD3DDev->CreateTexture(width, height, levels,
				0, destPixelFormat, D3DPOOL_MANAGED,
				&pMipMap);
			if ( FAILED(hr) ) {
				InterpretError(hr);
				return;
			}

			m_textures.SetTexture(pMipMap, destPixelFormat, internalformat);
		}

		glTexSubImage2D_Imp(pMipMap, level, 0, 0, width, height, format, type, compatablePixels,
			compatablePixelsPitch);

  		if ( FAILED(hr) ) {
			InterpretError(hr);
			return;
		}
	}

	void glTexParameterf (GLenum target, GLenum pname, GLfloat param){

		switch(target){
		case GL_TEXTURE_2D:
			{
				SetRenderStateDirty();
				TextureEntry* current = m_textures.GetCurrentEntry();
				m_textureState.DirtyTexture(m_textures.GetCurrentID());
				switch(pname) {
				case GL_TEXTURE_MIN_FILTER:
					current->m_glTexParameter2DMinFilter = param;
					break;
				case GL_TEXTURE_MAG_FILTER:
					current->m_glTexParameter2DMagFilter = param;
					break;
				case GL_TEXTURE_WRAP_S:
					current->m_glTexParameter2DWrapS = param;
					break;
				case GL_TEXTURE_WRAP_T:
					current->m_glTexParameter2DWrapT = param;
					break;
				case D3D_TEXTURE_MAXANISOTROPY:
					current->m_maxAnisotropy = param;
					break;
				default:
					LocalDebugBreak();
				}
			}
			break;
		default:
			LocalDebugBreak();
			break;
		}
	}

	void glTexSubImage2D (GLenum target, GLint level,
		GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
		GLenum format, GLenum type, const GLvoid *pixels){
		if ( target != GL_TEXTURE_2D ) {
			LocalDebugBreak();
			return;
		}
		if ( width <= 0 || height <= 0 ) {
			return;
		}

		IDirect3DTexture8* pTexture = m_textures.GetMipMap();
		if ( ! pTexture ) {
			return;
		}

		internalEnd(); // We may have a pending drawing using the old texture state.

		// To do: Convert the pixels on the fly while copying into the DX texture.

		char* compatablePixels = 0;
		DWORD compatablePixelsPitch;
		if ( FAILED(ConvertToCompatablePixels(m_textures.GetInternalFormat(),
				width, height,
				format, type, m_textures.GetSurfaceFormat(),
				pixels, &compatablePixels, &compatablePixelsPitch))) {
			LocalDebugBreak();
			return;
		}

		glTexSubImage2D_Imp(pTexture, level, xoffset, yoffset, width, height, format, type,
			compatablePixels, compatablePixelsPitch);
	}

	char* StickyAlloc(DWORD size){
		if ( m_stickyAllocSize < size ) {
			delete [] m_stickyAlloc;
			m_stickyAlloc = new char[size];
			m_stickyAllocSize = size;
		}
		return m_stickyAlloc;
	}

	void glTexSubImage2D_Imp (IDirect3DTexture8* pMipMap, GLint level,
		GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
		GLenum /* format */, GLenum /* type */, const char* compatablePixels, int compatablePixelsPitch){

		HRESULT hr = S_OK;

		D3DLOCKED_RECT lockRect;
        D3DSURFACE_DESC desc;
        RECT rect = { 0, 0, width, height };
        POINT point = { xoffset, yoffset };

        pMipMap->GetLevelDesc(level, &desc);
        pMipMap->LockRect(level, &lockRect, NULL, 0);

        XGSwizzleRect((void *)compatablePixels,
                      compatablePixelsPitch,
                      &rect,
                      lockRect.pBits,
                      desc.Width,
                      desc.Height,
                      &point,
                      lockRect.Pitch / desc.Width);

        pMipMap->UnlockRect(level);

#if 0 // Non-KELVIN code

		// Get MipMap level

		IDirect3DSurface8* lpDDLevel = 0;

		hr = pMipMap->GetSurfaceLevel(level, &lpDDLevel);
		
		if ( FAILED(hr) ) {
			InterpretError(hr);
			return;
		}

		D3DLOCKED_RECT lockedRect;
		RECT lockRect;
		lockRect.top = yoffset;
		lockRect.left = xoffset;
		lockRect.bottom = yoffset + height;
		lockRect.right = xoffset + width;

		// hr = lpDDLevel->LockRect(&lockedRect, &lockRect, D3DLOCK_DISCARD);
		hr = lpDDLevel->LockRect(&lockedRect, NULL, 0);
		if ( FAILED(hr) ) {
			InterpretError(hr);
		}
		else {
			const char* sp = compatablePixels;
			char* dp = (char*) lockedRect.pBits + yoffset * lockedRect.Pitch;
			if ( compatablePixelsPitch > lockedRect.Pitch ) {
				LocalDebugBreak();
			}
			if ( compatablePixelsPitch != lockedRect.Pitch ) {
				for(int i = 0; i < height; i++ ) {
					memcpy(dp, sp, compatablePixelsPitch);
					sp += compatablePixelsPitch;
					dp += lockedRect.Pitch;
				}
			}
			else {
				memcpy(dp, sp, compatablePixelsPitch * height);
			}
			lpDDLevel->UnlockRect();
		}

		RELEASENULL(lpDDLevel);

		if ( FAILED(hr) ) {
			InterpretError(hr);
		}

#endif // 0

	}

	void glTranslatef (GLfloat x, GLfloat y, GLfloat z){
		SetRenderStateDirty();
		D3DXMATRIX m;
		D3DXMatrixTranslation(&m, x, y, z);
		m_currentMatrixStack->MultMatrixLocal(&m);
		*m_currentMatrixStateDirty = true;
	}

	inline void glVertex2f (GLfloat x, GLfloat y){
		m_OGLPrimitiveVertexBuffer.SetVertex(x, y, 0);
	}

	inline void glVertex3f (GLfloat x, GLfloat y, GLfloat z){
		m_OGLPrimitiveVertexBuffer.SetVertex(x, y, z);
	}

	inline void glVertex3fv (const GLfloat *v){
		m_OGLPrimitiveVertexBuffer.SetVertex(v[0], v[1], v[2]);
	}

	void glViewport (GLint x, GLint y, GLsizei width, GLsizei height){
		if ( m_glViewPortX != x || m_glViewPortY != y ||
			m_glViewPortWidth != width || m_glViewPortHeight != height ) {
			SetRenderStateDirty();
			m_glViewPortX = x;
			m_glViewPortY = y;
			m_glViewPortWidth = width;
			m_glViewPortHeight = height;

			m_bViewPortDirty = true;
		}
	}

	void SwapBuffers(){
		HRESULT hr = S_OK;
		internalEnd();
		m_pD3DDev->EndScene();
		m_needBeginScene = true;
//		static int frameCounter;
//		frameCounter++;
//		char buf[100];
//		sprintf(buf, "Present %d\n", frameCounter);
//		OutputDebugString(buf);
        hr = m_pD3DDev->Present(NULL, NULL, NULL, NULL);
		if ( FAILED(hr) ){
			LocalDebugBreak();
		}
		// if ( frameCounter == 3 ) {
		//	Sleep(1700);
		//	LocalDebugBreak();
		// }
	}

	void SetGammaRamp(const unsigned char* gammaTable){
		D3DGAMMARAMP gammaRamp;
		for(int i = 0; i < 256; i++ ) {
			WORD value = gammaTable[i];
			value = value + (value << 8); // * 257
			gammaRamp.red[i] = value;
			gammaRamp.green[i] = value;
			gammaRamp.blue[i] = value;
		}

		m_pD3DDev->SetGammaRamp(D3DSGR_CALIBRATE, &gammaRamp);
	}

	void Hint_GenerateMipMaps(int value){
		m_hintGenerateMipMaps = value != 0;
	}

	void EvictTextures(){
	}
private:

	void SetRenderStateDirty(){
		if ( ! m_glRenderStateDirty ) {
			internalEnd();
			m_glRenderStateDirty = true;
		}
	}

	HRESULT HandleWindowedModeChanges()
	{
		return S_OK;
	}

	void SetGLRenderState(){
		if ( ! m_glRenderStateDirty ) {
			return;
		}
		m_glRenderStateDirty = false;
		HRESULT hr;
		if ( m_glAlphaStateDirty ){
			m_glAlphaStateDirty = false;
			// Alpha test
			m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE,
				m_glAlphaTest ? TRUE : FALSE );
			m_pD3DDev->SetRenderState(D3DRS_ALPHAFUNC,
				m_glAlphaTest ? GLToDXCompare(m_glAlphaFunc) : D3DCMP_ALWAYS);
			m_pD3DDev->SetRenderState(D3DRS_ALPHAREF, 255 * m_glAlphaFuncRef);
		}
		if ( m_glBlendStateDirty ){
			m_glBlendStateDirty = false;
			// Alpha blending
			DWORD srcBlend = m_glBlend ? GLToDXSBlend(m_glBlendFuncSFactor) : D3DBLEND_ONE;
			DWORD destBlend = m_glBlend ? GLToDXDBlend(m_glBlendFuncDFactor) : D3DBLEND_ZERO;
			m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  srcBlend );
			m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, destBlend );
			m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, m_glBlend ? TRUE : FALSE );
		}
		if ( m_glCullStateDirty ) {
			m_glCullStateDirty = false;
			D3DCULL cull = D3DCULL_NONE;
			if ( m_glCullFace ) {
				switch(m_glCullFaceMode){
				default:
				case GL_BACK:
					// Should deal with frontface function
					cull = D3DCULL_CCW;
					break;
				}
			}
			hr = m_pD3DDev->SetRenderState(D3DRS_CULLMODE, cull);
			if ( FAILED(hr) ){
				InterpretError(hr);
			}
		}
		if ( m_glShadeModelStateDirty ){
			m_glShadeModelStateDirty = false;
			// Shade model
			m_pD3DDev->SetRenderState( D3DRS_SHADEMODE,
				m_glShadeModel == GL_SMOOTH ? D3DSHADE_GOURAUD : D3DSHADE_FLAT );
		}

		{
			m_textureState.SetTextureStageState(m_pD3DDev, &m_textures);
		}

		if ( m_glDepthStateDirty ) {
			m_glDepthStateDirty = false;
			m_pD3DDev->SetRenderState( D3DRS_ZENABLE, m_glDepthTest ? D3DZB_TRUE : D3DZB_FALSE);
			m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, m_glDepthMask ? TRUE : FALSE);
			DWORD zfunc = GLToDXCompare(m_glDepthFunc);
			m_pD3DDev->SetRenderState( D3DRS_ZFUNC, zfunc );
		}
		if ( m_modelViewMatrixStateDirty ) {
			m_modelViewMatrixStateDirty = false;
			m_pD3DDev->SetTransform( D3DTS_WORLD, m_modelViewMatrixStack->GetTop() );
		}
		if ( m_viewMatrixStateDirty ) {
			m_viewMatrixStateDirty = false;
			m_pD3DDev->SetTransform( D3DTS_VIEW, & m_d3dViewMatrix );
		}
		if ( m_projectionMatrixStateDirty ) {
			m_projectionMatrixStateDirty = false;
			m_pD3DDev->SetTransform( D3DTS_PROJECTION, m_projectionMatrixStack->GetTop() );
		}
		if ( m_textureMatrixStateDirty ) {
			m_textureMatrixStateDirty = false;
			m_pD3DDev->SetTransform( D3DTS_TEXTURE0, m_textureMatrixStack->GetTop() );
		}
		if ( m_bViewPortDirty ) {
			m_bViewPortDirty = false;
			D3DVIEWPORT8 viewData;
			viewData.X = m_glViewPortX;
			viewData.Y = gHeight - (m_glViewPortY + m_glViewPortHeight);
			viewData.Width  = m_glViewPortWidth;
			viewData.Height = m_glViewPortHeight;
			viewData.MinZ = m_glDepthRangeNear;
			viewData.MaxZ = m_glDepthRangeFar;
			m_pD3DDev->SetViewport(&viewData);
		}
	}

	void EnsureDriverInfo() {
		if ( ! m_vendor ) {
			m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &m_dddi);
			m_vendor = m_dddi.Driver;
			m_renderer = m_dddi.Description;
			sprintf(m_version, "%u.%u.%u.%u %u.%u.%u.%u %u",
				HIWORD(m_dddi.DriverVersion.HighPart),
				LOWORD(m_dddi.DriverVersion.HighPart),
				HIWORD(m_dddi.DriverVersion.LowPart),
				LOWORD(m_dddi.DriverVersion.LowPart),
				m_dddi.VendorId,
				m_dddi.DeviceId,
				m_dddi.SubSysId,
				m_dddi.Revision,
				m_dddi.WHQLLevel
				);
			if ( m_textureState.GetMaxStages() > 1 ) {
				m_extensions = " GL_SGIS_multitexture GL_EXT_texture_object ";
			}
			else {
				m_extensions = " GL_EXT_texture_object ";
			}
		}
	}

	D3DFORMAT GLToDXPixelFormat(GLint internalformat, GLenum format){
		D3DFORMAT d3dFormat = D3DFMT_UNKNOWN;
		if ( g_force16bitTextures ) {
			switch ( format ) {
			case GL_RGBA:
				switch ( internalformat ) {
				default:
				case 4:
//					d3dFormat = D3DFMT_A1R5G5B5; break;
					d3dFormat = D3DFMT_A4R4G4B4; break;
				case 3:
					d3dFormat = D3DFMT_R5G6B5; break;
				}
				break;
#ifdef _XBOX
			case GL_COLOR_INDEX: d3dFormat = D3DFMT_A4R4G4B4; break;
			case GL_LUMINANCE: d3dFormat = D3DFMT_A4R4G4B4; break;
			case GL_ALPHA: d3dFormat = D3DFMT_A4R4G4B4; break;
			case GL_INTENSITY: d3dFormat = D3DFMT_A4R4G4B4; break;
#else
			case GL_COLOR_INDEX: d3dFormat = D3DFMT_P8; break;
			case GL_LUMINANCE: d3dFormat = D3DFMT_L8; break;
			case GL_ALPHA: d3dFormat = D3DFMT_A8; break;
			case GL_INTENSITY: d3dFormat = D3DFMT_L8; break;
#endif
			case GL_RGBA4: d3dFormat = D3DFMT_A4R4G4B4; break;
			default:
				InterpretError(E_FAIL);
			}
		}
		else {
			// for
			switch ( format ) {
			case GL_RGBA:
				switch ( internalformat ) {
				default:
				case 4:
					d3dFormat = D3DFMT_A8R8G8B8; break;
				case 3:
					d3dFormat = D3DFMT_X8R8G8B8; break;
				}
				break;
#ifdef _XBOX
			case GL_COLOR_INDEX: d3dFormat = D3DFMT_A4R4G4B4; break;
			case GL_LUMINANCE: d3dFormat = D3DFMT_A4R4G4B4; break;
			case GL_ALPHA: d3dFormat = D3DFMT_A4R4G4B4; break;
			case GL_INTENSITY: d3dFormat = D3DFMT_A4R4G4B4; break;
#else
			case GL_COLOR_INDEX: d3dFormat = D3DFMT_P8; break;
			case GL_LUMINANCE: d3dFormat = D3DFMT_L8; break;
			case GL_ALPHA: d3dFormat = D3DFMT_A8; break;
			case GL_INTENSITY: d3dFormat = D3DFMT_L8; break;
#endif
			case GL_RGBA4: d3dFormat = D3DFMT_A4R4G4B4; break;
			default:
				InterpretError(E_FAIL);
			}
		}
		// Only support D
		return d3dFormat;
	}

// Avoid warning 4061, enumerant 'foo' in switch of enum 'bar' is not explicitly handled by a case label.
#pragma warning( push )
#pragma warning( disable : 4061)

	HRESULT ConvertToCompatablePixels(GLint internalformat,
		GLsizei width, GLsizei height,
		GLenum /* format */, GLenum type,
		D3DFORMAT dxPixelFormat,
		const GLvoid *pixels, char**  compatablePixels,
		DWORD* newPitch){
		HRESULT hr = S_OK;
		if ( type != GL_UNSIGNED_BYTE ) {
			return E_FAIL;
		}
		switch ( dxPixelFormat ) {
		default:
			LocalDebugBreak();
			break;
		case D3DFMT_P8:
		case D3DFMT_L8:
		case D3DFMT_A8:
			{
				char* copy = StickyAlloc(width*height);
				memcpy(copy,pixels,width * height);
				*compatablePixels = copy;
				if ( newPitch ) {
					*newPitch = width;
				}
			}
			break;
		case D3DFMT_A4R4G4B4:
			{
				int textureElementSize = 2;
				const unsigned char* glpixels = (const unsigned char*) pixels;
				char* dxpixels = StickyAlloc(textureElementSize * width * height);
				switch ( internalformat ) {
				default:
					LocalDebugBreak();
					break;
				case 1:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const unsigned char* sp = glpixels + (y*width+x);
								unsigned short v;
								unsigned short s = 0xf & (sp[0] >> 4);
								v = s; // blue
								v |= s << 4; // green
								v |= s << 8; // red
								v |= s << 12; // alpha
								*dp = v;
							}
						}
					}
					break;
				case 3:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const unsigned char* sp = glpixels + (y*width+x)*4;
								unsigned short v;
								v = (0xf & (sp[2] >> 4)); // blue
								v |= (0xf & (sp[1] >> 4)) << 4; // green
								v |= (0xf & (sp[0] >> 4)) << 8; // red
								v |= 0xf000; // alpha
								*dp = v;
							}
						}
					}
					break;
				case 4:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*)(dxpixels + (y*width+x)*textureElementSize);
								const unsigned char* sp = glpixels + (y*width+x)*4;
								unsigned short v;
								v = (0xf & (sp[2] >> 4)); // blue
								v |= (0xf & (sp[1] >> 4)) << 4; // green
								v |= (0xf & (sp[0] >> 4)) << 8; // red
								v |= (0xf & (sp[3] >> 4)) << 12; // alpha
								*dp = v;
							}
						}
					}
					break;
				}
				*compatablePixels = dxpixels;
				if ( newPitch ) {
					*newPitch = 2 * width;
				}
			}
			break;
		case D3DFMT_R5G6B5:
			{
				int textureElementSize = 2;
				const char* glpixels = (const char*) pixels;
				char* dxpixels = StickyAlloc(textureElementSize * width * height);
				switch ( internalformat ) {
				default:
					LocalDebugBreak();
					break;
				case 1:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const char* sp = glpixels + (y*width+x);
								unsigned short v;
								v = (0x1f & (sp[0] >> 3)); // blue
								v |= (0x3f & (sp[0] >> 2)) << 5; // green
								v |= (0x1f & (sp[0] >> 3)) << 11; // red
								*dp = v;
							}
						}
					}
					break;
				case 3:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const char* sp = glpixels + (y*width+x)*4;
								unsigned short v;
								v = (0x1f & (sp[2] >> 3)); // blue
								v |= (0x3f & (sp[1] >> 2)) << 5; // green
								v |= (0x1f & (sp[0] >> 3)) << 11; // red
								*dp = v;
							}
						}
					}
					break;
				case 4:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const char* sp = glpixels + (y*width+x)*4;
								unsigned short v;
								v = (0x1f & (sp[2] >> 3)); // blue
								v |= (0x3f & (sp[1] >> 2)) << 5; // green
								v |= (0x1f & (sp[0] >> 3)) << 11; // red
								*dp = v;
							}
						}
					}
					break;
				}
				*compatablePixels = dxpixels;
				if ( newPitch ) {
					*newPitch = 2 * width;
				}
			}
			break;
		case D3DFMT_X1R5G5B5:
			{
				int textureElementSize = 2;
				const char* glpixels = (const char*) pixels;
				char* dxpixels = StickyAlloc(textureElementSize * width * height);
				switch ( internalformat ) {
				default:
					LocalDebugBreak();
					break;
				case 1:
					{
#define RGBTOR5G5B5(R, G, B) (0x8000 |  (0x1f & ((B) >> 3)) | ((0x1f & ((G) >> 3)) << 5) | ((0x1f & ((R) >> 3)) << 10))
#define Y5TOR5G5B5(Y) (0x8000 | ((Y) << 10) | ((Y) << 5) | (Y))
						static const unsigned short table[32] = {
							Y5TOR5G5B5(0), Y5TOR5G5B5(1), Y5TOR5G5B5(2), Y5TOR5G5B5(3),
							Y5TOR5G5B5(4), Y5TOR5G5B5(5), Y5TOR5G5B5(6), Y5TOR5G5B5(7),
							Y5TOR5G5B5(8), Y5TOR5G5B5(9), Y5TOR5G5B5(10), Y5TOR5G5B5(11),
							Y5TOR5G5B5(12), Y5TOR5G5B5(13), Y5TOR5G5B5(14), Y5TOR5G5B5(15),
							Y5TOR5G5B5(16), Y5TOR5G5B5(17), Y5TOR5G5B5(18), Y5TOR5G5B5(19),
							Y5TOR5G5B5(20), Y5TOR5G5B5(21), Y5TOR5G5B5(22), Y5TOR5G5B5(23),
							Y5TOR5G5B5(24), Y5TOR5G5B5(25), Y5TOR5G5B5(26), Y5TOR5G5B5(27),
							Y5TOR5G5B5(28), Y5TOR5G5B5(29), Y5TOR5G5B5(30), Y5TOR5G5B5(31)
						};
						unsigned short* dp = (unsigned short*) dxpixels;
						const unsigned char* sp = (const unsigned char*) glpixels;
						int numPixels = height * width;
						int i = numPixels >> 2;
						while(i > 0) {
							*dp++ = table[(*sp++) >> 3];
							*dp++ = table[(*sp++) >> 3];
							*dp++ = table[(*sp++) >> 3];
							*dp++ = table[(*sp++) >> 3];
							--i;
						}

						i = numPixels & 3;
						while(i > 0) {
							*dp++ = table[(*sp++) >> 3];
							--i;
						}
					}
					break;
				case 3:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const unsigned char* sp = (const unsigned char*) glpixels + (y*width+x)*4;
								unsigned short v;
								v = (sp[2] >> 3); // blue
								v |= (sp[1] >> 3) << 5; // green
								v |= (sp[0] >> 3) << 10; // red
								v |= 0x8000; // alpha
								*dp = v;
							}
						}
					}
					break;
				case 4:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const unsigned char* sp = (const unsigned char*) glpixels + (y*width+x)*4;
								unsigned short v;
								v = (sp[2] >> 3); // blue
								v |= (sp[1] >> 3) << 5; // green
								v |= (sp[0] >> 3) << 10; // red
								v |= 0x8000; // alpha
								*dp = v;
							}
						}
					}
					break;
				}
				*compatablePixels = dxpixels;
				if ( newPitch ) {
					*newPitch = 2 * width;
				}
			}
			break;
		case D3DFMT_A1R5G5B5:
			{
				int textureElementSize = 2;
				const char* glpixels = (const char*) pixels;
				char* dxpixels = StickyAlloc(textureElementSize * width * height);
				switch ( internalformat ) {
				default:
					LocalDebugBreak();
					break;
				case 1:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const char* sp = glpixels + (y*width+x);
								unsigned short v;
								v = (0x1f & (sp[0] >> 3)); // blue
								v |= (0x1f & (sp[0] >> 3)) << 5; // green
								v |= (0x1f & (sp[0] >> 3)) << 10; // red
								v |= (0x01 & (sp[0] >> 7)) << 15; // alpha
								*dp = v;
							}
						}
					}
					break;
				case 3:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const char* sp = glpixels + (y*width+x)*4;
								unsigned short v;
								v = (0x1f & (sp[2] >> 3)); // blue
								v |= (0x1f & (sp[1] >> 3)) << 5; // green
								v |= (0x1f & (sp[0] >> 3)) << 10; // red
								v |= 0x8000; // alpha
								*dp = v;
							}
						}
					}
					break;
				case 4:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned short* dp = (unsigned short*) (dxpixels + (y*width+x)*textureElementSize);
								const char* sp = glpixels + (y*width+x)*4;
								unsigned short v;
								v = (0x1f & (sp[2] >> 3)); // blue
								v |= (0x1f & (sp[1] >> 3)) << 5; // green
								v |= (0x1f & (sp[0] >> 3)) << 10; // red
								v |= (0x01 & (sp[3] >> 7)) << 15; // alpha
								*dp = v;
							}
						}
					}
					break;
				}
				*compatablePixels = dxpixels;
				if ( newPitch ) {
					*newPitch = 2 * width;
				}
			}
			break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
			{
				int textureElementSize = 4;
				const char* glpixels = (const char*) pixels;
				char* dxpixels = StickyAlloc(textureElementSize * width * height);
				switch ( internalformat ) {
				default:
					LocalDebugBreak();
					break;
				case 1:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								char* dp = dxpixels + (y*width+x)*textureElementSize;
								const char* sp = glpixels + (y*width+x);
								dp[0] = sp[0]; // blue
								dp[1] = sp[0]; // green
								dp[2] = sp[0]; // red
								dp[3] = sp[0];
							}
						}
					}
					break;
				case 3:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								unsigned char* dp = (unsigned char*) dxpixels + (y*width+x)*textureElementSize;
								const unsigned char* sp = (unsigned char*) glpixels + (y*width+x)*4;
								dp[0] = sp[2]; // blue
								dp[1] = sp[1]; // green
								dp[2] = sp[0]; // red
								dp[3] = 0xff;
							}
						}
					}
					break;
				case 4:
					{
						for(int y = 0; y < height; y++){
							for(int x = 0; x < width; x++){
								char* dp = dxpixels + (y*width+x)*textureElementSize;
								const char* sp = glpixels + (y*width+x)*4;
								dp[0] = sp[2]; // blue
								dp[1] = sp[1]; // green
								dp[2] = sp[0]; // red
								dp[3] = sp[3]; // alpha
							}
						}
					}
					break;
				}
				*compatablePixels = dxpixels;
				if ( newPitch ) {
					*newPitch = 4 * width;
				}
			}
		}

		return hr;
	}


#pragma warning( pop )

};

// TODO Fix this warning instead of disableing it
#pragma warning(disable:4273)

void APIENTRY glAlphaFunc (GLenum func, GLclampf ref){
	gFakeGL->glAlphaFunc(func, ref);
}

void APIENTRY glBegin (GLenum mode){
	gFakeGL->glBegin(mode);
}

void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor){
	gFakeGL->glBlendFunc(sfactor, dfactor);
}

void APIENTRY glClear (GLbitfield mask){
	gFakeGL->glClear(mask);
}

void APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha){
	gFakeGL->glClearColor(red, green, blue, alpha);
}

void APIENTRY glColor3f (GLfloat red, GLfloat green, GLfloat blue){
	gFakeGL->glColor3f(red, green, blue);
}

void APIENTRY glColor3ubv (const GLubyte *v){
	gFakeGL->glColor3ubv(v);
}

void APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
	gFakeGL->glColor4f(red, green, blue, alpha);
}

void APIENTRY glColor4fv (const GLfloat *v){
	gFakeGL->glColor4fv(v);
}

void APIENTRY glCullFace (GLenum mode){
	gFakeGL->glCullFace(mode);
}

void APIENTRY glDepthFunc (GLenum func){
	gFakeGL->glDepthFunc(func);
}

void APIENTRY glDepthMask (GLboolean flag){
	gFakeGL->glDepthMask(flag);
}

void APIENTRY glDepthRange (GLclampd zNear, GLclampd zFar){
	gFakeGL->glDepthRange(zNear, zFar);
}

void APIENTRY glDisable (GLenum cap){
	gFakeGL->glDisable(cap);
}

void APIENTRY glDrawBuffer (GLenum mode){
	gFakeGL->glDrawBuffer(mode);
}

void APIENTRY glEnable (GLenum cap){
	gFakeGL->glEnable(cap);
}

void APIENTRY glEnd (void){
	return; // Does nothing
//	gFakeGL->glEnd();
}

void APIENTRY glFinish (void){
	gFakeGL->glFinish();
}

void APIENTRY glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){
	gFakeGL->glFrustum(left, right, bottom, top, zNear, zFar);
}

void APIENTRY glGetFloatv (GLenum pname, GLfloat *params){
	gFakeGL->glGetFloatv(pname, params);
}

const GLubyte * APIENTRY glGetString (GLenum name){
	return gFakeGL->glGetString(name);
}

void APIENTRY glHint (GLenum target, GLenum mode){
	gFakeGL->glHint(target, mode);
}

void APIENTRY glLoadIdentity (void){
	gFakeGL->glLoadIdentity();
}

void APIENTRY glLoadMatrixf (const GLfloat *m){
	gFakeGL->glLoadMatrixf(m);
}

void APIENTRY glMatrixMode (GLenum mode){
	gFakeGL->glMatrixMode(mode);
}

void APIENTRY glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){
	gFakeGL->glOrtho(left, right, top, bottom, zNear, zFar);
}

void APIENTRY glPolygonMode (GLenum face, GLenum mode){
	gFakeGL->glPolygonMode(face, mode);
}

void APIENTRY glPopMatrix (void){
	gFakeGL->glPopMatrix();
}

void APIENTRY glPushMatrix (void){
	gFakeGL->glPushMatrix();
}

void APIENTRY glReadBuffer (GLenum mode){
	gFakeGL->glReadBuffer(mode);
}

void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels){
	gFakeGL->glReadPixels(x, y, width, height, format, type, pixels);
}

void APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
	gFakeGL->glRotatef(angle, x, y, z);
}

void APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z){
	gFakeGL->glScalef(x, y, z);
}

void APIENTRY glShadeModel (GLenum mode){
	gFakeGL->glShadeModel(mode);
}

void APIENTRY glTexCoord2f (GLfloat s, GLfloat t){
	gFakeGL->glTexCoord2f(s, t);
}

void APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param){
	gFakeGL->glTexEnvf(target, pname, param);
}

void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels){
	gFakeGL->glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param){
	gFakeGL->glTexParameterf(target, pname, param);
}

void APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels){
	gFakeGL->glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z){
	gFakeGL->glTranslatef(x, y, z);
}

void APIENTRY glVertex2f (GLfloat x, GLfloat y){
	gFakeGL->glVertex2f(x, y);
}

void APIENTRY glVertex3f (GLfloat x, GLfloat y, GLfloat z){
	gFakeGL->glVertex3f(x, y, z);
}

void APIENTRY glVertex3fv (const GLfloat *v){
	gFakeGL->glVertex3fv(v);
}

void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height){
	gFakeGL->glViewport(x, y, width, height);
}

HGLRC gHGLRC;

#ifndef _XBOX
extern "C" {

extern HWND mainwindow;

};
#endif

extern "C"
HGLRC WINAPI wglCreateContext(HDC /* hdc */){
#ifdef _XBOX
	return (HGLRC) new FakeGL(0);
#else
	return (HGLRC) new FakeGL(mainwindow);
#endif
}

extern "C"
BOOL  WINAPI wglDeleteContext(HGLRC hglrc){
	FakeGL* fgl = (FakeGL*) hglrc;
	delete fgl;
	return true;
}

extern "C"
HGLRC WINAPI wglGetCurrentContext(VOID){
	return gHGLRC;
}

static void APIENTRY BindTextureExt(GLenum target, GLuint texture){
	gFakeGL->glBindTexture(target, texture);
}

static void APIENTRY MTexCoord2fSGIS(GLenum target, GLfloat s, GLfloat t){
	gFakeGL->glMTexCoord2fSGIS(target, s, t);
}

static void APIENTRY SelectTextureSGIS(GLenum target){
	gFakeGL->glSelectTextureSGIS(target);
}

// type cast unsafe conversion from
#pragma warning( push )
#pragma warning( disable : 4191)

extern "C"
PROC  WINAPI wglGetProcAddress(LPCSTR s){
	static LPCSTR kBindTextureEXT = "glBindTextureEXT";
	static LPCSTR kMTexCoord2fSGIS = "glMTexCoord2fSGIS"; // Multitexture
	static LPCSTR kSelectTextureSGIS = "glSelectTextureSGIS";
	if ( strncmp(s, kBindTextureEXT, sizeof(kBindTextureEXT)-1) == 0){
		return (PROC) BindTextureExt;
	}
	else if ( strncmp(s, kMTexCoord2fSGIS, sizeof(kMTexCoord2fSGIS)-1) == 0){
		return (PROC) MTexCoord2fSGIS;
	}
	else if ( strncmp(s, kSelectTextureSGIS, sizeof(kSelectTextureSGIS)-1) == 0){
		return (PROC) SelectTextureSGIS;
	}
	// LocalDebugBreak();
	return 0;
}

#pragma warning( pop )

extern "C"
BOOL  WINAPI wglMakeCurrent(HDC hdc, HGLRC hglrc){
	gHGLRC = hglrc;
	gFakeGL = (FakeGL*) hglrc;
	return TRUE;
}

extern "C"{

void d3dSetMode(int fullscreen, int width, int height, int bpp, int zbpp);
void d3dEvictTextures();
void FakeSwapBuffers();
void d3dSetGammaRamp(const unsigned char* gammaTable);
void d3dInitSetForce16BitTextures(int force16bitTextures);
void d3dHint_GenerateMipMaps(int value);
float d3dGetD3DDriverVersion();
};

void d3dEvictTextures(){
	gFakeGL->EvictTextures();
}

void d3dSetMode(int fullscreen, int width, int height, int bpp, int zbpp){
#ifdef _XBOX
	gFullScreen = 0;
	gWidth = 640;
	gHeight = 480;
	gBpp = 32;
	gZbpp = 24;
#else
	gFullScreen = fullscreen != 0;
	gWidth = width;
	gHeight = height;
	gBpp = bpp;
	gZbpp = zbpp;
#endif
}

void FakeSwapBuffers(){
	if ( ! gFakeGL ) {
		return;
	}
	gFakeGL->SwapBuffers();
}

void d3dSetGammaRamp(const unsigned char* gammaTable){
	gFakeGL->SetGammaRamp(gammaTable);
}

void d3dInitSetForce16BitTextures(int force16bitTextures){
	// called before gFakeGL exits. That's why we set a global
#ifndef _XBOX
	g_force16bitTextures = force16bitTextures != 0;
#endif
}

void d3dHint_GenerateMipMaps(int value){
	gFakeGL->Hint_GenerateMipMaps(value);
}

float d3dGetD3DDriverVersion(){
	return 0.73f;
}

#ifdef _XBOX

const char *SzGetMSTypeDescr(DWORD mstype)
{
    #undef XTAG
    #define XTAG(_tag) { D3DMULTISAMPLE_##_tag, #_tag }
    static const struct
    {
        DWORD mstype;
        const char *szD3DMSStr;
    } rgszMSStr[] =
    {
        XTAG(NONE), XTAG(2_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(2_SAMPLES_MULTISAMPLE_QUINCUNX), XTAG(2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR),
        XTAG(2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR), XTAG(4_SAMPLES_MULTISAMPLE_LINEAR),
        XTAG(4_SAMPLES_MULTISAMPLE_GAUSSIAN), XTAG(4_SAMPLES_SUPERSAMPLE_LINEAR),
        XTAG(4_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(9_SAMPLES_MULTISAMPLE_GAUSSIAN),
        XTAG(9_SAMPLES_SUPERSAMPLE_GAUSSIAN), XTAG(PREFILTER_FORMAT_DEFAULT),
        XTAG(PREFILTER_FORMAT_X1R5G5B5), XTAG(PREFILTER_FORMAT_R5G6B5),
        XTAG(PREFILTER_FORMAT_X8R8G8B8), XTAG(PREFILTER_FORMAT_A8R8G8B8),
    };
    static const int cType = sizeof(rgszMSStr) / sizeof(rgszMSStr[0]);

    for(int ifmt = 0; ifmt < cType; ifmt++)
    {
        if(rgszMSStr[ifmt].mstype == mstype)
            return rgszMSStr[ifmt].szD3DMSStr;
    }

    return NULL;
}

extern "C" void FGL_SetAAType(int mstype)
{
    if(mstype != -1)
    {
        // get multisample description
        const char *szType = SzGetMSTypeDescr(mstype);

        if(!szType)
        {
            Con_Printf("unknown MultiSampleType: 0x%08lx\n", mstype);
        }
        else
        {
            Con_Printf("Switching MultiSampleType to: 0x%08lx %s\n",
                mstype, szType);

        	D3DPRESENT_PARAMETERS params = {0};

            params.MultiSampleType = (D3DMULTISAMPLE_TYPE)mstype;

        	params.BackBufferWidth           = gWidth;
        	params.BackBufferHeight          = gHeight;
        	params.BackBufferFormat          = D3DFMT_X8R8G8B8;
        	params.BackBufferCount           = 1;
        	params.Windowed                  = false;   // Must be false for Xbox.
        	params.EnableAutoDepthStencil    = true;
        	params.AutoDepthStencilFormat    = D3DFMT_D24S8;
        	params.SwapEffect                = D3DSWAPEFFECT_DISCARD;
        	params.FullScreen_RefreshRateInHz= 60;
        	params.hDeviceWindow             = NULL;
            params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

            HRESULT hr = D3DDevice_Reset(&params);

        	if(FAILED(hr))
        	{
        		char errStr[100];
        		D3DXGetErrorStringA(hr, errStr, sizeof(errStr) / sizeof(errStr[0]) );
                Con_Printf("D3D Error: %s\n", errStr);
        		LocalDebugBreak();
        	}
        }
    }
}

extern "C" void FGL_SaveScreenShot(char *szfile)
{
    IDirect3DSurface8 *pFrontBuffer = NULL;

    D3DDevice_GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
    if(pFrontBuffer)
    {
        D3DDevice_BlockUntilIdle();

        XGWriteSurfaceToFile(pFrontBuffer, szfile);
        pFrontBuffer->Release();
    }
    else
    {
        Con_Printf("Failed to get frontbuffer.\n");
    }
}

#endif

