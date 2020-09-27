//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include "2kIdMaster.h"
#include "2kId_Tests.h"
#include "tchar.h"
#include "CNetwork.h"
#include "..\\include\\fileformat.h"


VOID 
C2kIdTests::
SetDoRemote(IN BOOL b ) 
{
   m_bDoRemote = b; 

   if (FALSE == m_bDoRemote) {
      printf ("Local Provider Initializing.\n");
      //Start up the local crypto.
      InitializeProvider();
   } else {
      printf ("Remoting to NCipher chosen.\n");
   }
}
 
BOOL
C2kIdTests::
TestMACAddress( VOID )
{

   HRESULT hr;
   DWORD GameRegion = XC_GAME_REGION_NA;
   DWORD AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
   BYTE MACAddress[ETHERNET_MAC_ADDR_LEN] = { 0x66,0x66,0x66,0x66,0x66,0x66};
   char XboxSerialNumber[XBOX_SERIAL_NUMBER_LEN] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
   char HardDriveKey[HARD_DRIVE_KEY_LEN];
   char RecoveryKey[RECOVERY_KEY_LEN];
   DWORD dwOnlineKeyVersion = 0;
   char OnlineKey[ONLINE_KEY_PKENC_SIZE];
   BYTE buffer[512];
   DWORD dwBufferSize = sizeof(buffer);
   INT j;
   
   system("cls");
   printf ("\t++++++++++++++++++++++++++++++\n");
   printf ("\t+    MAC Address Loop        +\n");
   printf ("\t++++++++++++++++++++++++++++++\n\n");

   for (j = 1; j < 1000; j++) {

      MACAddress[0] = (BYTE)(j % 0xff);
      MACAddress[1] = (BYTE)(j % 0xff);
      MACAddress[2] = (BYTE)(j % 0xff);
      MACAddress[3] = (BYTE)(j % 0xff);
      MACAddress[4] = (BYTE)(j % 0xff);
      MACAddress[5] = (BYTE)(j % 0xff);


      dwBufferSize = sizeof(buffer);
      hr = GeneratePerBoxData (
                              GameRegion,
                              AVRegion,
                              MACAddress,
                              XboxSerialNumber,
                              HardDriveKey,
                              RecoveryKey,
                              &dwOnlineKeyVersion,
                              OnlineKey,
                              buffer,
                              &dwBufferSize );

      if ( FAILED( hr ) ) {
         printf("Failed GeneratePerBoxData - [%x]\n", hr);
         return FALSE;
      }
      printf("\r");
   }
   return TRUE;
}



BOOL
C2kIdTests::
TestMemLeakage( )
{

   HRESULT hr;
   BYTE buffer[100];
   DWORDLONG j;
   MEMORYSTATUSEX ms;
   BOOL b;

   // Get initial memory.
   ms.dwLength = sizeof (ms);
   GlobalMemoryStatusEx( &ms );
   DWORDLONG dwlInitialMemory = ms.ullAvailPhys/1024; // in KBytes
   DWORDLONG dwlInitialCommittedMemoryCharge = (ms.ullTotalPageFile -  ms.ullAvailPageFile)/1024; // in KBytes.
   DWORDLONG dwlCommittedMemoryCharge;

   system("cls");
   printf ("\t\t++++++++++++++++++++++++++++++\n");
   printf ("\t\t+ Memory Leak Detection Test +\n");
   printf ("\t\t++++++++++++++++++++++++++++++\n\n");
   printf ("            Committed Charge   Free Physical  Loop   \n" );
   printf ("            ----------------   -------------  -------\n" );
   printf ("INITIALLY:  %*I64d %sB         %*I64d %sB      0      \n", 
           7, dwlInitialCommittedMemoryCharge, "K",
           7, dwlInitialMemory, "K");


   for (j = 0; j < 100000000; j++) {


      hr = GenerateID( buffer,sizeof(buffer)  );
      if ( FAILED( hr ) ) {
         printf("Failed GeneratePerBoxData - [%x]\n", hr);
         return FALSE;
      }

      ms.dwLength = sizeof (ms);
      b = GlobalMemoryStatusEx( &ms );
      if ( !b ) return FALSE;

      // Calculate the committed charge. 
      dwlCommittedMemoryCharge = (ms.ullTotalPageFile -  ms.ullAvailPageFile)/1024; // in KBytes.

      printf ("CURRENTLY:  %*I64d %sB         %*I64d %sB      %I64d  ", 
              7, dwlCommittedMemoryCharge, "K",
              7,  ms.ullAvailPhys/1024, "K", j);


      if ( dwlCommittedMemoryCharge  >  dwlInitialCommittedMemoryCharge + 10000) {
         // if memory is above dwlInitialCommittedMemoryCharge by 10000 Kbytes then fail it as leaking.
         printf ("\n\nThere is a cumulative leak of over 10000 bytes in this loop. FAILED\n");
         return FALSE;
      }
      printf("\r");
   }
   return TRUE;
}




