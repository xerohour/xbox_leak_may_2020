#if !defined(AFX_SEGMENTDLG_H__B682037D_5EFD_4DF2_8B8E_5A5D7C86B386__INCLUDED_)
#define AFX_SEGMENTDLG_H__B682037D_5EFD_4DF2_8B8E_5A5D7C86B386__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SegmentListBox.h"
#include "DropTarget.h"

// SegmentDlg.h : header file
//

class CFileItem;
class CSegment;
interface IDMUSProdFramework;
interface IDMUSProdNode;

/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg dialog

class CSegmentDlg : public CDialog, public CDropControl
{
// Construction
public:
	CSegmentDlg(CWnd* pParent = NULL);   // standard constructor
    ~CSegmentDlg();

	void OnConnectionStateChanged( void );
	bool IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode );
	bool IsFileInUse( CFileItem *pFileItem );
	HRESULT AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode );
	virtual void HandleNotification( NOTIFICATION_TYPE notificationType, DWORD dwSegmentID, DWORD dwData1 );
	virtual void HandleSegmentNotification( CSegment *pSegment, DWORD dwNotification );
	virtual void OnPanic( void );
	virtual void DeleteSegment( CSegment *pSegment, bool fForceDeletion );
	virtual void UnloadAll( void );
	virtual void DeleteAll( void );
	virtual void ReCopyAll( void );

// Dialog Data
	//{{AFX_DATA(CSegmentDlg)
	enum { IDD = IDD_SEGMENT };
	CStatic	m_staticSegment;
	CSegmentListBox	m_listSegment;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// CDropControl implementation
	virtual void InternalDragOver( DWORD grfKeyState, POINTL ptScreen, IDataObject* pIDataObject, DWORD* pdwEffect );
	virtual void InternalDrop( IDataObject* pIDataObject, POINTL ptScreen, DWORD dwEffect);
	virtual void InternalDragLeave( void );

// Implementation
protected:
	CTypedPtrList< CPtrList, CSegment *> *m_plstSegments;
	CTypedPtrList< CPtrList, CSegment *> m_lstSegmentsToSynchronize;
	CDropTarget	m_DropTarget;
	UINT		m_cfSegment;
	UINT		m_cfFile;

	virtual HRESULT AddSegmentToList( CSegment *pSegment );
	CSegment *SegmentFromID( DWORD dwID );

	// Generated message map functions
	//{{AFX_MSG(CSegmentDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg LRESULT OnApp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTDLG_H__B682037D_5EFD_4DF2_8B8E_5A5D7C86B386__INCLUDED_)
