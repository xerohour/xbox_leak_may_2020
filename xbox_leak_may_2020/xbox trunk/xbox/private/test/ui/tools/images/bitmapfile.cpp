#include "imagepch.h"
#include "bitmapfile.h"

//////////////////////////////////////////////////////////////////////
// .BMP File format functions

BitmapFile::BitmapFile() 
:
m_dwImageSize( 0 ),
m_pdwPixels( NULL ),
m_iXpos( 0 ),
m_iYpos( 0 ),
//
// Initalize Bitmap File Header
m_wType( 19778 ),
m_dwFileSize( 192056 ), //check this
m_wReserved1( 0 ),
m_wReserved2( 0 ),
m_dwOffBits( 54 ), //check this
//
// Initialize Bitmap Info Header
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
}

BitmapFile::~BitmapFile()
{
    // Clean up memory, etc
    CleanUp();
}


// Destory any allocated memory, etc
void BitmapFile::CleanUp()
{
    if( m_pdwPixels )
    {
        delete [] m_pdwPixels;
        m_pdwPixels = NULL;
    }
}


void BitmapFile::write( char* pszFile, int iImageWidth, int iImageHeight, DWORD* pdwPixels )
{
    FILE* pfOutput;

    m_dwWidth     = iImageWidth;
    m_dwHeight    = iImageHeight;
    m_dwImageSize = iImageWidth * iImageHeight;

    //open the new bitmap file
    pfOutput = fopen( pszFile, "wb" );
    if( !pfOutput )
    {
        XDBGWRN( IMAGES_APP_TITLE_NAME_A, " BitmapFile::write():Failed to open the file!!" );

        return;
    }

    // Write the Bitmap File header information
    fwrite( &m_wType, sizeof( WORD ), 1, pfOutput );
    fwrite( &m_dwFileSize, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_wReserved1, sizeof( WORD ), 1, pfOutput );
    fwrite( &m_wReserved2, sizeof( WORD ), 1, pfOutput );
    fwrite( &m_dwOffBits, sizeof( DWORD ), 1, pfOutput );

    // Write the Bitmap Info header information
    fwrite( &m_dwInfoSize, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_dwWidth, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_dwHeight, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_wPlanes, sizeof( WORD ), 1, pfOutput );
    fwrite( &m_wBitCount, sizeof( WORD ), 1, pfOutput );
    fwrite( &m_dwCompression, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_dwSizeImage, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_dwXPelsPerMeter, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_dwYPelsPerMeter, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_dwClrUsed, sizeof( DWORD ), 1, pfOutput );
    fwrite( &m_dwClrImportant, sizeof( DWORD ), 1, pfOutput );

    fclose( pfOutput );
}

int BitmapFile::imagesize( char* pszFile )
{
    FILE* pfInput;
    pfInput = fopen( pszFile, "rb" );
    if( !pfInput )
    {
        XDBGWRN( IMAGES_APP_TITLE_NAME_A, " BitmapFile::imagesize():Failed to open the file!!" );

        return -1;
    }

    // Read the Bitmap File header information
    fread( &m_wType, sizeof( WORD ), 1, pfInput );
    fread( &m_dwFileSize, sizeof( DWORD ), 1, pfInput );
    fread( &m_wReserved1, sizeof( WORD ), 1, pfInput );
    fread( &m_wReserved2, sizeof( WORD ), 1, pfInput );
    fread( &m_dwOffBits, sizeof( DWORD ), 1, pfInput );

    //Read the Bitmap Info header information
    fread( &m_dwInfoSize, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwWidth, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwHeight, sizeof( DWORD ), 1, pfInput );

    fclose( pfInput );

    return ( m_dwWidth * m_dwHeight );
}


// Read the BMP and the alpha mask for the BMP
void BitmapFile::readWithAlpha( int iXloc, int iYloc, char* pszFile, char* pszAlphaFile )
{
    read( iXloc, iYloc, pszFile );
    readAlpha( pszAlphaFile );
}


