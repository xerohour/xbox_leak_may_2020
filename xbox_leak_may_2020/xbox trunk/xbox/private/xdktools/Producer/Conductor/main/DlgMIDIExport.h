#if !defined(AFX_DLGMIDIEXPORT_H__DD2755D3_1843_11D3_B447_00105A2796DE__INCLUDED_)
#define AFX_DLGMIDIEXPORT_H__DD2755D3_1843_11D3_B447_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMIDIExport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMIDIExport dialog

class CDlgMIDIExport : public CDialog
{
// Construction
public:
	CDlgMIDIExport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMIDIExport)
	enum { IDD = IDD_DIALOG_MIDI_EXPORT };
	CButton	m_radioLeadInMeasure;
	CButton	m_radioFirstMeasure;
	//}}AFX_DATA

public:
	BOOL	m_fLeadInMeasureMIDIExport;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMIDIExport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMIDIExport)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMIDIEXPORT_H__DD2755D3_1843_11D3_B447_00105A2796DE__INCLUDED_)
