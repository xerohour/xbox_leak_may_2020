//---------------------------------------------------------------------------
// Microsoft VC++
//
// Microsoft Confidential
// Copyright 1994 - 1997 Microsoft Corporation. All Rights Reserved.
//
// CVCARCHY.h: Definition of the CVCArchy class
//---------------------------------------------------------------------------

#pragma once

#include "vssolutn.h"
#include "fpstfmt.h"
#include "vssccprj.h"
#include "vssccmgr.h"
#include "prjnode.h"
#include "gencfg.h"
#include "vstrkdoc.h"

typedef CVCTypedPtrList<CVCPtrList, CVCNode*> HierNodeList;

// forward decl for helper class
class CShellDropHelper;

//
// When retrieving a shell interface from CVCArchy, perform a QI to get the desired interface.
// We can't perform a simple cast from CVCArchy to any shell interface because that prevents CVCArchy from 
// being aggregated correctly.
//
// Usage:	IVsHierarchy* pHier = VCQI_cast<IVsHierarchy>(this);
//			IVsProject*   pProj = VCQI_cast<IVsProject>(this);
//			etc.
//
template <typename T> CComPtr<T> VCQI_cast( const CVCArchy* pCVCArchy) 
{ 
	CComPtr<T>	pT;
	if( pCVCArchy )
	    ((CVCArchy*)pCVCArchy)->QueryInterface( __uuidof(T), (void**)&pT );
	ASSERT( pT );
	return pT;
}

class CVCArchy : 
	public IVsUIHierarchy,
	public IOleCommandTarget,
	public IVsPersistHierarchyItem,
	public IPersistFileFormat,	// derives from IPersist
	public IVsProject3,		// derives from IProject
	public IVsCfgProvider2,
	public IExtractIconW,
	public IVsHierarchyDeleteHandler,
	public IDispatchImpl<IDispatch, &IID_IDispatch>,
	public IVsProjectCfgProvider,
	public IVsPersistSolutionOpts,
	public IVsSccProject2,
	public IVsProjectResources,
	public IVsSupportItemHandoff,
	public IVsHierarchyDropDataSource,
	public IVsUpdateSolutionEvents,
	public IVsHierarchyDropDataTarget,
	public IVsUIHierWinClipboardHelperEvents,
	public IVsExtensibleObject,
	public IVsProjectSpecificEditorMap2,
	public CComObjectRoot
{
public:
	CVCArchy(void);
	virtual ~CVCArchy(void);

BEGIN_COM_MAP(CVCArchy)
	COM_INTERFACE_ENTRY(IVsHierarchy)		// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsUIHierarchy)		// derives from IVsHierarchy
	COM_INTERFACE_ENTRY(IOleCommandTarget)	// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsPersistHierarchyItem) // derives from IUnknown
	COM_INTERFACE_ENTRY(IPersist)					// derives from IUnknown
	COM_INTERFACE_ENTRY(IPersistFileFormat)			// derives from IPersist
	COM_INTERFACE_ENTRY(IVsProject)					// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsProject2)				// derives from IVsProject
	COM_INTERFACE_ENTRY(IVsProject3)				// derives from IVsProject2
	COM_INTERFACE_ENTRY(IVsSupportItemHandoff)
	COM_INTERFACE_ENTRY(IVsPersistSolutionOpts)
	COM_INTERFACE_ENTRY(IVsSccProject2)				// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsProjectCfgProvider)		// derives from IVsCfgProvider
	COM_INTERFACE_ENTRY(IVsCfgProvider2)			// derives from IVsCfgProvider
	COM_INTERFACE_ENTRY2(IVsCfgProvider, IVsProjectCfgProvider)	// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsHierarchyDeleteHandler)				// derives from IUnknown
	COM_INTERFACE_ENTRY_IID(IID_IExtractIcon, IExtractIconW)	// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsHierarchyDropDataSource)	// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsHierarchyDropDataTarget)	// derives from IUnknown
	COM_INTERFACE_ENTRY(IVsUIHierWinClipboardHelperEvents)	// derives from IUnknown
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVsProjectResources)
	COM_INTERFACE_ENTRY(IVsUpdateSolutionEvents)
	COM_INTERFACE_ENTRY(IVsExtensibleObject)
	COM_INTERFACE_ENTRY(IVsProjectSpecificEditorMap2)
	COM_INTERFACE_ENTRY(IVsProjectSpecificEditorMap)
