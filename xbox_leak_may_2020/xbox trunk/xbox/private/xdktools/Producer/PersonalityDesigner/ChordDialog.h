#if !defined(AFX_CHORDDIALOG_H__2B651783_E908_11D0_9EDC_00AA00A21BA9__INCLUDED_)
#define AFX_CHORDDIALOG_H__2B651783_E908_11D0_9EDC_00AA00A21BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ChordDialog.h : header file
//

#ifndef	__CHORDDIALOG_H__
#define __CHORDDIALOG_H__

#include "Timeline.h"
#include "Personality.h"

/////////////////////////////////////////////////////////////////////////////
// CChordDialog dialog

class CPersonalityCtrl;

class CChordDialog : public CDialog
{
	friend CPersonalityCtrl;
public:
	IDMUSProdStrip	*m_pChordMapStrip;
	
// Construction
public:
	void ClearSelections();
	void KillProps();
	void Refresh(bool isStructual);
	CChordDialog(CWnd* pParent = NULL);   // standard constructor
	~CChordDialog();
	double GetZoom()
	{
		// persist zoom factor to personality
		if(!m_pITimeline)
			return 0.0;
		VARIANT var;
		var.vt = VT_R8;
		m_pITimeline->GetTimelineProperty(TP_ZOOM, &var);
		return V_R8(&var);
	}
	LONG GetFunctionbarWidth()
	{
		// persist function bar width to personality
		if(!m_pITimeline)
			return 0;
		VARIANT var;
		m_pITimeline->GetTimelineProperty(TP_FUNCTIONBAR_WIDTH, &var);
		return V_I4(&var);
	}

	void InitializeDialog( CPersonality* pPersonality, CPersonalityCtrl* const pCtl );

	IDMUSProdTimeline	*m_pITimeline;
	CPersonality		*m_pPersonality;
	CPersonalityCtrl	*m_pPersonalityCtrl;

// Dialog Data
	//{{AFX_DATA(CChordDialog)
	enum { IDD = IDD_CHORD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChordDialog)
	public:
	virtual void OnFinalRelease();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HACCEL					m_hAcceleratorTable;

	// Generated message map functions
	//{{AFX_MSG(CChordDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CChordDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif

#endif // !defined(AFX_CHORDDIALOG_H__2B651783_E908_11D0_9EDC_00AA00A21BA9__INCLUDED_)
