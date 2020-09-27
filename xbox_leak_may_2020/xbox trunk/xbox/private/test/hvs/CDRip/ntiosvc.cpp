#include "cdrip.h"
#include "ntiosvc.h"

////////////////////////////////////////////////////////////////////////////

CNtIoctlCdromService::CNtIoctlCdromService()
{
    m_hDevice = INVALID_HANDLE_VALUE;
}

CNtIoctlCdromService::~CNtIoctlCdromService()
{
    Close();
}

void CNtIoctlCdromService::Close()
{
    if( m_hDevice != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hDevice );
        m_hDevice = INVALID_HANDLE_VALUE;
    }

    m_toc.Delete();
}

HRESULT CNtIoctlCdromService::Open()
{
    // If the user calls this, make sure we're closed before trying to re-open
    if( IsOpen() )
        Close();

    ASSERT( m_hDevice == INVALID_HANDLE_VALUE );

    m_hDevice = CreateFile( "CDROM0:",
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL );

    if (m_hDevice == INVALID_HANDLE_VALUE)
    {
        HRESULT hr = HRESULT_FROM_WIN32( GetLastError() );
        return hr;
    }

    if ( !GetTableOfContents() )
        Close();

    return S_OK;
}


bool CNtIoctlCdromService::GetTableOfContents()
{
    HRESULT hr;
    CDROM_TOC toc;

    for( int nRetry = 0; nRetry < 1; nRetry += 1 )
    {
        hr = DeviceIoControl( IOCTL_CDROM_READ_TOC, NULL, 0, &toc, sizeof(toc) );

        if( SUCCEEDED(hr) )
            break;

        if( hr != HRESULT_FROM_WIN32( ERROR_NOT_READY ) )
            return false;
        else
            DebugPrint( "CNtIoctlCdromService::GetTableOfContents():ERROR_NOT_READY\n" );
    }

    if( FAILED(hr) )
        return false;

    // BLOCK: Convert TOC format...
    {
        DebugPrint( "CNtIoctlCdromService::GetTableOfContents():Read TOC: %d tracks\n", toc.LastTrack );

        // Verify that the TOC is structurally correct and has at least one
        // track.
        if( toc.FirstTrack > toc.LastTrack )
            return false;

        BOOL fFoundEndTrack = FALSE;

        for( DWORD i = 0; i <= (DWORD)( toc.LastTrack - toc.FirstTrack + 1 ); i++ )
        {
#define TOC_DATA_TRACK              (0x04)
#define TOC_LAST_TRACK              (0xaa)

            m_toc.TrackAddr[i] = TocValToFrames( toc.TrackData[i].Address );

            // Break out if we find a last track marker.
            if( toc.TrackData[i].TrackNumber == TOC_LAST_TRACK )
            {
                fFoundEndTrack = TRUE;
                break;
            }

            // Break out if we find a data track.
            if( ( toc.TrackData[i].Control & TOC_DATA_TRACK ) != 0 )
            {
                // Knock off 2.5 minutes to account for the final leadin.
                toc.TrackData[i].Address[1] -= 2;
                toc.TrackData[i].Address[2] += 30;

                if( toc.TrackData[i].Address[2] < 60 )
                    toc.TrackData[i].Address[1] -= 1;
                else
                    toc.TrackData[i].Address[2] -= 60;

                m_toc.TrackAddr[i] = TocValToFrames( toc.TrackData[i].Address );

                fFoundEndTrack = TRUE;
                break;
            }
        }

        // If we didn't find an ending track, whether it be a data track or a
        // track with the last track marker, then knock off one track because we
        // don't know how long the last "audio" track might really be.  We can't
        // underflow "i" here because the above code already checked for an
        // empty TOC.
        if( !fFoundEndTrack )
            i--;

        m_toc.LastTrack = i;
    }

    return m_toc.LastTrack > 0;
}

HRESULT CNtIoctlCdromService::Read( DWORD dwReadStart, DWORD dwReadLength, LPVOID pvBuffer, DWORD dwRetries )
{
    RAW_READ_INFO rri;
    HRESULT hr;

    // NT drivers seem to want the disk offset to be (sector - 150) * 2048,
    // regardless of what the actual disk lead-in amount is.
    rri.DiskOffset.QuadPart = (ULONGLONG)( dwReadStart - 150 ) * (ULONGLONG)2048;
    rri.SectorCount = dwReadLength;
    rri.TrackMode = CDDA;

    do
    {
        hr = DeviceIoControl( IOCTL_CDROM_RAW_READ, &rri, sizeof( rri ), pvBuffer, CDAUDIO_BYTES_PER_FRAME * dwReadLength );
        if( SUCCEEDED(hr) )
            break;
    }
    while( dwRetries-- );

    return hr;
}

HRESULT CNtIoctlCdromService::DeviceIoControl( DWORD dwControlCode, LPVOID pvInBuffer, DWORD dwInBufferSize, LPVOID pvOutBuffer, DWORD dwOutBufferSize, LPDWORD pdwBytesReturned )
{
    DWORD dwBytesReturned;

    if( pdwBytesReturned == NULL )
        pdwBytesReturned = &dwBytesReturned;

    if( !::DeviceIoControl( m_hDevice, dwControlCode, pvInBuffer, dwInBufferSize, pvOutBuffer, dwOutBufferSize, pdwBytesReturned, NULL ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    return S_OK;
}

bool CNtIoctlCdromService::GetTotalLength( int* pnMinutes, int* pnSeconds, int* pnFrames )
{
    if( !IsOpen() )
        return false;

    DWORD dwFrames = m_toc.TrackAddr[GetTrackCount()] - m_toc.TrackAddr[0];
    DWORD dwMsf = FramesToMsf( dwFrames );

    if( pnMinutes != NULL )
        *pnMinutes = MCI_MSF_MINUTE( dwMsf );

    if( pnSeconds != NULL )
        *pnSeconds = MCI_MSF_SECOND( dwMsf );

    if( pnFrames != NULL )
        *pnFrames = MCI_MSF_FRAME( dwMsf );

    return true;
}

bool CNtIoctlCdromService::GetTrackLength( int nTrack, int* pnMinutes, int* pnSeconds, int* pnFrames )
{
    if ( !IsOpen() || ( nTrack < 0 ) || ( nTrack > ( GetTrackCount() - 1 ) ) )
        return false;

    DWORD dwFrames = m_toc.TrackAddr[nTrack + 1] - m_toc.TrackAddr[nTrack];
    DWORD dwMsf = FramesToMsf( dwFrames );

    if( pnMinutes != NULL )
        *pnMinutes = MCI_MSF_MINUTE( dwMsf );

    if( pnSeconds != NULL )
        *pnSeconds = MCI_MSF_SECOND( dwMsf );

    if( pnFrames != NULL )
        *pnFrames = MCI_MSF_FRAME( dwMsf );

    return true;
}

////////////////////////////////////////////////////////////////////////////

XCDROM_TOC::XCDROM_TOC()
{
    Clear();
}

XCDROM_TOC::~XCDROM_TOC()
{
    Delete();
}

void XCDROM_TOC::Delete()
{
    Clear();
}

void XCDROM_TOC::Clear()
{
    LastTrack = 0;
}

int XCDROM_TOC::GetTrackFromFrame( DWORD dwPosition ) const
{
    for( int i = 0; i < LastTrack; i++ )
    {
        if( dwPosition < TrackAddr[i + 1] )
            return i;
    }

    return 0;
}
