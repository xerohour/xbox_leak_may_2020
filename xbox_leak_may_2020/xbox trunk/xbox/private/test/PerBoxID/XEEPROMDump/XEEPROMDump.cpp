#include "XEEPROMDump.h"

#define DUMP_SIZE (EEPROM_TOTAL_MEMORY_SIZE + sizeof(RC4_SHA1_HEADER) + XBOX_KEY_LENGTH)

int 
__cdecl main()
{
   // Dump EEPROM Contents.
   //CXEEPROMDUMP* m = new CXEEPROMDUMP(TEXT("t:\\media\\tahoma.ttf"));
   CXEEPROMDUMP* m = new CXEEPROMDUMP(TEXT("d:\\tahoma.ttf"));
   
   CHAR szSerialNumber[ 13 ] = "";
   CHAR szFileName[ 40 ] = "";
   DATA_PACKET Data;
   WCHAR wszMUName[40];

   // Output Setup
   m->SetFontHeight(15);
   m->StartScreenThread();
   m->DrawText("Starting EEPROM Dump");

   EEPROM_LAYOUT* pel= (EEPROM_LAYOUT*)&Data.byteData;
   XBOX_FACTORY_SETTINGS* pxfs = (XBOX_FACTORY_SETTINGS*)&pel->FactorySection;
   
   
   // Read the EEPROM and dump it.
   m->ReadEEPROM(&Data );
   strncpy( szSerialNumber, (const char*)pxfs->SerialNumber,12);

   DWORD dwDeviceBitMask;
   DWORD dwReturn;
   DWORD dwReturnVal;
   CHAR sz;
   DWORD dwInsertions;                  
   DWORD dwRemovals;
   DWORD dwReturnMask;
   
   m->DrawText("Initializing MU's");
   XDEVICE_PREALLOC_TYPE xdpt = {XDEVICE_TYPE_MEMORY_UNIT, 8};
   XInitDevices( 1, &xdpt);
   dwDeviceBitMask = XGetDevices( XDEVICE_TYPE_MEMORY_UNIT );

   for (;;) {
      // Some removal or insertion happened
      if (TRUE == XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, 
                                     &dwInsertions, &dwRemovals )) {
         if ( !dwRemovals ) {
           dwReturnMask=m->DetermineBitMask(dwInsertions); 
 
            dwReturnVal = XMountMU(dwReturnMask,
                                   HIWORD(dwInsertions)? XDEVICE_BOTTOM_SLOT : XDEVICE_TOP_SLOT, 
                                   &sz  );
            if ( ERROR_SUCCESS != dwReturnVal ) {
               m->DrawText("XMountMU error %lu", dwReturnVal);
               if ( ERROR_UNRECOGNIZED_VOLUME == dwReturnVal )
                  m->DrawText("MU unformmated");
            }

            m->DrawText("Insert:: MU Plug %lu and %s Driveletter is %c", 
                         dwReturnMask, 
                         HIWORD(dwInsertions)? "BOTTOM":"TOP", 
                         sz );
            strncpy( szSerialNumber, (const char*)pxfs->SerialNumber,12);
            sprintf( szFileName,"%c:\\%s-%s.dat",sz,"EEPROM",szSerialNumber);
            XMUNameFromDriveLetter( sz, wszMUName, sizeof( wszMUName ));
            m->DrawText("Writing %s to %S",szFileName,wszMUName);
            m->FileToDrive( szFileName, 
                            (LPVOID)&Data.byteData,
                            DUMP_SIZE,
                            NEW_FILE );
            
            //Enumerate the MU
            //m->EnumDirectory( sz );
            XUnmountMU( dwReturnMask,
                        HIWORD(dwInsertions)? XDEVICE_BOTTOM_SLOT : XDEVICE_TOP_SLOT );
         }
         else
         if ( !dwInsertions ) {
            dwReturnMask=m->DetermineBitMask(dwRemovals); 
            m->DrawText("Removed:: MU Plug %lu and %s Driveletter is %c",
                        dwReturnMask, 
                        HIWORD(dwRemovals)? "BOTTOM":"TOP", 
                        sz );
          }
      }
   }
   delete m;
}

