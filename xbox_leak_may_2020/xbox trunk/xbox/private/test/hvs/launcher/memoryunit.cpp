/*****************************************************
*** memoryunit.cpp
***
*** CPP file for our Memory Unit class.
*** This class will contain functions and information
*** pertaining to an Xbox Memory Unit
***
*** by James N. Helm
*** March 28th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "memoryunit.h"

// Contructors and Destructor
CMemoryUnit::CMemoryUnit( void ) :
m_cDriveLetter( '\0' ),
m_pwszDefaultName( NULL ),
m_pwszMUName( NULL ),
m_bMounted( FALSE ),
m_bFormatted( FALSE ),
m_dwPort( 0l ),
m_dwSlot( 0l )
{
    m_pszDrivePath[3] = '\0';
}


CMemoryUnit::~CMemoryUnit( void )
{
    if( NULL == m_pwszDefaultName )
    {
        delete[] m_pwszDefaultName;
        m_pwszDefaultName = NULL;
    }

    if( NULL == m_pwszMUName )
    {
        delete[] m_pwszMUName;
        m_pwszMUName = NULL;
    }
}


// Mount the Memory Unit
HRESULT CMemoryUnit::Mount( void )
{
    // If the MU is already mounted, unmount it
    if( IsMounted() )
    {
        if( FAILED( Unmount() ) )
        {
            DebugPrint( "CMemoryUnit::Mount():Failed to Unmount the MU!!\n" );

            return E_FAIL;
        }
    }

    // Mount the MU
    DWORD dwReturnVal = XMountMU( m_dwPort, m_dwSlot, &m_cDriveLetter );
    if( ERROR_SUCCESS != dwReturnVal )
    //
    // There was a problem mounting the MU
    //
    {
        // Check to see if the failure was the result of an unformatted MU
        // If so, mark this MU as Unformatted
        if( ERROR_UNRECOGNIZED_VOLUME == dwReturnVal )
        //
        // Yes, the MU was unformatted, which caused the Mount Failure
        //
        {
            DebugPrint( "CMemoryUnit::Mount():Failed to Mount the MU, it was unformatted!! - '0x%0.8X (%d)'\n", dwReturnVal, dwReturnVal );

            m_bFormatted = FALSE;
        }
        else
        //
        // There was an error trying to mount, other than "not formatted"
        //
        {
            DebugPrint( "CMemoryUnit::Mount():Failed to Mount the MU!! - '0x%0.8X (%d)'\n", dwReturnVal, dwReturnVal );
        }


        return E_FAIL;
    }
    else
    //
    // The MU was mounted successfully
    //
    {
        m_bMounted = TRUE;
        m_bFormatted = TRUE;
    }

    HRESULT hr = ProcessMountedMU();

    return hr;
}


// Unmount the Memory Unit
HRESULT CMemoryUnit::Unmount( void )
{
    // Verify the MU was mounted, if not, bail
    if( !IsMounted() )
    {
        return E_ABORT;
    }

    // Unmount the MU
    DWORD dwReturnVal = XUnmountMU( m_dwPort, m_dwSlot );
    if( ERROR_SUCCESS != dwReturnVal )
    //
    // There was a problem unmounting the MU
    //
    {
        DebugPrint( "CMemoryUnit::Unmount():Failed to Unmount the MU!! - '%X'\n", dwReturnVal );

        return E_FAIL;
    }
    else
    //
    // The MU was unmounted successfully
    {
        m_bMounted = FALSE;
        m_bFormatted = FALSE;
    }

    // Clear out the MU Name
    if( m_pwszMUName )
    {
        delete[] m_pwszMUName;
        m_pwszMUName = NULL;
    }

    // Reset the MU Drive Letter
    m_cDriveLetter = '\0';

    return S_OK;
}


// Get the total number of blocks on the Memory Area
DWORD CMemoryUnit::GetTotalBlocks( void )
{
    // Check to see if the MU is mounted, if not, return 0
    if( !IsMounted() )
    {
        DebugPrint( "CMemoryUnit::GetTotalBlocks():The MU was not mounted -- Cannot retrieve space information!!\n" );

        return 0;
    }

    DWORD dwTotalBlocks, dwFreeBlocks;

    if( FAILED( CalcSpace( &dwTotalBlocks, &dwFreeBlocks ) ) )
    {
        DebugPrint( "CMemoryUnit::GetTotalBlocks():CalcSpace failed!!\n" );
    }

    return dwTotalBlocks;
}


// Get the number of blocks that are free on the Memory Area
DWORD CMemoryUnit::GetFreeBlocks( void )
{
    // Check to see if the MU is mounted, if not, return 0
    if( !IsMounted() )
    {
        DebugPrint( "CMemoryUnit::GetFreeBlocks():The MU was not mounted -- Cannot retrieve space information!!\n" );

        return 0;
    }

    DWORD dwTotalBlocks, dwFreeBlocks;

    if( FAILED( CalcSpace( &dwTotalBlocks, &dwFreeBlocks ) ) )
    {
        DebugPrint( "CMemoryUnit::GetFreeBlocks():CalcSpace failed!!\n" );
    }

    return dwFreeBlocks;
}


// Retreives block information for the current MU
HRESULT CMemoryUnit::CalcSpace( DWORD* pdwTotalBlocks, DWORD* pdwFreeBlocks )
{
    *pdwTotalBlocks = 0;
    *pdwFreeBlocks = 0;

    ULARGE_INTEGER uliFreeBytesAvailable, uliTotalNumBytes, uliTotalNumFreeBytes;

    // Get the disk space information for the MU -- If there was a problem, return 0
    if( !GetDiskFreeSpaceEx( m_pszDrivePath, &uliFreeBytesAvailable, &uliTotalNumBytes, &uliTotalNumFreeBytes ) )
    {
        DebugPrint( "CMemoryUnit::CalcSpace():Failed to 'GetDiskFreeSpaceEx'!! - '%hs', '0x%.8X (%d)'\n", m_pszDrivePath, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    // Calculate the block sizes, and store the values in our parameters
    *pdwTotalBlocks = (DWORD)( uliTotalNumBytes.QuadPart / gc_dwMEMORY_AREA_BLOCK_SIZE_IN_BYTES );
    *pdwFreeBlocks  = (DWORD)( uliTotalNumFreeBytes.QuadPart / gc_dwMEMORY_AREA_BLOCK_SIZE_IN_BYTES );

    return S_OK;
}


// Set the port and the slot this MU will be on
HRESULT CMemoryUnit::SetPortSlot( DWORD dwPort, DWORD dwSlot )
{
    // Check to make sure the parameters are withing the valid range
    if( ( dwPort < XDEVICE_PORT0 ) ||
        ( dwPort > XDEVICE_PORT3 ) ||
        ( ( dwSlot != XDEVICE_TOP_SLOT ) && ( dwSlot != XDEVICE_BOTTOM_SLOT ) ) )
    {
        DebugPrint( "CMemoryUnit::SetPortSlot():Invalid parameter!!\n" );

        return E_INVALIDARG;
    }

    // Set the default name
    if( NULL != m_pwszDefaultName )
    {
        delete[] m_pwszDefaultName;
        m_pwszDefaultName = NULL;
    }

    // We add 4 characters to fit the space, number/letter combo, and the NULL
    unsigned int uiStrLen = wcslen( gc_pwszMEMORY_UNIT_NAME_PATTERN ) + 4;
    m_pwszDefaultName = new WCHAR[uiStrLen];
    if( NULL == m_pwszDefaultName )
        DebugPrint( "Failed to set the Default Name of the MU!!\n" );
    else
        swprintf( m_pwszDefaultName, L"%ls %c%c", gc_pwszMEMORY_UNIT_NAME_PATTERN, ( dwPort + 1 ) + '0', XDEVICE_TOP_SLOT == dwSlot ? 'A' : 'B' );
    
    // DebugPrint( "CMemoryUnit::SetPortSlot():Default Name - '%ls'\n", m_pwszDefaultName );

    m_dwPort = dwPort;
    m_dwSlot = dwSlot;

    return S_OK;
}

// Used to determine if the MU is named
BOOL CMemoryUnit::IsNamed( void )
{
    if( ( NULL != m_pwszMUName ) && ( L'\0' != m_pwszMUName[0] ) )
    {
        return TRUE;
    }

    return FALSE;
}


// Get the Index of the current MU
unsigned int CMemoryUnit::GetIndex( void )
{
    return ( ( m_dwPort * 2 ) + ( m_dwSlot ) );
}


// Process a mounted MU (Get the name, etc)
HRESULT CMemoryUnit::ProcessMountedMU( void )
{
    // Create our drive path from the drive letter
    sprintf( m_pszDrivePath, "%c:\\", m_cDriveLetter );

    // Get the name of the MU
    WCHAR pwszMUName[MAX_MUNAME + 1];
    ZeroMemory( pwszMUName, sizeof( WCHAR) * MAX_MUNAME + 1 );

    DWORD dwReturnVal = XMUNameFromDriveLetter( m_cDriveLetter, pwszMUName, MAX_MUNAME );
    if( ERROR_SUCCESS != dwReturnVal )
        DebugPrint( "CMemoryUnit::ProcessMountedMU():Failed to Get the MU Name!! - '0x%.8X (%d)'\n", dwReturnVal, dwReturnVal );
    else
    //
    // We got the MU Name, let's copy it to our own memory
    //
    {
        if( NULL != m_pwszMUName )
        {
            delete[] m_pwszMUName;
            m_pwszMUName = NULL;
        }

        unsigned int uiStrLen = wcslen( pwszMUName );
        m_pwszMUName = new WCHAR[uiStrLen + 1];
        if( NULL == m_pwszMUName )
        {
            DebugPrint( "CMemoryUnit::ProcessMountedMU():Failed to Allocate Memory!!\n" );

            return E_FAIL;
        }

        wcscpy( m_pwszMUName, pwszMUName );
    }

    return S_OK;
}
