// VCNode.h: interface for the CVCNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCNODE_H__1FAE18F3_9021_11D0_8D12_00A0C91BC942__INCLUDED_)
#define AFX_VCNODE_H__1FAE18F3_9021_11D0_8D12_00A0C91BC942__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hu_ctnr.h"

#include <bldtypes.h>
class CProjItem;	// defined in pkgs\projbld\src\projitem.h
class CProject;		// defined in pkgs\projbld\src\project.h
class CVCDataDescriptor;

enum enumEnableStates 
{
	eEnableSupported = 0,
	eEnableSupportedAndEnabled = 1,
	eEnableInvisible = -1,
	eEnableNotSupported = -2,
	eEnableLatchedAndEnabled = -3,
	eEnableEnabled = -4,
	eEnableLatched = -5
};

class CVCNode : public CHierContainer
{
public:

DECLARE_NOT_AGGREGATABLE(CVCNode)


	enum enumKnownGuidList { eGuidUninitialized, eGuidVCCmdId, eGuidVSStd97 };

	UINT GetIconIndex();
					CVCNode(LPCTSTR pszName);
					CVCNode(void);
	virtual			~CVCNode(void);

	// Command Routing
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

	virtual HRESULT QueryStatusAtIndex(enumKnownGuidList eGuid, const GUID* pguidCmdGroup, OLECMD prgCmds[],
		OLECMDTEXT *pCmdText, ULONG nIndex);
	virtual HRESULT ProcessUIMsg(const MSG *pMsg);
	// REVIEW: This is only for debugging purposes
	virtual HRESULT DisplayContextMenu();
	virtual const GUID* PGuidGetType(void) const { return NULL; }
	virtual HRESULT	EnumerateChildren() { return S_OK; }
	virtual void OnOpenSettingsDialog();
	virtual HRESULT		GetProperty(VSHPROPID propid, VARIANT* pvar);
	virtual BOOL		IsContainer(void) const;	// returns FALSE
	virtual	UINT		GetKindOf(void) const;

	// CHierNode Properties
	HRESULT GetDisplayCaption(BSTR* pbstrCaption);
	virtual HRESULT GetDisplayCaption(CString& strCaption);

	HRESULT ShowContextMenu(UINT imxMenuID, IOleCommandTarget *pOleCommandTarget  = NULL);

	static DWORD TranslateEnableState(enumEnableStates eState);
	// CHierNode Properties

	STDMETHOD(get_Caption)(BSTR *pCaption) const;
	STDMETHOD(put_Caption)(LPCWSTR Caption);
	CString& GetFullName(CString& str) { str = m_strFullPath; return m_strFullPath;}

	BOOL IsExpanded(void) const
	{
		return (m_grfStateFlags & ST_IsExpanded);
	}

	CProjItem*		m_pProjItem;
	BOOL			m_fAutoExpand;
	DWORD			m_grfStateFlags;		// ChildrenEnumerated, IsExpanded, etc
	enum	// m_grfStateFlags
	{
		ST_ChildrenEnumerated = (1<<0),
		ST_IsExpanded = (1<<1),
		ST_FirstUserFlag = (1<<16)	// Derived classes are free to use these upper bits
	};

public:
	CProject* GetCProject();
	void SetCProject(CProject* pProject) { m_pProjItem = (CProjItem*)pProject; }
	HPROJECT GetHProject() { return (HPROJECT) m_pProjItem; }
	CProjItem* GetCProjItem() { return m_pProjItem; }


	CString			m_strCaption;	// this node's name
    CString			m_strFullPath;

	virtual const CString& GetCaption(void) { return m_strCaption; }
	virtual void SetCaption(LPCSTR pszNewCaption) {	// ansi method	
											m_strCaption = pszNewCaption; }
	virtual void SetCaption(LPCWSTR pszNewCaption) {	// unicode method
												m_strCaption = pszNewCaption; }
	virtual void SetCaption(const CString& strNewCaption);
	virtual const CString& GetFullPath(void) { return m_strFullPath; }

};

#endif // !defined(AFX_VCNODE_H__1FAE18F3_9021_11D0_8D12_00A0C91BC942__INCLUDED_)
