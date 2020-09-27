#if !defined(STATUSTOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_00AA00C08146__INCLUDED_)
#define STATUSTOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "resource.h"
#include "DMUSProd.h"

extern class CConductor* g_pconductor;
interface IDirectMusicPerformance;
interface IPChannelName;

class CMyButton :
	public CBitmapButton
{
public:
	CMyButton( DWORD dwPChannel );
	~CMyButton();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	BOOL	m_fDown;
	BYTE	m_bLevel;
	DWORD	m_dwChannel;
	BOOL	m_fNoteOn;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyButton)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CMyButton();

	static long m_lFontRefCount;
	static HFONT m_hFont;
//	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
};

/////////////////////////////////////////////////////////////////////////////
// CStatusToolbarHandler definition

class CStatusToolbarHandler :
	public CComObjectRoot,
	public CWindowImpl<CStatusToolbarHandler>,
	public IDMUSProdToolBar8
{
friend class CConductor;
friend class COutputTool;

public:
	CStatusToolbarHandler();
	~CStatusToolbarHandler();

public:
	void EnableTimer( BOOL fEnable );

public:
	BEGIN_COM_MAP(CStatusToolbarHandler)
		COM_INTERFACE_ENTRY(IDMUSProdToolBar)
		COM_INTERFACE_ENTRY(IDMUSProdToolBar8)
	END_COM_MAP()

	// IDMUSProdToolBar functions
	HRESULT STDMETHODCALLTYPE GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle );
	HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
	HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
	HRESULT STDMETHODCALLTYPE Initialize( HWND hWndToolBar );

	// IDMUSProdToolBar8 function
	HRESULT STDMETHODCALLTYPE ShowToolBar( BOOL bShowToolBar );

	// message map
	BEGIN_MSG_MAP(CStatusToolbarHandler)
	//BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)
		//TRACE("StatusToolbarMsg: %x %x %x\n", uMsg, wParam, lParam );
		// This code fixes 18125.  It's a hack, but it works.
		if( uMsg == WM_NOTIFY )
		{
			if( m_nAfterWindowPosChanged )
			{
				if( m_nAfterWindowPosChanged == 1)
				{
					//TRACE("Notify: %x %x %x\n", ((LPNMHDR)lParam)->hwndFrom, ((LPNMHDR)lParam)->idFrom, ((LPNMHDR)lParam)->code );
					::PostMessage(m_hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(50, 22));
				}
				m_nAfterWindowPosChanged--;
			}
		}
#pragma warning ( push )
#pragma warning ( disable : 4244 )
		NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnSize)
#pragma warning ( pop )
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
		COMMAND_HANDLER(ID_TRANSP_PANIC, BN_CLICKED, OnPanicClicked)
#pragma warning ( push )
#pragma warning ( disable : 4244 )
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnRightClick)
#pragma warning ( pop )
		COMMAND_HANDLER(ID_TRANSP_PANIC, BN_DOUBLECLICKED, OnPanicDblClicked)
		COMMAND_CODE_HANDLER(ID_ADD_BUTTON, OnAddButton)
		COMMAND_CODE_HANDLER(BN_CLICKED, OnClicked)
		COMMAND_CODE_HANDLER(BN_DOUBLECLICKED, OnDblClicked)
	END_MSG_MAP()

	// message handler methods
	LRESULT OnDrawItem(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& lResult);
	LRESULT OnWindowPosChanged(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& lResult);
	LRESULT OnAddButton( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnClicked( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnDblClicked( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnSize( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );
	LRESULT OnPanicClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );
	LRESULT OnPanicDblClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );

	void RefreshAllButtons();

	// Base class override methods
static CWndClassInfo& GetWndClassInfo();

	CTypedPtrMap< CMapWordToPtr, WORD, CMyButton*> m_mpButtonStatus;

protected:
	HRESULT SendMIDIMessage( IDirectMusicPerformance *pPerformance, DWORD dwPChannel, BYTE bStatus, BYTE bByte1 );
	void RemoveAllStatusButtons( void );

private:
	HWND			m_hWndToolbar;
	UINT			m_nTimerID;
	int				m_nAfterWindowPosChanged;
};

void CALLBACK EXPORT TimerProc( HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );

#endif // !defined(STATUSTOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_00AA00C08146__INCLUDED_)
