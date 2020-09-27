/*DOC*
**
** Suite : 
** File  : main.c
**
** Revision History :
** M001 2-Feb-1994
** - created from chain1.c from old callstack sources.
**
**DOC*/


#include <windows.h>
#include "chain3.h"

#define IDM_EXIT  100

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
int MyPostMessage( HWND hwnd );
extern int ChainMain();
extern int OtherMain();

#if !defined( SAVELABEL )
#define SAVELABEL( l )	if (0) goto l
#endif

#ifdef DLL
extern int callback();
int WINAPI mycallbackfcn();
#endif

char szGExeBuffer[128];

void __cdecl main()
{

     static char szAppName[] = "foo" ;
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

     if (!hPrevInstance)
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = (WNDPROC) WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION); 
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
          wndclass.lpszMenuName  = szAppName ;  // MAC?
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindowEx(
#if defined( _MAC )
					WS_EX_FORCESIZEBOX,
#else
					0,
#endif
     				szAppName,         // window class name
                    szAppName, 				// window caption
                    WS_OVERLAPPEDWINDOW,     // window style
                    CW_USEDEFAULT,           // initial x position
                    0, // CW_USEDEFAULT,           // initial y position
                    CW_USEDEFAULT,           // initial x size
                    0, // CW_USEDEFAULT,           // initial y size
                    NULL,                    // parent window handle
                    NULL,                    // window menu handle
                    hInstance,               // program instance handle
                    NULL) ;                  // creation parameters
     if (hwnd==0) {   //window not created
         return 1;
     }

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;
//     PostMessage( hwnd, WM_NULL, (WORD)0, (DWORD)0);

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
     {
     HDC         hdc ;
     PAINTSTRUCT ps ;
     RECT        rect ;
     static HINSTANCE hInst;
     static FARPROC pCB;
     int x;
	 static int y = -1;

	 SAVELABEL(TagCallNoCVInfo);
	 SAVELABEL(TagCallOtherMain);
	 SAVELABEL(TagCallChainMain);
	 SAVELABEL(TagCreateCase);

     switch (message)
          {
          case WM_CREATE:
TagCreateCase: hInst= ((LPCREATESTRUCT) lParam)->hInstance;

               ////////////// MAKE TESTING CALLS HERE  /////////

TagCallChainMain: ChainMain();

TagCallOtherMain: OtherMain();

TagCallNoCVInfo: y = chain_3(99);

#ifdef DLL
CALL_CALLBACK:;
               pCB = MakeProcInstance(mycallbackfcn, hInst);
               x= callback(pCB, 4);
#endif
               x = 0;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;
               GetClientRect (hwnd, &rect) ;
               DrawText (hdc, "Hello, Windows!", -1, &rect,
                          DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;
               EndPaint (hwnd, &ps) ;
               return 0 ;

#if defined( _MAC )
          case WM_SYSCOMMAND:
#endif
          case WM_COMMAND:        /* message: command from application menu */
               switch( wParam )
                  {
                  case IDM_EXIT:
                       DestroyWindow(hwnd);
                       break;
                  }
               break;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
#if !defined( _MAC )
           case WM_NULL:
               MyPostMessage(hwnd);
               return 0 ;
#endif
          }

     return DefWindowProc (hwnd, message, wParam, lParam) ;
}


int MyPostMessage( HWND hwnd )
{
    static int count = 0;

#if !defined( _MAC )
	SAVELABEL(TagPost);
					 
    if ( count < 5 )
    {
TagPost: PostMessage( hwnd, WM_PAINT, (WORD)0, (DWORD)0);
         PostMessage( hwnd, 0x0040, (WORD)0, (DWORD)0);
    }
    return count++;
#endif 

}


#ifdef DLL
int WINAPI mycallbackfcn()
{
   int x;

START:
   x=1;
   x=x+3;
   return x;
END:;
};
#endif



