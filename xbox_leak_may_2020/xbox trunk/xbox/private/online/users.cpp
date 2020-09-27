/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:        xonline.cpp
 *  Content:    Implementation of Xbox online API
 *
 *  History:
 *   Date        By            Reason
 *   ====        ==            ======
 * 10/20/00      tonychen     Created
 *  7/25/01      t-ankurm     MU support added
 ========================================================================== */

#include "xonp.h"
#include "xonver.h"
#include <cryptkeys.h>
#include <fat.h>
#include <xboxp.h>

VOID CXo::CompleteRecentLogonTimeCacheOpenTask()
{
    HRESULT hr;

    if (m_dwLogonTimeCacheState == XON_CACHE_STATE_OPENING)
    {    
        //
        // busy loop until the task is done
        //
        do
        {
            hr = XOnlineTaskContinue( m_hLogonTimeCacheTask );
        } while (hr == XONLINETASK_S_RUNNING);

        if (FAILED(hr))
        {
            if (m_hLogonTimeCacheTask != NULL)
            {
                XOnlineTaskClose(m_hLogonTimeCacheTask);
                m_hLogonTimeCacheTask = NULL;
            }
            
            if (m_hLogonTimeCache != NULL)
            {
                CacheClose(m_hLogonTimeCache);
                m_hLogonTimeCache = NULL;
            }
            m_dwLogonTimeCacheState = XON_CACHE_STATE_ERROR;
        }
        else
        {
            m_dwLogonTimeCacheState = XON_CACHE_STATE_OPENED;
        }
    }
}

//
// Copy user into destination and fix things up
//
VOID CXo::CopyAndAdjustUser(
    IN PXC_ONLINE_USER_ACCOUNT_STRUCT pUserAccount,
    IN DWORD dwMUOptions,
    IN OUT PXONLINEP_USER pUser
    )
{
    RtlCopyMemory( pUser, pUserAccount, sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) );
    pUser->xuid.dwUserFlags = 0;
    pUser->hr = 0;
    pUser->index = 0;
    if (dwMUOptions & XONLINE_USER_OPTION_CAME_FROM_MU)
    {
        DecryptKeyWithMUKey( pUser->key, sizeof(pUser->key) );
        pUser->dwUserOptions |= dwMUOptions;
    }
    else
    {
        DecryptKeyWithHardDriveKeyEx( GetHdKey(), pUser->key, sizeof(pUser->key) );
    }
}

VOID CXo::CollectValidUser(
    IN PXC_ONLINE_USER_ACCOUNT_STRUCT pUserAccount,
    IN DWORD dwMUOptions,
    IN OUT FILETIME* pLastLogon,
    IN OUT PXONLINEP_USER pUsers,
    IN OUT DWORD *pcUsers
    )
{
    HRESULT hr;
    XONLINE_RECENT_LOGON_TIME recentLogonTime;
    XONLC_CONTEXT cacheContext;
    DWORD i,j;

    //
    // Check if user valid
    //
    if (!VerifyOnlineUserSignature(pUserAccount))
    {
        return;
    }
    
    //
    // Lookup recent logon time
    //
    RtlZeroMemory(&recentLogonTime, sizeof(recentLogonTime));
    if (m_hLogonTimeCache != NULL)
    {
        RtlZeroMemory(&cacheContext, sizeof(cacheContext));
        recentLogonTime.qwUserID = pUserAccount->xuid.qwUserID;
        cacheContext.pbIndexData = (BYTE*)&recentLogonTime;
        hr = CacheLookup(m_hLogonTimeCache, &cacheContext);
        Assert (FAILED(hr) || !cacheContext.bValidDataInCache || !cacheContext.bCallRetrieveNext);
    }
    
    //
    // See if user already present in array
    //
    for (i=0; i<*pcUsers; ++i)
    {
        if (pUsers[i].xuid.qwUserID == pUserAccount->xuid.qwUserID)
        {
            if (pUsers[i].dwSignatureTime < pUserAccount->dwSignatureTime)
            {
                //
                // Overwrite Old User
                //
                CopyAndAdjustUser(pUserAccount, dwMUOptions, &pUsers[i]);
                TraceSz1( AuthVerbose, "XOnlineGetUsers: newer MU user overwrote HD user %s", pUserAccount->name );
            }
            else
            {
                TraceSz1( AuthVerbose, "XOnlineGetUsers: duplicate (or older) MU user thrown away, using HD user %s", pUserAccount->name );
            }
            return;
        }
    }
    
    //
    // Use insertion sort to return user in order of recent logon
    //
    for (i=0; i<*pcUsers; ++i)
    {
        if (*((ULONGLONG*)&(pLastLogon[i])) < *((ULONGLONG*)&(recentLogonTime.lastLogon)))
        {
            break;
        }
    }
    
    for (j = *pcUsers; j > i; --j)
    {
        RtlCopyMemory( &pUsers[j], &pUsers[j-1], sizeof(XONLINE_USER) );
    }

    CopyAndAdjustUser(pUserAccount, dwMUOptions, &pUsers[i]);
    
    pLastLogon[i] = recentLogonTime.lastLogon;
    
    ++(*pcUsers);
}

