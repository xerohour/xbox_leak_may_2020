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

#include "memarpch.h"
#include "memoryunit.h"

// Contructors and Destructor
CMemoryUnit::CMemoryUnit()
: CMemoryArea(),
m_pwszMUName( NULL ),
m_bMounted( FALSE ),
m_bFormatted( FALSE ),
m_dwPort( 0 ),
m_dwSlot( 0 )
{
}


CMemoryUnit::~CMemoryUnit()
{
    // If we have set the name, let's clean up the memory
    if( m_pwszMUName )
    {
        delete[] m_pwszMUName;
        m_pwszMUName = NULL;
    }
}


// Format the Memory Unit
HRESULT CMemoryUnit::Format( BOOL bMount/*=FALSE*/ )
{
    // Format the MU
    ANSI_STRING ostMU;
    char pszMU[64];
    ostMU.Length = 0;
    ostMU.MaximumLength = 63;
    ostMU.Buffer = pszMU;

    // Check to see if the MU is mounted.  If it is, we MUST unmount it BEFORE we format it
    if( IsMounted() )
    {
        if( FAILED( Unmount() ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Format():Failed to Unmount the MU!!" );

            return E_FAIL;
        }
    }

    // Open the MU
    BOOL bFormatSuccessful = FALSE;
    if( NT_SUCCESS( MU_CreateDeviceObject( m_dwPort, m_dwSlot, &ostMU ) ) )
    //
    // We created the device object, begin the format
    //
    {
        // Perform the format
        bFormatSuccessful = XapiFormatFATVolume( &ostMU );

        // Close the MU
        MU_CloseDeviceObject( m_dwPort, m_dwSlot );
    }
    else
    //
    // There was a problem creating the device object
    //
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Format():Failed to MU_CreateDeviceObject the MU!! - '%d', '%d'", m_dwPort, m_dwSlot );
    }

    if( !bFormatSuccessful )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Format():The Format Failed!!" );

        return E_FAIL;
    }

    // Check to see if our MU should be mounted, and if so, Mount it
    if( bMount )
    {
        if( FAILED( Mount() ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Format():Failed to Mount the newly formatted MU!!" );
        }
    }

    return S_OK;
}


#define EXTRA_SIZE 4096
#define SECTOR_MASK 0xFFFFFF000
#define TEST_BUFFER_SIZE (4096*2)    //8KB
#define MU_MEDIA_OFFSET  0

// Unformat the Memory Unit
HRESULT CMemoryUnit::Unformat()
{
    OBJECT_ATTRIBUTES oa;
    HANDLE hVolume;
    IO_STATUS_BLOCK statusBlock;
    ANSI_STRING ostMU;
    unsigned char* pBuffer1;
    char pszMU[64];
    ostMU.Length = 0;
    ostMU.MaximumLength = 63;
    ostMU.Buffer = pszMU;
    HRESULT hr = S_OK;          // Return value

    // Unmount the MU if it is currently mounted
    if( IsMounted() )
    {
        if( FAILED( Unmount() ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Unformat():Failed to Unmount the MU!!" );

            return E_FAIL;
        }
    }


    // Open the MU
    if( NT_SUCCESS( MU_CreateDeviceObject( m_dwPort, m_dwSlot, &ostMU ) ) )
    //
    // The device object was created successfully
    //
    {
        unsigned char* pTemp = pBuffer1 = (unsigned char*) LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, TEST_BUFFER_SIZE+EXTRA_SIZE );

        pBuffer1 = (unsigned char*)((ULONG) pBuffer1 & SECTOR_MASK);
        pBuffer1 += EXTRA_SIZE;

        //
        // fill the buffer with a pattern
        //
        memset( pBuffer1, 0xFF, TEST_BUFFER_SIZE );

        InitializeObjectAttributes(&oa, (POBJECT_STRING) &ostMU, OBJ_CASE_INSENSITIVE, NULL, NULL);

        NTSTATUS status = NtOpenFile(&hVolume,
                                     SYNCHRONIZE | GENERIC_ALL,
                                     &oa,
                                     &statusBlock,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_ALERT);

        if( NT_SUCCESS( status ) )
        //
        // We opened the file (volume) successfully, let's "unformat" it by writing our data to it
        //
        {
            LARGE_INTEGER offset;
            offset.QuadPart = MU_MEDIA_OFFSET;

            status = NtWriteFile(hVolume,
                                 0,
                                 NULL,
                                 NULL,
                                 &statusBlock,
                                 pBuffer1,
                                 TEST_BUFFER_SIZE,
                                 &offset );

            //
            // Dismount the file system
            //
            NtFsControlFile(hVolume, NULL, NULL, NULL, &statusBlock,
                FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0);


            //
            //  Close the volume
            //
            NtClose(hVolume);

            // Free our allocated memory
            LocalFree( pTemp );
        }
        else
        //
        // We failed to open the file (volume)
        //
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Unformat():Failed NtOpenFile!!" );

            hr = E_FAIL;
        }

        // Close the MU
        MU_CloseDeviceObject( m_dwPort, m_dwSlot );
    }
    else
    //
    // We failed to create the Device object, we can't unformat the MU
    //
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Unformat():Failed to MU_CreateDeviceObject the MU!! - '%d', '%d'", m_dwPort, m_dwSlot );

        hr = E_FAIL;
    }

    return hr;
}


