/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: FlyingICountUI.cpp
*
* File Comments:
*
*
***********************************************************************/

#include <stdio.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "commctrl.h"
#include "resource.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib" )

class VProg;
class VComp;
class VProc;
#include "flyingicount.h"

HWND hWndProcesses;
HWND hWndModules;
HWND hWndProcedures;


BOOL CALLBACK VADlgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL __stdcall WinMain(HINSTANCE hModule, HINSTANCE hInst, LPSTR lpCmdLine, int nCmdShow)
{
   INITCOMMONCONTROLSEX icc;
   icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icc.dwICC = ICC_LISTVIEW_CLASSES;
   InitCommonControlsEx( &icc );

   HWND hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_STATUSDLG), NULL, VADlgProc ); 
   ShowWindow(hWnd, nCmdShow);

   //pump messages
   while (1)
   {
      MSG msg;
      BOOL f = GetMessage( &msg, NULL, 0, 0 );
      if (!f)
      {
         return 0; //done
      }

      if (f != -1)
      {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
   }
}

BOOL CALLBACK VADlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static DWORD mytimer;
   
   switch (msg)
   {
      case WM_INITDIALOG:
      {
         hWndProcesses  = GetDlgItem( hWnd, IDC_PROCESS );
         hWndModules    = GetDlgItem( hWnd, IDC_COMPONENT );
         hWndProcedures = GetDlgItem( hWnd, IDC_FUNCTION );

         // Fill the process list
         RefreshProcesses();

         SetTimer( hWnd, 42, 100, NULL );
         return true;
      }

      case WM_TIMER:
      {
          if (wParam == 42)
          {
              int i = GetCounter();
              SetDlgItemInt( hWnd, IDC_ICOUNT, i, false );
              return true;
          }
      }

      case WM_CLOSE:
      {
          PostQuitMessage(0);
          EndDialog( hWnd, 0 );
          return true;
      }

      case WM_COMMAND:
      {
          DWORD idCtrl = LOWORD(wParam);
          switch (HIWORD(wParam))
          {
              case LBN_SELCHANGE:
                  switch (idCtrl)
                  {
                      case IDC_PROCESS:
                      {
                          int iSel = SendMessage( hWndProcesses, LB_GETCURSEL, 0, 0 ); 
                          VProg *pProg = NULL;
                          if (iSel != LB_ERR)
                          {
                             pProg = (VProg *)SendMessage( hWndProcesses, LB_GETITEMDATA, iSel, 0 );
                          }
                          
                          RefreshModules(pProg);
                          break;
                      }

                      case IDC_COMPONENT:
                      {
                          int iSel = SendMessage( hWndModules, LB_GETCURSEL, 0, 0 ); 
                          VComp *pComp = NULL;
                          if (iSel != LB_ERR)
                          {
                             pComp = (VComp *)SendMessage( hWndModules, LB_GETITEMDATA, iSel, 0 );
                          }
                          
                          RefreshFunctions(pComp);
                          break;
                      }

                      case IDC_FUNCTION:
                      {
                          int iSel = SendMessage( hWndProcedures, LB_GETCURSEL, 0, 0 ); 
                          VProc *pProc = NULL;
                          if (iSel != LB_ERR)
                          {
                             pProc = (VProc *)SendMessage( hWndProcedures, LB_GETITEMDATA, iSel, 0 );
                          }
                          
                          SetInstrumentation(pProc);
                          break;
                      }
                  }
                  break;

            case BN_CLICKED:
                  switch (idCtrl)
                  {
                      case IDC_REFRESH:
                          RefreshProcesses();
                          break;
                  }
          }
          break;
      }
   }

   return false;
}

void ClearProgramList()
{
    SendMessage( hWndProcesses, LB_RESETCONTENT, 0, 0 );
}

void AddProgramToList( const char *sz, VProg *pProg )
{
    int iItem = SendMessage( hWndProcesses, LB_ADDSTRING, 0, WPARAM(sz) );
    SendMessage( hWndProcesses, LB_SETITEMDATA, iItem, LPARAM(pProg) ); 
}

void ClearComponentList()
{
    SendMessage( hWndModules, LB_RESETCONTENT, 0, 0 );
}

void AddComponentToList( const char *sz, VComp *pComp )
{
    int iItem = SendMessage( hWndModules, LB_ADDSTRING, 0, WPARAM(sz) );
    SendMessage( hWndModules, LB_SETITEMDATA, iItem, LPARAM(pComp) ); 
}

void ClearFunctionList()
{
    SendMessage( hWndProcedures, LB_RESETCONTENT, 0, 0 );
}

void AddFunctionToList( const char *sz, VProc *pProc )
{
    int iItem = SendMessage( hWndProcedures, LB_ADDSTRING, 0, WPARAM(sz) );
    SendMessage( hWndProcedures, LB_SETITEMDATA, iItem, LPARAM(pProc) ); 
}

void Error( const char *szError )
{
    MessageBox( NULL, szError, "Error", MB_OK | MB_ICONEXCLAMATION );
}