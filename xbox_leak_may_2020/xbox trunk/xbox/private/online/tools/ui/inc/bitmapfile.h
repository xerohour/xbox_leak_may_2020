///////////////////////////////////////////////////////////////////////////////
//
// Name: bitmapfile.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the bitmap file handler
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <stdio.h>

class CBitmapFile 
{

public:

	//
	// constructor and destructor
	//

	CBitmapFile( void );
	~CBitmapFile( void );

	//
	// read from memory a file to memory
	//

	HRESULT Read( INT iXloc, INT iYloc, LPSTR pszFile );

	//
	// render bitmap to a d3d device
	//

	HRESULT Render( IDirect3DDevice8* pD3DDevice ); 

	//
	// accessor for widht, height, and bitmap position
	//

    DWORD GetWidth() { return m_dwWidth; };    
    DWORD GetHeight() { return m_dwHeight; }; 
    INT   GetXPos() { return m_iXpos; };        
    INT   GetYPos() { return m_iYpos; };       

	//
	// modifiers for x and y pos
	//

    void  SetXPos( INT iX ) { m_iXpos = iX; };  
    void  SetYPos( INT iY ) { m_iYpos = iY; };  

private:

	//
	// helper function to clean up internal state
	//

    void CleanUp( void );       

	//
	// the tital image size
	//

	DWORD   m_dwImageSize;

	//
	// the array of pixels values
	//

	PDWORD  m_pdwPixels;

	//
	// the x and y positions
	//

	INT     m_iXpos;
    INT     m_iYpos;

	//
	// BMP header file info
	//

	WORD    m_wType;
	DWORD   m_dwFileSize;
	WORD    m_wReserved1;
	WORD    m_wReserved2;
	DWORD   m_dwOffBits;
	
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
};

