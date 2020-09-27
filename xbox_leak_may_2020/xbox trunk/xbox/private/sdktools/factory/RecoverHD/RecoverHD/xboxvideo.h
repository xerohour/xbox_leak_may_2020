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
    
    void DrawBox( float x1,                             // Draw a box on the screen
                  float y1,
                  float x2,
                  float y2,
                  DWORD color );

    void DrawBitBlt( float fx,                          // Draw the inputed bit array to the screen
                     float fy,
                     ULONG ulWidth,
                     ULONG ulHeight,
                     const DWORD* pData );
    
    void DrawClearRect( float x1,                       // Draw a clear rectangle to the screen
                        float y1,
                        float x2,
                        float y2,
                        DWORD color );

    HRESULT DrawLine( float x1,                         // Draw a line on the screen
                      float y1,
                      float x2,
                      float y2,
                      float fLineWidth,
                      DWORD color );

    void DrawOutline( float x1,                         // Draw an outline at the current location with a specific border size
                      float y1,
                      float x2,
                      float y2,
                      float fLineWidth,
                      DWORD color );

    int DrawText( float x1,                             // Prints formatted text, using the default font size and color
                  float y1,
                  DWORD fgcolor,
                  DWORD bgcolor,
                  const TCHAR* format,
                  ... );

    int GetStringPixelWidth( IN LPCWSTR string,         // Get the pixel width of a string
                             IN int length = -1 );

    int GetStringPixelWidth( IN char* string,           // Get the pixel width of a char*
                             IN int length /*=-1*/ ); 

    void GetFontSize( unsigned int* pheight,            // Get the size of the currently selected font
                      unsigned int* pdecent );

    void LightenImage( int nWidth,                      // Lighten the image specified by "pBits"
                       int nHeight,
                       DWORD* pBits,
                       DWORD dwAddto );
    void DarkenImage( int nWidth,                       // Darken the image specified by "pBits"
                      int nHeight,
                      DWORD* pBits,
                      DWORD dwSubtract );

    HRESULT TruncateStringToFit( char* pString,         // Truncate a string to fit within a certain screen size
                                 unsigned int nStrSize,
                                 float x1,
                                 float x2 );

    HRESULT TruncateStringToFit( WCHAR* pString,        // Truncate a string to fit within a certain screen size
                                 unsigned int nStrSize,
                                 float x1,
                                 float x2 );

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
    
    // Font Properties
    XFONT* m_Font;
    float m_FontWidth;
    float m_FontHeight;
    DWORD m_FontColor;
    DWORD m_FontBgColor;

    // Private Methods
};

#endif // _XBOXVIDEO_H_