BOOL
C2kIdTests::
TestXClient( DWORD dwFlag )
{

   DWORD j;
   INT   ret;
   DATA_PACKET  SendData;
   DATA_PACKET  RecvData;
   char  szIp[20] = "";
   BOOL bResult;
   HRESULT hr;
   
   UNREFERENCED_PARAMETER(dwFlag);

   if (m_dwTestLoop == 0) m_dwTestLoop = 1;

   for (j = 0; j < m_dwTestLoop; j++) {
      
      szIp[0] = 0;
      printf("Do Xbox\n");

      printf("\n-------------------START SESSION %lu----------------\n",j);
      printf("[STATUS]\t\t[Description of Action]\n");

      CNetWork cn( szIp, m_szXboxName, COMMUNICATION_PORT );
      
      ZeroMemory( (PDATA_PACKET)&SendData, sizeof( SendData )); 
      ZeroMemory( (PDATA_PACKET)&RecvData, sizeof( RecvData )); 
      
      DWORD GameRegion = XC_GAME_REGION_NA;

      DWORD AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
      BYTE MACAddress[ETHERNET_MAC_ADDR_LEN] = { 0x1A,0x2A,0x3A,0x4A,0x5A,0x6A};
      char XboxSerialNumber[XBOX_SERIAL_NUMBER_LEN] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
      char RecoveryKey[RECOVERY_KEY_LEN];
      DWORD dwOnlineKeyVersion = 0;
      
      char OnlineKeyEncryptedBase64Encoded[173];
      BYTE OnlineKeyEncrypted             [131];
      BYTE OnlineKey                      [17];
      ULONG ulOnlineKey = 0;
      char HardDriveKeyBase64Encoded      [25];
      BYTE HardDriveKey                   [20];
      ULONG ulHardDriveKey = 0;

      ZeroMemory(OnlineKeyEncryptedBase64Encoded, 173);
      ZeroMemory(OnlineKeyEncrypted, 131);
      ZeroMemory(HardDriveKeyBase64Encoded, 25);
      ZeroMemory(HardDriveKey, 20);
      ZeroMemory(OnlineKey, 17);


      if (j % 2) {
        GameRegion = XC_GAME_REGION_JAPAN;
      } else
        GameRegion = XC_GAME_REGION_NA;


      DWORD dwOutBufferSize;
      dwOutBufferSize = PACKET_DATA_SIZE; 
      
      printf("  OK\t\t\tGenerating Data\n");
      hr = GeneratePerBoxData ( GameRegion,
                                AVRegion,
                                MACAddress,
                                XboxSerialNumber,
                                HardDriveKeyBase64Encoded,
                                RecoveryKey,
                                &dwOnlineKeyVersion,
                                OnlineKeyEncryptedBase64Encoded,
                                SendData.byteData,
                                &dwOutBufferSize );
      if ( FAILED( hr ) ) {
         if ( hr == E_INVALIDARG )
            printf("  NOT_OK\t\tFailed GeneratePerBoxData - [%x]\n", E_INVALIDARG);
         else
            printf("  NOT_OK\t\tFailed GeneratePerBoxData - [%x]\n", hr);
         return FALSE;
      }
     /*
      FileToDrive( "c:\\w.dat",  
             OnlineKeyEncryptedBase64Encoded, 
             172,
             NEW_FILE );

      FileToDrive( "c:\\wireEEPROM.dat",  
                   SendData.byteData, 
                   EEPROM_TOTAL_MEMORY_SIZE, // from xconfig.h = 256,
                   NEW_FILE );
     */
      
      //-------- ONLINE KEY RECOVERY  -------------------------------------      
      // Base64 decode to shave off the 172 bytes down to 128 bytes.
      bResult = Base64Decode( OnlineKeyEncryptedBase64Encoded,
                              172,
                              OnlineKeyEncrypted,
                              &ulOnlineKey );
      assert(bResult == TRUE);
      assert( ulOnlineKey == 128 );
      if ( !bResult ) {
         printf("  NOT_OK\t\t\tOnlineKey base64Decoding failed.\n");
         return FALSE;
      }
      if (TRUE == m_bDoRemote) {
         RemoteNCipherDecryptKeys( OnlineKeyEncrypted, OnlineKey, sizeof(OnlineKey) ); 
      } else {
         DecryptOnlineKey( OnlineKeyEncrypted, OnlineKey, sizeof(OnlineKey));
      }
      //-------- HARDDRIVE KEY RECOVERY ------------------------------------      
      // Base64 decode to shave off the 24 bytes down to 16 bytes.
      bResult = Base64Decode( HardDriveKeyBase64Encoded,
                              24,
                              HardDriveKey, // This must be at least BASE64_DECODE_BUFFER_SIZE( ulSrcSize ) ((((ulSrcSize) / 4) * 3) + 1)
                              &ulHardDriveKey );
      assert( ulHardDriveKey == 16 );
      if ( !bResult ) {
         printf("  NOT_OK\t\t\tHardDriveKey Base64Decoding failed.\n");
         return FALSE;
      }
      
      //-------- START TRANSPORT DATA -------------------------------------      
      // Send the Command Block 
      sprintf( SendData.szMessage,"Command Block");
      SendData.dwCommand = DO_VERIFY_WRITEPERBOXDATA;
      if (SOCKET_ERROR == (ret = cn.SendData( &SendData, sizeof(SendData)))){
         return FALSE;
      }

      // Send the data 
      sprintf( SendData.szMessage,"Generated BINARY ID");
      SendData.ulDataSize = dwOutBufferSize; 
      if (SOCKET_ERROR == (ret = cn.SendData( &SendData, sizeof(SendData)))){
         return FALSE;
      }


      // Wait for acknowledgement of EEPROM from the XBE
      if (SOCKET_ERROR == ( ret = cn.RecvData(&RecvData, sizeof(RecvData)))) {
         printf("  NOT_OK\t\t\tReceiving EEPROM Written file failed\n");
         return FALSE;
      } else {
         printf("  OK\t\t\tReceiving EEPROM Written file.\n");
      }


      // Dump the EEPROMs data to a file.
      //WriteFileOut( "EEPROM.dat",  RecvData.byteData, EEPROM_TOTAL_MEMORY_SIZE /* from xconfig.h = 256*/ );


      // Carve up the returned memory into the layout we believe is there.
      // First copy it so its persistent over next connection.
      BYTE bEEPROMData[EEPROM_TOTAL_MEMORY_SIZE];

      RtlCopyMemory(bEEPROMData,RecvData.byteData,EEPROM_TOTAL_MEMORY_SIZE);
      EEPROM_LAYOUT* pEEPROM = (EEPROM_LAYOUT*)bEEPROMData;
      
      XBOX_FACTORY_SETTINGS* pxfs = ( XBOX_FACTORY_SETTINGS* )pEEPROM->FactorySection; 
      XBOX_USER_SETTINGS*    pxus = ( XBOX_USER_SETTINGS* )pEEPROM->UserConfigSection;
      // Currently not encrypted on a dev kit. We will unmorph the Onlinekey on the 
      // XBE server end before getting back here. Sinces the MorphFunction is an
      // XBE library. The rom *isn't* totally the same one as directly read off the 256byte
      // EEPROM.
      PXBOX_ENCRYPTED_EEPROM_DATA pxeed = ( PXBOX_ENCRYPTED_EEPROM_DATA )pEEPROM->EncryptedSection;
      // Comparisons of EEPROM datas
      //-------- COMPARISON of EEPROM DATA --------------------------------      
      
      // XboxOnlineKey
      // ONLINE_KEY_LEN  16
      printf("\t[  *  IMPORTANT  *  ]\n");
      printf("\t[Checking Encrypted Base64 Encoded Online Key matches EEPROMS]\n");
      
      // The catch if the machine is a dev kit.
      if (TRUE == RecvData.bDevKit) {
         RtlZeroMemory(HardDriveKey,HARD_DRIVE_KEY_LEN);
      }

      
      if (TRUE == RtlEqualMemory(pxfs->OnlineKey,
                                   OnlineKey, ONLINE_KEY_LEN)) {
         printf("  OK\t\t\tOnline Key\n");
      } else {
         printf("  NOT_OK\t\t\tOnline Key\n");
         printf("  Dumping Online Key returned for storage from GenPerBoxData\n     ");
         DumpBytesHex(OnlineKey, ONLINE_KEY_LEN );
         printf("\n");
         printf("  Dumping EEPROMS Online Key\n     ");
         DumpBytesHex(pxfs->OnlineKey, ONLINE_KEY_LEN );
         printf("\n");
      }
      
      // HardDriveKey
      // HARD_DRIVE_KEY_LEN  16
      if (TRUE == RtlEqualMemory(pxeed->HardDriveKey,
                                 HardDriveKey, HARD_DRIVE_KEY_LEN)) {
         printf("  OK\t\t\tHardDrive Key\n");
      } else {
         printf("  NOT_OK\t\t\tHardDrive Key\n");
         printf("  Dumping HardDrive Key\n");
         DumpBytesHex(HardDriveKey, HARD_DRIVE_KEY_LEN );
         printf("\n");
         printf("  Dumping EEPROM HardDrive Key\n\n");
         DumpBytesHex(pxeed->HardDriveKey, HARD_DRIVE_KEY_LEN );
         printf("\n");
      }
      // This is what was sent to WritePerBoxData. It use to come back decrypted.
      // and that isn't a good thing. This has been bugged.
      if (SOCKET_ERROR == ( ret = cn.RecvData(&RecvData, sizeof(RecvData))))
         return FALSE;
      //-------- END   TRANSPORT DATA -------------------------------------      
     
 
      // This is returning what we sent over.
      PXBOX_ENCRYPTED_PER_BOX_DATA pxRecvData =  (PXBOX_ENCRYPTED_PER_BOX_DATA)
                                             (RecvData.byteData + sizeof(RC4_SHA1_HEADER));
      // This is a pointer to what is sent.
      PXBOX_ENCRYPTED_PER_BOX_DATA pxSendData =  (PXBOX_ENCRYPTED_PER_BOX_DATA)
                                             (SendData.byteData + sizeof(RC4_SHA1_HEADER));

      // Do comparision and decoding of the information returned.
      // Decrypt the data blob from the GeneratePerBoxId and compare.
      bResult = Decrypt(SendData.byteData, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA));
      assert( bResult == TRUE );
      if ( !bResult ) {
         printf("  NOT_OK\t\t\tDecrypting the sent data ID failed\n");
         return FALSE;
      }else
         printf("  OK\t\t\tDecrypting the sent data ID from GeneratePerBoxData\n");

      // Decrypt the returned data blob from the WritePerBoxData.
      bResult = Decrypt(RecvData.byteData, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA));
      assert( bResult == TRUE );
      if ( !bResult ) {
         printf("  NOT_OK\t\t\tDecrypting the recv data ID failed\n");
         return FALSE;
      }else
         printf("  OK\t\t\tDecrypting the sent recv ID from GeneratePerBoxData\n");


      // We'll compare the sent buffer to the returned buffer to make sure that 
      // the code does not decrypted the sent buffer. It used to.
      if (TRUE == RtlEqualMemory(pxRecvData,
                                 pxSendData, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA))) {
         printf("  OK\t\t\tSent Buffer NOT decrypted by WritePerBoxData\n");
      } else {
         printf("  NOT_OK\t\t\tSent Buffer IS decrypted by WritePerBoxData\n");
      }

      // Testing what we have on this end to what is on the EEPROM.
      // MAC Address
      if (TRUE == RtlEqualMemory(pxSendData->MACAddress,
                                 pxfs->EthernetAddr, sizeof(pxSendData->MACAddress))) {
         printf("  OK\t\t\tMAC Address\n");
         printf("  \t\t\tOriginal MACAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                                    pxSendData->MACAddress[0],
                                    pxSendData->MACAddress[1],
                                    pxSendData->MACAddress[2],
                                    pxSendData->MACAddress[3],
                                    pxSendData->MACAddress[4],
                                    pxSendData->MACAddress[5] );
         printf("  \t\t\tEEPROM   MACAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                                    pxfs->EthernetAddr[0],
                                    pxfs->EthernetAddr[1],
                                    pxfs->EthernetAddr[2],
                                    pxfs->EthernetAddr[3],
                                    pxfs->EthernetAddr[4],
                                    pxfs->EthernetAddr[5] );
      }else{
         printf("  NOT_OK\t\t\tMAC Address\n");
         printf("  \t\t\tOriginal MACAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                                    pxSendData->MACAddress[0],
                                    pxSendData->MACAddress[1],
                                    pxSendData->MACAddress[2],
                                    pxSendData->MACAddress[3],
                                    pxSendData->MACAddress[4],
                                    pxSendData->MACAddress[5] );
         printf("  \t\t\tEEPROM   MACAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                                    pxfs->EthernetAddr[0],
                                    pxfs->EthernetAddr[1],
                                    pxfs->EthernetAddr[2],
                                    pxfs->EthernetAddr[3],
                                    pxfs->EthernetAddr[4],
                                    pxfs->EthernetAddr[5] );
      }
      
      // Clear the Highbit.
      pxeed->GameRegion ^= XC_GAME_REGION_MANUFACTURING;
      if (TRUE == RtlEqualMemory(&pxSendData->GameRegion,
                                 &pxeed->GameRegion, sizeof(pxeed->GameRegion))) {
         printf("  OK\t\t\tGameRegion\n");
         printf("\t\tGameRegion SEND: %lu\n", pxSendData->GameRegion );
         printf("\t\tGameRegion EEPM: %lu\n", pxeed->GameRegion );
      
      }else{
         printf("  NOT_OK\t\tGameRegion\n");
         printf("GameRegion SEND: %lu\n", pxSendData->GameRegion );
         printf("GameRegion EEPM: %lu\n", pxeed->GameRegion );
      }


      // Language settings must match for Japanese etc.
      // Everywhere else its English.
      if ( GameRegion == XC_GAME_REGION_JAPAN) {
         if (pxus->Language == XC_LANGUAGE_JAPANESE) {
            printf("  OK\t\t\tLanguage Settings Japanese\n");
            printf("  \t\t\tJapanese Language for Japanese GameRegion: %lu\n", 
                   pxus->Language, pxeed->GameRegion  );
         } else {
            printf("  NOT_OK\t\tLanguageSettings\n");
            printf("  \t\t\tJapanese Language not set Japanese GameRegion: %lu\n", 
                   pxus->Language, pxeed->GameRegion );
         }

      }else{
         if (pxus->Language == 0 ){ // default is ZERO no english XC_LANGUAGE_ENGLISH) {
            printf("  OK\t\t\tLanguage Settings NO language set\n");
         } else {
            printf("  NOT_OK\t\tLanguageSettings has a language set FAILED.\n");
         }
         printf("  \t\t\tNO Language set : GameRegion: %lu\n", 
                pxeed->GameRegion );
      }
      
      printf("\n-------------------END   SESSION--------------------\n");
      Sleep(500);
   }
   return TRUE;
}



