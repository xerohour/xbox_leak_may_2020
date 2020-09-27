#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "CService.h"


// static variables
CService* CService::m_pCServiceThis = NULL;



DWORD WINAPI 
CService::
ServiceThread(LPDWORD param)
{

   UNREFERENCED_PARAMETER(param);
   // Since this is static , its basically a global local variable. Wallyho
   CService* p = m_pCServiceThis;
   
   
   HANDLE   hThread = 0;
   DWORD dwExitCode = 0;
   DWORD dwReturn = TRUE;
   printf(TEXT("Starting ServiceThread\n"));

   // initialize non blocking accept thread.
   hThread = CreateThread(NULL,
                          0,
                          AcceptThread,
                          (LPVOID)0,
                          0,
                          NULL);
   if (hThread == NULL) {
      printf(TEXT("CreateThread() failed: %d\n   "), GetLastError());
      dwReturn = FALSE;
      goto c0;
   }
   // The keeps the service from exiting.
   while ( p->m_bRunning ) {
     Sleep(100);
   }
   //WaitForSingleObject (p->m_hEVENT_TerminateServiceThread, INFINITE);

   // Terminate the Listening thread.
   GetExitCodeThread( hThread, &dwExitCode);
   //ExitThread(dwExitCode);
   TerminateThread(hThread, dwExitCode);
   c0:
   return dwReturn;
}



DWORD WINAPI
CService::
AcceptThread(LPVOID lpParam)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    7/6/2001

    Routine Description:
       Worker Thread.
       
    Arguments:
            
   Return Value:

--*/
{
   
   UNREFERENCED_PARAMETER(lpParam);
    CService* p = m_pCServiceThis;

   SOCKET sAccept;
   for (;;){
      if ( INVALID_SOCKET == (sAccept = p->Accept())) {
         printf(TEXT("INVALID_SOCKET on calling accept....\n"));
      } else
         p->StartThread( ClientThread,(LPVOID)sAccept);
   }
}




DWORD WINAPI
CService::
ClientThread(LPVOID lpParam)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    6/25/2001

    Routine Description:
       Worker Thread.
       
    Arguments:
        A variable void pointer holding a socket.
    Return Value:

--*/

{

   SOCKET sAccept = (SOCKET)lpParam;

   CNetWorkConnection* c= new CNetWorkConnection( sAccept );
   CService* p = m_pCServiceThis;
   

   DATA_PACKET InBuffer;
   DATA_PACKET OutBuffer;
   ZeroMemory(&InBuffer, sizeof(InBuffer));
   if ( SOCKET_ERROR == c->RecvData(&InBuffer, sizeof(InBuffer)))
      goto cleanup;
   
   ZeroMemory(&OutBuffer, sizeof(OutBuffer));
   p->DecryptOnlineKey(InBuffer.byteData, OutBuffer.byteData);
   
   sprintf(OutBuffer.szMessage, "Decrypted Online Key Only" );
   if ( SOCKET_ERROR == c->SendData(&OutBuffer, sizeof(OutBuffer)))
      goto cleanup;

   cleanup:
   delete c;
   printf("DONE Sending.\n Waiting for next connection\n" );
   return 0;
}




BOOL
CService::
InitService()
// Initializes the service by starting its thread
{

   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED,"Initializing CSP Service","","");

    // Start the service's thread
    m_hThread = CreateThread(NULL,
                             0,
                             (LPTHREAD_START_ROUTINE)ServiceThread,
                             (LPVOID)0,
                             0,
                             NULL);
    if (m_hThread==0)
        return FALSE;
    else {
        m_bRunning = TRUE;
        return TRUE;
    }
}





BOOL
CService::
SendStatusToSCM ( DWORD dwCurrentState,
                  DWORD dwWin32ExitCode,
                  DWORD dwServiceSpecificExitCode,
                  DWORD dwCheckPoint,
                  DWORD dwWaitHint)
/*
   This function consolidates the activities of
   updating the service status with
   SetServiceStatus
*/

