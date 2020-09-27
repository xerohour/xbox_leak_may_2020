

/////////////////////////////////////////////////////////////////////////////
// CDebugOpt dialog

class CDebugOpt : public CDlgTab
{
// Construction
public:
	CDebugOpt();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDebugOpt)
	enum { IDD = IDDP_DEBUG };
	CString	m_szExpr;
	BOOL	m_fAnnotate;
	BOOL	m_fCodeBytes;
	BOOL	m_fFixedWidth;
	BOOL	m_fHexMode;
	BOOL	m_fLive;
	BOOL	m_fOLERPC;
	BOOL	m_fRaw;
	BOOL	m_fShowArgs;
	BOOL	m_fFPU;
	BOOL	m_fShowTypes;
	int		m_iItemsPerLine;
	BOOL	m_fSymbols;
	int		m_iFormat;
	BOOL	m_fEnableJit;
	BOOL	m_fReturnValues;
	BOOL	m_fUnicodeStrings;
    BOOL    m_fENCDebugging;
	BOOL	m_fLoadExports;
	//}}AFX_DATA

// Implementation
public:
	virtual BOOL	OnInitDialog();
	virtual BOOL	ValidateTab();
	virtual void	CommitTab();

	BOOL GetJitState();
	VOID SetJitState(BOOL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDebugOpt)
	afx_msg void OnClickedFixedwidth();
	afx_msg void OnClickedJit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CTLOpt dialog

class CTLOpt : public C3dDialog
{
// Construction
public:
	CTLOpt();
    ~CTLOpt();

// Dialog Data
	//{{AFX_DATA(CDebugOpt)
	enum { IDD = IDD_TRANSPORT_SELECT };
	//}}AFX_DATA

public:
	int 			m_cPlatforms;
	int				m_iPlatform;
	uniq_platform *	m_rgPlatforms;
	UINT *			m_rgiTL;
    BOOL            m_fConnectNow;

private:
	CComboBox *		m_plstPlatform;
	CListBox *		m_plstTransport;
	CButton *		m_pbutSetup;

// Implementation
public:
	virtual BOOL	OnInitDialog();

protected:

	// Generated message map functions
	//{{AFX_MSG(CTLOpt)
	afx_msg void OnHelp();
	afx_msg void OnClickedSetup();
	afx_msg void OnPlatformChange();
	afx_msg void OnTransportChange();
    afx_msg void OnClickedConnectNow ();
    afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