BOOL
C2kIdTests::
TestLockHardDrive( IN DWORD dwControl )
{
   
   
   DATA_PACKET  SendData = {0};
   DATA_PACKET  RecvData = {0};
   char  szIp[20] = "";
   SOCKET ret;

   if (dwControl == 1) {
      SendData.dwCommand = DO_LOCK_HARDDRIVE;
   } else {
      SendData.dwCommand = DO_UNLOCK_HARDDRIVE;
   }

   CNetWork cn( szIp, m_szXboxName, COMMUNICATION_PORT );
   printf("\n-------------------START SESSION--------------------\n");
   printf("[STATUS]\t\t[Description of Action]\n" );

   //-------- START TRANSPORT DATA -------------------------------------      
   // Send the Command Block 
   sprintf( SendData.szMessage,"Command Block");
   if (SOCKET_ERROR == (ret = cn.SendData( &SendData, sizeof(SendData)))) {
      return FALSE;
   }

   // Receive the Message
   if (SOCKET_ERROR == ( ret = cn.RecvData(&RecvData, sizeof(RecvData))))
      return FALSE;

   printf("\n-------------------END   SESSION--------------------\n");
   Sleep(500);

   return TRUE;
}



BOOL
C2kIdTests::
ReadRemotesEEPROM( DWORD dwFlag )
{

   UNREFERENCED_PARAMETER(dwFlag);
   DWORD j;
   DATA_PACKET  SendData;
   DATA_PACKET  RecvData;
   char  szIp[20] = "";
   char  szFileName[260]= "";
   SOCKET ret;

   if (m_dwTestLoop == 0) 
      m_dwTestLoop =1;

   for (j = 0; j < m_dwTestLoop; j++) {
      szIp[0] = NULL;
      CNetWork cn( szIp, m_szXboxName, COMMUNICATION_PORT );


      printf("\n-------------------START SESSION--------------------\n");
      printf("[STATUS]\t\t[Description of Action]\n");

      ZeroMemory( (PDATA_PACKET)&SendData, sizeof( SendData )); 
      ZeroMemory( (PDATA_PACKET)&RecvData, sizeof( RecvData )); 
      
      char OnlineKeyEncryptedBase64Encoded[173];
      BYTE OnlineKeyEncrypted             [131];
      BYTE OnlineKey                      [17];
      // ULONG ulOnlineKey = 0;
      char HardDriveKeyBase64Encoded[25];
      BYTE HardDriveKey                   [20];

      ZeroMemory(OnlineKeyEncryptedBase64Encoded, 173);
      ZeroMemory(OnlineKeyEncrypted, 131);
      ZeroMemory(HardDriveKeyBase64Encoded, 25);
      ZeroMemory(HardDriveKey, 20);
      ZeroMemory(OnlineKey, 17);

      //-------- START TRANSPORT DATA -------------------------------------      
      // Send the Command Block 
      sprintf( SendData.szMessage,"Command Block");
      SendData.dwCommand = DO_SENDBACK_EEPROM;
      if (SOCKET_ERROR == (ret = cn.SendData( &SendData, sizeof(SendData)))){
         return FALSE;
      }

      // Receive the EEPROM
      if (SOCKET_ERROR == ( ret = cn.RecvData(&RecvData, sizeof(RecvData))))
         return FALSE;
      sprintf( szFileName,"EEPROM%s.dat", szIp);
      
      EEPROM_LAYOUT* eeprom = (EEPROM_LAYOUT*) RecvData.byteData;
      //XBOX_FACTORY_SETTINGS* factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
      XBOX_USER_SETTINGS* userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
      PXBOX_ENCRYPTED_EEPROM_DATA  pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;
      printf("Language is %lu\n", userSettings->Language);
      if (pEncryptedEEPROMData->GameRegion & XC_GAME_REGION_MANUFACTURING ) {
         // Clear out the GameRegion Bit.
         pEncryptedEEPROMData->GameRegion ^= XC_GAME_REGION_MANUFACTURING;
      }
      printf("GameRegion is %lu\n", pEncryptedEEPROMData->GameRegion);

      FileToDrive(szFileName, RecvData.byteData, 256 , NEW_FILE );
      // Read Info from File and compare.
      printf("\n-------------------END   SESSION--------------------\n");
      Sleep(500);
   }
   
   return TRUE;
}

