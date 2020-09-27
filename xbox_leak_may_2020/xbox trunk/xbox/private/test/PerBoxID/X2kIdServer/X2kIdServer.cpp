#include "X2kIdServer.h"



int 
__cdecl main()
{
   CNetWork cn;
   SOCKET   sAccept;
   
   // Start the screen processing thread
   g_xv.StartScreenThread();
   g_xv.SetFontHeight(13);

   // In a continous loop, wait for incoming clients. Once one 
   // is detected, create a thread and pass the handle off to it.
   g_xv.DrawText("Listening....");
   
   while (1) {
      // Accept will block then thread will start.
      if ( INVALID_SOCKET == (sAccept= cn.Accept())){
         g_xv.DrawText("INVALID_SOCKET on calling accept....");
      } else
         StartThread( ClientThread,(LPVOID)sAccept);
   }
   WSACleanup();
   return 0;
}




DWORD WINAPI 
ClientThread(LPVOID lpParam)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    4/25/2001

    Routine Description:
        This is the thread function that will Do the receving work.
        it handles a given client connection.  The parameter passed 
        in is the socket handle returned from an accept() call.  
        This function reads data from the client and writes it back.

    Arguments:
        A Variable void pointer.
    Return Value:

--*/

{
   
   HRESULT hr;
   SOCKET        RecvSocket = (SOCKET)lpParam;
   CNetWorkConnection cnc( (SOCKET)lpParam );
   DATA_PACKET InBuffer;
   DATA_PACKET dpEEPROM;
   g_xv.DrawText("[----------- INITIATING SESSION ----------]");

   // Read Command Buffer.
   ZeroMemory(&InBuffer, sizeof(InBuffer));
   if ( SOCKET_ERROR == cnc.RecvData(&InBuffer, sizeof(InBuffer)))
      goto cleanup;
   g_xv.DrawText("Command %lu",InBuffer.dwCommand );

   if (InBuffer.dwCommand & DO_VERIFY_WRITEPERBOXDATA ) {
      TestWritePerBoxData( RecvSocket );  
   }
   else if(InBuffer.dwCommand & DO_VERIFY_RETAILSYSTEM ){
      // Make it a retail kit override the DevKitFlag.
      ((XBOX_HARDWARE_INFO*)XboxHardwareInfo)->Flags = 0;
      TestRetailSystem ( RecvSocket );  
   }
   else if(InBuffer.dwCommand & DO_VERIFY_VERIFYPERBOXDATA ){
      TestVerifyPerBoxData( RecvSocket );  
   } 
   else if(InBuffer.dwCommand & DO_SENDBACK_EEPROM ){
      SendBackEEPROMContents( RecvSocket );  
   } 
   else if(InBuffer.dwCommand & DO_LOCK_HARDDRIVE ){
      LockHardDrive( RecvSocket, TRUE );  
   } 
   else if(InBuffer.dwCommand & DO_UNLOCK_HARDDRIVE ){
      LockHardDrive( RecvSocket, FALSE );  
   } 
   cleanup:
   g_xv.DrawText("[----------- TERMINATING SESSION ---------]");
   g_xv.DrawText(" ");
   return 0;
}


