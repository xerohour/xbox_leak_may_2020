//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include "2kId_Utils.h"
#include "..\\include\\fileformat.h"
#include "..\\include\\DataPacket.h"
#include <PerBoxData.h>
#include <stdlib.h>
#include <stdio.h>


VOID
C2kIdUtils::
WriteFileFromMemory(
                   LPCSTR lpFileName,
                   LPVOID pvFileContents,
                   DWORD dwFileSize
                   )
{

   HANDLE hFile;
   DWORD dwBytesWrote;

   hFile = CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   if (hFile == INVALID_HANDLE_VALUE) {
      fprintf(stderr, "Error: unable to open %s\n", lpFileName);
      return;
   }

   if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) ||
       (dwBytesWrote != dwFileSize)) {
      fprintf(stderr, "Error: cannot write to %s\n", lpFileName);
      return;
   }
   CloseHandle(hFile);
}


VOID
C2kIdUtils::
FileToDrive( LPCSTR lpFileName,
             LPVOID pvFileContents,
             DWORD dwFileSize,
             DWORD dwWriteFlags )
{

   HANDLE hFile;
   DWORD dwBytesWrote;
   DWORD dwPos;
   DWORD dwFileStyle;
   CHAR  szOperation[30];
   DWORD dwFilePosition;

   switch (dwWriteFlags) {
      case APPEND_FILE:
         dwFileStyle = OPEN_ALWAYS;
         sprintf(szOperation,"%s","Appending File");
         dwFilePosition = FILE_END;
         break;
      case NEW_FILE:
         dwFileStyle = CREATE_ALWAYS;
         sprintf(szOperation,"%s","New File");
         dwFilePosition = FILE_BEGIN;
         break;
      default:
         dwFileStyle = 0;
         dwFilePosition = FILE_BEGIN;
         break; 
      }
   hFile = CreateFile(lpFileName, 
                      GENERIC_WRITE, 
                      FILE_SHARE_READ, 
                      NULL,
                      dwFileStyle, 
                      FILE_ATTRIBUTE_NORMAL, 
                      NULL);

   if (hFile == INVALID_HANDLE_VALUE) {
      printf("%s:unable to open %s, error %X\n", szOperation,lpFileName, GetLastError());
      return;
   }

   dwPos = SetFilePointer( hFile, 0, NULL, dwFilePosition );
   if ( dwPos == INVALID_SET_FILE_POINTER ) {
      printf("%s: unable to seek %s, error %X\n", szOperation, lpFileName, GetLastError());
      return;        
   }

   if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) || (dwBytesWrote != dwFileSize)) {
      printf("%s: cannot write to %s, error %X\n", szOperation, lpFileName, GetLastError() );
      return;
   }
   CloseHandle(hFile);
}



BOOL
C2kIdUtils::
ByteCmp(IN LPBYTE First, 
        IN LPBYTE Second, 
        IN DWORD  dwSize)
{

   BOOL b = TRUE;
   for (DWORD i =0; i < dwSize; i++) {
      if (First[i] != Second[i]) {
         b = FALSE;
      }
   }
   return b;
}

BOOL
C2kIdUtils::
DumpBytesHex(IN LPBYTE lpbID,
             IN UINT   uiSize)
{
   
   for ( UINT i = 0; i < uiSize; i++ ) {
      printf("%02X", lpbID[i]);
   }
   printf("\n");
   return TRUE;
}

BOOL
C2kIdUtils::
DumpBytesChar(IN LPBYTE lpbID,
              IN UINT   uiSize)
{
   
   for ( UINT i = 0; i < uiSize; i++ ) {
      printf("%C", lpbID[i]);
   }
   printf("\n");
   return TRUE;
}



VOID
C2kIdUtils::
ResetVariables( IN LPGENPERBOXDATA_INFO x)
{

   INT i;

   x->hr = 0;
   RtlZeroMemory(x->byteData, sizeof(x->byteData));
   x->GameRegion = XC_GAME_REGION_NA;
   x->AVRegion   = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
   RtlZeroMemory(x->MACAddress, sizeof(x->MACAddress));
   for (i =0; i < 6; i++) x->MACAddress[i] = 0x66;
     
   RtlZeroMemory(x->MACAddressSmall, sizeof(x->MACAddressSmall));
   for (i =0; i < 2; i++) x->MACAddressSmall[i] = 0x66;

   RtlZeroMemory(x->XboxSerialNumber, sizeof(x->XboxSerialNumber));
   for (i =0; i < 12; i++) x->XboxSerialNumber[i] = (CHAR)(65 + i);
   RtlZeroMemory(x->RecoveryKey, sizeof(x->RecoveryKey));
   x->dwOnlineKeyVersion = 0;

   RtlZeroMemory(x->OnlineKeyEncryptedBase64Encoded, sizeof(x->OnlineKeyEncryptedBase64Encoded));
   RtlZeroMemory(x->HardDriveKeyBase64Encoded, sizeof(x->HardDriveKeyBase64Encoded));

   RtlZeroMemory(x->OnlineKeyDataBaseInfo, sizeof(x->OnlineKeyDataBaseInfo));
   x->dwOutBufferSize = PACKET_DATA_SIZE;

   RtlZeroMemory(x->OnlineKeyEncrypted, sizeof(x->OnlineKeyEncrypted));
   RtlZeroMemory(x->OnlineKey, sizeof(x->OnlineKey));
   x->ulOnlineKey = 0;
}