END_COM_MAP()
DECLARE_AGGREGATABLE(CVCArchy)
// DECLARE_POLY_AGGREGATABLE(CVCArchy)
// DECLARE_NOT_AGGREGATABLE(CVCArchy)


public:	
// IVsHierarchyEvent propagation
	HRESULT OnItemAdded(CVCNode *pNodeParent, CVCNode *pNodePrev, CVCNode *pNodeAdded) const;
	HRESULT OnItemDeleted(CVCNode *pNode) const;
	HRESULT OnPropertyChanged(CVCNode *pNode, VSHPROPID propid, DWORD flags) const;
	HRESULT OnInvalidateItems(CVCNode *pNode) const;
	HRESULT	OnInvalidateIcon(HICON hIcon) const;

public:
//---------------------------------------------------------------------------
// interface: IVsProject
//---------------------------------------------------------------------------

	STDMETHOD(IsDocumentInProject)(LPCOLESTR pszMkDocument, BOOL* pfFound, VSDOCUMENTPRIORITY* pdwPriority, VSITEMID* pvsid);
	STDMETHOD(GetMkDocument)(VSITEMID vsid, BSTR *pbstrMkDocument); 
	STDMETHOD(OpenItem)(VSITEMID vsid, REFGUID guidDocViewType, IUnknown* punkDocDataExisting, IVsWindowFrame** ppWindowFrame);
	STDMETHOD(GetItemContext)(VSITEMID vsid, IServiceProvider **ppSP);
	STDMETHOD(GenerateUniqueItemName)(VSITEMID itemidLoc, LPCOLESTR pszExt, LPCOLESTR pszSuggestedRoot, BSTR* pbstrItemName);
	STDMETHOD(AddItem)(/*[in]*/ VSITEMID itemidLoc, /*[in]*/ VSADDITEMOPERATION dwAddItemOperation, 
		/*[in]*/ LPCOLESTR pszItemName, /*[in]*/ DWORD cFilesToOpen, 
		/*[in, size_is(cFilesToOpen)]*/ LPCOLESTR rgpszFilesToOpen[], /*[in]*/ HWND hwndDlg, 
		/*[out, retval]*/ VSADDRESULT* pResult);
	STDMETHOD (Close)(void);

//---------------------------------------------------------------------------
// interface: IVsProject2
//---------------------------------------------------------------------------
	STDMETHOD(RemoveItem)(/*[in]*/ DWORD dwReserved, /*[in]*/ VSITEMID itemid, /*[out, retval]*/ BOOL __RPC_FAR *pfResult);
	STDMETHOD(ReopenItem)(/*[in]*/ VSITEMID itemid, /*[in]*/ REFGUID rguidEditorType, /*[in]*/ LPCOLESTR pszPhysicalView,
		/*[in]*/ REFGUID rguidLogicalView, /*[in]*/ IUnknown __RPC_FAR *punkDocDataExisting,
		/*[out, retval]*/ IVsWindowFrame __RPC_FAR *__RPC_FAR *ppWindowFrame);

// helper
protected:
	HRESULT DoRemoveItem(VSITEMID itemid, BOOL __RPC_FAR *pfResult, BOOL bCloseIfOpen = TRUE);
	HRESULT DoAddItem(/*[in]*/ VSITEMID itemidLoc, /*[in]*/ VSADDITEMOPERATION dwAddItemOperation,
		/*[in]*/ LPCOLESTR pszItemName, /*[in]*/ DWORD cFilesToOpen, 
		/*[in, size_is(cFilesToOpen)]*/ LPCOLESTR rgpszFilesToOpen[], /*[in]*/ HWND hwndDlg, 
		/*[out, retval]*/ VSADDRESULT* pResult);