HRESULT
TestWritePerBoxData(IN SOCKET s)
{
   HRESULT hr = S_OK;
   CNetWorkConnection* c= new CNetWorkConnection( s );

   DATA_PACKET InBuffer;
   DATA_PACKET dpEEPROM;
   char buffer[256];


   ZeroMemory(&InBuffer, sizeof(InBuffer));
   if ( SOCKET_ERROR == c->RecvData(&InBuffer, sizeof(InBuffer))) {
      hr = E_FAIL;
      goto c0;
   }
   // Do the WritePerBoxData
   //extern XBOX_HARDWARE_INFO* XboxHardwareInfo;
   
   hr = WritePerBoxData( InBuffer.byteData , InBuffer.ulDataSize );
   if ( FAILED( hr ) ) {

      //sprintf(buffer, "FAILED WritePerBoxData returned %x", hr);
      //AppendFile( SZDATAFILE, buffer,strlen( buffer ));
      g_xv.DrawText("FAILED WritePerBoxData returned %x", hr );
      hr = E_FAIL;
      goto c0;
   } else
      g_xv.DrawText("SUCCEEDED WritePerBoxData");


   // Read out the EEPROM and send what we have
   sprintf(dpEEPROM.szMessage, "EEPROM DATA" );
   /*
      The flag XboxHardwareInfo is defined in init.h under.
      ntos\inc. This controls if the machine is a devkit 
      or if it is a retail box. We'll used the same thing 
      to tell our test program that the devkit has a
      Zero Hard Drive Key.
             
      #if !defined(_NTSYSTEM_)
      extern const XBOX_HARDWARE_INFO* XboxHardwareInfo;
      #else
      extern XBOX_HARDWARE_INFO XboxHardwareInfo;
      #endif
      
      //
      //  Define XboxHardwareFlags - a list of 
      //  flags indicating hardware differences
      //  the title libraries must know about.
      //
      #define XBOX_HW_FLAG_INTERNAL_USB_HUB   0x00000001
      #define XBOX_HW_FLAG_DEVKIT_KERNEL      0x00000002
   */
   if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
   {
      dpEEPROM.bDevKit = TRUE;
   }else {
      dpEEPROM.bDevKit = FALSE;
   }

   hr = ReadEEPROM ( &dpEEPROM );
   if ( FAILED( hr ) ) {
      g_xv.DrawText("FAILED ReadEEPROM, returned %x", hr );
      hr = E_FAIL;
      goto c0;
   } else
      g_xv.DrawText("SUCCEEDED ReadEEPROM");

   if ( SOCKET_ERROR == c->SendData(&dpEEPROM, sizeof(dpEEPROM))){
      hr = E_FAIL;
      goto c0;
   }

// Send the STATUS back
   // Make sure we write all the data
   sprintf(InBuffer.szMessage, "Decrypted buffer Data");
   if ( SOCKET_ERROR == c->SendData  (&InBuffer, sizeof(InBuffer))){
      hr = E_FAIL;
      goto c0;
   }

   UnlockXboxHardDrive( NULL, 0 );
   c0:
   // Should close the socket
   delete c;
   return hr;
}



