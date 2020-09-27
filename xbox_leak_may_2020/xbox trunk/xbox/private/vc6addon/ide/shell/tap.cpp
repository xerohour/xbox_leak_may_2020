// tap.cpp : implementation file
//

#include "stdafx.h"
#include "tap.h"
#include "imeutil.h"

// The V4,x implementation of this control barred many keys from being entered
// These keys included standard windows keys, and other general-purpose keys.
// For Orion, we are allowing all keys to be customised except:
//
// Unshifted backspace (needed to remove items from the control)
// Double tab
//
// Because users without mice will want to use tab to move out of the control,
// we define that tab will move us out of the control when there is something in
// the control, but insert a tab otherwise. This allows single tab to be customised.
//
// This means that while the focus is in this control almost all standard 
// windows navigation keys are disabled. But the user can always use tab to
// get out of the control, at which point all the controls become enabled 
// again.
// martynl 12Mar96

/////////////////////////////////////////////////////////////////////////////
// CTap control

CTap::CTap()
{
	m_accel.fVirt = FVIRTKEY;	// by default, I prefer to deal with VirtKeys
	m_accel.key = 0;
	m_accel.cmd = 0;
	m_chord.fVirt = FVIRTKEY;
	m_chord.key = 0;
	m_chord.cmd = 0;
	m_fState = FVIRTKEY;
}

CTap::~CTap()
{
}


BEGIN_MESSAGE_MAP(CTap, CWnd)
	//{{AFX_MSG_MAP(CTap)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_GETDLGCODE()
	ON_WM_MENUCHAR()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTap message handlers

void CTap::ResetAll()
{
	m_accel.fVirt = m_fState;
	m_accel.key = 0;
	m_accel.cmd = 0;

	ResetChord();
}

void CTap::ResetChord()
{
	m_chord.fVirt = FVIRTKEY;
	m_chord.key = 0;

	InvalidateContent();
}

void CTap::SetAccel(ACCEL& accel)
{
	m_accel = accel;
	InvalidateContent();
	NotifyParent(TAPN_CHANGE);
}

void CTap::SetChord(ACCEL& chord)
{
	m_chord = chord;
	InvalidateContent();
	NotifyParent(TAPN_CHANGE);
}

void CTap::InvalidateContent(BOOL bErase /* = TRUE */)
{
	CRect rc;
	GetClientRect( rc );

	rc.InflateRect( -3, -3 );
	rc.OffsetRect( 1, 1 );
	InvalidateRect( rc, FALSE/*bErase*/ );
}

BOOL CTap::Create(const char* szName, const RECT& rc, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	CRect rect(rc);
	rect.InflateRect(1, 1); // to accomodate border...
	return CWnd::Create( NULL,		// m_szClassName ?
						 szName, 
						 WS_TABSTOP | WS_VISIBLE | WS_CHILD,
						 rc, pParentWnd, nID, pContext );

}

BOOL CTap::PreTranslateMessage( MSG *pMsg )
{
	switch (pMsg->message)
	{
	case WM_SYSCHAR:
		return TRUE;
	default:
		return FALSE;
	}
}

UINT CTap::OnGetDlgCode()
{
	ASSERT(CWnd::OnGetDlgCode() == 0);	// I'm asserting that I don't care about the base class function
	return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTTAB;
}

BOOL CTap::SuppressKey(UINT nChar) const
{
	return FALSE;
	// V4.x supressed:VK_TAB, VK_ESCAPE, VK_RETURN, VK_BACK, VK_SPACE
}

