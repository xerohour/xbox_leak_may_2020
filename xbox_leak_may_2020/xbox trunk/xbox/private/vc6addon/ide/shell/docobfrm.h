// docobfrm.h : interface of the CDocObjectFrame class
//

#ifndef __DOCOBFRM_H__
#define __DOCOBFRM_H__

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
// CDocObjectFrame 

class CDocObjectFrame : public CPartFrame
{
	DECLARE_DYNCREATE(CDocObjectFrame)

public:
	CDocObjectFrame();
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnClose();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO * pMinMaxInfo);
};

#undef AFX_DATA
#define AFX_DATA NEAR

#endif	// __DOCOBFRM_H__