HRESULT CXo::XOnlineGetUsers(
    OUT PXONLINE_USER pUsers,
    OUT DWORD *pcUsers
    )
{
    XoEnter("XOnlineGetUsers");
    XoCheck(pUsers != NULL);
    XoCheck(pcUsers != NULL);

#if DBG
    DWORD dwStartTickCount = GetTickCount();
#endif
    HRESULT hr = S_OK;
    NTSTATUS Status;
    DWORD dwMUOptions;
    BYTE abConfigData[CONFIG_DATA_LENGTH];
    FILETIME lastLogon[XONLINE_MAX_STORED_ONLINE_USERS];
    PXC_ONLINE_USER_ACCOUNT_STRUCT userAccounts;
    DWORD dwCurrentDevices;
    DWORD i,j;

    Assert( sizeof(XONLINE_USER) == sizeof(XONLINEP_USER) );
    Assert( sizeof(XONLINE_USER) == sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) + sizeof(HRESULT) + sizeof(DWORD) );

    RtlZeroMemory( pUsers, sizeof(XONLINE_USER) * XONLINE_MAX_STORED_ONLINE_USERS );
    *pcUsers = 0;
    
    userAccounts = (PXC_ONLINE_USER_ACCOUNT_STRUCT)abConfigData;

    CompleteRecentLogonTimeCacheOpenTask();

    for (i=0; i<XONLINE_MAX_HD_ONLINE_USERS/XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++i)
    {
        if (!XNetLoadConfigSector(m_hConfigVolume, BASE_USER_ACCOUNT_CONFIG_SECTOR + i, abConfigData, CONFIG_DATA_LENGTH))
        {
            RtlZeroMemory( abConfigData, CONFIG_DATA_LENGTH );
        }

        for (j=0; j<XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++j)
        {
            CollectValidUser(&userAccounts[j], 0, lastLogon, (PXONLINEP_USER)pUsers, pcUsers);
        }
    }

#ifdef XONLINE_FEATURE_XBOX
    dwCurrentDevices = XPeekDevices( XDEVICE_TYPE_MEMORY_UNIT, NULL, NULL);

    for (i=0; i<XGetPortCount(); ++i)
    {
        for (j=0; j<2; ++j)
        {
            if (dwCurrentDevices & (1 << (i + j*16)))
            {
                Status = XReadMUMetaData( i, j, &userAccounts[0], FIELD_OFFSET(FAT_VOLUME_METADATA, OnlineData), sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) );
                if (NT_SUCCESS(Status))
                {
                    dwMUOptions = XONLINE_USER_OPTION_CAME_FROM_MU | (i << XONLINE_USER_OPTION_MU_PORT_SHIFT) | (j << XONLINE_USER_OPTION_MU_SLOT_SHIFT);
                    CollectValidUser(&userAccounts[0], dwMUOptions, lastLogon, (PXONLINEP_USER)pUsers, pcUsers);
                }
            }
        }
    }
#endif

#if DBG
    TraceSz2(AuthVerbose,"XOnlineGetUsers: took %d ms to return %d users", GetTickCount() - dwStartTickCount, *pcUsers);
