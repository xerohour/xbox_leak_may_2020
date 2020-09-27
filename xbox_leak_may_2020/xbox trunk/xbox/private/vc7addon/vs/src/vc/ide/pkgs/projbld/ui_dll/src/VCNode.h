// VCNode.h: interface for the CVCNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCNODE_H__1FAE18F3_9021_11D0_8D12_00A0C91BC942__INCLUDED_)
#define AFX_VCNODE_H__1FAE18F3_9021_11D0_8D12_00A0C91BC942__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <bldtypes.h>

#ifdef AARDVARK
enum eMagic
{
	magicNone,
	magicReferenceFolder,
	magicOpenReferenceFolder,
	magicReference,
	magicBadReference,
	magicWinForm,
	magicUserControl,
	magicClass,
	magicWebCustomControl,
	magicSatelliteFolder,
	magicSatellite,
	// always leave this last:
	magicNotSet
};
#endif // AARDVARK

enum enumEnableStates 
{
	eEnableSupported = 0,
	eEnableSupportedAndEnabled = 1,
	eEnableInvisible = -1,
	eEnableNotSupported = -2,
	eEnableLatchedAndEnabled = -3,
	eEnableEnabled = -4,
	eEnableLatched = -5,
	eEnableSupportedAndInvisible = -6
};

enum {
	 Type_CHierNode = 1
	,Type_CHierContainer = 2
	,Type_CHierNestedContainer = 3
	,Type_CVCNode = 4
	,Type_CVCFile = 5
	,Type_CVCBaseFolderNode = 6
	,Type_CVCFileGroup = 7
	,Type_CVCProjectNode = 8
};

typedef enum ICON_TYPE
{
	ICON_Open,
	ICON_Closed,
	ICON_StateImage
} ICON_TYPE;

class CVCArchy;
class CVCProjectNode;

class CVCNode : 
	public IUnknown,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CVCNode)
	COM_INTERFACE_ENTRY(IUnknown)
END_COM_MAP()

public:
	enum enumKnownGuidList { eGuidUninitialized, eGuidVSStd2K, eGuidVSStd97 };

 	CVCNode(void);
	virtual ~CVCNode(void){};

	virtual UINT GetIconIndex( ICON_TYPE idx ) = 0;
	virtual CVCProjectNode* GetVCProjectNode() = 0;

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
	// REVIEW: This is only for debugging purposes
	virtual const GUID* PGuidGetType(void) const { return NULL; }
	virtual HRESULT	EnumerateChildren() { return S_OK; }
	virtual HRESULT GetProperty(VSHPROPID propid, VARIANT* pvar);
	virtual HRESULT SetProperty(VSHPROPID propid, const VARIANT& var);
	virtual HRESULT GetGuidProperty( VSHPROPID propid, GUID *pguid);
	virtual HRESULT SetGuidProperty( VSHPROPID propid, GUID guid);

	HRESULT ShowContextMenu(UINT imxMenuID, VARIANT *pvaIn, GUID guidMenuGroup, IOleCommandTarget *pOleCommandTarget  = NULL);

	static DWORD TranslateEnableState(enumEnableStates eState);

	CComQIPtr<IDispatch> m_dispkeyItem;

	virtual HRESULT GetVCProject(VCProject** ppProject);
	virtual HRESULT GetDispVCProject(IDispatch** ppDispProject);
	HRESULT GetVCItem(IDispatch** ppItem);
	IDispatch* GetPropertyContainer();
	virtual HRESULT GetExtObject(CComVariant& varRes) { return E_FAIL; }
	virtual CVCArchy* GetHierarchy(void) const;
	HRESULT GetActiveVCProjectConfig(IDispatch** ppProjCfg);

 	void SetCaption(BSTR bstrNewCaption);
	virtual HRESULT OnDelete(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE) = 0;
	virtual HRESULT CleanUpUI(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE) = 0;
	HRESULT ShowSettings(REFGUID guidProps);
	virtual HRESULT DoDefaultAction(BOOL fNewFile){ return E_NOTIMPL; };
	virtual HRESULT GetCanonicalName(BSTR* pbstrCanonicalName) = 0;
	virtual HRESULT GetName(BSTR* pbstrName) = 0;

	CVCNode* GetParent(void) const
	{
		return m_pNodeParent;
	}
	void SetParent(CVCNode *pNode)
	{
		m_pNodeParent = pNode;
	}
	CVCNode* GetNext(void) const
	{
		return m_pNodeNext;
	}
	void SetNext(CVCNode *pHierNode)
	{
		m_pNodeNext = pHierNode;
	}
	CVCNode* GetPrev(void) const;
 	CVCNode* GetRootNode(void) const;

	virtual UINT GetKindOf( void ) = 0;

	// return a CHierNode typecasted to a VSITEMID or VISTEMID_ROOT
	// Override in each derived class that can also be a VSITEMID_ROOT
	virtual VSITEMID GetVsItemID(void) const;

	virtual HRESULT DisplayContextMenu(VARIANT *pvaIn) = 0;

protected:
	// helper functions
 	HRESULT CanDirtyProject( void );
	// automation extender helper functions
	HRESULT GetExtender( LPOLESTR wszGuid, BSTR bstrName, IDispatch **ppDisp );
	HRESULT GetExtenderNames( LPOLESTR wszGuid, VARIANT *pvarNames );

	// inheritable data members
	CVCNode* m_pNodeParent;
	CVCNode* m_pNodeNext;	// this node is a part of a singly-linked list

public:
	virtual BOOL IsZombie() { return (m_dispkeyItem == NULL); }

public:
  BOOL m_fDragDropMoved;
};

//-----------------------------------------------------------------------------
// class CVCWaitCursor
//-----------------------------------------------------------------------------
class CVCWaitCursor
{
public:
	CVCWaitCursor() {m_hPrevCur = ::SetCursor(m_hWaitCur);}
	~CVCWaitCursor() {::SetCursor(m_hPrevCur); }
	void Restore() {::SetCursor(m_hWaitCur);}

protected:
	HCURSOR m_hPrevCur;
	static HCURSOR m_hWaitCur;
};

#endif // !defined(AFX_VCNODE_H__1FAE18F3_9021_11D0_8D12_00A0C91BC942__INCLUDED_)
