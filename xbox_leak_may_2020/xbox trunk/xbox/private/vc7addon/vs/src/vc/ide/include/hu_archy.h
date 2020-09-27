//---------------------------------------------------------------------------
// Microsoft Visual InterDev
//
// Microsoft Confidential
// Copyright 1994 - 1997 Microsoft Corporation. All Rights Reserved.
//
// hu_archy.h: Definition of the CVsHierarchy class
//
//---------------------------------------------------------------------------

#ifndef __HU_ARCHY_H__
#define __HU_ARCHY_H__

#ifndef  __docobj_h__
#include <DOCOBJ.H>		// OLE interfaces
#endif

#include "vsshell.h"
#include "hu_node.h"

#ifndef  __OBJEXT_H
#include <objext.h>		// VB6 interfaces & guids
#endif

#include "vccoll.h"

interface IVsHierarchyEvents;

//typedef CSimpleArray<CHierNode *> HierNodeList;
typedef CVCTypedPtrList<CVCPtrList, CHierNode*> HierNodeList;

typedef enum ICON_TYPE
{
	ICON_Open,
	ICON_Closed,
	ICON_StateImage
} ICON_TYPE;

//---------------------------------------------------------------------------
// provides:
//		interface: IVsHierarchy
//		interface: IVsUIHiearchy
//		interface: IOleCommandTarget
//		interface: ISelectionContainer
//		interface: IPkgDropDataSource
//		interface: IPkgDropDataTarget
// consumes:
//		interface: ITrackSelectionEx
//---------------------------------------------------------------------------
class CVsHierarchy : 
	public IVsUIHierarchy,
	public IOleCommandTarget,
	public IVsHierarchyDropDataSource,
	public IVsHierarchyDropDataTarget,
	public IVsPersistHierarchyItem,
	public ISupportErrorInfo,
	public CComObjectRootEx<CComSingleThreadModel>		// explict
{
protected:
	virtual ~CVsHierarchy(void);
	virtual UINT GetIconIndex(CHierNode* pNode, ICON_TYPE icontype) = 0;

public:
	CVsHierarchy(void);

public:	// get/set property methods
	void SetRootNode(CHierContainer *pRootNode)
	{
		m_pRootNode = pRootNode;
	}
	CHierContainer* GetRootNode(void) const
	{
		return m_pRootNode;
	}
	IVsHierarchy* GetVsHierarchy(void) const
	{
		return static_cast<IVsHierarchy *>(const_cast<CVsHierarchy *>(this));
	}
	void SetImageList(HIMAGELIST hImageList)
	{
		m_hImageList = hImageList;
	}
	HIMAGELIST GetImageList(void) const
	{
		return m_hImageList;
	}
	BOOL IsZombie(void) const
	{
		return (m_pRootNode == NULL);
	}
	HRESULT GetSelectedNodes(HierNodeList& rgNodes) const;
	virtual HRESULT DisplayContextMenu(HierNodeList &rgSelection, VARIANT *pvaIn);

public:	// IVsHierarchyEvent propagation
	HRESULT OnItemAdded(CHierNode *pNodeParent, CHierNode *pNodePrev, CHierNode *pNodeAdded) const;
	HRESULT OnItemDeleted(CHierNode *pNode) const;
	HRESULT OnPropertyChanged(CHierNode *pNode, VSHPROPID propid, DWORD flags) const;
	HRESULT OnInvalidateItems(CHierNode *pNode) const;
	HRESULT	OnInvalidateIcon(HICON hIcon) const;

BEGIN_COM_MAP(CVsHierarchy)
	COM_INTERFACE_ENTRY(IVsHierarchy)		// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsUIHierarchy)		// derives from IVsHierarchy
	COM_INTERFACE_ENTRY(IOleCommandTarget)	// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsHierarchyDropDataSource)	// dervies from IUnknown
	COM_INTERFACE_ENTRY(IVsHierarchyDropDataTarget)	// dervies from IUnknown
	COM_INTERFACE_ENTRY(IVsPersistHierarchyItem) // derives from IUnknown
	COM_INTERFACE_ENTRY(ISupportErrorInfo)	// dervies from IUnknown
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVsHierarchy)