void CTap::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if((nFlags & 0x4000) != 0) // ignore repeats to prevent flashing
		return;

	if(IsChordSet() &&		// we had a full chord, so this must be from scratch
	   !SuppressKey(nChar))
			ResetAll();			// clear everything

	switch(nChar)
	{
	case VK_CONTROL:
		SetControl(!IsKeySet());
		break;

	case VK_SHIFT:
		SetShift(!IsKeySet());
		break;

	case VK_MENU:
		SetAlt(!IsKeySet());
		break;

	case VK_TAB:
		if(IsKeySet())
		{
			if(m_fState & FSHIFT) {
				ClearShift(!IsKeySet());
			}
			SymTab();
		}
		else
		{
			goto KDefault;
		}
		break;

	case VK_BACK:
		// delete is delete if it is unshifted, or if in second position
		if (IsKeySet() || (!IsKeySet() && !IsAlt() && !IsControl() && !IsShift())) {
			if(IsChordSet())
				ResetChord();
			else
				ResetAll();
			break;
		}
		// DELIBERATE FALL THROUGH


	default:
	KDefault:
		if (!IsKeySet())
		{
			m_accel.key = (WORD)nChar;
#if 0 //BC
			if (nChar >= 0x80 && (nFlags & 0x80) == 0)		// user typed a non-virtkey punctuation
			{											
				m_accel.key = VkToAscii(nChar, nFlags, m_accel);	// get Ascii char, not non-standard VirtKey
				SetAscii(!IsKeySet());								// flip to ascii so it is displayed ok
				ClearShift(!IsKeySet());							// and forget SHIFT, since the char is shown shifted
			}
#endif
		}
		else	// create chord
		{
			ASSERT(!IsChordSet());
			m_chord.key = (WORD)nChar;
#if 0 //BC
			if (nChar >= 0x80 && (nFlags & 0x80) == 0)		// user typed a non-virtkey punctuation
			{											
				m_chord.key = VkToAscii(nChar, nFlags, m_chord);	// get Ascii char, not non-standard VirtKey
			}
#endif
			ASSERT((m_chord.key & 0xff00) == 0 || (m_chord.key & 0xff00) == 0xff00);
			m_chord.key &= 0x00ff;
			m_chord.fVirt = m_fState;
		}
		break;
#if 0
	// used in 4.x to handle other special characters
	case VK_ESCAPE:
		SymExit();
		break;

	case VK_RETURN:
		SymOk();
		break;

	case VK_SPACE:
		break;		// do nothing - alt+space and ctrl+space still enabled
#endif

	}
	InvalidateContent();
	NotifyParent(TAPN_CHANGE);

//	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTap::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt > 1)
		return;

	switch(nChar)
	{
		case VK_CONTROL:
			ClearControl(!IsKeySet());
			break;
		case VK_SHIFT:
			ClearShift(!IsKeySet());
			break;
		case VK_MENU:
			ClearAlt(!IsKeySet());
			break;
	}
	InvalidateContent();
	NotifyParent(TAPN_CHANGE);

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CTap::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt > 1 || (nFlags & 0x4000))
		return;

	if(IsChordSet() &&						// we had a full chord, so this must be from scratch
	   !SuppressKey(nChar))
			ResetAll();			// clear everything

	switch(nChar)
	{
	case VK_CONTROL:
		SetControl(!IsKeySet());
		break;

	case VK_SHIFT:
		SetShift(!IsKeySet());
		break;

	case VK_MENU:
		SetAlt(!IsKeySet());
		break;

	case VK_BACK:
		// delete is delete if it is unshifted, or if in second position
		if (IsKeySet()) {
			if(IsChordSet())
				ResetChord();
			else
				ResetAll();
			break;
		}
		// DELIBERATE FALL THROUGH

	default:
#if 0
LDefault:
#endif
		if(!IsKeySet())
		{
			m_accel.key = (WORD)nChar;
		}
		else	// create chord
		{
			ASSERT(!IsChordSet());
			m_chord.key = (WORD)nChar;
		}
		break;

#if 0
	// used in V4.x to handle special characters
	case VK_RETURN:
	case VK_SPACE:
		if (!IsAlt() && !IsControl() && !IsShift())
			goto LDefault;
		// FALL THROUGH

	case VK_TAB:
	case VK_ESCAPE:
	case VK_F1:
		break;			// suppress these keys, as they can't be used in valid accels
#endif

	}
	InvalidateContent();
	NotifyParent(TAPN_CHANGE);
}

void CTap::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt > 1)
		return;

	switch(nChar)
	{
		case VK_CONTROL:
			ClearControl(!IsKeySet());
			break;
		case VK_SHIFT:
			ClearShift(!IsKeySet());
			break;
		case VK_MENU:
			ClearAlt(!IsKeySet());
			break;
	}
	InvalidateContent();
	NotifyParent(TAPN_CHANGE);
	
	CWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CTap::OnSysCommand(UINT nID, LPARAM lParam)
{
	if((nID == SC_KEYMENU) || (nID == SC_HOTKEY))
		return;
	
	CWnd::OnSysCommand(nID, lParam);
}

