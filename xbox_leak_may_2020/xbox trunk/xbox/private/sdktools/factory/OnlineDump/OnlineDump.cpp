#include <ntos.h>
#include <init.h>
#include <xtl.h>
#include <xboxp.h>
#include <xconfig.h>
#include <av.h>
#include <stdio.h>
#include <xdbg.h>
#include <malloc.h>
#define XFONT_TRUETYPE
#include <xfont.h>

#include "cryptkeys.h"
#include "xboxvideo.h"
#include "smcdef.h"
#include "xboxverp.h"
#include "ctype.h"
#include <xdbg.h>

#define XFONT_TRUETYPE
#include <xfont.h>

#include "CXCrypto.h"

#include "onlinedump.h"

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }


#define DUMP_SIZE (EEPROM_TOTAL_MEMORY_SIZE + sizeof(RC4_SHA1_HEADER) + XBOX_KEY_LENGTH)


CXBoxVideo* g_pVideo;


VOID
DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2, DWORD dwColor)
{
    ASSERT(g_pVideo && pcszTextLine1);

    g_pVideo->ClearScreen(dwColor);

    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_TEXTAREA_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       pcszTextLine1);

    if (NULL != pcszTextLine2)
    {
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + FONT_DEFAULT_HEIGHT + ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine2);
    }


    WCHAR szBuildString[64];
    wsprintfW(szBuildString, L"XTL Version %hs", VER_PRODUCTVERSION_STR);
    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_BUILDNUM_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       szBuildString);

    g_pVideo->ShowScreen();

}

int __cdecl main()
{
   //
   // Dump Online related Contents.
   //
   COnlineDump* m = new COnlineDump();
   
   CHAR szSerialNumber[13];
   CHAR szFileName[400];
   WCHAR wszSuccessMessage[400];
   BYTE bData[DUMP_SIZE];
   BOOL bResult;
   DWORD dwDeviceBitMask;
   DWORD dwReturn;
   DWORD dwReturnVal;
   CHAR sz;
   DWORD dwInsertions;                  
   DWORD dwRemovals;
   DWORD dwReturnMask;
   XDEVICE_PREALLOC_TYPE xdpt = {XDEVICE_TYPE_MEMORY_UNIT, 8};
   EEPROM_LAYOUT* pel= (EEPROM_LAYOUT*)bData;
   XBOX_FACTORY_SETTINGS* pxfs = (XBOX_FACTORY_SETTINGS*)&pel->FactorySection;   
   
   //
   // Initialize our graphics.
   //
   if ((g_pVideo = new CXBoxVideo) == NULL)
   {
      m->DebugOut("Couldn't allocate video objects");
      goto Cleanup;
   }

   g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );
   g_pVideo->ClearScreen(COLOR_BLACK);
   g_pVideo->ShowScreen();

   // Read the EEPROM and dump it.
   bResult = m->ReadEEPROM(bData);
   if (!bResult)
   {
      DrawMessage (L"Online Dump", L"Failed to read EEPROM. Aborting...", COLOR_TV_RED);
      goto Cleanup;
   }
   
   strncpy( szSerialNumber, (const char*)pxfs->SerialNumber,12);
   szSerialNumber[12] = '\0';

   //m->DrawText("Initializing MU's");
   XInitDevices( 1, &xdpt);
   dwDeviceBitMask = XGetDevices( XDEVICE_TYPE_MEMORY_UNIT );

   for (;;) {
      // Some removal or insertion happened
      if (TRUE == XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, 
                                     &dwInsertions, &dwRemovals )) {
         if ( !dwRemovals ) {
            dwReturnMask = m->DetermineBitMask(dwInsertions); 
 
            dwReturnVal = XMountMU(dwReturnMask, HIWORD(dwInsertions)? XDEVICE_BOTTOM_SLOT : XDEVICE_TOP_SLOT, &sz );
            if ( ERROR_SUCCESS != dwReturnVal ) {
               DrawMessage (L"Online Dump", L"Failed to Mount MU.", COLOR_TV_RED);
               goto Cleanup;
            }

            //m->DrawText("Insert:: MU Plug %lu and %s Driveletter is %c", dwReturnMask, HIWORD(dwInsertions)? "BOTTOM":"TOP", sz );

            sprintf( szFileName,"%c:\\%s-%s.dat",sz,"EEPROM",szSerialNumber);
            wsprintfW( wszSuccessMessage, L"Dump to %hs successful.",szFileName);
            
            //XMUNameFromDriveLetter( sz, wszMUName, sizeof( wszMUName ));
            //m->DrawText("Writing %s to %S",szFileName,wszMUName);
            
            bResult = m->FileToDrive( szFileName, (LPVOID)bData, DUMP_SIZE, NEW_FILE );
            if (!bResult)
            {
               DrawMessage (L"Online Dump", L"Failed to write configuration out.", COLOR_TV_RED);
            }
            else
            {
               DrawMessage (L"Online Dump", wszSuccessMessage, COLOR_DARK_GREEN);
            }
            
            XUnmountMU( dwReturnMask, HIWORD(dwInsertions)? XDEVICE_BOTTOM_SLOT : XDEVICE_TOP_SLOT );

            goto Cleanup;
         }
         else if ( !dwInsertions )
         {
            dwReturnMask=m->DetermineBitMask(dwRemovals);
            // m->DrawText("Removed:: MU Plug %lu and %s Driveletter is %c", dwReturnMask, HIWORD(dwRemovals)? "BOTTOM":"TOP", sz );
         }
      }
   }

Cleanup:

   delete m;
   Sleep (INFINITE);

   return 0;
}




DWORD
COnlineDump::
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