// Mount the Memory Unit
HRESULT CMemoryUnit::Mount( BOOL bAsGameTitle/*=FALSE*/ )
{
    // If the MU is already mounted, unmount it
    if( IsMounted() )
    {
        if( FAILED( Unmount() ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Mount():Failed to Unmount the MU!!" );

            return E_FAIL;
        }
    }

    // Mount the MU
    DWORD dwReturnVal = 0;
    if( !bAsGameTitle )
        dwReturnVal = XMountMURoot( m_dwPort, m_dwSlot, &m_cUDataDriveLetter );
    else
        dwReturnVal = XMountMU( m_dwPort, m_dwSlot, &m_cUDataDriveLetter );

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
            XDBGTRC( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Mount():Failed to Mount the MU, it was unformatted!! - '0x%0.8X (%d)'", dwReturnVal, dwReturnVal );

            m_bFormatted = FALSE;
        }
        else
        //
        // There was an error trying to mount, other than "not formatted"
        //
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Mount():Failed to Mount the MU!! - '0x%0.8X (%d)'", dwReturnVal, dwReturnVal );
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
HRESULT CMemoryUnit::Unmount()
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
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Unmount():Failed to Unmount the MU!! - '%X'", dwReturnVal );

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
    m_cUDataDriveLetter = '\0';

    return S_OK;
}


// Refresh the name of the Memory Unit (it could change)
HRESULT CMemoryUnit::RefreshName()
{
    // If we have set the name, let's clean up the memory
    if( m_pwszMUName )
    {
        delete[] m_pwszMUName;
        m_pwszMUName = NULL;
    }

    // Get the name of the MU
    WCHAR pwszMUName[MAX_MUNAME + 1];
    ZeroMemory( pwszMUName, sizeof( WCHAR ) * MAX_MUNAME + 1 );

    DWORD dwReturnVal = XMUNameFromDriveLetter( m_cUDataDriveLetter, pwszMUName, MAX_MUNAME );
    if( ERROR_SUCCESS != dwReturnVal )
    // There was a problem getting the MU Name
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::RefreshName():Failed to Get the MU Name!! - '0x%08X (%d)'", dwReturnVal, dwReturnVal );
    }
    else    // We got the MU Name, let's copy it to our own memory
    {
        unsigned int uiStrLen = wcslen( pwszMUName );
        m_pwszMUName = new WCHAR[uiStrLen + 1];
        if( NULL == m_pwszMUName )
        {
            XDBGERR( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::RefreshName():Failed to Allocate Memory!!" );
            return E_FAIL;
        }

        wcscpy( m_pwszMUName, pwszMUName );
    }

    return S_OK;
}

// Name the Memory Unit
HRESULT CMemoryUnit::Name( WCHAR* wpszName )
{
    if( !wpszName )
    {
        return E_INVALIDARG;
    }

    if( wcslen( wpszName ) >= MAX_MUNAME )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Name():Name to long!! Length - '%d', Max Length - '%d'", wcslen( wpszName ), MAX_MUNAME - 1 );

        return E_INVALIDARG;
    }

    // If we have set the name, let's clean up the memory
    if( m_pwszMUName )
    {
        delete[] m_pwszMUName;
        m_pwszMUName = NULL;
    }

    // Write the MU Name to the Drive Letter
    HRESULT hr = S_OK;

    DWORD dwRetVal = XMUWriteNameToDriveLetter( GetUDataDrive(), wpszName );
    if( ERROR_SUCCESS != dwRetVal )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Name():Failed to Write the MU Name!! - '0x%X.8'", dwRetVal );

        hr = E_FAIL;
    }
    else
    {
        unsigned int uiStrLen = wcslen( wpszName );
        m_pwszMUName = new WCHAR[uiStrLen + 1];
        if( NULL == m_pwszMUName )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::Name():Failed to allocate memory!!" );

            hr = E_OUTOFMEMORY;
        }
        else
        {
            m_pwszMUName[uiStrLen] = '\0';

            wcscpy( m_pwszMUName, wpszName );
        }
    }

    return hr;
}


