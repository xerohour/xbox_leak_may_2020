/*DOC***
**
** Suite : bvt
** Test  : dllappx.c
**
** Purpose : test Codeview stepping model
**
** General : stepping
** Primary : redirect
** Secondy : file comparison
**
** Dependencies : none (no screen dumps)
**
** Products : CV410
**
** Revision History :
**
** Mxxx dd-Mon-yy       email name
** - description
** M000 28-Jun-92       waltcr
** - created
**
**
**
**DOC***/

#include <windows.h>   // required for all Windows applications
#include "dllapp.h"   // specific to this program
#include "dec.h"
#include "cvtest.h"

HINSTANCE hInst;        /* current instance */

char szAppName[] = "DllApp";
char szTitle[]   = "DllApp"; // The title bar text
int gi[5] = {0, 0, 0, 0, 0}; // some data for testing


/***
** for testing
*/

WORD   wValue = 10;
HANDLE hGlobalMem = NULL;
LPSTR  lpstr;

/***
**
**      FUNCTION: WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
**
**      PURPOSE: calls initialization function, processes message loop
**
**      COMMENTS:
**
**              Windows recognizes this function by name as the initial entry point
**              for the program.  This function calls the application initialization
**              routine, if no other instance of the program is running, and always
**              calls the instance initialization routine.      It then executes a message
**              retrieval and dispatch loop that is the top-level control structure
**              for the remainder of execution.  The loop is terminated when a WM_QUIT
**              message is received, at which time this function exits the application
**              instance by returning the value passed by PostQuitMessage().
**
**              If this function must abort before entering the message loop, it
**              returns the conventional value NULL.
**
*/

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG msg;        /* message */

	if (!hPrevInstance)             /* Other instances of app running? */
		if (!InitApplication(hInstance))        /* Initialize shared things */
			return (FALSE);         /* Exits if unable to initialize */

	/* Perform initializations that apply to a specific instance */
	if (!InitInstance(hInstance, nCmdShow))
		return (FALSE);

	/* Acquire and dispatch messages until a WM_QUIT message is received. */
	while (GetMessage(&msg,         /* message structure */
		NULL,   /* handle of window receiving the message */
		0,   /* lowest message to examine    */
		0))  /* highest message to examine   */
	{
		TranslateMessage(&msg);         /* Translates virtual key codes           */
		DispatchMessage(&msg);          /* Dispatches message to window           */
	}
	return (msg.wParam);            /* Returns the value from PostQuitMessage */
}


/***
**
**      FUNCTION: InitApplication(HANDLE)
**
**      PURPOSE: Initializes window data and registers window class
**
**      COMMENTS:
**
**              This function is called at initialization time only if no other
**              instances of the application are running.  This function performs
**              initialization tasks that can be done once for any number of running
**              instances.
**
**              In this case, we initialize a window class by filling out a data
**              structure of type WNDCLASS and calling the Windows RegisterClass()
**              function. Since all instances of this application use the same window
**              class, we only need to do this when the first instance is initialized.
**
**
*/

BOOL InitApplication(HINSTANCE hInst) {
	WNDCLASS  wc;

	/* Fill in window class structure with parameters that describe the
		main window. */

	wc.style = 0;                                /* Class style(s) */
	wc.lpfnWndProc = MainWndProc;   /* Func to retrieve msgs for  */
									/* windows of this class */
	wc.cbClsExtra = 0;                              /* No per-class extra data */
	wc.cbWndExtra = 0;                              /* No per-window extra data */
	wc.hInstance = hInst;               /* Application that owns the class */
	wc.hIcon = LoadIcon(hInst, szAppName); // Icon name from .rc
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName =  szAppName;   /* Name of menu resource in .RC file. */
	wc.lpszClassName = szAppName;   /* Name used in call to CreateWindow. */

	/* Register the window class and return success/failure code. */
	return (RegisterClass(&wc));
}