BOOL
C2kIdTests::
TestDirectoryOfEEPROMS( IN PCHAR szDirectory,
                        IN PCHAR szDataBaseFile )
{

   //Read in the file.
   WIN32_FIND_DATA   ffd;
   HANDLE            hFind;
   TCHAR             szFindFiles  [ MAX_PATH ];

   BYTE bTotalDatFile[512];
   PCRITICAL_KEY_PAIR pckp;

   BYTE bEEPROMMemory[EEPROM_TOTAL_MEMORY_SIZE];
   EEPROM_LAYOUT*  eeprom                 = (EEPROM_LAYOUT*)bEEPROMMemory;
   XBOX_FACTORY_SETTINGS* factorySettings = (XBOX_FACTORY_SETTINGS*)    &eeprom->FactorySection;
   XBOX_USER_SETTINGS* userSettings       = (XBOX_USER_SETTINGS*)       &eeprom->UserConfigSection;
   PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA)&eeprom->EncryptedSection;

   BOOL bResult;
   BYTE OnlineKeyEncryptedBase64Encoded[172];
   BYTE OnlineKeyEncrypted             [131];
   BYTE OnlineKey                      [17];
   ULONG ulOnlineKey = 0;

   ZeroMemory(OnlineKeyEncrypted, 131);
   ZeroMemory(OnlineKey, 17);
   CHAR szSerialNumber[12];


   _stprintf(szFindFiles,TEXT("%s\\*.dat"),szDirectory);
   if ( (hFind = FindFirstFile(szFindFiles, &ffd) ) == INVALID_HANDLE_VALUE) {
      _tprintf(TEXT("Could not find any files in %s!!\n"), szDirectory );
      return FALSE;
   }
   // else found files.

   do {
      if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
         _tprintf(TEXT("%s\\%s\n"),szDirectory,ffd.cFileName);
         // Perform task.
         _stprintf(szFindFiles,TEXT("%s\\%s"),szDirectory,ffd.cFileName);
         ZeroMemory( eeprom, EEPROM_TOTAL_MEMORY_SIZE);

         ReadFileIntoBuffer( szFindFiles, (BYTE*)&bTotalDatFile,  400 );
         RtlCopyMemory( eeprom, bTotalDatFile, EEPROM_TOTAL_MEMORY_SIZE );
         
         LPBYTE pEcryptedCriticalPair = bTotalDatFile + EEPROM_TOTAL_MEMORY_SIZE; 
         bResult=
         rc4HmacDecrypt( (LPBYTE)DAT_ENCRYPTION_KEY, 
                         DAT_ENCRYPTION_KEY_LEN,
                         (LPBYTE)(pEcryptedCriticalPair + sizeof(RC4_SHA1_HEADER)),
                         sizeof(CRITICAL_KEY_PAIR),
                         pEcryptedCriticalPair);

         if (FALSE == bResult) {
            _tprintf("Error Decrypting.\n");
         }
         pckp = (PCRITICAL_KEY_PAIR)(pEcryptedCriticalPair + sizeof(RC4_SHA1_HEADER)); 
         
         _tprintf( "HardDriveKey ");
         DumpBytesHex( pckp->HardDriveKey, 16 );
         if (pEncryptedEEPROMData->GameRegion & XC_GAME_REGION_MANUFACTURING ) {
            // Clear out the GameRegion Bit.
            pEncryptedEEPROMData->GameRegion ^= XC_GAME_REGION_MANUFACTURING;
         }
         _tprintf( "GameRegion %lu\n",pEncryptedEEPROMData->GameRegion);
         _tprintf( "SerialNumber ");
         DumpBytesChar( factorySettings->SerialNumber, 12);
         sprintf(szSerialNumber,"%0.12s", factorySettings->SerialNumber);

         if (szDataBaseFile != NULL) {
            // Get Serial From Database
            BOOL b = ScanInformation( szDataBaseFile, 
                                      szSerialNumber,
                                      (char*)OnlineKeyEncryptedBase64Encoded );
            ZeroMemory(OnlineKey,16);
            if (b == TRUE) {
               // Base64 decode to shave off the 172 bytes down to 128 bytes.
               bResult = Base64Decode( (char*)OnlineKeyEncryptedBase64Encoded,
                                       172,
                                       OnlineKeyEncrypted,
                                       &ulOnlineKey );
               assert(bResult == TRUE);
               assert( ulOnlineKey == 128 );
               if ( !bResult ) {
                  printf("  NOT_OK\t\tOnlineKey base64Decoding failed %lu .\n", ulOnlineKey);
                  return FALSE;
               }

               if (TRUE == m_bDoRemote) {
                  RemoteNCipherDecryptKeys( OnlineKeyEncrypted,
                                            OnlineKey, sizeof(OnlineKey) ); 
               } else {
                  DecryptOnlineKey( OnlineKeyEncrypted, OnlineKey, sizeof(OnlineKey));
               }
            }

            _tprintf( "EthernetAddress ");
            DumpBytesHex( factorySettings->EthernetAddr, 6);
            _tprintf( "Online Key ");
            //DumpBytesHex( factorySettings->OnlineKey, 16);
            if (TRUE == RtlEqualMemory(pckp->OnlineKey, OnlineKey, 16 )) {
               printf("  OK\t\t\tOnline Keys Match\n");
            } else {
               printf("  NOT_OK\t\t\tOnline Keys Do Not Match\n");
            }
            _tprintf( "DATABASE:\n");
            DumpBytesHex( OnlineKey, 16);
         }
         _tprintf( "EEPROM:\n");
         DumpBytesHex( pckp->OnlineKey, 16);
         _tprintf( "AVRegion is %s\n", AvRegionBitTest (factorySettings->AVRegion));
         _tprintf( "Language %lu\n\n\n",userSettings->Language );
      }

   }while ( FindNextFile(hFind, &ffd));
    FindClose(hFind);
   return TRUE;
}


