#ifndef __CUSTBTN_H__
#define __CUSTBTN_H__

// This dialog appears when the user selects 'button appearance' from the toolbar button popup menu 
// that is available when the toolbar customize dialog is up

// The user can either choose how the button will appear, - image, text, or both, and can specify the image and text
// If the text includes an ampersand, that will become the buttons 'hot' key.

// The dialog returns IDOK if the user presses the assign button. In this case:
// If m_buttonText is not empty, then the user selected 'text button' and the new string for
//         the button is contained in m_buttonText
// Otherwise, m_buttonImage contains the id of the (fake) command whose bitmap is to be stolen by
//         the button.

#include "resource.h"


class CDockManager;
class CChoiceBar;

class CCustomButtonDlg : public C3dDialog
{
public:
// Construction
	CCustomButtonDlg(CString buttonName, CString buttonText, APPEARANCE at, CDockManager *m_pManager, APPEARANCEOPTION aoCanText, APPEARANCEOPTION aoCanImage, HBITMAP hbmCurrent=NULL, int nIndexCurrent=-1, CWnd* pParent = NULL);
	virtual ~CCustomButtonDlg();

// Dialog Data
	//{{AFX_DATA(CCustomButtonDlg)
	enum { IDD = IDD_CUSTOMBUTTON };
	CStatic	m_tools;
	CString	m_buttonName;				// The name of the button (e.g. DebugGo)
	CString	m_buttonText;				// The text drawn on the button
	APPEARANCE m_aAppearance;
	//}}AFX_DATA
	// If m_buttonText is empty on return, this contains the bitmap to be used for the custom button
	HBITMAP m_hbmCustomGlyph;
	APPEARANCEOPTION m_aoCanImage;
	APPEARANCEOPTION m_aoCanText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomButtonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HWND m_hWndNextClip;				// next window in the clipboard chain
	CDockManager *m_pManager;
	CChoiceBar *m_pButtonSource;		// the bar with all the potential custom buttons
	int m_nOldSelection;
	BOOL m_bLarge;

	HBITMAP m_hbmCurrent;				// BITMAP containing the current glyph, or NULL
	int m_nIndexCurrent;				// index of the current glyph, or -1

	// Generated message map functions
	//{{AFX_MSG(CCustomButtonDlg)
	afx_msg void OnImageEdit();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeButtonText();
	afx_msg void OnDestroy();
	afx_msg void OnChangeCbChain( HWND hWndRemove, HWND hWndAfter );
	afx_msg void OnDrawClipboard( );
	afx_msg void OnImagePaste();
	afx_msg void OnImageReset();
	afx_msg void OnImageOnly();
	afx_msg void OnImageText();
	afx_msg void OnTextOnly();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// update the enable status of the assign button (IDOK)
	// nIndex is the index of the currently selected toolbar button
	void UpdateControls();
};

#endif

