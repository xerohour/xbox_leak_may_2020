#if !defined(AFX_RIGHTPANEDLG_H__EE744E37_B3CB_4336_8069_D050BA0AF9C6__INCLUDED_)
#define AFX_RIGHTPANEDLG_H__EE744E37_B3CB_4336_8069_D050BA0AF9C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "hsplitter.h"

class CFileItem;
class COtherFilesDlg;
//class CScriptsDlg;
interface IDMUSProdNode;

// RightPaneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRightPaneDlg dialog

class CRightPaneDlg : public CDialog //: public CEndTrack
{
// Construction
public:
	CRightPaneDlg(CWnd* pParent = NULL);   // standard constructor
	~CRightPaneDlg();

    //void EndTrack( long lNewPos );
	void OnConnectionStateChanged( void );
	bool IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode );
	bool IsFileInUse( CFileItem *pFileItem );
	HRESULT AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode );
	void CleanUpDisplay( void );
	void DeleteAll( void );
	//void ReCopyAll( void );

// Dialog Data
	//{{AFX_DATA(CRightPaneDlg)
	enum { IDD = IDD_RIGHT_PANE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRightPaneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//CHSplitter			m_wndHSplitter;
    COtherFilesDlg      *m_pOtherFilesDlg;
    //CScriptsDlg         *m_pScriptsDlg;

    //WORD                m_wSplitterPos;

    void UpdateListBoxPositions( void );

	// Generated message map functions
	//{{AFX_MSG(CRightPaneDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RIGHTPANEDLG_H__EE744E37_B3CB_4336_8069_D050BA0AF9C6__INCLUDED_)
