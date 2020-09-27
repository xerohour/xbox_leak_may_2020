//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#ifndef _2KIDTESTS_H_
#define _2KIDTESTS_H_

#include <windows.h>
#include "2kId_Utils.h"
#include "CCrypto.h"
#include <xbox.h>
#include <xboxp.h>
#include <assert.h>
#include <PerBoxData.h>


class C2kIdTests: CCrypto, C2kIdUtils {

public:

   BOOL TestMemLeakage( VOID );
   BOOL TestMACAddress( VOID );
   BOOL TestXClient ( IN DWORD dwFlag );
   BOOL TestParametersGenPerBoxData( VOID );
   BOOL TestRemoteDecrypt( IN DWORD dwFlag );
   BOOL ReadRemotesEEPROM( IN DWORD dwFlag );
   BOOL TestLockHardDrive( IN DWORD dwControl );
   BOOL TestVerifyCheckSum( IN DWORD dwFlag );
   BOOL TestDirectoryOfEEPROMS( IN PCHAR szDirectory, 
                                IN PCHAR szDataBaseFile );
   
   BOOL TestDirectoryOfTonyChensEEPROMS( IN PCHAR szDirectory,
                                         IN PCHAR szDataBaseFile); 
   BOOL TestRemoteDecodingOfOnlineKey( IN PCHAR OnlineKeyEncryptedBase64Encoded );
   VOID SetLoop(IN DWORD dw ){ m_dwTestLoop = dw;}
   VOID SetName(IN PCHAR s ) { strcpy(m_szXboxName,s);}
   VOID SetDoRemote(IN BOOL b );
   
   C2kIdTests(){m_dwTestLoop = 0; m_szXboxName[0]='\0';}
   
   C2kIdTests(IN DWORD dw ){ m_dwTestLoop = dw; m_bDoRemote = FALSE;}
   ~C2kIdTests(){}

private:
   DWORD m_dwTestLoop;
   CHAR  m_szXboxName[MAX_PATH];
   BOOL  m_bDoRemote;

};
#endif
