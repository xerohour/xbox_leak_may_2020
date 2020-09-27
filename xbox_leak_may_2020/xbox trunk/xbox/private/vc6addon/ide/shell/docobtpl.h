// docobtpl.h : interface of the CDocObjectTemplate class
//

#ifndef __DOCOBTPL_H__
#define __DOCOBTPL_H__

class CPackage;

#undef AFX_DATA
#define AFX_DATA

/////////////////////////////////////////////////////////////////////////////
// CDocObjectTemplate

class AFX_EXT_CLASS CDocObjectTemplate : public CPartTemplate
{
	DECLARE_DYNAMIC(CDocObjectTemplate)

public:
	CString m_strFilter;
	HICON m_hIcon;

public:
	CDocObjectTemplate(CPackage* pPackage, REFCLSID clsid,
		const CString &strDefaultExtension,
		const CString &strDescName, 
		const CString &strDocBaseName, HICON hIcon,
		CRuntimeClass *pDocRuntimeClass = NULL,
		CRuntimeClass *pViewRuntimeClass = NULL);

	virtual Confidence MatchDocType(const TCHAR* pszPathName,
					CDocument*& rpDocMatch);
	virtual void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
		BOOL bMakeVisible = TRUE);
	virtual CDocument* OpenDocumentFile(const TCHAR* pszPathName,
		BOOL bMakeVisible = TRUE);

	// override to get alternative icon
	virtual HICON GetIcon() { 
		if(m_hIcon) return m_hIcon;
		else	// The HICON is null on Win95, so we use the default one...
			return CPartTemplate::GetIcon();
	};
	// override to specify tab type
	virtual FileNewTabType GetTabType() { return newother; };

protected:
	CRuntimeClass *m_pDocRuntimeClass;
	CRuntimeClass *m_pViewRuntimeClass;
};

/////////////////////////////////////////////////////////////////////////////
// CRegistryKeyEnum (generally useful thing)

class CRegistryKeyEnum
{
public:
	HKEY m_hkey;
	CString m_strKeyName;
	CString m_strDefaultExtension;
	CString m_strDescName;
	CString m_strDocNameBase;
	HICON m_hIcon;
private:
	HKEY m_hkeyRoot;
	ULONG m_ikey;
	CString m_strRequiredSubkey;
public:
	CRegistryKeyEnum(HKEY hkeyRoot, LPCTSTR szRequiredSubkey = NULL);
	CRegistryKeyEnum(CRegistryKeyEnum& r) { ASSERT(FALSE); }	// no copy ctor implemented
	~CRegistryKeyEnum();
	BOOL Next(LPCTSTR szType = NULL);
};

/////////////////////////////////////////////////////////////////////////////
// CTaskAllocator

class CTaskAllocator
{
public:
	IMalloc *m_pmalloc;

	CTaskAllocator();
	CTaskAllocator(CTaskAllocator &taskalloc);
	~CTaskAllocator();
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __DOCOBTPL_H__
