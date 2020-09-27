// VCPrjNode.h: interface for the CVCProjectNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCPRJNODE_H__1FAE18F5_9021_11D0_8D12_00A0C91BC942__INCLUDED_)
#define AFX_VCPRJNODE_H__1FAE18F5_9021_11D0_8D12_00A0C91BC942__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "vc.h"
#include "VCBsNode.h"
// for IVsGetCfgProvider
#include "vssolutn.h"
#include <prjids.h>
#include <BldMarshal.h>
#include "DataObject.h"
#include "ocdesign.h"

#include <atlctl.h>

enum  ICON_TYPE;
class CVCArchy;
class COnDropHelper;

class CVCProjectNode;

template <typename T> CComPtr<T> VCQI_cast( const CVCProjectNode* pCVCProjectNode) 
{ 
	CComPtr<T>	pT;
	((CVCProjectNode*)pCVCProjectNode)->QueryInterface( __uuidof(T), (void**)&pT );
	ASSERT( pT );
	return pT;
}

class CVCProjectNode :	public CVCBaseFolderNode,
						public IVsGetCfgProvider,
						public IDispatchImpl<_VCProject, &IID__VCProject, &LIBID_VCPrivateProjectLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
						public IVsPerPropertyBrowsing,
						public IOleCommandTarget,
						public IProvidePropertyBuilder,				
						public IPerPropertyBrowsing,
						public IPropertyNotifySinkCP<CVCProjectNode>,
						public IConnectionPointContainerImpl<CVCProjectNode>
{
public:
	CVCProjectNode(void);
	virtual ~CVCProjectNode();

	BOOL Open();
	virtual VSITEMID GetVsItemID(void) const;
	static HRESULT CreateInstance(CVCProjectNode **);
	virtual HRESULT DoDefaultAction(BOOL fNewFile);
	virtual HRESULT GetCanonicalName(BSTR* pbstrCanonicalName);
	virtual HRESULT GetName(BSTR* pbstrName);
	virtual HRESULT DisplayContextMenu(VARIANT *pvaIn);
	virtual CVCArchy* GetHierarchy(void) const;

	virtual	HRESULT CleanUpBeforeDelete();
	virtual HRESULT OnDelete(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE);
	virtual HRESULT CleanUpUI(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE);

	HRESULT CleanUpObjectModel(IDispatch* pParentDisp, IDispatch* pProjectDisp);
	HRESULT GetObjectsForDelete(IDispatch** ppParentDisp, IDispatch** ppProjectDisp);
	STDMETHOD(OnSaveProject)(LPCOLESTR pszNewName);
	BOOL IsProjectFileDirty();
	BOOL MatchesHierarchy(IVsHierarchy* pIVsHierarchy);
	void OnActiveProjectCfgChange();

public:
	virtual HRESULT GetExtObject(CComVariant& varRes);
	HRESULT GetActiveConfig(IVCGenericConfiguration** ppGenCfg);
	HRESULT CompileFileItems(CVCPtrList& rlistFiles);
	HRESULT OnDropMove(CVCOleDataObject& rDataObject, CVCNode* pDropNode, CVCProjDataSource* pOurDataSource, BOOL bCrossVC,
		BOOL bDrag, BOOL& bReported);
	HRESULT OnDropCopy(CVCOleDataObject& rDataObject, CVCNode* pDropNode, CVCProjDataSource* pOurDataSource, BOOL bCrossVC,
		BOOL bDrag, BOOL& bReported);
	virtual CVCProjectNode* GetVCProjectNode() { return this; }
	HRESULT GetBscFileName(BSTR* pbstrBSCName);

BEGIN_COM_MAP(CVCProjectNode)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
	COM_INTERFACE_ENTRY(_VCProject)
	COM_INTERFACE_ENTRY(IVsGetCfgProvider)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCProjectNode)

BEGIN_PROP_MAP(CVCProjectNode)
	PROP_ENTRY( "Name", VCPRJNODID_Name, CLSID_NULL )
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CVCProjectNode)
	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
END_CONNECTION_POINT_MAP() 

public:
// _VCProject
	STDMETHOD(get_ProjectFile)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ProjectDependencies)( BSTR *pVal );

	// automation extender methods
	STDMETHOD(get_Extender)(BSTR bstrName, IDispatch **ppDisp);
	STDMETHOD(get_ExtenderNames)(VARIANT *pvarNames);
	STDMETHOD(get_ExtenderCATID)(BSTR *pbstrGuid);

// IPerPropertyBrowsing
// Everything that should not be viewable by the user should be E_NOTIMPL!!
	STDMETHOD(GetDisplayString)( DISPID dispID, BSTR *pBstr )
		{ return E_NOTIMPL; /* CVCProjectNode::GetDisplayString */ }
	STDMETHOD(MapPropertyToPage)( DISPID dispID, CLSID *pClsid )
		{ return E_NOTIMPL; /* CVCProjectNode::MapPropertyToPage */ }
	STDMETHOD(GetPredefinedStrings)( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut )
		{ return E_NOTIMPL; /* CVCProjectNode::GetPredefinedStrings */ }
	STDMETHOD(GetPredefinedValue)( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut )
		{ return E_NOTIMPL; /* CVCProjectNode::GetPredefinedValue */ }

