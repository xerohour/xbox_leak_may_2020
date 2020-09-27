//////////////////////////////////////////////////////////////////
// updepdlg.h

#ifndef _UPDEPDLG_H_
#define _UPDEPDLG_H_

#include "resource.h"
#include "prjconfg.h"
#include "utilctrl.h"
#include <dlgbase.h>	// C3dDialog

/////////////////////////////////////////////////////////////////
// CUpdateDepDlg class
//////////////////////////////////////////////////////////////////

class CUpdateDepDlg: public C3dDialog
{
//  Construction 
public:
 	CUpdateDepDlg(CWnd * pParent = NULL );
 	 
// Dialog Data
	//{{AFC_DATA(CUpdateDepDlg)
	enum { IDD = IDD_UPDATE_ALLDEP };
	//}}AFX_DATA

	CCheckList m_lbConfigs;

	static CMapStringToPtr m_mapSelected;

	static BOOL LoadSelFromOpt(CArchive & archive);
	static BOOL SaveSelToOpt(CArchive & archive);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	void ScanSelectedConfigs();

public:
	//{{AFX_MSG(CUpdateDepDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG

// private data
private:
	CProject * m_pProject ;
} ;
#endif // _UPDEPDLG_H_
