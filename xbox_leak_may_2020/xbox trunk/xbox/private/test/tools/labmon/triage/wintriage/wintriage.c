#include "wintriage.h"

VOID ErrorHandler(TCHAR *szCall, 
				  DWORD dwErrorCode)
{
	TCHAR szErrString[ERROR_STRING_LENGTH];

	_stprintf(szErrString, TEXT("%s Failed with %d\n"), szCall, dwErrorCode);

	MessageBox(NULL, szErrString, TEXT("Fatal Error!"), MB_OK|MB_ICONERROR);
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccel;

	ghInstance = hInstance;

    if (!(ghwndMain = CreateDialog(ghInstance, 
                       MAKEINTRESOURCE(100), 
                       NULL, 
                       TriageDlgProc)))
	{
		ErrorHandler(TEXT("CreateDialog"), GetLastError());
		return GetLastError();
	}

    // Load the accelerator table resource
	if (!(hAccel = LoadAccelerators(ghInstance, 
							MAKEINTRESOURCE(IDR_ACCELERATOR))))
	{
		ErrorHandler(TEXT("LoadAccelerators"), GetLastError());
		return GetLastError();
	}

    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if ((ghwndMain == 0) ||(!IsDialogMessage(ghwndMain, &msg)))
        {
            if (!TranslateAccelerator(ghwndMain, hAccel, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
	
    return (int) msg.wParam;
}

INT_PTR CALLBACK TriageDlgProc(HWND hWnd,  // handle to dialog box
                               UINT uMsg,     // message
                               WPARAM wParam, // first message parameter
                               LPARAM lParam)  // second message parameter
{
	switch(uMsg)
	{
       	case WM_CLOSE:
			TriageCleanup();
			DestroyWindow(hWnd);	// Generates a WM_DESTROY
			return FALSE;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return FALSE;
			break;

        case WM_INITDIALOG:
            SendMessage(GetDlgItem(hWnd, ID_NOSYM), BM_SETCHECK, 1, 0);
            return FALSE;
            break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case ID_EXIT:
			case ID_FILE_EXIT:
				SendMessage(hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
				return TRUE;
				break;
			
			case ID_SENDMAIL:
			case ID_MAIL_SENDFAILUREMAIL:
				{
					PTCHAR pszFollowup = NULL;
					PTCHAR pszTitle = NULL;
                    PTCHAR pszBody = NULL;
					DWORD dwFollowup = 0, dwTitle = 0, dwBody = 0;

					// Get the followup string                          
					if (dwFollowup = GetWindowTextLength(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT)))
					{
						if (!(pszFollowup = malloc((dwFollowup + 1) * sizeof(TCHAR))))
						{
							MessageBox(ghwndMain, 
										TEXT("Malloc Failed to Allocate Memory for Followup!"),
										TEXT("Can Not Send Mail!"), 
										MB_OK|MB_ICONEXCLAMATION);
							return TRUE;
						}

						GetWindowText(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT),
										pszFollowup,
										dwFollowup + 1);
					
						if (_tcsstr(_tcslwr(pszFollowup), TEXT("machine owner")))
						{
							free(pszFollowup);
							MessageBox(ghwndMain, 
										TEXT("Can't send mail to Machine Owner!"),
										TEXT("Can Not Send Mail!"), 
										MB_OK|MB_ICONEXCLAMATION);
							return TRUE;
						}

					}
					else
					{
							MessageBox(ghwndMain, 
										TEXT("There are no followups specified!"),
										TEXT("Can Not Send Mail!"), 
										MB_OK|MB_ICONEXCLAMATION);
							return TRUE;
					}

					// Get the body string
					if ((dwBody = GetWindowTextLength(GetDlgItem(hWnd, IDC_STACK_EDIT))))
					{
						dwBody += (_tcslen(HEADER) + 2); // 2 /n/n
						dwBody += (_tcslen(TEXT("Remote /c ")) + 3);  //space between names and 2 \n
						dwBody += GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT));
						dwBody += GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT));
						dwBody += (_tcslen(TEXT("\nFollowup: ")) + 2);  //space between names and 2 \n
						dwBody += GetWindowTextLength(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT));

						if (!(pszBody = malloc((dwBody + 1) * sizeof(TCHAR))))
						{
							MessageBox(ghwndMain, 
										TEXT("Malloc Failed to Allocate Memory for Stack Trace!"),
										TEXT("Can Not Send Mail!"), 
										MB_OK|MB_ICONEXCLAMATION);
							if (pszFollowup)
								free(pszFollowup);
							return TRUE;
						}

						_tcscpy(pszBody, HEADER);
						_tcscat(pszBody, TEXT("Remote /c "));

						GetWindowText(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT),
										pszBody + _tcslen(pszBody),
										dwBody + 1 - _tcslen(pszBody));
						
						_tcscat(pszBody, TEXT(" "));

						GetWindowText(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT),
										pszBody + _tcslen(pszBody),
										dwBody + 1 - _tcslen(pszBody));

						_tcscat(pszBody, TEXT("\n\n"));

						GetWindowText(GetDlgItem(hWnd, IDC_STACK_EDIT),
										pszBody + _tcslen(pszBody),
										dwBody + 1 - _tcslen(pszBody));

						_tcscat(pszBody, TEXT("\nFollowup: "));

						GetWindowText(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT),
										pszBody + _tcslen(pszBody),
										dwBody + 1 - _tcslen(pszBody));

						_tcscat(pszBody, TEXT("\n\n"));

					}
					
					// Get the title string
					if ((dwTitle = GetWindowTextLength(GetDlgItem(hWnd, IDC_TITLE_EDIT))))
					{
						dwTitle += _tcslen(TEXT("WINTRIAGE:"));
						if (!(pszTitle = malloc((dwTitle + 1) * sizeof(TCHAR))))
						{
							MessageBox(ghwndMain, 
										TEXT("Malloc Failed to Allocate Memory for Title!"),
										TEXT("Can Not Send Mail!"), 
										MB_OK|MB_ICONEXCLAMATION);
							if (pszFollowup)
								free(pszFollowup);
							if (pszBody)
								free(pszBody);
							return TRUE;
						}

						_tcscpy(pszTitle, TEXT("WINTRIAGE:"));
						GetWindowText(GetDlgItem(hWnd, IDC_TITLE_EDIT),
										pszTitle + _tcslen(pszTitle),
										dwTitle + 1 - _tcslen(pszTitle));
					}

					SendOffFailure(hWnd, pszFollowup, pszTitle, pszBody);
					if (pszFollowup)
						free(pszFollowup);
					if (pszBody)
						free(pszBody);
					if (pszTitle)
						free(pszTitle);
					return TRUE;
				}
				break;

			case ID_REMOTE:
				DoRemote(hWnd);
				return TRUE;
				break;
						
			case ID_TRIAGE:
				{
					HANDLE hThread = INVALID_HANDLE_VALUE;
					DWORD dwTID = 0;

					//Make sure debugger and debuggee are specifed
					if (!(GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT))))
					{
						MessageBox(ghwndMain, 
									TEXT("You Must Supply a Debugger Name"),
									TEXT("Can Not Triage!"), 
									MB_OK|MB_ICONEXCLAMATION);

						SetFocus(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT));
						return TRUE;
					}

					if (!(GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT))))
					{
						MessageBox(ghwndMain, 
									TEXT("You Must Supply a Debuggee Name"),
									TEXT("Can Not Triage!"), 
									MB_OK|MB_ICONEXCLAMATION);
						SetFocus(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT));
						return TRUE;					}

					SetWindowText(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT), TEXT(""));
					SetWindowText(GetDlgItem(hWnd, IDC_TITLE_EDIT), TEXT(""));
					SetWindowText(GetDlgItem(hWnd, IDC_STACK_EDIT), TEXT(""));

					hThread = CreateThread(NULL,
											0,
											(LPTHREAD_START_ROUTINE)DoTriage,
											(LPVOID)hWnd,
											0,
											&dwTID);

					if (hThread == INVALID_HANDLE_VALUE)
					{
						ErrorHandler(TEXT("Triage Command - CreateThread"), 
									GetLastError());
					}
					else
						CloseHandle(hThread);
					
					return TRUE;
					break;
				}
			case IDC_DEBUGGER_EDIT:
				switch (HIWORD(wParam))
				{
				case EN_CHANGE:
					SetWindowText(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT), TEXT(""));
					SetWindowText(GetDlgItem(hWnd, IDC_TITLE_EDIT), TEXT(""));
					SetWindowText(GetDlgItem(hWnd, IDC_STACK_EDIT), TEXT(""));
					EnableWindow(GetDlgItem(hWnd, ID_SENDMAIL), FALSE);
                    EnableMenuItem(GetMenu(hWnd), ID_MAIL_SENDFAILUREMAIL, MF_GRAYED);

					if ((GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT)))&&
							(GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT))))
					{
						EnableWindow(GetDlgItem(hWnd, ID_TRIAGE), TRUE);
						EnableWindow(GetDlgItem(hWnd, ID_REMOTE), TRUE);
					}
					else
					{
						EnableWindow(GetDlgItem(hWnd, ID_TRIAGE), FALSE);
						EnableWindow(GetDlgItem(hWnd, ID_REMOTE), FALSE);
					}
					return TRUE;
					break;

				}
				break;

			case IDC_DEBUGGEE_EDIT:
				switch (HIWORD(wParam))
				{
				case EN_CHANGE:	
					SetWindowText(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT), TEXT(""));
					SetWindowText(GetDlgItem(hWnd, IDC_TITLE_EDIT), TEXT(""));
					SetWindowText(GetDlgItem(hWnd, IDC_STACK_EDIT), TEXT(""));
					EnableWindow(GetDlgItem(hWnd, ID_SENDMAIL), FALSE);
                    EnableMenuItem(GetMenu(hWnd), ID_MAIL_SENDFAILUREMAIL, MF_GRAYED);
					
                    if ((GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT)))&&
							(GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT))))
					{
						EnableWindow(GetDlgItem(hWnd, ID_TRIAGE), TRUE);
						EnableWindow(GetDlgItem(hWnd, ID_REMOTE), TRUE);
					}
					else
					{
						EnableWindow(GetDlgItem(hWnd, ID_TRIAGE), FALSE);
						EnableWindow(GetDlgItem(hWnd, ID_REMOTE), FALSE);
					}
					return TRUE;
					break;

				}
				break;

			}
			break;
	}
    return FALSE;
}


