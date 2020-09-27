/*--------------------------------------------------------
   foo.C -- a hellowin.c vairiant, (c) Charles Petzold, 1990

  --------------------------------------------------------*/

#include <windows.h>
#define IDM_EXIT  100

LONG WINAPI WndProc (HWND hwnd, UINT message, UINT wParam, LONG lParam);
int MyPostMessage( HWND hwnd );

extern int foo();
extern int recursive_dll();
extern int FAR callback();
int WINAPI mycallbackfcn();
int global;

int WINAPI  WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                         LPSTR lpszCmdLine, int nCmdShow)
{

     static char szAppName[] = "foo" ;
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

     if (!hPrevInstance)
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon(hInstance, szAppName); // Icon name from .rc
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName,         // window class name
                    szAppName, 				// window caption
                    WS_OVERLAPPEDWINDOW,     // window style
                    CW_USEDEFAULT,           // initial x position
                    CW_USEDEFAULT,           // initial y position
                    CW_USEDEFAULT,           // initial x size
                    CW_USEDEFAULT,           // initial y size
                    NULL,                    // parent window handle
                    NULL,                    // window menu handle
                    hInstance,               // program instance handle
                    NULL) ;                  // creation parameters
     if (hwnd==0) {   //window not created
         return 1;
     }

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;
     PostMessage( hwnd, WM_NULL, (WORD)0, (DWORD)0);

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }


LONG WINAPI WndProc (HWND hwnd, UINT message, UINT wParam, LONG lParam)
     {
     HDC         hdc ;
     PAINTSTRUCT ps ;
     RECT        rect ;
     static HANDLE hInst;
     static FARPROC pCB;
     int local;
     int localx;

     switch (message)
          {
          case WM_CREATE:
               hInst= ((LPCREATESTRUCT) lParam)->hInstance;
               ////////////// MAKE TESTING CALLS HERE  /////////
               local= foo(1 , 2);
               localx= recursive_dll(3 , FALSE);
CALL_CALLBACK:;
               pCB = MakeProcInstance(mycallbackfcn, hInst);
               global= callback(pCB, 4);

               for (local=0; local < 4; local++)
               {
                   for (global=10; global < 13; global++)
                   {
                       localx=local+global;
                   };
               };	//	_MIPS_:local is incremented
               global=0;
               localx=local/global;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;
               GetClientRect (hwnd, &rect) ;
               DrawText (hdc, "Hello, Windows!", -1, &rect,
                          DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;
               EndPaint (hwnd, &ps) ;
               return 0 ;

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

          case WM_NULL:
               MyPostMessage(hwnd);
               return 0 ;
          }

     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int MyPostMessage( HWND hwnd )
{
    static int count = 0;
    if ( count < 5 )
    {
         PostMessage( hwnd, WM_PAINT, (WORD)0, (DWORD)0);
         PostMessage( hwnd, 0x0040, (WORD)0, (DWORD)0);
    }
    return count++;
}


int WINAPI mycallbackfcn()
{
   int x;

START:
   x=1;
   x=x+3;
   return x;
END:;
};