//---------------------------------------------------------------------------
// interface: IVsProject3
//---------------------------------------------------------------------------
public:
	// AddItemWithSpecific is used to add item(s) to the project and additionally ask the project to
	// open the item using the specified editor information.  An extension of IVsProject::AddItem().
	STDMETHOD(AddItemWithSpecific)( VSITEMID itemidLoc, VSADDITEMOPERATION dwAddItemOperation, LPCOLESTR pszItemName,
		ULONG cFilesToOpen, LPCOLESTR rgpszFilesToOpen[], HWND hwndDlg, VSSPECIFICEDITORFLAGS grfEditorFlags,
		REFGUID rguidEditorType, LPCOLESTR pszPhysicalView, REFGUID rguidLogicalView, VSADDRESULT* pResult);
	// OpenItemWithSpecific is used to ask the project to open the item using the
	// specified editor information.  An extension of IVsProject::OpenItem().
	STDMETHOD(OpenItemWithSpecific)( VSITEMID itemid, VSSPECIFICEDITORFLAGS grfEditorFlags, REFGUID rguidEditorType,
		LPCOLESTR pszPhysicalView, REFGUID rguidLogicalView, IUnknown* punkDocDataExisting, IVsWindowFrame** ppWindowFrame);
	// TransferItem is used to transfer ownership of a running document to the project.
	// The project should call IVsRunningDocumentTable::RenameDocument to transfer ownership
	// of the document to its hierarchy and give the document a new itemid within the project.
	STDMETHOD(TransferItem)( LPCOLESTR pszMkDocumentOld,  // passed as pszDocumentOld to IVsRunningDocumentTable::RenameDocument
					   LPCOLESTR pszMkDocumentNew,  // passed as pszDocumentNew to IVsRunningDocumentTable::RenameDocument
					   IVsWindowFrame *punkWindowFrame);  // optional if document not open

//---------------------------------------------------------------------------
// IExtractIconW
//---------------------------------------------------------------------------
public:
	STDMETHOD(GetIconLocation)(UINT uFlags, LPWSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags); 
	STDMETHOD(Extract)(LPCWSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize); 

// IPersist
	STDMETHOD(GetClassID)(/*[out]*/ CLSID *pClassID);

// IPersistFileFormat
public:
	STDMETHOD(IsDirty)(BOOL __RPC_FAR *pfIsDirty);
	STDMETHOD(InitNew)(/*[in]*/ DWORD nFormatIndex);
	STDMETHOD(Load)(/*[in]*/ LPCOLESTR pszFilename, /*[in]*/ DWORD grfMode, /*[in]*/ BOOL fReadOnly);
	STDMETHOD(Save)(/*[in]*/ LPCOLESTR pszFilename, /*[in]*/ BOOL fRemember, /*[in]*/ DWORD nFormatIndex);
	STDMETHOD(SaveCompleted)(/*[unique, in]*/ LPCOLESTR pszFileName);
	STDMETHOD(GetCurFile)(/*[out]*/ LPOLESTR __RPC_FAR *ppszFilename, /*[out]*/ DWORD __RPC_FAR *pnFormatIndex);
	STDMETHOD(GetFormatList)(/*[out]*/ LPOLESTR __RPC_FAR *ppszFormatList);

