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
//#include <xapip.h>
#include <xtl.h>
#define XFONT_TRUETYPE 1 // Turn TrueType Fonts on before calling xfont.h
#include <xfont.h>
#include <tchar.h>


 // Font info
#define FONT_DEFAULT_HEIGHT                     10
#define FONT_DEFAULT_WIDTH                      4
#define FONT_DEFAULT_STYLE                      XFONT_NORMAL // XFONT_BOLD | XFONT_ITALICS | XFONT_BOLDITALICS
#define FONT_ALIAS_LEVEL                        4

// Colors
#define COLOR_WHITE                             0xFFFFFFFF
#define COLOR_BLACK                             0xFF000000
#define COLOR_LIGHT_YELLOW                      0xFFDFF77F
#define COLOR_YELLOW                            0xFFFFFF00
#define COLOR_LIGHT_GRAY                        0xFFC8C8C8
#define COLOR_DARK_GRAY                         0xFF828282
#define COLOR_LIME_GREEN                        0xFF9BD351
#define COLOR_MEDIUM_GREEN                      0xFF57932E
#define COLOR_DARK_GREEN2                       0xFF357515
#define COLOR_DARK_GREEN                        0xFF2C531E

#define SCREEN_DEFAULT_BACKGROUND_COLOR         COLOR_BLACK
#define SCREEN_DEFAULT_TEXT_FG_COLOR            COLOR_LIGHT_GRAY
#define SCREEN_DEFAULT_TEXT_BG_COLOR            SCREEN_DEFAULT_BACKGROUND_COLOR

#define SCREEN_WIDTH     640
#define SCREEN_HEIGHT    480
#define FILE_DATA_DEFAULT_FONT_FILENAME         TEXT("t:\\media\\tahoma.ttf")

// Macros
//#define CHECK(code) do { int _ret__; _ret__ = code; if(_ret__ != D3D_OK) XDBGWRN( APP_TITLE_NAME_A, #code "File %s Line %d, Failed: %d", __LINE__, __FILE__, _ret__); } while (0);
#define CHECK(code) do { int _ret__; _ret__ = code; if(_ret__ != D3D_OK) continue; } while (0);
#define NUM_VB_VERTS            100


// XBox configuration
class CXBoxVideo
{

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
    TCHAR m_szDirectoryAndFont[ MAX_PATH ];
    INT m_iFontHeight;
    // Private Methods

public:
    // TextBlock Array.
    CHAR szTextBlock[40][100];
    int iTextBlockIndex;

    // Constructors and Destructors
    CXBoxVideo( void );
    CXBoxVideo( IN LPTSTR szFontDirectory );
    ~CXBoxVideo( void );


    static DWORD WINAPI DrawScreenThread(LPVOID lpParam);
    BOOL StartScreenThread( VOID );



    // Public Methods
    void SetFontHeight( IN int iFont ){m_iFontHeight =iFont; }
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
                  const PCHAR format,
                  ... );
    int DrawText( const PCHAR format, ... );
    BOOL DrawTextBlock( VOID );

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


    HRESULT  BeginScene( VOID );
    HRESULT  EndScene( VOID );
};



class Key
{
public:
	int xorigin, yorigin;		// x,y coordinate of the key's upper left pixel
	int width, height;			// width and height of the key
	DWORD selectColor;			// color to hilight the key with upon selection
	WCHAR* resultChar;			// value attached to the key
	bool render;				// should the key be rendered?

	// Constructors and Destructors
	Key();
	~Key();

	void define(int x, int y, int w, int h, DWORD color);
	void defineText(WCHAR* result);
	void setRender(bool value);
	bool getRender(void);
};

#endif // _XBOXVIDEO_H_