// docobdoc.h : interface of the CDocObjectDoc class
//

#ifndef __DOCOBDOC_H__
#define __DOCOBDOC_H__

class CDocObjectContainerItem;

#undef AFX_DATA
#define AFX_DATA

/////////////////////////////////////////////////////////////////////////////
// CDocObjectDoc

class AFX_EXT_CLASS CDocObjectDoc : public CPartDoc
{
	DECLARE_DYNCREATE(CDocObjectDoc)

protected:
	CDocObjectDoc();

public:
	CLSID m_clsid;	// the CLSID of the thing we are hosting

public:
	//{{AFX_VIRTUAL(CDocObjectDoc)
	public:
	virtual BOOL NewDocumentHook();
	virtual BOOL OpenDocumentHook(LPCTSTR lpszPathName);
	virtual BOOL SaveDocumentHook(const char* pszPathName, UINT nSaveType = 0);
	virtual void CloseDocumentHook();
	//}}AFX_VIRTUAL
	virtual HRESULT	GetService(REFGUID guidService, REFIID riid, void **ppvObj);

public:
	virtual ~CDocObjectDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL IsModified();				// overridden for 'save on close?'
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CDocObjectContainerItem *m_pItem;	// the OleClientItem which provides the container site

	virtual void DoClose();

protected:
	//{{AFX_MSG(CDocObjectDoc)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __DOCOBDOC_H__