BOOL
C2kIdTests::
TestDirectoryOfTonyChensEEPROMS( 
   IN PCHAR szDirectory,
   IN PCHAR szDataBaseFile 
)
{

   //Read in the file.
   WIN32_FIND_DATA   ffd;
   HANDLE            hFind;
   TCHAR             szFindFiles  [ MAX_PATH ];

   BYTE bTotalDatFile[300];
   //PCRITICAL_KEY_PAIR pckp;

   BYTE bEEPROMMemory[EEPROM_TOTAL_MEMORY_SIZE];
   EEPROM_LAYOUT*  eeprom                 = (EEPROM_LAYOUT*)bEEPROMMemory;
   XBOX_FACTORY_SETTINGS* factorySettings = (XBOX_FACTORY_SETTINGS*)    &eeprom->FactorySection;
   //XBOX_USER_SETTINGS* userSettings       = (XBOX_USER_SETTINGS*)       &eeprom->UserConfigSection;
   //PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA)&eeprom->EncryptedSection;

   BOOL bResult;
   BYTE OnlineKeyEncryptedBase64Encoded[172];
   BYTE OnlineKeyEncrypted             [131];
   BYTE OnlineKey                      [16];
   ULONG ulOnlineKey = 0;

   ZeroMemory(OnlineKeyEncrypted, 131);
   ZeroMemory(OnlineKey, 17);
   CHAR szSerialNumber[12];

   _stprintf(szFindFiles,TEXT("%s\\*.dat"),szDirectory);
   if ( (hFind = FindFirstFile(szFindFiles, &ffd) ) == INVALID_HANDLE_VALUE) {
      _tprintf(TEXT("Could not find any files in %s!!\n"), szDirectory );
      return FALSE;
   }
   // else found files.

   do {
      if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
         _tprintf(TEXT("%s\\%s\n"),szDirectory,ffd.cFileName);
         // Perform task.
         _stprintf(szFindFiles,TEXT("%s\\%s"),szDirectory,ffd.cFileName);
         ZeroMemory( eeprom, EEPROM_TOTAL_MEMORY_SIZE);
         ReadFileIntoBuffer( szFindFiles, (BYTE*)&bTotalDatFile,  300 );
         RtlCopyMemory( eeprom, bTotalDatFile, EEPROM_TOTAL_MEMORY_SIZE );
         
         _tprintf( "SerialNumber ");
         DumpBytesChar( factorySettings->SerialNumber, 12);
         sprintf(szSerialNumber,"%0.12s", factorySettings->SerialNumber);

         if (szDataBaseFile != NULL) {
            // Get Serial From Database
            BOOL b = ScanInformationXLS( szDataBaseFile, 
                                         szSerialNumber,
                                         (char*)OnlineKeyEncryptedBase64Encoded );

            /*
            BOOL b = ScanInformation( szDataBaseFile, 
                                      szSerialNumber,
                                      (char*)OnlineKeyEncryptedBase64Encoded );
            */
            ZeroMemory(OnlineKey,16);
            // means its found in the database.
            if (b == TRUE) {
               // Base64 decode to shave off the 172 bytes down to 128 bytes.
               bResult = Base64Decode( (char*)OnlineKeyEncryptedBase64Encoded,
                                       172,
                                       OnlineKeyEncrypted,
                                       &ulOnlineKey );
               assert(bResult == TRUE);
               assert( ulOnlineKey == 128 );
               if ( !bResult ) {
                  printf("  NOT_OK\t\tOnlineKey base64Decoding failed %lu .\n", ulOnlineKey);
                  return FALSE;
               }
               if (TRUE == m_bDoRemote) {
                  RemoteNCipherDecryptKeys( OnlineKeyEncrypted,
                                            OnlineKey, sizeof(OnlineKey) ); 
               } else {
                  DecryptOnlineKey( OnlineKeyEncrypted, OnlineKey, sizeof(OnlineKey));
               }

               
               // try to decrypt the data package.
               LPBYTE pEcryptedCriticalData = bTotalDatFile + EEPROM_TOTAL_MEMORY_SIZE; 

               //#define DAT "\x3F\x5B\xF1\xA4\xDF\xD7\xEF\x49\x93\xB0\x8F\x87\x86\xC8\x70\xB5"
               bResult=
               rc4HmacDecrypt( (LPBYTE)OnlineKey, 
                               sizeof(OnlineKey),
                               (LPBYTE)(pEcryptedCriticalData + sizeof(RC4_SHA1_HEADER)),
                               16,
                               pEcryptedCriticalData);
               if (TRUE == bResult )
                  printf("  OK\t\t\tOnline Key\n");
               else
                  printf("  NOT_OK\t\t\tOnline Key\n");

            } else {
               printf("  NOT_OK\t\tOnline key is not in the database.\n");
               // Not  found in Database file.
            }

            ZeroMemory(OnlineKey,sizeof(OnlineKey));
            //_tprintf( "DATABASE: Onlinekey\n");
            //DumpBytesHex( OnlineKey, 16);
         }


      }
   
   }while ( FindNextFile(hFind, &ffd));
    FindClose(hFind);
   return TRUE;
}



