#if !defined(AFX_SYNTHSTATUSTOOLBAR_H__9DA12B12_089B_11D3_A709_00105A26620B__INCLUDED_)
#define AFX_SYNTHSTATUSTOOLBAR_H__9DA12B12_089B_11D3_A709_00105A26620B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SynthStatusToolbar.h : header file
//

class CResetButton :
	public CButton
{
// Construction
public:
	CResetButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResetButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResetButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResetButton)
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	void UpdateStyle( void );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSynthStatusToolbar window

class CSynthStatusToolbar :
	public CComObjectRoot,
	public CWindowImpl<COptionsToolbarHandler>,
	public IDMUSProdToolBar
{
friend class CConductor;
friend void CALLBACK EXPORT SynthStatusTimerProc( HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );

// Construction
public:
	CSynthStatusToolbar();

// Implementation
public:
	virtual ~CSynthStatusToolbar();

public:
	BEGIN_COM_MAP(CSynthStatusToolbar)
		COM_INTERFACE_ENTRY(IDMUSProdToolBar)
	END_COM_MAP()

	// IDMUSProdToolBar functions
	HRESULT STDMETHODCALLTYPE GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle );
	HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
	HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
	HRESULT STDMETHODCALLTYPE Initialize( HWND hWndToolBar );

	// message map
	BEGIN_MSG_MAP(CSynthStatusToolbar)
		COMMAND_HANDLER(IDC_SYNTH_RESET, BN_CLICKED, OnResetClicked)
/*
#pragma warning ( push )
#pragma warning ( disable : 4244 )
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnRightClick)
#pragma warning ( pop )
*/
	END_MSG_MAP()

	// message handler methods
	LRESULT OnResetClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
//	LRESULT OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );

	// Operations
	void EnableTimer( BOOL fEnable );

	// Base class override methods
//static CWndClassInfo& GetWndClassInfo();

private:
	CFont			m_font;
	HWND			m_hWndToolbar;
	CStatic			m_staticCPUText;
	CStatic			m_staticVoicesText;
	CStatic			m_staticMemoryText;
	CStatic			m_staticLatencyText;
	CResetButton	m_buttonReset;
	int				m_nTimerID;
	DWORD			m_dwPeakCPU;
	DWORD			m_dwPeakVoices;
	REFERENCE_TIME	m_rtPeakCPU;
	REFERENCE_TIME	m_rtPeakVoices;
};

#endif // !defined(AFX_SYNTHSTATUSTOOLBAR_H__9DA12B12_089B_11D3_A709_00105A26620B__INCLUDED_)