/***
**
**      FUNCTION:  InitInstance(HANDLE, int)
**
**      PURPOSE:  Saves instance handle and creates main window
**
**      COMMENTS:
**
**              This function is called at initialization time for every instance of
**              this application.  This function performs initialization tasks that
**              cannot be shared by multiple instances.
**
**              In this case, we save the instance handle in a static variable and
**              create and display the main program window.
**        
*/

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	HWND    hWnd;   /* Main window handle */

	/* Save the instance handle in static variable, which will be used in
	   many subsequent calls from this application to Windows.      */

	hInst = hInstance;

	/* Create a main window for this application instance.  */

	hWnd = CreateWindow(
		szAppName,      /* See RegisterClass() call */
		szTitle,        /* Text for window title bar */
		WS_OVERLAPPEDWINDOW,    /* Window style */
		CW_USEDEFAULT,  /* Default horizontal position */
		CW_USEDEFAULT,  /* Default vertical position */
		CW_USEDEFAULT,  /* Default width */
		CW_USEDEFAULT,  /* Default height */
		NULL,   /* Overlapped windows have no parent */
		NULL,   /* Use the window class menu */
		hInstance,      /* This instance owns this window */
		NULL    /* Pointer not needed */
    );

	/* If window could not be created, return "failure" */

	if (!hWnd)
		return (FALSE);

	/* Make the window invisible; update its client area; and return "success" */

	ShowWindow(hWnd, SW_HIDE);             /* Show the window */
	UpdateWindow(hWnd);     /* Sends WM_PAINT message */
	SendMessage(hWnd, WM_NULL, 0, 0);
	return (TRUE);  /* Returns the value from PostQuitMessage */
}

/***
**
**      FUNCTION: MainWndProc(HWND, UINT, UINT, LONG)
**
**      PURPOSE:  Processes messages
**
**      MESSAGES:
**
**              WM_COMMAND        - application menu (About dialog box)
**              WM_DESTROY        - destroy window
**
**      COMMENTS:
**
**              To process the IDM_ABOUT message, call MakeProcInstance() to get the
**              current instance address of the About() function.  Then call Dialog
**              box which will create the box according to the information in your
**              DllApp.rc file and turn control over to the About() function.   When
**              it returns, free the intance address.
**
*/