LRESULT CTap::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	ASSERT(m_accel.fVirt & FALT);	// if this isn't true,
	ASSERT(m_accel.key == nChar);	// I suppose I should make it true

	// tell windows we are eating the character
	// however, to do that, we must provide a dlgitem, so how about ourselves
	return MAKELRESULT(GetDlgCtrlID(),2);
}

void CTap::DrawFrame( CDC &dc, LPCRECT lprc )
{
	// FUTURE: Remove this function.
	ASSERT(FALSE);
}


void CTap::OnPaint()
{
	CString strKey = MakeAccelName(m_accel);
	if (IsChordSet())
	{
		strKey += ", ";
		strKey += MakeAccelName(m_chord);
	}

	CRect rc;
	GetClientRect(&rc);
	CPaintDC dc(this);

	dc.Draw3dRect(rc, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
	rc.InflateRect(-1, -1);
	dc.Draw3dRect(rc, GetSysColor(COLOR_WINDOWFRAME), GetSysColor(COLOR_BTNFACE));
	rc.InflateRect(-1, -1);

	CFont* pOldFont = dc.SelectObject(GetStdFont(font_Normal));

	dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkColor(GetSysColor(COLOR_WINDOW));
	dc.ExtTextOut(rc.left + 2, rc.top + 1, ETO_OPAQUE | ETO_CLIPPED, rc, strKey, strKey.GetLength(), NULL);

	if (GetFocus() == this)
	{
		CPoint pt;
		pt.x = rc.left + 2;
		pt.y = rc.top + 1;

		if (!strKey.IsEmpty())
			pt.x += dc.GetTextExtent(strKey, strKey.GetLength()).cx;
		SetCaretPos(pt);
	}

	dc.SelectObject(pOldFont);
}

CString CTap::MakeAccelName(ACCEL& accel)
{
	CString str;

	if(accel.fVirt & FCONTROL)
		str += CKeymap::c_szCtrl;
	if(accel.fVirt & FALT)
		str += CKeymap::c_szAlt;
	if(accel.fVirt & FSHIFT)
		str += CKeymap::c_szShift;

	if(accel.key != 0)
	{
		if(accel.fVirt & FVIRTKEY)
		{
			const char* psz = GetNameFromVK(accel.key);
			if(psz != NULL)
				str += psz;
			else
				str += (char)accel.key;
		}
		else
			str += (char)accel.key;
	}

	return str;
}

// FUTURE: VkToAscii is not currently used anywhere.  But this is such beastly
//   code, that I figured I'd hang onto it in case I needed it again -- DG.

WORD CTap::VkToAscii(UINT nChar, UINT nFlags, ACCEL& accel) const
{
	WORD wTransKey = 0;	// was a DWORD in Win16
	UINT oemScan = LOBYTE(nFlags);
	
	BYTE kbdState[256] = {0};
//	memset(kbdState, 0, 256);
	kbdState[oemScan] |= 0x80;

	if(accel.fVirt & FSHIFT)
		kbdState[VK_SHIFT] |= 0x80;
	if(accel.fVirt & FCONTROL)
		kbdState[VK_CONTROL] |= 0x80;
	
	int fAscii = ToAscii(nChar, oemScan, kbdState, &wTransKey, 0);
	ASSERT(!(DWORD)fAscii == !wTransKey);		// they should both be 0, or both non-0

	if(fAscii = 1)
		return wTransKey;
	else if (fAscii = 2)
	{
		ASSERT(FALSE);	// FUTURE: I need to handle this case too.
		return 0;
	}
	else
		return 0;
}

BOOL CTap::IsAccelLegal() const
{
	return TRUE;
#if 0
		// These were used in V4.x to handle special cases
	if (!IsKeySet())
		return TRUE; // Empty key is building towards legal accel

	if (IsVirtKey())
	{
		switch (m_accel.key)
		{
		case VK_TAB:
			return FALSE;

		case VK_F1:
			return FALSE;

		case VK_F2:
			if (IsAlt())
				return FALSE;
			break;

#if 0	// olympus 10464 : kherold per suggestion of rkern.  The user has other
		// ways to shut down the app, etc.
		case VK_F4:
			if (IsAlt() != IsControl() && !IsShift())
				return FALSE;
			break;
#endif

		case VK_F6:
			if (!IsAlt() && IsControl())
				return FALSE;
			break;

		case VK_DELETE:
			if (!IsBase() && !IsShift() && IsControl() && IsAlt())
				return FALSE;
			break;

		case VK_RETURN:
			if (!IsShift() && !IsControl() && !IsAlt())
				return FALSE;
			break;

		case VK_ESCAPE:
			return FALSE;
		}
	}

	return TRUE;
#endif

/*			
	switch (m_accel.key)
	{
	case VK_F2:
		// can have any F2 except Alt+F2
		return IsVirtKey() && (IsAlt() ? (IsShift() || IsControl()) : TRUE);
		
	case VK_F4:
		// Can have any F4 except Alt+F4 and Ctrl+F4
		return IsVirtKey() && (IsAlt() == IsControl() || IsShift());
		
	case VK_INSERT:
	case VK_ADD:
	case VK_SUBTRACT:
	case VK_MULTIPLY:
	case VK_DIVIDE:
		// can have any Insert, or Numpad /,*,-,+
		return IsVirtKey();
	
	case VK_DELETE:
		// can have any shifted Delete except Ctrl+Alt+Del
		return IsVirtKey() && (IsBase() || IsShift() ||
			(!IsControl() || !IsAlt()));
	}
	
	// Invalid F2-F12 were handled in the switch above
	if (m_accel.key >= VK_F2 && m_accel.key <= VK_F12)
		return IsVirtKey();
	
	// can have any Alt or Control modified A-Z, 0-9, or Space
	if ((m_accel.key >= 'A' && m_accel.key <= 'Z') ||
		(m_accel.key >= '0' && m_accel.key <= '9') ||
		(m_accel.key == VK_SPACE))
		return IsAlt() || IsControl();
	
	// [Alt, Ctrl] + [Numpad0 - Numpad9]
	if (m_accel.key >= VK_NUMPAD0 && m_accel.key <= VK_NUMPAD9)
		return IsVirtKey() && (IsAlt() || IsControl());
	
	// everything else is NOT legal for customized Accelerators
	return FALSE;
*/
}

BOOL CTap::IsChordLegal() const
{
	return TRUE;
/*
	// This wasn't even used in 4.x
	// Looser rules on second characters, because no shifting
	
	if(!IsChordSet())
		return TRUE; // No chord is certainly legal
	
	if( (m_chord.key >= VK_F2) && (m_chord.key <= VK_F12) )          
		return IsVirtKeyC();								// F2 - F12
			
	if( (m_chord.key >= 'A' && m_chord.key <= 'Z') ||
		(m_chord.key >= '0' && m_chord.key <= '9') ||
		(m_chord.key == VK_SPACE) )
		return TRUE;										// A-Z, 0-9, Space
			
	if( (m_chord.key == VK_INSERT) ||
		(m_chord.key == VK_DELETE) ||
		(m_chord.key == VK_ADD) ||
		(m_chord.key == VK_SUBTRACT) ||
		(m_chord.key == VK_MULTIPLY) ||
		(m_chord.key == VK_DIVIDE) )
		return IsVirtKeyC();								// Insert, Delete, or Numpad /,*,-,+

	return FALSE; // everything else is NOT legal for customized Accelerators' chords
*/
}

void CTap::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CTap::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	
	HideCaret();
	::DestroyCaret();
	if (!IsKeySet())
	{
		ClearControl( TRUE );
		ClearAlt( TRUE );
		ClearShift( TRUE );
	}
	Invalidate(FALSE);	// so as to redraw border

	imeEnableIME( NULL, TRUE );
}

