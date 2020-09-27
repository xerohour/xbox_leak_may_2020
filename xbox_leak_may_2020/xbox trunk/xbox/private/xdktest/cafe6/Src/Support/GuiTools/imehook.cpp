///////////////////////////////////////////////////////////////////////////////
//	IMEHOOK.CPP
//
//	Created by :			Date :
//		MichMa					1/1/97
//
//	Description :
//		Implementation of the NT/J IME subclassing hook.
//

/***	OVERVIEW	***

this file (built as imehook.dll) is used by cafe to control the ide's ime on nt/j.
since the ime gets instantiated on a per-process basis on nt/j, cafe needs a way
to access it out-of-process. the method we chose was to hook the ide and subclass it
via imehook.dll, and then communicate with that dll via private messages. the
procedure works as follows:

1.	in cafe, the CIDE object gets created. if it is running on nt/j, CIDE::CIDE()
	loads imehook.dll via LoadLibrary() and initializes pointers to 2 functions via
	GetProcAddress(): HookIDE() and	UnhookIDE().

2. 	when imehook.dll is loaded into cafe's address space, DllMain() registers the
	following messages while processing DLL_PROCESS_ATTACH:
	
		msgSubclassIDE
		msgDeSubclassIDE
		msgIMESetOpen
		msgIMEGetOpen
		msgIMESetMode
		msgIMEGetMode

	it also preserves the HINSTANCE in a global variable.

3.	in cafe, the CIME object gets created. if it is running on nt/j, CIME::CIME()
	registers the same messages as imehook.dll's DllMain(), except for
	msgSubclassIDE and msgDeSubclassIDE, which are only used by HookIDE() and
	UnhookIDE(). the other messages are used by CIME routines to communicate
	IME operations to the subclassed IDE's window procedure.

4.	CIDE::Launch() calls HookIDE() after the IDE has come up, passing to it the
	handle to the IDE's main window.

5.	HookIDE() sets a WH_GETMESSAGE hook via SetWindowsHookEx, using the global
	HINSTANCE of imehook.dll preserved by DllMain(), and the IDE's main thread id
	derived from the window handle passed to HookIDE(). the hook procedure is
	CafeIMEHookProc(). the handle to the hook is preserved in a global variable for
	later use by UnhookIDE(). imehook.dll is now loaded into the IDE's address space. 
	DllMain() is called and it does the same registering of messages and preserving
	of its HINSTANCE as was done in cafe's address space.

6.	HookIDE() then posts the msgSubclassIDE message, registered by DllMain(), to the
	IDE. it must post the message rather than send it because WH_GETMESSAGE hooks
	only get called when the hooked app asynchronously processes messages with 
	GetMessage(). the lParam of this message is the hook's handle, returned from
	SetWindowsHookEx.

7.	CafeIMEHookProc() receives the msgSubclassIDE message and uses SetWindowLong to
	subclass the IDE, installing CafeIMEWndProc() as the new window procedure and
	preserving the original window procedure in a global variable for restoration
	later. we need to subclass the window because we want cafe to be able to
	synchronously control the IDE's IME via SendMessage(). a WH_GETMESSAGE hook
	only allows asynchronous communication via GetMessage(). and asynchronous
	communication within cafe can lead to timing problems.

8.	remember that the lParam of the msgSubclassIDE message was set to the hook's
	handle before being posted by HookIDE(). CafeIMEHookProc() saves this value in
	a global variable since it needs it as a parameter to CallNextHookEx(), which
	it calls for every message it receives. remember also that the handle to the hook
	was already preserved in a global variable when the hook was created in HookIDE().
	but that was in cafe's address space, so the global variable in the IDE's address
	space is uninitialized until this point.

9.	with the subclass in place, the CIME routines used for controlling the IDE's IME
	can now communicate IME operations to CafeIMEWndProc by sending messages registered
	in imehook.dll's DllMain() in cafe's address space. CafeIMEWndProc, which is also
	in imehook.dll but used only in the IDE's address space and not cafe's, responds to
	the messages with one or more of the appropriate Imm function calls. all of the
	operations require getting and releasing an IME context via ImmGetContext() and
	ImmReleaseContext(). other functions used are:
	
	Imm function				CafeIMEWndProc() msg		CIME function
	------------				--------------------		-------------
	ImmSetOpenStatus()			msgIMESetOpen				CIME::Open()
	ImmGetOpenStatus()			msgIMEGetOpen				CIME::IsOpen()
	ImmSetConversionMode()		msgIMESetMode				CIME::SetMode()
	ImmGetConversionMode()		msgIMEGetMode				CIME::GetMode()

10.	CIDE::Exit() calls UnhookIDE() before terminating the IDE, passing to it the
	handle to the IDE's main window.

11. UnhookIDE() sends the msgDeSubclassIDE message to CafeIMEWndProc(), which processes
	this message by using SetWindowLong() to restore the IDE's original window procedure,
	which it preserved in a global variable when doing the original subclassing in 
	CafeIMEHookProc().

12. UnhookIDE() then removes the hook via UnhookWindowsHookEx(), passing to it the hook
	handle which was preserved in a global variable when the hook was set in HookIDE().
	this unloads imehook.dll from the IDE's address space.

13. CIDE::~CIDE() unloads imehook.dll from cafe's address space.

*/