BOOL
C2kIdTests::
TestRemoteDecrypt( DWORD dwFlag )
{
   DWORD j;
   DATA_PACKET  SendData;
   DATA_PACKET  RecvData;
   BYTE bWireStructure[PACKET_DATA_SIZE];
   
   char  szIp[20] ="";
   BOOL bResult;
   HRESULT hr;
   if (m_dwTestLoop == 0) 
      m_dwTestLoop =1;

   for (j = 0; j < m_dwTestLoop; j++) {
      if ( dwFlag & DO_LOCALHOST ) {
         strcpy ( szIp, "157.56.13.95" );
         m_szXboxName[0] = NULL;
         printf("Do localhost\n");
      } else {
         szIp[0] = 0;
         printf("Do Xbox\n");
      }

      printf("\n-------------------START SESSION--------------------\n");
      printf("[STATUS]\t\t[Description of Action]\n");
      ZeroMemory( (PDATA_PACKET)&SendData, sizeof( SendData )); 
      ZeroMemory( (PDATA_PACKET)&RecvData, sizeof( RecvData )); 
      
      DWORD GameRegion = XC_GAME_REGION_NA;
      DWORD AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
      BYTE  MACAddress[ETHERNET_MAC_ADDR_LEN] = { 0x66,0x66,0x66,0x66,0x66,0x66};
      char  XboxSerialNumber[XBOX_SERIAL_NUMBER_LEN] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
      char  RecoveryKey[RECOVERY_KEY_LEN];
      DWORD dwOnlineKeyVersion = 0;
      
      char OnlineKeyEncryptedBase64Encoded[173];
      BYTE OnlineKeyEncrypted             [131];
      BYTE OnlineKey                      [17];
      ULONG ulOnlineKey = 0;
      char HardDriveKeyBase64Encoded[25];
      BYTE HardDriveKey                   [20];


      ZeroMemory(OnlineKeyEncryptedBase64Encoded, 173);
      ZeroMemory(OnlineKeyEncrypted, 131);
      ZeroMemory(HardDriveKeyBase64Encoded, 25);
      ZeroMemory(HardDriveKey, 20);
      ZeroMemory(OnlineKey, 17);

      DWORD dwOutBufferSize;
      dwOutBufferSize = PACKET_DATA_SIZE; 
      
      printf("  OK\t\t\tGenerating Data\n");

      hr = GeneratePerBoxData ( GameRegion,
                                AVRegion,
                                MACAddress,
                                XboxSerialNumber,
                                HardDriveKeyBase64Encoded,
                                RecoveryKey,
                                &dwOnlineKeyVersion,
                                OnlineKeyEncryptedBase64Encoded,
                                bWireStructure,
                                &dwOutBufferSize );
      if ( FAILED( hr ) ) {
         if ( hr == E_INVALIDARG )
            printf("  NOT_OK\t\tFailed GeneratePerBoxData - [%x]\n", E_INVALIDARG);
         else
            printf("  NOT_OK\t\tFailed GeneratePerBoxData - [%x]\n", hr);
         return FALSE;
      }

      //-------- ONLINE KEY RECOVERY  -------------------------------------      
      // Base64 decode to shave off the 172 bytes down to 128 bytes.
      bResult = Base64Decode( OnlineKeyEncryptedBase64Encoded,
                              172,
                              OnlineKeyEncrypted,
                              &ulOnlineKey );
      assert(bResult == TRUE);
      assert( ulOnlineKey == 128 );
      if ( !bResult ) {
         printf("  NOT_OK\t\t\tOnlineKey base64Decoding failed.\n");
         return FALSE;
      }


      if (TRUE == m_bDoRemote) {
         RemoteNCipherDecryptKeys( OnlineKeyEncrypted, OnlineKey, sizeof(OnlineKey) ); 
      } else {
         DecryptOnlineKey( OnlineKeyEncrypted, OnlineKey, sizeof(OnlineKey));
      }


      // Carve up the returned memory into the layout we believe is there.
      // First copy it so its persistent over next connection.
      XBOX_ENCRYPTED_PER_BOX_DATA xepbd;
      Decrypt(bWireStructure, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA));
      
      RtlCopyMemory(&xepbd, bWireStructure + sizeof(RC4_SHA1_HEADER), 
                    sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) );
      

      // XboxOnlineKey
      // ONLINE_KEY_LEN  16
      printf("\t[  *  IMPORTANT  *  ]\n");
      printf("\t[Checking Encrypted Base64 Encoded Online Key matches EEPROMS]\n");
      if (TRUE == RtlEqualMemory(xepbd.OnlineKey,
                                   OnlineKey, ONLINE_KEY_LEN)) {
         printf("  OK\t\t\tOnline Key\n");
         printf("  Dumping Online Key remotely decrypted.\n");
         DumpBytesHex(OnlineKey, ONLINE_KEY_LEN );
         printf("\n");
         printf("  Dumping WireBuffer's Online Key\n");
         DumpBytesHex(xepbd.OnlineKey, ONLINE_KEY_LEN );
         printf("\n");
      } else {
         printf("  NOT_OK\t\t\tOnline Key\n");
         printf("  Dumping Online Key remotely decrypted.\n");
         DumpBytesHex(OnlineKey, ONLINE_KEY_LEN );
         printf("\n");
         printf("  Dumping WireBuffer's Online Key\n");
         DumpBytesHex(xepbd.OnlineKey, ONLINE_KEY_LEN );
         printf("\n");
      }
      printf("\n-------------------END   SESSION--------------------\n");
      Sleep(500);
   }
   
   return TRUE;
}