#endif

    return(XoLeave(hr));
}

HRESULT CXo::_XOnlineGetUsersFromHD(
    OUT PXONLINE_USER pUsers,
    OUT DWORD *pcUsers
    )
{
    XoEnter("_XOnlineGetUsersFromHD");
    XoCheck(pUsers != NULL);
    XoCheck(pcUsers != NULL);

    BYTE abConfigData[CONFIG_DATA_LENGTH];
    PXC_ONLINE_USER_ACCOUNT_STRUCT userAccounts;
    DWORD i,j;

    RtlZeroMemory( pUsers, sizeof(XONLINE_USER) * XONLINE_MAX_HD_ONLINE_USERS );
    *pcUsers = 0;
    
    userAccounts = (PXC_ONLINE_USER_ACCOUNT_STRUCT)abConfigData;

    for (i=0; i<XONLINE_MAX_HD_ONLINE_USERS/XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++i)
    {
        if (!XNetLoadConfigSector(m_hConfigVolume, BASE_USER_ACCOUNT_CONFIG_SECTOR + i, abConfigData, CONFIG_DATA_LENGTH))
        {
            RtlZeroMemory( abConfigData, CONFIG_DATA_LENGTH );
        }

        for (j=0; j<XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++j)
        {
            if (VerifyOnlineUserSignature(&userAccounts[j]))
            {
                CopyAndAdjustUser(&userAccounts[j], 0, (PXONLINEP_USER)&pUsers[*pcUsers]);
                ++(*pcUsers);
            }
        }
    }

    return(XoLeave(S_OK));
}

HRESULT CXo::_XOnlineGetUserFromMU (
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT PXONLINE_USER pUser
    )
{
    XoEnter("_XOnlineGetUserFromMU");
    XoCheck((dwPort < XGetPortCount()) && (dwSlot < 2));
    XoCheck(pUser != NULL);

#ifdef XONLINE_FEATURE_XBOX

    HRESULT hr = XONLINE_E_NO_USER;
    NTSTATUS Status;
    DWORD dwMUOptions;
    XC_ONLINE_USER_ACCOUNT_STRUCT userAccount;

    RtlZeroMemory( pUser, sizeof(XONLINE_USER) );

    Status = XReadMUMetaData( dwPort, dwSlot, &userAccount, FIELD_OFFSET(FAT_VOLUME_METADATA, OnlineData), sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) );
    if (NT_SUCCESS(Status))
    {
        if (VerifyOnlineUserSignature(&userAccount))
        {
            dwMUOptions = XONLINE_USER_OPTION_CAME_FROM_MU | (dwPort << XONLINE_USER_OPTION_MU_PORT_SHIFT) | (dwSlot << XONLINE_USER_OPTION_MU_SLOT_SHIFT);
            CopyAndAdjustUser(&userAccount, dwMUOptions, (PXONLINEP_USER)pUser);
            hr = S_OK;
        }
    }

    return(XoLeave(hr));
#else
    return(XoLeave(E_NOTIMPL));
#endif
}

