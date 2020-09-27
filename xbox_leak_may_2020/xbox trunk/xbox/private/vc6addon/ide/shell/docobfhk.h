// docobfhk.h : interface of the CDocObjectFrameHook class
//

#ifndef __DOCOBFHK_H__
#define __DOCOBFHK_H__

/////////////////////////////////////////////////////////////////////////////
// CDocObjectFrameHook 

class CDocObjectFrameHook : public COleFrameHook
{

public:
	CDocObjectFrameHook(CFrameWnd* pFrameWnd, COleClientItem* pItem);
	~CDocObjectFrameHook();

	virtual BOOL OnDocActivate(BOOL bActive);

protected:
	DECLARE_INTERFACE_MAP()

	// we implement IOleCommandTarget in addition to the normal frame hook interfaces because
	// PowerPoint queries our IOleInPlaceFrame interface for our IOleCommandTarget interface.
	BEGIN_INTERFACE_PART(OleCommandTarget, IOleCommandTarget)
		INIT_INTERFACE_PART(CDocObjectFrameHook, OleCommandTarget)
		STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds,
				  OLECMD prgCmds[], OLECMDTEXT *pCmdText);
		STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANTARG *pvaIn, VARIANTARG *pvaOut);
	END_INTERFACE_PART(OleCommandTarget)
};

#endif	// __DOCOBFHK_H__
