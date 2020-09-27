// thdnode.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CThdTreeNode object

class CThdTreeNode : public CSpyTreeNode
{
	DECLARE_DYNCREATE(CThdTreeNode);

public:
	// Win32 version
	BOOL Create(CPROCDB *pProcessDatabase, int iThdIndex);
	// Chicago version
	BOOL Create(HANDLE hThreadList, DWORD dwThreadID, char *szExePath);

// Attributes
public:

// Operations
public:
	virtual void GetDescription(CString& str);
	virtual void ShowProperties();
	virtual int GetBitmapIndex()
	{
		return IDX_THDBITMAP;
	}
	virtual DWORD GetObjectHandle()
	{
		return (DWORD)m_tid;
	}
	virtual int GetObjectType()
	{
		return OT_THREAD;
	}
	virtual CString GetModule()
	{
		return m_strModule;
	}

	static void AddChildren(CMapPtrToPtr* pMap);

// Implementation
protected:
	DWORD m_tid;
	CString m_strModule;
};

/////////////////////////////////////////////////////////////////////////////
