
// HelpBindHost.h
//
// Declares functions etc. to help implement IBindHost
//

#ifndef __HelpBindHost_H__
#define __HelpBindHost_H__

void CleanupUrlmonStubs();

STDAPI AllocBindHostService(LPCTSTR szContainerFileName,
    IServiceProvider **ppsp);

	// Wrappers for URLMON functions.
// These wrappers are defined so we can run without urlmon.dll existing
// on the target system. This is useful only for the Netscape Plugin.
//
// These definitions depend on symbols defined in urlmon.h (e.g.
// IBindStatusCallback). 
STDAPI HelpCreateAsyncBindCtx(DWORD reserved, IBindStatusCallback *pBSCb, IEnumFORMATETC *pEFetc, IBindCtx **ppBC);
STDAPI HelpCreateURLMoniker(LPMONIKER pMkCtx, LPCWSTR szURL, LPMONIKER FAR * ppmk);
STDAPI HelpMkParseDisplayNameEx(IBindCtx *pbc, LPCWSTR szDisplayName, ULONG *pchEaten, LPMONIKER *ppmk);
STDAPI HelpRegisterBindStatusCallback(LPBC pBC, IBindStatusCallback *pBSCb, IBindStatusCallback** ppBSCBPrev, DWORD dwReserved);
STDAPI HelpRevokeBindStatusCallback(LPBC pBC, IBindStatusCallback *pBSCb);
STDAPI HelpURLOpenStreamA(LPUNKNOWN punk, LPCSTR szURL, DWORD dwReserved, LPBINDSTATUSCALLBACK pbsc);
STDAPI HelpURLDownloadToCacheFileA(LPUNKNOWN punk, LPCSTR szURL, LPTSTR szFile, DWORD cch, DWORD dwReserved, LPBINDSTATUSCALLBACK pbsc);

#endif // __HelpBindHost_H__