HRESULT
TestRetailSystem(IN SOCKET s)
{
   HRESULT hr = S_OK;
   CNetWorkConnection cnc( s );
   DATA_PACKET InBuffer;
   DATA_PACKET dpEEPROM;
   char buffer[256];

   ZeroMemory(&InBuffer, sizeof(InBuffer));
   if ( SOCKET_ERROR == cnc.RecvData(&InBuffer, sizeof(InBuffer))) {
      hr = E_FAIL;
      goto c0;

   }
   // Do the WritePerBoxData
   hr = WritePerBoxData( InBuffer.byteData , InBuffer.ulDataSize );
   if ( FAILED( hr ) ) {

      //sprintf(buffer, "FAILED WritePerBoxData returned %x", hr);
      //AppendFile( SZDATAFILE, buffer,strlen( buffer ));
      g_xv.DrawText("FAILED WritePerBoxData returned %x", hr );
      hr = E_FAIL;
      goto c0;
   } else
      g_xv.DrawText("SUCCEEDED WritePerBoxData");


   // Read out the EEPROM and send what we have
   sprintf(dpEEPROM.szMessage, "EEPROM DATA" );
   
   /*
   
      The flag XboxHardwareInfo is defined in init.h under.
      ntos\inc. This controls if the machine is a devkit 
      or if it is a retail box. We'll used the same thing 
      to tell our test program that the devkit has a
      Zero Hard Drive Key.
             
      #if !defined(_NTSYSTEM_)
      extern const XBOX_HARDWARE_INFO* XboxHardwareInfo;
      #else
      extern XBOX_HARDWARE_INFO XboxHardwareInfo;
      #endif
      
      //
      //  Define XboxHardwareFlags - a list of 
      //  flags indicating hardware differences
      //  the title libraries must know about.
      //
      #define XBOX_HW_FLAG_INTERNAL_USB_HUB   0x00000001
      #define XBOX_HW_FLAG_DEVKIT_KERNEL      0x00000002
   */
   if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
   {
      dpEEPROM.bDevKit = TRUE;
   }else {
      dpEEPROM.bDevKit = FALSE;
   }


   hr = ReadEEPROM ( &dpEEPROM );
   if ( FAILED( hr ) ) {
      g_xv.DrawText("FAILED ReadEEPROM, returned %x", hr );
      hr = E_FAIL;
      goto c0;
   } else
      g_xv.DrawText("SUCCEEDED ReadEEPROM");


   if ( SOCKET_ERROR == cnc.SendData(&dpEEPROM, sizeof(dpEEPROM))){
      hr = E_FAIL;
      goto c0;
   }

// Send the STATUS back
   // Make sure we write all the data
   sprintf(InBuffer.szMessage, "Decrypted buffer Data");
   if ( SOCKET_ERROR == cnc.SendData  (&InBuffer, sizeof(InBuffer))){
      hr = E_FAIL;
      goto c0;
   }

   //Unlock it with the key we sent over.
   PXBOX_ENCRYPTED_PER_BOX_DATA p = (PXBOX_ENCRYPTED_PER_BOX_DATA)InBuffer.byteData;
   UnlockXboxHardDrive( p->HardDriveKey , XBOX_KEY_LENGTH );
   c0:
   if (0!= s )
      closesocket(s);
   return hr;
}


HRESULT
TestVerifyPerBoxData(IN SOCKET s)
{
   DATA_PACKET dp;
   EEPROM_LAYOUT* eeprom = (EEPROM_LAYOUT*) dp.byteData;
   XBOX_FACTORY_SETTINGS* factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
   XBOX_USER_SETTINGS* userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
   PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;

/*
   WriteEEPROM( &dp);
   switch(VerifyPerBoxData()){
   case NTE_BAD_DATA:
   }
  */ 
   return S_OK;
}

HRESULT
SendBackEEPROMContents(IN SOCKET s)
{
   
   CONST PCHAR SZDATAFILE = "t:\\EEPROMContents.dat";
   HRESULT   hr = S_OK;
   CNetWorkConnection cnc( s );
   DATA_PACKET InBuffer;
   DATA_PACKET dpEEPROM;
   char buffer[256];


   // Read out the EEPROM and send what we have
   sprintf(dpEEPROM.szMessage, "EEPROM DATA" );
   /*
      The flag XboxHardwareInfo is defined in init.h under.
      ntos\inc. This controls if the machine is a devkit 
      or if it is a retail box. We'll used the same thing 
      to tell our test program that the devkit has a
      Zero Hard Drive Key.
             
      #if !defined(_NTSYSTEM_)
      extern const XBOX_HARDWARE_INFO* XboxHardwareInfo;
      #else
      extern XBOX_HARDWARE_INFO XboxHardwareInfo;
      #endif
      
      //
      //  Define XboxHardwareFlags - a list of 
      //  flags indicating hardware differences
      //  the title libraries must know about.
      //
      #define XBOX_HW_FLAG_INTERNAL_USB_HUB   0x00000001
      #define XBOX_HW_FLAG_DEVKIT_KERNEL      0x00000002
   */
   
   g_xv.DrawText("FLAGS  %lu", XboxHardwareInfo->Flags );
   g_xv.DrawText("Kernel %lu", XBOX_HW_FLAG_DEVKIT_KERNEL );

   if ( !(XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL)) {
      // retail 
      dpEEPROM.bDevKit = FALSE;
   } else {
      dpEEPROM.bDevKit = TRUE;
   }


   hr = ReadEEPROM ( &dpEEPROM );
   if ( FAILED( hr ) ) {
      g_xv.DrawText("FAILED to read EEPROM, returned %x", hr );
      hr = E_FAIL;
      goto c0;
   } else
      g_xv.DrawText("SUCCEEDED in reading EEPROM");

   FileToDrive(SZDATAFILE, dpEEPROM.byteData, sizeof(dpEEPROM.byteData), APPEND_FILE );
   sprintf(buffer,"\n-----------------------------------------------------\n");
   FileToDrive( SZDATAFILE, buffer,strlen( buffer ), APPEND_FILE);


   // Send the EEPROM over.
   if ( SOCKET_ERROR == cnc.SendData(&dpEEPROM, sizeof(dpEEPROM))) {
      hr = E_FAIL;
      goto c0;
   }
   c0:
   if (0 != s ) {
      closesocket(s);
      s = 0;
   }

   return hr;
}