//	IVsProjectCfgProvider2 IVsCfgProvider2
public:
	STDMETHOD(OpenProjectCfg)(/*[in]*/ LPCOLESTR szProjectCfgCanonicalName, IVsProjectCfg **ppIVsProjectCfg);
	STDMETHOD(get_UsesIndependentConfigurations)(/*[out]*/ BOOL *pfUsesIndependentConfigurations);
	STDMETHOD(GetCfgs)(/*[in]*/ ULONG celt, /*[in, out, size_is(celt)]*/ IVsCfg *rgpcfg[], /*[out, optional]*/ ULONG* pcActual,
		/*[out, optional]*/ VSCFGFLAGS *prgfFlags);
    STDMETHOD(GetCfgNames)(/*[in]*/ ULONG celt, /*[size_is, out, in]*/ BSTR rgbstr[], /*[optional, out]*/ ULONG* pcActual);
    STDMETHOD(GetPlatformNames)(/*[in]*/ ULONG celt, /*[size_is, out, in]*/ BSTR rgbstr[], /*[optional][out]*/ ULONG* pcActual);
    STDMETHOD(GetCfgOfName)(/*[in]*/ LPCOLESTR pszCfgName, /*[in]*/ LPCOLESTR pszPlatformName, /*[out]*/ IVsCfg** ppCfg);
    STDMETHOD(AddCfgsOfCfgName)(/*[in]*/ LPCOLESTR pszCfgName, /*[in]*/ LPCOLESTR pszCloneCfgName, /*[in]*/ BOOL fPrivate);
    STDMETHOD(DeleteCfgsOfCfgName)(/*[in]*/ LPCOLESTR pszCfgName);
    STDMETHOD(RenameCfgsOfCfgName)(/*[in]*/ LPCOLESTR pszOldName, /*[in]*/ LPCOLESTR pszNewName);
    STDMETHOD(AddCfgsOfPlatformName)(/*[in]*/ LPCOLESTR pszPlatformName, /*[in]*/ LPCOLESTR pszClonePlatformName);
    STDMETHOD(DeleteCfgsOfPlatformName)(/*[in]*/ LPCOLESTR pszPlatformName);
    STDMETHOD(GetSupportedPlatformNames)(/*[in]*/ ULONG celt, /*[size_is, out, in]*/ BSTR rgbstr[], 
		/*[optional, out]*/ ULONG*pcActual);
    STDMETHOD(GetCfgProviderProperty)(/*[in]*/ VSCFGPROPID propid, /*[out]*/ VARIANT*pvar);
	STDMETHOD(AdviseCfgProviderEvents)(/*[in]*/ IVsCfgProviderEvents*pCPE, /*[out]*/ VSCOOKIE* pdwCookie);
    STDMETHOD(UnadviseCfgProviderEvents)(/*[in]*/ VSCOOKIE dwCookie);

// IVsHierarchyDeleteHandler
public:
	STDMETHOD(QueryDeleteItem)(/*[in]*/ VSDELETEITEMOPERATION dwDelItemOp, /*[in]*/ VSITEMID itemid,
		/*[out, retval]*/ BOOL __RPC_FAR *pfCanDelete);
	STDMETHOD(DeleteItem)(/*[in]*/ VSDELETEITEMOPERATION dwDelItemOp, /*[in]*/ VSITEMID itemid);

// helpers
protected:
	HRESULT DoDeleteItem(VSDELETEITEMOPERATION dwDelItemOp, VSITEMID itemid, BOOL bCloseIfOpen = TRUE);

// IVsSccProject2
public:
	// helper method for SccGlyphChanged:
	void RecurseChildrenAndRefreshGlyphs( CVCNode* pHead );
    STDMETHOD(SccGlyphChanged)(/*[in]*/ int cAffectedNodes, /*[size_is, in]*/ const VSITEMID rgitemidAffectedNodes[],
        /*[size_is, in]*/ const VsStateIcon rgsiNewGlyphs[], /*[size_is, in]*/ const DWORD rgdwNewSccStatus[]);
    STDMETHOD(SetSccLocation)(/*[in]*/ LPCOLESTR pszSccProjectName, /*[in]*/ LPCOLESTR pszSccAuxPath,
		/*[in]*/ LPCOLESTR pszSccLocalPath, /*[in]*/ LPCOLESTR pszSccProvider);
    STDMETHOD(GetSccFiles)(/*[in]*/ VSITEMID, /*[out]*/ CALPOLESTR* pCaStringsOut, /*[out]*/ CADWORD* pCaFlagsOut);
    STDMETHOD(GetSccSpecialFiles)(/*[in]*/ VSITEMID, /*[in]*/ LPCOLESTR pszSccFile, /*[out]*/ CALPOLESTR *pCaStringsOut,
        /*[out]*/ CADWORD* pCaFlagsOut);

// IVsPersistSolutionOpts
public:
    STDMETHOD(SaveUserOptions)(IVsSolutionPersistence *pPersistence)
		{return E_NOTIMPL; /* CVCArchy::SaveUserOptions */ }
    STDMETHOD(LoadUserOptions)(IVsSolutionPersistence *pPersistence, VSLOADUSEROPTS grfLoadOpts)
		{return E_NOTIMPL; /* CVCArchy::LoadUserOptions */ }
    STDMETHOD(WriteUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);
    STDMETHOD(ReadUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);

// IVsHierarchy
public:
	STDMETHOD(SetSite)(/*[in]*/ IServiceProvider *pSP);
	STDMETHOD(GetSite)(/*[out]*/ IServiceProvider **ppSP);
	STDMETHOD(QueryClose)(/*[out]*/ BOOL *pfCanClose);