#include <windows.h>
#include <stdio.h>		// file i/o for errors.
#include <imm.h>		// standard header file for Imm functions.

// all errors that occur in imehook.dll are written to imehook.log.
FILE *fileLog;
// the handle to the ide hook is first initialized within cafe's 
// address space when it calls HookIDE() from CIDE::Launch() then it is
// immediately communicated to the imehook.dll in the IDE's address 
// space via the lParam of a message posted to the new hook.
HHOOK hCafeIMEHook;
// handle to imehook.dll, initialized in DllMain().
HINSTANCE hIMEHookDll;
// address of the IDE's original window procedure, saved in the hook 
// procedure (CafeIMEHookProc) before subclassing the ide.
LONG IDEWndProc;

// the following messages are used to communicate from cafe and the
// imehook.dll loaded into cafe's address space to the imehook.dll
// loaded into the ide's address space.	they are registered with
// RegisterWindowMessage in DllMain() while processing DLL_PROCESS_ATTACH.

		// posted from HookIDE() informing the hook to subclass the ide.
UINT	msgSubclassIDE,
		// sent from UnhookIDE() informing the subclass window procedure
		// to restore the ide's default window procedure.
		msgDeSubclassIDE,
		// sent from CIME::Open().
		msgIMESetOpen,
		// sent from CIME::IsOpen().
		msgIMEGetOpen,
		// sent from CIME::SetMode().
		msgIMESetMode,
		// sent from CIME::GetMode().
		msgIMEGetMode;