PCHAR
C2kIdUtils::
AvRegionBitTest( IN DWORD dwBitMask)
{

   static CHAR sz[ 260 ] = "";

   ZeroMemory(sz, 260);
   if (AV_STANDARD_NTSC_M == (dwBitMask & AV_STANDARD_NTSC_M) ) 
      strcat(sz,"NTSC ");
   if (AV_STANDARD_NTSC_J == (dwBitMask & AV_STANDARD_NTSC_J) ) 
      strcat(sz,"Japanese NTSC ");
   if (AV_STANDARD_PAL_I == (dwBitMask & AV_STANDARD_PAL_I) ) 
      strcat(sz,"PAL I ");
   if (AV_STANDARD_PAL_M == (dwBitMask & AV_STANDARD_PAL_M)  ) 
      strcat(sz,"PAL M ");
   
   if ( dwBitMask & AV_FLAGS_60Hz  ) 
      strcat(sz,"60Hz ");
   if ( dwBitMask & AV_FLAGS_50Hz  ) 
      strcat(sz,"50Hz ");

   return sz;
}


HRESULT
C2kIdUtils::
GenerateID ( LPBYTE lpbData, DWORD dwSize )
{

   // from xboxp.h
   //#define XC_GAME_REGION_NA           1
   //#define XC_GAME_REGION_JAPAN        2
   //#define XC_GAME_REGION_RESTOFWORLD  4
   
   DWORD GameRegion = XC_GAME_REGION_NA;
   DWORD AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
   BYTE MACAddress[ETHERNET_MAC_ADDR_LEN] = { 0x66,0x66,0x66,0x66,0x66,0x66};
   char XboxSerialNumber[XBOX_SERIAL_NUMBER_LEN ] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
   char HardDriveKey[HARD_DRIVE_KEY_LEN];
   char RecoveryKey[RECOVERY_KEY_LEN];
   DWORD dwOnlineKeyVersion = 0;
   char OnlineKey[173];

   DWORD dwOutBufferSize;
   dwOutBufferSize = dwSize;
   return  GeneratePerBoxData ( GameRegion,
                                AVRegion,
                                MACAddress,
                                XboxSerialNumber,
                                HardDriveKey,
                                RecoveryKey,
                                &dwOnlineKeyVersion,
                                OnlineKey,
                                lpbData,
                                &dwOutBufferSize );

}


HRESULT
C2kIdUtils::
ReadFileIntoBuffer (IN char* szFileName, 
                    IN BYTE* bFile,
                    IN DWORD dwSizeOfBuffer)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    4/27/2001

    Routine Description:
        This routine will read the contents of a file into bFile
    Arguments:
        The name of the file to read and a buffer to put it in.
        Size of buffer to writeinto.
    Return Value:
      True for success; False for failure.
--*/
{

   HANDLE h;

   DWORD dwNumberOfBytesToRead = dwSizeOfBuffer;
   DWORD dwNumberOfBytesRead;
   h = CreateFile ( szFileName, 
                    GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,// | FILE_FLAG_NO_BUFFERING,
                    NULL);

   if ( h == INVALID_HANDLE_VALUE ) {
      printf("CreatFile Failed\n" );
      return FALSE;
   }
   
   ReadFile( h, 
             bFile,
             dwNumberOfBytesToRead,
             &dwNumberOfBytesRead, 
             NULL );
   CloseHandle(h);

   return TRUE;
}



