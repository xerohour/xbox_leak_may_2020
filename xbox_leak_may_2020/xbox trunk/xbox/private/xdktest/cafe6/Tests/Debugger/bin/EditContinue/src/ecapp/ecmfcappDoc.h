// ecmfcappDoc.h : interface of the CEcmfcappDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECMFCAPPDOC_H__49433FCD_FD1C_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_ECMFCAPPDOC_H__49433FCD_FD1C_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CEcmfcappDoc : public CDocument
{
protected: // create from serialization only
	CEcmfcappDoc();
	DECLARE_DYNCREATE(CEcmfcappDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEcmfcappDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEcmfcappDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEcmfcappDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECMFCAPPDOC_H__49433FCD_FD1C_11D0_84BF_00AA00C00848__INCLUDED_)
