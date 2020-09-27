/*****************************************************
*** udtexture.h
***
*** Header file for our udtexture class.  This class
*** will hold a texture and allow the user to perform
*** certain actions on this texture, like writing
*** text, etc
*** 
*** by James N. Helm
*** January 24th, 2001
*** 
*** Modified
*** 03/09/2001 - James N. Helm
*****************************************************/

#ifndef _UDTEXTURE_H_
#define _UDTEXTURE_H_

#include "xfont.h"

typedef struct _UDTEXTURE_VERTEX
{
	D3DXVECTOR3 v;
	float       fRHW;
	D3DCOLOR    cDiffuse;
} UDTEXTURE_VERTEX;

#define D3DFVF_UDTEXTURE_VERTEX     ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

class CUDTexture
{
public:
    // Constructors and Destructors
    CUDTexture( void );
    ~CUDTexture( void );

    // Public Properties

    // Public Methods
    void Initialize( IDirect3DDevice8* pDevice,             // Initialize our texture and create our surface 
                     char* pszFileName,
                     int iTextureWidth,
                     int iTextureHeight );

    void CleanUp();                                         // Clean up any memory we have allocated
    
    void Clear( DWORD dwColor );                            // Clear the current Texture, setting the background to specified color
    
    void DrawBox( float fX1,                                // Draw a box on the screen
                  float fY1,
                  float fX2,
                  float fY2,
                  DWORD dwColor );

    void DrawClearRect( float fX1,                          // Draw a clear rectangle to the screen
                        float fY1,
                        float fX2,
                        float fY2,
                        DWORD dwColor );

    void DrawLine( float fX1,                               // Draw a line on the screen
                   float fY1,
                   float fX2,
                   float fY2,
                   float fLineWidth,
                   DWORD dwColor );

    void DrawOutline( float fX1,                            // Draw an outline at the current location with a specific border size
                      float fY1,
                      float fX2,
                      float fY2,
                      float fLineWidth,
                      DWORD dwColor );

    void DrawText( XFONT* pXFont,                           // Prints formatted text, using the default font size, in the specified font
                   float fX1,
                   float fY1,
                   DWORD dwFGColor,
                   DWORD dwBGColor,
                   const WCHAR* pwszFormat,
                   ... );

    void DrawText( float fX1,                               // Prints formatted text, using the default font size
                   float fY1,
                   DWORD dwFGColor,
                   DWORD dwBGColor,
                   const WCHAR* pwszFormat,
                   ... );

    void Lock();                                            // Lock the texture surface so that we can render on to it
    void Unlock();                                          // Unlock the texture surface after we have rendered on to it

    IDirect3DTexture8* GetTexture() { return m_pTexture; };                 // Get the address of the texture
    IDirect3DSurface8* GetTextureSurface() { return m_pTextureSurface; };   // Get the address of the texture surface

private:
    // Private Properties
    IDirect3DDevice8*       m_pD3DDevice;               // Direct 3D Device Pointer
    IDirect3DTexture8*      m_pTexture;                 // Actual texture being worked on
    IDirect3DSurface8*      m_pTextureSurface;          // Surface of the texture -- can render to this
    IDirect3DSurface8*      m_pCurrentRenderTarget;     // Used to store the render target that is set outside the texture
    IDirect3DVertexBuffer8* m_pVertexBuffer;            // Vertex Buffer used for our Primitive calls
    IDirect3DVertexBuffer8* m_pCurrentVBuffer;          // Used to store Vertex Buffer that is set outside the texture
    UINT                    m_uiCurrentStride;          // Used to store the stride of the VB that is set outside the texture
    D3DLOCKED_RECT          m_d3dlr;                    // Rect structure used in our "lock"
    int                     m_iTextureWidth;            // Width of the Texture
    int                     m_iTextureHeight;           // Height of the Texture
    unsigned int            m_uiTextureLockedRef;       // Used to determine if our texture is locked
    
    // Private Methods
};

#endif // _UDTEXTURE_H_