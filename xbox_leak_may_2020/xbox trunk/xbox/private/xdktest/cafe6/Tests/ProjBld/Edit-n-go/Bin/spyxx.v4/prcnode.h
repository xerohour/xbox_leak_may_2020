// prcnode.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CPrcTreeNode object

class CPrcTreeNode : public CSpyTreeNode
{
	DECLARE_DYNCREATE(CPrcTreeNode);

public:
	// Win32 version
	BOOL Create(CPROCDB *pProcessDatabase, int iPrcIndex, CMapPtrToPtr* pThdMap);
	// Chicago version
	BOOL Create(HANDLE hProcessList, DWORD dwProcessID, char *szExePath, CMapPtrToPtr* pThdMap);

// Attributes

// Operations
public:
	virtual void GetDescription(CString& str);
	virtual void ShowProperties();
	virtual int GetBitmapIndex()
	{
		return IDX_PRCBITMAP;
	}
	virtual DWORD GetObjectHandle()
	{
		return (DWORD)m_pid;
	}
	virtual int GetObjectType()
	{
		return OT_PROCESS;
	}
	virtual CString GetModule()
	{
		return m_strModule;
	}

// Implementation
protected:
	DWORD m_pid;
	CString m_strModule;
};

/////////////////////////////////////////////////////////////////////////////