{
   BOOL success;
   SERVICE_STATUS serviceStatus;

   ZeroMemory (&serviceStatus, sizeof(serviceStatus));


   // Fill in all of the SERVICE_STATUS fields
   serviceStatus.dwServiceType  = SERVICE_WIN32_OWN_PROCESS;
   serviceStatus.dwCurrentState = dwCurrentState;
   
   // If in the process of something, then accept
   // no control events, else accept anything
   if (dwCurrentState == SERVICE_START_PENDING)
      serviceStatus.dwControlsAccepted = 0;
   else
      serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                         SERVICE_ACCEPT_PAUSE_CONTINUE |
                                         SERVICE_ACCEPT_SHUTDOWN;

   // if a specific exit code is defines, set up
   // the win32 exit code properly
   if (dwServiceSpecificExitCode == 0)
      serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
   else
      serviceStatus.dwWin32ExitCode =  ERROR_SERVICE_SPECIFIC_ERROR;

   
   serviceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
   serviceStatus.dwCheckPoint = dwCheckPoint;
   serviceStatus.dwWaitHint   = dwWaitHint;

   
   // Pass the status record to the SCM
   success = SetServiceStatus ( m_ServiceStatusHandle,&serviceStatus);
   if (!success)
      StopService();
   return success;
}



VOID
CService::
ServiceCtrlHandler (DWORD controlCode)
/*++
   Copyright (c) 2000, Microsoft.

   Author:  Wally W. Ho (wallyho) 
   Date:    7/31/2000

   Routine Description:
       This handles events received from the service control manager.
   Arguments:
       Standard control codes for a service of:
         SERVICE_CONTROL_STOP;
         SERVICE_CONTROL_PAUSE;
         SERVICE_CONTROL_CONTINUE;
         SERVICE_CONTROL_INTERROGATE;
         SERVICE_CONTROL_SHUTDOWN;
         
   Return Value:
       NONE
--*/
{

    DWORD  currentState = 0;
    BOOL success;

    CService* p = m_pCServiceThis;

    switch (controlCode) {
       // Stop the service
       case SERVICE_CONTROL_STOP:
           currentState = SERVICE_STOP_PENDING;
           success = p->SendStatusToSCM( SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 1000);
           p->Terminate( NO_ERROR  );
           return ;
      
           // Pause the service
       case SERVICE_CONTROL_PAUSE:
           if (p->m_bRunning && 
               (p->m_bPause == TRUE)) {
               success = p->SendStatusToSCM(SERVICE_PAUSE_PENDING,NO_ERROR, 0, 1, 1000);
               p->PauseService();
               currentState = SERVICE_PAUSED;
           }
           break;
           // Resume from a pause
       case SERVICE_CONTROL_CONTINUE:
           if (p->m_bRunning && p->m_bPause) {
      
               success = p->SendStatusToSCM( SERVICE_CONTINUE_PENDING,NO_ERROR, 0, 1, 1000);
               p->ResumeService();
               currentState = SERVICE_RUNNING;
           }
           break;
           // Update current status
       case SERVICE_CONTROL_INTERROGATE:
           break;
      
       case SERVICE_CONTROL_SHUTDOWN:
           // Do nothing on shutdown
           return ;
       default:
           break;
    }
    
    p->SendStatusToSCM(currentState, NO_ERROR, 0, 0, 0);
}


VOID
CService::
Terminate(DWORD error)
/*++
   
   Handle an error from ServiceMain by cleaning up
   and telling SCM that the service didn't start.

--*/ 

{
   
   // if terminateEvent has been created, close it.
   if (m_hEVENT_TerminateServiceMain)
      CloseHandle(m_hEVENT_TerminateServiceMain);
   
   if (m_hEVENT_TerminateServiceThread)
      CloseHandle(m_hEVENT_TerminateServiceThread);
    
   // Send a message to the scm to tell about stoppage
   if (m_ServiceStatusHandle)
      SendStatusToSCM(SERVICE_STOPPED, error,0, 0, 0);

   // If the thread has started kill it off
   if (m_hThread)
      CloseHandle(m_hThread);
   // Do not need to close serviceStatusHandle
   
}



