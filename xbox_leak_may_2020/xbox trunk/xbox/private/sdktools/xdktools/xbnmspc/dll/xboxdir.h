/*****************************************************************************
 *	xboxdir.h
 *****************************************************************************/

#ifndef _XBOXDIR_H
#define _XBOXDIR_H

#include "xboxsite.h"
#include "xboxfldr.h"
#include "xboxlist.h"
#include "xboxglob.h"
#include "xboxpl.h"


typedef struct tagSETNAMEOFINFO
{
    LPCITEMIDLIST pidlOld;
    LPCITEMIDLIST pidlNew;
} SETNAMEOFINFO, * LPSETNAMEOFINFO;

int CALLBACK _CompareDirs(LPVOID pvPidl, LPVOID pvXboxDir, LPARAM lParam);

/*****************************************************************************\
    CLASS: CXboxDir

    DESCRIPTION:
        This class is the cache of a directory on some server.  m_pfs identifies
    the server.

    BUGBUG: PERF - PERF - PERF - PERF
        This directory contains the folder contents in the form of a list of
    pidls (m_pflHfpl).  We need to keep them in order based on name so that
    way looking up and changing is fast because of all the work we need to do
    with change notify.  Also, when we go to parse a display name, we look here
    first, so that needs to be fast.
\*****************************************************************************/

class CXboxDir           : public IUnknown
{
public:
    //////////////////////////////////////////////////////
    // Public Interfaces
    //////////////////////////////////////////////////////
    
    // *** IUnknown ***
    virtual STDMETHODIMP_(ULONG) AddRef(void);
    virtual STDMETHODIMP_(ULONG) Release(void);
    virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);


public:
    CXboxDir();
    ~CXboxDir(void);

    // Public Member Functions
    void CollectMotd(HINTERNET hint);
    void SetCache(CXboxPidlList * pflHfpl);
    CXboxPidlList * GetHfpl(void);
    HRESULT GetHint(HWND hwnd, CStatusBar * psb, HINTERNET * phint, IUnknown * punkSite, CXboxFolder * pff);
    void ReleaseHint(HINTERNET hint);
    STDMETHODIMP WithHint(CStatusBar * psb, HWND hwnd, HINTPROC hp, LPCVOID pv, IUnknown * punkSite, CXboxFolder * pff);
    HRESULT SetNameOf(CXboxFolder * pff, HWND hwndOwner, LPCITEMIDLIST pidl, LPCTSTR pszName, DWORD dwReserved, LPITEMIDLIST *ppidlOut);
    BOOL IsRoot(void);
    BOOL IsCHMODSupported(void) {return m_pfs->IsCHMODSupported();};
    BOOL IsUTF8Supported(void) {return m_pfs->IsUTF8Supported();};
    HRESULT GetFindDataForDisplayPath(HWND hwnd, LPCWSTR pwzDisplayPath, LPXBOX_FIND_DATA pwfd, CXboxFolder * pff);
    HRESULT GetFindData(HWND hwnd, LPCWIRESTR pwWireName, LPXBOX_FIND_DATA pwfd, CXboxFolder * pff);
    HRESULT GetNameOf(LPCITEMIDLIST pidl, DWORD shgno, LPSTRRET pstr);
//    HRESULT DisambiguatePidl(LPCITEMIDLIST pidl);
    CXboxSite * GetXboxSite(void);
    CXboxDir * GetSubXboxDir(CXboxFolder * pff, LPCITEMIDLIST pidl, BOOL fPublic);
    HRESULT GetDisplayPath(LPTSTR pszUrlPath, DWORD cchSize);

    LPCITEMIDLIST GetPathPidlReference(void) { return m_pidlXboxDir;};
    LPCITEMIDLIST GetPidlReference(void) { return m_pidl;};
    LPCITEMIDLIST GetPidlFromWireName(LPCWIRESTR pwWireName);
    LPCITEMIDLIST GetPidlFromDisplayName(LPCWSTR pwzDisplayName);
    LPITEMIDLIST GetSubPidl(CXboxFolder * pff, LPCITEMIDLIST pidlRelative, BOOL fPublic);
    HRESULT AddItem(LPCITEMIDLIST pidl);
    HRESULT ChangeFolderName(LPCITEMIDLIST pidlXboxPath);
    HRESULT ReplacePidl(LPCITEMIDLIST pidlSrc, LPCITEMIDLIST pidlDest) { if (!m_pflHfpl) return S_OK; return m_pflHfpl->ReplacePidl(pidlSrc, pidlDest); };
    HRESULT DeletePidl(LPCITEMIDLIST pidl) { if (!m_pflHfpl) return S_OK; return m_pflHfpl->CompareAndDeletePidl(pidl); };

    static HRESULT _SetNameOfCB(HINTERNET hint, HINTPROCINFO * phpi, LPVOID pv, BOOL * pfReleaseHint);
    static HRESULT _GetFindData(HINTERNET hint0, HINTPROCINFO * phpi, LPVOID pv, BOOL * pfReleaseHint);


    // Friend Functions
    friend HRESULT CXboxDir_Create(CXboxSite * pfs, LPCITEMIDLIST pidl, CXboxDir ** ppfd);

    friend int CALLBACK _CompareDirs(LPVOID pvPidl, LPVOID pvXboxDir, LPARAM lParam);


protected:
    int                     m_cRef;

    CXboxSite *              m_pfs;          // The XBOX site I belong to. (WARNING: No Ref Held)
    CXboxPidlList *          m_pflHfpl;      // The items inside this directory
    CXboxGlob *              m_pfgMotd;      // The message of the day
    LPITEMIDLIST            m_pidlXboxDir;   // Name of subdirectory w/o Virtual Root and decoded. Doesn't include Server ID
    LPITEMIDLIST            m_pidl;         // Where we live.  May include the virtual root

    BOOL _DoesItemExist(HWND hwnd, CXboxFolder * pff, LPCITEMIDLIST pidl);
    BOOL _ConfirmReplaceWithRename(HWND hwnd);
    HRESULT _SetXboxDir(CXboxSite * pfs, CXboxDir * pfd, LPCITEMIDLIST pidl);
};

#endif // _XBOXDIR_H
