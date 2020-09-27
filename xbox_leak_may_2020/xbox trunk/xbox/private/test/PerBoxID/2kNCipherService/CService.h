/*++
Copyright (c) 2000  Microsoft Corporation

Filename:
      Serivce.h       
Abstract:
      This the class for the service.
Author:

       Wally Ho (wallyho) 6/15/2001

Revision History:
   6/15/2001         Created

--*/



#ifndef SERVICE_H
#define SERVICE_H
#include <windows.h>
#include "CNetWork.h"
#include "CCrypto.h"


    CONST LPTSTR m_szSCMDisplayName = TEXT("2kNCipher Service");
    CONST LPTSTR m_szServiceName    = TEXT("2kNCipher_Service");
 

class CService: public CNetWork, public CCrypto{
   
private:
   
   HANDLE m_hThread;
   BOOL m_bPause;
   BOOL m_bRunning;
   HANDLE m_hEVENT_TerminateServiceMain;
   HANDLE m_hEVENT_TerminateServiceThread;
   
   HANDLE m_hEventSource;
   SERVICE_STATUS  m_ServiceStatus;
   SERVICE_STATUS_HANDLE m_ServiceStatusHandle;


public:


   BOOL RemoveService(LPTSTR szServiceNameSCM);
   BOOL InstallService(LPTSTR szServiceNameSCM,
                       LPTSTR szServiceLabel,
                       LPTSTR szExeFullPath);
   static void WINAPI  ServiceMain(DWORD argc, LPTSTR *argv);
   static void WINAPI  ServiceCtrlHandler (DWORD controlCode);
   static DWORD WINAPI ServiceThread (LPDWORD param);
   static DWORD WINAPI ClientThread (LPVOID lpParam);
   static DWORD WINAPI AcceptThread(LPVOID lpParam);

   VOID TEST(){
      ServiceThread(0);
   }

   BOOL InitService();
   VOID ResumeService() { m_bPause = FALSE;   ResumeThread(m_hThread); }
   VOID PauseService()  { m_bPause = TRUE;    SuspendThread(m_hThread); }
   VOID StopService();

   BOOL SendStatusToSCM ( DWORD dwCurrentState,
                          DWORD dwWin32ExitCode, 
                          DWORD dwServiceSpecificExitCode,
                          DWORD dwCheckPoint,
                          DWORD dwWaitHint);
   VOID Terminate(DWORD error);
   BOOL ParseStandardArgs( VOID );

   void  LogEvent(WORD wType, DWORD dwID,
               const char* pszS1,
               const char* pszS2,
               const char* pszS3);

   static CService* m_pCServiceThis;

   ~CService(){}
   CService(){
       
       m_hThread = NULL;
       m_bPause   = FALSE;
       m_bRunning = TRUE;
       m_hEVENT_TerminateServiceMain = NULL;
       m_hEVENT_TerminateServiceThread = NULL;
       m_hEventSource = NULL;
       m_pCServiceThis = this;
   }
};



//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: EVMSG_INSTALLED
//
// MessageText:
//
//  The %1 service was installed.
//
#define EVMSG_INSTALLED                  0x00000064L

//
// MessageId: EVMSG_REMOVED
//
// MessageText:
//
//  The %1 service was removed.
//
#define EVMSG_REMOVED                    0x00000065L

//
// MessageId: EVMSG_NOTREMOVED
//
// MessageText:
//
//  The %1 service could not be removed.
//
#define EVMSG_NOTREMOVED                 0x00000066L

//
// MessageId: EVMSG_CTRLHANDLERNOTINSTALLED
//
// MessageText:
//
//  The control handler could not be installed.
//
#define EVMSG_CTRLHANDLERNOTINSTALLED    0x00000067L

//
// MessageId: EVMSG_FAILEDINIT
//
// MessageText:
//
//  The initialization process failed.
//
#define EVMSG_FAILEDINIT                 0x00000068L

//
// MessageId: EVMSG_STARTED
//
// MessageText:
//
//  The service was started.
//
#define EVMSG_STARTED                    0x00000069L

//
// MessageId: EVMSG_BADREQUEST
//
// MessageText:
//
//  The service received an unsupported request.
//
#define EVMSG_BADREQUEST                 0x0000006AL

//
// MessageId: EVMSG_DEBUG
//
// MessageText:
//
//  Debug: %1
//
#define EVMSG_DEBUG                      0x0000006BL

//
// MessageId: EVMSG_STOPPED
//
// MessageText:
//
//  The service was stopped.
//
#define EVMSG_STOPPED                    0x0000006CL

#endif

