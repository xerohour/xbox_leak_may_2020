#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <CService.h>
/*++

   Filename :  2kNCipherService.cpp

   Description: Main for 2kNCipherService.cpp
   
   Created by:  Wally Ho

   History:     Created on 13/06/2001.

   Contains these functions:

--*/

  

INT WINAPI 
WinMain (HINSTANCE hInstance,
         HINSTANCE hPrev,
         LPSTR     lpCmdLine,
         INT       nCmdShow)
{

     UNREFERENCED_PARAMETER(nCmdShow);
     UNREFERENCED_PARAMETER(hPrev);
     UNREFERENCED_PARAMETER(lpCmdLine);
     UNREFERENCED_PARAMETER(hInstance);
   
    // Create the service object
    CService* s = new CService;
    // Parse for standard arguments (install, uninstall, version etc.)
    s->ParseStandardArgs();
    delete s;
    printf("Exiting...\n");
    // When we get here, the service has been stopped
    //return MyService.m_Status.dwWin32ExitCode;
    return TRUE;
}