// Get the total number of blocks on the MU
DWORD CMemoryUnit::GetTotalBlocks()
{
    // Check to see if the MU is mounted, if not, return 0
    if( !IsMounted() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::GetTotalBlocks():The MU was not mounted -- Cannot retrieve space information!!" );

        return 0;
    }

    return CMemoryArea::GetTotalBlocks();
}


// Get the number of blocks that are free on the MU
DWORD CMemoryUnit::GetFreeBlocks()
{
    // Check to see if the MU is mounted, if not, return 0
    if( !IsMounted() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::GetFreeBlocks():The MU was not mounted -- Cannot retrieve space information!!" );

        return 0;
    }

    return CMemoryArea::GetFreeBlocks();
}


// Get the current MU Name
HRESULT CMemoryUnit::GetName( WCHAR* wpszNameBuffer, unsigned int uiBufSize )
{
    // Check to make sure a valid parameter was passed in
    if( !wpszNameBuffer )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::GetName():Invalid parameter passed in!!" );

        return E_INVALIDARG;
    }

    // Set the return buffer to Nothing
    wpszNameBuffer[0] = L'\0';

    // Check to make sure this MU has a name, otherwise do nothing and return
    if( m_pwszMUName )
    //
    // Yes, this MU has a name
    //
    {
        // Copy the current name to the callers buffer (lstrcpynW always null
        // terminates the buffer unlike wcsncpy)
        lstrcpynW( wpszNameBuffer, m_pwszMUName, uiBufSize );
    }

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
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::SetPortSlot():Invalid parameter!!" );

        return E_INVALIDARG;
    }

    // We add 4 characters to fit the space, number/letter combo, and the NULL
    unsigned int uiStrLen = wcslen( MEMORY_UNIT_NAME_PATTERN ) + 4;
    WCHAR* pwszDefName = new WCHAR[uiStrLen];
    if( NULL != pwszDefName )
    {
        ZeroMemory( pwszDefName, sizeof( WCHAR ) * uiStrLen );

        swprintf( pwszDefName, L"%ls %c%c", MEMORY_UNIT_NAME_PATTERN, ( dwPort + 1 ) + '0', XDEVICE_TOP_SLOT == dwSlot ? 'A' : 'B' );

        XDBGTRC( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::SetPortSlot():Default Name - '%ls'", pwszDefName );

        HRESULT hr = SetDefaultName( pwszDefName );
        if( FAILED( hr ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::SetPortSlot():Failed to set the default name!! - '%s', '0x%.8X (%d)'", pwszDefName, hr, hr );
        }

        // Clean up
        delete[] pwszDefName;
        pwszDefName = NULL;
    }
    else
    //
    // We failed to allocate memory for our Temp Default Name var
    //
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::SetPortSlot():Failed to allocate memory for our Default Name!!" );
    }

    m_dwPort = dwPort;
    m_dwSlot = dwSlot;

    return S_OK;
}