// Read the alpha mask for the BMP
void BitmapFile::readAlpha( char* pszFile )
{
    if( NULL == pszFile )
    {
        XDBGWRN( IMAGES_APP_TITLE_NAME_A, " BitmapFile::readAlpha():Invalid argument(s) passed in!!" );

        return;
    }

    //open the new bitmap file
    FILE* pfInput = fopen( pszFile, "rb" );
    if( NULL == pfInput )
    {
        XDBGWRN( IMAGES_APP_TITLE_NAME_A, " BitmapFile::readAlpha():fopen failed!! - '%hs'", pszFile );

        return;
    }

    WORD wType, wReserved1, wReserved2, wPlanes, wBitCount;
    DWORD dwFileSize, dwOffBits, dwInfoSize, dwWidth, dwHeight, dwCompression, dwSizeImage, dwXPelsPerMeter;
    DWORD dwYPelsPerMeter, dwClrUsed, dwClrImportant;


    // Read the Bitmap File header information
    fread( &wType, sizeof( WORD ), 1, pfInput );
    fread( &dwFileSize, sizeof( DWORD ), 1, pfInput );
    fread( &wReserved1, sizeof( WORD ), 1, pfInput );
    fread( &wReserved2, sizeof( WORD ), 1, pfInput );
    fread( &dwOffBits, sizeof( DWORD ), 1, pfInput );

    //Read the Bitmap Info header information
    fread( &dwInfoSize, sizeof( DWORD ), 1, pfInput );
    fread( &dwWidth, sizeof( DWORD ), 1, pfInput );
    fread( &dwHeight, sizeof( DWORD ), 1, pfInput );
    fread( &wPlanes, sizeof( WORD ), 1, pfInput );
    fread( &wBitCount, sizeof( WORD ), 1, pfInput );
    fread( &dwCompression, sizeof( DWORD ), 1, pfInput );
    fread( &dwSizeImage, sizeof( DWORD ), 1, pfInput );
    fread( &dwXPelsPerMeter, sizeof( DWORD ), 1, pfInput );
    fread( &dwYPelsPerMeter, sizeof( DWORD ), 1, pfInput );
    fread( &dwClrUsed, sizeof( DWORD ), 1, pfInput );
    fread( &dwClrImportant, sizeof( DWORD ), 1, pfInput );

    if( m_dwImageSize != ( m_dwWidth * m_dwHeight ) )
    {
        XDBGWRN( IMAGES_APP_TITLE_NAME_A, " BitmapFile::readAlpha():Alpha BMP and Original BMP not the same size!!" );

        return;
    }

    float fRemainer = (float)dwWidth / 4;
    float fDelta = fRemainer - (int)fRemainer;

    BYTE bBlue, bGreen, bRed;
    for( int y = (int)dwHeight - 1; y >= 0 ; y-- )
    {
        for( int x = 0; x < (int)dwWidth; x++ )
        {
            fread( &bBlue, sizeof( BYTE ), 1, pfInput );
            fread( &bGreen, sizeof( BYTE ), 1, pfInput );
            fread( &bRed, sizeof( BYTE ), 1, pfInput );
            int iPosition = ( y * (int)dwWidth ) + x;
            m_pdwPixels[iPosition] = ( m_pdwPixels[iPosition] & 0x00FFFFFF ) | ( bBlue << 24 );
        }

        //account for the extra bytes in bitmap files
        if( fDelta == 0.25f )
        {
            fseek( pfInput, 1, SEEK_CUR );
        }
        else if( fDelta == 0.50f )
        {
            fseek( pfInput, 2, SEEK_CUR );
        }
        else if( fDelta == 0.75f )
        {
            fseek( pfInput, 3, SEEK_CUR );
        }
    }

    fclose( pfInput );
}


