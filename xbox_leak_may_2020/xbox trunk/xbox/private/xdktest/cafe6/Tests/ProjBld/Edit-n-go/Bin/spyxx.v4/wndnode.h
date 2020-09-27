// wndnode.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CWndTreeNode object

class CWndTreeNode : public CSpyTreeNode
{
	DECLARE_DYNCREATE(CWndTreeNode);

public:
	BOOL Create(HWND hwnd, BOOL fEnumChildren);

// Attributes
public:

// Operations
public:
	virtual void GetDescription(CString& str);
	virtual void ShowProperties();
	virtual int GetBitmapIndex()
	{
		return IDX_WNDBITMAP;
	}
	virtual DWORD GetObjectHandle()
	{
		return (DWORD)m_hwnd;
	}
	virtual int GetObjectType()
	{
		return OT_WINDOW;
	}
	virtual CString GetCaption()
	{
		return m_strText;
	}
	virtual CString GetClass()
	{
		return m_strClass;
	}

// Implementation
protected:
	HWND m_hwnd;
	CString m_strText;
	CString m_strClass;
};

/////////////////////////////////////////////////////////////////////////////