// this function is the subclass window procedure that gets installed for the IDE, via SetWindowProc,
// when the _WH_GETMESSAGE hook procedure (CafeIMEHookProc) is processing the msgSubclassIDE message.
// this window procedure receives all messages sent or posted to the IDE's main window. we only process
// the private messages intended to communicate IME operations from cafe. the rest we pass on to the
// IDE's default window procedure, via CallWindowProc.
LPARAM CALLBACK CafeIMEWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	
	{
	// sent from CIME::Open().
	if(msg == msgIMESetOpen)
		{
		// get the IME context for the IDE's main window.
		if(HIMC hIMC = ImmGetContext(hwnd))
			{
			// set the open status of the IME.
			// if lParam is TRUE, the IME is opened.
			// if lParam is FALSE, the IME is closed.
			if(ImmSetOpenStatus(hIMC, lParam))
				{
				// release the IME context for the IDE's main window.
				if(ImmReleaseContext(hwnd, hIMC))
					// CIME::Open() returns TRUE if everything passed.
					return TRUE;
				else
					{
					// failed to release the IME context.
					fprintf(fileLog, "IMEHOOK.DLL: error %d releasing IME context "
									 "while processing msgIMESetOpen.", GetLastError());
					return FALSE;
					}
				}
			else
				{
				// failed to set the open status.
				fprintf(fileLog, "IMEHOOK.DLL: error %d setting IME open status "
								 "while processing msgIMESetOpen.", GetLastError());
				return FALSE;
				}
			}
		else
			{
			// failed to get the IME context.
			fprintf(fileLog, "IMEHOOK.DLL: error %d getting IME context "
							 "while processing msgIMESetOpen.", GetLastError());
			return FALSE;
			}
		}

	// sent from CIME::IsOpen().
	else if(msg == msgIMEGetOpen)
		{
		// get the IME context for the IDE's main window.
		if(HIMC hIMC = ImmGetContext(hwnd))
			{
			// get the open status of the IME.
			BOOL bOpenStatus = ImmGetOpenStatus(hIMC);
			// release the IME context for the IDE's main window.
			if(ImmReleaseContext(hwnd, hIMC))
				// CIME::IsOpen() returns one of the following:
				//		 0 if the IME is closed.
  				//		-1 if the IME is open, with no bytes in the buffer.
				//		the number of bytes in the buffer. 
				// REIVEW (michma): third option not currently supported here.
				return bOpenStatus ? -1 : 0;
			else
				{
				// failed to release the IME context. report IME as closed.				
				fprintf(fileLog, "IMEHOOK.DLL: error %d releasing IME context "
								 "while processing msgIMEGetOpen.", GetLastError());
				return 0;
				}
			}
		else
			{
			// failed to get the IME context. report IME as closed.
			fprintf(fileLog, "IMEHOOK.DLL: error %d getting IME context "
							 "while processing msgIMEGetOpen.", GetLastError());
			return 0;
			}
		}

	// sent from CIME::SetMode().
	else if(msg == msgIMESetMode)
		{
		// get the IME context for the IDE's main window.
		if(HIMC hIMC = ImmGetContext(hwnd))
			{
			DWORD dwConversion, dwSentence;
			// setting the conversion mode requires setting the sentence too.
			// we don't want to screw up the sentence, so first we get the current
			// conversion mode and sentence...
			if(ImmGetConversionStatus(hIMC, &dwConversion, &dwSentence))
				{
				// ...then we reset the conversion mode to that passed in by CIME::SetMode().
				dwConversion = (DWORD)lParam;
				// and set the new conversion mode using the old sentence.
				if(ImmSetConversionStatus(hIMC, dwConversion, dwSentence))
					{
					// release the IME context for the IDE's main window.
					if(ImmReleaseContext(hwnd, hIMC))
						// CIME::SetMode() returns the conversion mode actually set.
						return dwConversion;
					else
						{
						// failed to release the IME context. return invalid conversion mode.
						fprintf(fileLog, "IMEHOOK.DLL: error %d releasing IME context "
										 "while processing msgIMESetMode.", GetLastError());
						return 0;
						}
					}
				else
					{
					// failed to set the conversion status. return invalid conversion mode.
					fprintf(fileLog, "IMEHOOK.DLL: error %d setting IME conversion status "
									 "while processing msgIMESetMode.", GetLastError());
					return 0;
					}
				}
			else
				{
				// failed to get the conversion status. return invalid conversion mode. 
				fprintf(fileLog, "IMEHOOK.DLL: error %d getting IME conversion status "
								 "while processing msgIMESetMode.", GetLastError());
				return 0;
				}
			}
		else
			{
			// failed to get the IME context. return invalid conversion mode.
			fprintf(fileLog, "IMEHOOK.DLL: error %d getting IME context "
							 "while processing msgIMESetMode.", GetLastError());
			return 0;
			}
		}

	// sent from CIME::GetMode().
	else if(msg == msgIMEGetMode)
		{
		// get the IME context for the IDE's main window.
		if(HIMC hIMC = ImmGetContext(hwnd))
			{
			DWORD dwConversion, dwSentence;
			// get the conversoin mode (and sentence, which we don't use).
			if(ImmGetConversionStatus(hIMC, &dwConversion, &dwSentence))
				{
				// release the IME context for the IDE's main window.
				if(ImmReleaseContext(hwnd, hIMC))
					// CIME::GetMode() returns the conversion mode.
					return dwConversion;
				else
					{
					// failed to release the IME context.
					fprintf(fileLog, "IMEHOOK.DLL: error %d releasing IME context "
									 "while processing msgIMEGetMode.", GetLastError());
					return 0;
					}
				}
			else
				{
				// failed to get the conversion mode.
				fprintf(fileLog, "IMEHOOK.DLL: error %d getting IME conversion status "
								 "while processing msgIMEGetMode.", GetLastError());
				return 0;
				}
			}
		else
			{
			// failed to get the IME context.
			fprintf(fileLog, "IMEHOOK.DLL: error %d getting IME context "
							 "while processing msgIMEGetMode.", GetLastError());
			return 0;
			}
		}
	
	// sent from UnHookIDE().		
	else if(msg == msgDeSubclassIDE)
		{
		// restore the IDE's original window procedure, which was saved in a global
		// variable while processing msgSubclassIDE in CafeIMEHookProc().
		if(!SetWindowLong(hwnd, GWL_WNDPROC, IDEWndProc))
			fprintf(fileLog, "IMEHOOK.DLL: error %d setting window long "
							 "while processing msgDeSubclassIDE.", GetLastError());

		return 0;
		}

	// if the message isn't from CIME or UnhookIDE(), 
	// pass it on to the IDE's original window procedure.
	return CallWindowProc((int (__stdcall *)(void))IDEWndProc, hwnd, msg, wParam, lParam);
	}


// this function is the WH_GETMESSAGE hook procedure that HookIDE() installs.
// hooking the ide gets imehook.dll loaded into the ide's address space
// so we can access its ime in-process. all this hook does is wait for
// the message from HookIDE to subclass the ide (so CIME can communicate
// synchronously with the ide's imehook.dll).
LRESULT CALLBACK CafeIMEHookProc(int code, WPARAM wParam, LPARAM lParam)
	
	{
	// posted from HookIDE().
	if(((MSG *)lParam)->message == msgSubclassIDE)
		{
		// the hook handle was passed from HookIDE so that the ide's
		// instance of imehook.dll could use it for CallNextHookEx.
		hCafeIMEHook = (HHOOK)(((MSG *)lParam)->lParam);
		// subclass the ide with CafeIMEWndProc while saving the default
		// window procedure for restoration later.
		if(!(IDEWndProc = SetWindowLong(((MSG *)lParam)->hwnd, GWL_WNDPROC, (long)CafeIMEWndProc)))
			fprintf(fileLog, "IMEHOOK.DLL: error %d setting window long "
							 "while processing msgSubclassIDE.", GetLastError());
 		}

	// call the next hook in the chain (if any).
	return CallNextHookEx(hCafeIMEHook, code, wParam, lParam);
	}


