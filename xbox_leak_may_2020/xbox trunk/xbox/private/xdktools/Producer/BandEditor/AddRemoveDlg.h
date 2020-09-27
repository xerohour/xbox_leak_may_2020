#if !defined(AFX_ADDREMOVEDLG_H__B603A605_2231_11D2_850F_00A0C99F7E74__INCLUDED_)
#define AFX_ADDREMOVEDLG_H__B603A605_2231_11D2_850F_00A0C99F7E74__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AddRemoveDlg.h : header file
//

#include <afxtempl.h>


/////////////////////////////////////////////////////////////////////////////
// CAddRemoveDlg dialog

class CAddRemoveDlg : public CDialog
{
// Construction
public:
	CAddRemoveDlg(CWnd* pParent = NULL);   // standard constructor

	void SetBandDialog(CBandDlg* pBandDlg);
	void SetInstrumentList(CPtrList* pList);

	CPtrList* m_pInstrumentList;

	
// Dialog Data
	//{{AFX_DATA(CAddRemoveDlg)
	enum { IDD = IDD_ADDREMOVE };
	CSpinButtonCtrl	m_ChannelSpin;
	CListBox	m_ChanListBox;
	CEdit		m_ChannelEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddRemoveDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddRemoveDlg)
	afx_msg void OnAddBtn();
	afx_msg void OnRemoveBtn();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDeltaposPchannelSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Operations
private:
	BOOL	AddToChannels(int nChannel);
	void	AddPChannel(int nChannel);
	void	RemovePChannel(int nChannel);
	void	CloneChannels();
	CString GetPChannelName(CDMInstrument* pInstrument);
	void	PopulateChannelList();
	int		GetNextAvailablePChannel(int nStartIndex);
	int		GetLastAvailablePChannel(int nStartIndex);


// Attributes
private:
	CBandDlg* m_pBandDlg;
	CList<int, int> m_Channels;		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDREMOVEDLG_H__B603A605_2231_11D2_850F_00A0C99F7E74__INCLUDED_)