// 	STDMETHOD(Close)(void);
	STDMETHOD(GetGuidProperty)(/*[in]*/ VSITEMID itemid, /*[in]*/ VSHPROPID propid, /*[out]*/ GUID* pguid);
	STDMETHOD(SetGuidProperty)(/*[in]*/ VSITEMID itemid, /*[in]*/ VSHPROPID propid, /*[in]*/ REFGUID guid);
	STDMETHOD(GetProperty)(/*[in]*/ VSITEMID itemid, /*[in]*/ VSHPROPID propid, /*[out]*/ VARIANT* pvar);
	STDMETHOD(SetProperty)(/*[in]*/ VSITEMID itemid, /*[in]*/ VSHPROPID propid, /*[in]*/ VARIANT var);
 	STDMETHOD(GetNestedHierarchy)(/*[in]*/ VSITEMID itemid, /*[in]*/ REFIID riid, /*[out]*/ void** ppHierarchyNested,
		/*[out]*/ VSITEMID* pitemidNested);
	STDMETHOD(GetCanonicalName)(/*[in]*/ VSITEMID itemid, /*[out]*/ BSTR* ppszName);
	STDMETHOD(ParseCanonicalName)(/*[in]*/ LPCOLESTR pszName, /*[out]*/ VSITEMID* pitemid);
	STDMETHOD(GetAutomationObject)(/*[out]*/ IDispatch** ppdisp);
	STDMETHOD(AdviseHierarchyEvents)(/*[in]*/ IVsHierarchyEvents *pEventSink, /*[out]*/ VSCOOKIE* pdwCookie);
	STDMETHOD(UnadviseHierarchyEvents)(/*[in]*/ VSCOOKIE dwCookie);
	STDMETHOD(Unused0)(void);
	STDMETHOD(Unused1)(void);
    STDMETHOD(Unused2)(void);
    STDMETHOD(Unused3)(void);
    STDMETHOD(Unused4)(void);
       
	// helpers for IVsHierarchy
	virtual HRESULT GetProperty(/*[in]*/ CVCNode* pNode, /*[in]*/ VSHPROPID propid, /*[out]*/ VARIANT* pvar);
	virtual HRESULT SetProperty(/*[in]*/ CVCNode* pNode, /*[in]*/ VSHPROPID propid, /*[in]*/ const VARIANT& var);

// IVsUIHierarchy
public:
    STDMETHOD (QueryStatusCommand)(/*[in]*/ VSITEMID itemid, /*[in, unique]*/ const GUID* pguidCmdGroup, /*[in]*/ ULONG cCmds,
		/*[in, out, size_is(cCmds)]*/ OLECMD prgCmds[], /*[in, out, unique]*/ OLECMDTEXT* pCmdText);
	STDMETHOD(ExecCommand)(/*[in]*/ VSITEMID itemid, /*[in, unique]*/ const GUID* pguidCmdGroup, /*[in]*/ DWORD nCmdID,
		/*[in]*/ DWORD nCmdexecopt, /*[in, unique]*/ VARIANT* pvaIn, /*[in, out, unique]*/ VARIANT* pvaOut);

// IVsPersistHierarchyItem
public:
    STDMETHOD(IsItemDirty)(/*[in]*/ VSITEMID vsid, /*[in]*/ IUnknown __RPC_FAR* punkDocData, /*[out]*/ BOOL __RPC_FAR* pfDirty);
    STDMETHOD(SaveItem)(/*[in]*/ VSSAVEFLAGS dwSave, /*[in]*/ LPCOLESTR lpstrMkDocument, /*[in]*/ VSITEMID vsid,
        /*[in]*/ IUnknown __RPC_FAR* punkDocData, /*[out]*/ BOOL __RPC_FAR* pfCanceled);

// IVsHierarchyDropDataSource
public:
	STDMETHOD(GetDropInfo)(/*[out]*/ DWORD* pdwOKEffects, /*[out]*/ IDataObject** ppDataObject,
		/*[out]*/ IDropSource** pDropSource);
	STDMETHOD(OnDropNotify)(/*[in]*/ BOOL fDropped, /*[in]*/ DWORD dwEffects);