BOOL
CXEEPROMDUMP::
EnumDirectory( IN CHAR szDrive )
{
   
   //Read in the file.
   WIN32_FIND_DATA   ffd;
   HANDLE            hFind;
   CHAR  szFindFiles[ MAX_PATH ];
   DATA_PACKET MemData;

   BYTE bEEPROMMemory[EEPROM_TOTAL_MEMORY_SIZE];
   EEPROM_LAYOUT*  eeprom                 = (EEPROM_LAYOUT*)bEEPROMMemory;
   XBOX_FACTORY_SETTINGS* factorySettings = (XBOX_FACTORY_SETTINGS*)    &eeprom->FactorySection;
   XBOX_USER_SETTINGS* userSettings       = (XBOX_USER_SETTINGS*)       &eeprom->UserConfigSection;
   PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA)&eeprom->EncryptedSection;



   EEPROM_LAYOUT* pel1= (EEPROM_LAYOUT*)&MemData.byteData;
   XBOX_FACTORY_SETTINGS* pxfs2 = (XBOX_FACTORY_SETTINGS*)&pel1->FactorySection;

   sprintf(szFindFiles,"%c:\\EEPROM*.dat",szDrive );
   DrawText("Looking in %s", szFindFiles );
   hFind = FindFirstFile( szFindFiles, &ffd);

   if (INVALID_HANDLE_VALUE == hFind) {
      DrawText("Could not find any files in %c:\\ Error %lu", szDrive, GetLastError() );
      return FALSE;
   }
   // else found files.
   do {
      //if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
         
         sprintf( szFindFiles,"%c:\\%s",szDrive,ffd.cFileName);
         DrawText("We found %s ", szFindFiles ); 

         ZeroMemory(&MemData,sizeof(MemData));
         ReadFileIntoBuffer ( szFindFiles, 
                             MemData.byteData,
                             sizeof(MemData.byteData));
         /*
         _tprintf(TEXT("%s\\%s\n"),szDirectory,ffd.cFileName);
         // Perform task.
         _stprintf(szFindFiles,TEXT("%s\\%s"),szDirectory,ffd.cFileName);
         ZeroMemory( eeprom, EEPROM_TOTAL_MEMORY_SIZE);
         
         ReadFileIntoBuffer( szFindFiles, (BYTE*)eeprom,  EEPROM_TOTAL_MEMORY_SIZE);

         _tprintf( "This is the HardDriveKey ");
         DumpBytesHex( pEncryptedEEPROMData->HardDriveKey, 16 );
         pEncryptedEEPROMData->GameRegion ^= XC_GAME_REGION_MANUFACTURING;
         _tprintf( "This is the GameRegion %lu\n",pEncryptedEEPROMData->GameRegion);
         
         _tprintf( "This is the SerialNumber ");
         DumpBytesChar( factorySettings->SerialNumber, 12);
         
         _tprintf( "This is the EthernetAddress ");
         DumpBytesHex( factorySettings->EthernetAddr, 6);

         _tprintf( "This is the Online Key ");
         DumpBytesHex( factorySettings->OnlineKey, 16);
         
         _tprintf( "This is the AVRegion %lu\n",factorySettings->AVRegion);
         _tprintf( "This is the Language %lu\n",userSettings->Language );
      */
      //}

   }while( FindNextFile(hFind, &ffd));
   FindClose(hFind);
   return TRUE;
}



DWORD
CXEEPROMDUMP::
DetermineBitMask(IN DWORD dwBitMask)
{


   DWORD dwReturnMask;

   if ( dwBitMask & XDEVICE_PORT0_TOP_MASK ) {
      dwReturnMask = XDEVICE_PORT0;
   } else
      if ( dwBitMask & XDEVICE_PORT0_BOTTOM_MASK  ) {
      // Port 1 Bottom
      dwReturnMask = XDEVICE_PORT0;

   } else
      if ( dwBitMask & XDEVICE_PORT1_TOP_MASK   ) {
      // Port 2 Bottom
      dwReturnMask = XDEVICE_PORT1;//MAKELONG(TOP,1);
   } else
      if ( dwBitMask &  XDEVICE_PORT1_BOTTOM_MASK ) {
      // Port 2 Bottom
      dwReturnMask = XDEVICE_PORT1;//MAKELONG(BOTTOM ,1);
   } else
      if ( dwBitMask & XDEVICE_PORT2_TOP_MASK   ) {
      // Port 3 Bottom
      dwReturnMask = XDEVICE_PORT2;//MAKELONG(TOP ,2);
   } else
      if ( dwBitMask & XDEVICE_PORT2_BOTTOM_MASK  ) {
      // Port 3 Bottom
      dwReturnMask = XDEVICE_PORT2;//MAKELONG(BOTTOM ,2);
   } else
      if ( dwBitMask & XDEVICE_PORT3_TOP_MASK  ) {
      // Port 4 Bottom
      dwReturnMask = XDEVICE_PORT3;//MAKELONG(TOP ,3);
   } else
      if ( dwBitMask & XDEVICE_PORT3_BOTTOM_MASK  ) {
      // Port 4 Bottom
      dwReturnMask = XDEVICE_PORT3;//MAKELONG(BOTTOM , 3 );
   }
   return dwReturnMask;

}

HRESULT
CXEEPROMDUMP::
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
      DrawText("CreatFile Failed");
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






