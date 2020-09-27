///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceVideo.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the video ui class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>
#define XFONT_TRUETYPE
#include <xfont.h>

#include "bitmapfile.h"

#include <vector>
using namespace std;

#include "UserInterfaceInput.h"
#include "UserInterfaceAudio.h"
#include "UserInterfaceScreen.h"

//
// the user interface video decleration
//

class CUserInterfaceVideo {

public:

	//
	// constructor and destructor
	//

	CUserInterfaceVideo( void );
	~CUserInterfaceVideo( void );

	//
	// methods to init and update video
	//

	HRESULT Initialize( CUserInterfaceScreen* pScreen,
		                UINT uWidth, 
						UINT uHeight,
						DWORD dwClearColor,
						LPSTR szFileName );

	HRESULT Update( CUserInterfaceInput* pInput, 
		            CUserInterfaceAudio* pAudio );

private:

	//
	// Display modifiers
	//

	HRESULT ClearScreen( DWORD dwColor );

	//
	// Prints formatted text
	//
							 
	HRESULT DrawText( LONG lX,						   
					  LONG lY, 
					  DWORD dwFGColor, 
					  DWORD dwBGColor, 
					  LPCWSTR szFormat, 
				      ... );

	//
	// draw the ui screen
	//

	HRESULT DrawScreen( void );

	//
	// the d3d device pointer
	//

	PDIRECT3DDEVICE8 m_pDevice;

	//
	// a pointer to the current Screen
	//

	CUserInterfaceScreen* m_pScreen;

	//
	// the font
	//

    XFONT* m_pFont;

    //
    // the backbuffer (surprise surprise)
    //

    D3DSurface* m_pBackBuffer;

	//
	// the width, height
	//

	UINT m_uWidth;
	UINT m_uHeight;

	//
	// the clear color
	//

	DWORD m_dwClearColor;

	//
	// the optional bitmap background
	//

	CBitmapFile* m_pBitmap;

};
