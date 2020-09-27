#include <windows.h>

#define IDM_EXIT  100

EXTERN_C void dbg_main();
EXTERN_C void Types_Tests(int);
EXTERN_C int main_Calls();

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
int MyPostMessage( HWND hwnd );

#if !defined( SAVELABEL )
#define SAVELABEL( l )	if (0) goto l
#endif

char szGExeBuffer[128];

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                         LPSTR lpszCmdLine, int nCmdShow)
{

     static char szAppName[] = "ecwinapp" ;
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
     int iCalls, nLoop = 10;

	 SAVELABEL(TagCalldbg_main);
	 SAVELABEL(TagCallmain_Calls);
	 SAVELABEL(TagCallTypes_Tests);
	 SAVELABEL(TagCreateCase);

     switch (message)
          {
          case WM_CREATE:
TagCreateCase: hInst= ((LPCREATESTRUCT) lParam)->hInstance;

               ////////////// MAKE TESTING CALLS HERE  /////////


TagCalldbg_main:	dbg_main();

TagCallmain_Calls:	iCalls = main_Calls();

TagCallTypes_Tests:	Types_Tests(nLoop);

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





