/////////////////////////////////////////////////////////////////////////////
//	SHLAPI_.H
//		Private interfaces between shell and devaut1
//		(Other private interfaces implemented by the shell
//		are welcome.  Please, join the party.)

#ifndef __SHLAPI__H__
#define __SHLAPI__H__

/////////////////////////////////////////////////////////////////////////////
//	IAutomationItems interface.  This interface is implemented by
//   the shell, and is called exclusively by devaut1.pkg

// This structure is used to pass information from the shell to
//  devaut1 via IAutomationItems::GetAutomationItems.
#define MAX_LENGTH_NAMED_ITEM 64
struct CAutomationItem
{
	TCHAR m_szName[MAX_LENGTH_NAMED_ITEM];		// Name of named item
	GUID m_tlid;								// GUID of its type library
	GUID m_clsid;								// CLSID of its object
	WORD m_wMajorVersion;						// Major ver.# of typelib
	WORD m_wMinorVersion;						// Minor ver.# of typelib
};

#undef  INTERFACE
#define INTERFACE IAutomationItems

DECLARE_INTERFACE_(IAutomationItems, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IAutomationItems methods

	// The caller (aut1) allocates a *pdwNumItems-count array of CAutomationItem's
	//  in pAutomationItems.  The shell fills in the array with data it gathered
	//  when loading the packages and calling CPackage::GetApplicationExtensions
	//  for each package.  The callee (shell) fills in *pdwNumItems with the number
	//  of elements of the array it filled in.
	// If the caller passes NULL for pAutomationItems, the callee will leave that
	//  alone, and will fill in *pdwNumItems with the total number of items.
	//  The callee can use this to determine how much space to allocate for
	//  the next call to GetAutomationItems.
	STDMETHOD(GetAutomationItems)(CAutomationItem* pAutomationItems, DWORD* pdwNumItems) PURE;

	// Aut1 will call this for each CAutomationItem::m_szName it was passed by the shell
	//  during a previous call to GetAutomationItems.  This is called when VBS requests
	//  of devaut1 an IUnknown for a named item.
	STDMETHOD(GetIDispatch)(LPCTSTR szItemName, IDispatch** ppDispatch) PURE;
};


/////////////////////////////////////////////////////////////////////////////
//	IRecordNotifyShell interface.  This interface is implemented by
//   the shell, and is called exclusively by devaut1.pkg to inform the
//   the shell of state changes with the Macro Recorder.

#undef  INTERFACE
#define INTERFACE IRecordNotifyShell

DECLARE_INTERFACE_(IRecordNotifyShell, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IRecordNotifyShell methods
	STDMETHOD(OnStartRecord)(THIS) PURE;
	STDMETHOD(OnStopRecord)(THIS) PURE;
	STDMETHOD(OnPauseRecord)(THIS) PURE;
	STDMETHOD(OnResumeRecord)(THIS) PURE;
	STDMETHOD(SetShouldRecordText)(THIS_ BOOL bShouldRecordText) PURE;
};


#endif //__SHLAPI__H__
