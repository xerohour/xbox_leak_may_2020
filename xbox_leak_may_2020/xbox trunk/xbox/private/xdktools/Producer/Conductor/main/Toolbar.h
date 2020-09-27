#if !defined(TOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_00AA00C08146__INCLUDED_)
#define TOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DMUSProd.h"
#include "resource.h"

interface IDMUSProdConductorToolbar : public IDMUSProdToolBar
{
	HWND Create( HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL, DWORD dwStyle = WS_CHILD | WS_VISIBLE, DWORD dwExStyle = 0, UINT nID = 0 );
};

class CMyEdit :
	public CEdit
{
	DECLARE_DYNCREATE(CMyEdit)
public:
	CMyEdit();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
};

class CRatioButton : public CButton
{
	DECLARE_DYNCREATE(CRatioButton)
public:
	CRatioButton();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////
// CToolbarHandler definition

class CToolbarHandler :
	public CComObjectRoot,
	public CWindowImpl<CToolbarHandler>,
	public IDMUSProdToolBar
{
friend class CConductor;
friend class CStatusToolbarHandler;
friend UINT AFX_CDECL NotifyThreadProc( LPVOID pParam );

public:
	CToolbarHandler();
	~CToolbarHandler();

protected:
	BOOL SetBtnState( int nID, UINT nState );
	BOOL SetState( DWORD dwFlags );
	void SetStateFromEngine( DWORD *pdwState );
	void PlayCountInSegment( IDMUSProdTransport* pITransport );
	void TransitionToNothing( void );

public:
	void UpdateButtonRatio( void );
	void UpdatePlayingFromEngine( void );
	void UpdateActiveFromEngine( void );
	void SetStateAuto( void );
	inline BOOL IsValid( ITransportRegEntry *pITransportRegEntry )
	{
		return ( pITransportRegEntry != NULL ) ? TRUE : FALSE;
	}
	void AddTransport( ITransportRegEntry *pITransportRegEntry );
	void RemoveTransport( ITransportRegEntry *pITransportRegEntry );
	void SetActiveTransport( ITransportRegEntry *pITransportRegEntry );

public:
	BEGIN_COM_MAP(CToolbarHandler)
		COM_INTERFACE_ENTRY(IDMUSProdToolBar)
	END_COM_MAP()

	// IDMUSProdToolBar functions
	HRESULT STDMETHODCALLTYPE GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle );
	HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
	HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
	HRESULT STDMETHODCALLTYPE Initialize( HWND hWndToolBar );

//		TRACE( "Message: %x %x %x\n", uMsg, wParam, lParam );
	// message map
	BEGIN_MSG_MAP(CToolbarHandler)
		COMMAND_HANDLER(ID_TRANSP_PLAY, BN_CLICKED, OnPlayClicked)
		COMMAND_HANDLER(ID_TRANSP_STOP, BN_CLICKED, OnStopClicked)
		COMMAND_HANDLER(ID_TRANSP_TRANSITION, BN_CLICKED, OnTransitionClicked)
		COMMAND_HANDLER(ID_TRANSP_RECORD, BN_CLICKED, OnRecordClicked)
		COMMAND_HANDLER(IDC_TRANSP_COMBO, CBN_SELCHANGE, OnSelchangeComboTransport)
		COMMAND_HANDLER(IDC_RELATIVE_TEMPO, BN_CLICKED, OnRelTempoClicked)
		COMMAND_HANDLER(IDC_TEMPO, EN_KILLFOCUS, OnKillFocusTempo)
		COMMAND_HANDLER(1, 0, OnKillFocusTempo)
		COMMAND_HANDLER(ID_TRANS_REWPLAY, BN_CLICKED, OnRewPlayClicked)
		COMMAND_HANDLER(ID_TRANS_STOPIMMEDIATE, BN_CLICKED, OnStopImmediateClicked)
		COMMAND_CODE_HANDLER(ID_SET_STATE_AUTO, OnSetStateAuto)
		COMMAND_CODE_HANDLER(ID_SET_ACTIVE_FROM_POINTER, OnSetActiveFromPointer)
		COMMAND_CODE_HANDLER(ID_MUSIC_STOPPED, OnMusicStopped)
		COMMAND_CODE_HANDLER(ID_PRESS_PLAY_BUTTON, OnPressPlayButton)
		COMMAND_CODE_HANDLER(ID_PRESS_STOP_BUTTON, OnPressStopButton)
#pragma warning ( push )
#pragma warning ( disable : 4244 )
		NOTIFY_HANDLER(IDC_SPIN_TEMPO, UDN_DELTAPOS, OnDeltaPosTempo)
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnRightClick)
#pragma warning ( pop )
	END_MSG_MAP()

	// message handler methods
	LRESULT OnPlayClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnStopClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnTransitionClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnRecordClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnSelchangeComboTransport( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnRelTempoClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnKillFocusTempo( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnDeltaPosTempo( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );
	LRESULT OnRewPlayClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnStopImmediateClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnSetStateAuto( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnSetActiveFromPointer( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnMusicStopped( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnPressPlayButton( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnPressStopButton( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	LRESULT OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );
	LRESULT OnParentNotify(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& lResult);

	// Base class override methods
static CWndClassInfo& GetWndClassInfo();

	CComboBox		m_comboActive;
	CRatioButton	m_buttonRatio;
	CSpinButtonCtrl m_spinTempo;
	CMyEdit			m_editTempo;
	BOOL			m_fInTransition;
	MUSIC_TIME		m_mtPlayTime;

	IDirectMusicSegmentState *m_pCountInSegmentState;

private:
	CFont			m_font;
	HWND			m_hWndToolbar;
	BOOL			m_fInOnBarButtonClicked;
	BOOL			m_fPlayFromStart;
	BOOL			m_fDontUpdateRewPlay;
	BOOL			m_fDisplayingPlay;
	BOOL			m_fStopImmediate;
};

#endif // !defined(TOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_00AA00C08146__INCLUDED_)
