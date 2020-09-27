///////////////////////////////////////////////////////////////////////////////
//
// Name: BitmapFile.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for bitmap file class
//
///////////////////////////////////////////////////////////////////////////////

#include "bitmapfile.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: CBitmapFile constructor
//
// Input: None
//
// Output: None
//
// Description: creates an instance of the bitmap file class
//
///////////////////////////////////////////////////////////////////////////////

CBitmapFile::CBitmapFile( void ) 
	: m_dwImageSize( 0 ),
	  m_pdwPixels( NULL ),
	  m_iXpos( 0 ),
	  m_iYpos( 0 ),
	  m_wType( 19778 ),
	  m_dwFileSize( 192056 ), 
	  m_wReserved1( 0 ),
	  m_wReserved2( 0 ),
	  m_dwOffBits( 54 ),
	  m_dwInfoSize( 40 ),
	  m_dwWidth( 0 ),
	  m_dwHeight( 0 ),
	  m_wPlanes( 1 ),
	  m_wBitCount( 24 ),
	  m_dwCompression( 0 ),
	  m_dwSizeImage( 0 ),
	  m_dwXPelsPerMeter( 2834 ),
	  m_dwYPelsPerMeter( 2834 ),
	  m_dwClrUsed( 0 ),
	  m_dwClrImportant( 0 )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: CBitmapFile destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the bitmap file class
//
///////////////////////////////////////////////////////////////////////////////

CBitmapFile::~CBitmapFile( void )
{
	//
	// simply clean up the array
	//

    CleanUp();
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Cleanup
//
// Input: None
//
// Output: None
//
// Description: destroy any memory allocated for the bitmap file
//
///////////////////////////////////////////////////////////////////////////////

void 
CBitmapFile::CleanUp( void )
{
	//
	// if the array exists, delete it and reset it
	//

    if( m_pdwPixels )
    {
        delete [] m_pdwPixels;
        m_pdwPixels = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Read
//
// Input: iXloc - the x location to read from
//        iYloc - the y location to read from
//        pszFile - the name of the file ot read from
//
// Output: None
//
// Description: creates an instance of the bitmap file class
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CBitmapFile::Read( INT iXloc, INT iYloc, LPSTR pszFile )
{
    FILE* pfInput = NULL;
    m_iXpos = iXloc;
    m_iYpos = iYloc;

	size_t uiSize = 0;

	//
    // Clean up any memory if necessary
	//

    CleanUp();

	//
    //open the new bitmap file
	//

    pfInput = fopen( pszFile, "rb" );

	//
	// return a failure if appropriate
	//

    if( !pfInput )
    {
		ASSERT( !"Could not open file" );
		return E_FAIL;
    }

	//
    // Read the Bitmap File header information
	//

    uiSize = fread( &m_wType, sizeof( WORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwFileSize, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_wReserved1, sizeof( WORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_wReserved2, sizeof( WORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwOffBits, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwInfoSize, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwWidth, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwHeight, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_wPlanes, sizeof( WORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_wBitCount, sizeof( WORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwCompression, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwSizeImage, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwXPelsPerMeter, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwYPelsPerMeter, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwClrUsed, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );

    uiSize = fread( &m_dwClrImportant, sizeof( DWORD ), 1, pfInput );
	ASSERT( uiSize == 1 );


	//
	// claculate the image isze
	//

    m_dwImageSize = m_dwWidth * m_dwHeight;

	//
	// allocate the pixels
	//

    m_pdwPixels = new DWORD[m_dwImageSize];

	//
	// return an error if there is one
	//

	if ( !m_pdwPixels )
	{
		ASSERT( !"Could not allocate memory" );
		return E_OUTOFMEMORY;
	}

	//
	// calculate the delta and remainder
	//

    float fRemainer = (float)m_dwWidth / 4;
    float fDelta = fRemainer - (int)fRemainer;

	//
	// start to read pixels
	//

    BYTE bBlue, bGreen, bRed;

	//
	// go through each row
	//

    for( int y = (int)m_dwHeight - 1; y >= 0 ; y-- )
    {

		//
		// go through each column
		//

        for( int x = 0; x < (int)m_dwWidth; x++ )
        {
			//
			// read the red, green and blue values
			//

            uiSize = fread( &bBlue, sizeof( BYTE ), 1, pfInput );
			ASSERT( uiSize == 1 );

            uiSize = fread( &bGreen, sizeof( BYTE ), 1, pfInput );
			ASSERT( uiSize == 1 );

            uiSize = fread( &bRed, sizeof( BYTE ), 1, pfInput );
			ASSERT( uiSize == 1 );


			//
			// calculate the psosition
			//

            int iPosition = ( y * (int)m_dwWidth ) + x;

			//
			// construct a 32 bit pixel value
			//

            m_pdwPixels[iPosition] = ( 0xff << 24 ) | 
				                     ( bRed << 16 ) | 
									 ( bGreen << 8 ) | 
									 ( bBlue );
        }

		//
        //account for the extra bytes in bitmap files
		//

        if( fDelta == 0.25f )
        {
            uiSize = fseek( pfInput, 1, SEEK_CUR );
			ASSERT( 0 == uiSize );
        }
        else if( fDelta == 0.50f )
        {
            uiSize = fseek( pfInput, 2, SEEK_CUR );
			ASSERT( 0 == uiSize );
        }
        else if( fDelta == 0.75f )
        {
            uiSize = fseek( pfInput, 3, SEEK_CUR );
			ASSERT( 0 == uiSize );
        }
    }

	//
	// close the file
	//

    fclose( pfInput );

	//
	// error paths above
	//

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Render
//
// Input: pD3DDevice - the pointer to the screen
//
// Output: S_OK on success, E_XXXX on failure
//
// Description: renders the bitman to the back buffer
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CBitmapFile::Render( IDirect3DDevice8* pD3DDevice )
{
    HRESULT hr = S_OK;
	IDirect3DSurface8* pD3DSurface = NULL;
    D3DLOCKED_RECT surfaceInfo;
    RECT container;

	//
    // Get the back buffer
	//

    hr = pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pD3DSurface );
	ASSERT( SUCCEEDED( hr ) || !"could not get back buffer" );

	//
	// fail if we can't get it
	//

	if ( FAILED( hr ) )
	{
		return hr;
    }

	//
	// set up the rect where we will draw to
	//

    container.top    = m_iYpos;
    container.left   = m_iXpos;
    container.bottom = m_iYpos + m_dwHeight;
    container.right  = m_iXpos + m_dwWidth;

	//
    // Lock our surface so we can write to it
	//

    hr = pD3DSurface->LockRect( &surfaceInfo, &container, D3DLOCK_TILED );
	ASSERT( SUCCEEDED( hr ) || !"could not lock rect" );

	//
	// fail off if an error occured
	//

	if ( FAILED( hr ) )
	{
		return hr;
	}

	//
	// copy the bits!
	//

    int iDestPitch = surfaceInfo.Pitch;
    int iSourcePitch = m_dwWidth * 4;
    BYTE* pbS = (BYTE*)m_pdwPixels;
    BYTE* pbP = (BYTE*)surfaceInfo.pBits;

    for( int y = 0; y < (int)m_dwHeight; y++ )
    {
        memcpy( pbP, pbS, iSourcePitch );
        pbS += iSourcePitch;
        pbP += iDestPitch;
    }

	//
    // Unlock our surface
	//

    pD3DSurface->UnlockRect();

	//
    // Release our Surface
	//

    if( pD3DSurface )
    {
        pD3DSurface->Release();
        pD3DSurface = NULL;
    }

    return hr;
}
