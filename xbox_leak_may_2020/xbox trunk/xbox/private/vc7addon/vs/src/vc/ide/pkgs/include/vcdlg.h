
// std ATL includes MUST be included before this file

#ifndef _VCDLG_H_
#define _VCDLG_H_

/////////////////////////////////////////////////////////////////////////////
// Base class to handle Unicode-ness in dialogs
template <class T> 
class ATL_NO_VTABLE CVCAxDialogImpl :
	public CDialogImpl<T>
{
protected:
	// protected so that descendants can access
	bool m_bIsWin9x;

public:
	// construction
	CVCAxDialogImpl() : m_bIsWin9x( true )
	{
		// what OS are we running on?
		OSVERSIONINFO osver;
		memset( &osver, 0, sizeof(osver) );
		osver.dwOSVersionInfoSize = sizeof(osver);
		GetVersionEx( &osver );
		if( osver.dwPlatformId == VER_PLATFORM_WIN32_NT )
			m_bIsWin9x = false;
	}
	
	// modal dialogs
	INT_PTR DoModalW(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL)
	{
		ATLASSERT(m_hWnd == NULL);
		// NOTE: CWindow is used directly here because we don't have access to
		// the TBASE template arg that CAxDialogImpl does
		_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT<CWindow>*)this);
#ifdef _DEBUG
		m_bModal = true;
#endif //_DEBUG
		T* pT;
		pT = static_cast<T*>(this);
		if( m_bIsWin9x )
			return AtlAxDialogBox(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(pT->IDD),
					hWndParent, T::StartDialogProc, dwInitParam);
		else
			return AtlAxDialogBoxW(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCEW(pT->IDD),
					hWndParent, T::StartDialogProc, dwInitParam);
	}

	// modeless dialogs
	HWND CreateW(HWND hWndParent, LPARAM dwInitParam = NULL)
	{
		ATLASSERT(m_hWnd == NULL);
		// NOTE: CWindow is used directly here because we don't have access to
		// the TBASE template arg that CAxDialogImpl does
		_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT<CWindow>*)this);
#ifdef _DEBUG
		m_bModal = false;
#endif //_DEBUG
		T* pT;
		pT = static_cast<T*>(this);
		HWND hWnd;
		if( m_bIsWin9x )
			hWnd = AtlAxCreateDialog(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(pT->IDD),
					hWndParent, T::StartDialogProc, dwInitParam);
		else
			hWnd = AtlAxCreateDialogW(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCEW(pT->IDD),
					hWndParent, T::StartDialogProc, dwInitParam);
		ATLASSERT(m_hWnd == hWnd);
		return hWnd;
	}
};

#endif // include fence
