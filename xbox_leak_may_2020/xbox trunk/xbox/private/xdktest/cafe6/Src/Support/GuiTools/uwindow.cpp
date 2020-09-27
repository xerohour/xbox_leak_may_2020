///////////////////////////////////////////////////////////////////////////////
//	UWINDOW.CPP
//
//	Created by :			Date :
//		DavidGa					9/20/93
//
//	Description :
//		Declaration of the UIWindow class
//

#include "stdafx.h"
#include "uwindow.h"
#include "testutil.h"
#include "mstwrap.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/*	UIWindow
 *		Default parameter allows single function to be both simple constructor
 *		and copy constructor.  I have chosen not to have a constructor that
 *		attaches to the active window.
 */
UIWindow::UIWindow(HWND hwnd)
{
	m_hWnd = hwnd;
	OnUpdate();
}

/*	operator= and Attach
 *		Two ways to attach an HWND to a UIWindow.
 *		operator= returns an HWND, so that ='s may be chained.
 *		Attach returns a BOOL, stating whether the HWND was valid or not.
 */
// BEGIN_HELP_COMMENT
// Function: HWND UIWindow::operator=(HWND hwnd)
// Description: Assign this UIWindow object or HWND to another UIWindow object or HWND.
// Return: HWND that specifies the UIWindow object.
// Param: hwnd The HWND of the object to assign from.
// END_HELP_COMMENT
HWND UIWindow::operator=(HWND hwnd)
{
	m_hWnd = hwnd;
	OnUpdate();
	return m_hWnd;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::Attach(HWND hwnd)
// Description: Attach to the window with the given handle.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: hwnd An HWND that specifies the handle of the window to attach to.
// END_HELP_COMMENT
BOOL UIWindow::Attach(HWND hwnd)
{
	operator=(hwnd);
	return IsValid();
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::AttachActive(void)
// Description: Attach to the active window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWindow::AttachActive(void)
{
	return Attach(MST.WGetActWnd(0));
}

/*	Find
 *		Derived classes should override this function, and implement a search
 *		for a window that fits their expectations.
 *		Once found, call Attach with that HWND - do not call the base class.
 *		REVIEW: can the base class be made more useful?
 */
// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::Find(void)
// Description: Find the window and attach to it. This member function must be overridden by the derived class.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWindow::Find(void)
{
	return Attach(NULL);
}

/*	OnUpdate
 *		Called every time a new hwnd has been attached to the UIWindow.
 *		Override this function in order to keep other member variables in sync.
 *		Do NOT call the base class's OnUpdate, as the only way this function is
 *		called is through a constructor or operator=, both of which use macros 
 *		to make sure that all classes get called.
 */
void UIWindow::OnUpdate(void)
{
}

/*	IsValid
 *		Should cantain any convenient checks that will ensure that m_hWnd
 *		(derived classes must use HWnd() or PWnd()) is really of the type
 *		expected for the particular class.
 *		Always call the base class first, both because anything that fails the
 *		base class test should necessarilybe invalid, and because that will
 *		help avoiding crashes where you assumed things like ::IsWindow(m_hWnd).
 */
// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::IsValid(void) const
// Description: Determine if the window is valid by verifying the validity of its HWND and that it's visible.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWindow::IsValid(void) const
{
	return ::IsWindow(m_hWnd) && IsVisible();	// REVIEW(davidga): don't test HasFocus()
}

/*	GetText
 *		Get's the title text (using SendMessage(WM_GETTEXT)) of m_hWnd.
 *		The text is placed either in a provided char buffer, or else into
 *		a new CString.
 */
// BEGIN_HELP_COMMENT
// Function: LPCSTR UIWindow::GetText(LPSTR sz, int cMax) const
// Description: Get the window text (by sending a WM_GETTEXT) message to the window. (See CString UIWindow::GetText(void) to get the text without allocating a buffer.)
// Return: A pointer to the beginning of the buffer pointed to by the sz parameter.
// Param: sz A pointer to a string that will contain the text of the window.
// Param: cMax An integer that specifies the size of the sz buffer.
// END_HELP_COMMENT
LPCSTR UIWindow::GetText(LPSTR sz, int cMax) const
{
	return ::GetText(m_hWnd, sz, cMax);
}

// BEGIN_HELP_COMMENT
// Function: CString UIWindow::GetText(void) const
// Description: Get the window text (by sending a WM_GETTEXT) message to the window. (See LPCSTR UIWindow::GetText(LPSTR, int) to get the text in a user allocated buffer.)
// Return: A pointer to the beginning of the buffer pointed to by the sz parameter.
// END_HELP_COMMENT
CString UIWindow::GetText(void) const
{
	return ::GetText(m_hWnd);
}

/*	IsActive
 *		Determines if the m_hWnd is the currently active window.
 *		Override this function for classes representing child windows, and 
 *		other things that will not be returned by WGetActWnd()
 *		(e.g. MDI children).
 */
// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::IsActive(void)
// Description: Determine if the window is active.
// Return: A Boolean value that indicates whether the window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIWindow::IsActive(void)
{
	return HWnd() == WGetActWnd(0);
}

/*	Activate
 *		Activates this HWND.  I don't know if this will work for child windows
 *		as well - I'm not sure what WSetActWnd does.  If there is a problem
 *		with child windows, override this function in the appropriate classes.
 */
// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::Activate(void)
// Description: Make this window the active window. (See UIWindow::BlockingActivate() to make this window active and wait until it's active.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWindow::Activate(void)
{
	MST.WSetActWnd(HWnd());								
	return IsActive();			
}

/*	BlockingActivate
 *		Activates this HWND.  Same as Activate but blocks until window isActive
 */
// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::BlockingActivate(void)
// Description: Make this window the active window and wait until it is active. (See UIWindow::Activate() to make this window active without waiting.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWindow::BlockingActivate(void)
{
	::WSetActWnd(HWnd());								
	while (!IsActive())
		 Sleep(10);
	return IsActive();			
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::HasFocus(void) const
// Description: Determine if this window has focus.
// Return: A Boolean value that indicates whether this window has focus (TRUE) or not.
// END_HELP_COMMENT
BOOL UIWindow::HasFocus(void) const
{
	HWND hwnd = MST.WGetFocus();
	return (hwnd == HWnd()) ||			// is the focus in the hwnd
			(IsChild(HWnd(), hwnd) );	// or at least in one of its children
}

/*	WaitAttachActive
 *		Waits until the window retrieved by AttachActive passes the IsValid test,
 *		or until dwMilliseconds times out.
 *		For UIWindow, this is the same as AttachActive.  But in derived classes, 
 *		with good overrides of IsValid and AttachActive, this can be very useful.
 *		Returns the HWND, to be more useful.
 */
// BEGIN_HELP_COMMENT
// Function: HWND UIWindow::WaitAttachActive(DWORD dwMilliseconds)
// Description: Wait until active window is attached to successfully or the specified time-out is reached.
// Return: An HWND that specifies the window attached to.
// Param: dwMilliseconds A DWORD that specifies the number of milliseconds to wait.
// END_HELP_COMMENT
HWND UIWindow::WaitAttachActive(DWORD dwMilliseconds)
{
	while( !AttachActive() )		// Attach returns IsValid of new HWND
	{
		if( dwMilliseconds == 0 )
		{
//			Detach();		// REVIEW: this makes testing a pain
			return NULL;
		}
		DWORD dwSleep = dwMilliseconds > 100 ? 100 : dwMilliseconds;  //Waynebr reduce 1000 to 100 ms 
		Sleep(dwSleep);
		dwMilliseconds -= dwSleep;
	}
	return HWnd();
}

/*	WaitUntilGone
 *		Checks every second (or until dwMilliSeconds times out) for m_hWnd
 *		to go away, as determined by ::IsWindow().
 */
// BEGIN_HELP_COMMENT
// Function: BOOL UIWindow::WaitUntilGone(DWORD dwMilliseconds)
// Description: Wait until the window is no longer valid or the specified time-out is reached.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: dwMilliseconds A DWORD that specifies the number of milliseconds to wait.
// END_HELP_COMMENT
BOOL UIWindow::WaitUntilGone(DWORD dwMilliseconds)
{
	while( ::IsWindow(HWnd()) )		// As long as m_hWnd is valid, then it's still there
	{
		if( dwMilliseconds == 0 )
			return FALSE;
		DWORD dwSleep = dwMilliseconds > 1000 ? 1000 : dwMilliseconds;
		Sleep(dwSleep);
		dwMilliseconds -= dwSleep;
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: int UIWindow::ResizeWindow(BOOL bIsDocked, WINDOW_SIDE wSide, MOVE_DIRECTION wDirection, int count)
// Description: Resizes window
// Return: int actual position change (in number of arrow hits)
// Param: A Boolean value that specifies if the window is Docked or not. If TRUE it is Docked; FALSE otherwise.
// Param: WINDOW_SIDE wSide Determines which side should be moved (SLeft, STop, SRight, SBottom)
// Param: MOVE_DIRECTION wDirection Determined the direction where to move the side (DLeft, DUp, DRight, DDown)
// Param:  int desired position change (in number of arrow hits)
// Param: BOOL DragFullWindows A Boolean value that specifies if "Show window contents while dagging".
// END_HELP_COMMENT
int UIWindow::ResizeWindow(BOOL bIsDocked, WINDOW_SIDE wSide, MOVE_DIRECTION wDirection, int count, BOOL DragFullWindows)
{

	WNDPOS WinPos;
	int nActualCount;
	POINT old_curpos, new_curpos;
	RECT old_rect, new_rect;
	const int max_int = 65535;
	bool bCheckOldPosition;

	//Activate();

	if(bIsDocked)		// If Docked we emulate right mouse click on the title bar
	{
		::WGetWndPos(m_hWnd, &WinPos, FALSE);
		WinPos.left = (WinPos.left < 0) ? 1 : WinPos.left;
		WinPos.top = (WinPos.top < 0) ? 1 : WinPos.top;
		::QueMouseClick(VK_RBUTTON, WinPos.left + 10, WinPos.top + 5);
		::QueFlush(TRUE);
	}
	else				// If Not Docked we emulate "Alt -" keystroke
	{
		MST.DoKeys("%{-}");
	}

	// Choose size
	MST.DoKeys("{DOWN 2}");
	MST.DoKeys("{ENTER}");

	// Choose the side we are going to move
	switch(wSide)
	{
		case SLeft:
			MST.DoKeys("{Left}");
			break;
		case STop:
			MST.DoKeys("{Up}");
			break;
		case SRight:
			MST.DoKeys("{Right}");
			break;
		case SBottom:
			MST.DoKeys("{Down}");
			break;
	}

	// count < 0 means as much as possible
	if(count < 0)
		count = max_int; // Is it possible to move more than 65535 pixels? dklem

	// Move the side
	for(bCheckOldPosition = TRUE, nActualCount = 0; nActualCount < count; nActualCount++)
	{
		GetCursorPos(&old_curpos);
		GetWindowRect(m_hWnd, &old_rect);		

		switch(wDirection)
		{
		case DLeft:
			MST.DoKeys("{Left}");
			break;
		case DUp:
			MST.DoKeys("{Up}");
			break;
		case DRight:
			MST.DoKeys("{Right}");
			break;
		case DDown:
			MST.DoKeys("{Down}");
			break;
		}
		// Do size change until position is not changed
		GetCursorPos(&new_curpos);
		GetWindowRect(m_hWnd, &new_rect);
		if(	bCheckOldPosition && (
			(DragFullWindows && new_rect.left == old_rect.left && new_rect.top == old_rect.top &&	new_rect.right == old_rect.right && new_rect.bottom == old_rect.bottom)
			||
			(!DragFullWindows && new_curpos.x == old_curpos.x && new_curpos.y == old_curpos.y)
			)
		  )
		{
			count =  nActualCount + 3; // Let's hit arrow a couple of times.  Just in case.
			bCheckOldPosition = FALSE;
		}
	}


	// Fix new position
	MST.DoKeys("{ENTER}");

	if(!bCheckOldPosition)
		nActualCount -= 2;

	// Return actual position change (in arrow hits)
	return (nActualCount < 0) ? -nActualCount: nActualCount;
}


// BEGIN_HELP_COMMENT
// Function: int UIWindow::MoveWindow(BOOL bIsDocked, MOVE_DIRECTION wDirection, int count)
// Description: Moves window
// Return: int actual position change (in number of arrow hits)
// Param: A Boolean value that specifies if the window is Docked or not. If TRUE it is Docked; FALSE otherwise.
// Param: MOVE_DIRECTION wDirection Determined the direction where to move the window (DLeft, DUp, DRight, DDown)
// Param:  int desired posizion change (in number of arrow hits)
// Param: BOOL DragFullWindows A Boolean value that specifies if "Show window contents while dagging".
// END_HELP_COMMENT
int UIWindow::MoveWindow(BOOL bIsDocked, MOVE_DIRECTION wDirection, int count, BOOL DragFullWindows)
{

	WNDPOS WinPos;
	int nActualCount;
	POINT old_curpos, new_curpos;
	RECT old_rect, new_rect;
	const int max_int = 65535;
	bool bCheckOldPosition;

	//Activate();

	if(bIsDocked)		// If Docked we emulate right mouse click on the title bar
	{
		::WGetWndPos(m_hWnd, &WinPos, FALSE);
		WinPos.left = (WinPos.left < 0) ? 1 : WinPos.left;
		WinPos.top = (WinPos.top < 0) ? 1 : WinPos.top;
		::QueMouseClick(VK_RBUTTON, WinPos.left + 10, WinPos.top + 5);
		::QueFlush(TRUE);
	}
	else				// If Not Docked we emulate "Alt -" keystroke
	{
		MST.DoKeys("%{-}");
	}

	// Choose size
	MST.DoKeys("{DOWN}");
	MST.DoKeys("{ENTER}");

	// count < 0 means as much as possible
	if(count < 0)
		count = max_int; // Is it possible to move more than 65535 pixels? dklem

	// Move the side
	for(bCheckOldPosition = TRUE, nActualCount = 0; nActualCount < count; nActualCount++)
	{
		GetCursorPos(&old_curpos);
		GetWindowRect(m_hWnd, &old_rect);		

		switch(wDirection)
		{
		case DLeft:
			MST.DoKeys("{Left}");
			break;
		case DUp:
			MST.DoKeys("{Up}");
			break;
		case DRight:
			MST.DoKeys("{Right}");
			break;
		case DDown:
			MST.DoKeys("{Down}");
			break;
		}
		// Do size change until position is not changed
		GetCursorPos(&new_curpos);
		GetWindowRect(m_hWnd, &new_rect);
		if(	bCheckOldPosition && (
			(DragFullWindows && new_rect.left == old_rect.left && new_rect.top == old_rect.top &&	new_rect.right == old_rect.right && new_rect.bottom == old_rect.bottom)
			||
			(!DragFullWindows && new_curpos.x == old_curpos.x && new_curpos.y == old_curpos.y)
			)
		  )
		{
			count =  nActualCount + 3; // Let's hit arrow a couple of times.  Just in case.
			bCheckOldPosition = FALSE;
		}
	}


	// Fix new position
	MST.DoKeys("{ENTER}");

	if(!bCheckOldPosition)
		nActualCount -= 2;

	// Return actual posizion change (in arrow hits)
	return (nActualCount < 0) ? -nActualCount: nActualCount;
}