// IProvidepropertyBuilder
	STDMETHOD(MapPropertyToBuilder)( LONG dispid, LONG *pdwCtlBldType, BSTR *pbstrGuidBldr, VARIANT_BOOL *pfRetVal );
	STDMETHOD(ExecuteBuilder)( LONG dispid, BSTR bstrGuidBldr, IDispatch *pdispApp, LONG_PTR hwndBldrOwner, VARIANT *pvarValue, VARIANT_BOOL *pfRetVal );

// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)(DISPID dispid, BOOL *pfHide)
		{ return E_NOTIMPL; /* CVCProjectNode::HideProperty */ }
	STDMETHOD(DisplayChildProperties)(DISPID dispid, BOOL *pfDisplay)
		{ return E_NOTIMPL; /* CVCProjectNode::DisplayChildProperties */ }
	STDMETHOD(HasDefaultValue)(DISPID dispid, BOOL *pfDefault)
		{ return E_NOTIMPL; /* CVCProjectNode::HasDefaultValue */ }
	STDMETHOD(IsPropertyReadOnly)(DISPID dispid, BOOL *fReadOnly);
	STDMETHOD(GetLocalizedPropertyInfo)(DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc);
	STDMETHOD(GetClassName)(BSTR* pbstrClassName);
    STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* ) {return E_NOTIMPL;}	
    STDMETHOD(ResetPropertyValue)(DISPID ) {return E_NOTIMPL;}


public:
	UINT GetIconIndex(ICON_TYPE);
	HRESULT GetGuidProperty( VSHPROPID propid, GUID *pguid);
	HRESULT SetGuidProperty( VSHPROPID propid, GUID guid );
	HRESULT SetProperty(VSHPROPID propid, const VARIANT& var);
	HRESULT GetProperty(VSHPROPID propid, VARIANT *pvar);

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
	
// IVsGetCfgProvider
public:
	STDMETHOD(GetCfgProvider)(IVsCfgProvider** ppCfgProvider);

public:
	virtual UINT GetKindOf( void )
	{
		return Type_CVCProjectNode;
	}

protected:
	CVCArchy *m_pArchy;
	CBldMarshaller<IVsHierarchy> m_marshalVsHierarchy;

public:
	virtual HRESULT GetVCProject(VCProject** ppProject)
	{ 
		CHECK_POINTER_NULL(ppProject);
		CComQIPtr<VCProject> spProject = m_dispkeyItem;
		*ppProject = spProject.Detach();
		return S_OK;
	}
	virtual HRESULT GetDispVCProject(IDispatch** ppProject)
	{ 
		CHECK_POINTER_NULL(ppProject);
		*ppProject = m_dispkeyItem;
		if (*ppProject)
			(*ppProject)->AddRef();
		return S_OK;
	}
	CVCArchy* GetArchy();
	void SetArchy(CVCArchy* pArchy);
	HRESULT GetVsHierarchy(IVsHierarchy** ppHier, BOOL bInMainThread = FALSE);