HRESULT
LockHardDrive(IN SOCKET s, 
              IN BOOL bLock)
{

   CNetWorkConnection cnc( s );
   DATA_PACKET dpOut= {0};
   char buffer[256];
   HRESULT hr = S_OK;

   if (TRUE == bLock) {
      g_xv.DrawText("Locking Harddrive.");
      VerifyPerBoxData();
      // Send the operation over.
      sprintf(dpOut.szMessage,"%s","Locking Hardrive");
      if ( SOCKET_ERROR == cnc.SendData(&dpOut, sizeof(dpOut))) {
         hr = E_FAIL;
         goto c0;
      }

   } else if ( FALSE == bLock) {
      g_xv.DrawText("UnLocking Harddrive.");
      UnlockXboxHardDrive( NULL, 0 );
      // Send the operation over.
      sprintf(dpOut.szMessage,"%s","UnLocking Hardrive");
      if ( SOCKET_ERROR == cnc.SendData(&dpOut, sizeof(dpOut))) {
         hr = E_FAIL;
         goto c0;
      }
   } else {
      g_xv.DrawText("Nothing.");
   }

   c0:
   if (0 != s ){
      closesocket(s);
      s = 0;
   }
   return S_OK;
}


BOOL
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
                    FILE_SHARE_WRITE,
                    NULL,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
   ReadFile( h, 
             bFile,
             dwNumberOfBytesToRead,
             &dwNumberOfBytesRead, 
             NULL );
   CloseHandle(h);

   return TRUE;
}

BOOL
StartThread( LPTHREAD_START_ROUTINE lpStartAddress, 
             LPVOID Param )
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    5/7/2001

    Routine Description:
        This routine creates a thread.
    Arguments:
        The Thread Function address.
        The Paremer to the function.
    Return Value:

--*/

{
   HANDLE   hThread;
   hThread = CreateThread(NULL,
                          0,
                          lpStartAddress,
                          (LPVOID)Param,
                          0,
                          NULL);
   if (hThread == NULL) {
      g_xv.DrawText("CreateThread() failed: %d", GetLastError());
      return FALSE;
   }
   CloseHandle(hThread);
   return TRUE;
}