BOOL 
CService::
ParseStandardArgs( VOID )
{


   if (FALSE != _tcsstr(GetCommandLine(), TEXT("-install")  )) {

      // if -install specified then we'll register the service.
      TCHAR szServiceExe[ MAX_PATH ];
      GetModuleFileName( NULL, szServiceExe, sizeof(szServiceExe)/sizeof(szServiceExe[0]));         
      /*
      LPTSTR p;
      p = _tcsrchr(szServiceExe,TEXT('\\'));
      if (NULL == p) {
        return FALSE;
      }
      *p = TEXT('\0');
      _tcscat( szServiceExe, TEXT("\\s.exe"));
      */
      InstallService(m_szServiceName , m_szSCMDisplayName, szServiceExe);
      return TRUE;
   } else if (FALSE !=_tcsstr(GetCommandLine(), TEXT("-remove")  )) {
      
      RemoveService( m_szServiceName );
      
      return TRUE;
   } else if (FALSE !=_tcsstr(GetCommandLine(), TEXT("-removeall")  )) {
      
      for (INT o = 1; o < 10; o++) {
         char sz[100];
         _stprintf(sz,TEXT("2kNCipher%d_Service"), o);
         RemoveService( sz );
      }
      return TRUE;
   
   } else if (FALSE !=_tcsstr(GetCommandLine(), TEXT("-console")  )) {
      InitNetWork();
      InitCrypto();
      TEST();
      return TRUE;
   } 
   else {
      // default is to start as a service.
      SERVICE_TABLE_ENTRY st[] ={
         { m_szServiceName , ServiceMain},
         { NULL, NULL}
      };
      
      InitNetWork();
      InitCrypto();
      StartServiceCtrlDispatcher(st);
   }
   return FALSE;
}




// static member function (callback)