void CTap::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	
//	UpdateStateFlags();

	CRect rc;
	GetWindowRect(&rc);
	CreateSolidCaret(1, rc.Height() - 6);
	::SetCaretPos(4, 3);
	ShowCaret();

	Invalidate(FALSE);	// so as to redraw border

	imeEnableIME( NULL, FALSE );
}

void CTap::UpdateStateFlags()
{
	BYTE kbdState[256] = {0};		// while focus was away, we probably got out of sync
	GetKeyboardState(kbdState);		// so get m_fState back in sync

	m_fState = FVIRTKEY;
	if(kbdState[VK_SHIFT] & 0x80)
		SetShift(FALSE);
	if(kbdState[VK_CONTROL] & 0x80)
		SetControl(FALSE);
	if(kbdState[VK_MENU] & 0x80)
		SetAlt(FALSE);

	if(!IsKeySet())
	{
		ResetAll();		// might as well do sync all at once
		Invalidate(FALSE);
		NotifyParent(TAPN_CHANGE);
	}
}

CWnd *GetTabWnd( const CWnd *pStartingControl, UINT nCmd )
{
	CWnd *pWnd = (CWnd *)pStartingControl;

	do
	{
		pWnd = pWnd->GetWindow(nCmd);
	} while ( pWnd && ((pWnd->GetStyle() & (WS_TABSTOP | WS_DISABLED)) != WS_TABSTOP) );
	return pWnd;
}