BOOL CXo::_XOnlineIsUserCredentialPresent(
    IN PXONLINE_USER pUser
    )
{
#ifdef XONLINE_FEATURE_XBOX
    NTSTATUS Status;
    XC_ONLINE_USER_ACCOUNT_STRUCT userAccount;
    DWORD dwCurrentDevices;
    DWORD i,j;

    //
    // All user from HD are present
    //
    if ((pUser->dwUserOptions & XONLINE_USER_OPTION_CAME_FROM_MU) == 0)
    {
        return TRUE;
    }

    dwCurrentDevices = XPeekDevices( XDEVICE_TYPE_MEMORY_UNIT, NULL, NULL);

    for (i=0; i<XGetPortCount(); ++i)
    {
        for (j=0; j<2; ++j)
        {
            if (dwCurrentDevices & (1 << (i + j*16)))
            {
                Status = XReadMUMetaData( i, j, &userAccount, FIELD_OFFSET(FAT_VOLUME_METADATA, OnlineData), sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) );
                if (NT_SUCCESS(Status))
                {
                    if (VerifyOnlineUserSignature(&userAccount))
                    {
                        if (RtlEqualMemory(userAccount.signature, ((PXONLINEP_USER)pUser)->signature, sizeof(userAccount.signature)))
                        {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    return FALSE;
#else
    return TRUE;
#endif
}

BOOL CXo::SetMachineAccount(
    IN PXONLINEP_USER pUser
    )
{
    HRESULT hr = S_OK;
    XC_ONLINE_MACHINE_ACCOUNT_STRUCT* pMachineAccount;
    BYTE abConfigData[CONFIG_DATA_LENGTH];
    
    //
    // Pull out information about the machine account from config sector
    //
    if (!XNetLoadConfigSector(m_hConfigVolume, MACHINE_ACCOUNT_CONFIG_SECTOR, abConfigData, sizeof(abConfigData)))
    {
        RtlZeroMemory( abConfigData, sizeof(abConfigData) );
    }

    pMachineAccount = (XC_ONLINE_MACHINE_ACCOUNT_STRUCT*)abConfigData;
    
    RtlCopyMemory( pMachineAccount, pUser, sizeof(*pMachineAccount) );

    //
    // Encrypt Key and save to Config Sector on HD
    //
    EncryptKeyWithHardDriveKeyEx( GetHdKey(), pMachineAccount->key, sizeof(pMachineAccount->key) );

    //
    // Sign the struct signature
    //
    SignOnlineUserStruct( pMachineAccount );

    //
    // Store it back to the config sector
    //
    if (!XNetSaveConfigSector(m_hConfigVolume, MACHINE_ACCOUNT_CONFIG_SECTOR, abConfigData, sizeof(abConfigData)))
    {
        return FALSE;
    }
    
    return TRUE;
}



HRESULT CXo::_XOnlineAddUserToHD(
    IN PXONLINE_USER pUser
    )
{
    XoEnter("_XOnlineAddUserToHD");
    XoCheck(pUser != NULL && pUser->xuid.qwUserID != 0);

    HRESULT hr = S_OK;
    BYTE abConfigData[XONLINE_MAX_HD_ONLINE_USERS/XONLINE_MAX_ONLINE_USERS_PER_SECTOR][CONFIG_DATA_LENGTH];
    PXC_ONLINE_USER_ACCOUNT_STRUCT userAccounts;
    #define NO_EMPTY_SLOT_FOUND 0xFFFFFFFF
    DWORD dwEmptySector;
    DWORD dwEmptySlot = NO_EMPTY_SLOT_FOUND;
    DWORD i,j;

    for (i=0; i<XONLINE_MAX_HD_ONLINE_USERS/XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++i)
    {
        if (!XNetLoadConfigSector(m_hConfigVolume, BASE_USER_ACCOUNT_CONFIG_SECTOR + i, abConfigData[i], CONFIG_DATA_LENGTH))
        {
            RtlZeroMemory( abConfigData[i] , CONFIG_DATA_LENGTH );
        }
        userAccounts = (PXC_ONLINE_USER_ACCOUNT_STRUCT)abConfigData[i];

        for (j=0; j<XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++j)
        {
            if (!VerifyOnlineUserSignature(&userAccounts[j]))
            {
                if (dwEmptySlot == NO_EMPTY_SLOT_FOUND)
                {
                    dwEmptySector = i;
                    dwEmptySlot = j;
                }
            }
            else if (userAccounts[j].xuid.qwUserID == pUser->xuid.qwUserID)
            {
                //
                // Overwrite if account already exists
                //
                dwEmptySector = i;
                dwEmptySlot = j;
            }
        }
    }

    if (dwEmptySlot != NO_EMPTY_SLOT_FOUND)
    {
        userAccounts = (PXC_ONLINE_USER_ACCOUNT_STRUCT)abConfigData[dwEmptySector];

        RtlCopyMemory( &userAccounts[dwEmptySlot], pUser, sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) );

        EncryptKeyWithHardDriveKeyEx( GetHdKey(), userAccounts[dwEmptySlot].key, sizeof(userAccounts[dwEmptySlot].key) );
        SignOnlineUserStruct(&userAccounts[dwEmptySlot]);

#if DBG
        FILETIME CurrentTime;
    	GetSystemTimeAsFileTime(&CurrentTime);
        Assert( *((ULONGLONG*)&CurrentTime) > BASE_SIGNATURE_TIME );
        Assert( ((*((ULONGLONG*)&CurrentTime) - BASE_SIGNATURE_TIME) / SIGNATURE_TIME_INCREMENT) <= ((ULONGLONG)0xFFFFFFFF) );
#endif

        if (!XNetSaveConfigSector(m_hConfigVolume, BASE_USER_ACCOUNT_CONFIG_SECTOR + dwEmptySector, abConfigData[dwEmptySector], CONFIG_DATA_LENGTH))
        {
            Assert(!"XNetSaveConfigSector failed");
            hr = E_FAIL;
        }
    }
    else
    {
        //
        // No more space left to add new user
        //
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

Cleanup:

    return(XoLeave(hr));
}

HRESULT CXo::_XOnlineSetUserInMU(
    IN CHAR chDrive,
    IN PXONLINE_USER pUser
    )
{
    XoEnter("_XOnlineSetUserInMU");
    XoCheck(('F' <= chDrive && chDrive <= 'M') || ('f' <= chDrive && chDrive <= 'm'));
    XoCheck(pUser != NULL);

#ifdef XONLINE_FEATURE_XBOX
    NTSTATUS Status;
    OCHAR szDosDevice[8];
    OBJECT_STRING DosDevice;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    HRESULT hr = S_OK;
    IO_STATUS_BLOCK IoStatusBlock;
    FSCTL_VOLUME_METADATA VolumeMetadata;
    XC_ONLINE_USER_ACCOUNT_STRUCT userAccount;
    FAT_VOLUME_METADATA MUMetaData;
    BOOL fSetMUVolumeName;
    XC_ONLINE_USER_ACCOUNT_STRUCT* pOrigMUUser;
    DWORD i;
    
    //
    // Open a handle to the volume or directory of the drive.
    //
    soprintf(szDosDevice, OTEXT("\\??\\%c:"), chDrive);
    RtlInitObjectString(&DosDevice, szDosDevice);

    InitializeObjectAttributes(
        &Obja,
        (POBJECT_STRING) &DosDevice,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtOpenFile(&Handle,
                        SYNCHRONIZE | GENERIC_WRITE,
                        &Obja,
                        &IoStatusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        FILE_SYNCHRONOUS_IO_ALERT);

    if (!NT_SUCCESS(Status))
    {
        TraceSz( AuthWarn, "MU: error -- NtOpenFile");
        return HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
    }

    Assert(sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) <= FAT_ONLINE_DATA_LENGTH);

    VolumeMetadata.ByteOffset = FIELD_OFFSET(FAT_VOLUME_METADATA, VolumeName);
    VolumeMetadata.TransferLength = sizeof(MUMetaData.VolumeName) + sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT);
    VolumeMetadata.TransferBuffer = MUMetaData.VolumeName;

    Status = NtFsControlFile(Handle,
                             NULL,
                             NULL,
                             NULL,
                             &IoStatusBlock,
                             FSCTL_READ_VOLUME_METADATA,
                             &VolumeMetadata,
                             sizeof(VolumeMetadata),
                             NULL,
                             0);
    if (!NT_SUCCESS(Status))
    {
        hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
        goto Cleanup;
    }

    RtlCopyMemory( &userAccount, pUser, sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) );

    pOrigMUUser = (XC_ONLINE_USER_ACCOUNT_STRUCT*)MUMetaData.OnlineData;
    
    fSetMUVolumeName = (MUMetaData.VolumeName[0] == 0);

    if ( !fSetMUVolumeName )
    {
        for (i=0; i<XONLINE_NAME_SIZE; ++i)
        {
            if (MUMetaData.VolumeName[i] != (WCHAR)pOrigMUUser->name[i])
            {
                break;
            }
            if (pOrigMUUser->name[i] == 0)
            {
                //
                // The names match, overwrite it with new name
                //
                fSetMUVolumeName = TRUE;
                break;
            }
        }
    }

    if (fSetMUVolumeName)
    {
        //
        // Set the MU Name with the user name
        //
        for (i=0; i<XONLINE_NAME_SIZE; ++i)
        {
            MUMetaData.VolumeName[i] = (WCHAR)userAccount.name[i];
        }
    }
    
    if( userAccount.xuid.qwUserID != 0 )
    {
        EncryptKeyWithMUKey( userAccount.key, sizeof(userAccount.key) );
        SignOnlineUserStruct(&userAccount);
    }
    
    RtlCopyMemory( MUMetaData.OnlineData, &userAccount, sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) );

    VolumeMetadata.ByteOffset = FIELD_OFFSET(FAT_VOLUME_METADATA, VolumeName);
    VolumeMetadata.TransferLength = sizeof(MUMetaData.VolumeName) + sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT);
    VolumeMetadata.TransferBuffer = MUMetaData.VolumeName;

    Status = NtFsControlFile(Handle,
                             NULL,
                             NULL,
                             NULL,
                             &IoStatusBlock,
                             FSCTL_WRITE_VOLUME_METADATA,
                             &VolumeMetadata,
                             sizeof(VolumeMetadata),
                             NULL,
                             0);
    hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
    
Cleanup:
    NtClose(Handle);        

    return(XoLeave(hr));
#else
    return(XoLeave(E_NOTIMPL));
#endif
}

HRESULT CXo::_XOnlineRemoveUserFromHD(
    IN PXONLINE_USER pUser
    )
{
    XoEnter("_XOnlineRemoveUserFromHD");
    XoCheck(pUser != NULL && pUser->xuid.qwUserID != 0);

    HRESULT hr;
    BYTE abConfigData[CONFIG_DATA_LENGTH];
    PXC_ONLINE_USER_ACCOUNT_STRUCT userAccounts;
    XC_ONLINE_USER_ACCOUNT_STRUCT* pUserAccount;
    DWORD i,j;
    
    pUserAccount = (XC_ONLINE_USER_ACCOUNT_STRUCT*) pUser;
    
    for (i=0; i<XONLINE_MAX_HD_ONLINE_USERS/XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++i)
    {
        if (!XNetLoadConfigSector(m_hConfigVolume, BASE_USER_ACCOUNT_CONFIG_SECTOR + i, abConfigData, CONFIG_DATA_LENGTH))
        {
            RtlZeroMemory( abConfigData , CONFIG_DATA_LENGTH );
        }
        userAccounts = (PXC_ONLINE_USER_ACCOUNT_STRUCT)abConfigData;

        for (j=0; j<XONLINE_MAX_ONLINE_USERS_PER_SECTOR; ++j)
        {
            if ( userAccounts[j].xuid.qwUserID == pUserAccount->xuid.qwUserID && VerifyOnlineUserSignature(&userAccounts[j]) )
            {
                userAccounts[j].xuid.qwUserID = 0;

                if (!XNetSaveConfigSector(m_hConfigVolume, BASE_USER_ACCOUNT_CONFIG_SECTOR + i, abConfigData, CONFIG_DATA_LENGTH))
                {
                    Assert(!"XNetSaveConfigSector failed");
                    hr = E_FAIL;
                    goto Cleanup;
                }
                
                hr = S_OK;
                goto Cleanup;
            }
        }
    }

    //
    // The user entry was not found
    //
    hr = XONLINE_E_NO_USER;

Cleanup:
    return(XoLeave(hr));
}

typedef struct _XONLINE_PIN_INPUT_STATE
{
    XINPUT_STATE previousState;
} XONLINE_PIN_INPUT_STATE, *PXONLINE_PIN_INPUT_STATE;

XPININPUTHANDLE
CXo::XOnlinePINStartInput(
    IN PXINPUT_STATE pInputState
    )
{
    XoEnter_("XOnlinePINStartInput", NULL);
    XoCheck_(pInputState != NULL, NULL);

    PXONLINE_PIN_INPUT_STATE pPinInputState;
    pPinInputState = (PXONLINE_PIN_INPUT_STATE)SysAlloc(sizeof(XONLINE_PIN_INPUT_STATE), PTAG_XONLINE_PIN_INPUT_STATE);
    if ( pPinInputState == NULL )
    {
        goto error;
    }
    pPinInputState->previousState = *pInputState;
    
error:
    return (XPININPUTHANDLE)pPinInputState;
}

#define BUTTON_LEN  FIELD_OFFSET( XINPUT_GAMEPAD, sThumbLX )

HRESULT
CXo::XOnlinePINDecodeInput(
    IN XPININPUTHANDLE handle,
    IN PXINPUT_STATE pInputState,
    IN PBYTE pPINByte
    )
{
    XoEnter("XOnlinePINDecodeInput");
    XoCheck(handle != NULL);
    XoCheck(pInputState != NULL);
    XoCheck(pPINByte != NULL);

    static XINPUT_GAMEPAD unpressedGamepad = { 0,{0,0,0,0,0,0,0,0},0,0,0,0 };
    
    PXONLINE_PIN_INPUT_STATE pPinInputState = (PXONLINE_PIN_INPUT_STATE) handle;
    HRESULT hr;
    DWORD dwButtonPress;

    if ( pPinInputState->previousState.dwPacketNumber != pInputState->dwPacketNumber )
    {
        TraceSz11( AuthVerbose, "Packet %d -> %d : %.4X %.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
            pPinInputState->previousState.dwPacketNumber,
            pInputState->dwPacketNumber,
            pInputState->Gamepad.wButtons,
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A],
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B],
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X],
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y],
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK],
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE],
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER],
            pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] );
    }

    if ( RtlEqualMemory( &(pPinInputState->previousState.Gamepad), &unpressedGamepad, BUTTON_LEN ) )
    {
        if ( RtlEqualMemory( &(pInputState->Gamepad), &unpressedGamepad, BUTTON_LEN ) )
        {
            // Nothing new pressed
            hr = S_FALSE;
        }
        else
        {
            // New button is pressed, assume a new byte will be output.
            hr = S_OK;
            
            // Check if it is a DPAD button
            dwButtonPress = pInputState->Gamepad.wButtons & 0xf;
            if (dwButtonPress != 0)
            {
                // DPAD button is pressed.
                if (dwButtonPress == XINPUT_GAMEPAD_DPAD_UP)
                {
                    *pPINByte = 1;
                }
                else if (dwButtonPress == XINPUT_GAMEPAD_DPAD_DOWN)
                {
                    *pPINByte = 2;
                }
                else if (dwButtonPress == XINPUT_GAMEPAD_DPAD_LEFT)
                {
                    *pPINByte = 3;
                }
                else if (dwButtonPress == XINPUT_GAMEPAD_DPAD_RIGHT)
                {
                    *pPINByte = 4;
                }
                else
                {
                    // Multiple directions pressed, counts as nothing.
                    hr = S_FALSE;
                }
            }
            else
            {
                // Some analog button is pressed, find first one.
                if (pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > 0)
                {
                    *pPINByte = 5;
                }
                else if (pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 0)
                {
                    *pPINByte = 6;
                }
                else if (pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 0)
                {
                    *pPINByte = 7;
                }
                else if (pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 0)
                {
                    *pPINByte = 8;
                }
                else if (pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0)
                {
                    *pPINByte = 9;
                }
                else if (pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0)
                {
                    *pPINByte = 10;
                }
                else
                {
                    // no analog button pressed, counts as nothing.
                    hr = S_FALSE;
                }
            }
        }
    }
    else
    {
        // Still releasing previous button, no new input byte;
        hr = S_FALSE;
    }

    if ( hr == S_OK )
    {
        TraceSz1( AuthVerbose, "XOnlinePINDecodeInput output %d", *pPINByte );
    }

    pPinInputState->previousState = *pInputState;    

    return(XoLeave(hr));
}

HRESULT
CXo::XOnlinePINEndInput (
    IN XPININPUTHANDLE handle
    )
{
    XoEnter("XOnlinePINEndInput");
    XoCheck(handle != NULL);

    PXONLINE_PIN_INPUT_STATE pPinInputState = (PXONLINE_PIN_INPUT_STATE) handle;

    SysFree(pPinInputState);

    return(XoLeave(S_OK));
}

