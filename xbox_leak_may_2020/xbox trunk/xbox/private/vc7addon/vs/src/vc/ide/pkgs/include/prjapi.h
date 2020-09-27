/////////////////////////////////////////////////////////////////////////////
//	PRJAPI.H
//		Project package interface declarations.

#pragma once
#include "vcpkg.h"

__interface IProjectWorkspaceWindow;

typedef IProjectWorkspaceWindow* LPPROJECTWORKSPACEWINDOW;

class CDefProvidedNode;
class CStateSaver;

// These interfaces are used by the build system, class view, and information
// presentation to interact with the project and global workspace windows.

HRESULT CreateClassWinPackageComponent(IVCPackageComponent **ppVCPkgComp);

////////////////////////////////////////////////////////////////////////////
// IProjectWorkspaceWindow

__interface __declspec(uuid("{AD8B7E82-8DA6-11cf-8FB8-00A0C903494F}")) IProjectWorkspaceWindow : public IUnknown
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IProjectWorkspaceWindow methods
	STDMETHOD(AddRootProvidedNode)(CDefProvidedNode *pProvidedNode,	CSlob* pAddSlob) PURE; 	// Non-standard COM.
	STDMETHOD(RemoveSlob)(CSlob *pRemoveSlob, BOOL fRemovePaneIfEmpty) PURE; 	// Non-standard COM.
	STDMETHOD(ActivateContainingPane)(CSlob *pContainedSlob, BOOL bEnsureVisible, BOOL bSetFocusUpon) PURE; 	// Non-standard COM.
	STDMETHOD(FreezeContainingPane)(CSlob *pContainedSlob) PURE; 	// Non-standard COM.
	STDMETHOD(ThawContainingPane)(CSlob *pContainedSlob) PURE; 	// Non-standard COM.
	STDMETHOD(SelectSlob)(CSlob *pSelectSlob, BOOL bClearSelection) PURE; 	// Non-standard COM.
	STDMETHOD(DeselectSlob)(CSlob *pDeselectSlob) PURE; 	// Non-standard COM.
	STDMETHOD(ExpandSlob)(CSlob *pExpandSlob, BOOL bExpandFully) PURE; 	// Non-standard COM.
	STDMETHOD(CollapseSlob)(CSlob *pCollapseSlob) PURE; 	// Non-standard COM.
	STDMETHOD(RefreshSlob)(CSlob *pRefreshSlob) PURE; 	// Non-standard COM.
	STDMETHOD(ScrollSlobIntoView)(CSlob *pViewSlob, BOOL bScrollToTop) PURE; 	// Non-standard COM.
	STDMETHOD(GetVerbPosition)(UINT *pu) PURE;
	STDMETHOD(GetSelectedSlob)(CMultiSlob **ppSlob) PURE; 	// Non-standard COM.
	STDMETHOD(GetSelectedItems)(CMultiSlob *pSlob) PURE;	// Non-standard COM.
	STDMETHOD(IsWindowActive)(VOID) PURE;
	STDMETHOD(IsContainingPaneActive)(CSlob *pContainedSlob) PURE; 	// Non-standard COM.
	STDMETHOD(IsWindowLocked)(VOID) PURE;
	STDMETHOD(PreModalWindow)(VOID) PURE;
	STDMETHOD(PostModalWindow)(VOID) PURE;
	STDMETHOD(BeginUndo)(UINT nID) PURE;
	STDMETHOD(EndUndo)(VOID) PURE;
	STDMETHOD(FlushUndo)(VOID) PURE;
	STDMETHOD(CreateDragger)(CSlobDraggerEx** ppDragger) PURE; 	// Non-standard COM.
	STDMETHOD(GetSlobFromPoint)(POINT *ppt, CSlob **ppSlob) PURE;
	STDMETHOD(GetCurrentSlobRect)(RECT *prt) PURE;
	STDMETHOD(GetSlobRect)(CSlob * pSlob, RECT * prt);
	STDMETHOD(GetSelectedIDispatch)(UINT * piCount, IUnknown *** prgpIDispatch);
};

IProjectWorkspaceWindow *GetIProjectWorkspaceWindow();

/////////////////////////////////////////////////////////////////////////////
//	IPkgDropDataSource interface
//	Use this interface for OLE based project drag drop
#undef  INTERFACE
#define INTERFACE IPkgDropDataSource

DECLARE_INTERFACE_(IPkgDropDataSource, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (void) PURE;
	STDMETHOD_(ULONG,Release) (void) PURE;

	// Data Provider methods for Drop Sources
	STDMETHOD(GetDropInfo) (DWORD *pdwOKEffects, 
							IDataObject **ppDataObject, IDropSource **pDropSource) PURE;
	STDMETHOD(OnDropNotify) (BOOL fDropped, DWORD dwEffects) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IPkgDropDataTarget interface
//	Use this interface for OLE based project drag drop
#undef  INTERFACE
#define INTERFACE IPkgDropDataTarget

DECLARE_INTERFACE_(IPkgDropDataTarget, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (void) PURE;
	STDMETHOD_(ULONG,Release) (void) PURE;

	// Data Provider methods for Drop Target
	STDMETHOD(OnDragEnter)(IDataObject *pDataObj, 
							DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) PURE;
	STDMETHOD(OnDragOver)(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) PURE;
	STDMETHOD(OnDragLeave)(void) PURE;
	STDMETHOD(OnDrop)(IDataObject *pDataObj, 
							DWORD grfKeyState, POINTL pt,DWORD *pdwEffect) PURE;
};
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
