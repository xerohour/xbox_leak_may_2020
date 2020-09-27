///////////////////////////////////////////////////////////////////////////////
//  TESTUTIL.CPP
//
//  Created by :            Date :
//      DavidGa                 8/20/93
//
//	Description :
//		Implementation of global utility functions
//

#include "stdafx.h"

#include <stdarg.h>
#include <stdio.h>
#include <fstream.h>
#include <windows.h>					    

#include "testutil.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

//
//  Process Control
//

/* MilliSleep
 *  Sleeps for X milliseconds
 */
// BEGIN_HELP_COMMENT
// Function: void MilliSleep( DWORD milliseconds )
// Description: Sleep for the number of milliseconds specified. This function is obsolete; use ::Sleep() instead.
// Return: <none>
// Param: milliseconds A DWORD that specifies the number of milliseconds to sleep.
// END_HELP_COMMENT
GUI_API void MilliSleep( DWORD milliseconds )
{
/*  DWORD dwSleepUntil = GetTickCount() + milliseconds;
    while( GetTickCount() < dwSleepUntil );*/
	::Sleep(milliseconds);
}

// BEGIN_HELP_COMMENT
// Function: CString ExtractHotKey(CString)
// Description: Given a string, extracts the hotkeys and returns them as one string.
// Return: A CString containing the hotkeys.
// END_HELP_COMMENT
GUI_API CString ExtractHotKey(CString Caption)
{
	CString HotKeys;
	for(CString mnem; mnem != Caption; )
	{
	   mnem = Caption.SpanExcluding("&");
	   if (mnem != Caption) // If there was a hotkey in the menu string
	   {
		HotKeys += Caption.Mid(mnem.GetLength()+1,1);
		Caption = Caption.Mid(mnem.GetLength()+1) ; // Start the search past the previous &
	   }
	}
	return HotKeys;
}
//
//  Clipboard
//



