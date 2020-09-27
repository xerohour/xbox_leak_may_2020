#ifndef _BITMAPFILE_H_
#define _BITMAPFILE_H_

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

class BitmapFile 
{
public:
    // Constructors and Destructor
	BitmapFile(void);
	~BitmapFile(void);
	
    // Public Methods
	void write( char* pszFile,
                int iImageWidth,
                int iImageHeight,
                DWORD* pdwPixels );
	
    int imagesize( char* pszFile );
    void readWithAlpha( int iXloc,                          // Read the BMP and the alpha mask for the BMP
                        int iYloc,
                        char* pszFile,
                        char* pszAlphaFile );
	void read( int iXloc,
               int iYloc,
               char* pszFile );
	BOOL render( IDirect3DDevice8* pD3DDevice,              // Render to a specified surface, or default to the back buffer
                 IDirect3DSurface8* pD3DSurface = NULL );

    DWORD GetWidth() { return m_dwWidth; };                 // Get the Width of the bitmap file
    DWORD GetHeight() { return m_dwHeight; };               // Get the Height of the bitmap file
    int   GetXPos() { return m_iXpos; };                    // Get the X Position of the bitmap file
    int   GetYPos() { return m_iYpos; };                    // Get the Y Position of the bitmap file
    void  SetXPos( int iX ) { m_iXpos = iX; };              // Set the X Position of the bitmap file
    void  SetYPos( int iY ) { m_iYpos = iY; };              // Set the Y Position of the bitmap file

private:
    // Private Properties
	DWORD   m_dwImageSize;
	DWORD*  m_pdwPixels;
	int     m_iXpos;
    int     m_iYpos;

	// Bitmap File Header
	WORD    m_wType;
	DWORD   m_dwFileSize;
	WORD    m_wReserved1;
	WORD    m_wReserved2;
	DWORD   m_dwOffBits;
	
	// Bitmap Info Header
	DWORD   m_dwInfoSize;
	DWORD   m_dwWidth;
	DWORD   m_dwHeight;
	WORD    m_wPlanes;
	WORD    m_wBitCount;
	DWORD   m_dwCompression;
	DWORD   m_dwSizeImage;
	DWORD   m_dwXPelsPerMeter;
	DWORD   m_dwYPelsPerMeter;
	DWORD   m_dwClrUsed;
	DWORD   m_dwClrImportant;

    // Private Methods
    void CleanUp();                     // Destory any allocated memory, etc
    void readAlpha( char* pszFile );    // Read the alpha mask for the BMP
};

#endif // _BITMAPFILE_H_