HRESULT
CXEEPROMDUMP::
ReadEEPROM ( IN PDATA_PACKET dpData )
{

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
   BOOL bResult; 
   NTSTATUS status;
   DWORD type, size;
   BYTE bEEPROMMemory[EEPROM_TOTAL_MEMORY_SIZE];
   EEPROM_LAYOUT* eeprom                  = (EEPROM_LAYOUT*)bEEPROMMemory;
   XBOX_FACTORY_SETTINGS* factorySettings = (XBOX_FACTORY_SETTINGS*)&eeprom->FactorySection;
   XBOX_USER_SETTINGS* userSettings       = (XBOX_USER_SETTINGS*)   &eeprom->UserConfigSection;
   PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA)&eeprom->EncryptedSection;

   BYTE bEncryptedTail[sizeof(RC4_SHA1_HEADER) + XBOX_KEY_LENGTH];

   // read EEPROM Data
   status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, bEEPROMMemory, EEPROM_TOTAL_MEMORY_SIZE, &size);
   if (!NT_SUCCESS(status)) {
      DebugOut("ExQueryNonVolatileSetting FAILED");
      dpData->ulDataSize = 0;
      return E_FAIL;
   }

   //
   // Decrypt the Encrypted part of the EEPROM if it is encrypted.
   //
   if (!RtlEqualMemory(pEncryptedEEPROMData->HardDriveKey,ZERO_KEY,XBOX_KEY_LENGTH))
   {
      // Only Encrypt if it is not a dev kit
      //
      bResult = rc4HmacDecrypt(
                              (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
                              pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
                              (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
      if ( !bResult ) {
         DrawText("Decrypting HardriveKey and GameRegion Failed");
      }
   }

   // DeMorph the OnlineKey before returning it.
   // Simply put, we call morph a second time.
   // TODO: For now, the hardDrivekey is still not encrypted so we can pass it as such.
   MorphKeyByHardDriveKey( factorySettings->OnlineKey,sizeof( factorySettings->OnlineKey));

   //
   // Create Encrypted Tail Section
   //
   ASSERT( sizeof(pEncryptedEEPROMData->HardDriveKey) == XBOX_KEY_LENGTH );
   RtlCopyMemory( bEncryptedTail+sizeof(RC4_SHA1_HEADER), pEncryptedEEPROMData->HardDriveKey, XBOX_KEY_LENGTH );
   rc4HmacEncrypt(
       pEncryptedEEPROMData->EncHeader, // use as confounder
       factorySettings->OnlineKey, sizeof( factorySettings->OnlineKey),
       bEncryptedTail+sizeof(RC4_SHA1_HEADER), XBOX_KEY_LENGTH,
       (LPBYTE)bEncryptedTail );

   //
   // Zero out critical memory
   //
   RtlZeroMemory( pEncryptedEEPROMData->EncHeader, sizeof(pEncryptedEEPROMData->EncHeader) );
   RtlZeroMemory( factorySettings->OnlineKey, sizeof( factorySettings->OnlineKey) );
   RtlZeroMemory( pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) );
   
   // Copy the EEPROM over to the databuffer.
   ASSERT( sizeof(bEEPROMMemory) + sizeof(bEncryptedTail) == DUMP_SIZE );
   ASSERT( DUMP_SIZE <= sizeof(dpData->byteData) );
   RtlCopyMemory(dpData->byteData, eeprom, sizeof(bEEPROMMemory));
   RtlCopyMemory(dpData->byteData+sizeof(bEEPROMMemory), bEncryptedTail, sizeof(bEncryptedTail));
   
   return S_OK;
}


VOID
CXEEPROMDUMP::
DebugOut (PCHAR szMessage,...)
/*++

Routine Description:
   It behaves much like printf but sends to the debugOut.

Arguments:
   same as printf.

Return Value:
   NONE

Author: Wally Ho (wallyho) April 25th, 2001   

--*/
{
   va_list vaArgs;
   char szBuffer[ MAX_PATH];

   //  Write the formatted string to the log.
   va_start( vaArgs, szMessage );
   vsprintf( szBuffer, szMessage, vaArgs );

   OutputDebugStringA( szBuffer );
   va_end  ( vaArgs );
   return;
}



VOID
CXEEPROMDUMP::
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
      break; 
   }
   
   hFile = CreateFile(lpFileName, 
                      GENERIC_WRITE, 
                      FILE_SHARE_READ | FILE_SHARE_WRITE, 
                      NULL,
                      dwFileStyle, 
                      FILE_ATTRIBUTE_NORMAL, 
                      NULL);

   if (hFile == INVALID_HANDLE_VALUE) {
      DebugOut("%s:unable to open %s, error %X\n", szOperation,lpFileName, GetLastError());
      return;
   }

   dwPos = SetFilePointer( hFile, 0, NULL, dwFilePosition );
   if ( dwPos == INVALID_SET_FILE_POINTER ) {
      DebugOut("%s: unable to seek %s, error %X\n", szOperation, lpFileName, GetLastError());
      return;        
   }

   if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) || (dwBytesWrote != dwFileSize)) {
      DebugOut("%s: cannot write to %s, error %X\n", szOperation, lpFileName, GetLastError() );
      return;
   }
   CloseHandle(hFile);
}