void CTap::SymTab(void) const
{
	// simulate the parent dialog tabbing around controls
	CWnd* pNext;
	if(m_fState & FSHIFT)	// shift+tab to previous control
		pNext = GetTabWnd( this, GW_HWNDPREV );
	else
		pNext = GetTabWnd( this, GW_HWNDNEXT );

	if(pNext != NULL)
		pNext->SetFocus();
}

#if 0
// These are no longer required, as the keys that go with these commands 
// are allowed to be customised
void CTap::SymExit() const
{
	GetParent()->SendMessage(WM_CLOSE,0,0);
}

void CTap::SymOk() const
{
	UINT idDef = ((CDialog*)GetParent())->GetDefID();
	CWnd* pDef = GetDlgItem(idDef);
	if (pDef != NULL)
		GetParent()->SendMessage(WM_COMMAND,MAKELPARAM(idDef,BN_CLICKED),(LPARAM)pDef->m_hWnd);
}
#endif

/////////////////////////////////////////////////////////////////////////////

static VirtKey VKList[] =
{
    VK_LBUTTON,    _T("LBUTTON"),	
    VK_RBUTTON,    _T("RBUTTON"),	
    VK_CANCEL,     _T("Break"),	
    VK_MBUTTON,    _T("MBUTTON"),	
    VK_BACK,       _T("Backspace"),	
    VK_TAB,        _T("Tab"),		
    VK_CLEAR,      _T("NumPad Clear"),
    VK_RETURN,     _T("Enter"),	
    VK_SHIFT,      _T("SHIFT"),	
    VK_CONTROL,    _T("CONTROL"),	
    VK_MENU,       _T("ALT"),	
    VK_PAUSE,      _T("Pause"),	
    VK_CAPITAL,    _T("Caps Lock"),	
    VK_ESCAPE,     _T("Esc"),	
    VK_SPACE,      _T("Space"),	
    VK_PRIOR,      _T("Page Up"),	
    VK_NEXT,       _T("Page Down"),	
    VK_END,        _T("End"),		
    VK_HOME,       _T("Home"),	
    VK_LEFT,       _T("Left Arrow"),	
    VK_UP,         _T("Up Arrow"),		
    VK_RIGHT,      _T("Right Arrow"),	
    VK_DOWN,       _T("Down Arrow"),	
    VK_SELECT,     _T("Select"),	
    VK_PRINT,      _T("Print"),	
    VK_EXECUTE,    _T("Execute"),	
    VK_SNAPSHOT,   _T("Snapshot"),	
    VK_INSERT,     _T("Ins"),	
    VK_DELETE,     _T("Del"),	
    VK_HELP,       _T("Help"),	
    VK_NUMPAD0,    _T("Num 0"),
    VK_NUMPAD1,    _T("Num 1"),	
    VK_NUMPAD2,    _T("Num 2"),	
    VK_NUMPAD3,    _T("Num 3"),	
    VK_NUMPAD4,    _T("Num 4"),	
    VK_NUMPAD5,    _T("Num 5"),	
    VK_NUMPAD6,    _T("Num 6"),	
    VK_NUMPAD7,    _T("Num 7"),	
    VK_NUMPAD8,    _T("Num 8"),	
    VK_NUMPAD9,    _T("Num 9"),	
    VK_MULTIPLY,   _T("Num *"),	
    VK_ADD,        _T("Num +"),		
    VK_SEPARATOR,  _T("Separator"),
    VK_SUBTRACT,   _T("Num -"),
    VK_DECIMAL,    _T("Num ."),
    VK_DIVIDE,     _T("Num /"),
    VK_F1,         _T("F1"),
    VK_F2,         _T("F2"),
    VK_F3,         _T("F3"),
    VK_F4,         _T("F4"),
    VK_F5,         _T("F5"),
    VK_F6,         _T("F6"),
    VK_F7,         _T("F7"),
    VK_F8,         _T("F8"),
    VK_F9,         _T("F9"),
    VK_F10,        _T("F10"),
    VK_F11,        _T("F11"),		
    VK_F12,        _T("F12"),		
    VK_F13,        _T("F13"),		
    VK_F14,        _T("F14"),		
    VK_F15,        _T("F15"),		
    VK_F16,        _T("F16"),		
    VK_F17,        _T("F17"),		
    VK_F18,        _T("F18"),		
    VK_F19,        _T("F19"),		
    VK_F20,        _T("F20"),
    VK_F21,        _T("F21"),		
    VK_F22,        _T("F22"),		
    VK_F23,        _T("F23"),		
    VK_F24,        _T("F24"),		
    VK_NUMLOCK,    _T("Num Lock"),	
    VK_SCROLL,     _T("Scroll Lock"),
    VK_LWIN,       _T("Left Windows"),
    VK_RWIN,       _T("Right Windows"),
    VK_APPS,       _T("Application"),
	0,			   _T("")
};

