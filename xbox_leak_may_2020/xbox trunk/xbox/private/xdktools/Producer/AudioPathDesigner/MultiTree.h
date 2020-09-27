#if !defined(AFX_MLTITREE_H__9F4750E5_C512_11D2_880A_02608C7CB3EE__INCLUDED_)
#define AFX_MLTITREE_H__9F4750E5_C512_11D2_880A_02608C7CB3EE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MltiTree.h : header file
// Original code Copyright (c) 1999 Richard Hazlewood
// This code is provided as-is.  Use at your own peril.
// Modifications Copyright 1999-2001 Microsoft Corp.

#include "ItemInfo.h"

#ifndef RELEASE
#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0
#endif
/////////////////////////////////////////////////////////////////////////////

typedef CTypedPtrList<CPtrList, ItemInfo *> CItemList;
class CAudioPathDlg;
class CEditLabel;
struct BusEffectInfo;

typedef enum { NONE_COL = 0, PCHANNEL_COL = 1, BUS_BUFFER_COL = 2, EFFECT_COL = 3 } TreeColumn;

struct ItemInfoWrapper
{
	ItemInfoWrapper( ItemInfo *pNewItemInfo, BufferOptions *pNewBufferOptions )
	{
		pItemInfo = pNewItemInfo;
		pBufferOptions = pNewBufferOptions;
	}
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;;
};

/////////////////////////////////////////////////////////////////////////////
// CMultiTree window

#ifndef MST_AFX_EXT_CLASS
#define MST_AFX_EXT_CLASS
//define this if building class for DLL export/import
// typically: #define MST_AFX_EXT_CLASS AFX_EXT_CLASS
#endif

class MST_AFX_EXT_CLASS CMultiTree :
	public CListCtrl /*,
	public IDropSource,
	public IDropTarget*/
{
friend class CAudioPathDlg;
	DECLARE_DYNAMIC(CMultiTree)
// Construction
public:
	CMultiTree();
	virtual ~CMultiTree();


// Attributes
public:
	POINT		m_pointPopUpMenu;
	CAudioPathDlg *m_pAudioPathDlg;

// Operations
public:
	void SelectAll( void );	//doesn't affect focus
	void UnselectAll( void );
	void InvalidateItem( ItemInfo *pItemInfo );
	void InvalidateItem( int nItem );

	ItemInfo *GetFirstSelectedMixGroupItem() const;
	TreeColumn GetFirstSelectedThing( ItemInfo **ppItemInfo, BufferOptions **ppBufferOptions, BusEffectInfo **ppBusEffectInfo ) const;
	void SetDMAudioPathDlg( CAudioPathDlg *pDMAudioPathDlg );

	HRESULT	CreateDataObject( IDataObject** ppIDataObject );
	bool IsSelected(int nItem) const {return !!(LVIS_SELECTED & CListCtrl::GetItemState(nItem, LVIS_SELECTED));};
	void SelectItem( ItemInfo *pItemInfo );

	void GetSelectedList(CItemList& list) const;

	void GetEffectRect( const POINT &point, int nItem, BusEffectInfo **ppBusEffectInfo, RECT *pRect );
	TreeColumn GetItemBufferEffectUnderPoint( const POINT &point, ItemInfo **ppItem, BufferOptions **ppBufferOptions, BusEffectInfo **ppBusEffectInfo );
	TreeColumn GetThingUnderPointAndItem( const POINT &point, int nTreeItem, void **ppThing );
	TreeColumn GetColumn( long lXPos );
	BOOL GetColumn( int nCol, LVCOLUMN* pColumn ) const {return CListCtrl::GetColumn( nCol, pColumn );};

	/*
	void GetItemAndEffectRect( const POINT &point, ItemInfo **ppItem, BusEffectInfo **ppBusEffectInfo, RECT *pRect );
	TreeColumn GetItemAndThingUnderPoint( const POINT &point, ItemInfo **ppItem, void **ppThing );
	*/

	ItemInfo *GetItemInfo( int nItem ) const;

	void DisplayPPGForFirstSelectedThing( void );

protected:
	CEditLabel	*m_pEditCtrl;

protected:
	void OnButtonDown(BOOL bLeft, UINT nFlags, CPoint point);
	/*
	//void DoPreSelection(int nItem, BOOL bLeft, UINT nFlags);
	//void DoAction(int nItem, BOOL bLeft, UINT nFlags, CPoint point, bool fEditInPlace);
	//void DoBanding(BOOL bLeft, UINT nFlags, CPoint point);
	void DoNothingForMousePress(BOOL bLeft);
	void UpdateSelectionForRect(LPCRECT pRect, UINT nFlags, CTreeItemList& list);
	void SelectAllIgnore(BOOL bSelect, HTREEITEM hIgnore);
	void SelectItemIfUnselected( int nItem, DWORD dwState );
	*/
	void OnButtonDownOnItemRight(int nItem, UINT nFlags, CPoint point);
	void UnselectAllSubFields( void );
	void DoDelete( void );

// Edit helper functions
	HRESULT	CreateDataObjectEffect( IDataObject** ppIDataObject );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiTree)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CMultiTree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditPChannelCopy();
	afx_msg void OnEditPChannelCut();
	afx_msg void OnEditPChannelDelete();
	afx_msg void OnEditPChannelInsert();
	afx_msg void OnEditPChannelPaste();
	afx_msg void OnEditPChannelSelectAll();
	afx_msg void OnEditInsertMixGroup();
	afx_msg void OnEditMixGroupProperties();
	afx_msg void OnEditEffectCopy();
	afx_msg void OnEditEffectCut();
	afx_msg void OnEditEffectDelete();
	afx_msg void OnEditEffectInsert();
	afx_msg void OnEditEffectInsertSend();
	afx_msg void OnEditEffectPaste();
	afx_msg void OnEditEffectSelectAll();
	afx_msg void OnEditEffectProperties();
	afx_msg void OnEditBusProperties();
	afx_msg void OnEditBufferInsert();
	afx_msg void OnEditBufferDelete();
	afx_msg void OnEditBusAddRemove();
	afx_msg void OnEditInsertEnvReverb();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEditLabel window

class CEditLabel : public CEdit
{
// Construction
public:
	CEditLabel( CString strOrigName );

// Attributes
protected:
	CString		m_strOrigName;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditLabel)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MLTITREE_H__9F4750E5_C512_11D2_880A_02608C7CB3EE__INCLUDED_)
