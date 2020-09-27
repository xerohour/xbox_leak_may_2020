/*****************************************************************************
 *    xboxsite.h
 *****************************************************************************/

#ifndef _XBOXSITE_H
#define _XBOXSITE_H

#include "xboxfldr.h"
#include "xboxlist.h"
#include "xboxinet.h"
#include "xboxurl.h"
#include "account.h"
#include "util.h"

HRESULT SiteCache_PidlLookup(LPCITEMIDLIST pidl, BOOL fPasswordRedir, IMalloc * pm, CXboxSite ** ppfs);


int CALLBACK _CompareSites(LPVOID pvStrSite, LPVOID pvXboxSite, LPARAM lParam);
HRESULT CXboxPunkList_Purge(CXboxList ** pfl);

/*****************************************************************************
 *    CXboxSite
 *****************************************************************************/

class CXboxSite              : public IUnknown
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
    CXboxSite();
    ~CXboxSite();

    // Public Member Functions
    void CollectMotd(HINTERNET hint);
    void ReleaseHint(LPCITEMIDLIST pidlXboxPath, HINTERNET hint);
    HRESULT GetHint(HWND hwnd, LPCITEMIDLIST pidlXboxPath, CStatusBar * psb, HINTERNET * phint, IUnknown * punkSite, CXboxFolder * pff);
    BOOL QueryMotd(void);
    BOOL IsServerVMS(void) {return m_fIsServerVMS;};
    BOOL HasVirtualRoot(void);
    CXboxGlob * GetMotd(void);
    CXboxList * GetCXboxList(void);
    CWireEncoding * GetCWireEncoding(void) {return &m_cwe;};
    HRESULT GetXboxDir(LPCITEMIDLIST pidl, CXboxDir ** ppfd);
    HRESULT GetXboxDir(LPCTSTR pszUrlPath, CXboxDir ** ppfd) {return GetXboxDir(m_pszServer, pszUrlPath, ppfd);};
    HRESULT GetXboxDir(LPCTSTR pszServer, LPCTSTR pszUrlPath, CXboxDir ** ppfd);

    HRESULT GetVirtualRoot(LPITEMIDLIST * ppidl);
    HRESULT PidlInsertVirtualRoot(LPCITEMIDLIST pidlXboxPath, LPITEMIDLIST * ppidl);
    LPCITEMIDLIST GetVirtualRootReference(void) {return (LPCITEMIDLIST) m_pidlVirtualDir;};

    HRESULT GetServer(LPTSTR pszServer, DWORD cchSize) { StrCpyN(pszServer, HANDLE_NULLSTR(m_pszServer), cchSize); return S_OK; };
    HRESULT GetUser(LPTSTR pszUser, DWORD cchSize) { StrCpyN(pszUser, HANDLE_NULLSTR(m_pszUser), cchSize); return S_OK; };
    HRESULT GetPassword(LPTSTR pszPassword, DWORD cchSize) { StrCpyN(pszPassword, HANDLE_NULLSTR(m_pszPassword), cchSize); return S_OK; };
    HRESULT UpdateHiddenPassword(LPITEMIDLIST pidl);
    HRESULT SetRedirPassword(LPCTSTR pszPassword) {Str_SetPtr(&m_pszRedirPassword, pszPassword); return S_OK;};
    HRESULT FlushSubDirs(LPCITEMIDLIST pidl);

    LPITEMIDLIST GetPidl(void);

    BOOL IsCHMODSupported(void) {return m_fIsCHMODSupported;};
    BOOL IsUTF8Supported(void) {return m_fInUTF8Mode;};
    BOOL IsSiteBlockedByRatings(HWND hwndDialogOwner);
    void FlushRatingsInfo(void) {m_fRatingsChecked = m_fRatingsAllow = FALSE;};

    static void FlushHintCB(LPVOID pvXboxSite);


    // Friend Functions
    friend HRESULT CXboxSite_Create(LPCITEMIDLIST pidl, LPCTSTR pszLookupStr, IMalloc * pm, CXboxSite ** ppfs);
    friend HRESULT SiteCache_PidlLookup(LPCITEMIDLIST pidl, BOOL fPasswordRedir, IMalloc * pm, CXboxSite ** ppfs);

    friend int CALLBACK _CompareSites(LPVOID pvStrSite, LPVOID pvXboxSite, LPARAM lParam);
    friend class CXboxView;


