#if !defined(AFX_PERSONALITYABOUT_H__9497C282_01E4_11D1_9EDC_00AA00A21BA9__INCLUDED_)
#define AFX_PERSONALITYABOUT_H__9497C282_01E4_11D1_9EDC_00AA00A21BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PersonalityAbout.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPersonalityAbout dialog

class CPersonalityAbout : public CDialog
{
// Construction
public:
	CPersonalityAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPersonalityAbout)
	enum { IDD = IDD_ABOUTBOX_PERSONALITY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPersonalityAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPersonalityAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERSONALITYABOUT_H__9497C282_01E4_11D1_9EDC_00AA00A21BA9__INCLUDED_)