BOOL
C2kIdUtils::
ScanInformation( IN PCHAR szDirectory, 
                 IN PCHAR inSerialNumber,
                 IN PCHAR inOnlineKey )
{
   
   CHAR szLine[300];
   CHAR szLineTest[50];
   CHAR szOnlineKeyGuess[5][200];
   CHAR szOnlineKey[200];
   
   
   CHAR szSerialNumber[40];
   CHAR szSerialNumberGuess[5][40];
   
   CHAR szDontCare[40][30];

   FILE *read;
   BOOL bReturn = FALSE;
   

   ZeroMemory(szSerialNumber,40);
   ZeroMemory(szOnlineKeyGuess,200*5);
   ZeroMemory(szSerialNumberGuess,40*5);

   read = fopen( szDirectory, "r");
   if (read == NULL)
      printf( "Null fead\n" );
   while ( !feof( read ) ) {
      
      if ( fgets( szLine, 300, read ) == NULL) {
         break;
      }
      sscanf( szLine,"%s", szLineTest );

      if (0 != strstr(szLineTest, "OBJ")) {
         //Find SerialNumber
         sscanf( szLine,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
                 szDontCare[0],
                 szDontCare[1],
                 szDontCare[2],
                 szDontCare[3],
                 szDontCare[4],
                 szDontCare[5],
                 szDontCare[6],
                 szDontCare[7],
                 szDontCare[8],
                 szDontCare[9],
                 szDontCare[10],
                 szDontCare[11],
                 szDontCare[12],
                 szSerialNumberGuess[0], 
                 szSerialNumberGuess[1], 
                 szSerialNumberGuess[2], 
                 szSerialNumberGuess[3], 
                 szSerialNumberGuess[4] 
                 );


         // This will grab the Serial Number key even if it moves 5 positions.
         if (0 == strcmp (szSerialNumberGuess[0],inSerialNumber)) {
            strcpy (szSerialNumber, szSerialNumberGuess[0]);
         }else 
         if (0 == strcmp (szSerialNumberGuess[1],inSerialNumber)) {
            strcpy (szSerialNumber, szSerialNumberGuess[1]);
         }else
         if (0 == strcmp (szSerialNumberGuess[2],inSerialNumber)) {
            strcpy (szSerialNumber,szSerialNumberGuess[2]);
         }else
         if (0 == strcmp (szSerialNumberGuess[3],inSerialNumber)) {
            strcpy (szSerialNumber,szSerialNumberGuess[3]);
         }else
         if (0 == strcmp (szSerialNumberGuess[4],inSerialNumber)) {
            strcpy (szSerialNumber,szSerialNumberGuess[4]);
         }


         //for (int o = 0; o < 16; o++) {
         //   printf( "Token: %s \n", szDontCare[o] );
         //}
         
         //printf( "SerialNumber:: %s | %s\n", szSerialNumber, inSerialNumber );
         // Match SerialNumber we want.
         if(0 == strcmp(szSerialNumber,inSerialNumber)){
            printf( "SerialNumber:: FOUND  %s\n", szSerialNumber );
            // Find the Online Key. Its Next Line
            if ( fgets( szLine, 300, read ) == NULL) {
               printf( "fgets error\n" );
               break;
            }
            sscanf( szLine,"%s%s%s%s%s%s%s%s%s%s%s", 
                    szDontCare[0],
                    szDontCare[1],
                    szDontCare[2],
                    szDontCare[3],
                    szDontCare[4],
                    szDontCare[5],
                    szDontCare[6],
                    szOnlineKeyGuess[0],
                    szOnlineKeyGuess[1],
                    szOnlineKeyGuess[2] );
                                        
            // This will grab the Online key even if it moves 3 positions.
            if (0 != strstr (szOnlineKeyGuess[0],"=")) {
               strcpy (szOnlineKey, szOnlineKeyGuess[0]);
            }else 
            if (0 != strstr (szOnlineKeyGuess[1],"=")) {
               strcpy (szOnlineKey, szOnlineKeyGuess[1]);
            }else
            if (0 != strstr (szOnlineKeyGuess[2],"=")) {
               strcpy (szOnlineKey, szOnlineKeyGuess[2]);
            }
            
            //for (int o = 0; o < 8; o++) {
            //   printf( "Token: %s \n", szDontCare[o] );
            //}
            //printf( "DB:=>%s\n\n", szOnlineKey );
            RtlCopyMemory( inOnlineKey, szOnlineKey, 172 );
            bReturn = TRUE;
            goto c0;
         }
      }
   }
   c0:
   fclose( read );
   return bReturn;
}


BOOL
C2kIdUtils::
ScanInformationXLS( IN PCHAR szDirectory, 
                    IN PCHAR inSerialNumber,
                    IN PCHAR inOnlineKey )
{
   
   CHAR szLine[300];
   CHAR szLineTest[50];
   CHAR szOnlineKeyGuess[5][200];
   CHAR szOnlineKey[200];
   
   
   CHAR szSerialNumber[40];
   CHAR szSerialNumberGuess[5][40];
   
   CHAR szDontCare[40][30];

   FILE *read;
   BOOL bReturn = FALSE;
   

   ZeroMemory(szSerialNumber,40);
   ZeroMemory(szOnlineKeyGuess,200*5);
   ZeroMemory(szSerialNumberGuess,40*5);

   read = fopen( szDirectory, "r");
   if (read == NULL)
      printf( "Null fead\n" );
   while ( !feof( read ) ) {

      if ( fgets( szLine, 300, read ) == NULL) {
         break;
      }
      sscanf( szLine,"%s", szLineTest );
      if (0 != strstr(szLineTest, "-")) {
         //Find SerialNumber
         sscanf( szLine,"%s%s%s%s%s%s%s%s", 
                 szDontCare[0],
                 szDontCare[1],
                 szSerialNumberGuess[0], 
                 szDontCare[3],
                 szDontCare[4],
                 szDontCare[5],
                 szDontCare[6],
                 szOnlineKeyGuess[0]
               );

         strcpy (szSerialNumber, szSerialNumberGuess[0]);
         if (0 == strcmp(szSerialNumber,inSerialNumber)) {
            printf( "SerialNumber:: FOUND  %s\n", szSerialNumber );
            strcpy (szOnlineKey, szOnlineKeyGuess[0]);
            RtlCopyMemory( inOnlineKey, szOnlineKey, 172 );
            bReturn = TRUE;
            goto c0;
         }
      }
   }
   c0:
   fclose( read );
   return bReturn;
}

