/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: FlyParamsUI.cpp
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
#include "flyingparams.h"
#include "resource.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment( lib, "comctl32.lib" )

// Local helper functions
BOOL CALLBACK VADlgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CallSelChangeParams( HTREEITEM hItemSel );

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
          Cleanup();
          return 0; //done
      }

      if (f != -1)
      {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
   }
}

HWND hWndProgams = NULL;
HWND hWndComponents = NULL;
HWND hWndFunctions = NULL;
HWND hWndParams    = NULL;
HWND hWndValues    = NULL;
HWND hWndMachine   = NULL;

// Global used to know where to put items in param tree
HTREEITEM g_hItemParent = TVI_ROOT;

BOOL CALLBACK VADlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static DWORD mytimer;
   
   switch (msg)
   {
      case WM_INITDIALOG:
      {
         hWndProgams  = GetDlgItem( hWnd, IDC_PROCESS );
         hWndComponents = GetDlgItem( hWnd, IDC_COMPONENT );
         hWndFunctions  = GetDlgItem( hWnd, IDC_FUNCTION );
         hWndParams     = GetDlgItem( hWnd, IDC_PARAMS );
         hWndValues     = GetDlgItem( hWnd, IDC_VALUES );
         hWndMachine    = GetDlgItem( hWnd, IDC_MACHINE );
         
         LVCOLUMN col;
         col.mask = LVCF_TEXT | LVCF_WIDTH;
         col.cx = 85;
         col.pszText = "Value";
         SendMessage( hWndValues, LVM_INSERTCOLUMN, 0, LPARAM(&col) );

         col.cx = 65;
         col.pszText = "Count";
         SendMessage( hWndValues, LVM_INSERTCOLUMN, 1, LPARAM(&col) );

         ListView_SetExtendedListViewStyle( hWndValues, LVS_EX_FULLROWSELECT );
         
         // Fill the process list
         char szMachine[256];
         GetWindowText( hWndMachine, szMachine, sizeof(szMachine) );
         RefreshProcesses(szMachine);

         SetTimer( hWnd, 42, 300, NULL );
         return true;
      }

      case WM_TIMER:
      {
          if (wParam == 42)
          {
              int c = CValuesUpdateValueData();
              ListView_SetItemCountEx( hWndValues, c, LVSICF_NOSCROLL  );
              InvalidateRect( hWndValues, NULL, true );
              
              UpdateProcedureHitData();
              InvalidateRect( hWndFunctions, NULL, true );
              return true;
          }
      }

      case WM_CLOSE:
      {
          PostQuitMessage(0);
          EndDialog( hWnd, 0 );
          return true;
      }


      case WM_MEASUREITEM: 
      {
          MEASUREITEMSTRUCT *pmis = (MEASUREITEMSTRUCT *)lParam; 

          HDC hDC = GetDC(hWndFunctions);

          TEXTMETRIC tm; 
          GetTextMetrics(hDC, &tm); 

          ReleaseDC( hWndFunctions, hDC );

          pmis->itemHeight = tm.tmHeight; 

          return true; 
      }
 
      case WM_DRAWITEM: 
      {
          DRAWITEMSTRUCT *lpdis = (DRAWITEMSTRUCT *) lParam; 
          if (lpdis->itemID == -1) 
          { 
             break; 
          } 
 
          // Draw the bitmap and text for the list box item. Draw a 
          // rectangle around the bitmap if it is selected. 
 
          switch (lpdis->itemAction) 
          { 
             case ODA_SELECT: 
             case ODA_DRAWENTIRE: 
                // Display the bitmap associated with the item. 
 
                VProc *pProc = (VProc *)(lpdis->itemData);
                bool fHit;
                const char *szName = SzNameAndHitFromProcedure( pProc, lpdis->itemID, fHit );
                
                TEXTMETRIC tm; 
                GetTextMetrics(lpdis->hDC, &tm); 
                
                COLORREF cobk;
                COLORREF co = RGB(0,0,0);
                
                if (fHit)
                {
                    co = RGB(255, 0, 0);
                }

                co = SetTextColor( lpdis->hDC, co );
                
                if (lpdis->itemState & ODS_SELECTED) 
                {
                    cobk = SetBkColor( lpdis->hDC, RGB(0,0,128) );
                    SetTextColor( lpdis->hDC, RGB(255,255,255) );
                }
 
                int y = (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2; 
                ExtTextOut( lpdis->hDC, 2, y, ETO_OPAQUE | ETO_CLIPPED, &lpdis->rcItem, szName, strlen(szName), NULL );

                SetTextColor( lpdis->hDC, co );
 
                // Is the item selected? 
                if (lpdis->itemState & ODS_SELECTED) 
                { 
                    SetBkColor( lpdis->hDC, cobk );
                } 
                break; 
          }
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
                          int iSel = SendMessage( hWndProgams, LB_GETCURSEL, 0, 0 ); 
                          if (iSel == LB_ERR)
                          {
                              OnSelChangeProcesses( NULL );
                          }
                          else
                          {
                              VProg *pProg = (VProg *)SendMessage( hWndProgams, LB_GETITEMDATA, iSel, 0 );
                              OnSelChangeProcesses( pProg );
                          }
                          break;
                      }

                      case IDC_COMPONENT:
                      {
                          int iSel = SendMessage( hWndComponents, LB_GETCURSEL, 0, 0 ); 
                          if (iSel == LB_ERR)
                          {
                              OnSelChangeComponent( NULL );
                          }
                          else
                          {
                              VComp *pComp = (VComp *)SendMessage( hWndComponents, LB_GETITEMDATA, iSel, 0 );
                              OnSelChangeComponent( pComp );
                          }
                          break;
                      }

                      case IDC_FUNCTION:
                      {
                          int iSel = SendMessage( hWndFunctions, LB_GETCURSEL, 0, 0 ); 
                          if (iSel == LB_ERR)
                          {
                              OnSelChangeProcedures( NULL );    
                          }
                          else
                          {
                              VProc *pProc = (VProc *)SendMessage( hWndFunctions, LB_GETITEMDATA, iSel, 0 );
                              OnSelChangeProcedures( pProc );
                          }
                          break;
                      }
                  }
                  break;

            case BN_CLICKED:
                  switch (idCtrl)
                  {
                      case IDC_REFRESH:
                      {
                          char szMachine[256];
                          GetWindowText( hWndMachine, szMachine, sizeof(szMachine) );
                          RefreshProcesses(szMachine);
                          break;
                      }
                  }
          }
          break;
      }

      case WM_NOTIFY:
      {
          DWORD idCtrl = wParam;
          NMHDR *pnmh = (NMHDR *) lParam; 

          if (pnmh->code == LVN_GETDISPINFO)
          {
              NMLVDISPINFO *pdi = (NMLVDISPINFO*)pnmh;
              static char szGlobalBuf[100];
              if (pdi->item.mask | LVIF_TEXT)
              {
                  switch (pdi->item.iSubItem)
                  {
                  case 0:
                      GetValueStringFromValueIndex( pdi->item.iItem, szGlobalBuf );
                      pdi->item.pszText = szGlobalBuf;
                      break;

                  case 1:
                      int count = GetValueCountFromValueIndex( pdi->item.iItem );
                      sprintf(szGlobalBuf, "%d", count );
                      pdi->item.pszText = szGlobalBuf;
                      break;
                  }
              }
          }

          if (pnmh->code == TVN_SELCHANGED)
          {
              // The only tree
              NMTREEVIEW *pnmtv = (NMTREEVIEW *)pnmh;
              CallSelChangeParams(pnmtv->itemNew.hItem);
          }

          if (pnmh->code == TVN_ITEMEXPANDING)
          {
              NMTREEVIEW *pnmtv = (NMTREEVIEW *)pnmh;
              if (pnmtv->hdr.idFrom == IDC_PARAMS &&
                  pnmtv->action == TVE_EXPAND )
              {
                  if (NULL == TreeView_GetChild( pnmtv->hdr.hwndFrom, pnmtv->itemNew.hItem ))
                  {
                      // should have children, but it doesn't yet.. add them now
                      g_hItemParent = pnmtv->itemNew.hItem;
                      OnExpandParam( (VSymbol *)pnmtv->itemNew.lParam );
                      g_hItemParent = TVI_ROOT;
                  }
              }
          }
      }

   }

   return false;
}

