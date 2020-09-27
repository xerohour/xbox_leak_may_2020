#if !defined(SECONDARYTOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_6__INCLUDED_)
#define SECONDARYTOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <DMUSProd.h>
#include "resource.h"

interface IDirectMusicSegmentState;
interface IDMUSProdRIFFStream;
interface ISecondaryTransportRegEntry;

#define MAX_BUTTONS 99

struct ButtonState
{
	ButtonState()
	{
		pActiveRegEntry = NULL;
		dwSegFlags = DMUS_SEGF_DEFAULT;
	}
	virtual ~ButtonState()
	{
	}
	ISecondaryTransportRegEntry	*pActiveRegEntry;
	DWORD			dwSegFlags;
};

struct ButtonInfo : ButtonState
{
	ButtonInfo() : ButtonState()
	{
		fDisplayingPlay = TRUE;
		fWaitForStart = FALSE;
		pISegmentState = NULL;
		dwIndex = 0;
	}
	virtual ~ButtonInfo()
	{
		RELEASE(pISegmentState);
	}
	CComboBox		comboActive;
	CStatic			staticIdent;
	BOOL			fDisplayingPlay;
	BOOL			fWaitForStart;
	IDirectMusicSegmentState *pISegmentState;
	DWORD			dwIndex;
};

/////////////////////////////////////////////////////////////////////////////
// CSecondaryToolbarHandler definition

class CSecondaryToolbarHandler :
	public CComObjectRoot,
	public CWindowImpl<CSecondaryToolbarHandler>,
	public IDMUSProdToolBar
{
friend class CConductor;
friend UINT AFX_CDECL NotifyThreadProc( LPVOID pParam );

public:
	CSecondaryToolbarHandler();
	~CSecondaryToolbarHandler();

protected:
	BOOL SetBtnState( int nID, UINT nState );
	void UpdatePlayStopBtns( ButtonInfo *pButtonInfo );
	void AddButton( DWORD dwIndex );
	void SetNumButtons( DWORD dwNumButtons );
	void RemoveButton( DWORD dwIndex );

public:
	void SetStateAuto( void );
	void Activate( BOOL fActivate );

public:
	BEGIN_COM_MAP(CSecondaryToolbarHandler)
		COM_INTERFACE_ENTRY(IDMUSProdToolBar)
	END_COM_MAP()

	// IDMUSProdToolBar functions
	HRESULT STDMETHODCALLTYPE GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle );
	HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
	HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
	HRESULT STDMETHODCALLTYPE Initialize( HWND hWndToolBar );

	// message map
	BEGIN_MSG_MAP(CSecondaryToolbarHandler)
		COMMAND_CODE_HANDLER(ID_SET_STATE_AUTO, OnSetStateAuto)
		COMMAND_CODE_HANDLER(BN_CLICKED, OnButtonClicked)
		COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnSelchangeCombo)
		COMMAND_HANDLER(IDC_EDIT_SECONDARY, EN_CHANGE, OnChangeSecondaryEdit)
		//COMMAND_HANDLER(IDC_EDIT_SECONDARY, EN_KILLFOCUS, OnKillFocusSecondaryEdit)
#pragma warning ( push )
#pragma warning ( disable : 4244 )
		NOTIFY_HANDLER(IDC_SPIN_SECONDARY, UDN_DELTAPOS, OnDeltaPosSecondary)
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnRightClick)
#pragma warning ( pop )
	END_MSG_MAP()

	// message handler methods
	LRESULT OnButtonClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnSelchangeCombo( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnPlayClicked( WORD wID );
	LRESULT OnStopClicked( WORD wID );
	//LRESULT OnKillFocusSecondaryEdit( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnChangeSecondaryEdit( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnDeltaPosSecondary( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );

	LRESULT OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );
	LRESULT OnShowBoundaryOptions( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );

	LRESULT OnSetStateAuto( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled );
	HRESULT Save( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO *pckMain );

	void SetActiveTransport( CComboBox &comboActive, IDMUSProdSecondaryTransport*  pISecondaryTransport, ISecondaryTransportRegEntry *& pActiveRegEntry );

	void OnSegEndNotify( IUnknown *punkSegment );
	void OnSegStartNotify( IUnknown *punkSegment );

	// Base class override methods
static CWndClassInfo& GetWndClassInfo();

	DWORD			m_dwNumButtons;
	CTypedPtrArray<CPtrArray, ButtonInfo*> m_arrayButtons;
	ButtonInfo*		m_apOldButtonInfo[MAX_BUTTONS];

private:
	int				m_cxChar;
	int				m_cyChar;
	CSpinButtonCtrl m_spinControl;
	CEdit			m_editControl;
	//DWORD			m_dwSecondaryToolbarID;
	CFont			m_font;
	HWND			m_hWndToolbar;
};

#endif // !defined(SECONDARYTOOLBAR_H__36F6DDF43_46CE_11D0_B9DB_6__INCLUDED_)