protected:
	HRESULT DoInternalMove(CVCNode* pTgtNode, DWORD cItems, CVCNode* rgMovedNodes[], BOOL bDrag, BOOL& bReported);
	HRESULT DoCrossVCMoveOrCopy(CVCOleDataObject& rDataObject, CVCNode* pTgtNode, bool bIsMove, BOOL bDrag, BOOL& bReported);
	HRESULT DoNonVCCopy(CVCOleDataObject& rDataObject, CVCNode* pTgtNode, BOOL bDrag, BOOL& bReported);
	HRESULT CanMoveOrCopyElement(VCProject* pProjectParent, VCFilter* pFilterParent, IDispatch* pItem, bool bTopLevel, 
		bool bIsMove, BOOL bDrag, BOOL& bReported);
	HRESULT CanAddElementCollection(VCProject* pProject, VCFilter* pFilter, IDispatch* pItemCollection, BOOL bDrag, BOOL& bReported);
	HRESULT AddElement(VCProject* pProjectParent, VCFilter* pFilterParent, IDispatch* pItem, bool bIsMove, BOOL bDrag,
		BOOL& bReported);
	HRESULT AddElementCollection(VCProject* pProject, VCFilter* pFilter, IDispatch* pItemCollection, BOOL bDrag,
		BOOL& bReported);
	HRESULT BuildProjItemList(COnDropHelper& dropHelper, CVCOleDataObject& rDataObject, BOOL bCrossVC);
	void GetVCItemFromVsItem(IVsHierarchy* pHier, VSITEMID itemID, IDispatch** ppVCItem);
	void GetVsItemNameAndType(IVsHierarchy* pHier, VSITEMID itemID, BSTR* pbstrName, BOOL& bIsFile, BOOL& bIsFilter);
	HRESULT CanCopyVsElement(VCProject* pProjectParent, VCFilter* pFilterParent, IVsHierarchy* pHier, VSITEMID itemID, 
		bool bIsTopLevel, BOOL bDrag, BOOL& bReported);
	HRESULT AddVsElement(VCProject* pProjectParent, VCFilter* pFilterParent, IVsHierarchy* pHier, VSITEMID itemID, BOOL bDrag,
		BOOL& bReported);

	HRESULT ReportMoveError(HRESULT hrReport, IDispatch* pItem, IDispatch* pParent, BOOL bDrag, BOOL& bReported);
	HRESULT ReportVCCopyError(HRESULT hrReport, VCProject* pProjectParent, VCFilter* pFilterParent, IDispatch* pItem, 
		bool bIsMove, BOOL bDrag, BOOL& bReported);
	HRESULT ReportVsCopyError(HRESULT hrReport, VCProject* pProjectParent, VCFilter* pFilterParent, IVsHierarchy* pHier, 
		VSITEMID itemID, BOOL bDrag, BOOL& bReported);
	HRESULT ReportRemoveError(HRESULT hrReport, IDispatch* pItem, BOOL& bReported);

	static HRESULT ReportCopyError(HRESULT hrReport, IDispatch* pParent, CStringW& strItemName, CStringW& strParentName, 
		BOOL bDrag, BOOL& bReported);
	static void GetIdentifierString(IDispatch* pItem, CStringW& strIdentifier);

	enumEnableStates SupportsBuildType(bldActionTypes buildType);
	HRESULT DoBuildType(bldActionTypes buildType);

public:
	BOOL InBuild();

	static HRESULT ReportShellCopyError(HRESULT hrReport, VCProject* pProject, VCFilter* pFilter, LPWSTR wstrFile, 
		BOOL& bReported);
	static BOOL VsHandlesError(HRESULT hrReport);
	static void GetProjectNameString(IDispatch* pItem, CStringW& strProj);

private:
	// flag to prevent recursive closing 
	// (ugly hack, but necessary because we're called back from the shell)
	bool m_bStartedClose;
	CComBSTR m_bstrProjectType;
private:
	// automation extender CATID
	static const LPOLESTR s_wszCATID;
};

//-----------------------------------------------------------------------------
// Execution context class. Allows program to set state about an execution through
// the use of key/value pairs. For example: the delete commaand could set the ctx
// IDYES for it's delete confirmation dialog. So when user checks "apply to all items"
// it won't prompt. The code can get back the ctx (in this case IDYES) and use
// that. The Start/EndSingleOp() method should be used at the start and end of an operation to
// flush out the context. MultiSelect handlers should use the start/end MultiOp 
// funcitons.
// The class provides a generic CANCELLED state since this applies across all items.
//-----------------------------------------------------------------------------
#define CTX_MULTISELECT  0xffffffff
class CVCExecutionCtx
{
public:
				CVCExecutionCtx();
				~CVCExecutionCtx();
		void	SetCancelled(){m_bCancelled = TRUE;}
		void	ClearCancelled(){m_bCancelled = FALSE;}
		void	SetMultiSelect(DWORD dwCount){SetValue(CTX_MULTISELECT, dwCount);}
		DWORD	GetMultiSelect(){return GetValue(CTX_MULTISELECT);}
		void	StartOp();
		void	EndOp();
		BOOL	IsCancelled(){return m_bCancelled;}
		DWORD	GetValue(DWORD key);
		void	SetValue(DWORD key, DWORD value);
		// Use nesting level to determine multiop func
		BOOL	IsMultiOp() {return m_dwRef > 1;}
		DWORD	GetRefCount() {return m_dwRef;}
		void	ClearData();
protected:
		void	Clear();

	struct CExCtx
		{
			DWORD key;
			DWORD value;
		};
		BOOL	m_bCancelled;
		DWORD	m_dwRef;	// Nesting control
		CVCTypedPtrArray<CVCPtrArray, CExCtx*> m_exCtxArray;

		CExCtx* GetExCtx(DWORD key);
};

// Useful helper to automatically do the start/end operations for you
class CVCExecution
{
public:
				CVCExecution(CVCExecutionCtx* pCtx) : m_pCtx(pCtx){pCtx->StartOp();};
				~CVCExecution() {m_pCtx->EndOp();};
protected:
	CVCExecutionCtx*	m_pCtx;
};

class COnDropHelper
{
public:
	COnDropHelper();
	~COnDropHelper();

	HDROP m_hItemDropInfo;
	int m_numFiles;
	int m_numActualFiles;
	VSITEMID* m_rgSrcItemIDs;
	IVsHierarchy** m_rgSrcHierarchies;
};

#endif // !defined(AFX_VCPRJNODE_H__1FAE18F5_9021_11D0_8D12_00A0C91BC942__INCLUDED_)
