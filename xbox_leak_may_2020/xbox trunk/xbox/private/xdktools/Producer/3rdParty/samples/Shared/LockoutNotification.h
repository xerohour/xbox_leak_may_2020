/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#if !defined(AFX_LOCKOUTNOTIFICATION_H__8B971CDC_E747_11D0_BC07_00A0C922E6EB__INCLUDED_)
#define AFX_LOCKOUTNOTIFICATION_H__8B971CDC_E747_11D0_BC07_00A0C922E6EB__INCLUDED_

// CLockoutNotification class

class CLockoutNotification
{
public:
	CLockoutNotification( HWND hWnd)
	{
		// prevent control notifications from being dispatched
		m_pThreadState = AfxGetThreadState();
		m_hWndOldLockout = m_pThreadState->m_hLockoutNotifyWindow;
		m_fReset = FALSE;
		if( m_hWndOldLockout != hWnd )
		{
			m_fReset = TRUE;
			m_pThreadState->m_hLockoutNotifyWindow = hWnd;
		}
	}
	~CLockoutNotification()
	{
		// Reinstate control notifications
		if( m_fReset )
		{
			m_pThreadState->m_hLockoutNotifyWindow = m_hWndOldLockout;
		}
	}

	BOOL				m_fReset;
	_AFX_THREAD_STATE*	m_pThreadState;
	HWND				m_hWndOldLockout;

};

#endif // !defined(AFX_LOCKOUTNOTIFICATION_H__8B971CDC_E747_11D0_BC07_00A0C922E6EB__INCLUDED_)