LONG APIENTRY MainWndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam) {
	FARPROC lpProcAbout;              /* pointer to the "About" function */

    HDC         hdc ;
    PAINTSTRUCT ps ;
	RECT            rect ;

	SAVELABEL( CALLDLLATSTART );
	SAVELABEL( CALLDLLATTERM );
	SAVELABEL( TagEndPaint );

	switch (message)
	{
		case WM_COMMAND:        /* message: command from application menu */
			switch( wParam )
			{
				case IDM_ABOUT:
					lpProcAbout = MakeProcInstance(About, hInst);
	
					DialogBox(hInst,                 /* current instance             */
						"AboutBox",                      /* resource to use              */
						hWnd,                    /* parent handle                */
						lpProcAbout);            /* About() instance address */
		
					FreeProcInstance(lpProcAbout);
					break;
	
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
			}
			break;

		case WM_NULL:
TagEndPaint:return 0 ;
			break;	

		case WM_PAINT:
			hdc = BeginPaint (hWnd, &ps) ;
			GetClientRect (hWnd, &rect) ;
			DrawText (hdc, "DLL Application", -1, &rect,
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			EndPaint (hWnd, &ps) ;
			return 0;
			break;

		case WM_CREATE:

			// a func call for testing purposes
CALLDLLATSTART:
			Dec( &wValue );

			/*
			** some arbitrary code for testing purposes
			*/
			if ( hGlobalMem = GlobalAlloc( GMEM_MOVEABLE, 100L ) )
			{
				if ( lpstr = GlobalLock( hGlobalMem ) )
				{
					lstrcpy( lpstr, "The Walt-Meister" );
					GlobalUnlock( hGlobalMem );
					// TODO: check return code?
					//
					//  You can display the contents of the string with the 
					//  following sequence of codeview commands:
					// 
					//  >wgh hGlobalMem
					//  0192:6E30
					//  >? *(char far*) 0x0192:0x6E30,s
				}
			}


			BPWithLengthTest();

			break;
	
		case WM_DESTROY:                  /* message: window being destroyed */
			// a func call for testing purposes
CALLDLLATTERM:
			Dec( &wValue );
			if (hGlobalMem)
				GlobalFree(hGlobalMem);
				// TODO: check return code ?
			PostQuitMessage(0);
			break;

		default:                          /* Passes it on if unproccessed        */
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

/***
**
**      FUNCTION: About(HWND, UINT, UINT, LONG)
**
**      PURPOSE:  Processes messages for "About" dialog box
**
**      MESSAGES:
**
**              WM_INITDIALOG - initialize dialog box
**              WM_COMMAND        - Input received
**
**      COMMENTS:
**
**              No initialization is needed for this particular dialog box, but TRUE
**              must be returned to Windows.
**
**              Wait for user to click on "Ok" button, then close the dialog box.
**
*/

BOOL APIENTRY About(HWND hDlg, UINT message, UINT wParam, LONG lParam) {
	switch (message)
	{
		case WM_INITDIALOG:             /* message: initialize dialog box */
		{
			/*
			** some arbitrary code for testing
			*/
			HANDLE hLocalMem = NULL;
			int i;
			char *   pMem;
			char *   pTmp;
			if ( hLocalMem = LocalAlloc( LMEM_MOVEABLE, 100 ) );
			{
				if (pMem = LocalLock( hLocalMem ));
				{
					/* now locked, so mucked with it */
					pTmp = pMem;
					for (i=0; i<20; i++)
					{
						*pTmp = 'w';
						++pTmp;
					}
					LocalUnlock( hLocalMem );
					// TODO: check return code ?
					}
			}
		 
			//  Now, after setting a breakpoint immediately after the call to LocalLock,
			//  the following command displays the array location:
			// 
			//  >dw pnArray
			// 
			//  Outside of this fragment, though, you cannot rely on the value of the
			//  pnArray variable since the actual data in the memory object may move.
			//  Therefore, use the following sequence to display the correct array
			//  location:
			//  >wlh hLocalMem 
			//  0192:100A
			//  dw 0192:100A

			if ( hLocalMem )
				LocalFree( hLocalMem );  //TODO: check return code ?

			CenterWindow (hDlg, GetWindow (hDlg, GW_OWNER));
			return (TRUE);
			break;
		}

		case WM_COMMAND:        /* message: received a command */
			if (wParam == IDOK || wParam == IDCANCEL)
			{
				EndDialog(hDlg, TRUE);  /* Exits the dialog box */
				return (TRUE);
			}
			break;
	}
	return (FALSE);         /* Didn't process a message */
}

/***
**
**      FUNCTION: CenterWindow (HWND, HWND)
**
**      PURPOSE:  Center one window over another
**
**      COMMENTS:
**
**      Used to center the "About" box over application window
**      Dialog boxes take on the screen position that they were designed at,
**      which is not always appropriate. Centering the dialog over a particular
**      window usually results in a better position.
**
**      Used to center the "About" box over application window.
**
*/

BOOL CenterWindow (HWND hwndChild, HWND hwndParent)
{
	RECT    rChild, rParent;
	int     wChild, hChild, wParent, hParent;
	int     wScreen, hScreen, xNew, yNew;
	HDC     hdc;

	// Get the Height and Width of the child window
	GetWindowRect (hwndChild, &rChild);
	wChild = rChild.right - rChild.left;
	hChild = rChild.bottom - rChild.top;

	// Get the Height and Width of the parent window
	GetWindowRect (hwndParent, &rParent);
	wParent = rParent.right - rParent.left;
	hParent = rParent.bottom - rParent.top;

	// Get the display limits
	hdc = GetDC (hwndChild);
	wScreen = GetDeviceCaps (hdc, HORZRES);
	hScreen = GetDeviceCaps (hdc, VERTRES);
	ReleaseDC (hwndChild, hdc);

	// Calculate new X position, then adjust for screen
	xNew = rParent.left + ((wParent - wChild) /2);
	if (xNew < 0) {
		xNew = 0;
	} else if ((xNew+wChild) > wScreen) {
		xNew = wScreen - wChild;
	}

	// Calculate new Y position, then adjust for screen
	yNew = rParent.top  + ((hParent - hChild) /2);
	if (yNew < 0) {
		yNew = 0;
	} else if ((yNew+hChild) > hScreen) {
		yNew = hScreen - hChild;
	}

	// Set it, and return
	return SetWindowPos (hwndChild, NULL,
		xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


void BPWithLengthTest(void)
{
	START:  
	gi[4]=99;
	gi[2]=99;  // breakpoint should fire for this line/instruction, but stop on the next line/instruction
	BREAK:     // tagged with the BREAK label
	gi[2]=0; gi[3]=99;  
	END:
	return;
}
