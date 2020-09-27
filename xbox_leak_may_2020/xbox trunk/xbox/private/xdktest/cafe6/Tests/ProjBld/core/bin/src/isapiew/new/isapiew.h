#if !defined(AFX_ISAPIEW_H__7EFBEC11_CAA8_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_ISAPIEW_H__7EFBEC11_CAA8_11D0_84BF_00AA00C00848__INCLUDED_

// ISAPIEW.H - Header file for your Internet Server
//    isapiew Extension
#define ISAPIEWTEST
#include "resource.h"

class CIsapiewExtension : public CHttpServer
{
public:
	CIsapiewExtension();
	~CIsapiewExtension();

// Overrides
	// ClassWizard generated virtual function overrides
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_VIRTUAL(CIsapiewExtension)
	public:
	virtual BOOL GetExtensionVersion(HSE_VERSION_INFO* pVer);
	//}}AFX_VIRTUAL

	// TODO: Add handlers for your commands here.
	// For example:

	void Default(CHttpServerContext* pCtxt);

	DECLARE_PARSE_MAP()

	//{{AFX_MSG(CIsapiewExtension)
	//}}AFX_MSG
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISAPIEW_H__7EFBEC11_CAA8_11D0_84BF_00AA00C00848__INCLUDED)
