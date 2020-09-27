// tag.h : header file
//

#ifndef __TAP_H__
#define __TAP_H__

/////////////////////////////////////////////////////////////////////////////
// CTap control

class CTap : public CWnd
{
// Construction
public:
	CTap();

// Data members
protected:
	ACCEL m_accel;	// accelerator gleaned from the tap
	ACCEL m_chord;	// second accelerator to make chord - only key is used
	BYTE m_fState;	// state of shift keys, because m_accel can lose track

// Attributes
public:
	ACCEL GetAccel() const;
	ACCEL GetChord() const;

	BOOL IsKeySet() const;
	BOOL IsChordSet() const;
	BOOL IsShift() const;
	BOOL IsControl() const;
	BOOL IsAlt() const;
	BOOL IsBase() const;
	BOOL IsVirtKey() const;
	BOOL IsVirtKeyC() const;

	BOOL IsAccelLegal() const;
	BOOL IsChordLegal() const;

// Operations
public:
	void ResetAll();
	void ResetChord();
	void SetAccel(ACCEL& accel);
	void SetChord(ACCEL& chord);
	void InvalidateContent(BOOL bErase = TRUE);

	void SetShift(BOOL bAccel = TRUE);
	void SetControl(BOOL bAccel = TRUE);
	void SetAlt(BOOL bAccel = TRUE);
	void SetVirtKey(BOOL bAccel = TRUE);
	void SetAscii(BOOL bAccel = TRUE);
	void SetAccelState(BOOL bAccel = TRUE);
	void ClearShift(BOOL bAccel = TRUE);
	void ClearControl(BOOL bAccel = TRUE);
	void ClearAlt(BOOL bAccel = TRUE);

	void UpdateStateFlags();

	void NotifyParent(UINT idNotify);

// Implementation
public:
	virtual ~CTap();

	virtual BOOL Create(const char* szName, const RECT& rc, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage( MSG *pMsg );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CTap)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG

// Helper functions
public:
	static CString MakeAccelName(ACCEL& accel);
	WORD VkToAscii(UINT nChar, UINT nFlags, ACCEL& accel) const;
	BOOL SuppressKey(UINT nChar) const;

	void SymTab() const;
#if 0
	void SymExit() const;
	void SymOk() const;
#endif

	void DrawFrame( CDC &dc, LPCRECT lprc );
};


inline ACCEL CTap::GetAccel() const
	{	return m_accel;	}
	
inline ACCEL CTap::GetChord() const
	{	return m_chord;	}

inline BOOL CTap::IsKeySet() const
	{	return m_accel.key != 0;	}
	
inline BOOL CTap::IsChordSet() const
	{	return m_chord.key != 0;	}
	
inline BOOL CTap::IsShift() const
	{	return (m_accel.fVirt & FSHIFT) != 0;	}
	
inline BOOL CTap::IsControl() const
	{	return (m_accel.fVirt & FCONTROL) != 0;	}
	
inline BOOL CTap::IsAlt() const
	{	return (m_accel.fVirt & FALT) != 0;	}
	
inline BOOL CTap::IsBase() const
	{	return (m_accel.fVirt & (FCONTROL | FALT | FSHIFT)) == 0;	}
	
inline BOOL CTap::IsVirtKey() const
	{	return (m_accel.fVirt & FVIRTKEY) != 0;	}
	
inline BOOL CTap::IsVirtKeyC() const
	{	return (m_chord.fVirt & FVIRTKEY) != 0;	}

inline void CTap::SetShift(BOOL bAccel)
	{	m_fState |= FSHIFT;		SetAccelState(bAccel);	}
	
inline void CTap::SetControl(BOOL bAccel)
	{	m_fState |= FCONTROL;	SetAccelState(bAccel);	}
	
inline void CTap::SetAlt(BOOL bAccel)
	{	m_fState |= FALT;		SetAccelState(bAccel);	}
	
inline void CTap::SetVirtKey(BOOL bAccel)
	{	m_fState |= FVIRTKEY;	SetAccelState(bAccel);	}
	
inline void CTap::SetAscii(BOOL bAccel)
	{	m_fState &= ~FVIRTKEY;	SetAccelState(bAccel);	}
	
inline void CTap::ClearShift(BOOL bAccel)
	{	m_fState &= ~FSHIFT;	SetAccelState(bAccel);	}
	
inline void CTap::ClearControl(BOOL bAccel)
	{	m_fState &= ~FCONTROL;	SetAccelState(bAccel);	}
	
inline void CTap::ClearAlt(BOOL bAccel)
	{	m_fState &= ~FALT;		SetAccelState(bAccel);	}
	
inline void CTap::SetAccelState(BOOL bAccel)
	{	if(bAccel) m_accel.fVirt = m_fState;	}

	
// TAP control notifications
//
#define TAPN_CHANGE		0

/////////////////////////////////////////////////////////////////////////////

// CString MakeAccelName(ACCEL& accel);		// utility function also used outside of CTap

/////////////////////////////////////////////////////////////////////////////

typedef struct tagVirtKey
{
	WORD key;
	LPTSTR name;
} VirtKey;

LPCTSTR GetNameFromVK(WORD n);
// given a string containing the textual name of the key, this will convert it
// to a key code, and updated offset, which is the portion of the string that
// has been parsed. Returns 0 if there is no appropriate key code
WORD GetKeyCodeFromName(LPCTSTR pszKeyName, int *iOffset);

#endif	// __TAP_H__

