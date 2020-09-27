//	VCPKG.H
//
//
//	VC package interface

#pragma once

// Forward Declarations
//

class CStateSaver;
#include <commctrl.h>
// Package Component Interface Access
//

// So we do not have to #include the headers.
//


struct IVCPackage;
struct IVCPackageComponent;
struct IBuildSystem;
struct IBuildSystem2;			
struct IVCSourceParser;
struct ICodeManager;				
struct ICodeManagerMenuBuilder;	
class  VCCodeModelService;	
struct IDesignerObjectFilters;	
struct IProjectWorkspaceWindow;	
__interface INCB;
struct BscEx;
struct IVsUIShell;

extern IVCPackage *					GetIVCPackage				(void);
extern IOleCommandTarget*			GetIVCCommandTarget			(void);
extern IBuildSystem *				GetIBuildSystem				(void);
extern IBuildSystem2 *				GetIBuildSystem2			(void);
extern ICodeManager *				GetICodeManager				(void);
extern ICodeManagerMenuBuilder *	GetICodeManagerMenuBuilder	(void);
extern VCCodeModelService *			GetVCCodeModelService		(void);
extern IDesignerObjectFilters *		GetIDesignerObjectFilters	(void);
extern IProjectWorkspaceWindow *	GetIProjectWorkspaceWindow	(void);
extern IVCSourceParser *            GetIVCSourceParser          (void);
extern BscEx *						GetSolutionBSC				(void);
extern IDispatch * GetCVSelectedDispatch(void);
extern void SetCVSelectedDispatch(IDispatch* pDispSelected);
#if defined(_NCB_PACKAGE_COMPONENT_)
extern INCB *						GetINCB						(void);
#endif // _NCB_PACKAGE_COMPONENT_
// HACK! HACK! Remove as soon as we are using the DTE to get to the Solution
// object.
//

class IWorkspaceDO_Internal;
extern IWorkspaceDO_Internal * GetWorkspaceDO(void);

// Remove this implementation as you implement these for real in the appropriate 
// static library.
//

inline IBuildSystem *				GetIBuildSystem				(void) { return NULL; }
inline IBuildSystem2 *				GetIBuildSystem2			(void) { return NULL; }
inline ICodeManager *				GetICodeManager				(void) { return NULL; }
inline ICodeManagerMenuBuilder *	GetICodeManagerMenuBuilder	(void) { return NULL; }
inline IDesignerObjectFilters *		GetIDesignerObjectFilters	(void) { return NULL; }

// Package Component Creation
//
// These are defined by their respective package extensions
//

extern HRESULT CreateClassViewPackageComponent(IVCPackageComponent** ppIVCPackageComponent);
extern HRESULT CreateClassWinPackageComponent(IVCPackageComponent** ppIVCPackageComponent);
// this one is the old taliesin stuff
//extern HRESULT CreateVCDesignerPackageComponent(IVCPackageComponent** ppIVCPackageComponent);
#if defined(_NCB_PACKAGE_COMPONENT_)
extern HRESULT CreateNcbPackageComponent(IVCPackageComponent ** ppIVCPackageComponent);
#endif // _NCB_PACKAGE_COMPONENT_
extern HRESULT CreateParserPackageComponent(IVCPackageComponent ** ppIVCPackageComponent);
extern HRESULT CreateCodeModelPackageComponent(IVCPackageComponent ** ppIVCPackageComponent);
extern HRESULT CreateIsensePackageComponent(IVCPackageComponent ** ppIVCPackageComponent);
extern HRESULT CreateCRToolsPackageComponent(IVCPackageComponent ** ppIVCPackageComponent);

#define IMGLIST_WIDTH           16
#define IMGLIST_BACKGROUND      0x0000ff00

enum VCPackageComponent 
{
	vcPackageComponentInvalid = -1,
	vcPackageComponentCodeModel = 0,
#if defined(_NCB_PACKAGE_COMPONENT_)
	vcPackageComponentNCB,
#endif // _NCB_PACKAGE_COMPONENT_
	vcPackageComponentParser,
//	vcPackageComponentTaliesin,
	vcPackageComponentCRTools,
	vcPackageComponentIsense,
#ifndef DAVEWAGON
	vcPackageComponentLast,
#endif
	vcPackageComponentClassView = vcPackageComponentIsense+1,
	vcPackageComponentClassWin,
#ifdef DAVEWAGON
	vcPackageComponentLast,
#endif
	vcPackageComponentCppService = vcPackageComponentClassWin+1 // After vcPackageComponentLast because it is not a package component
};


