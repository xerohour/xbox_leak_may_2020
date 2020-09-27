// ODBCMTDoc.h : interface of the CODBCMTDoc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __ODBCMTDOC_H__
#define __ODBCMTDOC_H__

class CODBCMTDoc : public CDocument
{
private:
	CRITICAL_SECTION m_critsecRunning; //protects access to m_iCountRunning 

protected: // create from serialization only
	CODBCMTDoc();
	DECLARE_DYNCREATE(CODBCMTDoc)

// Attributes
public:
	CSize m_sizeDSN;
// Operations
public:
	// void NotifyStatus();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CODBCMTDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

	void CreateAndExecute(BOOL Async=FALSE,int NbRuns=1);
	void ExecutionDone();


// Implementation
public:
	virtual ~CODBCMTDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CObArray	m_ThreadArray;
	int			m_iCountConect;
	int			m_iCountRunning; //no of threads running
	int			m_iCountWaiting; //no of thread groups queued for run
// Generated message map functions
protected:
	//{{AFX_MSG(CODBCMTDoc)
	afx_msg void OnCreateConnection();
	afx_msg void OnEditConnect();
	afx_msg void OnConnectConnect();
	afx_msg void OnConnectDisconnect();
	afx_msg void OnConnectKillAll();
	afx_msg void OnCreateThreads();
	afx_msg void OnExecuteAll();
	afx_msg void OnThreadKillAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
/////////////////////////////////////////////////////////////////////////////
