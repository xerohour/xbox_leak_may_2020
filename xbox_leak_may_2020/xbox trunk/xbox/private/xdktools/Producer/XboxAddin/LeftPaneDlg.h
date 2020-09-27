#if !defined(AFX_LEFTPANEDLG_H__FC797C3D_3DFB_45F7_8235_C30AF74C8BDF__INCLUDED_)
#define AFX_LEFTPANEDLG_H__FC797C3D_3DFB_45F7_8235_C30AF74C8BDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hsplitter.h"
#include "AudiopathListBox.h"
#include "DropTarget.h"

class CAudiopath;
class CFileItem;
class CPrimaryDlg;
class CSecondaryDlg;
interface IDMUSProdNode;

#define MAX_NUM_AUDIOPATHS (3)

// LeftPaneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLeftPaneDlg dialog

class CLeftPaneDlg : public CEndTrack, public CDropControl
{
// Construction
public:
	CLeftPaneDlg(CWnd* pParent = NULL);   // standard constructor
	~CLeftPaneDlg();

    void EndTrack( long lNewPos );
	void OnConnectionStateChanged( void );
	bool IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode );
	bool IsFileInUse( CFileItem *pFileItem );
	HRESULT AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode );
	void HandleNotification( NOTIFICATION_TYPE notificationType, DWORD dwSegmentID, DWORD dwData1 );
	void OnPanic( void );
	void UnloadAll( void );
	void DeleteAll( void );
	void ReCopyAll( void );

// Dialog Data
	//{{AFX_DATA(CLeftPaneDlg)
	enum { IDD = IDD_LEFT_PANE };
	CStatic	m_staticAudiopath;
	CAudiopathListBox	m_listAudiopath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftPaneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// CDropControl implementation
	virtual void InternalDragOver( DWORD grfKeyState, POINTL ptScreen, IDataObject* pIDataObject, DWORD* pdwEffect );
	virtual void InternalDrop( IDataObject* pIDataObject, POINTL ptScreen, DWORD dwEffect);
	virtual void InternalDragLeave( void );

// Implementation
protected:
	CHSplitter			m_wndHSplitter;
    CPrimaryDlg         *m_pPrimaryDlg;
    CSecondaryDlg       *m_pSecondaryDlg;

    WORD                m_wSplitterPos;
	CTypedPtrList< CPtrList, CAudiopath *> *m_plstAudiopaths;
    CTypedPtrList< CPtrList, CAudiopath *> m_lstAudiopathsToSynchronize;
	CDropTarget			m_DropTarget;
	UINT				m_cfAudiopath;
	UINT				m_cfFile;

    void UpdateListBoxPositions( void );
	void AddAudiopathToDisplay( POINTL ptScreen, IDMUSProdNode *pIDMUSProdNode );
	void SetDefault( CAudiopath *pAudiopath );
	void AddStandard( DWORD dwType );
	void DeleteAudiopath( CAudiopath *pAudiopath );
	void AddAudiopathToList( CAudiopath *pAudiopath );

	// Generated message map functions
	//{{AFX_MSG(CLeftPaneDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnApp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTPANEDLG_H__FC797C3D_3DFB_45F7_8235_C30AF74C8BDF__INCLUDED_)