// IVsHierarchy
public:
	STDMETHOD(SetSite)(
            /* [in] */ IServiceProvider *pSP);
        
	STDMETHOD(GetSite)(
            /* [out] */ IServiceProvider **ppSP);
	
	STDMETHOD(QueryClose)(
			/* [out] */ BOOL *pfCanClose);
        
	STDMETHOD(Close)(void);
 
	STDMETHOD(GetGuidProperty)(
			/* [in] */ VSITEMID itemid,
			/* [in] */ VSHPROPID propid,
			/* [out] */ GUID *pguid);

	STDMETHOD(SetGuidProperty)(
			/* [in] */ VSITEMID itemid, 
			/* [in] */ VSHPROPID propid,
			/* [in] */ REFGUID guid);


	virtual HRESULT GetProperty(	// not in interface, helper routine
			/* [in] */ CHierNode *pNode,
            /* [in] */ VSHPROPID propid,
            /* [out] */ VARIANT *pvar);

	STDMETHOD(GetProperty)(
            /* [in] */ VSITEMID itemid,
            /* [in] */ VSHPROPID propid,
            /* [out] */ VARIANT *pvar);
        
	virtual HRESULT SetProperty(	// not in interface, helper routine
            /* [in] */ CHierNode *pNode,
            /* [in] */ VSHPROPID propid,
            /* [in] */ const VARIANT& var);

	STDMETHOD(SetProperty)(
            /* [in] */ VSITEMID itemid,
            /* [in] */ VSHPROPID propid,
            /* [in] */ VARIANT var);
        
	STDMETHOD(GetNestedHierarchy)(
            /* [in] */ VSITEMID itemid,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppHierarchyNested,
            /* [out] */ VSITEMID *pitemidNested);
        
	STDMETHOD(GetCanonicalName)(
            /* [in] */ VSITEMID itemid,
            /* [out] */ BSTR *ppszName);
        
	STDMETHOD(ParseCanonicalName)(
            /* [in] */ LPCOLESTR pszName,
            /* [out] */ VSITEMID *pitemid);
        
	STDMETHOD(GetAutomationObject)(
            /* [out] */ IDispatch **ppdisp);

	STDMETHOD(AdviseHierarchyEvents)(
			/* [in] */ IVsHierarchyEvents *pEventSink,
			/* [out] */ DWORD *pdwCookie);

	STDMETHOD(UnadviseHierarchyEvents)(
			/* [in] */ DWORD dwCookie);

	STDMETHOD(Unused0)(void);
	STDMETHOD(Unused1)(void);
    STDMETHOD(Unused2)(void);
    STDMETHOD(Unused3)(void);
    STDMETHOD(Unused4)(void);

// IVsUIHierarchy
public:
    STDMETHOD (QueryStatusCommand)( 
			/* [in] */        VSITEMID     itemid, 
			/* [in, unique] */ const GUID * pguidCmdGroup,
			/* [in] */        ULONG cCmds,
			/* [in, out, size_is(cCmds)] */ OLECMD prgCmds[],
			/* [in, out, unique] */ OLECMDTEXT *pCmdText);

	STDMETHOD(ExecCommand)( 
			/* [in] */        VSITEMID     itemid, 
			/* [in, unique] */ const GUID * pguidCmdGroup,
			/* [in] */       DWORD        nCmdID,
			/* [in] */        DWORD        nCmdexecopt,
			/* [in, unique] */ VARIANT *    pvaIn,
			/* [in, out, unique] */ VARIANT* pvaOut);

// IVsPersistHierarchyItem
public:
    STDMETHOD(IsItemDirty)( 
        /* [in] */ VSITEMID vsid,
        /* [in] */ IUnknown __RPC_FAR *punkDocData,
        /* [out] */ BOOL __RPC_FAR *pfDirty);
    
    STDMETHOD(SaveItem)( 
        /* [in] */ VSSAVEFLAGS dwSave,
        /* [in] */ LPCOLESTR lpstrMkDocument,
        /* [in] */ VSITEMID vsid,
        /* [in] */ IUnknown __RPC_FAR *punkDocData,
        /* [out] */ BOOL __RPC_FAR *pfCanceled);


