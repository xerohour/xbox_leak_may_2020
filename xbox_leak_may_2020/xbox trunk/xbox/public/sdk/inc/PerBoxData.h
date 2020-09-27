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
// Same as GeneratePerBoxData, but hard drive key is supplied.
// Used during repair to preserve hard drive contents.
//
HRESULT WINAPI GeneratePerBoxDataEx (
    IN char* pcInputHardDriveKey, // 16 bytes base64 encoded + 1 = 25 bytes
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
// VerifyPerBoxDataEx makes resetting the region bits optional
//
HRESULT WINAPI VerifyPerBoxData (void);
HRESULT WINAPI VerifyPerBoxDataEx (BOOL fResetRegionBits);

//
// This is a generic API to be used by manufacturing/refurbishment code
// to unlock the hard drive. An optional hard drive key can be passed in.
// If it is not supplied, the key in the EEPROM is used.
//
HRESULT WINAPI UnlockXboxHardDrive (
    IN LPBYTE pbHardDriveKey,
    IN DWORD dwHardDriveKeySize
    );

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