// BEGIN_HELP_COMMENT
// Function: BOOL EmptyClip(void) 
// Description: Clear the text on the system clipboard. This functions is obsolete; use COClip::Empty() instead.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
GUI_API BOOL EmptyClip( /*BTHROW bThrowFail /*= DO_THROW*/ ) 
{
	if ( !OpenClipboard( NULL ) ) 
//		EXPECT_EXEC( bThrowFail, "Cannot open clipboard to empty contents" );
		LOG->RecordInfo("Cannot open clipboard to empty contents");
	else if ( !EmptyClipboard() )
	{
		CloseClipboard();
//		EXPECT_EXEC( bThrowFail, "Cannot empty clipboard" );
		LOG->RecordInfo("Cannot empty clipboard");
	}
	else
	{
		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL GetClipText( CString& rstrTextOut)
// Description: Get the text from the system clipboard. This functions is obsolete; use COClip::GetText() instead.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: A CString reference that will contain the text from the system clipboard.
// END_HELP_COMMENT
GUI_API BOOL GetClipText( CString& rstrTextOut/*, BTHROW bThrowFail /*= DO_THROW*/ )
{
	int tries = 3;

	while (--tries && !OpenClipboard( NULL )) Sleep(2000);
	if ( !tries )
//		EXPECT_EXEC( bThrowFail, "Cannot open clipboard to get text" );
		LOG->RecordInfo("Cannot open clipboard to get text");
	else
	{
		HANDLE hText = GetClipboardData( CF_TEXT );
		if ( !hText )
		{
			CloseClipboard();
//			EXPECT_EXEC( bThrowFail, "Cannot get handle to clipboard text" );
			LOG->RecordInfo("Cannot get handle to clipboard text");
		}
		else
		{
			LPSTR lpText = (LPSTR)GlobalLock( hText );
			if ( !lpText )
			{
				CloseClipboard();
//				EXPECT_EXEC( bThrowFail, "Cannot lock handle for clipboard text" );
				LOG->RecordInfo("Cannot lock handle for clipboard text");
			}
			else
			{
				rstrTextOut = lpText;
				GlobalUnlock( hText );
				CloseClipboard();
				return TRUE;
			}
		}   
	}
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL SetClipText( LPCSTR szTextIn)
// Description: Set the text in the system clipboard. This functions is obsolete; use COClip::SetText() instead.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szTextIn A pointer to a string containing the text to put in the clipboard.
// END_HELP_COMMENT
GUI_API BOOL SetClipText( LPCSTR szTextIn/*, BTHROW bThrowFail /*= DO_THROW*/ )
{

// Don't delete the following EmptyClip call.

		if ( !EmptyClip() )
				return FALSE;

// The precedinging EmptyClip call is currently (4-20-94) needed because for some hideous reason CTL3D32.DLL will
// not pick up the true clipboard text if we just set the clipboard without emptying it first.  -CFlaat


    if ( !OpenClipboard( NULL ) )
    {
//        EXPECT_EXEC( bThrowFail, "Could not open clipboard to set text" );
		LOG->RecordInfo("Could not open clipboard to set text");
        return FALSE;
    }

    
    HANDLE hText = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, 1+strlen(szTextIn) );


    if ( !hText )
    {
        CloseClipboard();
//        EXPECT_EXEC( bThrowFail, "Could not allocate memory for clipboard text" );
		LOG->RecordInfo("Could not allocate memory for clipboard text");
        return FALSE;
    }
    

    LPSTR lpText = (LPSTR)GlobalLock( hText );

    if ( !lpText )
    {
        CloseClipboard();
        GlobalFree(hText);
//        EXPECT_EXEC( bThrowFail, "Could not lock handle for clipboard text" );
		LOG->RecordInfo("Could not lock handle for clipboard text");
        return FALSE;
    }

    strcpy( lpText, szTextIn);

    GlobalUnlock( hText );
    
    HANDLE hData = SetClipboardData( CF_TEXT, hText );

    CloseClipboard();

    if (!hData)
    {
//        EXPECT_EXEC( bThrowFail, "Could not set clipboard data" );
		LOG->RecordInfo("Could not set clipboard data");
        return FALSE;
    }

 
    return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: void PasteKeys(LPCSTR szPasteText)
// Description: Paste the given string into the control with the focus.
// Param: szPasteText A pointer to a string that contains the text to paste.
// END_HELP_COMMENT
GUI_API void PasteKeys(LPCSTR szPasteText/*, BTHROW bThrowFail /*= DO_THROW*/)
{
  if (SetClipText(szPasteText/*, bThrowFail*/))
    MST.DoKeys("^v");
    else
    {
//      EXPECT_EXEC(bThrowFail, "Can't set clipboard text in PasteKeys!");
	  LOG->RecordInfo("Can't set clipboard text in PasteKeys!");
        Sleep(500); // maybe it was a synchronization problem
        MST.DoKeys(szPasteText);  // try to stuff 'em in anyway
    }
}

/*	DoKeysSlow
 *		Uses QueKeys rather than DoKeys to put a delay between each character.
 *		Used because speed bugs found by autotests aren't realistic.
 */
// BEGIN_HELP_COMMENT
// Function: void DoKeysSlow(LPCSTR szKeys, WORD wMilliseconds = 0, BOOL bLiteral = FALSE)
// Description: Perform DoKeys at a slower rate.
// Param: szKeys Pointer to a null-terminated string containing the DoKeys-formatted keystrokes.
// Param: wMilliseconds The number of milliseconds to wait between keystrokes. (Default value is 0.)
// Param: bLiteral TRUE to interpret the keystrokes literally; FALSE to interpret the keystrokes as DoKeys keystrokes. (Default value is FALSE.)
// END_HELP_COMMENT
GUI_API void DoKeysSlow(LPCSTR szKeys, WORD wMilliseconds /*=0*/, BOOL bLiteral /*=FALSE*/)
{
	if( !bLiteral )
	{
		WaitStepInstructions("Do Keys '%s'", szKeys);
		MST.QueSetSpeed(wMilliseconds);
		MST.QueKeys((LPSTR)szKeys);
		MST.QueFlush();
		MST.QueSetSpeed(0);		// reset to no delay
		return;
	}
	CString str;
	while( *szKeys != '\0' )	// this routine taken from MST.DoKeys.  Maybe we need a helper function.
	{						
		if( strchr("+^%(){}~", *szKeys) == NULL )
		{
			str += *szKeys++;
			if( IsDBCSLeadByte(*szKeys) )
				str += *szKeys++;
		}
		else
		{
			str += "{";			// enclose every control character in {}
			str += *szKeys++;	// so MS-Test to treats them literally
			str += "}";
		}					// NOTE: \x escape codes still work
	}
	WaitStepInstructions("Do Keys '%s'", str);
	MST.QueSetSpeed(wMilliseconds);
	MST.QueKeys(str);
	MST.QueFlush();
	MST.QueSetSpeed(0);
	return;
}


/*  ClickMouse    (Enriquep)
       Takes a set of coordinates and a handle to a Window
       and clicks either of the mouse buttons there       */

//REVIEW: Overload function to take a CPoint 
// BEGIN_HELP_COMMENT
// Function: void ClickMouse(int nButton, HWND hwnd, int cX,  int cY) 
// Description: Click the mouse at the given coordinates (relative to the window specified).
// Return: <none>
// Param: nButton An integer that specifies the button to click: VK_LBUTTON, VK_MBUTTON, VK_RBUTTON.
// Param: hwnd An HWND that specifies the window to click in.
// Param: cX An integer that specifies the x-coordinate of the click.
// Param: cY An integer that specifies the y-coordinate of the click.
// END_HELP_COMMENT
GUI_API void ClickMouse( int nButton, HWND hwnd, int cX,  int cY) 
{
	CPoint ptClick(cX, cY);
	if (hwnd != NULL)
		ClientToScreen(hwnd, &ptClick);

	WaitStepInstructions("Click mouse at %d,%d", ptClick.x, ptClick.y);

	// SPEED 10   *if NT
	MST.QueMouseClick(nButton, ptClick);
	MST.QueFlush(TRUE);   
}



/*  XDragMouse       (Enriquep)
 *      Takes two sets of coordinates (including a handle to a window)
 *      and drags from point1 to point2
 */
// REVIEW: Overload to remove the second CWnd*
// BEGIN_HELP_COMMENT
// Function: void DragMouse(int nButton, HWND hwnd1, int cX1, int cY1, HWND hwnd2, int cX2, int cY2)
// Description: Drag the mouse from the given coordinates to the given coordinates (relative to the window specified).
// Return: <none>
// Param: nButton An integer that specifies the button to click: VK_LBUTTON, VK_MBUTTON, VK_RBUTTON.
// Param: hwnd1 An HWND that specifies the window to start dragging from.
// Param: cX1 An integer that specifies the x-coordinate of the beginning drag position.
// Param: cY1 An integer that specifies the y-coordinate of the beginning drag position.
// Param: hwnd2 An HWND that specifies the window the end dragging in.
// Param: cX2 An integer that specifies the x-coordinate of the ending drag position.
// Param: cY2 An integer that specifies the y-coordinate of the ending drag position.
// END_HELP_COMMENT
GUI_API void DragMouse ( int nButton, HWND hwnd1, int cX1, int cY1, HWND hwnd2, int cX2, int cY2)
{
	// Mouse down at point1    

	CPoint ptFrom(cX1, cY1);
	ASSERT(hwnd1 != NULL);
		ClientToScreen(hwnd1, &ptFrom);

	CPoint ptTo(cX2, cY2);
	ASSERT(hwnd2 != NULL);
		ClientToScreen(hwnd2, &ptTo);

	WaitStepInstructions("Drag mouse from %d,%d to %d,%d", ptFrom.x, ptFrom.y, ptTo.x, ptTo.y);

	MST.QueMouseDn(nButton, ptFrom);
	//  Drag.  Mouse up at X2,Y2 on dialog2 (possibly the same dialog)
	// SPEED 10         if NT
	MST.QueMouseMove(ptTo);
	MST.QueMouseUp(nButton,ptTo);

	QueFlush(TRUE);
}

/*  ScreenToClient and ClientToScreen
 *      are provided because these MFC functions are widely used,
 *      and would be missed if UWindow restricts use of MFC.
 */
// BEGIN_HELP_COMMENT
// Function: BOOL ScreenToClient(HWND hwnd, CRect* pRect)
// Description: Converts the coordinates in the given rectangle from screen coordinates to client coordinates for the specified window.
// Return: A Boolean value that indicates success. TRUE if success; FALSE otherwise.
// Param: hwnd An HWND that specifies the window to convert the screen coordinates to client coordinates.
// Param: pRect A pointer to a CRect that contains the rectangle to convert. The converted coordinates will be stored in this object when the function returns.
// END_HELP_COMMENT
GUI_API BOOL ScreenToClient(HWND hwnd, CRect* pRect)
{
	return ScreenToClient(hwnd, (LPPOINT)pRect) &&      // top-left
			ScreenToClient(hwnd, ((LPPOINT)pRect+1));   // bottom-right
}

// BEGIN_HELP_COMMENT
// Function: BOOL ClientToScreen(HWND hwnd, CRect* pRect)
// Description: Converts the coordinates in the given rectangle from client coordinates for the specified window to screen coordinates.
// Return: A Boolean value that indicates success. TRUE if success; FALSE otherwise.
// Param: hwnd An HWND that specifies the window that contains the client coordinates.
// Param: pRect A pointer to a CRect that contains the rectangle to convert. The converted coordinates will be stored in this object when the function returns.
// END_HELP_COMMENT
GUI_API BOOL ClientToScreen(HWND hwnd, CRect* pRect)
{
	return ClientToScreen(hwnd, (LPPOINT)pRect) &&      // top-left
			ClientToScreen(hwnd, ((LPPOINT)pRect+1));   // bottom-right
}

/*  CompareFiles    (Enriquep)
 *      Compares two files given two path strings
 *      returns CF_SAME (0) if identical, 
 *      or an error code describing the difference
 */
// BEGIN_HELP_COMMENT
// Function: int CompareFiles(LPCSTR strSource, LPCSTR strTarget, int nStart)
// Description: Compare the contents of two files.
// Return: A value that specifies the result of the comparison: CF_SAME, CF_DUPNAME, CF_CANTOPEN1, CF_CANTOPEN2, CF_DIFFSIZE, CF_NOTSAME.
// Param: strSource A pointer to a string that contains the path and filename of the first file to compare.
// Param: strTarget A pointer to a string that contains the path and filename of the second file to compare.
// Param: nStart An integer that contains the offset from the beginning of the files to start the comparison.
// END_HELP_COMMENT
GUI_API int CompareFiles(LPCSTR strSource, LPCSTR strTarget, int nStart)
{
    // Error if the file names are the same
    if ( lstrcmp( strSource, strTarget ) == 0 )
        return CF_DUPNAME;

    // Open both the files
    HFILE hSource, hTarget;
    if ( (hSource = _lopen( strSource, OF_READ )) == -1 )
        return CF_CANTOPEN1;

    if ( (hTarget = _lopen( strTarget, OF_READ )) == -1 )
    {
        _lclose( hSource );
        return CF_CANTOPEN2;
    }

    // Check and see if the files are the same size
    LONG SourceSize = _llseek( hSource, 0L, SEEK_END );
    LONG TargetSize = _llseek( hTarget, 0L, SEEK_END );
    if (SourceSize != TargetSize)
    {
        _lclose( hSource );
        _lclose( hTarget );
        return CF_DIFFSIZE;
    }

    // Reset file pointers and compare byte-by-byte
    // Start comparing at "nStart", used since the header 
    // will be different depending on the device...
    if (nStart < 0)
        nStart = 0;
    _llseek( hSource, (LONG)nStart, SEEK_SET );
    _llseek( hTarget, (LONG)nStart, SEEK_SET );

    char iSource[512], iTarget[512];
    int cbIntSize = 512;
    int cbBytesRead;

    do
    {
        cbBytesRead = _lread(hSource, &iSource, cbIntSize);
        cbBytesRead = _lread(hTarget, &iTarget, cbIntSize);

        if (memcmp(&iSource, &iTarget, cbBytesRead) != 0)
        {
            _lclose( hSource );
            _lclose( hTarget );
            return CF_NOTSAME;
        }
    } while (cbBytesRead > (cbIntSize-1));

    /* Close files */
    _lclose( hSource );
    _lclose( hTarget );


    return CF_SAME;
}

/*	KillFile
 *		Deletes all files that meet the pattern provided.
 *		If any of these files cannot be deleted (e.g. directories and read-only
 *		files), KillFile will abort - so don't KillFile("*.*");
 *		If you really do want to delete read-only files, pass TRUE for the 3rd param.
 */
// BEGIN_HELP_COMMENT
// Function: void KillFile(LPCSTR szPathName, LPCSTR szFileName = NULL, BOOL bIgnoreReadOnly = FALSE)
// Description: Delete all files that match the pattern provided in szFileName.
// Param: szPathName A string containing the path to the files.
// Param: szFileName A string containing the filename to delete (wildcards are supported).
// Param: bIgnoreReadOnly TRUE to ignore the read-only bit; FALSE to honor the read-only bit.
// END_HELP_COMMENT
GUI_API void KillFile(LPCSTR szPathName, LPCSTR szFileName /*=NULL*/, BOOL bIgnoreReadOnly /*=FALSE*/)
{
	WIN32_FIND_DATA ffdImgFile;
	HANDLE hFile;
	CString strPattern, strPath = szPathName;
	if( szFileName != NULL )
	{
		if( (strPath.Right(1) != "\\") && (szFileName[0] != '\\') )
			strPath += "\\";
		strPattern = strPath + szFileName; 
	}
	else
		strPattern = strPath;

	while( (hFile = FindFirstFile(strPattern, &ffdImgFile)) != INVALID_HANDLE_VALUE )
	{
		BOOL bCantDelete;
		CString str = strPath + ffdImgFile.cFileName;
		if( !(bCantDelete = DeleteFile(str)) )
		{
			DWORD dwStyle = GetFileAttributes(str);
			if( dwStyle & FILE_ATTRIBUTE_READONLY )
			{
				if( bIgnoreReadOnly )	// try again if test doesn't care about read-only
				{
					LOG->RecordInfo("Attempting to delete read-only file '%s'", (LPCSTR)str);
					SetFileAttributes(str, dwStyle & ~FILE_ATTRIBUTE_READONLY);
					if( !(bCantDelete = DeleteFile(str)) )
					{
						SetFileAttributes(str, dwStyle);	// might as well revert to previous attributes
						LOG->RecordInfo("Still couldn't delete '%s'.  Aborting KillFile function.", (LPCSTR)str);
					}
				}
				else
					LOG->RecordInfo("Unable to delete '%s'.  File was read-only.  Aborting KillFile function.", (LPCSTR)str);
			}
		}
		FindClose(hFile);
		if( !bCantDelete )	// if DeleteFile failed, don't bother continueing, 
			return;			// because FindFirstFile will grab the same file.
	}
}

/* KillAllFiles
 *  The equivalent of calling KillFile(szPathName, "*.*");
 *  except that *.* doesn't work for KillFile, because "." and ".." get found.
 */

// BEGIN_HELP_COMMENT
// Function: void KillAllFiles(LPCSTR szPathName, BOOL bRemoveDir = TRUE)
// Description: Delete an entire directory, recursively.
// Param: szPathName A pointer to a string that contains the path to the directory to be deleted.
// Param: bRemoveDir TRUE to remove the directory itself; FALSE to keep the directory but delete all the files in it.
// END_HELP_COMMENT
GUI_API BOOL KillAllFiles(LPCSTR szPathName, BOOL bRemoveDir /*=TRUE*/)
{
	CStringArray sa;
	WIN32_FIND_DATA ffdImgFile;

	CString strPattern = szPathName;

	if (strPattern.SpanExcluding(" ").IsEmpty())
		return FALSE;

#ifdef _DEBUG
	if (bRemoveDir && !CMDLINE->IsDefined("no_del_warn"))
	{
		CFileStatus status;
		CFile::GetStatus(szPathName, status);
		CString strMsg = CString("About to delete everything starting at ") + status.m_szFullName;

		HWND hwndTarget = GetForegroundWindow();	// Save active target window.
		SetForegroundWindow(g_hwndHotKey);	
		int choice = MessageBox(g_hwndHotKey, strMsg, "CAFE Warning", MB_YESNO | MB_ICONEXCLAMATION);
		SetForegroundWindow(hwndTarget);			// Restore focus to target.
		
		while(GetForegroundWindow() != hwndTarget)	// Wait for full activation!
			;

		if (choice == IDNO)
			return FALSE;
	}

#endif

	if( strPattern.Right(1) != "\\" )
		strPattern += "\\";
	strPattern += "*.*";

	HANDLE hFile = FindFirstFile(strPattern, &ffdImgFile);
	if( hFile != INVALID_HANDLE_VALUE )
		do
		{
			if( !(ffdImgFile.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
			{
				// Modified to handle handle nested subdirectories (Ivanl)
				if(ffdImgFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // if a directory 
				{  CString Name = ffdImgFile.cFileName ;
					if((Name != ".") && (Name != ".."))
					{
				 		CString newPath = szPathName ;
				 		if(newPath.Right(1) != "\\")  //If the pathname didn't have a "\" add one.
				 			newPath += "\\" ;
				 		newPath += ffdImgFile.cFileName ;
						// KillAllFiles recursively
				 		KillAllFiles(newPath);
					}
				}
				if(!(ffdImgFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
					sa.Add(ffdImgFile.cFileName);	// build a list of the rest
			}				// this way I don't disrupt the file finding routines
			
		}
						// by deleting the reference points it is using
		while( FindNextFile(hFile, &ffdImgFile) );
	FindClose(hFile);	

	for( int n = 0; n < sa.GetSize(); n++ )
		KillFile(szPathName, sa.GetAt(n));		// delete all the files we could.

	return bRemoveDir && RemoveDirectory(szPathName);
}

// BEGIN_HELP_COMMENT
// Function: BOOL CopyTree(LPCSTR szFrom, LPCSTR szTo)
// Description: Copy a tree of directories and files from one tree to another.
// Return: FALSE on failure
// Param: szFrom A pointer to a string that contains the path to the source tree (the tree to copy from).
// Param: szTo A pointer to a string that contains the path to the destination tree (the tree to copy to).
// END_HELP_COMMENT
GUI_API BOOL CopyTree(LPCSTR szFrom, LPCSTR szTo)
{
    _TCHAR acFrom[_MAX_PATH];

    lstrcpy(acFrom, szFrom);
    int nFrom = lstrlen(acFrom);
	ASSERT(nFrom);
	_TCHAR* pszFromEnd = &acFrom[nFrom];
	ASSERT(!*pszFromEnd);
	if(_tcsncmp(_tcsdec(acFrom, pszFromEnd), _T("\\"), 1))
	{
		lstrcpy(pszFromEnd, _T("\\"));
		pszFromEnd = _tcsinc(pszFromEnd);
	}
	lstrcat(acFrom, "*.*");

	_TCHAR acTo[_MAX_PATH];
    lstrcpy(acTo, szTo);
    int nTo = lstrlen(acTo);
	ASSERT(nTo);
	_TCHAR* pszToEnd = &acTo[nTo];
	ASSERT(!*pszToEnd);
	if(_tcsncmp(_tcsdec(acTo, pszToEnd), _T("\\"), 1))
	{
		lstrcpy(pszToEnd, _T("\\"));
		pszToEnd = _tcsinc(pszToEnd);
	}

    WIN32_FIND_DATA ffdImgFile;
    HANDLE hFile = FindFirstFile(acFrom, &ffdImgFile);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
            lstrcpy(pszFromEnd, ffdImgFile.cFileName);
            lstrcpy(pszToEnd, ffdImgFile.cFileName);
            if( !(ffdImgFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
				if(!CopyFile(acFrom, acTo, FALSE))      // could add fFailIfExists as optional param
				{
					LOG->RecordInfo("Can't CopyFile() %s to '%s'. Aborting CopyTree function.", (LPCSTR)acFrom, (LPCSTR)acTo);
					return FALSE;
				}
                DWORD dwAttrib = ffdImgFile.dwFileAttributes & ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
                if(!SetFileAttributes(acTo, dwAttrib))
				{
					LOG->RecordInfo("Can't SetFileAttributes() on %s'. Aborting CopyTree function.", (LPCSTR)acTo);
					return FALSE;
				}
            }
            else if( (strcmp(ffdImgFile.cFileName, ".") != 0) &&
                    (strcmp(ffdImgFile.cFileName, "..") != 0) )     // ignore . and ..
            {
                if(!CreateDirectory(acTo, NULL))
				{
					LOG->RecordInfo("Can't CreateDirectory() %s'. Aborting CopyTree function.", (LPCSTR)acTo);
					return FALSE;
				}
				if(!CopyTree(acFrom, acTo))     // recursive call for subdirectories
					return FALSE;
            }
        } while( FindNextFile(hFile, &ffdImgFile) );

        EXPECT( GetLastError() == ERROR_NO_MORE_FILES );
    }
	return TRUE;
}

/*	FindStrInFile
 *		Searches a text file for a string
 *		Returns the line# on which it was found (first line is #1)
 *		or returns 0 if string not found
 */
// BEGIN_HELP_COMMENT
// Function: int FindStrInFile(LPCSTR szFileName, LPCSTR szStr, int nStart /*=1*/)
// Description: Search for the specified string in a file.
// Return: An integer that contains the line number in the file that contains the string. 0 if the string was not found.
// Param: szFileName A pointer to a string that contains the path and filename of the file to search.
// Param: szStr A pointer to the string to find.
// Param: nStart An integer that contains the offset from the beginning of the file to start the search.
// END_HELP_COMMENT
GUI_API int FindStrInFile(LPCSTR szFileName, LPCSTR szStr, int nStart /*=1*/)
{
	ifstream f(szFileName, ios::in | ios::nocreate);
	if( !f.is_open() )
	{
		LOG->RecordInfo("FindStrInFile: Can't open %s", szFileName);
		return 0;
	}

	int nLine = 0;
	char acBuf[512];
	while( f.getline(acBuf, 512, '\n') )
	{
		nLine++;
		if( (nLine >= nStart) && (strstr(acBuf, szStr) != NULL) )
			return nLine;	// return on what line we found the string
	}
	return 0;	// string not found (or it was before line nStart)
}
	
	
/*  GetText
 *      GetWindowText should SendMessage(WM_GETTEXT), but doesn't always.
 *      Therefore, here are two versions of a function that will.
 */
// BEGIN_HELP_COMMENT
// Function: LPCSTR GetText(HWND hwnd, LPSTR sz, int cMax)
// Description: Get the text associated with the specified window by sending the WM_GETTEXT message to the window.
// Return: A pointer to the buffer specified by sz that contains the text associated with the window.
// Param: hwnd An HWND of the window to get the text for.
// Param: sz A pointer to a buffer that will contain the text associated with the window.
// Param: cMax An integer that specifies the maximum size of the buffer sz.
// END_HELP_COMMENT
GUI_API LPCSTR GetText(HWND hwnd, LPSTR sz, int cMax)
{
	if( hwnd == NULL )	// REVIEW(davidga): can SendMessage handle NULL hwnd?
		*sz = '\0';
	else		
		::SendMessage(hwnd, WM_GETTEXT, (WPARAM)cMax, (LPARAM)sz);
	return sz;
}

// BEGIN_HELP_COMMENT
// Function: CString GetText(HWND hwnd)
// Description: Get the text associated with the specified window by sending the WM_GETTEXT message to the window.
// Return: A pointer to a buffer that contains the text associated with the window.
// Param: hwnd An HWND of the window to get the text for.
// END_HELP_COMMENT
GUI_API CString GetText(HWND hwnd)
{
	CString str;
	GetText(hwnd, str.GetBufferSetLength(256), 255);    // call first function
	str.ReleaseBuffer(-1);
	return str;
}

/* Do

 *  Calls DoSingleMenu, passing each of the szMenu parameters in turn.
 *  If we fail to execute any of the steps, we escape out of the menu 
 *  structure, and return an index (base 1) to that step.  Thus, returning 
 *  zero is good, while returning non-zero signifies an error.
 */
// BEGIN_HELP_COMMENT
// Function: int DoMenu(LPCSTR szMenu1, LPCSTR szMenu2, LPCSTR szMenu3 /*=NULL*/, LPCSTR szMenu4 /*=NULL*/, LPCSTR szMenu5 /*=NULL*/, LPCSTR szMenu6 /*=NULL*/, LPCSTR szMenu7 /*=NULL*/, LPCSTR szMenu8 /*=NULL*/, LPCSTR szMenu9 /*=NULL*/)
// Description: Open a series of menus. This function is obsolete; use CMSTest::DoMenu() instead.
// Return: An integer that indicates success. 0 if success; a positive value that contains the menu level that failed otherwise.
// Param: szMenu1 A pointer to a string that contains the name of the menu to open.
// Param: szMenu2 A pointer to a string that contains the name of the menu to open.
// Param: szMenu3 A pointer to a string that contains the name of the menu to open. (Default value is NULL.)
// Param: szMenu4 A pointer to a string that contains the name of the menu to open. (Default value is NULL.)
// Param: szMenu5 A pointer to a string that contains the name of the menu to open. (Default value is NULL.)
// Param: szMenu6 A pointer to a string that contains the name of the menu to open. (Default value is NULL.)
// Param: szMenu7 A pointer to a string that contains the name of the menu to open. (Default value is NULL.)
// Param: szMenu8 A pointer to a string that contains the name of the menu to open. (Default value is NULL.)
// Param: szMenu9 A pointer to a string that contains the name of the menu to open. (Default value is NULL.)
// END_HELP_COMMENT
GUI_API int DoMenu( LPCSTR szMenu1, LPCSTR szMenu2, LPCSTR szMenu3, 
			LPCSTR szMenu4, LPCSTR szMenu5, LPCSTR szMenu6,
			LPCSTR szMenu7, LPCSTR szMenu8, LPCSTR szMenu9 )
{
	LPCSTR aMenu[10];       // arbitrary limit of 9 menu levels
	aMenu[0] = szMenu1;     // params 3-9 default to NULL
	aMenu[1] = szMenu2;     // 2 params are required, to differentiate DoMenu's
	aMenu[2] = szMenu3;
	aMenu[3] = szMenu4;
	aMenu[4] = szMenu5;
	aMenu[5] = szMenu6;
	aMenu[6] = szMenu7;
	aMenu[7] = szMenu8;
	aMenu[8] = szMenu9;
	aMenu[9] = NULL;

	int i = 0;
	LPCSTR szMenu;
	while( (szMenu = aMenu[i++]) != NULL )
	{
		if( !DoSingleMenu(szMenu) )
		{
//          WMenuEnd();     // does "% %" to clear any lingering menu state
			return i;       // return which menu level failed
		}
	}
	return 0;
}

/* DoMenu
 *  Variation of the above DoMenu, which receives all of its menu strings
 *  in a \0 delimited list.
 *  Otherwise, the behavior and return value are the same.
 */
// BEGIN_HELP_COMMENT
// Function: int DoMenu(LPCSTR szMenu)
// Description: Open a series of menus in a NULL-separated list. This function is obsolete; use CMSTest::DoMenu() instead.
// Return: An integer that indicates success. 0 if success; a positive value that contains the menu level that failed otherwise.
// Param: szMenu A pointer to a string that contains the names of the menu to open. The list of menus is NULL-separated and NULL-terminated.
// END_HELP_COMMENT
GUI_API int DoMenu(LPCSTR szMenu)
{
    ASSERT( szMenu != NULL );

    int i = 1;
    LPCSTR pchSingle = szMenu;
    while( *pchSingle != '\0' )
    {
        if( !DoSingleMenu(pchSingle) )
        {
            MST.WMenuEnd();     // does "% %" to clear any lingering menu state
            return i;       // return which menu level failed
        }
        pchSingle += strlen(pchSingle) + 1;
        i++;
    }
    return 0;
}

/* DoSingleMenu
 *  Activates a menu in the current popup.
 *  Before the menu is called, we check if it exists, and if it's enabled.
 */
// BEGIN_HELP_COMMENT
// Function: BOOL DoSingleMenu(LPCSTR szMenu)
// Description: Open a menu. This function is obsolete; use CMSTest::DoMenu() instead.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szMenu A pointer to a string that contains the name of the menu to open.
// END_HELP_COMMENT
GUI_API BOOL DoSingleMenu(LPCSTR szMenu)
{
//LOG->Comment("DoSingleMenu : start %s", szMenu);
    if( !MST.WMenuExists(szMenu) )
    {
        MilliSleep(500);            // sleep 1/2 second
        if( !MST.WMenuExists(szMenu) )  // and try again
        {
            LOG->Comment("'%s' menu doesn't exist", szMenu);
            return FALSE;
        }
    }
//LOG->Comment("DoSingleMenu : WMenuExists %s", szMenu);

    if(!MST.WMenuEnabled(szMenu))
    {
        LOG->Comment("'%s' menu isn't enabled", szMenu);
        return FALSE;
    }
//LOG->Comment("DoSingleMenu : WMenuEnabled %s", szMenu);

    MST.WMenu(szMenu);
//LOG->Comment("DoSingleMenu : WMenu %s", szMenu);

#if 0
	// this is no longer possible for the current command bars code
    if( MST.WMenuExists(szMenu) )   // except for top-level non-popups (and coincidental dups)
    {                           // this is always false if WMenu succeeds
//LOG->Comment("DoSingleMenu : Toplevel failed - sleep .5 sec");
        MilliSleep(500);            // sleep .5 seconds
//LOG->Comment("DoSingleMenu : Toplevel failed - done sleeping");
        if( MST.WMenuExists(szMenu) )   // and if it's still not up
        {
            LOG->Comment("'%s' menu isn't popping up!  Trying one more time.", szMenu);
            MST.WMenu(szMenu);          // try again
                if( MST.WMenuExists(szMenu) )
                {
                    LOG->Comment("Nope, I give up on menu '%s'.", szMenu);
                    return FALSE;   // if it's still not up, fail as disabled
                }
        }
    }   // REVIEW: This presumes that all top level menus are popups
//LOG->Comment("DoSingleMenu : done %s", szMenu);
#endif
    return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL IsWindowActive(LPCSTR szWndTitle)
// Description: Determine if the specified window is the active window.
// Return: A Boolean value that indicates whether the specified window is the active window (TRUE) or not.
// Param: szWndTitle A pointer to a string that contains the title of the window.
// END_HELP_COMMENT
GUI_API BOOL IsWindowActive(LPCSTR szWndTitle)
{
	HWND hwnd = MST.WGetActWnd(0);
	char acBuf[256];
	GetText(hwnd, acBuf, 255);
	if (strcmp(acBuf, szWndTitle) != 0)  
		return FALSE;
	return TRUE;
}

// REVIEW(briancr): this needs to be code reviewed
// BEGIN_HELP_COMMENT
// Function: BOOL WaitMsgBoxText(LPCSTR szExpect, int nSeconds)
// Description: Wait for a specific message to be active.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szExpect A pointer to a string that contains the partial text expected in the message box.
// Param: nSeconds An integer that specifies the number of seconds to wait for the message box to appear.
// END_HELP_COMMENT
GUI_API BOOL WaitMsgBoxText(LPCSTR szExpect, int nSeconds)
{
	// rumor has it that there's a bug in WFndWndWaitC, 
	// so Brian was just looping using WFndWndC

	// REVIEW (michma): in some cases when no msg box comes up this function is making the
	// originally active window inactive and not re-activating it, causing tests to fail.
	HWND hwndActive = MST.WGetActWnd(0);

	// is there any msg/dlg box at all?
	if( MST.WFndWndWait(NULL, FW_DIALOG | FW_PART | FW_NOCASE | FW_FOCUS, nSeconds) )
	{
		HWND hwndText;
		// REVIEW(michma - 6/3/99): we're seeing this weird problem on w2k where a data breakpoint hit
		// message is coming up but the dlg isn't active. doesn't happen manually, only when Go is done
		// through cafe. this is a work-around so we can find the expected static text.
		// is there a second static text field?
		if( (hwndText = MST.WFndWndC("@2", "Static", GetSystem() & SYSTEM_NT_5 ? FW_DEFAULT : FW_ACTIVE)) != NULL )
		{
			// get the text from the second static text field
			CString strActual = GetText(hwndText);
			// is the expected string part of the actual string?
			if( strActual.Find(szExpect) != -1 )
				return TRUE;
		}
	}

	// no msg box was found, so restore focus to original window with focus (see michma REVIEW above).
	MST.WSetActWnd(hwndActive);
	return FALSE;
}


/*	GetLabelNo
 *		Determines the ord number of a class of a control given its hwnd
 */
GUI_API int GetLabelNo(HWND hCtl)
{ // REVIEW(DavidGa): MSVCStaticWindow - not generic!!
static LPCSTR szDialogControlLabels[] = {
	"Static",
	"Button",
	"Edit",
	"ListBox",
	"ComboBox",
	"SysListView32",
	"TabView32",
	"TreeView32",
	"Scrollbar32",
	"Slider32",
	"Spin32",
	"Statusbar32",
	"MSVCStaticWindow",
	"#32770"
}; //if you add/rearrange strings, make sure the public values
//	enum { GL_NONE, GL_STATIC, ...}
//  correspond correctly to the strings

	char acClass[128];
	GetClassName(hCtl, acClass, 128);
	int nlabels=sizeof(szDialogControlLabels)/sizeof(szDialogControlLabels[0]);
	for(int ilab=0;ilab<nlabels;ilab++)
		if(strstr(szDialogControlLabels[ilab],acClass)!=NULL)
			return ilab;
	LOG->RecordInfo("Unknown control '0x%X' class: (%s)", hCtl, acClass);
	return GL_NONE;
}

/*	GetLabel
 *		If the control is a list box, combo box, or edit field, the caption
 *		is the static control immediately preceding the control.
 *		Only the immediate children of the HWND are searched.  If no HWND
 *		is provided, the active popup window (presumably a dialog) is used.
 *		The bOkNoExist flag states whether to WriteWarning if the control isn't found.
 */

// BEGIN_HELP_COMMENT
// Function: LPCSTR GetLabel(UINT id, HWND hwnd /*=NULL*/, BOOL bOkNoExist /*=FALSE*/)
// Description: Get the caption of a control given its ID. If the control is a list box, combo box, or edit box, the caption is the static control immediately preceeding the control. Only the immediate children of the given window are searched.
// Return: A pointer to a string that contains the caption of the control.
// Param: id An integer that specifies the ID of the control.
// Param: hwnd An HWND that specifies the window that contains the control. NULL means use the currently active window. (Default value is NULL.)
// Param: bOkNoExist A Boolean value that specifies whether to log information if the control is not found (FALSE) or not.
// END_HELP_COMMENT
GUI_API LPCSTR GetLabel(UINT id, HWND hwnd /*=NULL*/, BOOL bOkNoExist /*=FALSE*/)
{
	static CString strLabel;	// contains the label name through end of scope
		// but DONT maintain a pointer here - stick it in a CString of your own!

	//HWND hwndDialog = hwnd == NULL ? WGetActWnd(0) : hwnd;  Changed by (Ivan) to handle AppWiz dlgs
	HWND hwndDialog = hwnd == NULL ? ControlOnPropPage(id) : hwnd;
	HWND hwndControl = GetDlgItem(hwndDialog, id);
	if( !IsWindow(hwndControl) )
	{
		if( !bOkNoExist )
			LOG->RecordInfo("Could not find a control 0x%X in dialog 0x%X ('%s')", id, hwndDialog, (LPCSTR)GetText(hwndDialog));
		return NULL;
	}

	char acClass[128];
	GetClassName(hwndControl, acClass, 128);
	int iClass = GetLabelNo(hwndControl);

	switch( iClass )
	{
		case GL_STATIC:
		case GL_BUTTON:
		case GL_MSVCSTATIC:
			strLabel = GetText(hwndControl);
			return strLabel;
		case GL_LISTVIEW:
			strLabel = GetText(hwndControl);
			return strLabel;
		case GL_EDIT:
		case GL_LISTBOX:
		case GL_COMBOBOX:
		{
			HWND hwndLabel = GetWindow(hwndControl, GW_HWNDPREV);
			if( hwndLabel == NULL )
			{
				LOG->RecordInfo("Found 0x%X (%s), but no label precedes it.", hwndControl, acClass);
				return NULL;
			}

			char acLabelClass[128];
			GetClassName(hwndLabel, acLabelClass, 128);
			int iPrec=GetLabelNo(hwndLabel);
			if( (iPrec == GL_STATIC) ||
				(iPrec == GL_MSVCSTATIC) )
			{
				strLabel = GetText(hwndLabel);
				return strLabel;
			}
			else
			{
				LOG->RecordInfo("Found 0x%X (%s), but the preceding control is a '%s', not a static label.", hwndControl, acClass, acLabelClass);
				return NULL;
			}
		}
		case GL_DIALOG:
		case GL_NONE:
		default:
			LOG->RecordInfo("Found the control 0x%X, but I can't figure out what it is (it says it's a '%s').", hwndControl, acClass);
	}
	return NULL;
}



// BEGIN_HELP_COMMENT
// Function: int GetDlgControlClass(HWND hCtl) 
// Description: Get the window class type of the specified control.
// Return: A value that indicates the window class type of the control: GL_NONE, GL_STATIC, GL_BUTTON, GL_EDIT, GL_LISTBOX, GL_COMBOBOX, GL_DIALOG, GL_CHECKBOX, GL_RADIOBUTTON.
// Param: hCtl An HWND that specifies the control.
// END_HELP_COMMENT
GUI_API int GetDlgControlClass(HWND hCtl) 
{
	// Validate the Window handle.
	if( !IsWindow(hCtl) )
	{
		LOG->RecordInfo("Cannot get control class of invalid HWND 0x%08x", hCtl);
		return GL_NONE;
	}

	int iClass = GetLabelNo(hCtl);
	
	if (iClass == GL_BUTTON) // If button 
		{	DWORD dwStyle = GetWindowLong(hCtl,GWL_STYLE) ;
		
			// Determine what kind of button 
			// If checkbutton
			if((dwStyle & BS_3STATE) || (dwStyle & BS_AUTO3STATE) || (dwStyle & BS_AUTOCHECKBOX) || (dwStyle & BS_CHECKBOX))
			{
				return GL_CHECKBOX ;
			}
			// If Radiobutton
			if ((dwStyle & BS_RADIOBUTTON) || (dwStyle & BS_AUTORADIOBUTTON))
			{
				return GL_RADIOBUTTON ;
			}
			// If any other button
			if ((dwStyle & BS_OWNERDRAW) || (dwStyle & BS_DEFPUSHBUTTON) || (dwStyle & BS_PUSHBUTTON))
			{
			 	return iClass ;
			}
		}

	return iClass ;
}


// Tabbed dialog support functions. 

/* 
 *	ControlOnProjPage(int) determines whether a given control identified by its ID exisists on 
 *	the current page. This works for both property paged and tabbed dialog pages.
 *	(Ivanl)
 */

HWND hActive = NULL  ;
// BEGIN_HELP_COMMENT
// Function: HWND ControlOnPropPage(int iCtrlId)
// Description: Find the parent window of the specified control in the active window.
// Return: An HWND that specifies the parent window of the control.
// Param: iCtrlId An integer that contains the ID of the control.
// END_HELP_COMMENT
GUI_API HWND ControlOnPropPage(int iCtrlId)
{	
	HWND hWnd = WGetActWnd(0) ;

	hActive = NULL ;
	if(!(hActive = GetDlgItem(WGetActWnd(0),iCtrlId))) // If option not on active child
	{
	 	EnumChildWindows(hWnd,WGetPageHandle,(LPARAM) iCtrlId) ;
	}
	else // if the child is on the active window 
	hActive = WGetActWnd(0) ;
 	return (hActive) ; 
 }

 /*
 	WGetPageHandle() is a callback function for ControlOnPropPage(). See ControlOnPropPage above for 
	more info. (Ivanl)
 */
GUI_API BOOL CALLBACK WGetPageHandle(HWND hChWnd,LPARAM iCtrlId)	  // Can this be a private member??
{

	if (GetDlgItem(hChWnd,iCtrlId))
	{	
		hActive = hChWnd ;
		return FALSE ;
   	}
	hActive = NULL ;
	return TRUE ;	

}


/*	DayRandSeed and DayRand
 *		DayRandSeed seeds the random number generator with the date.
 *		This way, the random numbers will be consistent for the duration of a day.
 *		DayRand retrieves a random number within a range.
 *		If no range is desired, just call rand()
 */

UINT nDayRandSeed;

// BEGIN_HELP_COMMENT
// Function: void DayRandSeed(void)
// Description: Seed the random number generator.
// Return: <none>
// END_HELP_COMMENT
GUI_API void DayRandSeed(void)
{
	CTime t = CTime::GetCurrentTime();
	nDayRandSeed = (t.GetDay() * 13) + t.GetMonth();
}

// BEGIN_HELP_COMMENT
// Function: int DayRand(int nRange)
// Description: Get a random number.
// Return: An integer that is a random number in the given range.
// Param: nRange An integer that specifies the upper end of the range (the range will be between 0 and nRange-1).
// END_HELP_COMMENT
GUI_API int DayRand(int nRange)
{
	ASSERT( nRange >= 0 );
	nDayRandSeed = (((0xffffffff - nDayRandSeed) % 9699689) + 30029) * 2309;
	return (int)(nDayRandSeed % nRange);
}

// Waits for a given number of miliseconds, or until the active Hwnd == hOrig.
// Great if an operation brings up several dialogs, and you want to make sure that 
// they are all gone before you perform the next operation. (Ivanl)
GUI_API int WaitForOriginalWnd(HWND hOrig, int Sec)	
{	
	while (Sec)
	{
		HWND hAct = WGetActWnd(0) ;
		if(hAct == hOrig)
			return 1 ;
		Sleep(1000) ;
		Sec-- ;
	}
	return 0 ;
}

/**************************************************************************************************
 *																								  *
 * WaitForWndWithCtrl() Takes a control ID which is expected to exist on the active window, Dialog *
 * together with the time in milliseconds to wait before returning a failure if the control is 	  *
 * not found on the active window.	REVIEW(Ivan) should we accept a handle to any window???		  *
 **************************************************************************************************/
// BEGIN_HELP_COMMENT
// Function: HWND WaitForWndWithCtrl(INT ID, DWORD dwMillSec /* = 3000*/)
// Description: Wait for a window with the specified control to be active.
// Return: A non-NULL handle of the window if successful; NULL otherwise.
// Param: ID An integer that contains the ID of the control.
// Param: dwMillSec A DWORD that contains the number of milliseconds to wait for the window to appear. (Default value is 3000.)
// END_HELP_COMMENT
GUI_API HWND WaitForWndWithCtrl(INT ID, DWORD dwMillSec /* = 3000*/)
{
	HWND hWnd = WGetActWnd(0) ;
	DWORD dwDone = 0 ;
   	while( dwDone <= dwMillSec )
	{ 	// Pardon the PropPage misnomer, the contol may not be a direct child of the active window
		// ControlOnPropPage() allows us to search for the control on the active window children as well.
		if(!ControlOnPropPage(ID))
		{	
			DWORD dwSleep = dwMillSec - dwDone >= 1000 ? 1000 : dwMillSec - dwDone;
			Sleep(dwSleep);			// wait another second (or whatever's left)
			dwDone += dwSleep;		// and try at least one more time
		}							
		else
		{										
			return hWnd ;
		}
		if( dwDone >= dwMillSec )	// no more waiting
			return NULL;			// just say we didn't find it
	}
		return NULL;				// should never get hit	
}

/*	GetSystem
 *		returns DWORD which will indicate the system being run on.
 */
// BEGIN_HELP_COMMENT
// Function: DWORD GetSystem(void)
// Description: Get the operating system currently in use.
// Return: A DWORD bit field that specifies the system currently in use: SYSTEM_NT, SYSTEM_DAYTONA, SYSTEM_CHICAGO, SYSTEM_DBCS, SYSTEM_JAPAN.
// END_HELP_COMMENT
GUI_API DWORD GetSystem(void)
{
	
	DWORD dw = 0;
	OSVERSIONINFO strcVerInfo;

	strcVerInfo.dwOSVersionInfoSize = sizeof(strcVerInfo);

	::GetVersionEx(&strcVerInfo);

	if (strcVerInfo.dwMajorVersion == 4 && strcVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if (strcVerInfo.dwMinorVersion == 10)  // this might change with Win98 release?
			dw |= SYSTEM_WIN_98;
		else if (strcVerInfo.dwMinorVersion == 90)  // this might change with Win MILL release?
			dw |= SYSTEM_WIN_MILL;

		else if (strcVerInfo.dwMinorVersion == 0)
			dw |= SYSTEM_WIN_95;
	}
	else if (strcVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (strcVerInfo.dwMajorVersion == 5)
		{
			if (strcVerInfo.dwMinorVersion == 1)
				dw |= SYSTEM_NT_51;
			else
				dw |= SYSTEM_NT_5;
		}
		else if (strcVerInfo.dwMajorVersion == 4)
			dw |= SYSTEM_NT_4;
	}
	if (dw == 0x0) // couldn't figure out what we were on
	{
		// OS not supported with this version of Cafe'
		ASSERT(0);
	}

	// REVIEW: there's got to be a better way to get this
	if( IsDBCSLeadByte((BYTE)0x81) )
	{
		dw |= SYSTEM_DBCS;
		if( !IsDBCSLeadByte((BYTE)0xa1) )
			dw |= SYSTEM_JAPAN;
	}

	return dw;
}

// BEGIN_HELP_COMMENT
// Function: int GetPowerOfTwo(int i)
// Description: Returns the power of 2 for the specified number (i).
// Param: int i - The number for which to calculate the power of 2.
// Return: int - The power of two for the specified number (i).
// END_HELP_COMMENT
GUI_API int GetPowerOfTwo(int i)
	
	{
	int power = 0;

	while(i /= 2)
		power++;
		
	return power;
	}

// BEGIN_HELP_COMMENT
// Function: HWND FindFirstChild(HWND hWnd, int nID)
// Description: This function performs a breadth first search of a window for a child window specified by the control ID given.  This function was created because ::GetDlgItem is not recursive, and CWnd::GetDescendantWindow does a breadth then depth first search, resulting in a different window being found than if a total breadth first search was done.
// Return: The HWND of the window found, or NULL if none were found.
// Param: hWnd - Window from which to start the search.
// Param: nID - Control ID of the child window for which to search
// END_HELP_COMMENT
GUI_API HWND FindFirstChild(HWND hWnd, int nID)
{
	HWND hWndChild;
	if ((hWndChild = ::GetDlgItem(hWnd, nID)) != NULL)
	{
		return hWndChild;
	}
	else
	{
		HWND hWndFoundChild;
		// walk each child
		for (hWndChild = ::GetTopWindow(hWnd); hWndChild != NULL; hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
		{
			hWndFoundChild = FindFirstChild(hWndChild, nID);
			if (hWndFoundChild != NULL)
				return hWndFoundChild;
		}
		return NULL;    // not found
	}
}

// BEGIN_HELP_COMMENT
// Function: HWND FindChildClass(HWND hWnd, LPCSTR pCaption, LPCSTR pClassType /* ==NULL */)
// Description: This function performs a breadth first search of a window for a child window specified by the caption & class.  This function was created because ::WCustomExists() must receive the exact match for the class it's looking for
// Return: The HWND of the window found, or NULL if none were found. Window found must have Non-null caption.
// Param: hWnd - Window from which to start the search.
// Param: pCaption - the caption of window searched for (exact match is required)
// Param: pClassType - the classtype (ex: "Afx:") of window searched for (must be prefix of the actual class found)
// END_HELP_COMMENT
GUI_API HWND FindChildWindow(HWND hWnd, LPCSTR pCaption, LPCSTR pClassType /*=NULL*/)
{
	char szFound[80];

	if(pCaption==NULL || *pCaption==0)
		return NULL; // no caption to compare with
	HWND childhwnd=::GetWindow(hWnd,GW_CHILD);		
	while(childhwnd!=NULL)
	{
		::GetWindowText(childhwnd, szFound, 80);
		if(strcmp(szFound,pCaption)==0)
		{	
			if(pClassType==NULL || *pClassType==0)
				return childhwnd;
			::GetClassName(childhwnd, szFound, 80);
			if(strstr(szFound,pClassType)==szFound)
				return childhwnd;
		}
		HWND hWndFoundChild = FindChildWindow(childhwnd, pCaption, pClassType);
		if (hWndFoundChild != NULL)
			return hWndFoundChild;
		childhwnd=::GetWindow(childhwnd,GW_HWNDNEXT);
	}
	return NULL;    // not found
}

// BEGIN_HELP_COMMENT
// Function: DWORD WaitForSingleControl(const DWORD dwProcessId, const int idControl, const DWORD dwTimeout)
// Description: Wait for a control to be found in any of the specified process's windows. This function can be used to wait for a window to appear with the specified control. See WaitForMultipleControls(int, int*, DWORD) to wait for one of any number of controls.
// Return: A DWORD that specifies the result: WAIT_CONTROL_0, if successful; WAIT_TIMEOUT otherwise.
// Param: dwProcessId A DWORD that specifies the ID of the process.
// Param: idControl An integer that specifies the control to look for on the active window.
// Param: dwTimeout A DWORD that specifies the number of milliseconds to wait for the control.
// END_HELP_COMMENT
GUI_API DWORD WaitForSingleControl(const DWORD dwProcessId, const int idControl, const DWORD dwTimeout)
{
	const int cnPeriod = 100;
	HWND hwnd;
	DWORD dwWait;
	DWORD dwHwndProcessId;

	// loop until the time specified by dwTimeout has elapsed
	for (dwWait = 0; dwWait <= dwTimeout; dwWait += cnPeriod) {
		// iterate through all top level windows
		for (hwnd = ::GetTopWindow(NULL); hwnd != NULL; hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT)) {
			// does this window belong to the process specified?
			GetWindowThreadProcessId(hwnd, &dwHwndProcessId);
			if (dwHwndProcessId == dwProcessId) {
				// look for the control
				if (FindFirstChild(hwnd, idControl)) {
					return WAIT_CONTROL_0;
				}
			}
		}

		// wait
		// if a timeout of 0 is specified, don't wait at all
		if (dwTimeout != 0) {
			Sleep(cnPeriod);
		}
	}

	return WAIT_TIMEOUT;
}

// BEGIN_HELP_COMMENT
// Function: DWORD WaitForMultipleControls(const DWORD dwProcessId, const int cControl, const int aControls[], const DWORD dwTimeout)
// Description: Wait for any of the specified controls to be found in any of the specified process's windows. This function can be used to wait for a window to appear with any one of the the specified controls. See WaitForSingleControl(int, DWORD) to wait for a single control.
// Return: A DWORD that specifies the control found: WAIT_CONTROL_0 through WAIT_CONTROL_n, if successful; WAIT_TIMEOUT otherwise. Only WAIT_CONTROL_0 through WAIT_CONTROL_9 are defined; use WAIT_CONTROL_0 + n to test the return value when more than ten controls are being waited on.
// Param: dwProcessId A DWORD that specifies the ID of the process.
// Param: cControl An integer that specifies the number of controls in the aControls array.
// Param: aControls An integer array that contains the controls to look for on the active window.  The controls should be uniquely chosen for specific window detection and ordered such that the controls for the specific windows requiring detection are listed first.  ID's that specify very generic controls, such as MSG_OK and MSG_YES should not be used, or be listed last, so as to not cause specific window to be incorrectly detected as a general dialog.
// Param: dwTimeout A DWORD that specifies the number of milliseconds to wait for the control.
// END_HELP_COMMENT
GUI_API DWORD WaitForMultipleControls(const DWORD dwProcessId, const int cControls, const int aControls[], const DWORD dwTimeout)
{
	const int cnPeriod = 100;
	DWORD dwWait;
	int nControl;

	// there must be at least one control
	ASSERT(cControls > 0);

	// loop until the time specified by dwTimeout has elapsed
	for (dwWait = 0; dwWait <= dwTimeout; dwWait += cnPeriod) {
		// iterate through each control
		for (nControl = 0; nControl < cControls; nControl++) {
			// is the current control visible?
			if (WaitForSingleControl(dwProcessId, aControls[nControl], 0) == WAIT_CONTROL_0) {
				return WAIT_CONTROL_0+nControl;
			}
		}
		// wait
		// if a timeout of 0 is specified, don't wait at all
		if (dwTimeout != 0) {
			Sleep(cnPeriod);
		}
	}

	return WAIT_TIMEOUT;
}
// BEGIN_HELP_COMMENT
// Function: void TrimString(CString* pStr)
// Description:  This function trims leading and trailing spaces from a string.  It assumes that there are no trailing \n's or \r's on the string that should also be ignored.
// Param: pStr - Pointer to the string to be modified.
// END_HELP_COMMENT
void TrimString(CString* pStr)
{
    char* pBuf;

    // Get a pointer to the first non-space.
    pBuf = pStr->GetBuffer(pStr->GetLength());
    for (int nFirst = 0; isspace(pBuf[nFirst]); nFirst++)
        ;

    // Get a pointer to the last non-space, converting spaces to nulls as we go.
    for (int nLast = strlen(pBuf) - 1; isspace(pBuf[nLast]); nLast--)
        ;

    pStr->ReleaseBuffer();

    *pStr = pStr->Mid(nFirst, nLast - nFirst + 1);
}


// BEGIN_HELP_COMMENT
// Function: BOOL SetFocusToDlgControl(UINT id, HWND hwnd /*=NULL*/)
// Description: Set focus to a control given its ID. If the control is a list box, combo box, or edit box, the caption is the static control immediately preceeding the control. Only the immediate children of the given window are searched.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: id An integer that specifies the ID of the control.
// Param: hwnd An HWND that specifies the window (dialog) that contains the control. NULL means use the currently active window. (Default value is NULL.)
// END_HELP_COMMENT
GUI_API BOOL SetFocusToDlgControl(UINT id, HWND hwnd /*=NULL*/)
{
	HWND hwndDialog = hwnd == NULL ? ControlOnPropPage(id) : hwnd;
	HWND hwndControl = GetDlgItem(hwndDialog, id);
	if( !IsWindow(hwndControl) )
	{
		LOG->RecordInfo("Could not find a control 0x%X in dialog 0x%X ('%s')", id, hwndDialog, (LPCSTR)GetText(hwndDialog));
		return FALSE;
	}

	::SetFocus(hwndControl);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: RandomNumberCheckList::RandomNumberCheckList(void)
// Description: Constructs a RandomNumberCheckList object. This object generates random numbers in a specified range (starting from 0), checking each off as it is generated so only one occurence of each number in the range will be generated for the lifetime of the object.
// Return: none.
// END_HELP_COMMENT
RandomNumberCheckList::RandomNumberCheckList(void)
{
	// seed the random number generator.
	srand(time(NULL));
}


// BEGIN_HELP_COMMENT
// Function: RandomNumberCheckList::RandomNumberCheckList(int iMax)
// Description: Constructs a RandomNumberCheckList object. This object generates random numbers in a specified range (starting from 0), checking each off as it is generated so only one occurence of each number in the range will be generated for the lifetime of the object.
// Return: none.
// Param: iMax An integer specifying the count of numbers the random number generator will have to choose from. The beginning of the range is 0 so, for example, an iMax value of 5 will generate random numbers 0-4.
// END_HELP_COMMENT
RandomNumberCheckList::RandomNumberCheckList(int iMax)
{
	m_iMax = iMax;
	// seed the random number generator.
	srand(time(NULL));
}


// BEGIN_HELP_COMMENT
// Function: void RandomNumberCheckList::SetMax(int iMax)
// Description: Sets the limit on the count of numbers generated by a RandomNumberCheckList object.
// Return: none.
// Param: iMax An integer specifying the count of numbers the random number generator will have to choose from. The beginning of the range is 0 so, for example, an iMax value of 5 will generate random numbers 0-4.
// END_HELP_COMMENT
void RandomNumberCheckList::SetMax(int iMax)
{
	m_iMax = iMax;
}


// BEGIN_HELP_COMMENT
// Function: void RandomNumberCheckList::UndoLast(void)
// Description: Makes the last number selected available for random selection again.
// Return: none.
// END_HELP_COMMENT
void RandomNumberCheckList::UndoLast(void)
{
	// ensure that there is at least one number to undo
	if(m_iaUsed.GetSize() > 0)
		// remove the last number selected.
		m_iaUsed.RemoveAt(m_iaUsed.GetSize() - 1);
}


// BEGIN_HELP_COMMENT
// Function: int RandomNumberCheckList::GetNext(void)
// Description: Gets the next random number from a RandomNumberCheckList object, checking that number on the list so it is not generated again.
// Return: A random number in the range specified when the RandomNumberCheckList object was constructed, unless all the numbers have been previously generated, in which case the return is -1.
// END_HELP_COMMENT
int RandomNumberCheckList::GetNext(void)
{
	// check if we've used up all the numbers.
	if(m_iaUsed.GetSize() == m_iMax)
		// -1 means all the numbers have been used.		
		return -1;	

	// used to store the random number generated.
	int n;
	// used to index the array of used numbers.
	int i;

	// keep generating numbers until an unused one is found.
	while(1)
	
	{
		// generate a number within the range specified. 
		n = rand() % m_iMax;

		// iterate through the list of used numbers.
		for(i = 0; i < m_iaUsed.GetSize(); i++)
		{
			// if n has already been used, we start over.
			if(m_iaUsed[i] == n)
				break;
		}

		// we checked the used list and didn't find it, so n is a new number.
		if(i == m_iaUsed.GetSize())
		{
			// and now it is also a used number.
			m_iaUsed.Add(n);
			return n;
		}
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL COApplication::Attach(LPCSTR szAppTitle /* NULL */, int secWait /* 0 */)
// Description: Attach to the specified application.
// Return: A BOOL that indicates success. TRUE if successful; FALSE otherwise.
// Param: szAppTitle A pointer to a string that contains the name of application to attach to (the window title). The default value is NULL. If NULL is specified, the function immediately attaches to the active application.
// Param: secWait An integer specifying the number of seconds to wait for the app to become active. The default is 0. If 0 is specified, the function immediately attaches to the active application.
// END_HELP_COMMENT
BOOL COApplication::Attach(LPCSTR szAppTitle /* NULL */, int secWait /* 0 */)

{
	// can't wait negative seconds.
	ASSERT(secWait >= 0);
	
	// either wait for the window with the specified title to appear,
	// or immediately attach to the active window.
	if(szAppTitle)
	{
		if(!(m_hWnd = MST.WFndWndWait(szAppTitle, FW_PART, secWait)))
			LOG->RecordInfo("ERROR in COApplication::Attach - "
							"cannot find app '%s' after %d seconds.",
							szAppTitle, secWait);
	}
	else
		m_hWnd = MST.WGetActWnd(0);

	return m_hWnd != NULL;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COApplication::Activate(void)
// Description: Activate the application. An application must be attached (see Attach function) before it can be activated.
// Return: A BOOL that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COApplication::Activate(void)
{

	// can't activate a null window handle.
	if(!m_hWnd)
	{
		LOG->RecordInfo("ERROR in COApplication::Activate - "
						"no app is attached to the object.");
		return FALSE;
	}

	WSetActWnd(m_hWnd);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COApplication::SendKeys(LPCSTR szKeys)
// Description: Send keystrokes to the application. The application must be active (see Activate function) before keystrokes can be sent to it.
// Return: void.
// Param: szKeys A pointer to a string that contains the keys to send.
// END_HELP_COMMENT
void COApplication::SendKeys(LPCSTR szKeys)
{
	// can't do NULL keys.
	ASSERT(szKeys);
	MST.DoKeys(szKeys);
}


GUI_API DWORD GetProductType(void)

{
	// determine if we are running against pro, enterprise, or learning edition.
	char szSkuValue[4];
	GetEnvironmentVariable("sku", szSkuValue, 4);

	if(((CString)szSkuValue == "p") || ((CString)szSkuValue == "P"))
		return (DWORD)VC_PRO;
	else if(((CString)szSkuValue == "l") || ((CString)szSkuValue == "L"))
		return (DWORD)VC_LRN;
	else
		return (DWORD)VC_ENT;
}

// BEGIN_HELP_COMMENT
// Function: TARGET_API void EditUndo(int count)
// Description: Does Undo (^Z) in active window
// Param: count A number of Undos that will be done 
// Return: void.
// END_HELP_COMMENT
GUI_API void EditUndo(int count)
{
	for(int ii = 0; ii < count; ii++)	
		MST.DoKeys("^z");
}

// BEGIN_HELP_COMMENT
// Function: TARGET_API void EditRedo(int count)
// Description: Does Undo (^Y) in active window
// Param: count A number of Undos that will be done 
// Return: void.
// END_HELP_COMMENT
GUI_API void EditRedo(int count)
{
	for(int ii = 0; ii < count; ii++)	
		MST.DoKeys("^y");
}


// BEGIN_HELP_COMMENT
// Function: TARGET_API BOOL GetDragFullWindows(BOOL *bResult)
// Description: Checks if "Show window contents while dragging" is turned on/off 
// Param: BOOL *bResult TRUE if "Show window contents while dragging" is turned on.  FALSE, otherwise.
// Return: TRUE if successfull.
// END_HELP_COMMENT
GUI_API BOOL GetDragFullWindows(BOOL *bResult)
{
	HKEY hkDragFullWin;
	DWORD cbData = 2;
	unsigned char lpData[2];
	unsigned long Type  = REG_SZ;

	if(ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, "Control Panel\\Desktop", &hkDragFullWin))
		return FALSE;

	if(ERROR_SUCCESS != RegQueryValueEx(hkDragFullWin, "DragFullWindows", NULL, &Type, lpData, &cbData))
		return FALSE;

	RegCloseKey(hkDragFullWin);

	if(*lpData == '0')
		*bResult = FALSE;
	else 
		*bResult = TRUE;

	return TRUE;
}