// to allow cafe to use non-mangled names when calling
// GetProcAddress() for HookIDE() and UnhookIDE().
extern "C"
{

// this function sets the GetMessage hook in the ide. it then posts a
// user-defined message to that hook informing the hook to subclass the ide.
// we can't simply use a GetMessage hook because then the CIME class would
// only be able to communicate with the ide's imehook.dll asynchronously
// (via PostMessage). for synchronous communication we need SendMessage,
// which requires a subclass.

// hWndIDE is the handle to the ide's main window, passed from CIDE::Launch.
__declspec(dllexport) void HookIDE(HWND hWndIDE)
	{
	// get the thread associated with the ide's main window (required by SetWindowsHookEx).
	if(DWORD idThreadIDE = GetWindowThreadProcessId(hWndIDE, NULL))
		{
		// set the GetMessage hook.
		if(hCafeIMEHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)CafeIMEHookProc, hIMEHookDll, idThreadIDE))
			{
			// post a message to the hook informing it to subclass the ide.
			// we send the handle to the hook via lParam because the hook
			// procedure needs to call CallNextHookEx, which requires the handle
			// of the current hook. even though hCafeIMEHook is a global
			// variable, HookIDE is called in cafe's address space, while the
			// the hook procedure is called in the ide's address space.
			if(!PostMessage(hWndIDE, msgSubclassIDE, 0, (LPARAM)hCafeIMEHook))
				{
				fprintf(fileLog, "IMEHOOK.DLL: error %d posting msgSubclassIDE.", GetLastError());
				}
			}
		else
			{
			fprintf(fileLog, "IMEHOOK.DLL: error %d setting ide hook.", GetLastError());
			}
		}
	else
		fprintf(fileLog, "IMEHOOK.DLL: error %d getting ide thread id.", GetLastError());
	}


// this function removes the GetMessage hook from the ide.
// first it sends a message to the subclass window procedure informing
// it to re-instate the default window procedure (saved in IDEWndProc).

// hWndIDE is the handle to the ide's main window, passed from CIDE::Exit.
__declspec(dllexport) void UnhookIDE(HWND hWndIDE)
	{
	// verify that the hook was created.
	if(hCafeIMEHook)
		{
		// tell the subclass procedure to restore the default window procedure.
		SendMessage(hWndIDE, msgDeSubclassIDE, 0, 0);
		// remove the hook from the ide. this will unload imehook.dll from
		// the ide's address space.
		if(!UnhookWindowsHookEx(hCafeIMEHook))
			fprintf(fileLog, "IMEHOOK.DLL: error %d removing hook.", GetLastError());
		}
	}

}	// extern "C"


// remember that this will be called both for the imehook.dll loaded into
// cafe's address space and for the imehook.dll loaded into the ide's
// address space.
INT WINAPI DllMain(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved) 
	
	{
	if(ulRbc == DLL_PROCESS_ATTACH)
		{
		// the imehook.dll loaded into cafe's address just needs the
		// the first two messages. the imehook.dll loaded into the ide's
		// address space needs them all.
		msgSubclassIDE = RegisterWindowMessage("msgSubclassIDE");
		msgDeSubclassIDE = RegisterWindowMessage("msgDeSubclassIDE");
		msgIMESetOpen = RegisterWindowMessage("msgIMESetOpen");
		msgIMEGetOpen = RegisterWindowMessage("msgIMEGetOpen");
		msgIMESetMode = RegisterWindowMessage("msgIMESetMode");
		msgIMEGetMode = RegisterWindowMessage("msgIMEGetMode");
		// the imehook.dll loaded into cafe's address space needs the dll's
		// handle for setting the hook in HookIDE().
		hIMEHookDll = hInst;
		// open the log file, imehook.log, for any errors that may occur.
		// for cafe's instance of imehook.dll, this file will be created in the same dir as cafedrv.exe.
		// for the ide's instance of imehook.dll, this file will be created in the same dir as msdev(d).exe.
		fileLog = fopen("imehook.log", "w");
		// we want all logging written to disk immediately (in case of crashes), so don't use a buffer.
		setvbuf( fileLog, NULL, _IONBF, 0);
		}
	else if(ulRbc == DLL_PROCESS_DETACH)
		// close imehook.log.
		fclose(fileLog);
	
	return TRUE;
	}
