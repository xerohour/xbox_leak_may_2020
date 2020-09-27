#if !defined(AFX_AUDIOPATHDLG_H__FECD4365_2E75_47E3_BB7F_3195F9A9CDE3__INCLUDED_)
#define AFX_AUDIOPATHDLG_H__FECD4365_2E75_47E3_BB7F_3195F9A9CDE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioPathDlg.h : header file
//

#include "TreeDropTarget.h"
#include "MultiTree.h"
#include "resource.h"
#include "MixGroupPPGMgr.h"
#include "BufferPPGMgr.h"
#include "EffectInfo.h"
#include "OlePropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CAudioPathDlg dialog

class CDirectMusicAudioPath;
class CAudioPathCtrl;
class CEffectListDlg;
class CMultiTree;

int CALLBACK ComparePChannelsInTree(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
void PChannelArrayToString( DWORD adwPChannels[], int nNumPChannels, CString &strText );
void BusListToString( const ItemInfo *pItem, const BufferOptions *pBufferOptions, CString &strText );
bool IsItemEnvReverb( const ItemInfo *pItemInfo );

class CAudioPathDlg : public CDialog, public IDMUSProdPropPageObject, public IDropSource, public IDMUSProdUpdateObjectNow
{
friend CAudioPathCtrl;
friend CEffectListDlg;
friend CTreeDropTarget;
friend CMultiTree;

// Construction
public:
	CAudioPathDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAudioPathDlg();

	// Additional methods
public:
	void RefreshControls();
	BOOL OnViewProperties();

	HRESULT DropOnTree( IDataObject* pIDataObject, DROPEFFECT dropEffect, POINT point);

	void InsertPChannel( POINT pointInsert );
	void InsertEnvReverb();
	void PastePChannelFromClipboard( POINT pointPaste );
	void PasteEffectFromClipboard( POINT pointPaste );
	void UpdateTreeItemName( const ItemInfo *pItemInfo );

	void SwitchToCorrectPropertyPage( void );

// Operations
public:
    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// IDMUSProdUpdateObjectNow functions
    HRESULT STDMETHODCALLTYPE UpdateObjectNow(LPUNKNOWN punkObject);

// IDropSource
	HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );

// Private attributes
private:
	long		m_cRef;
	bool		m_fEnteredSwitchTo;

public:
	CAudioPathCtrl*		m_pAudioPathCtrl;
	CDirectMusicAudioPath*	m_pAudioPath;

protected:
	CTreeDropTarget m_CTreeDropTarget;

	MixGroupInfoForPPG	m_MixGroupInfoForPPG;
	BufferInfoForPPG	m_BufferInfoForPPG;
	EffectInfoForPPG	m_EffectInfoForPPG;

	IDataObject *m_pISourceDataObject;
	DWORD		m_dwStartDragButton;

	void InsertPChannelIntoItem( ItemInfo *pItemInfo, DWORD dwPChannel );
	void DeletePChannel( DWORD dwPChannel );
	void UpdateTreeItemNames( void );
	void InsertItemIntoTree( ItemInfo *pItem );
	void PastePChannels( IStream *pIStream, POINT pointPaste );
	int FindItem( const ItemInfo *pItemInfo, const BufferOptions *pBufferOptions );
	bool HasEnvReverb( void );
	EffectInfo *CreateNewEffectInfoIfNecessary( EffectInfo *pEffectInfoToChange );
	bool GetColumnWidths( long &lMixGroup, long &lBus, long &lBuffer );
	void ResizeEffectColumn( void );
	int GetMinEffectWidth( void );

	void SwitchToMixGroupPPG( void );
	void SwitchToBufferPPG( void );
	void SwitchToEffectPPG( void );

	void UpdateMixGroupForPPG( void );
	void UpdateBufferForPPG( void );
	void UpdateEffectForPPG( void );

	//void SetI3DL2SrcDestinationIfNecessary( EffectInfo *pEffectInfoToChange );

	HRESULT SetData_MixGroup( MixGroupInfoForPPG *pMixGroupInfoForPPG );
	HRESULT SetData_Buffer( BufferInfoForPPG *pBufferInfoForPPG );
	HRESULT SetData_Effect( EffectInfoForPPG *pEffectInfoForPPG );

// Dialog Data
	//{{AFX_DATA(CAudioPathDlg)
	enum { IDD = IDD_DLG_AUDIOPATH };
	CMultiTree	m_tcTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioPathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAudioPathDlg)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTreePchannels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragTreePchannels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditInsert();
	afx_msg void OnEditPaste();
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	/*
	afx_msg void OnBeginTrackHeader(NMHDR* pNMHDR, LRESULT* pResult);
	*/
	afx_msg void OnEndTrackHeader(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDividerDblClickHeader(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOPATHDLG_H__FECD4365_2E75_47E3_BB7F_3195F9A9CDE3__INCLUDED_)
