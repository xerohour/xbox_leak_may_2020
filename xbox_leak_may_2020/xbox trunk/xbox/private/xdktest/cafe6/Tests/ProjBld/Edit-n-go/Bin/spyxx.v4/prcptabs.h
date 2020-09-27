// prcptabs.h : header file
//


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessGeneralPropTab window

class CProcessGeneralPropTab : public CPropertyTab
{
// Construction
public:
	CProcessGeneralPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CProcessGeneralPropTab)
	enum { IDD = IDD_PROCESSGENERALTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CProcessGeneralPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	CHotLinkCtrlPrc m_HotLinkParentProcessID;

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessGeneralPropTab)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessMemoryPropTab window

class CProcessMemoryPropTab : public CPropertyTab
{
// Construction
public:
	CProcessMemoryPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CProcessMemoryPropTab)
	enum { IDD = IDD_PROCESSMEMORYTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CProcessMemoryPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessMemoryPropTab)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessPageFilePropTab window

class CProcessPageFilePropTab : public CPropertyTab
{
// Construction
public:
	CProcessPageFilePropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CProcessPageFilePropTab)
	enum { IDD = IDD_PROCESSPAGEFILETAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CProcessPageFilePropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessPageFilePropTab)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessFileIOPropTab window

class CProcessFileIOPropTab : public CPropertyTab
{
// Construction
public:
	CProcessFileIOPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CProcessFileIOPropTab)
	enum { IDD = IDD_PROCESSFILEIOTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CProcessFileIOPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessFileIOPropTab)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProcessSpacePropTab window

class CProcessSpacePropTab : public CPropertyTab
{
// Construction
public:
	CProcessSpacePropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CProcessSpacePropTab)
	enum { IDD = IDD_PROCESSSPACETAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CProcessSpacePropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessSpacePropTab)
	afx_msg void OnSpaceTypeChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