void ClearProgramList()
{
    SendMessage( hWndProgams, LB_RESETCONTENT, 0 ,0 );
}

void AddProgramToList( const char *sz, VProg *pProg )
{
    int iItem = SendMessage( hWndProgams, LB_ADDSTRING, 0, WPARAM(sz) );
    SendMessage( hWndProgams, LB_SETITEMDATA, iItem, LPARAM(pProg) ); 
}

void ClearComponentList()
{
    SendMessage( hWndComponents, LB_RESETCONTENT, 0, 0 );
}

void AddComponentToList( const char *sz, VComp *pComp )
{
    int iItem = SendMessage( hWndComponents, LB_ADDSTRING, 0, WPARAM(sz) );
    SendMessage( hWndComponents, LB_SETITEMDATA, iItem, LPARAM(pComp) ); 
}

void ClearProcedureList()
{
    SendMessage( hWndFunctions, LB_RESETCONTENT, 0, 0 );
}

void AddProcedureToList( VProc *pProc, int iProc )
{
    SendMessage( hWndFunctions, LB_ADDSTRING, iProc, WPARAM(pProc) );
}

void ClearParamsList()
{
    TreeView_DeleteAllItems( hWndParams );
}

void AddParamToTree( const char *szSym, VSymbol *pSym, bool fChildren )
{
    TVINSERTSTRUCT tvis;
    memset( &tvis, 0, sizeof(tvis));
    tvis.hParent = g_hItemParent;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT;
    tvis.item.pszText = (char *)szSym;
    tvis.item.lParam = LPARAM(pSym);
    tvis.item.cChildren = fChildren;

    SendMessage( hWndParams, TVM_INSERTITEM, 0, LPARAM( &tvis ) );
}

VSymbol *PSymFromHItem( HWND hParams, HTREEITEM hItem)
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof(TVITEM) );
    tvi.mask = TVIF_PARAM;
    tvi.hItem = hItem;
    TreeView_GetItem( hParams, LPARAM(&tvi));

    return (VSymbol *)tvi.lParam;
}

void CallSelChangeParams(HTREEITEM hItemSel)
{
    // First, build up the array of selected items
    if (hItemSel == NULL)
    {
        OnSelChangeParams( NULL, 0 );
        return;
    }

    // Go get the array of pSym's
    VSymbol *rgSym[256];
    
    HTREEITEM hItem = hItemSel;
    int i = 0;
    while (hItem && i <256)
    {
        rgSym[i++] = PSymFromHItem( hWndParams, hItem );
        hItem = TreeView_GetParent( hWndParams, hItem );
    }

    if (i == 256)
    {
        OnSelChangeParams( NULL, 0 );
        return;
    }

    OnSelChangeParams( rgSym, i );
}

void Error( const char *szError )
{
    MessageBox( NULL, szError, "Error", MB_OK | MB_ICONEXCLAMATION );
}