// IVsHierarchyDropDataTarget
public:
	STDMETHOD(DragEnter)(/*[in]*/ IDataObject* pDataObj, /*[in]*/ DWORD grfKeyState, /*[in]*/ VSITEMID itemid,
		/*[out]*/ DWORD* pdwEffect);
	STDMETHOD(DragOver)(/*[in]*/ DWORD grfKeyState, /*[in]*/ VSITEMID itemid, /*[out]*/ DWORD* pdwEffect);
	STDMETHOD(DragLeave)(void);
	STDMETHOD(Drop)(/*[in]*/ IDataObject* pDataObj, /*[in]*/ DWORD grfKeyState, /*[in]*/ VSITEMID itemid,
		/*[out]*/ DWORD* pdwEffect);

// IVsHierarchyDropDataTarget helpers
	HRESULT GetDropEffect(DWORD grfKeyState, VSITEMID itemid, DWORD *pdwEffect);
	HRESULT GetSupportedDropEffects(DWORD *pdwEffects);
	HRESULT DragOver(DWORD grfKeyState, CVCNode *pNode, DWORD *pdwEffect);
	void AdviseClipboardEvents();
	void UnadviseClipboardEvents();

  // IVsUIHierWinClipboardHelperEvents
  STDMETHOD(OnPaste)(BOOL fDataWasCut, DWORD dwEffects);
  STDMETHOD(OnClear)(BOOL fDataWasCut);

// IOleCommandTarget
public:
	STDMETHOD(QueryStatus)(/*[unique, in]*/ const GUID* pguidCmdGroup, /*[in]*/ ULONG cCmds,
		/*[out, in, size_is]*/ OLECMD prgCmds[], /*[unique, out, in]*/ OLECMDTEXT* pCmdText);
	STDMETHOD(Exec)(/*[unique, in]*/ const GUID* pguidCmdGroup, /*[in]*/ DWORD nCmdID, /*[in]*/ DWORD nCmdexecopt,
		/*[unique, in]*/ VARIANT* pvaIn, /*[unique, out, in]*/ VARIANT* pvaOut);

//---------------------------------------------------------------------------
// ISupportItemHandoff
//---------------------------------------------------------------------------
	STDMETHOD(HandoffItem)(VSITEMID itemid, IVsProject3* pProjDest, LPCOLESTR pszMkDocumentOld, LPCOLESTR pszMkDocumentNew,
		IVsWindowFrame* punkWindowFrame);

//---------------------------------------------------------------------------
// IVsExtensibleObject
//---------------------------------------------------------------------------
	STDMETHOD(GetAutomationObject)(/*[in]*/ LPCOLESTR  pszPropName, /*[out]*/ IDispatch **ppDisp);

// IVsUpdateSolutionEvents
public:
    STDMETHOD(UpdateSolution_Begin)(BOOL* pbCancelUpdate) { return S_OK; }
    STDMETHOD(UpdateSolution_Done)(BOOL bSucceeded, BOOL bModified, BOOL bCancelCommand) { return S_OK; }
    STDMETHOD(UpdateSolution_StartUpdate)(BOOL* pbCancelUpdate) { return S_OK; }
    STDMETHOD(UpdateSolution_Cancel)() { return S_OK; }
    STDMETHOD(OnActiveProjectCfgChange)(IVsHierarchy* pIVsHierarchy);

// IVsProjectResources
public:
	STDMETHOD(GetResourceItem)( VSITEMID itemidDocument, LPCOLESTR pszCulture, VSPROJRESFLAGS grfPRF, VSITEMID* pitemidResource); 
	STDMETHOD(CreateResourceDocData)( VSITEMID itemidResource, IUnknown** punkDocData); 

// IVsProjectSpecificEditorMap
public:
	STDMETHOD(GetSpecificEditorType)(LPCOLESTR pszMkDocument, GUID *pguidEditorType);

// IVsProjectSpecificEditorMap2
public:
	STDMETHOD(GetSpecificLanguageService)(LPCOLESTR pszMkDocument, GUID *pguidLanguageService) { return E_NOTIMPL; }
	STDMETHOD(GetSpecificEditorProperty)(LPCOLESTR pszMkDocument, VSPSEPROPID propid, VARIANT *pvar);
	STDMETHOD(SetSpecificEditorProperty)(LPCOLESTR pszMkDocument, VSPSEPROPID propid, VARIANT var);