// IVsHierarchyDropDataSource
public:
	STDMETHOD(GetDropInfo)( // make pure after everyone implements
            /* [out] */ DWORD *pdwOKEffects,
            /* [out] */ IDataObject **ppDataObject,
            /* [out] */ IDropSource **pDropSource);
        
	STDMETHOD(OnDropNotify)( 
            /* [in] */ BOOL fDropped,
            /* [in] */ DWORD dwEffects);

// IVsHierarchyDropDataTarget
public:
	STDMETHOD(DragEnter)(	// not in interface, helper routine
            /* [in] */ IDataObject *pDataObj);

	STDMETHOD(DragEnter)( 
            /* [in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ VSITEMID itemid,
            /* [out] */ DWORD *pdwEffect);
        
	STDMETHOD(DragOver)(		// not in interface, helper routine
            /* [in] */ DWORD grfKeyState,
            /* [in] */ CHierNode *pNode,
            /* [out] */ DWORD *pdwEffect);

	STDMETHOD(DragOver)(
            /* [in] */ DWORD grfKeyState,
            /* [in] */ VSITEMID itemid,
            /* [out] */ DWORD *pdwEffect);
        
	STDMETHOD(DragLeave)(void);
        
	virtual HRESULT Drop(		// not in interface, helper routine
            /* [in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ CHierNode *pNode,
            /* [out] */ DWORD *pdwEffect);

	STDMETHOD(Drop)(
            /* [in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ VSITEMID itemid,
            /* [out] */ DWORD *pdwEffect);


// IOleCommandTarget
public:
	STDMETHOD(QueryStatus)( 
            /* [unique][in] */ const GUID *pguidCmdGroup,
            /* [in] */ ULONG cCmds,
            /* [out][in][size_is] */ OLECMD prgCmds[],
            /* [unique][out][in] */ OLECMDTEXT *pCmdText);
        
	STDMETHOD(Exec)( 
            /* [unique][in] */ const GUID *pguidCmdGroup,
            /* [in] */ DWORD nCmdID,
            /* [in] */ DWORD nCmdexecopt,
            /* [unique][in] */ VARIANT *pvaIn,
            /* [unique][out][in] */ VARIANT *pvaOut);

// ISupportsErrorInfo
public:
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

protected:	// protected methods
	HRESULT VSITEMID2Node(const VSITEMID itemid, CHierNode **ppNode) const;
	HRESULT VSITEMID2Node(const VSITEMID itemid, HierNodeList& rgNodeList) const;

	// Override to handle Query status in a particular way
	virtual HRESULT QueryStatusSelection(
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[],
		OLECMDTEXT *pCmdText,
		HierNodeList &rgSelection,
		BOOL		 bIsHierCmd);		// TRUE if cmd originated via CVSUiHierarchy::ExecCommand
	
	// Override to handle Exec in a particular way
	virtual HRESULT ExecSelection(
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut,
		HierNodeList &rgSelection,
		BOOL		 bIsHierCmd);		// TRUE if cmd originated via CVSUiHierarchy::ExecCommand
protected:	// protected variables
	HIMAGELIST m_hImageList;

	CHierContainer *m_pRootNode;

	static DWORD g_dwCookie;
	struct EventSink
	{		
		CComPtr<IVsHierarchyEvents> m_srpEventSink;
		DWORD m_dwCookie;
	};
	CSimpleArray<EventSink *> m_rgEventSinks;

	// Properties to support being a used as a nested hierarchy
	IUnknown *m_pParentHierarchy;
	VSITEMID m_dwParentHierarchyItemid;

	// cache this pointer to reduce QS
	IVsMonitorSelection *m_srpMonSel;

	IServiceProvider* m_srpServiceProvider;

	static BOOL	g_bStartedDrag;
};

#endif __HU_ARCHY_H__