const char* GetNameFromVK(WORD n)
{
	static TCHAR szBuf [40];

	if (n >= 'A' && n <= 'Z' || n >= '0' && n <= '9')
	{
		szBuf[0] = (TCHAR)n;
		szBuf[1] = '\0';
		return szBuf;
	}

	int vk = 0;
	do
	{
		if(VKList[vk].key == n)
			return VKList[vk].name;
	}
	while(VKList[vk++].key != 0);

	// If we don't have a name for it, try the keyboard driver...
	GetKeyNameText(MAKELONG(0, MapVirtualKey(n, 0)), 
		szBuf, sizeof (szBuf) / sizeof (TCHAR) - 1);
	return szBuf;
}

// advances iOffset over any whitespace until we hit non-whitespace, or end of string
static void SkipWhitespace(LPCTSTR pszName, int *iOffset)
{
	ASSERT(pszName!=NULL);
	ASSERT(iOffset!=NULL);
	ASSERT(iOffset>=0);

	TCHAR c;

	while(c=pszName[*iOffset], c!=(TCHAR)0 && _istspace(c))
	{
		++(*iOffset);
	}
}

// returns true (and advances iOffset) if pszName begins with pszMatch.
static BOOL BeginsWith(LPCTSTR pszName, int *iOffset, LPCTSTR pszMatch)
{
	ASSERT(pszName!=NULL);
	ASSERT(iOffset!=NULL);
	ASSERT(iOffset>=0);
	ASSERT(pszMatch!=NULL);

	int iNewOffset=*iOffset;

	int nMatchLen=_tcslen(pszMatch);

	// though everything does, conceptually, begin with nothing, it's more likely that
	// this is an indication of a coding bug higher up
	ASSERT(nMatchLen!=0);

	SkipWhitespace(pszName, &iNewOffset);

	// does it begin with match?
	if(_tcsnicmp(pszName+iNewOffset, pszMatch, nMatchLen)==0)
	{
		// skip it
		*iOffset=iNewOffset+nMatchLen;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// returns true (and advances iOffset) if pszName begins with pszMatch, followed by a plus sign,
// ignoring spaces.
static BOOL BeginsAndPlus(LPCTSTR pszName, int *iOffset, LPCTSTR pszMatch)
{
	ASSERT(pszName!=NULL);
	ASSERT(iOffset!=NULL);
	ASSERT(iOffset>=0);
	ASSERT(pszMatch!=NULL);

	int iNewOffset=*iOffset;

	if(BeginsWith(pszName, &iNewOffset, pszMatch))
	{
		// skip following spaces
		SkipWhitespace(pszName, &iNewOffset);

		// is there a plus?
		if(pszName[iNewOffset]==_T('+'))
		{
			*iOffset=iNewOffset+1;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}


WORD GetKeyCodeFromName(LPCTSTR pszName, int *iOffset)
{
	ASSERT(pszName!=NULL);
	ASSERT(iOffset!=NULL);
	ASSERT(iOffset>=0);

	WORD wKeyCode=0;
	int iNewOffset=*iOffset;

	// check for prefixes
	// The spec states that these *must* appear in the order ctrl, alt, shift

	// BUGBUG DevStudio 96 Bug 6242...This code used BeginAndPlus, however it was passing in
	// CKeymap::c_szCtrl etc which already have the plus. We would like to allow whitespace
	// between the plus, so we will remove the plus before we pass in the string.

	// Make sure that the last character is a +...otherwise the code may break.
	ASSERT(CKeymap::c_szCtrl[_tcslen(CKeymap::c_szCtrl)-1] ==_T('+')) ;
	ASSERT(CKeymap::c_szAlt[_tcslen(CKeymap::c_szAlt)-1] ==_T('+')) ;
	ASSERT(CKeymap::c_szShift[_tcslen(CKeymap::c_szShift)-1] ==_T('+')) ;

	CString strPrefix(CKeymap::c_szCtrl) ;	
	if(BeginsAndPlus(pszName, &iNewOffset, strPrefix.SpanExcluding(_T("+")))) 
	{
		wKeyCode|=KCF_CONTROL; 
	}

	strPrefix = CKeymap::c_szAlt ;
	if(BeginsAndPlus(pszName, &iNewOffset, 	strPrefix.SpanExcluding(_T("+"))))
	{
		wKeyCode|=KCF_ALT;
	}

	strPrefix = CKeymap::c_szShift ;
	if(BeginsAndPlus(pszName, &iNewOffset, 	strPrefix.SpanExcluding(_T("+"))))
	{
		wKeyCode|=KCF_SHIFT;
	}

	SkipWhitespace(pszName, &iNewOffset);

	// can't end yet!
	if(pszName[iNewOffset]==_T('\0'))
	{
		return 0;
	}

	// check for a virtual key
	int vk = 0;
	BOOL bFound=FALSE;
	while(VKList[vk].key!=0 && !bFound)
	{
		ASSERT(VKList[vk].name!=NULL);

		if(BeginsWith(pszName, &iNewOffset, VKList[vk].name))
		{
			bFound=TRUE;
		}
		else
		{
			// BUGBUG DevStudio 96 Bug 6242...the index was getting incremented,
			// after the key was found.
			++vk; 
		}
	}

	if(bFound)
	{
		// found a virtkey
		wKeyCode+=VKList[vk].key;
	}
	else
	{
		// assume that it's an ordinary kind of key
		if(pszName[iNewOffset]!=_T('\0'))
		{
			wKeyCode+=pszName[iNewOffset];
			iNewOffset++;
			bFound=TRUE;
		}
	}

	if(bFound)
	{
		// at this point, we have a single valid keystroke, but we need to check that there isn't 
		// unexpected garbage at the end of it. We can tolerate whitespace, end of string markers, 
		// and anything following a comma
		SkipWhitespace(pszName, &iNewOffset);

		if(pszName[iNewOffset]==_T(','))
		{
			// skip the comma, so parsing can continue just after
			++iNewOffset;

			// we must ensure that the comma is followed by something
			SkipWhitespace(pszName, &iNewOffset);

			// we can now accept anything except a string terminator, which implies a 
			// comma followed by nothing
			if(pszName[iNewOffset]==_T('\0'))
			{
				bFound=FALSE;
			}
		}
		else
		{
			// without the comma, the only thing we will allow is end of string
			if(pszName[iNewOffset]!=_T('\0'))
			{
				bFound=FALSE;
			}
		}
	}

	if(bFound)
	{
		*iOffset=iNewOffset;
		return wKeyCode;
	}
	else
	{
		return 0;
	}
}

void CTap::NotifyParent(UINT idNotify)
{
#ifdef _WIN32
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM((WORD)GetDlgCtrlID(), (WORD)idNotify), (LPARAM)m_hWnd);
#else
	GetParent()->SendMessage(WM_COMMAND, (WPARAM)GetDlgCtrlID(), MAKELONG((WORD)m_hWnd, idNotify));
#endif
}