BOOL
COnlineDump::
ReadEEPROM ( BYTE* pbData )
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

   BYTE shaDigest[XC_SERVICE_DIGEST_SIZE];
   BYTE shaCtxt[XC_SERVICE_SHA_CONTEXT_SIZE];
   BYTE bEncryptedTail[sizeof(RC4_SHA1_HEADER) + XBOX_KEY_LENGTH];
   XBOX_FACTORY_SETTINGS savedSettings;
   DWORD i;
   
   // read EEPROM Data
   status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, bEEPROMMemory, EEPROM_TOTAL_MEMORY_SIZE, &size);
   if (!NT_SUCCESS(status)) {
      DebugOut("ExQueryNonVolatileSetting FAILED");
      return FALSE;
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
         DebugOut("EEPROM Decrypt FAILED");
         return FALSE;
      }
   }

   //
   // Check for invalid characters in serial number
   //
   for(i=0; i<sizeof(factorySettings->SerialNumber); ++i)
   {
      if (!isalnum(factorySettings->SerialNumber[i]))
      {
          factorySettings->SerialNumber[i] = '#';
      }
   }
   
   // DeMorph the OnlineKey before using it.
   MorphKeyByHardDriveKey( factorySettings->OnlineKey,sizeof( factorySettings->OnlineKey));

   // Compute one way hash of HD Key
   ASSERT( sizeof(pEncryptedEEPROMData->HardDriveKey) == XBOX_KEY_LENGTH );
   XcSHAInit( shaCtxt );
   XcSHAUpdate( shaCtxt, pEncryptedEEPROMData->HardDriveKey, XBOX_KEY_LENGTH );
   XcSHAFinal( shaCtxt, shaDigest );

   //
   // Create Encrypted Tail Section
   //
   RtlCopyMemory( bEncryptedTail+sizeof(RC4_SHA1_HEADER), shaDigest, XBOX_KEY_LENGTH );
   rc4HmacEncrypt(
       pEncryptedEEPROMData->EncHeader, // use as confounder
       factorySettings->OnlineKey, sizeof( factorySettings->OnlineKey),
       bEncryptedTail+sizeof(RC4_SHA1_HEADER), XBOX_KEY_LENGTH,
       (LPBYTE)bEncryptedTail );

   //
   // Zero out everything but Serial number and EthernetAddr
   //
   RtlCopyMemory(savedSettings.SerialNumber, factorySettings->SerialNumber, sizeof(savedSettings.SerialNumber));
   RtlCopyMemory(savedSettings.EthernetAddr, factorySettings->EthernetAddr, sizeof(savedSettings.EthernetAddr));
   RtlZeroMemory( bEEPROMMemory, sizeof(bEEPROMMemory) );
   RtlCopyMemory(factorySettings->SerialNumber, savedSettings.SerialNumber, sizeof(savedSettings.SerialNumber));
   RtlCopyMemory(factorySettings->EthernetAddr, savedSettings.EthernetAddr, sizeof(savedSettings.EthernetAddr));
   
   // Copy everthing over to the databuffer.
   ASSERT( sizeof(bEEPROMMemory) + sizeof(bEncryptedTail) == DUMP_SIZE );
   RtlCopyMemory(pbData, bEEPROMMemory, sizeof(bEEPROMMemory));
   RtlCopyMemory(pbData+sizeof(bEEPROMMemory), bEncryptedTail, sizeof(bEncryptedTail));
   
   return TRUE;
}


VOID
COnlineDump::
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
   
#ifdef DBG
   vsprintf( szBuffer, szMessage, vaArgs );

   OutputDebugStringA( szBuffer );
#endif

   va_end  ( vaArgs );
   return;
}



BOOL
COnlineDump::
FileToDrive( LPCSTR lpFileName,
             LPVOID pvFileContents,
             DWORD dwFileSize,
             DWORD dwWriteFlags )
{

   HANDLE hFile = INVALID_HANDLE_VALUE;
   DWORD dwBytesWrote;
   DWORD dwPos;
   DWORD dwFileStyle;
   CHAR  szOperation[30];
   DWORD dwFilePosition;
   BOOL bResult = FALSE;
   
   switch (dwWriteFlags) {
   case APPEND_FILE:
      dwFileStyle = OPEN_ALWAYS;
      sprintf(szOperation,"%s","Appending File");
      dwFilePosition = FILE_END;
      break;
   case NEW_FILE:
   default:
      dwFileStyle = CREATE_ALWAYS;
      sprintf(szOperation,"%s","New File");
      dwFilePosition = FILE_BEGIN;
      break; 
   }
   
   hFile = CreateFile(lpFileName, 
                      GENERIC_WRITE, 
                      FILE_SHARE_READ | FILE_SHARE_WRITE, 
                      NULL,
                      dwFileStyle, 
                      FILE_ATTRIBUTE_NORMAL, 
                      NULL);

   if ( hFile == INVALID_HANDLE_VALUE ) {
      DebugOut("%s:unable to open %s, error %X\n", szOperation,lpFileName, GetLastError());
      goto Cleanup;
   }

   dwPos = SetFilePointer( hFile, 0, NULL, dwFilePosition );
   if ( dwPos == INVALID_SET_FILE_POINTER ) {
      DebugOut("%s: unable to seek %s, error %X\n", szOperation, lpFileName, GetLastError());
      goto Cleanup;
   }

   if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) || (dwBytesWrote != dwFileSize)) {
      DebugOut("%s: cannot write to %s, error %X\n", szOperation, lpFileName, GetLastError() );
      goto Cleanup;
   }

   bResult = TRUE;
   
Cleanup:

   if (hFile != INVALID_HANDLE_VALUE)
   {
      CloseHandle(hFile);
   }

   return bResult;
}



