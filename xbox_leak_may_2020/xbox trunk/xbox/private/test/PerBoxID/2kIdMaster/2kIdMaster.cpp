//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include "2kIdMaster.h"


extern "C"  
int _cdecl
main( int argc,
      char** argv )
{

   C2kIdMaster* cm = new C2kIdMaster;


   if (argc == 1) {
      cm->PrintHelp();
      return 1;
   }
   cm->ValidateArgs(argc, argv);
   delete cm;
   return 0;
}



VOID 
C2kIdMaster::
ValidateArgs(int argc, char **argv)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    5/7/2001

    Routine Description:
       Parse up the command line aruguments and react accordingly. 
    Arguments:
       Typical arguments to main.
    Return Value:
      None.

--*/

{
   int i;
   char szCommand[5];
   char * p;
   BOOL bFlag = FALSE;


   for (i = 1; i < argc; i++) {
      if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
         p = argv[i];
         strcpy(szCommand, ++p);
         CharLower( szCommand );
         if (0 != strstr(szCommand,"remote")) {
            bFlag = TRUE;
         }
      }
   }
   if (bFlag == TRUE) {
      SetDoRemote ( TRUE );
   }else{
      SetDoRemote ( FALSE );
   }




   for (i = 1; i < argc; i++) {
      if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
         p = argv[i];
         strcpy(szCommand, ++p);
         CharLower( szCommand );
         if (0 == strcmp(szCommand,"1")) {
            SetLoop(m_dwLoop); 
            SetName(m_szXboxName);
            TestMemLeakage();
         } else
            if (0 == strcmp(szCommand,"2")) {
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestMACAddress();
         } else
            if (0 == strcmp(szCommand,"3")) {
            m_dwLoop = atol(argv[i + 1]);
            sprintf(m_szXboxName,"%s",argv[i + 2]);
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestXClient( NULL );
         } else
            if (0 == strcmp(szCommand,"4")) {
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestMACAddress();
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestXClient( DO_LOCALHOST );
         } else
            if (0 == strcmp(szCommand,"5")) {
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestParametersGenPerBoxData( );
         } else
            if (0 == strcmp(szCommand,"6")) {
            m_dwLoop = atol(argv[i + 1]);
            // Set the utils Class
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestRemoteDecrypt( 0 );
         } else
            if (0 == strcmp(szCommand,"7")) {
            m_dwLoop = atol(argv[i + 1]);
            sprintf(m_szXboxName,"%s",argv[i + 2]);
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            ReadRemotesEEPROM(0);
         } else
            if (0 == strcmp(szCommand,"8")) {
            sprintf(m_szXboxName,"%s",argv[i + 2]);
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestLockHardDrive(atol(argv[i + 1]));
         } else
            if (0 == strcmp(szCommand,"9")) {
            //sprintf(m_szXboxName,"%s",argv[i + 2]);
            SetLoop(m_dwLoop);
            SetName(m_szXboxName);
            TestDirectoryOfTonyChensEEPROMS(argv[i + 1], NULL);
            //TestDirectoryOfEEPROMS( argv[i + 1], NULL );
         } else
            if (0 == strcmp(szCommand,"10")) {
            SetLoop(m_dwLoop); 
            SetName(m_szXboxName);
            TestDirectoryOfTonyChensEEPROMS( argv[i + 1], argv[i + 2]);
            //TestDirectoryOfEEPROMS( argv[i + 1], argv[i + 2]);
         } else
            if (0 == strcmp(szCommand,"11")) {
            SetLoop(m_dwLoop); 
            SetName(m_szXboxName);
            TestRemoteDecodingOfOnlineKey( argv[i + 1]);
         }
         // Set the utils Class
         SetLoop(m_dwLoop); 
         SetName(m_szXboxName);
      }
   }
}



VOID
C2kIdMaster::
PrintHelp( VOID )
{

   PCCH XeName = "2kIMaster";
   
   printf("Usage: \n");
   printf("General Help and Usage\n",     XeName);
   printf("Memory Leak test   : %s /1 \n",XeName);
   printf("MAC Address test   : %s /2 \n",XeName);
   printf("Full x test        : %s /3 1<loop> wallyhox<xboxname> \n",XeName);
   printf("Parameter Test     : %s /5\n",       XeName);
   printf("Test Rmt Decrypt   : %s /6 <loop>\n",XeName);
   printf("Rmt EEPROM Read    : %s /7 <loop> wallyhox<xboxname> \n",XeName);
   printf("HardDrive Un/Lock  : %s /8 <0|1> wallyhox<xboxname> \n", XeName);
   printf("Crack a dir of EEPROMS data : %s /9  <directory> \n",XeName);
   printf("Load and cmp EEPROMS online key : %s /10  <directory> <OnlineKey Database File>\n",XeName);
   printf("  eg:%s /10 c:\\ eeproms c:\\eeproms\\sst...tbs \n",             XeName);
   printf("Decode a Key       : %s /11 <OnlineKey>\n",XeName);
}