BOOL
C2kIdTests::
TestRemoteDecodingOfOnlineKey( IN PCHAR OnlineKeyEncryptedBase64Encoded )
{
   
   DATA_PACKET  SendData;
   DATA_PACKET  RecvData;
   BYTE bWireStructure[PACKET_DATA_SIZE];
   XBOX_ENCRYPTED_ONLINE_DATA xeod = {0};



   printf("\n-------------------START SESSION--------------------\n");
   printf("[STATUS]\t\t[Description of Action]\n");
   ZeroMemory( (PDATA_PACKET)&SendData, sizeof( SendData )); 
   ZeroMemory( (PDATA_PACKET)&RecvData, sizeof( RecvData )); 

   BYTE OnlineKeyEncrypted             [131];
   BYTE OnlineKey                      [17];
   ULONG ulOnlineKey = 0;
   ZeroMemory(OnlineKeyEncrypted, 131);
   ZeroMemory(OnlineKey, 17);


   //-------- ONLINE KEY RECOVERY  -------------------------------------      
   // Base64 decode to shave off the 172 bytes down to 128 bytes.
   BOOL 
   bResult = Base64Decode( OnlineKeyEncryptedBase64Encoded,
                           172,
                           OnlineKeyEncrypted,
                           &ulOnlineKey );

   assert(bResult == TRUE);
   assert( ulOnlineKey == 128 );
   if ( !bResult ) {
      printf("  NOT_OK\t\t\tOnlineKey base64Decoding failed.\n");
      return FALSE;
   }
   if (TRUE == m_bDoRemote) {
      RemoteNCipherDecryptKeys( OnlineKeyEncrypted, (LPBYTE)&xeod, sizeof(xeod) ); 
   } else {
      DecryptOnlineKey( OnlineKeyEncrypted, (LPBYTE)&xeod, sizeof(xeod));
   }

   // Carve up the returned memory into the layout we believe is there.
   // First copy it so its persistent over next connection.
   XBOX_ENCRYPTED_PER_BOX_DATA xepbd;
   Decrypt(bWireStructure, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA));
   RtlCopyMemory(&xepbd, bWireStructure + sizeof(RC4_SHA1_HEADER), 
                 sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) );

   printf("  Encrypted Key in CHAR\n");
   DumpBytesChar((LPBYTE)OnlineKeyEncryptedBase64Encoded, 172 );
   printf("\n");
   printf("  Base64 Decoded Key in HEX\n");
   DumpBytesHex(OnlineKeyEncrypted, 128 );
   printf("\n");
   printf("  PlainText Key in Hex\n");
   DumpBytesHex(xeod.OnlineKey, sizeof(xeod.OnlineKey));
   printf("  XboxSerialNumber\n");
   DumpBytesChar((UCHAR*)xeod.XboxSerialNumber, 12 );
   printf("  XboxHardDrive in Hex\n");
   DumpBytesHex(xeod.HardDriveKey, sizeof(xeod.HardDriveKey));
   
   printf("\n-------------------END   SESSION--------------------\n");
   return TRUE;
}


BOOL
C2kIdTests::
TestVerifyCheckSum( DWORD dwFlag )
{

   dwFlag =0;

   return TRUE;
}




