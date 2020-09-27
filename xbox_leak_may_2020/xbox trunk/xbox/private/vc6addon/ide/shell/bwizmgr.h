/////////////////////////////////////////////////////////////////////////////
//	BWIZMGR.H
//		Defines the IBuilderWizardManager object 

#ifndef __BWIZMGR_H__
#define __BWIZMGR_H__

#include <ocdesign.h>
#include "ipcmpctr.h"

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

///////////////////////////////////////////////////////////////////////////////
// CBuilderWizardManager
//
class CBuilderWizardManager : public CCmdTarget
{
	DECLARE_DYNAMIC(CBuilderWizardManager);

public:
	CBuilderWizardManager();
	virtual ~CBuilderWizardManager();

protected:
	BOOL DoesDevCategoryExist(LPCTSTR szCategory, HKEY *pKeyOut = NULL);

#if _DEBUG
// enusure that the system compcat registry entries
// have been properly setup
	BOOL DoesSysCategoryExist(LPCTSTR szCategory);
#endif

	BOOL GetDefaultCompCatCLSID(HKEY hCATIDKey, CLSID *pclsidBuilder);
	BOOL GetCompCatCLSID(HKEY hCATIDKey, 
					DWORD nKeyNumber, CLSID *pclsidBuilder);

        
// Interface Maps   
protected:
	// ITrackSelection
	BEGIN_INTERFACE_PART(BuilderWizardManager, IBuilderWizardManager)
		INIT_INTERFACE_PART(CBuilderWizardManager, BuilderWizardManager)
        STDMETHOD(DoesBuilderExist)(REFGUID rguidBuilder);
        STDMETHOD(MapObjectToBuilderCLSID)(REFCLSID rclsidObject, 
					DWORD dwPromptOpt, HWND hwndOwner, 
					CLSID *pclsidBuilder);
        STDMETHOD(MapBuilderCATIDToCLSID)(REFGUID rguidBuilder, 
					DWORD dwPromptOpt, HWND hwndOwner, 
					CLSID *pclsidBuilder);
		STDMETHOD(GetBuilder)(REFGUID rguidBuilder, DWORD grfGetOpt, 
					HWND hwndPromptOwner, IDispatch **ppdispApp,
					HWND *pwndBuilderOwner, REFIID riidBuilder, 
					IUnknown **ppunkBuilder);
		STDMETHOD(EnableModeless)(BOOL fEnable);
	END_INTERFACE_PART(BuilderWizardManager)

	DECLARE_INTERFACE_MAP()

public:
	static TCHAR *szSysCompCatKey;
};

///////////////////////////////////////////////////////////////////////////////
// CIPCompBuilderWizardManager
//
class CIPCompBuilderWizardManager : public CCmdTarget
{
	DECLARE_DYNAMIC(CIPCompBuilderWizardManager);

public:
	CIPCompBuilderWizardManager(CIPCompContainerItem *pItem);
	virtual ~CIPCompBuilderWizardManager();

// Interface Maps   
protected:
	// ITrackSelection
	BEGIN_INTERFACE_PART(IPCompBuilderWizardManager, IBuilderWizardManager)
		INIT_INTERFACE_PART(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
        STDMETHOD(DoesBuilderExist)(REFGUID rguidBuilder);
        STDMETHOD(MapObjectToBuilderCLSID)(REFCLSID rclsidObject, 
					DWORD dwPromptOpt, HWND hwndOwner, 
					CLSID *pclsidBuilder);
        STDMETHOD(MapBuilderCATIDToCLSID)(REFGUID rguidBuilder, 
					DWORD dwPromptOpt, HWND hwndOwner, 
					CLSID *pclsidBuilder);
		STDMETHOD(GetBuilder)(REFGUID rguidBuilder, DWORD grfGetOpt, 
					HWND hwndPromptOwner, IDispatch **ppdispApp,
					HWND *pwndBuilderOwner, REFIID riidBuilder,
					IUnknown **ppunkBuilder);
		STDMETHOD(EnableModeless)(BOOL fEnable);
	END_INTERFACE_PART(IPCompBuilderWizardManager)

	DECLARE_INTERFACE_MAP()

protected:
	COleRef<IBuilderWizardManager>	 m_srpBuilderWizardManager;
	CIPCompContainerItem			*m_pContainerItem;
};

#ifdef _DEBUG
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

#undef AFX_DATA
#define AFX_DATA NEAR

#endif		// __BWIZMGR_H__