protected:
    // Private Member Variables
    int m_cRef;

    BOOL            m_fMotd;            // There is a Motd at all
    BOOL            m_fNewMotd;         // Motd has changed
    HINTERNET       m_hint;             // Session for this site
    LPGLOBALTIMEOUTINFO m_hgti;         // Timeout for the session handle
    CXboxList *     m_XboxDirList;      // List of XboxDir's attached to me. (No Ref Held)
    CXboxGlob *     m_pfgMotd;          //
    IMalloc *       m_pm;               // Used for creating full pidls if needed.

    LPTSTR          m_pszServer;        // Server name
    LPITEMIDLIST    m_pidl;             // What Xbox dir is hint in? (Not including the virtual root) (Does begin with ServerID)
    LPTSTR          m_pszUser;          // 0 or "" means "anonymous"
    LPTSTR          m_pszPassword;      // User's Password
    LPTSTR          m_pszFragment;      // URL fragment
    LPITEMIDLIST    m_pidlVirtualDir;   // Our rooted directory on the server.
    LPTSTR          m_pszRedirPassword; // What was the password if it was changed?
    LPTSTR          m_pszLookupStr;     // Str to lookup.
    INTERNET_PORT   m_ipPortNum;        // The port number
    BOOL            m_fDLTypeSpecified; // Did the user specify a Download Type to use? (ASCII vs. Binary)
    BOOL            m_fASCIIDownload;   // If specified, was it ASCII? (Else, Binary)
    CAccounts       m_cAccount;
    BOOL            m_fRatingsChecked;  // Did I check ratings yet?
    BOOL            m_fRatingsAllow;    // Does ratings allow access to this site?
    BOOL            m_fFeaturesQueried; // 
    BOOL            m_fInUTF8Mode;      // Did a success value come back from the 'UTF8' command?
    BOOL            m_fIsCHMODSupported;// Is the CHMOD UNIX command supported via the 'SITE CHMOD' XBOX Command?
    BOOL            m_fIsServerVMS;     // Is this a VMS server?

    CWireEncoding   m_cwe;              // What codepage and confidence in that codepage of the MOTD and filenames?

    // Protected Member Functions
    HRESULT _RedirectAndUpdate(LPCTSTR pszServer, INTERNET_PORT ipPortNum, LPCTSTR pszUser, LPCTSTR pszPassword, LPCITEMIDLIST pidlXboxPath, LPCTSTR pszFragment, IUnknown * punkSite, CXboxFolder * pff);
    HRESULT _Redirect(LPITEMIDLIST pidl, IUnknown * punkSite, CXboxFolder * pff);
    HRESULT _SetDirectory(HINTERNET hint, HWND hwnd, LPCITEMIDLIST pidlNewDir, CStatusBar * psb, int * pnTriesLeft);

private:
    // Private Member Functions
    HRESULT _SetPidl(LPCITEMIDLIST pidlXboxPath);
    HRESULT _QueryServerFeatures(HINTERNET hint);
    HRESULT _CheckToEnableCHMOD(LPCWIRESTR pwResponse);
    HRESULT _LoginToTheServer(HWND hwnd, HINTERNET hintDll, HINTERNET * phint, LPCITEMIDLIST pidlXboxPath, CStatusBar * psb, IUnknown * punkSite, CXboxFolder * pff);
    HRESULT _SetRedirPassword(LPCTSTR pszServer, INTERNET_PORT ipPortNum, LPCTSTR pszUser, LPCTSTR pszPassword, LPCITEMIDLIST pidlXboxPath, LPCTSTR pszFragment);

    void FlushHint(void);
    void FlushHintCritial(void);

    // Private Friend Functions
    friend HRESULT SiteCache_PrivSearch(LPCTSTR pszLookup, LPCITEMIDLIST pidl, IMalloc * pm, CXboxSite ** ppfs);
};



HRESULT CXboxSite_Init(void);
HRESULT CXboxSite_Create(LPCITEMIDLIST pidl, LPCTSTR pszLookupStr, IMalloc * pm, CXboxSite ** ppfs);


#endif // _XBOXSITE_H