struct __declspec(uuid("{6025237E-ABF9-11d2-AA84-00C04F72DB55}")) IVCPackage : public IUnknown
{
	STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void** ppv) = 0; // Calls Shell's query service
	STDMETHOD(Init)(void) = 0;
	STDMETHOD_(HIMAGELIST, GetImageList)(void) = 0;
	STDMETHOD_(LPDLGTEMPLATE, LoadDialog)	(IVCPackageComponent * pIVCPackageComponent, UINT nIDResource) = 0;
	STDMETHOD_(HCURSOR, LoadCursor)			(IVCPackageComponent * pIVCPackageComponent, UINT nIDResource) = 0;
	STDMETHOD_(HICON, LoadIcon)				(IVCPackageComponent * pIVCPackageComponent, UINT nIDResource) = 0;
	STDMETHOD_(HBITMAP, LoadBitmap)			(IVCPackageComponent * pIVCPackageComponent, UINT nIDResource) = 0;
	STDMETHOD_(void, LoadString)			(IVCPackageComponent * pIVCPackageComponent, UINT nIDResource, BSTR*) = 0;
	STDMETHOD_(BOOL, PlaySound)				(IVCPackageComponent * pIVCPackageComponent, UINT nIDResource) = 0;
	STDMETHOD(VCMessageBox)(LPCWSTR wstrMsg) = 0;
};


// CVCPackageResourceManager
//
class CVCPackageResourceManager
{
public :
	CVCPackageResourceManager(void) {};
	void SetSite(IServiceProvider *pVsShellSP);

	// IVCPackageResourceManager
	STDMETHOD_(LPDLGTEMPLATE, LoadDialog)(VCPackageComponent vcPackageID, UINT nIDResource);
	STDMETHOD_(HCURSOR, LoadCursor)(VCPackageComponent vcPackageID, UINT nIDResource);
	STDMETHOD_(HICON, LoadIcon)(VCPackageComponent vcPackageID, UINT nIDResource);
	STDMETHOD_(HBITMAP, LoadBitmap)(VCPackageComponent vcPackageID, UINT nIDResource);
	STDMETHOD_(void, LoadString)(VCPackageComponent vcPackageID, UINT nIDResource, BSTR*);
	STDMETHOD_(BOOL, PlaySound)(VCPackageComponent vcPackageID, UINT nIDResource);
private:
	CComPtr<IServiceProvider>	m_spVsShellSP;
	HINSTANCE GetResourceInstance();
	bool VerifyPackageComponentResourceRange(VCPackageComponent vcPackageID, UINT nIDResource) const;
	friend class CVCPackage;

};


// helper class to synchronize SuspendParse and ResumeParse
class CSmartSuspendResumeThread
{
public:
	CSmartSuspendResumeThread();
	virtual ~CSmartSuspendResumeThread();
private:
	static bool bSuspend;

};

// IVCPackageComponent
//

struct __declspec(uuid("{6025237F-ABF9-11d2-AA84-00C04F72DB55}")) IVCPackageComponent : public IUnknown
{
	STDMETHOD_(UINT, GetPackageID)(void) const = 0;
	STDMETHOD(OnInit)(void) { return S_FALSE; }

	STDMETHOD(OnPreExit)(void) { return S_FALSE; }
	STDMETHOD(OnExit)(void) { return S_FALSE; }
	STDMETHOD(OnNotify)(DWORD dwID, DWORD dwHint) { return S_FALSE; }
	STDMETHOD_(BOOL, OnIdle)(long lIdleCount) { return FALSE; }
	// Registry Serialization
	//

	STDMETHOD(SaveSettings)(void) { return S_FALSE; }

	// Local Solution Level Serialization (.OPT)
	//

	STDMETHOD(SerializeWorkspaceSettings)(CStateSaver & stateSave, DWORD dwFlags) { return S_FALSE; }
	STDMETHOD(SerializeWkspcConvSettings)(CStateSaver & stateSave, DWORD dwFlags) { return S_FALSE; }

	// Shared Project Level Serializtion (.DSW)
	//

	STDMETHOD_(LPCTSTR, GetProjectWorkspaceInfoText)(LPCTSTR lpszProject) { return NULL; }
	STDMETHOD(SetProjectWorkspaceInfoText)(LPCTSTR lpszProject, LPCTSTR lpszInfo) { return S_FALSE; }

	// Shared Solution Level Serialization (.DSW)
	//

	STDMETHOD_(LPCTSTR, GetGlobalWorkspaceInfoText)(void) { return NULL; }
	STDMETHOD(SetGlobalWorkspaceInfoText)(LPCTSTR lpszInfo)	{ return S_FALSE; }
};


// GUIDs required during package registration
//

//class __declspec(uuid("{5A4901A4-BC76-11d2-AAA2-00C04F72DB55}")) ATL_NO_VTABLE CVCTaliesinEditorFactory;


// Clipboard formats (for drag & drop)
//

__declspec(selectany) extern const CLIPFORMAT g_cfDesignerObject(0x7007);

