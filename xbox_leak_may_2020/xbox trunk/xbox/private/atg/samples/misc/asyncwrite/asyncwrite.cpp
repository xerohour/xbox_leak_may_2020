//-----------------------------------------------------------------------------
// File: AsyncWrite.cpp
//
// Desc: Illustrates asynchronous writing of files on the XBox.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <cassert>
#include "resource.h"





//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Wings.bmp", resource_Wings_OFFSET },
    { "BiHull.bmp", resource_BiHull_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// We write to a huge file to really test the asynchronous nature.
// Another good way to test asynchronous writes is writing to an MU. Change
// FILE_BYTES to 4MB, FILE_DRIVE to 'F' and mount 'F' using XMountMU()
const DWORD ONE_K = 1024;
const DWORD FILE_BYTES = ONE_K * ONE_K * 32;        // 32MB 
const DWORD BYTES_PER_CALL = ONE_K * ONE_K * 2;;    // Write in 2MB chunks
const BYTE  FILL_BYTE_BASE = 0xC0;                  // For verification
const CHAR  FILE_DRIVE = 'T';                       // persistent data region




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    enum Mode
    {
        NOTHING,
        WRITING,
        READING
    };

    CXBFont           m_Font;
    CXBHelp           m_Help;
    BOOL              m_bDrawHelp;
    Mode              m_Mode;
	HANDLE            m_hFile;
	DWORD             m_dwBytesRemaining;
	DWORD             m_dwFileOffset;
    CXBPackedResource m_xprResource;
	CXBMesh*          m_pPlaneMesh;
    BYTE*             m_pBuffer;
    OVERLAPPED        m_Overlapped;
    BYTE              m_byFill;

public:

    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

    CXBoxSample();

    VOID AsyncWrite();
    VOID AsyncRead();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
: 
    CXBApplication(),
    m_Font        (),
    m_Help        (),
    m_bDrawHelp   ( FALSE ),
    m_Mode        ( NOTHING ),
	m_hFile       ( NULL ),
    m_xprResource (),
	m_pPlaneMesh  ( NULL ),
    m_pBuffer     ( NULL ),
    m_Overlapped  (),
    m_byFill      ( FILL_BYTE_BASE )
{
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the help system
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Set view position
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 40.0f);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

	// initialize the airplanes
	m_pPlaneMesh = new CXBMesh;
	m_pPlaneMesh->Create( g_pd3dDevice, "models\\airplane.xbg", &m_xprResource );

    // Initialize the data to be written
    m_pBuffer = new BYTE [ BYTES_PER_CALL ];
    assert( m_pBuffer != NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	// move plane
	static FLOAT fRot = 0.0f;
	fRot += 1.57f * m_fElapsedTime;

    D3DXMATRIX matWorld;
	D3DXMatrixRotationY( &matWorld, fRot );
	FLOAT x = 20.0f * (FLOAT)cos(fRot);
	FLOAT z = -20.0f * (FLOAT)sin(fRot);

	D3DXMATRIX m;
	D3DXMatrixTranslation( &m, x, 0.0f, z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Activate file write
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        if( m_Mode == NOTHING )
        {
            CHAR strFile[] = "x:\\testfile.tmp";
            strFile[0] = FILE_DRIVE;

            // Prepare the file for asynchronous writing.
            // Also prepare the file for asynchronous reading for the
            // verification pass.
            // Requires both the OVERLAPPED and NO_BUFFERING flags.
            m_Mode = WRITING;
            m_hFile = CreateFile( strFile, GENERIC_WRITE | GENERIC_READ,
                                  0, NULL, CREATE_ALWAYS,
                                  FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
                                  NULL );
            assert( m_hFile != INVALID_HANDLE_VALUE );

            // Start writing from the beginning
            m_Overlapped.Offset = 0;
            m_Overlapped.OffsetHigh = 0;
            m_byFill = FILL_BYTE_BASE;

            // Prepare the notification event. This event is signalled when
            // each write is complete.
            m_Overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
            assert( m_Overlapped.hEvent != NULL );

            // In order for asynchronous writes to be successful, the file
            // cannot grow during the WriteFile() operation. We set the file
            // to its full size before beginning to write. The file size
            // must be an integer multiple of the sector size
            CHAR strDrive[4] = "x:\\";
            strDrive[0] = FILE_DRIVE;
            assert( FILE_BYTES % XGetDiskSectorSize( strDrive ) == 0 );

            DWORD dwPos = SetFilePointer( m_hFile, FILE_BYTES, NULL, FILE_BEGIN );
            assert( dwPos == FILE_BYTES );
            (VOID)dwPos;
            BOOL bSuccess = SetEndOfFile( m_hFile );
            assert( bSuccess );
            (VOID)bSuccess;

#ifdef _DEBUG
            // Fill buffer to a known value for verification
            FillMemory( m_pBuffer, BYTES_PER_CALL, m_byFill );
#endif

            // Start the write. We expect that WriteFile() will initiate 
            // the write but won't finish it during the call.
            BOOL bComplete = WriteFile( m_hFile, m_pBuffer, BYTES_PER_CALL, 
                                        NULL, &m_Overlapped );
            assert( !bComplete );
            (VOID)bComplete;
            assert( GetLastError() == ERROR_IO_PENDING );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x400000, 1.0f, 0L );

    // Restore state that text clobbers
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	m_pPlaneMesh->Render(0);

    switch( m_Mode )
    {
        case NOTHING:                       break;
        case WRITING:   AsyncWrite();       break;
        case READING:   AsyncRead();        break;
        default:        assert( FALSE );    break;
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
    {
        const DWORD NUM_HELP_CALLOUTS = 2;
        XBHELP_CALLOUT HelpCallouts[] =
        {
            { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display\nhelp" },
            { XBHELP_A_BUTTON, XBHELP_PLACEMENT_1, L"Begin file write" },
        };
        m_Help.Render( &m_Font, HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
		m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"AsyncWrite" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( m_Mode == WRITING || m_Mode == READING )
		{
			WCHAR s[80];
			swprintf( s, L"File %s: %luK/%luK",
                      m_Mode == WRITING ? L"write" : L"verify",
                      m_Overlapped.Offset / ONE_K,
                      FILE_BYTES / ONE_K );
			m_Font.DrawText( 64, 70, 0xffffff00, s );
		}
		else
			m_Font.DrawText( 64, 70, 0xffffff00, L"Press A Button to write file" );

		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: AsyncWrite()
// Desc: Called while the async write is in progress. If the write is still
//       underway, simply returns. If the write has completed, sets up the
//       next write. If the file has been completely written, sets up the
//       verification pass.
//-----------------------------------------------------------------------------
VOID CXBoxSample::AsyncWrite()
{
    assert( m_hFile != INVALID_HANDLE_VALUE );

    // See if the previous write is complete
    DWORD dwBytesTransferred;
    BOOL bIsWriteDone = GetOverlappedResult( m_hFile, &m_Overlapped, 
                                             &dwBytesTransferred, FALSE );

    // If the write isn't complete, keep rendering
    if( !bIsWriteDone )
    {
        assert( GetLastError() == ERROR_IO_INCOMPLETE );
        return;
    }

    // If we get here, the write is complete.
    // Note that m_Overlapped.hEvent has also been reset to non-signalled by
    // GetOverlappedResult(), so we don't have to reset it.
    m_Overlapped.Offset += dwBytesTransferred;

    // Asynchronous writes require that the buffer address is DWORD aligned,
    // the offset is an integer multiple of the sector size, and the
    // number of bytes is an integer multiple of the sector size.
    CHAR strDrive[4] = "x:\\";
    strDrive[0] = FILE_DRIVE;
    assert( DWORD(m_pBuffer) % sizeof(DWORD) == 0 );
    assert( m_Overlapped.Offset % XGetDiskSectorSize( strDrive ) == 0 );
    assert( BYTES_PER_CALL % XGetDiskSectorSize( strDrive ) == 0 );

    // If we've written the entire file, begin the verification pass
    if( m_Overlapped.Offset >= FILE_BYTES )
    {
        // Return to the beginning of the file
        DWORD dwPos = SetFilePointer( m_hFile, 0, NULL, FILE_BEGIN );
        assert( dwPos == 0 );
        (VOID)dwPos;
        
        // Start reading from the beginning
        m_byFill = FILL_BYTE_BASE;
        m_Mode = READING;
        m_Overlapped.Offset = 0;

        // Start the read. We expect that ReadFile() will initiate 
        // the read but won't finish it during the call.
        BOOL bComplete = ReadFile( m_hFile, m_pBuffer, BYTES_PER_CALL,
                                   NULL, &m_Overlapped );
        assert( !bComplete );
        (VOID)bComplete;
        assert( GetLastError() == ERROR_IO_PENDING );
    }
    else
    {
#ifdef _DEBUG
        // Modify the buffer slightly
        ++m_byFill;
        FillMemory( m_pBuffer, BYTES_PER_CALL, m_byFill );
#endif

        // We still have more data to write. Start another asynchronous
        // write to the file.
        BOOL bComplete = WriteFile( m_hFile, m_pBuffer, BYTES_PER_CALL, 
                                    NULL, &m_Overlapped );
        assert( bComplete ||
              ( !bComplete && GetLastError() == ERROR_IO_PENDING ) );
        (VOID)bComplete;
    }
}




//-----------------------------------------------------------------------------
// Name: AsyncRead()
// Desc: Called while the async read is in progress. If the read is still
//       underway, simply returns. If the read has completed, sets up the
//       next read. If the file has been completely read, closes the file.
//-----------------------------------------------------------------------------
VOID CXBoxSample::AsyncRead()
{
    assert( m_hFile != INVALID_HANDLE_VALUE );

    // See if the previous read is complete
    DWORD dwBytesTransferred;
    BOOL bIsReadDone = GetOverlappedResult( m_hFile, &m_Overlapped, 
                                            &dwBytesTransferred, FALSE );

    // If the read isn't complete, keep rendering
    if( !bIsReadDone )
    {
        assert( GetLastError() == ERROR_IO_INCOMPLETE );
        return;
    }

    // If we get here, the read is complete.
    // Note that m_Overlapped.hEvent has also been reset to non-signalled by
    // GetOverlappedResult(), so we don't have to reset it.
    m_Overlapped.Offset += dwBytesTransferred;

    // Verify the buffer
    for( DWORD i = 0; i < BYTES_PER_CALL; ++i )
    {
        assert( m_pBuffer[i] == m_byFill );
    }
    ++m_byFill;

    // Asynchronous reads require that the buffer address is DWORD aligned,
    // the offset is an integer multiple of the sector size, and the
    // number of bytes is an integer multiple of the sector size.
    CHAR strDrive[4] = "x:\\";
    strDrive[0] = FILE_DRIVE;
    assert( DWORD(m_pBuffer) % sizeof(DWORD) == 0 );
    assert( m_Overlapped.Offset % XGetDiskSectorSize( strDrive ) == 0 );
    assert( BYTES_PER_CALL % XGetDiskSectorSize( strDrive ) == 0 );

    // If we've read the entire file, we're done
    if( m_Overlapped.Offset >= FILE_BYTES )
    {
        // Clear the event
        BOOL bSuccess = CloseHandle( m_Overlapped.hEvent );
        assert( bSuccess );
        m_Overlapped.hEvent = NULL;

        // Close the file
        bSuccess = CloseHandle( m_hFile );
        assert( bSuccess );
        m_hFile = NULL;

        // All done
        m_Mode = NOTHING;
    }
    else
    {
        // We still have more data to read. Start another asynchronous
        // read from the file.
        BOOL bComplete = ReadFile( m_hFile, m_pBuffer, BYTES_PER_CALL, 
                                   NULL, &m_Overlapped );
        assert( bComplete ||
              ( !bComplete && GetLastError() == ERROR_IO_PENDING ) );
        (VOID)bComplete;
    }
}
