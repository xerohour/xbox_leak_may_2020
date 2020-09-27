#if !defined(AFX_OPTIONSTOOLBAR_H__D2A5D885_F918_11D2_894A_00C04FBF8D15__INCLUDED_)
#define AFX_OPTIONSTOOLBAR_H__D2A5D885_F918_11D2_894A_00C04FBF8D15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsToolbar.h : header file
//

#include "DMUSProd.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsToolbarHandler definition

class COptionsToolbarHandler :
	public CComObjectRoot,
	public CWindowImpl<COptionsToolbarHandler>,
	public IDMUSProdToolBar
{
friend class CConductor;

public:
	COptionsToolbarHandler();
	~COptionsToolbarHandler();

protected:
	BOOL SetBtnState( int nID, UINT nState );

public:
	BEGIN_COM_MAP(COptionsToolbarHandler)
		COM_INTERFACE_ENTRY(IDMUSProdToolBar)
	END_COM_MAP()

	// IDMUSProdToolBar functions
	HRESULT STDMETHODCALLTYPE GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle );
	HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
	HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
	HRESULT STDMETHODCALLTYPE Initialize( HWND hWndToolBar );

//		TRACE( "Message: %x %x %x\n", uMsg, wParam, lParam );
	// message map
	BEGIN_MSG_MAP(COptionsToolbarHandler)
		COMMAND_HANDLER(ID_TRANSP_MIDI, BN_CLICKED, OnMidiClicked)
		COMMAND_HANDLER(ID_TRANSP_OPT, BN_CLICKED, OnOptionsClicked)
		COMMAND_HANDLER(ID_TRANSP_METRONOME, BN_CLICKED, OnMetronomeClicked)
		COMMAND_HANDLER(ID_TRANSP_CURSOR, BN_CLICKED, OnCursorClicked)
		COMMAND_HANDLER(ID_TRANSP_AUDIOPATH, CBN_CLOSEUP, OnCloseUpComboAudiopath)
		COMMAND_HANDLER(ID_TRANSP_AUDIOPATH, CBN_DROPDOWN, OnDropDownComboAudiopath)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
#pragma warning ( push )
#pragma warning ( disable : 4244 )
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnRightClick)
#pragma warning ( pop )
	END_MSG_MAP()

	// message handler methods
	LRESULT OnMidiClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnOptionsClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnMetronomeClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnCursorClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled );
	LRESULT OnCloseUpComboAudiopath( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnDropDownComboAudiopath( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& lResult);

	// Helper methods
	void UpdateComboBoxFromConductor( void );

	// Base class override methods
static CWndClassInfo& GetWndClassInfo();

	CComboBox		m_comboAudiopath;
	BOOL			m_fCursorEnabled;

	long			m_lCountInBars;
	BOOL			m_fCountOnlyOnRecord;

private:
	HWND			m_hWndToolbar;
	CFont			m_font;
};

#endif // !defined(AFX_OPTIONSTOOLBAR_H__D2A5D885_F918_11D2_894A_00C04FBF8D15__INCLUDED_)