VOID
CService::
ServiceMain(DWORD argc, LPTSTR *argv)
/*++
   Copyright (c) 2000, Microsoft.

   Author:  Wally W. Ho (wallyho) 
   Date:    8/2/2000

   Routine Description:
         ServiceMain is called when the SCM wants to start the service. 
         When it returns, the service has stopped. It therefore waits 
         on an event just before the end of the function, and that event
         gets set when it is time to stop. It also returns on any error 
         because the service cannot start if there is an error.
 
   Arguments:
       Normal arguments like Main would have.
   Return Value:
       NONE
--*/
{

   UNREFERENCED_PARAMETER(argc);
   UNREFERENCED_PARAMETER(argv);
   // Get a pointer to the C++ object
   // Since this is static , its basically a global local variable. Wallyho
   CService* p = m_pCServiceThis;
   
   BOOL success;
   // Call Registration function
   p->m_ServiceStatusHandle = 
   RegisterServiceCtrlHandler( m_szSCMDisplayName, ServiceCtrlHandler);

   if (!p->m_ServiceStatusHandle) 
      return;

   // Notify SCM of progress
   success = p->SendStatusToSCM( SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
   if (!success)
      return;

   // create the Service Main Termination event
   p->m_hEVENT_TerminateServiceMain = CreateEvent (NULL, TRUE, FALSE, NULL);
   if (!p->m_hEVENT_TerminateServiceMain)
      return;


   // create the Service Thread Termination event
   p->m_hEVENT_TerminateServiceThread = CreateEvent (NULL, TRUE, FALSE, NULL);
   if (!p->m_hEVENT_TerminateServiceThread)
      return;

   // Notify SCM of progress
   success = p->SendStatusToSCM( SERVICE_START_PENDING, NO_ERROR, 0, 3, 5000);
   if (!success)
      return;

   // Start the service itself
   success = p->InitService();
   if (!success)
      return;

   // The service is now running. Notify SCM of progress
   success = p->SendStatusToSCM(SERVICE_RUNNING,NO_ERROR, 0, 0, 0);
   if (!success)
      return;

   
   // Wait for stop signal, and then terminate
   WaitForSingleObject (p->m_hEVENT_TerminateServiceMain, INFINITE);
   SetLastError(0);
}



BOOL
CService::
InstallService( LPTSTR szServiceNameSCM,
                LPTSTR szServiceLabel,
                LPTSTR szExeFullPath)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    6/15/2001

    Routine Description:
        This installs a service 
    Arguments:
       szServiceNameSCM is the name used internally by the SCM\n";
       szServiceLabel is the name that appears in the Services applet\n";
       szExeFullPath ie. eg "c:\winnt\xxx.exe"
    Return Value:
       NONE
--*/
{

   BOOL b = TRUE;
   DWORD dwData;
   SC_HANDLE newScm;
   char szKey[256];
   HKEY hKey = NULL;

   printf(TEXT("Installing a new service\n"));
   // open a connection to the SCM
   SC_HANDLE scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);

   if (!scm)
      printf(TEXT("In OpenScManager: Error %lu\n"),GetLastError());

   newScm = CreateService( scm,
                           szServiceNameSCM,
                           szServiceLabel,
                           SERVICE_ALL_ACCESS,
                           SERVICE_WIN32_OWN_PROCESS,
                           SERVICE_AUTO_START,
                           SERVICE_ERROR_NORMAL,
                           szExeFullPath,
                           NULL,
                           NULL, 
                           NULL, 
                           NULL, 
                           NULL);
   if (!newScm){
      printf(TEXT("Problem installing Service: Error %lu.\n"),GetLastError());
      b = FALSE;
      goto c0;
   }
   else
      printf(TEXT("Finished installing a new service.\n"));




   // make registry entries to support logging messages
   // Add the source name as a subkey under the Application
   // key in the EventLog service portion of the registry.
   strcpy(szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
   strcat(szKey, szServiceNameSCM);
   if (RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
      b = FALSE;
      goto c1;
   }

   // Add the Event ID message-file name to the 'EventMessageFile' subkey.
   RegSetValueEx(hKey,
                   "EventMessageFile",
                   0,
                   REG_EXPAND_SZ, 
                   (CONST BYTE*)szExeFullPath,
                   strlen(szExeFullPath) + 1);     
   

   // Set the supported types flags.
   dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
   ::RegSetValueEx(hKey,
                   "TypesSupported",
                   0,
                   REG_DWORD,
                   (CONST BYTE*)&dwData,
                    sizeof(DWORD));
   ::RegCloseKey(hKey);

   //LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, m_szServiceName);

   // clean up
  c1:
   CloseServiceHandle(newScm);
  c0: 
   CloseServiceHandle(scm);
   return b;
}


BOOL
CService::
RemoveService( IN LPTSTR szServiceNameSCM)
/*++
   Copyright (c) 2000, Microsoft.

   Author:  Wally W. Ho (wallyho) 
   Date:    7/27/2000

   Routine Description:
       This function removes the specified service from the machine.
       
   Arguments:
       The service to remove's internal name.

   Return Value:
       TRUE on success
       FALSE on failure
--*/

{
   
   
   SC_HANDLE service, scm;
   BOOL success;
   SERVICE_STATUS status;
   BOOL b = TRUE;
   char szKey[256];



   printf(TEXT("Removing the %s service.\n"), szServiceNameSCM);
   // open a connection to the SCM
   scm = OpenSCManager(NULL, 
                       NULL,
                       SC_MANAGER_ALL_ACCESS);
   if (!scm) {
      printf(TEXT("In OpenScManager: Error %lu\n"),GetLastError());
      b= FALSE;
      goto c0;
   }

   // Get the service's handle
   service = OpenService(scm,
                         szServiceNameSCM,
                         SERVICE_ALL_ACCESS);
   
   if (!service) {
      printf(TEXT("In OpenService: Error %lu\n"),GetLastError());
      b= FALSE;
      goto c1;
   }

   // Stop the service if necessary
   success = QueryServiceStatus(service, &status);
   if (!success) {
      printf(TEXT("In QueryServiceStatus: Error %lu\n"),GetLastError());
      b= FALSE;
      goto c1;
   }

   if (status.dwCurrentState != SERVICE_STOPPED) {
      printf(TEXT("Stopping Service...\n"));
      success = ControlService(service,
                               SERVICE_CONTROL_STOP,
                               &status);
      if (!success) {
         printf(TEXT("In ControlService: Error %lu\n"),GetLastError());
         b= FALSE;
         goto c1;
      }
      Sleep(500);
   }

   // Remove the service
   success = DeleteService(service);
   if (success)
      printf(TEXT("Finished Deleting Service.\n"));
   else {
      printf(TEXT("Problem Deleting Service: Error %lu.\n"),GetLastError());
      if ( ERROR_SERVICE_MARKED_FOR_DELETE == GetLastError()) {
         printf(TEXT(" This service is already marked for delete.\n"));
      }
      b= FALSE;
      goto c1;
   }


   // remove registry entries to support logging messages
   // The source name as a subkey under the Application
   // key in the EventLog service portion of the registry.
   
   strcpy(szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
   strcat(szKey, szServiceNameSCM);
   if (RegDeleteKey(HKEY_LOCAL_MACHINE, szKey) != ERROR_SUCCESS) {
      b = FALSE;
      goto c1;
   }

   // EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
   //LogEvent(EVENTLOG_INFORMATION_TYPE, 1, m_szServiceName);

   // clean up
  c1:
   CloseServiceHandle(service);
  c0: 
   CloseServiceHandle(scm);
   return b;
}


void 
CService::
LogEvent( WORD wType, DWORD dwID,
         const char* pszS1,
         const char* pszS2,
         const char* pszS3)
{
    const char* ps[3];
    ps[0] = pszS1;
    ps[1] = pszS2;
    ps[2] = pszS3;

    WORD iStr = 0;
    for (int i = 0; i < 3; i++) {
        if (ps[i] != NULL) 
            iStr++;
    }
        
    // Check the event source has been registered and if
    // not then register it now
    if (!m_hEventSource) {
        m_hEventSource = RegisterEventSource(NULL, // local machine
                                             m_szServiceName); // source name
    }
    if (m_hEventSource) {
        ::ReportEvent(m_hEventSource,
                      wType,
                      0,
                      dwID,
                      NULL, // sid
                      iStr,
                      0,
                      ps,
                      NULL);
    }
}

VOID
CService::
StopService(){ 
   
   m_bRunning = FALSE;
   SetEvent(m_hEVENT_TerminateServiceThread); 
   SetEvent(m_hEVENT_TerminateServiceMain); 
}


/********************************************************

 BOOL
 CService::
 SetIcon(HICON hicon, LPCSTR lpTip)
 {

    UINT msg;
    m_nid.uFlags = 0;
    // Set the icon
    if (hicon) {
       // Add or replace icon in system tray
       msg = m_nid.hIcon ? NIM_MODIFY : NIM_ADD;
       m_nid.hIcon = hicon;
       m_nid.uFlags |= NIF_ICON;
    } else { // remove icon from tray
       if (m_nid.hIcon==NULL)
          return TRUE;  // already deleted
       msg = NIM_DELETE;
    }


    // Use the tip, if any
    if (lpTip)
       strncpy(m_nid.szTip, lpTip, sizeof(m_nid.szTip));
    if (m_nid.szTip[0])
       m_nid.uFlags |= NIF_TIP;
    // Use callback if any
    if (m_nid.uCallbackMessage && m_nid.hWnd)
       m_nid.uFlags |= NIF_MESSAGE;
    // Do it
    BOOL bRet = Shell_NotifyIcon(msg, &m_nid);
    if (msg==NIM_DELETE || !bRet)
       m_nid.hIcon = NULL; // failed
    return bRet;
 }

********************************************************/


/***********************************************************************************************


 // NTService.cpp
 //
 // Implementation of CNTService

 #include <windows.h>
 #include <stdio.h>
 #include "NTService.h"



 // static variables
 CNTService* CNTService::m_pThis = NULL;

 CNTService::CNTService(const char* szServiceName)
 {
     // copy the address of the current object so we can access it from
     // the static member callback functions.
     // WARNING: This limits the application to only one CNTService object.
     m_pThis = this;

     // Set the default service name and version
     strncpy(m_szServiceName, szServiceName, sizeof(m_szServiceName)-1);
     m_iMajorVersion = 1;
     m_iMinorVersion = 0;
     m_hEventSource = NULL;

     // set up the initial service status
     m_hServiceStatus = NULL;
     m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
     m_Status.dwCurrentState = SERVICE_STOPPED;
     m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
     m_Status.dwWin32ExitCode = 0;
     m_Status.dwServiceSpecificExitCode = 0;
     m_Status.dwCheckPoint = 0;
     m_Status.dwWaitHint = 0;
     m_bIsRunning = FALSE;
 }

 CNTService::~CNTService()
 {
     DebugMsg("CNTService::~CNTService()");
     if (m_hEventSource) {
         ::DeregisterEventSource(m_hEventSource);
     }
 }

 ////////////////////////////////////////////////////////////////////////////////////////
 // Default command line argument parsing

 // Returns TRUE if it found an arg it recognised, FALSE if not
 // Note: processing some arguments causes output to stdout to be generated.
 BOOL CNTService::ParseStandardArgs(int argc, char* argv[])
 {
     // See if we have any command line args we recognise
     if (argc <= 1) return FALSE;

     if (_stricmp(argv[1], "-v") == 0) {

         // Spit out version info
         printf("%s Version %d.%d\n",
                m_szServiceName, m_iMajorVersion, m_iMinorVersion);
         printf("The service is %s installed\n",
                IsInstalled() ? "currently" : "not");
         return TRUE; // say we processed the argument

     } else if (_stricmp(argv[1], "-i") == 0) {

         // Request to install.
         if (IsInstalled()) {
             printf("%s is already installed\n", m_szServiceName);
         } else {
             // Try and install the copy that's running
             if (Install()) {
                 printf("%s installed\n", m_szServiceName);
             } else {
                 printf("%s failed to install. Error %d\n", m_szServiceName, GetLastError());
             }
         }
         return TRUE; // say we processed the argument

     } else if (_stricmp(argv[1], "-u") == 0) {

         // Request to uninstall.
         if (!IsInstalled()) {
             printf("%s is not installed\n", m_szServiceName);
         } else {
             // Try and remove the copy that's installed
             if (Uninstall()) {
                 // Get the executable file path
                 char szFilePath[_MAX_PATH];
                 ::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
                 printf("%s removed. (You must delete the file (%s) yourself.)\n",
                        m_szServiceName, szFilePath);
             } else {
                 printf("Could not remove %s. Error %d\n", m_szServiceName, GetLastError());
             }
         }
         return TRUE; // say we processed the argument

     }

     // Don't recognise the args
     return FALSE;
 }

 ////////////////////////////////////////////////////////////////////////////////////////
 // Install/uninstall routines

 // Test if the service is currently installed
 BOOL CNTService::IsInstalled()
 {
     BOOL bResult = FALSE;

     // Open the Service Control Manager
     SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                      NULL, // ServicesActive database
                                      SC_MANAGER_ALL_ACCESS); // full access
     if (hSCM) {

         // Try to open the service
         SC_HANDLE hService = ::OpenService(hSCM,
                                            m_szServiceName,
                                            SERVICE_QUERY_CONFIG);
         if (hService) {
             bResult = TRUE;
             ::CloseServiceHandle(hService);
         }

         ::CloseServiceHandle(hSCM);
     }

     return bResult;
 }

 BOOL CNTService::Install()
 {
     // Open the Service Control Manager
     SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                      NULL, // ServicesActive database
                                      SC_MANAGER_ALL_ACCESS); // full access
     if (!hSCM) return FALSE;

     // Get the executable file path
     char szFilePath[_MAX_PATH];
     ::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));

     // Create the service
     SC_HANDLE hService = ::CreateService(hSCM,
                                          m_szServiceName,
                                          m_szServiceName,
                                          SERVICE_ALL_ACCESS,
                                          SERVICE_WIN32_OWN_PROCESS,
                                          SERVICE_DEMAND_START,        // start condition
                                          SERVICE_ERROR_NORMAL,
                                          szFilePath,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
     if (!hService) {
         ::CloseServiceHandle(hSCM);
         return FALSE;
     }

     // make registry entries to support logging messages
     // Add the source name as a subkey under the Application
     // key in the EventLog service portion of the registry.
     char szKey[256];
     HKEY hKey = NULL;
     strcpy(szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
     strcat(szKey, m_szServiceName);
     if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
         ::CloseServiceHandle(hService);
         ::CloseServiceHandle(hSCM);
         return FALSE;
     }

     // Add the Event ID message-file name to the 'EventMessageFile' subkey.
     ::RegSetValueEx(hKey,
                     "EventMessageFile",
                     0,
                     REG_EXPAND_SZ,
                     (CONST BYTE*)szFilePath,
                     strlen(szFilePath) + 1);

     // Set the supported types flags.
     DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
     ::RegSetValueEx(hKey,
                     "TypesSupported",
                     0,
                     REG_DWORD,
                     (CONST BYTE*)&dwData,
                      sizeof(DWORD));
     ::RegCloseKey(hKey);

     LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, m_szServiceName);

     // tidy up
     ::CloseServiceHandle(hService);
     ::CloseServiceHandle(hSCM);
     return TRUE;
 }

 BOOL CNTService::Uninstall()
 {
     // Open the Service Control Manager
     SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                      NULL, // ServicesActive database
                                      SC_MANAGER_ALL_ACCESS); // full access
     if (!hSCM) return FALSE;

     BOOL bResult = FALSE;
     SC_HANDLE hService = ::OpenService(hSCM,
                                        m_szServiceName,
                                        DELETE);
     if (hService) {
         if (::DeleteService(hService)) {
             LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_REMOVED, m_szServiceName);
             bResult = TRUE;
         } else {
             LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_NOTREMOVED, m_szServiceName);
         }
         ::CloseServiceHandle(hService);
     }

     ::CloseServiceHandle(hSCM);
     return bResult;
 }

 ///////////////////////////////////////////////////////////////////////////////////////
 // Logging functions

 // This function makes an entry into the application event log
 void CNTService::LogEvent(WORD wType, DWORD dwID,
                           const char* pszS1,
                           const char* pszS2,
                           const char* pszS3)
 {
     const char* ps[3];
     ps[0] = pszS1;
     ps[1] = pszS2;
     ps[2] = pszS3;

     int iStr = 0;
     for (int i = 0; i < 3; i++) {
         if (ps[i] != NULL) iStr++;
     }

     // Check the event source has been registered and if
     // not then register it now
     if (!m_hEventSource) {
         m_hEventSource = ::RegisterEventSource(NULL,  // local machine
                                                m_szServiceName); // source name
     }

     if (m_hEventSource) {
         ::ReportEvent(m_hEventSource,
                       wType,
                       0,
                       dwID,
                       NULL, // sid
                       iStr,
                       0,
                       ps,
                       NULL);
     }
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 // Service startup and registration

 BOOL CNTService::StartService()
 {
     SERVICE_TABLE_ENTRY st[] = {
         {m_szServiceName, ServiceMain},
         {NULL, NULL}
     };

     DebugMsg("Calling StartServiceCtrlDispatcher()");
     BOOL b = ::StartServiceCtrlDispatcher(st);
     DebugMsg("Returned from StartServiceCtrlDispatcher()");
     return b;
 }

 // static member function (callback)
 void CNTService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
 {
     // Get a pointer to the C++ object
     CNTService* pService = m_pThis;

     pService->DebugMsg("Entering CNTService::ServiceMain()");
     // Register the control request handler
     pService->m_Status.dwCurrentState = SERVICE_START_PENDING;
     pService->m_hServiceStatus = RegisterServiceCtrlHandler(pService->m_szServiceName,
                                                            Handler);
     if (pService->m_hServiceStatus == NULL) {
         pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_CTRLHANDLERNOTINSTALLED);
         return;
     }

     // Start the initialisation
     if (pService->Initialize()) {

         // Do the real work.
         // When the Run function returns, the service has stopped.
         pService->m_bIsRunning = TRUE;
         pService->m_Status.dwWin32ExitCode = 0;
         pService->m_Status.dwCheckPoint = 0;
         pService->m_Status.dwWaitHint = 0;
         pService->Run();
     }

     // Tell the service manager we are stopped
     pService->SetStatus(SERVICE_STOPPED);
     pService->DebugMsg("Leaving CNTService::ServiceMain()");
 }


 void CNTService::
 SetStatus(DWORD dwState)
 // status functions
 {
     DebugMsg("CNTService::SetStatus(%lu, %lu)", m_hServiceStatus, dwState);
     m_Status.dwCurrentState = dwState;
     ::SetServiceStatus(m_hServiceStatus, &m_Status);
 }


 BOOL CNTService::
 Initialize()
 // Service initialization
 {
     DebugMsg("Entering CNTService::Initialize()");

     // Start the initialization
     SetStatus(SERVICE_START_PENDING);

     // Perform the actual initialization
     BOOL bResult = OnInit();

     // Set final state
     m_Status.dwWin32ExitCode = GetLastError();
     m_Status.dwCheckPoint = 0;
     m_Status.dwWaitHint = 0;
     if (!bResult) {
         LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILEDINIT);
         SetStatus(SERVICE_STOPPED);
         return FALSE;
     }

     LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED);
     SetStatus(SERVICE_RUNNING);

     DebugMsg("Leaving CNTService::Initialize()");
     return TRUE;
 }


 void CNTService::
 Run()
 // main function to do the real work of the service

 // This function performs the main work of the service.
 // When this function returns the service has stopped.
 {
     DebugMsg("Entering CNTService::Run()");

     while (m_bIsRunning) {
         DebugMsg("Sleeping...");
         Sleep(5000);
     }

     // nothing more to do
     DebugMsg("Leaving CNTService::Run()");
 }

 //////////////////////////////////////////////////////////////////////////////////////
 // Control request handlers

 // static member function (callback) to handle commands from the
 // service control manager
 void CNTService::
 Handler(DWORD dwOpcode)
 {
     // Get a pointer to the object
     CNTService* pService = m_pThis;

     pService->DebugMsg("CNTService::Handler(%lu)", dwOpcode);
     switch (dwOpcode) {
     case SERVICE_CONTROL_STOP: // 1
         pService->SetStatus(SERVICE_STOP_PENDING);
         pService->OnStop();
         pService->m_bIsRunning = FALSE;
         pService->LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
         break;

     case SERVICE_CONTROL_PAUSE: // 2
         pService->OnPause();
         break;

     case SERVICE_CONTROL_CONTINUE: // 3
         pService->OnContinue();
         break;

     case SERVICE_CONTROL_INTERROGATE: // 4
         pService->OnInterrogate();
         break;

     case SERVICE_CONTROL_SHUTDOWN: // 5
         pService->OnShutdown();
         break;

     default:
         if (dwOpcode >= SERVICE_CONTROL_USER) {
             if (!pService->OnUserControl(dwOpcode)) {
                 pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
             }
         } else {
             pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
         }
         break;
     }

     // Report current status
     pService->DebugMsg("Updating status (%lu, %lu)",
                        pService->m_hServiceStatus,
                        pService->m_Status.dwCurrentState);
     ::SetServiceStatus(pService->m_hServiceStatus, &pService->m_Status);
 }

 // Called when the service is first initialized
 BOOL CNTService::OnInit()
 {
     DebugMsg("CNTService::OnInit()");
     return TRUE;
 }

 void CNTService::
 OnStop()
 // Called when the service control manager wants to stop the service
 {
     DebugMsg("CNTService::OnStop()");
 }

 void CNTService::
 OnInterrogate()
 // called when the service is interrogated
 {
     DebugMsg("CNTService::OnInterrogate()");
 }

 // called when the service is paused
 void CNTService::
 OnPause()
 {
     DebugMsg("CNTService::OnPause()");
 }

 // called when the service is continued
 void CNTService::
 OnContinue()
 {
     DebugMsg("CNTService::OnContinue()");
 }

 // called when the service is shut down
 void CNTService::
 OnShutdown()
 {
     DebugMsg("CNTService::OnShutdown()");
 }

 // called when the service gets a user control message
 BOOL CNTService::
 OnUserControl(DWORD dwOpcode)
 {
     DebugMsg("CNTService::OnUserControl(%8.8lXH)", dwOpcode);
     return FALSE; // say not handled
 }



 void CNTService::
 DebugMsg(const char* pszFormat, ...)
 {
     char buf[1024];
     sprintf(buf, "[%s](%lu): ", m_szServiceName, GetCurrentThreadId());
     va_list arglist;
     va_start(arglist, pszFormat);
     vsprintf(&buf[strlen(buf)], pszFormat, arglist);
     va_end(arglist);
     strcat(buf, "\n");
     OutputDebugString(buf);
 }

***********************************************************************************************/
