//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#ifndef _2KID_UTILS_H_
#define _2KID_UTILS_H_

#include <windows.h>
#include <cryptkeys.h>
// This typdef is for xconfig.h
typedef long NTSTATUS;
// This is needed so xconfig.h will work
#include <init.h>
#include <xconfig.h>
#include <av.h>
#include <base64.h>
#include <list.h>



typedef struct 
_GENPERBOXDATA_INFO {
   
   HRESULT hr;
   BYTE byteData[175];
   DWORD GameRegion;
   DWORD AVRegion;
   BYTE MACAddress[10];
   BYTE MACAddressSmall[2];
   char XboxSerialNumber[20];
   char RecoveryKey[12];
   DWORD dwOnlineKeyVersion;
   char OnlineKeyEncryptedBase64Encoded[173];
   BYTE OnlineKeyEncrypted[129];
   BYTE OnlineKey[17];
   char OnlineKeyDataBaseInfo[129];
   DWORD ulOnlineKey;
   char HardDriveKeyBase64Encoded[25];
   DWORD dwOutBufferSize;

} GENPERBOXDATA_INFO, *LPGENPERBOXDATA_INFO;


class C2kIdUtils {
   

   public:
      VOID 
      WriteFileFromMemory( LPCSTR lpFileName, LPVOID pvFileContents, DWORD dwFileSize );
      VOID 
      FileToDrive( LPCSTR lpFileName, LPVOID pvFileContents, DWORD dwFileSize,DWORD dwWriteFlags );
      BOOL
      ByteCmp(IN LPBYTE First, 
              IN LPBYTE Second, 
              IN DWORD  dwSize);
      BOOL
      DumpBytesHex(IN LPBYTE lpbID,
                IN UINT   uiSize);
      BOOL
      DumpBytesChar(IN LPBYTE lpbID,
                    IN UINT   uiSize);

      VOID
      ResetVariables( IN LPGENPERBOXDATA_INFO x);
      
      PCHAR
      AvRegionBitTest( IN DWORD dwBitMask);

      HRESULT
      GenerateID ( LPBYTE lpbData, DWORD dwSize );
   
      HRESULT
      ReadFileIntoBuffer (IN char* szFileName, 
                          IN BYTE* bFile,
                          IN DWORD dwSizeOfBuffer);
      BOOL
      ScanInformation( IN PCHAR szDirectory, 
                       IN PCHAR inSerialNumber,
                       IN PCHAR inOnlineKey );
      BOOL
      ScanInformationXLS( IN PCHAR szDirectory, 
                          IN PCHAR inSerialNumber,
                          IN PCHAR inOnlineKey );

   C2kIdUtils(){}
   ~C2kIdUtils(){}
   private:

};

#endif