BOOL
WriteEEPROM ( IN PDATA_PACKET dpData )
{

   BYTE bSaveEEPROMMemory[EEPROM_TOTAL_MEMORY_SIZE];
   BYTE bQueryEEPROMMemory[EEPROM_TOTAL_MEMORY_SIZE];
   EEPROM_LAYOUT* eeprom;
   XBOX_FACTORY_SETTINGS* factorySettings;
   XBOX_USER_SETTINGS* userSettings;
   PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
   NTSTATUS status;
   DWORD type, size;
   BOOL bResult;
   // Copy the databuffer to the EEPROM structure.
   RtlCopyMemory(bSaveEEPROMMemory, dpData->byteData, EEPROM_TOTAL_MEMORY_SIZE );

   eeprom = (EEPROM_LAYOUT*) bSaveEEPROMMemory;
   factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
   userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
   pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;

   
   // write EEPROM Data
   status = ExSaveNonVolatileSetting( XC_MAX_ALL, REG_BINARY, bSaveEEPROMMemory, EEPROM_TOTAL_MEMORY_SIZE);
   if (!NT_SUCCESS(status)) {
      g_xv.DrawText("ExSaveNonVolatileSetting FAILED");
      return E_FAIL;
   }
   
   // read EEPROM Data, this will ensure that what we write is the same as what we have in 
   // the EEPROM.
   RtlZeroMemory(bQueryEEPROMMemory,EEPROM_TOTAL_MEMORY_SIZE);
   status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, bQueryEEPROMMemory, PACKET_DATA_SIZE, &size);
   if (!NT_SUCCESS(status)) {
      g_xv.DrawText("ExQueryNonVolatileSetting FAILED");
      return E_FAIL;
   }
   dpData->ulDataSize = size;

   if( TRUE == RtlEqualMemory(bQueryEEPROMMemory,bSaveEEPROMMemory,EEPROM_TOTAL_MEMORY_SIZE)){
      return S_OK;
   }else
      return S_OK;
}



HRESULT
ReadEEPROM ( IN PDATA_PACKET dpData )
{

   BYTE bEEPROMMemory[EEPROM_TOTAL_MEMORY_SIZE];
   EEPROM_LAYOUT* eeprom;
   XBOX_FACTORY_SETTINGS* factorySettings;
   XBOX_USER_SETTINGS* userSettings;
   PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
   NTSTATUS status;
   DWORD type, size;
   BOOL bResult;

   eeprom               = (EEPROM_LAYOUT*) bEEPROMMemory;
   factorySettings      = (XBOX_FACTORY_SETTINGS*)    &eeprom->FactorySection;
   userSettings         = (XBOX_USER_SETTINGS*)       &eeprom->UserConfigSection;
   pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA)  &eeprom->EncryptedSection;
   
   // read EEPROM Data
   status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, bEEPROMMemory, EEPROM_TOTAL_MEMORY_SIZE, &size);
   if (!NT_SUCCESS(status)) {
      g_xv.DrawText("ExQueryNonVolatileSetting FAILED");
      dpData->ulDataSize = 0;
      return E_FAIL;
   }
    
   CONST PCHAR SZDATAFILE = "t:\\WhatWasSent.log";
   // DeMorph the OnlineKey before returning it.
   // Simply put, we call morph a second time.
   // TODO: For now, the hardDrivekey is still not encrypted so we can pass it as such.
   MorphKeyByHardDriveKey( factorySettings->OnlineKey,sizeof( factorySettings->OnlineKey));
   // Copy the EEPROM over to the databuffer.
   RtlCopyMemory(dpData->byteData, eeprom, sizeof(bEEPROMMemory));
   return S_OK;
}


VOID
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

   switch(dwWriteFlags)
   {
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
                      FILE_SHARE_READ, 
                      NULL,
                      dwFileStyle, 
                      FILE_ATTRIBUTE_NORMAL, 
                      NULL);

   if (hFile == INVALID_HANDLE_VALUE) {
      g_xv.DrawText("%s:unable to open %s, error %X\n", szOperation,lpFileName, GetLastError());
      return;
   }

   dwPos = SetFilePointer( hFile, 0, NULL, dwFilePosition );
   if ( dwPos == INVALID_SET_FILE_POINTER ) {
      g_xv.DrawText("%s: unable to seek %s, error %X\n", szOperation, lpFileName, GetLastError());
      return;        
   }

   if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) || (dwBytesWrote != dwFileSize)) {
     g_xv.DrawText("%s: cannot write to %s, error %X\n", szOperation, lpFileName, GetLastError() );
      return;
   }
   CloseHandle(hFile);
}
