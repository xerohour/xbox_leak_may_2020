#if !defined(AFX_EFFECTLISTDLG_H__8F60957D_55B7_4513_81AC_ADB50E6C6910__INCLUDED_)
#define AFX_EFFECTLISTDLG_H__8F60957D_55B7_4513_81AC_ADB50E6C6910__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffectListDlg.h : header file
//

#include "resource.h"
#include "DMUSProd.h"
#include "EffectInfo.h"
#include "TreeDropTarget.h"
#include "EffectListCtl.h"

class CAudioPathCtrl;
class CDirectMusicAudioPath;

/////////////////////////////////////////////////////////////////////////////
// CEffectListDlg dialog

class CEffectListDlg : public CDialog
{
friend CAudioPathCtrl;
// Construction
public:
	CEffectListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectListDlg();

// Dialog Data
	//{{AFX_DATA(CEffectListDlg)
	enum { IDD = IDD_DLG_EFFECTLIST };
	CEffectListCtl	m_listEffects;
	//}}AFX_DATA

public:
	CAudioPathCtrl*		m_pAudioPathCtrl;
	CDirectMusicAudioPath*	m_pAudioPath;

	void RefreshControls();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );


// Implementation
protected:
	int FindEffectInfoIndex( const EffectInfo *pEffectInfo );
	bool AnyEffectsSelected( void );
	void AddEffectToList( EffectInfo *pEffectInfo );
	void EmptyEffectList( void );
	HRESULT	CreateDataObject( IDataObject** ppIDataObject );

	// Generated message map functions
	//{{AFX_MSG(CEffectListDlg)
	afx_msg void OnBegindragListEffects(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditInsert();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnSetfocusListEffects(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTLISTDLG_H__8F60957D_55B7_4513_81AC_ADB50E6C6910__INCLUDED_)
