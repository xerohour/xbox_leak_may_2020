///////////////////////////////////////////////////////////////////////////////
//  COCLIP.CPP
//
//  Created by :            Date :
//      DavidGa                 1/11/94
//
//	Description :
//		Implementation of the COClipboard component object class
//

#include "stdafx.h"
#include "testxcpt.h"
#include "coclip.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//  COClipboard class

COClipboard::COClipboard(void)
{
}

// BEGIN_HELP_COMMENT
// Function: BOOL COClipboard::Empty(void)
// Description: Clear the text on the system clipboard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COClipboard::Empty(void)
{
	if( !OpenClipboard( NULL ) )
		EXPECT_EXEC( FALSE, "Cannot open clipboard to empty contents" );
	else if( !EmptyClipboard() )
	{
		CloseClipboard();
		EXPECT_EXEC( FALSE, "Cannot empty clipboard" );
	}
	else
	{
		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: CString COClipboard::GetText(void)
// Description: Get the text from the system clipboard.
// Return: A CString that contains the text from the system clipboard.
// END_HELP_COMMENT
CString COClipboard::GetText(void)
{
	CString str;

	EXPECT_EXEC( OpenClipboard( NULL ), "Cannot open clipboard to get text" );

	HANDLE hText = GetClipboardData( CF_TEXT );
	if ( !hText )
	{
		CloseClipboard();
		EXPECT_EXEC( FALSE, "Cannot get handle to clipboard text" );
	}
	else
	{
		LPSTR lpText = (LPSTR)GlobalLock( hText );
		if ( !lpText )
		{
			CloseClipboard();
			EXPECT_EXEC( FALSE, "Cannot lock handle for clipboard text" );
		}
		else
		{
			str = lpText;
			GlobalUnlock( hText );
			CloseClipboard();
		}
	}   

	return str;
}

// BEGIN_HELP_COMMENT
// Function: BOOL COClipboard::SetText(LPCSTR szTextIn)
// Description: Set the text in the system clipboard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szTextIn A pointer to a string containing the text to put in the clipboard.
// END_HELP_COMMENT
BOOL COClipboard::SetText(LPCSTR szTextIn)
{
    if ( !OpenClipboard( NULL ) )
    {
        EXPECT_EXEC( FALSE, "Could not open clipboard to set text" );
        return FALSE;
    }

    
    HANDLE hText = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, 1+strlen(szTextIn) );


    if ( !hText )
    {
        CloseClipboard();
        EXPECT_EXEC( FALSE, "Could not allocate memory for clipboard text" );
        return FALSE;
    }
    

    LPSTR lpText = (LPSTR)GlobalLock( hText );

    if ( !lpText )
    {
        CloseClipboard();
        GlobalFree(hText);
        EXPECT_EXEC( FALSE, "Could not lock handle for clipboard text" );
        return FALSE;
    }

    strcpy( lpText, szTextIn);

    GlobalUnlock( hText );
    
    HANDLE hData = SetClipboardData( CF_TEXT, hText );

    CloseClipboard();

    if (!hData)
    {
        EXPECT_EXEC( FALSE, "Could not set clipboard data" );
        return FALSE;
    }

 
    return TRUE;
}