// helpers for IVsProjectSpecificEditorMap2
protected:
	BOOL IsResourceFile(LPCOLESTR pszMkDocument);

// Non-interface methods:
protected:
    void SetDirty(boolean fDirty) { m_fDirty = fDirty; }

	virtual UINT GetIconIndex(CVCNode* pNode, ICON_TYPE iconType);

	virtual HRESULT ExecSelection(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT* pvaIn, VARIANT *pvaOut,
		HierNodeList &rgSelection, BOOL bIsHierCmd);	// bIsHierCmd is TRUE if cmd originated via CVSUiHierarchy::ExecCommand

	virtual HRESULT QueryStatusSelection(const GUID* pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT* pCmdText,
		HierNodeList &rgSelection, BOOL bIsHierCmd);	// bIsHierCmd is TRUE if cmd originated via CVSUiHierarchy::ExecCommand

public:
// scc helpers that aren't in any interface.
    HRESULT RegisterSccProject(); // called by CVCProjectFactory
    HRESULT UnregisterSccProject();

	virtual HRESULT DisplayContextMenu(HierNodeList &rgSelection, VARIANT *pvaIn);

	CVCProjectNode* GetProjectNode(void)
	{
		VSASSERT(NULL != m_pRootNode, "Hierarchy has no root node!");
		return static_cast<CVCProjectNode *>(m_pRootNode);
	}
	VSITEMID ItemIDFromName (CVCNode *pRoot, LPCOLESTR pszMkDocument);
	HRESULT	VSITEMID2Node(VSITEMID itemid, CVCNode **pNode);
	HRESULT VSITEMID2Node(VSITEMID itemid, HierNodeList& rgNodeList);
	HRESULT GetSelectedNodes(HierNodeList& rgNodes);

public:
	HRESULT GetVCProject(CVCNode* pNode, VCProject** ppProject);
	HRESULT GetDispVCProject(CVCNode* pNode, IDispatch** ppDispProject);
	HRESULT CreateGenCfg(VCConfiguration* pProjCfg, IVCGenericConfiguration** ppGenCfg);
	HRESULT CompileFileItems(HierNodeList& rgSelection);

	// methods to inform the shell that configs have been
	// added/removed/renamed
	HRESULT InformConfigurationAdded( LPOLESTR pszCfgName );
	HRESULT InformConfigurationRemoved( LPOLESTR pszCfgName );
	HRESULT InformConfigurationRenamed( LPOLESTR pszOldCfgName, LPOLESTR pszNewCfgName );

private:
	VSITEMID DoItemIDFromName(CVCNode *pRoot, VARIANT_BOOL bFullOnly, LPCOLESTR pszMkDocument);

protected:
	HRESULT CanDirtyProject( void );
	CVCNode* ResolveProjNode(CVCNode* pNode);
	HRESULT GetCfgOfName(LPCOLESTR szCfgName, VCConfiguration** ppVCCfg);
	HRESULT GetSupportedPlatformNames(IVCCollection* pPlatforms, ULONG celt, BSTR rgbstr[], ULONG* pcActual);
	VARIANT_BOOL SupportsMultiplePlatforms();

public:	// get/set property methods
	void SetRootNode(CVCNode *pRootNode)
	{
		if (m_pRootNode)
			m_pRootNode->Release();
		m_pRootNode = pRootNode;
	}
	CVCNode* GetRootNode(void) const { return m_pRootNode; }
//	IVsHierarchy* GetVsHierarchy(void) const { return VCQI_cast<IVsHierarchy>(const_cast<CVCArchy*>(this)); }
	void SetImageList(HIMAGELIST hImageList) { m_hImageList = hImageList; }
	HIMAGELIST GetImageList(void) const { return m_hImageList; }
	BOOL IsZombie(void) const { return (m_pRootNode == NULL); }

	// return the controlling IUnk. A CComPtr copy is returned so that we don't lose the addref
	CComPtr<IVsHierarchy>	GetIVsHierarchy(void) const { return VCQI_cast<IVsHierarchy>( this ); }
	// return the controlling IUnk. A CComPtr copy is returned so that we don't lose the addref
	CComPtr<IVsUIHierarchy>   GetIVsUIHierarchy(void) const { return VCQI_cast<IVsUIHierarchy>( this ); }

    HRESULT RegisterClipboardNotifications(BOOL bRegister);
    // Returns our ui hierarchy window.
    HRESULT GetUIHierarchyWindow(IVsUIHierarchyWindow** ppIVsUIHierarchyWindow);

	IUnknown *GetParentHierarchy( void ) { return m_pParentHierarchy; }
	VSITEMID GetParentHierarchyItemID( void ) { return m_dwParentHierarchyItemid; }

	// private data members