// Used to determine if the MU is named
BOOL CMemoryUnit::IsNamed()
{
    if( ( NULL != m_pwszMUName ) && ( L'\0' != m_pwszMUName[0] ) )
    {
        return TRUE;
    }

    return FALSE;
}


// Get the Index of the current MU
unsigned int CMemoryUnit::GetIndex()
{
    return ( ( m_dwPort * 2 ) + ( m_dwSlot ) );
}


// Process a mounted MU (Get the name, etc)
HRESULT CMemoryUnit::ProcessMountedMU()
{
    // Get the name of the MU
    WCHAR pwszMUName[MAX_MUNAME + 1];
    ZeroMemory( pwszMUName, sizeof( WCHAR ) * MAX_MUNAME + 1 );

    DWORD dwReturnVal = XMUNameFromDriveLetter( m_cUDataDriveLetter, pwszMUName, MAX_MUNAME );
    if( ERROR_SUCCESS != dwReturnVal )
    // There was a problem getting the MU Name
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::ProcessMountedMU():Failed to Get the MU Name!! - '0x%.8X (%d)'", dwReturnVal, dwReturnVal );
    }
    else    // We got the MU Name, let's copy it to our own memory
    {
        unsigned int uiStrLen = wcslen( pwszMUName );
        m_pwszMUName = new WCHAR[uiStrLen + 1];
        if( NULL == m_pwszMUName )
        {
            XDBGERR( MEMAREA_APP_TITLE_NAME_A, "CMemoryUnit::ProcessMountedMU():Failed to Allocate Memory!!" );

            return E_FAIL;
        }

        wcscpy( m_pwszMUName, pwszMUName );
    }

    return S_OK;
}


// Map the UDATA drive to it's orignal location
HRESULT CMemoryUnit::MapUDataToOrigLoc()
{
    Unmount();
    Mount();

    return S_OK;
}


// Map the UDATA drive letter to the specified Title ID and Mount the MU
HRESULT CMemoryUnit::MapUDataToTitleID( char* pszTitleID )
{
    if( NULL == pszTitleID )
    {
        return E_INVALIDARG;
    }

    // Store our current Title ID, so we can set this to be correct when we are finished
    DWORD dwOriginalTitleID = XeImageHeader()->Certificate->TitleID;

    // Convert the passed in String to a DWORD
    DWORD dwTitleID = strtoul( pszTitleID, NULL, 16 );
    XeImageHeader()->Certificate->TitleID = dwTitleID;

    // Mount the MU as the Title
    HRESULT hr = Mount( TRUE );

    // Set the TitleID back to it's original value
    XeImageHeader()->Certificate->TitleID = dwOriginalTitleID;

    return hr;
}


// Map the UDATA drive letter to the specified Title ID and Mount the MU
HRESULT CMemoryUnit::MapUDataToTitleID( WCHAR* pwszTitleID )
{
    if( NULL == pwszTitleID )
    {
        return E_INVALIDARG;
    }

    // Store our current Title ID, so we can set this to be correct when we are finished
    DWORD dwOriginalTitleID = XeImageHeader()->Certificate->TitleID;

    // Convert the passed in String to a DWORD
    DWORD dwTitleID = wcstoul( pwszTitleID, NULL, 16 );
    XeImageHeader()->Certificate->TitleID = dwTitleID;

    // Mount the MU as the Title
    HRESULT hr = Mount( TRUE );

    // Set the TitleID back to it's original value
    XeImageHeader()->Certificate->TitleID = dwOriginalTitleID;

    return hr;
}


// Map the UDATA drive letter to the specified Title ID and Mount the MU
HRESULT CMemoryUnit::MapUDataToTitleID( DWORD dwTitleID )
{
    // Store our current Title ID, so we can set this to be correct when we are finished
    DWORD dwOriginalTitleID = XeImageHeader()->Certificate->TitleID;

    XeImageHeader()->Certificate->TitleID = dwTitleID;

    HRESULT hr = Mount( TRUE );

    // Set the TitleID back to it's original value
    XeImageHeader()->Certificate->TitleID = dwOriginalTitleID;

    return hr;
}