DWORD WINAPI DoTriage(LPVOID lpParameter)
{
	DWORD	dwDebuggerLength = 0;
	DWORD	dwDebuggeeLength = 0;
	PTCHAR	pszDebugger = NULL;
	PTCHAR	pszDebuggee = NULL;
	PTCHAR	TextBuffer = NULL;
	DWORD	dwFlags = 0;
	HWND	hWnd = (HWND) lpParameter;

	//Get the String Lengths
	(INT) dwDebuggerLength = GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT));
	(INT) dwDebuggeeLength = GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT));

	// Allocate the memory
	if (!(pszDebugger = malloc((dwDebuggerLength + 1) * sizeof(TCHAR))))
	{
		MessageBox(ghwndMain, TEXT("Malloc Failed to Allocate Memory!"),
				TEXT("Can Not Triage!"), MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if (!(pszDebuggee = malloc((dwDebuggeeLength + 1) * sizeof(TCHAR))))
	{
		MessageBox(ghwndMain, TEXT("Malloc Failed to Allocate Memory!"),
				TEXT("Can Not Triage!"), MB_OK|MB_ICONEXCLAMATION);
		free(pszDebugger);
		return FALSE;
	}

	// Make sure the buttons are disabled
	EnableWindow(GetDlgItem(hWnd, ID_TRIAGE), FALSE);
	EnableWindow(GetDlgItem(hWnd, ID_SENDMAIL), FALSE);

	// Do we need to disable symbol fixing
	if (SendMessage(GetDlgItem(hWnd, ID_NOSYM),
					BM_GETSTATE,
					(WPARAM)0,
					(LPARAM)0) == BST_UNCHECKED)
		dwFlags = dwFlags | FLAG_NO_SYMBOLS;

	// Get the actual strings
	GetWindowText(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT),
				pszDebugger,
				dwDebuggerLength + 1);


	GetWindowText(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT),
				pszDebuggee,
				dwDebuggeeLength + 1);

	// Triage the failure
	TextBuffer = Triage(pszDebugger,
						pszDebuggee,
						dwFlags, 
						NULL);

	SetBoxesFromTextBuffer(lpParameter, TextBuffer);
	
	EnableWindow(GetDlgItem(hWnd, ID_SENDMAIL), TRUE);
	EnableMenuItem(GetMenu(hWnd), ID_MAIL_SENDFAILUREMAIL, MF_ENABLED);
	EnableWindow(GetDlgItem(hWnd, ID_TRIAGE), TRUE);

	TriageCleanup();
	free(pszDebugger);
	free(pszDebuggee);

	return TRUE;
}