BOOL
C2kIdTests::
TestParametersGenPerBoxData( )
{
   
   INT i;
   GENPERBOXDATA_INFO x;

   printf("\n-------------------START SESSION--------------------\n");
   printf("[STATUS]\t\t[Description of Action]\n");

   // Test OutBufferSize is too small.
   ResetVariables(&x);
   x.dwOutBufferSize = 0; 
   x.hr = GeneratePerBoxData ( x.GameRegion,
                             x.AVRegion,
                             x.MACAddress,
                             x.XboxSerialNumber,
                             x.HardDriveKeyBase64Encoded,
                             x.RecoveryKey,
                             &x.dwOnlineKeyVersion,
                             x.OnlineKeyEncryptedBase64Encoded,
                             x.byteData,
                             &x.dwOutBufferSize );
   if ( x.hr == E_INVALIDARG )
      printf("  OK\t\t\tSmall buffer correct failure E_INVALIDARG.\n");
   else
      printf("  NOT_OK\t\t\tSmall buffer incorrect failure.\n");

//--------------------------------------------------------------------------------------

   // Test pcOutputBuffer is null OutputBufferSize returns necessary size.
   ResetVariables(&x);
   x.dwOutBufferSize = 0;
   x.hr = GeneratePerBoxData ( x.GameRegion,
                             x.AVRegion,
                             x.MACAddress,
                             x.XboxSerialNumber,
                             x.HardDriveKeyBase64Encoded,
                             x.RecoveryKey,
                             &x.dwOnlineKeyVersion,
                             x.OnlineKeyEncryptedBase64Encoded,
                             NULL,
                             &x.dwOutBufferSize );
   

   if ( SUCCEEDED(x.hr) ) {
      if ( x.dwOutBufferSize == 88 )
         printf("  OK\t\t\tNULL OutputBuffer, NULL OutBufferSize does return 88.\n");
      else
         printf("  NOT_OK\t\tNULL OutputBuffer, NULL OutBufferSize does not  return 88.\n");
   } else {
      printf("  NOT_OK\t\tNULL OutputBuffer does not return correct S_OK.\n");
   }
//--------------------------------------------------------------------------------------
   // Test MAC address being over 6 bytes. Expect only 6 copied
   for (i =0; i < 3; i++) {
      x.MACAddress[i] = 0x66;
   }
   for (i =3; i < 10; i++) {
      x.MACAddress[i] = 0x77;
   }
   // MAC address should look like { 0x66,0x66,0x66,0x66,0x66,0x66,0x77,0x77,0x77,0x77 };
   ResetVariables(&x);
   x.dwOutBufferSize = 88;
   x.hr = GeneratePerBoxData ( x.GameRegion,
                             x.AVRegion,
                             x.MACAddress,
                             x.XboxSerialNumber,
                             x.HardDriveKeyBase64Encoded,
                             x.RecoveryKey,
                             &x.dwOnlineKeyVersion,
                             x.OnlineKeyEncryptedBase64Encoded,
                             x.byteData,
                             &x.dwOutBufferSize );
   if ( SUCCEEDED(x.hr) ) {

      BOOL bResult; 
      // Decrypt the send buffer.
      bResult =  rc4HmacDecrypt( (LPBYTE)FACTORY_RAND_KEY, 
                                  FACTORY_RAND_KEY_LEN,
                                  x.byteData + sizeof(RC4_SHA1_HEADER),
                                  sizeof(XBOX_ENCRYPTED_PER_BOX_DATA),
                                  x.byteData);
      assert( bResult == TRUE );
      
      // Decode the Online Key Buffer.
      // Base64 decode to shave off the 172 bytes down to 128 bytes.
      bResult = Base64Decode( (LPCSTR)x.OnlineKeyEncryptedBase64Encoded,
                              172,
                              (LPBYTE)x.OnlineKeyEncrypted,
                              &x.ulOnlineKey );
      assert( x.ulOnlineKey == 128 );
      
      if (TRUE == m_bDoRemote) {
         RemoteNCipherDecryptKeys( x.OnlineKeyEncrypted, x.OnlineKey, sizeof(x.OnlineKey) ); 
      } else {
         DecryptOnlineKey( x.OnlineKeyEncrypted, x.OnlineKey, sizeof(x.OnlineKey));
      }

      // This is a pointer to the wire data.
      PXBOX_ENCRYPTED_PER_BOX_DATA pxWireData =  (PXBOX_ENCRYPTED_PER_BOX_DATA)
                                             (x.byteData + sizeof(RC4_SHA1_HEADER));

      // Check only the 6 bytes.. If the match then we really don't care about the next bytes
      // as long as the 6 bytes stored are correct
      if (TRUE == RtlEqualMemory(pxWireData->MACAddress,
                                 x.MACAddress,
                                 sizeof(pxWireData->MACAddress))) {
         printf("  OK\t\t\tMAC Address\n");
      } else {
         printf("  NOT_OK\t\t\tMAC Address\n");
         printf("  MACAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                pxWireData->MACAddress[0],
                pxWireData->MACAddress[1],
                pxWireData->MACAddress[2],
                pxWireData->MACAddress[3],
                pxWireData->MACAddress[4],
                pxWireData->MACAddress[5] );
         printf("  MACAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                x.MACAddress[0],
                x.MACAddress[1],
                x.MACAddress[2],
                x.MACAddress[3],
                x.MACAddress[4],
                x.MACAddress[5] );
      }
   }else
      printf("  NOT_OK\t\tMAC Address did not return S_OK\n");
   
//--------------------------------------------------------------------------------------
   // Test with a 13 byte Serial Number only 12 should be copied.
   ResetVariables(&x);
   char sz13ByteSerialNum[13] = {'A','B','C','D','E','F','G','H','I','J','K','L','M'};
   x.hr = GeneratePerBoxData ( x.GameRegion,
                             x.AVRegion,
                             x.MACAddress,
                             sz13ByteSerialNum,
                             x.HardDriveKeyBase64Encoded,
                             x.RecoveryKey,
                             &x.dwOnlineKeyVersion,
                             x.OnlineKeyEncryptedBase64Encoded,
                             x.byteData,
                             &x.dwOutBufferSize );

   if ( SUCCEEDED(x.hr) ) {

      BOOL bResult; 
      // Decrypt the send buffer.
      bResult =  rc4HmacDecrypt( (LPBYTE)FACTORY_RAND_KEY, 
                                    FACTORY_RAND_KEY_LEN,
                                    x.byteData + sizeof(RC4_SHA1_HEADER),
                                    sizeof(XBOX_ENCRYPTED_PER_BOX_DATA),
                                    x.byteData);
      assert( bResult == TRUE );

      // Decode the Online Key Buffer.
      // Base64 decode to shave off the 172 bytes down to 128 bytes.
      bResult = Base64Decode( (LPCSTR)x.OnlineKeyEncryptedBase64Encoded,
                              172,
                              (LPBYTE)x.OnlineKeyEncrypted,
                              &x.ulOnlineKey );
      assert( x.ulOnlineKey == 128 );
      if (TRUE == m_bDoRemote) {
         RemoteNCipherDecryptKeys( x.OnlineKeyEncrypted, x.OnlineKey, sizeof(x.OnlineKey) ); 
      } else {
         DecryptOnlineKey( x.OnlineKeyEncrypted, x.OnlineKey, sizeof(x.OnlineKey));
      }

      // Noting that the OnlineKey is also the entire structure.
      // make sure that the Serial Number is correct
      PXBOX_ENCRYPTED_ONLINE_DATA pOnline = 
         (PXBOX_ENCRYPTED_ONLINE_DATA)x.OnlineKeyDataBaseInfo;

      // This is a pointer to the wire data.
      PXBOX_ENCRYPTED_PER_BOX_DATA pxWireData =  (PXBOX_ENCRYPTED_PER_BOX_DATA)
                                                 (x.byteData + sizeof(RC4_SHA1_HEADER));

      if (FALSE == ByteCmp((LPBYTE)pxWireData->XboxSerialNumber, (LPBYTE)sz13ByteSerialNum, 12)) {
         printf("  NOT_OK\t\tSerNum wire data 13 bytes not 12 long.\n");
      }else {
         //printf("  %s %s\n",pxWireData->XboxSerialNumber,sz13ByteSerialNum);
         printf("  OK\t\t\tSerNum, wire data 12 bytes, truncated from 13.\n");
      }
      if (FALSE == ByteCmp((LPBYTE)pOnline->XboxSerialNumber,(LPBYTE)sz13ByteSerialNum, 12)) {
         printf("  NOT_OK\t\tSerNum Online Buffer 13 bytes  not 12 long.\n");
      }else{
         printf("  OK\t\t\tSerNum Online Buffer 12 bytes, truncated from 13.\n");
      }
   }else{
      printf("  NOT_OK\t\tSerial Number did not return S_OK\n");
   }

   

//--------------------------------------------------------------------------------------
   // Test with a non alpha numberic characters.
   /*
   ResetVariables(&x);
   char sz13ByteSerialNum[13] = {'A','B','C','D','E','F','G','H','I','J','K','L','M'};
   x.hr = GeneratePerBoxData ( x.GameRegion,
                             x.AVRegion,
                             x.MACAddress,
                             sz13ByteSerialNum,
                             x.HardDriveKeyBase64Encoded,
                             x.RecoveryKey,
                             &x.dwOnlineKeyVersion,
                             x.OnlineKeyEncryptedBase64Encoded,
                             x.byteData,
                             &x.dwOutBufferSize );

   if ( SUCCEEDED(x.hr) ) {

      BOOL bResult; 
      // Decrypt the send buffer.
      bResult =  rc4HmacDecrypt( (LPBYTE)FACTORY_RAND_KEY, 
                                    FACTORY_RAND_KEY_LEN,
                                    x.byteData + sizeof(RC4_SHA1_HEADER),
                                    sizeof(XBOX_ENCRYPTED_PER_BOX_DATA),
                                    x.byteData);
      assert( bResult == TRUE );

      // Decode the Online Key Buffer.
      // Base64 decode to shave off the 172 bytes down to 128 bytes.
      bResult = Base64Decode( (LPCSTR)x.OnlineKeyEncryptedBase64Encoded,
                              172,
                              (LPBYTE)x.OnlineKeyEncrypted,
                              &x.ulOnlineKey );
      assert( x.ulOnlineKey == 128 );
      hr = DecryptOnlineKey((LPBYTE)x.OnlineKeyEncrypted, 
                               (LPBYTE)x.OnlineKeyDataBaseInfo, 
                               sizeof(x.OnlineKeyDataBaseInfo));
      assert( hr == S_OK );

      // Noting that the OnlineKey is also the entire structure.
      // make sure that the Serial Number is correct
      PXBOX_ENCRYPTED_ONLINE_DATA pOnline = 
         (PXBOX_ENCRYPTED_ONLINE_DATA)x.OnlineKeyDataBaseInfo;


      // This is a pointer to the wire data.
      PXBOX_ENCRYPTED_PER_BOX_DATA pxWireData =  (PXBOX_ENCRYPTED_PER_BOX_DATA)
                                                 (x.byteData + sizeof(RC4_SHA1_HEADER));

      if (FALSE == ByteCmp((LPBYTE)pxWireData->XboxSerialNumber, (LPBYTE)sz13ByteSerialNum, 12)) {
         printf("  NOT_OK\t\tSerNum wire data 13 bytes not 12 long.\n");
      }else {
         printf("  %s %s\n",pxWireData->XboxSerialNumber,sz13ByteSerialNum);
         printf("  OK\t\t\tSerNum, wire data 12 bytes, truncated from 13.\n");
      }
      if (FALSE == ByteCmp((LPBYTE)pOnline->XboxSerialNumber,(LPBYTE)sz13ByteSerialNum, 12)) {
         printf("  NOT_OK\t\tSerNum Online Buffer 13 bytes  not 12 long.\n");
      }else{
         printf("  OK\t\t\tSerNum Online Buffer 12 bytes, truncated from 13.\n");
      }
   }else{
      printf("  NOT_OK\t\tSerial Number did not return S_OK\n");
   }
     */
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
   return TRUE;
}

