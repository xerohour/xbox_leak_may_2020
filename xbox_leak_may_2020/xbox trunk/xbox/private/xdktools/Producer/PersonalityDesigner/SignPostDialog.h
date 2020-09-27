//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_SIGNPOSTDIALOG_H__2B651786_E908_11D0_9EDC_00AA00A21BA9__INCLUDED_)
#define AFX_SIGNPOSTDIALOG_H__2B651786_E908_11D0_9EDC_00AA00A21BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SignPostDialog.h : header file
//

#ifndef	__SIGNPOSTDIALOG_H__
#define __SIGNPOSTDIALOG_H__

#include "personalitydesigner.h"
#include "msflexgrid.h"
#include "Chord.h"

#include "PropChord.h"
#include "PropPageMgr.h"

#include "ChordDialog.h"

#define CHORDNAME_MAX 12
#define WM_INSERT_CHORD (WM_USER + 51)

// Special SignPost List Column values
const int MAX_COLUMNS	= 17;
const int COL_SELECT	= 0;
const int COL_C1		= 1;
const int COL_C2		= 2;
const int COL_SP		= 3;
const int COL_1			= 4;
const int COL_2			= 5;
const int COL_3			= 6;
const int COL_4			= 7;
const int COL_5			= 8;
const int COL_6			= 9;
const int COL_7			= 10;
const int COL_A			= 11;
const int COL_B			= 12;
const int COL_C			= 13;
const int COL_D			= 14;
const int COL_E			= 15;
const int COL_F			= 16;

class CPersonality;

/////////////////////////////////////////////////////////////////////////////
// CSignPostDialog dialog

class CChordDialog;
class CPersonalityCtrl;

class CSignPostDialog : public CDialog, public IDMUSProdPropPageObject, 
						public IDropSource , public IDropTarget
{
	friend CPersonalityCtrl;
private:
	bool	m_bInitialized;
	bool	m_bIgnoreClick;
	SignPostList	*m_pSignPostList;
	CChordDialog	*m_pChordDialog;
	POINTS			m_ptLastMouse;
	long			m_ptXLastMousePixels;
	long			m_ptYLastMousePixels;
	IDMUSProdFramework* m_pIFramework;
	CPersonality	*m_pIPersonality;

	// Supports Chord Property Page.
	CPropChord				m_SelectedChord;
	CPropertyPage*			m_pPropertyPage;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	CPersonalityCtrl	*m_pPersonalityCtrl;

	// drag drop state
	IDataObject*	m_pISourceDataObject;			// Object being dragged	
	IDataObject*	m_pITargetDataObject;
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton;		// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect;		// Drag effects stored in IDropTarget::DragOver
	CImageList*		m_pDragImage;			// Image used for drag operation feedback
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	UINT			m_cfChordList;			// clipboard format for signpost chords
	UINT			m_cfChordMapList;		// clipboard format for chordmap chords
	POINTL			m_startDragPosition;	// pos where drag was started

	ChordSelection*	m_pDragChord;			// intra-chordmap chord being dragged
	POINTS			m_ptDrop;				// where a drag move or drag copy is completed
	bool			m_bInternalDrop;		// false at beg of drag and set to true if source == target
	BOOL			m_fDragging;
	CDllJazzDataObject*		m_pCopyDataObject;
	UINT			m_nLastEdit;			// resource id of last edit

// Construction
public:
	enum {All = 0};
	void SetDrawingState(SignPost* pSignPost = 0);
	bool ToggleSignpostGroupBoxes();
	int ComputeColHeight();
	int ComputeRowWidth();
	void DMusicSync(int type);
	void SetRowText(SignPost* pSignPost, int row);
	HRESULT ShowPropPage(BOOL fShow);
	bool IsPropPageShowing(void);
	bool IsMyPropPageShowing(void);
	void SendEditNotification(UINT type);
	void Refresh(bool isStructualChange);
	CSignPostDialog(CWnd* pParent = NULL);   // standard constructor

	int GetHorizTwips(); 
	void TwipsToPixels(POINT& pt);
	void PixelsToTwips(POINT& pt);
	void RowColFromPoint(POINT& pt, int& row, int& col);
	void RowColFromTwips(POINT& pt, int& row, int& col);
	
	void SetSignPostListPtr( DWORD pSignPostList );
	HRESULT	SetAcceptChords( long lRow, long lCol, BOOL bSet = TRUE );
	HRESULT	AddSignPostToList();
	HRESULT AddSignPostToGrid( SignPost* pSignPost, int nRow = -1);
	HRESULT AddSignPostToList( SignPost* pSignPost, int nCol );
	HRESULT SetSelectedChord( SignPost* pSignPost, int nCol );
	void NormalizeRowCol();
	void InitializeDialog( CChordDialog* pChordDialog, CPersonality* pPersonality, CPersonalityCtrl* const pCtrl );
	void OnClickGrid();

	long GetRowFromRowData( DWORD dwRowData );
	void ResizeGrid();
	HRESULT UpdateCadenceChord(int nRow, int nCol, bool bVal);
	HRESULT UpdateSignPostChord( int row );
	void UpdateOrphanStatus(bool bRefreshDisplay, SignPost *pSignPost=0);
	HRESULT STDMETHODCALLTYPE PreparePropChord( SignPost *pSignPost );

	// Edit Methods (not IDMUSProdTimelineEdit)
	HRESULT STDMETHODCALLTYPE Cut (IDataObject** );
	HRESULT STDMETHODCALLTYPE Copy( /* in */ /* out */ IDataObject **ppIDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( /* in */ IDataObject* pIDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( /* in */ IDataObject* pIDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( void );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );

	// IDMUSProdPropPageObject methods
	HRESULT STDMETHODCALLTYPE GetData( /* [retval][out] */ void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( /* [in] */ void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);
	// IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// IDropSource
	HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );
	
	// IDropSource helpers
	CImageList* CreateDragImage();
	HRESULT		CreateDataObject(IDataObject**, POINTS where);
	HRESULT SaveSelectedChords(LPSTREAM, ChordSelection* pChordAtDragPoint, BOOL bNormalize = FALSE);
	ChordSelection* InternalCopy(POINTS ptDrop);

// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// IDropTarget helpers
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteAt( IDataObject* pIDataObject, POINTL pt, bool bDropNotEditPaste);

// Dialog Data
	//{{AFX_DATA(CSignPostDialog)
	enum { IDD = IDD_SIGNPOST_DIALOG };
	CMSFlexGrid	m_Grid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSignPostDialog)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSignPostDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyUpGrid(short FAR* KeyCode, short Shift);
	afx_msg void OnMouseUpGrid(short Button, short Shift, long x, long y);
	afx_msg void OnDestroy();
	afx_msg void OnMouseDownGrid(short Button, short Shift, long x, long y);
	afx_msg void OnOLEDragOverGrid(LPDISPATCH FAR* Data, long FAR* Effect, short FAR* Button, short FAR* Shift, float FAR* x, float FAR* y, short FAR* State);
	afx_msg void OnOLEDragDropGrid(LPDISPATCH FAR* Data, long FAR* Effect, short FAR* Button, short FAR* Shift, float FAR* x, float FAR* y);
	afx_msg void OnClickCMSFlexGrid();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CSignPostDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGNPOSTDIALOG_H__2B651786_E908_11D0_9EDC_00AA00A21BA9__INCLUDED_)