BOOL SetBoxesFromTextBuffer(HWND hWnd, PTCHAR TextBuffer)
{
	PTCHAR ptr1 = NULL, ptr2 = NULL;
	DWORD BufferLen = (_tcslen(TextBuffer) + 1);

	TCHAR *temp = malloc((_tcslen(TextBuffer) + 1) * sizeof(TCHAR));

	ptr1 = _tcsstr(TextBuffer, TEXT("FOLLOWUP:"));

	if (ptr1)
	{
		// Clean up the crap in the followup string
		while ((ptr2 = _tcschr(ptr1, TEXT('\r')))||
				(ptr2 = _tcschr(ptr1, TEXT('\n')))||
				(ptr2 = _tcschr(ptr1, TEXT('\t'))))
		{
			ptr2[0] = TEXT('\0');
		}

		ptr2 = ptr1 + 1 + _tcslen(TEXT("FOLLOWUP:"));

		SetWindowText(GetDlgItem(hWnd, IDC_FOLLOWUP_EDIT),
						ptr2);
		ptr1[0] = TEXT('\0');
		
		// Clean any trailing crap while we are here
		while ((*(ptr1 - 1) == TEXT('\r'))||
				(*(ptr1 - 1) == TEXT('\n'))||
				(*(ptr1 - 1) == TEXT('\t')))
		{
			*(ptr1 - 1) = TEXT('\0');
			ptr1--;
		}
	}

	ptr1 = _tcschr(TextBuffer, TEXT('\n'));
	if (ptr1)
	{
		ptr1[0] = TEXT('\0');
	}

	SetWindowText(GetDlgItem(hWnd, IDC_TITLE_EDIT),
				TextBuffer);


	if (ptr1)
	{
		while ((*(ptr1 + 1) == TEXT('\r'))||
			(*(ptr1 + 1) == TEXT('\n'))||
			(*(ptr1 + 1) == TEXT('\t')))
			ptr1++;

		ptr1++;
		_tcscpy(TextBuffer, ptr1);

		// Royal Hack
		BufferLen -= (_tcslen(TextBuffer) + 1);
		_tcscpy(temp, TextBuffer);
		TextBuffer[0] = TEXT('\0');
		ptr2 = temp;

		ptr1 = _tcschr(ptr2, TEXT('\n'));

		while ((ptr1)&&(BufferLen > 1)&&(temp))
		{
			ptr1[0] = TEXT('\0');
			_tcscat(TextBuffer, ptr2);
			_tcscat(TextBuffer, TEXT("\r\n"));

			ptr2 = ptr1 + 1;
			ptr1 = _tcschr(ptr2, TEXT('\n'));
			BufferLen--;
		}
		_tcscat(TextBuffer, ptr2);
		_tcscat(TextBuffer, TEXT("\r\n"));

		SetWindowText(GetDlgItem(hWnd, IDC_STACK_EDIT),
						TextBuffer);
	}
	
	if (temp)
		free(temp);

	return TRUE;
}


