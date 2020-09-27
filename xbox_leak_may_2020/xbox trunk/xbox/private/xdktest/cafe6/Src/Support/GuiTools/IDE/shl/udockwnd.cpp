///////////////////////////////////////////////////////////////////////////////
//  UDOCKWND.CPP
//
//  Created by :            Date :
//      DavidGa                 12/10/93
//
//  Description :
//      Implementation of the UIDockWindow class
//

#include "stdafx.h"
#include "udockwnd.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "uwbframe.h"
#include "..\sym\reg.h"
#include "..\sym\dockids.h"
#include "..\sym\qcqp.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::AttachActive(void)
// Description: Initialize this object by attaching to the active dockable window.
// Return: TRUE if successfully attached to the active dockable window; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDockWindow::AttachActive(void)
{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}
    HWND hwndFocus = MST.WGetFocus();
    HWND hwndDock = UIWB.GetToolbar(GetID(),pkgId);
    if( (hwndFocus == hwndDock) || (::IsChild(hwndDock, hwndFocus)) )
        return Attach(hwndDock);
    else
        return Attach(hwndFocus);       // will return FALSE, but the HWnd() will still point to something.
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::Find(void)
// Description: Find the dockable window indicated by this object. This function is useful only when used on object of a class derived from UIDockWindow.
// Return: TRUE if the dockable window indicated by this object is found; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDockWindow::Find(void)
{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}
    return Attach(UIWB.GetToolbar(GetID(),pkgId));
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::IsValid(void) const
// Description: Determine if the dockable window indicated by this object is valid.
// Return: TRUE if the dockable window is valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDockWindow::IsValid(void) const
{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}
    return UIWindow::IsValid() && (HWnd() == UIWB.GetToolbar(GetID(),pkgId));
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::IsActive(void)
// Description: Determine if the dockable window indicated by this object is active.
// Return: TRUE if the dockable window is active; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDockWindow::IsActive(void)
{
    HWND hwnd = MST.WGetFocus();
    return (hwnd == HWnd()) || IsChild(HWnd(), hwnd);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::IsDockable(void)
// Description: Determine if the dockable window is dockable or MDI.
// Return: A Boolean value that indicates whether the dockable window is dockable (TRUE) or MDI.
// END_HELP_COMMENT
BOOL UIDockWindow::IsDockable(void)
{
	// Check extended style of window.  If it's MDI, then it's not dockable.
	return (GetWindowLong(HWnd(), GWL_EXSTYLE) & WS_EX_MDICHILD)? FALSE: TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::Close(void)
// Description: Close the dockable window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDockWindow::Close(void)

    {
    if(Activate())

        {
        if(IsDockable())
			{
			// REVIEW (michma): where is Window.Hide?
			// %wd now points to Window.Docking View.
            //UIWB.DoCommand(ID_WINDOW_HIDE, DC_MNEMONIC);
			}
		else
            UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);
        
        return TRUE;
        }
                    
    return FALSE;
    }

// BEGIN_HELP_COMMENT
// Function: UINT UIDockWindow::GetID(void) const
// Description: Get the id of the dockable window indicated by this object. This function is only valid when used on object of a class derived from UIDockWindow.
// Return: The id of the dockable window, 0 if not associated with any dockable window.
// END_HELP_COMMENT
UINT UIDockWindow::GetID(void) const
{
    return 0;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::ActivatePane(LPCSTR)
// Description: Activate a specified pane within this docked window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDockWindow::ActivatePane(LPCSTR szPaneName)

{
	const char* const THIS_FUNCTION = "UIDockWindow::ActivatePane";

    Activate();     // Activate this particular dockable window.
	CString strTarget(szPaneName);	// Store the target pane name.
	CString strCurrent;

	HWND hwndFirst = MST.WGetFocus();
	HWND hwndCurrent = hwndFirst;

    BOOL bFound = FALSE;    
    while(!bFound)

    {
		// This window's caption contains the name of the currently active pane.
		MST.WGetText(hwndCurrent, strCurrent);
		LOG->RecordInfo("%s: Current pane is '%s'.", THIS_FUNCTION, strCurrent);

        if (strCurrent == strTarget)    
            bFound = TRUE;              // Current panename matches target.
        else
        
		{   
            // Goto next pane.
            MST.DoKeys("^({PgUp})");
			hwndCurrent = MST.WGetFocus();
            
			if (hwndFirst == hwndCurrent)
			{
				LOG->RecordInfo("ERROR in %s: Could not find pane '%s'.", THIS_FUNCTION, szPaneName);
				break;                  // Been through them all.
			}
        }
    }

    return bFound;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDockWindow::IsDocked(int nLocation = /* DW_DONT_CARE */)
// Description: Determine if this docking window is currently docked in the position given by nLocation.
// Return: TRUE if the toolbar is docked; FALSE otherwise.
// Param: loc The dock location of the toolbar: DW_DONT_CARE, DW_TOP, DW_BOTTOM, DW_RIGHT, DW_LEFT.
// END_HELP_COMMENT
BOOL UIDockWindow::IsDocked(DockLocation loc)
{
    // If it's not visible or dockable, then it's definitely not docked.
    if (!IsWindowVisible(HWnd()) || !IsDockable())
        return FALSE;

    // If it's style indicates that it's a popup window, then it's dockable, but not docked.
    if ( ::GetWindowLong(HWnd(), GWL_STYLE) & WS_POPUP)
        return FALSE;                           // Window is floating

    // We know it's docked, now check it's position by comparing it with the 
    // MDI client window position.
    RECT rcWBClient;

    ::GetClientRect( UIWB,  &rcWBClient);
    ::MapWindowPoints( UIWB, HWND_DESKTOP, (POINT*)&rcWBClient, 2); 
    
    RECT rcWBMDIClient;
    HWND hMDIClient = UIWB.GetClient();                 // Returns MDI Client
    ::GetWindowRect( hMDIClient, &rcWBMDIClient );
                
    
    RECT rcDockableWnd;

    ::GetWindowRect(HWnd(), &rcDockableWnd);              // Get dockable window dimensions
    
    int nXBorder = ::GetSystemMetrics(SM_CXBORDER);     // Width of a border (usually 1 pix)
    int nYBorder = ::GetSystemMetrics(SM_CYBORDER);     // Heigth of border (usually 1 pix) 
    
	switch(loc)
	{
	case DontCare:
		return TRUE;

	case Top:
		if( (rcWBClient.top  <= rcDockableWnd.top + nYBorder) && (rcWBMDIClient.top >= rcDockableWnd.bottom - nYBorder) )
			return TRUE;
		break;

	case Bottom:
		if( (rcWBClient.bottom >= rcDockableWnd.bottom - nYBorder) && (rcWBMDIClient.bottom <= rcDockableWnd.top + nYBorder) )
			return TRUE;
		break;

	case Left:
		if( (rcWBClient.left <= rcDockableWnd.left + nXBorder) && (rcWBMDIClient.left >= rcDockableWnd.right - nXBorder) )
			return TRUE;
		break;

	case Right:
		if( (rcWBClient.right >= rcDockableWnd.right - nXBorder) && (rcWBMDIClient.right <= rcDockableWnd.left + nXBorder ) )
			return TRUE;
		break;

	default:
		ASSERT(FALSE);	// Can't happen.
	}

    return FALSE;
}      
