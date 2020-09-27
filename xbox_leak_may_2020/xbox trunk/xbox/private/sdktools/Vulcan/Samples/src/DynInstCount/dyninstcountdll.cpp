/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: DynInstCountDll.cpp
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
#include "vulcanapi.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdi32.lib")

void OutputDataToFile();
int *rgDynInstCount;
int cProc;
const char *pchNames;

extern "C" _declspec (dllexport)
void _cdecl SavePDynInstCount(int *p, int c, char *pch)
{
    rgDynInstCount = p;
    pchNames = pch;
    cProc = c;
}

// Code for handling the window
#include "resource.h"
#include <list>


DWORD WINAPI VAWnd( void *);
HINSTANCE hInst;
HWND hWnd;

extern "C" BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID)
{
   switch (dwReason)
   {
      case DLL_PROCESS_ATTACH :
         hInst = hModule;
         CreateThread( NULL, 0, &VAWnd, NULL, 0, NULL );
         break;

      case DLL_PROCESS_DETACH :
         OutputDataToFile();
         break;
   }

   return(TRUE);
}

struct FUNDATA
{
    const char *m_szFun;
    __int64 m_cPrev;
};

FUNDATA *v_rgFD;
VComp *v_pComp;

int CALLBACK CmpFunItem( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
    FUNDATA *p1 = (FUNDATA *)lParam1;
    FUNDATA *p2 = (FUNDATA *)lParam2;

    if (p2->m_cPrev > p1->m_cPrev)
    {
        return 1;
    }
    else if (p2->m_cPrev < p1->m_cPrev)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

std::list< int * > lstOverTimes;

void OutputOverTime()
{
   int *rgTimes = new int[ cProc + 1 ];

   lstOverTimes.push_back( rgTimes );

   rgTimes[cProc] = 0;
   for (int i=0; i<cProc; i++)
   {
      rgTimes[cProc] += rgTimes[i] = rgDynInstCount[i];
   }
    
}

__int64 UpdateCount( FUNDATA * pFD )
{
   int iProc = pFD - v_rgFD;

   pFD->m_cPrev += rgDynInstCount[iProc];
   rgDynInstCount[iProc] = 0;

   return pFD->m_cPrev;
}

int _cdecl cmpIFun( const void *pv1, const void *pv2)
{
   int i1 = *(const int *)pv1;
   int i2 = *(const int *)pv2;

   if (v_rgFD[i1].m_cPrev < v_rgFD[i2].m_cPrev)
   {
       return 1;
   }
   else if (v_rgFD[i1].m_cPrev > v_rgFD[i2].m_cPrev)
   {
       return -1;
   }
   else
   {
       return 0;
   }
}

void OutputDataToFile()
{
   if (v_rgFD)
   {
      OutputOverTime();

      int *rgFunSort = new int[ cProc ];
      for ( int iFun = 0; iFun < cProc; iFun ++)
      {
         UpdateCount( v_rgFD + iFun );
         rgFunSort[iFun] = iFun;
      }
      qsort( rgFunSort, cProc, sizeof(int), cmpIFun );

      FILE *pf = fopen( "dyninstcount.log", "w" );

      for ( iFun = 0; iFun < cProc; iFun ++)
      {
         int iSort = rgFunSort[iFun];
         fprintf(pf, "%I64d\t%s\n", v_rgFD[iSort].m_cPrev , v_rgFD[iSort].m_szFun );
      }

      fclose( pf );


      pf = fopen( "OverTime.log", "w" );
      for ( iFun = 0; iFun < min(cProc,50); iFun ++)
      {
         int iSort = rgFunSort[iFun];
         fprintf(pf, "%s\t", v_rgFD[iSort].m_szFun );
      }
      fprintf(pf, "Others\n");

      for (std::list< int * >::iterator i =  lstOverTimes.begin(); i != lstOverTimes.end(); i++)
      {
         int cRest = (*i)[cProc];

         for ( iFun = 0; iFun < min(cProc,50); iFun ++)
         {
            int iSort = rgFunSort[iFun];
            cRest -= (*i)[iSort];
            fprintf(pf, "%d\t", (*i)[iSort] );
         }

         fprintf(pf, "%d\n", cRest );
      }

      fclose(pf);
      delete [] rgFunSort;
   }
}

BOOL CALLBACK VADlgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static DWORD mytimer;
   
   switch (msg)
   {
      case WM_INITDIALOG:
      {
         // Add 2 columns to the listview
         HWND hwndList = GetDlgItem( hwnd, IDC_FUNCTIONS );
         LV_COLUMN lvc;
         lvc.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
         lvc.cx = 150;
         lvc.fmt = LVCFMT_LEFT;
         lvc.pszText = "Function";
         lvc.iSubItem = 0;
         ListView_InsertColumn( hwndList, 0, &lvc );
         
         lvc.cx = 100;
         lvc.pszText = "Dyn Inst Count";
         lvc.iSubItem = 1;
         ListView_InsertColumn( hwndList, 1, &lvc );
         
         // Open data file and read it in
         int iFun = 0;
         v_rgFD = new FUNDATA[ cProc ];
         const char *pch = pchNames;

         for (iFun = 0; iFun < cProc; iFun++)
         {
             FUNDATA *pFD = v_rgFD + iFun;
             pFD->m_cPrev = 0;

             pFD->m_szFun = pch;
             pch += strlen(pch) + 1;

             // Add to list control
             LV_ITEM lvi;
             lvi.mask = LVIF_PARAM | LVIF_TEXT;
             lvi.iItem = iFun;
             lvi.iSubItem = 0;
             lvi.pszText = (char *)pFD->m_szFun;
             lvi.lParam = LPARAM(pFD);

             ListView_InsertItem( hwndList, &lvi );

             char sz[25];
             sprintf(sz, "%u", 0 );
             lvi.mask = LVIF_TEXT;
             lvi.iItem = iFun;
             lvi.iSubItem = 1;
             lvi.pszText = sz;
             lvi.lParam = LPARAM(pFD);

             ListView_InsertItem( hwndList, &lvi );
         }

         OutputOverTime();

         mytimer = SetTimer(hwnd, 42, 1000, NULL );
         break;
      }

      case WM_TIMER:
      {
         if ( mytimer == wParam)
         {
            OutputOverTime();
                 HWND hwndList = GetDlgItem( hwnd, IDC_FUNCTIONS );

            // Update the IC column

            for ( int iFun = 0; iFun < cProc; iFun ++)
            {
               LV_ITEM lvi;
               lvi.mask = LVIF_PARAM;
               lvi.iItem = iFun;
               lvi.iSubItem = 0;

               if (ListView_GetItem( hwndList, &lvi ))
               {
                  FUNDATA *pfd = (FUNDATA *)lvi.lParam;

                  char sz[25];
                  __int64 i = UpdateCount(pfd);
                                      sprintf(sz, "%I64d",  i);

                  lvi.mask = LVIF_TEXT;
                  lvi.iItem = iFun;
                  lvi.iSubItem = 1;
                  lvi.pszText = sz;

                  ListView_SetItem( hwndList, &lvi );
               }
            }

            ListView_SortItems( hwndList, CmpFunItem, 0 );
         }
         break;
      }
   }
   
   return false;
}

#pragma comment( lib, "comctl32.lib" )

DWORD WINAPI VAWnd( void *)
{
   INITCOMMONCONTROLSEX icc;
   icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icc.dwICC = ICC_LISTVIEW_CLASSES;
   InitCommonControlsEx( &icc );

   hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_STATUSDLG), NULL, VADlgProc ); 
   DWORD dw = GetLastError();
   ShowWindow(hWnd, SW_SHOW);

   // Pump messages

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