BOOL DoRemote(HWND hWnd)
{
	DWORD	dwDebuggerLength = 0;
	DWORD	dwDebuggeeLength = 0;
	PTCHAR	pszDebugger = NULL;
	PTCHAR	pszDebuggee = NULL;
	TCHAR	szRemoteLines[10];

	//Get the String Lengths
	if (!((INT) dwDebuggerLength = GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT))))
	{
		MessageBox(ghwndMain, TEXT("You Must Supply a Debugger Name"),
				TEXT("Can Not Remote!"), MB_OK|MB_ICONEXCLAMATION);
		SetFocus(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT));
		return FALSE;
	}

	if (!((INT) dwDebuggeeLength = GetWindowTextLength(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT))))
	{
		MessageBox(ghwndMain, TEXT("You Must Supply a Debuggee Name"),
				TEXT("Can Not Remote!"), MB_OK|MB_ICONEXCLAMATION);
		SetFocus(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT));
		return FALSE;
	}

	// Allocate the memory
	if (!(pszDebugger = malloc((dwDebuggerLength + 1) * sizeof(TCHAR))))
	{
		MessageBox(ghwndMain, TEXT("Malloc Failed to Allocate Memory!"),
				TEXT("Can Not Remote!"), MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if (!(pszDebuggee = malloc((dwDebuggeeLength + 1) * sizeof(TCHAR))))
	{
		MessageBox(ghwndMain, TEXT("Malloc Failed to Allocate Memory!"),
				TEXT("Can Not Remote!"), MB_OK|MB_ICONEXCLAMATION);
		free(pszDebugger);
		return FALSE;
	}

	// Get the actual strings
	GetWindowText(GetDlgItem(hWnd, IDC_DEBUGGER_EDIT),
				pszDebugger,
				dwDebuggerLength + 1);


	GetWindowText(GetDlgItem(hWnd, IDC_DEBUGGEE_EDIT),
				pszDebuggee,
				dwDebuggeeLength + 1);

	//Eventually this will come from the registry.
	_stprintf(szRemoteLines, TEXT("%u"), DEFAULT_REMOTE_LINES);

	// Spawn the remote process
	_tspawnlp(_P_NOWAIT, TEXT("cmd"), TEXT("/K"), 
		TEXT("remote"), TEXT("/C"), pszDebugger, pszDebuggee, TEXT("/L"), szRemoteLines, NULL);

	free(pszDebugger);
	free(pszDebuggee);

	return TRUE;
}