private:
    boolean m_fDirty;
	boolean m_fRegisteredWithScc;
 	CComPtr<IVsCfgProviderEventsHelper> m_pCfgProviderEventsHelper;
	HIMAGELIST m_hImageList;
	CVCNode *m_pRootNode;
	static VSCOOKIE g_dwCookie;
	// Properties to support being a used as a nested hierarchy
	IUnknown *m_pParentHierarchy;
	VSITEMID m_dwParentHierarchyItemid;
	// cache this pointer to reduce QS
	CComPtr<IVsMonitorSelection> m_srpMonSel;
	CComPtr<IServiceProvider> m_srpServiceProvider;
	struct EventSink
	{		
		CComPtr<IVsHierarchyEvents> m_srpEventSink;
		DWORD m_dwCookie;
	};
	CSimpleArray<EventSink *> m_rgEventSinks;
	CVCNode* m_pCachedNode;
	UINT m_nCachedIconType;

protected:
    HRESULT DoShellDrop(CVCNode* pDropNode, HDROP hDropInfo, BOOL& bReported);
    HRESULT DoProjItemDrop(CVCNode* pDropNode, CVCOleDataObject& rDataObject, BOOL bDrag, DWORD dwDropEffect, BOOL& bReported);
	HRESULT GetDragDropNodesFromSelection(HierNodeList& rgSelection, CVCNodeArray& rgDragDropNodes);
	HRESULT OnAfterDropOrPaste(BOOL bWasCut, BOOL bWasMove, BOOL bDrag, BOOL bCleanupOnly);
	CVCProjDataSource* GetDataSource(DROPEFFECT* pde);
	HRESULT DragEnter(IDataObject* pDataObj, CVCNode* pNode);
	HRESULT OnCommandCut();
	HRESULT OnCommandCopy();
	HRESULT FlushClipboard();
	inline CVCExecutionCtx& GetExecutionCtx() { return CBuildPackage::s_executionCtx; }
	BOOL IsDropTargetCandidate(CVCNode* pNode);
	HRESULT PrepareShellDrop(CVCNode* pDropNode, CShellDropHelper& dropHelper, HDROP hDropInfo, int& iNumActual,
		bool& bContainsFolders);

public:
    HRESULT PasteFromClipboard(CVCNode *pDropNode);
    BOOL    CanPasteFromClipboard();

public:
	static BOOL	s_bStartedDrag;

    enum //Drop types
    {
        DT_NONE,
        DT_SHELL,
        DT_PROJITEM,
		DT_VSITEM,
		DT_VSREFITEM
    };

    int             m_dropType;
    BOOL            m_bInternalDrop;
	VSCOOKIE	    m_dwClipEvtCookie;// cookie for IVsUIHierWinClipboardHelperEvents advise

    // Our drag-drop and clipboard data objects.
    CVCProjDataSource*  m_pDragDropDataObject;
    CVCProjDataSource*  m_pClipboardDataObject;
    BOOL            m_bWasCut;
};

class CVCTrackProjectDocumentsBatcher
{
public:
            CVCTrackProjectDocumentsBatcher(BOOL bBatch);
            ~CVCTrackProjectDocumentsBatcher();

    HRESULT BeginBatch();
    HRESULT EndBatch();

protected:
    int m_cBatches;
	CComPtr<IVsTrackProjectDocuments> m_spTracker;
};

class CShellDropHelper
{
public:
	CShellDropHelper() : m_numFiles(0), m_rgFiles(NULL), m_rgActualFiles(NULL), m_hr(S_OK) {}
	~CShellDropHelper();
	int m_numFiles;
	LPWSTR* m_rgFiles;
	LPWSTR* m_rgActualFiles;
	HRESULT m_hr;
};

