/*****************************************************
*** xboxvideo.h
***
*** Header file for our xboxvideo class.  This class
*** will initial D3D graphics, and allow the user to
*** draw text or graphics to the screen.
***
*** by James N. Helm
*** November 1st, 2000
***
*****************************************************/

#ifndef _XBOXVIDEO_H_
#define _XBOXVIDEO_H_

#include "constants.h"

// Macros
#define CHECK(code) do { int _ret__; _ret__ = code; if(_ret__ != D3D_OK) XDBGWRN( APP_TITLE_NAME_A, #code "File %s Line %d, Failed: %d", __LINE__, __FILE__, _ret__); } while (0);

#define NUM_VB_VERTS            100

// XBox configuration
class CXBoxVideo
{
public:
    // Constructors and Destructors
    CXBoxVideo( void );
    ~CXBoxVideo( void );

    // Public Properties

    // Public Methods
    void Initialize( int width,                         // Initialize D3D for this screen -- height and width default
                     int height );

    void DeInitialize( void );                          // Free our objects
    void ClearScreen( DWORD color );                    // Clear the current screen

    IDirect3DDevice8* GetD3DDevicePtr() { return m_Device; }; // Return the D3DDevice pointer to the caller

    void ShowScreen();                                  // Display the current backbuffer on the screen

    HRESULT GetTransform( D3DTRANSFORMSTATETYPE State,  // Get the D3D Transform
                          D3DMATRIX* pMatrix );         // Set the D3D Transform

    HRESULT SetTransform( D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix );

    HRESULT SetVertexShader( DWORD Handle );            // Set the D3D Vertex Shader
    HRESULT GetVertexShader( DWORD* pHandle );          // Get the D3D Vertex Shader

private:
    // Vertext structure
    struct MYVERTEX
    {
	    D3DXVECTOR3 v;
	    float       fRHW;
	    D3DCOLOR    cDiffuse;
    };

    // Private Properties
    IDirect3DDevice8* m_Device;
    IDirect3DSurface8* m_BackBuffer;
    IDirect3DVertexBuffer8* m_VertexBuffer;
    int m_ScreenWidth;
    int m_ScreenHeight;

    // Private Methods
};

#endif // _XBOXVIDEO_H_