void BitmapFile::read( int iXloc, int iYloc, char* pszFile )
{
    FILE* pfInput;
    m_iXpos = iXloc;
    m_iYpos = iYloc;

    // Clean up any memory if necessary
    CleanUp();

    //open the new bitmap file
    pfInput = fopen( pszFile, "rb" );

    if( !pfInput )
    {
        XDBGWRN( IMAGES_APP_TITLE_NAME_A, " BitmapFile::read():fopen failed!!" );

        return;
    }

    // Read the Bitmap File header information
    fread( &m_wType, sizeof( WORD ), 1, pfInput );
    fread( &m_dwFileSize, sizeof( DWORD ), 1, pfInput );
    fread( &m_wReserved1, sizeof( WORD ), 1, pfInput );
    fread( &m_wReserved2, sizeof( WORD ), 1, pfInput );
    fread( &m_dwOffBits, sizeof( DWORD ), 1, pfInput );

    //Read the Bitmap Info header information
    fread( &m_dwInfoSize, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwWidth, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwHeight, sizeof( DWORD ), 1, pfInput );
    fread( &m_wPlanes, sizeof( WORD ), 1, pfInput );
    fread( &m_wBitCount, sizeof( WORD ), 1, pfInput );
    fread( &m_dwCompression, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwSizeImage, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwXPelsPerMeter, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwYPelsPerMeter, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwClrUsed, sizeof( DWORD ), 1, pfInput );
    fread( &m_dwClrImportant, sizeof( DWORD ), 1, pfInput );

    m_dwImageSize = m_dwWidth * m_dwHeight;

    m_pdwPixels = new DWORD[m_dwImageSize];

    float fRemainer = (float)m_dwWidth / 4;
    float fDelta = fRemainer - (int)fRemainer;

    BYTE bBlue, bGreen, bRed;
    for( int y = (int)m_dwHeight - 1; y >= 0 ; y-- )
    {
        for( int x = 0; x < (int)m_dwWidth; x++ )
        {
            fread( &bBlue, sizeof( BYTE ), 1, pfInput );
            fread( &bGreen, sizeof( BYTE ), 1, pfInput );
            fread( &bRed, sizeof( BYTE ), 1, pfInput );
            int iPosition = ( y * (int)m_dwWidth ) + x;
            m_pdwPixels[iPosition] = ( 0xff << 24 ) | ( bRed << 16 ) | ( bGreen << 8 ) | ( bBlue );
        }

        //account for the extra bytes in bitmap files
        if( fDelta == 0.25f )
        {
            fseek( pfInput, 1, SEEK_CUR );
        }
        else if( fDelta == 0.50f )
        {
            fseek( pfInput, 2, SEEK_CUR );
        }
        else if( fDelta == 0.75f )
        {
            fseek( pfInput, 3, SEEK_CUR );
        }
    }

    fclose( pfInput );
}


// Render the image to the back buffer
BOOL BitmapFile::render( IDirect3DDevice8* pD3DDevice, IDirect3DSurface8* pD3DSurface/*=NULL*/ )
{
    HRESULT hr;
    BOOL fRelease = FALSE;
    D3DLOCKED_RECT surfaceInfo;
    RECT container;

    if( !pD3DSurface )
    {
        // Get the back buffer
        hr = pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pD3DSurface );
        fRelease = TRUE;
    }

    container.top    = m_iYpos;
    container.left   = m_iXpos;
    container.bottom = m_iYpos + m_dwHeight;
    container.right  = m_iXpos + m_dwWidth;

    // Lock our surface so we can write to it
    hr = pD3DSurface->LockRect( &surfaceInfo, &container, 0 );

    int iDestPitch = surfaceInfo.Pitch;
    int iSourcePitch = m_dwWidth * 4;
    // BYTE* pbS = (BYTE*)m_pdwPixels;
    // BYTE* pbP = (BYTE*)surfaceInfo.pBits;
    DWORD* pdwS = m_pdwPixels;
    DWORD* pdwP = (DWORD*)surfaceInfo.pBits;
    for( int y = 0; y < (int)m_dwHeight; y++ )
    {
        for( int x = 0; x < (int)m_dwWidth; x++ )
        {
            switch (pdwS[x] & 0xff000000) 
			{
            case 0xff000000:
                pdwP[x] = pdwS[x]; //  | 0xFF000000;
                break;
            case 0:
                break;
            default: 
				{
					DWORD dwDst = pdwP[x];
					float fAlpha = (float)(pdwS[x] >> 24) / 255.0f;
					float fRed = (float)((pdwS[x] & 0xff0000) >> 16) * fAlpha + (float)((dwDst & 0xff0000) >> 16) * (1.0f - fAlpha);
					float fGreen = (float)((pdwS[x] & 0xff00) >> 8) * fAlpha + (float)((dwDst & 0xff00) >> 8) * (1.0f - fAlpha);
					float fBlue = (float)(pdwS[x] & 0xff) * fAlpha + (float)(dwDst & 0xff) * (1.0f - fAlpha);
					pdwP[x] = (0xff << 24) | ((BYTE)fRed << 16) | ((BYTE)fGreen << 8) | (BYTE)fBlue;
					break;
				}
            }

			// pdwP[x] = pdwS[x];
        }
        
        pdwS += iSourcePitch >> 2;
        pdwP += iDestPitch >> 2;
    }

    // Unlock our surface
    pD3DSurface->UnlockRect();

    // Release our Surface
    if( fRelease )
    {
        pD3DSurface->Release();
        pD3DSurface = NULL;
    }

    return TRUE;
}
