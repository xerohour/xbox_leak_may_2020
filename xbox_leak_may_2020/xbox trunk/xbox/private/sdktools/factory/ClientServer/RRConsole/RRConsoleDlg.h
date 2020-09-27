// RRConsoleDlg.h : header file
//

#if !defined(AFX_RRCONSOLEDLG_H__D8A8E625_4811_411D_B3D0_582CE9AC998E__INCLUDED_)
#define AFX_RRCONSOLEDLG_H__D8A8E625_4811_411D_B3D0_582CE9AC998E__INCLUDED_

#include "CNetwork.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRRConsoleDlg dialog

#define IniName "RRConsoleConfigV2.ini"
#define RED 0xFF0000

char * FindTaggedData(char * pInputBuff,char * szTargetStr);

class CRRConsoleDlg : public CDialog
{
// Construction
public:
	CBitmap * m_pbmRed;
	CRRConsoleDlg(CWnd* pParent = NULL);	// standard constructor
	void		ResetDlgButtons();
	DWORD		UpdateGenealogy();
	CNetErrors	SendVerifyXBoxCommand(PDATA_PACKET pPacket);
	CNetErrors	SendGenealogyData(char * pSerialNumber,GENDATA * pGenData);
	DWORD		GetGenealogyData(char * pSerialNumber);
	DWORD		GetHDKeyFromServer(char *, byte *);
	ShowErrorStr(UINT nID /* string id */);
	CNetErrors	UnlockHD(byte * HDKey);
	char *		m_pBuffer;
	DWORD		m_dwFileSize;
	void		OnOK() {;}; // disable OK default behavior

private:

	char		m_GenSource[MAX_PATH];
	char		m_GenAltSource[MAX_PATH];
	char 		m_GenUpdate[MAX_PATH];
	HICON		m_hIcon;
	CNetWork	m_XBServer;
	char		m_szXBIPString[20];
	char		m_szSFIPString[20];
	GENDATA		m_GenData;
	BOOL		m_fNoUpdate;
	BOOL		m_fAltExists;


// Dialog Data
	//{{AFX_DATA(CRRConsoleDlg)
	enum { IDD = IDD_RRCONSOLE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRConsoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:


	// Generated message map functions
	//{{AFX_MSG(CRRConsoleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnectXBOX();
	afx_msg void OnHDReplace();
	afx_msg void OnCompleteXBox();
	afx_msg void OnClose();
	afx_msg void OnVerifyOnly();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RRCONSOLEDLG_H__D8A8E625_4811_411D_B3D0_582CE9AC998E__INCLUDED_)
