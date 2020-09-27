/*===========================================================================*\
 | 
 |  FILE:	DoImport.cpp
 |			Skeleton project and code for a Scene Importer 
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 29-3-99
 | 
\*===========================================================================*/

#include "SceneImport.h"



/*===========================================================================*\
 |  Preferences dialog handler
\*===========================================================================*/

static BOOL CALLBACK PrefsDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SkeletonImporter *si = (SkeletonImporter*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!si && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) {
		case WM_INITDIALOG:	
			// Update class pointer
			si = (SkeletonImporter*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);

			// Setup preferences initial state
			SetCheckBox(hWnd,IDC_SIMPLE,si->simpleParam);
			break;

		case WM_DESTROY:
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CANCEL:
					EndDialog(hWnd,1);
					break;

				case IDC_OK:
					// Retrieve preferences
					si->simpleParam = GetCheckBox(hWnd,IDC_SIMPLE);

					EndDialog(hWnd,0);
				break;
			}
			break;

		default:
			return FALSE;
		}	
	return TRUE;
} 



/*===========================================================================*\
 |  Mini file viewer
\*===========================================================================*/

static DWORD CALLBACK
stream_callback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	if (ReadFile((HANDLE)dwCookie, pbBuff, (DWORD)cb, (DWORD*)pcb, NULL))
		return 0;
	else
		return GetLastError();
}
static DWORD CALLBACK
stream_out_callback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb)
{
	if (WriteFile((HANDLE)dwCookie, pbBuff, cb, (DWORD*)pcb, NULL ))
		return 0;
	else
		return GetLastError();
}

static BOOL CALLBACK ViewerDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SkeletonImporter *si = (SkeletonImporter*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!si && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) {
		case WM_INITDIALOG:	
			// Update class pointer
			si = (SkeletonImporter*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);

			// Open the imported file and read it into our richedit control
			HANDLE	   file;
			EDITSTREAM es;
			if ((file = CreateFile(si->filenameTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
			{
				es.dwCookie = (DWORD)file;
				es.pfnCallback = stream_callback;
				es.dwError = 0;
 				int foo = SendMessage(GetDlgItem(hWnd,IDC_FILEVIEW), EM_STREAMIN, (WPARAM)SF_TEXT, (LPARAM)&es);
				CloseHandle(file);
			}	

			// Simple parameter state - put it to the window title
			char tmp[MAX_PATH];
			sprintf(tmp, "Importer's Simple Parameter State: %s", si->simpleParam?"TRUE":"FALSE");
			SetWindowText(hWnd,tmp);

			break;


		// resize the richedit control to fit the dialog
		case WM_SIZE:			
			MoveWindow(GetDlgItem(hWnd,IDC_FILEVIEW),0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
			break;

		case WM_DESTROY:
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
				case IDCANCEL:
					EndDialog(hWnd,1);
					break;
			}
			break;

		default:
			return FALSE;
		}	
	return TRUE;
} 


/*===========================================================================*\
 |  Read in information from a file
 |  We will use a simple viewing dialog to see the text in the file
\*===========================================================================*/

int	SkeletonImporter::DoImport(const TCHAR *name, ImpInterface *ii,Interface *i, BOOL suppressPrompts)
{
	// Set local interface pointer
	ip = i;

	// load the configuration from disk
	// so that it can be used in our dialog box
	if(!LoadImporterConfig()) return 0;

	if(!suppressPrompts)
	{

		// Show preferences setup dialog
		int res = DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDD_SKELETON_SCIMP),
			i->GetMAXHWnd(),
			PrefsDlgProc,
			(LPARAM)this);

		// User clicked 'Cancel'
		if(res!=0) return 0;

	}


	// put the filename we're given into storage, and open the viewer dialog
	// the viewer will do all the reading of the file itself
	filenameTemp = name;

	DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_VIEW),
		i->GetMAXHWnd(),
		ViewerDlgProc,
		(LPARAM)this);


	// Save the current configuration back out to disk
	// for use next time the importer is run
	SaveImporterConfig();


	return 1;
}
