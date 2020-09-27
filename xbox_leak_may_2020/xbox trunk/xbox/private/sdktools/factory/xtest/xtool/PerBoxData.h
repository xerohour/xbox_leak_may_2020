//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#ifndef __PerBoxData__
#define __PerBoxData__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//
// This is executed on the System Integration Test PC
// It generates the keys, the data blob that needs to be sent to WritePerBoxData,
// and the data that needs to be recorded back to the genealogy database.
//
HRESULT WINAPI GeneratePerBoxData (
    IN DWORD GameRegion,
    IN DWORD TVRegion,
    IN LPBYTE pbMACAddress, // 6 bytes
    IN char* pcXboxSerialNumber, // 12 bytes
    OUT char* pcHardDriveKey, // 16 bytes base64 encoded + 1 = 25 bytes
    OUT char* pcRecoveryKey, // 8 bytes
    OUT DWORD* pdwOnlineKeyVersion,
    OUT char* pcOnlineKey, // 128 bytes base64 encoded + 1 = 173 bytes
    OUT LPBYTE pbOutputBuffer,
    IN OUT DWORD* pdwOutputBufferSize
    );

//
// This is executed on the Xbox during System Integration Test
// It will initialize the contents of the EEPROM
//
HRESULT WINAPI WritePerBoxData(
    IN LPBYTE pbInputBuffer,
    IN DWORD dwInputBufferSize
    );

//
// This is executed on the Xbox during Extended Test
// It should be the last thing called before the Xbox leaves the factory
// because it clear the factory bit in the game region and no other
// factory code can be executed on the Xbox anymore.
// This routine will also lock the hard drive.
//
HRESULT WINAPI VerifyPerBoxData ();

//
// This is a generic API to be used by manufacturing/refurbishment code
// to unlock the hard drive. An optional hard drive key can be passed in.
// If it is not supplied, the key in the EEPROM is used.
//
HRESULT WINAPI UnlockXboxHardDrive (
    IN LPBYTE pbHardDriveKey,
    IN DWORD dwHardDriveKeySize
    );


// Define the fatal error codes.  The bitmap of UEMs stored
// in EEPROM starts at HDD_NOT_LOCKED being at bit zero.  
// CORE_DIGITAL and BAD_RAM are values used by the SMC.
//

#define FATAL_ERROR_NONE                0x00
#define FATAL_ERROR_CORE_DIGITAL        0x01
#define FATAL_ERROR_BAD_EEPROM          0x02
#define FATAL_ERROR_UNUSED1             0x03    
#define FATAL_ERROR_BAD_RAM             0x04
#define FATAL_ERROR_HDD_NOT_LOCKED      0x05
#define FATAL_ERROR_HDD_CANNOT_UNLOCK   0x06
#define FATAL_ERROR_HDD_TIMEOUT         0x07
#define FATAL_ERROR_HDD_NOT_FOUND       0x08
#define FATAL_ERROR_HDD_BAD_CONFIG      0x09
#define FATAL_ERROR_DVD_TIMEOUT         0x0A
#define FATAL_ERROR_DVD_NOT_FOUND       0x0B
#define FATAL_ERROR_DVD_BAD_CONFIG      0x0C
#define FATAL_ERROR_XBE_DASH_GENERIC    0x0D
#define FATAL_ERROR_XBE_DASH_ERROR      0x0E
#define FATAL_ERROR_UNUSED2             0x0F
#define FATAL_ERROR_XBE_DASH_SETTINGS   0x10
#define FATAL_ERROR_UNUSED3             0x11
#define FATAL_ERROR_UNUSED4             0x12
#define FATAL_ERROR_UNUSED5             0x13
#define FATAL_ERROR_XBE_DASH_X2_PASS    0x14
#define FATAL_ERROR_REBOOT_ROUTINE      0x15
#define FATAL_ERROR_RESERVED            0xFF

//
// If the system failed to boot immediately before this successful boot, then
// the fatal error code from the last unsuccessful boot can be obtained
// with this API call.  Only true in Manufacturing region.  
//

HRESULT WINAPI QueryFatalErrorLastCode(
    OUT LPDWORD pdwFatalErrorCode
    );

//
// Returns a bitmap where the bits correspond to the fatal error codes.  If
// a fatal error has ever occurred on this system then the corresponding bit 
// will be set in this history DWORD
//
HRESULT WINAPI QueryFatalErrorHistory(
    OUT LPDWORD pdwFatalErrorHistory
    );

//
// Clears the fatal error bitmap which stores the fatal error code.
//
HRESULT WINAPI ClearFatalErrorHistory(
    VOID
    );

//
// In manufacturing region, sets the retry flag for the fatal error (UEM) handling.
// When the flag is TRUE, the system will retry when a fatal error occurs, otherwise
// the UEM is displayed
//
HRESULT WINAPI SetFatalErrorRetryFlag(
    IN BOOL Retry
    );


#ifdef __cplusplus
}
#endif // __cplusplus

#endif
