// VCBsNode.h: interface for the CVCBaseFolderNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__VCBSNODE_H__)
#define __VCBSNODE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "vc.h"
#include "VCNode.h"

class CVCArchy;

class CVCBaseFolderNode :
	public CVCNode
{
public:
	CVCBaseFolderNode();
	virtual ~CVCBaseFolderNode();

	// Message handlers. Public since called by more than just menu cmds.
	// IOleCommandTarget
	STDMETHOD(QueryStatus)(
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[],
		OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut);

	UINT GetIconIndex();
	virtual HRESULT DisplayContextMenu();
	virtual HRESULT OnNewFolder();
	virtual HRESULT OnAddNewFiles();
	virtual HRESULT OnAddClass();
	virtual HRESULT OnAddExistingFiles();
	virtual HRESULT OnAddResource();
	virtual HRESULT QueryStatusAtIndex(CVCNode::enumKnownGuidList eGuid, const GUID* pguidCmdGroup, 
		OLECMD prgCmds[], OLECMDTEXT *pCmdText, ULONG nIndex);
	HRESULT	HandleAddItemDlg(
            /* [in] */ VSADDITEMOPERATION dwAddItemOperation,
            /* [in] */ LPCOLESTR pszItemName,
			/* [in] */ DWORD cFilesToOpen,
            /* [in] */ LPCOLESTR rgpszFilesToOpen[],
            /* [in] */ HWND hwndDlg,
            /* [retval][out] */ VSADDRESULT __RPC_FAR *pResult);

	virtual HRESULT OnDelete(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE){return E_NOTIMPL;}	// CVCBaseFolderNode
	virtual HRESULT CleanUpUI(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE){return E_NOTIMPL;} // CVCBaseFolderNode
	virtual UINT GetKindOf( void ){return Type_CVCBaseFolderNode;}
	virtual HRESULT Refresh(CVCArchy *pHierarchy);
	HRESULT OnAddWebRef();
	BOOL CanAddWebRef();

	BOOL HaveChildrenBeenEvaluated(void) const
	{
		return (m_grfStateFlags & ST_ChildrenEnumerated);
	}

	virtual VSITEMID GetFirstChild(void);
	virtual CVCNode* GetHeadEx(void);
	CVCNode* GetHead(void) const
	{
		return m_pHeadNode;
	}
	CVCNode* GetTail(void) const
	{
		return m_pTailNode;
	}
	long GetCount(void) const;	// return number of children
	CVCNode* GetPrevChildOf(const CVCNode* pCurrent) const;

	// Override to get custom add behavior suchas keeping the list sorted.
	// Default calls AddTail(). 
	virtual void Add(CVCNode *const pNode);
	void 	AddAfter(CVCNode* const pCurrNode, CVCNode* const pNewNode);
	void	AddHead(CVCNode *const pNode);
	void	AddTail(CVCNode *const pNode);
	virtual HRESULT Remove(CVCNode* pNode);
	virtual HRESULT Delete(CVCNode* pNode, CVCArchy *pHierarchy);
	virtual void	DeleteAll(CVCArchy *pHierarchy);

	virtual HRESULT GetProperty(VSHPROPID propid, VARIANT* pvar);
	virtual HRESULT SetProperty(VSHPROPID propid, const VARIANT & pvar);

	HRESULT OnPaste();
	BOOL CanPaste();


protected:
    enum    // m_grfStateFlags
    {
        ST_ChildrenEnumerated = (1<<0),
        ST_IsExpanded = (1<<1),
        ST_FirstUserFlag = (1<<16)  // Derived classes are free to use these upper bits
    };

	CVCNode*	m_pHeadNode;
	CVCNode*	m_pTailNode;
	DWORD m_grfStateFlags;
	BOOL m_fAutoExpand;
};

#endif // !defined(__VCBSNODE_H__)
