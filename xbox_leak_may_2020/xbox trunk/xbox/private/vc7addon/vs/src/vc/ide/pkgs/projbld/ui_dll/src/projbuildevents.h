#pragma once
#include "vcnode.h"
#include "prjnode.h"
#include "vcfile.h"
#include "vcfgrp.h"

/////////////////////////////////////////////////////////////////////////////
// CVCProjBuildEvents
 
class CVCProjBuildEvents : 
	public IDispatchImpl<IVCProjectEngineEvents, &__uuidof(IVCProjectEngineEvents), &LIBID_VCProjectLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CComObjectRoot
{
// Construction
public:
	CVCProjBuildEvents();   // standard constructor
	~CVCProjBuildEvents();
	static HRESULT CreateInstance(IVCProjectEngineEvents** ppOutput);
BEGIN_COM_MAP(CVCProjBuildEvents)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCProjectEngineEvents)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCProjBuildEvents) 

// IVCProjectEngineEvents
public:
	STDMETHOD(ItemAdded)(IDispatch *pItem, IDispatch *pItemParent);
    STDMETHOD(ItemRemoved)(IDispatch *pItem, IDispatch *pItemParent);
    STDMETHOD(ItemRenamed)(IDispatch *pItem, IDispatch *pItemParent, BSTR bstrOldName);
	STDMETHOD(ItemMoved)(IDispatch* pItem, IDispatch* pNewParent, IDispatch* pOldParent);
    STDMETHOD(ItemPropertyChange)(IDispatch *pItem, long dispid);
	STDMETHOD(SccEvent)(IDispatch *pItem, enumSccEvent event, VARIANT_BOOL *pRet );
	STDMETHOD(ReportError)( BSTR bstrErrMsg, HRESULT hrReport, BSTR bstrHelpKeyword );
	STDMETHOD(ProjectBuildStarted)( IDispatch *pCfg );
	STDMETHOD(ProjectBuildFinished)( IDispatch *pCfg, long warnings, long errors, VARIANT_BOOL bCancelled );

// helpers
protected:
	HRESULT FindProjectParent(IDispatch* pItem, IDispatch* pItemParent, VCProject** ppProject);
	void FindNodeForItem(IDispatch* pItem, CVCNode** ppNode);
	HRESULT FindItemInHierarchy(IDispatch* pItem, IDispatch* pItemParent, CVCProjectNode** ppProjNode,
		CVCFileNode** ppFileNode, CVCFileGroup** ppFileGroup);
	void MakeOutputFileUnique(VCFile* pFile);
};
