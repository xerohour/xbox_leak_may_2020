/*****************************************************************************\
    FILE: xboxdir.cpp

    DESCRIPTION:
        Internal object that manages a single XBOX directory

    The idea is that each XboxSite maintains a linked list of the
    XboxDir's that it owns.  Gets and Releases are done through the
    XboxSite.  Each XboxDir retains a non-refcounted pointer back
    to the XboxSite that owns it.

    The reason this is necessary is that there might be multiple
    IShellFolder's all looking at the same physical directory.  Since
    enumerations are expensive, we cache the enumeration information
    here, so that each IShellFolder client can use the information.

    This also lets us hold the motd, so that multiple clients can
    query for the motd without constantly pinging the site.
\*****************************************************************************/


#include "priv.h"
#include "xboxdir.h"
#include "xboxsite.h"
#include "xboxpidl.h"
#include "xboxurl.h"
#include "xboxpidl.h"
#include "statusbr.h"


/*****************************************************************************\
    FUNCTION: GetDisplayPath

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxDir::GetDisplayPath(LPWSTR pwzDisplayPath, DWORD cchSize)
{
    return GetDisplayPathFromPidl(m_pidlXboxDir, pwzDisplayPath, cchSize, FALSE);
}


/*****************************************************************************\
    FUNCTION: CollectMotd

    DESCRIPTION:
        An InternetConnect has just completed.  Get the motd and cache it.

    hint - the connected handle, possibly 0 if error
\*****************************************************************************/
void CXboxDir::CollectMotd(HINTERNET hint)
{
    CXboxGlob * pfg = GetXboxResponse(GetXboxSite()->GetCWireEncoding());

    if (m_pfgMotd)
        m_pfgMotd->Release();

    m_pfgMotd = pfg;  // m_pfgMotd will take pfg's ref.
}


/*****************************************************************************\
    FUNCTION: CollectMotd

    DESCRIPTION:
        Shove a value into the cached list.
\*****************************************************************************/
void CXboxDir::SetCache(CXboxPidlList * pflHfpl)
{
    IUnknown_Set(&m_pflHfpl, pflHfpl);

    // If we are flushing the cache, then flush the Ratings info also.
    // This way the user can reenter the parent password if wanted.
    if (!pflHfpl && m_pfs)
        m_pfs->FlushRatingsInfo();
}


/*****************************************************************************\
    FUNCTION: CollectMotd

    DESCRIPTION:
        Get the value out of the cache.
\*****************************************************************************/
CXboxPidlList * CXboxDir::GetHfpl(void)
{
    CXboxPidlList * pfl;
    
    pfl = m_pflHfpl;
    if (pfl)
        pfl->AddRef();

    return pfl;
}


/*****************************************************************************\
    FUNCTION: CollectMotd

    DESCRIPTION:
        Get the XBOX site associated with a directory.
    This doesn't AddRef the return value.
\*****************************************************************************/
CXboxSite * CXboxDir::GetXboxSite(void)
{
    return m_pfs;
}


CXboxDir * CXboxDir::GetSubXboxDir(CXboxFolder * pff, LPCITEMIDLIST pidl, BOOL fPublic)
{
    CXboxDir * pfd = NULL;

    if (EVAL(pidl))
    {
        LPITEMIDLIST pidlChild = GetSubPidl(pff, pidl, fPublic);
        
        if (EVAL(pidlChild))
        {
            m_pfs->GetXboxDir(pidlChild, &pfd);
            ILFree(pidlChild);
        }
    }

    return pfd;
}


LPITEMIDLIST CXboxDir::GetSubPidl(CXboxFolder * pff, LPCITEMIDLIST pidlRelative, BOOL fPublic)
{
    LPITEMIDLIST pidlRoot = ((fPublic && pff) ? pff->GetPublicPidlRootIDClone() : NULL);
    LPITEMIDLIST pidlPublic = ILCombine(pidlRoot, m_pidl);
    LPITEMIDLIST pidlFull = NULL;

    if (pidlPublic)
    {
        pidlFull = ILCombine(pidlPublic, pidlRelative);
        ILFree(pidlPublic);
    }

    ILFree(pidlRoot);
    return pidlFull;
}


HRESULT CXboxDir::AddItem(LPCITEMIDLIST pidl)
{
    if (!m_pflHfpl)
        return S_OK;

#ifdef DEBUG
#if 0
    WCHAR wzDisplayPath[MAX_PATH];

    EVAL(SUCCEEDED(GetDisplayPathFromPidl(m_pidlXboxDir, wzDisplayPath, ARRAYSIZE(wzDisplayPath), FALSE)));

    TraceMsg(TF_ALWAYS, "CXboxDir::AddItem() Dir=\"%ls\", Item=\"%ls\".", wzDisplayPath, XboxPidl_GetFileDisplayName(pidl));
#endif // 0
#endif // DEBUG

    return m_pflHfpl->InsertSorted(pidl);
}


/*****************************************************************************\
    FUNCTION: CollectMotd

    DESCRIPTION:
        Get a HINTERNET for this directory.
\*****************************************************************************/
HRESULT CXboxDir::GetHint(HWND hwnd, CStatusBar * psb, HINTERNET * phint, IUnknown * punkSite, CXboxFolder * pff)
{
    HRESULT hr = m_pfs->GetHint(hwnd, m_pidlXboxDir, psb, phint, punkSite, pff);

    return hr;
}


/*****************************************************************************\
    FUNCTION: CollectMotd

    DESCRIPTION:
        Give a HINTERNET back to the XboxSite.
\*****************************************************************************/
void CXboxDir::ReleaseHint(HINTERNET hint)
{
    ASSERT(!hint || m_pfs); // If we have a hint to release, we need to call ::ReleaseHint()
    if (m_pfs)
        m_pfs->ReleaseHint(m_pidlXboxDir, hint);
}


/*****************************************************************************\
    FUNCTION: CollectMotd

    DESCRIPTION:
        Perform an operation with a temporary internet handle which is
    already connected to the site and resides in the correct directory.
\*****************************************************************************/
STDMETHODIMP CXboxDir::WithHint(CStatusBar * psb, HWND hwnd, HINTPROC hp, LPCVOID pv, IUnknown * punkSite, CXboxFolder * pff)
{
    HRESULT hr = E_FAIL;

    // Did the user turn off XBOX Folders?
    // If so, don't connect.  This will fix NT #406423 where the user turned
    // of XBOX Folders because they have a firewall (CISCO filtering Router)
    // that will kill packets in such a way the caller (WinSock/Wininet) needs
    // to wait for a timeout.  During this timeout, the browser will hang causing
    // the user to think it crashed.
    if (!SHRegGetBoolUSValue(SZ_REGKEY_XBOXFOLDER, SZ_REGKEY_USE_OLD_UI, FALSE, FALSE))
    {
        HINTERNET hint;
        HINTPROCINFO hpi;

        ASSERTNONCRITICAL;        // Cannot do psb (CStatusBar *) with the crst
        ASSERT(m_pfs);
        hpi.pfd = this;
        hpi.hwnd = hwnd;
        hpi.psb = psb;

        hr = GetHint(hwnd, psb, &hint, punkSite, pff);
        if (SUCCEEDED(hr)) // Ok if fails
        {
            BOOL fReleaseHint = TRUE;

            if (hp)
                hr = hp(hint, &hpi, (LPVOID)pv, &fReleaseHint);

            if (fReleaseHint)
                ReleaseHint(hint);
        }
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: _SetNameOfCB

    DESCRIPTION:
        If we were able to rename the file, return the output pidl.
    Also tell anybody who cares that this LPITEMIDLIST needs to be refreshed.

    The "A" emphasizes that the filename is received in ANSI.

    _UNDOCUMENTED_: The documentation on SetNameOf's treatment of
    the source pidl is random.  It seems to suggest that the source
    pidl is ILFree'd by SetNameOf, but it isn't.
\*****************************************************************************/
HRESULT CXboxDir::_SetNameOfCB(HINTERNET hint, HINTPROCINFO * phpi, LPVOID pv, BOOL * pfReleaseHint)
{
    LPSETNAMEOFINFO psnoi = (LPSETNAMEOFINFO) pv;

    if (phpi->psb)
        phpi->psb->SetStatusMessage(IDS_RENAMING, XboxPidl_GetLastItemDisplayName(psnoi->pidlOld));

    // Remember, XBOX filenames are always in the ANSI character set
    return XboxRenameFilePidlWrap(hint, TRUE, psnoi->pidlOld, psnoi->pidlNew);
}


BOOL CXboxDir::_DoesItemExist(HWND hwnd, CXboxFolder * pff, LPCITEMIDLIST pidl)
{
    XBOX_FIND_DATA wfd;
    HRESULT hr = GetFindData(hwnd, XboxPidl_GetLastItemWireName(pidl), &wfd, pff);

    return ((S_OK == hr) ? TRUE : FALSE);
}


BOOL CXboxDir::_ConfirmReplaceWithRename(HWND hwnd)
{
    TCHAR szTitle[MAX_PATH];
    TCHAR szMessage[MAX_PATH];

    EVAL(LoadString(HINST_THISDLL, IDS_XBOXERR_TITLE, szTitle, ARRAYSIZE(szTitle)));
    EVAL(LoadString(HINST_THISDLL, IDS_XBOXERR_RENAME_REPLACE, szMessage, ARRAYSIZE(szMessage)));

    return ((IDYES == MessageBox(hwnd, szMessage, szTitle, (MB_ICONQUESTION | MB_YESNO))) ? TRUE : FALSE);
}


HRESULT CXboxDir::SetNameOf(CXboxFolder * pff, HWND hwndOwner, LPCITEMIDLIST pidl,
           LPCWSTR pwzName, DWORD dwReserved, LPITEMIDLIST *ppidlOut)
{
    HRESULT hr = S_OK;
    SETNAMEOFINFO snoi;
    CWireEncoding cWireEncoding;

    ASSERT(pff);

    if (!pwzName)
        return E_FAIL;

    snoi.pidlOld = pidl;
    cWireEncoding.ChangeXboxItemIDName(NULL, pidl, pwzName, IsUTF8Supported(), (LPITEMIDLIST *) &snoi.pidlNew);

    if (snoi.pidlNew)
    {
#ifdef FEATURE_REPLACE_IN_RENAME
        // Disable this feature because we don't ever do the delete and there is no
        // way for us to have wininet do the delete for us.

        // Does it already exist?  We don't care if we don't have an hwnd because
        // we can't ask the user to replace so we will just go ahead.
        if (hwndOwner && _DoesItemExist(hwndOwner, pff, snoi.pidlNew))
        {
            // Yes, so let's make sure it's OK with the user to replace it.
            hr = (_ConfirmReplaceWithRename(hwndOwner) ? S_OK : HRESULT_FROM_WIN32(ERROR_CANCELLED));
            bugbug; // Delete the dest file so we will succeed with the rename.
        }
#endif FEATURE_REPLACE_IN_RENAME

        if (S_OK == hr)
        {
            hr = WithHint(NULL, hwndOwner, _SetNameOfCB, (LPVOID) &snoi, NULL, pff);
            if (SUCCEEDED(hr))  // Will fail if use didn't have permission to rename
            {
                // WARNING: The date/time stamp on the server may be different than what we give to SHChangeNotify()
                //          but this is probably reasonable for perf reasons.
                XboxChangeNotify(hwndOwner, XboxPidl_DirChoose(pidl, SHCNE_RENAMEFOLDER, SHCNE_RENAMEITEM), pff, this, pidl, snoi.pidlNew, TRUE);

                if (ppidlOut)
                    *ppidlOut = ILClone(snoi.pidlNew);
            }
        }

        ILFree((LPITEMIDLIST) snoi.pidlNew);
    }

    return hr;
}


LPCITEMIDLIST CXboxDir::GetPidlFromWireName(LPCWIRESTR pwWireName)
{
    LPITEMIDLIST pidlToFind = NULL;
    LPITEMIDLIST pidlTemp;
    WCHAR wzDisplayName[MAX_PATH];
    
    // This isn't valid because the code page could be wrong, but we don't care
    // because it's not used in the search for the pidl, the pwWireName is.
    SHAnsiToUnicode(pwWireName, wzDisplayName, ARRAYSIZE(wzDisplayName));
    if (m_pflHfpl && EVAL(SUCCEEDED(XboxItemID_CreateFake(wzDisplayName, pwWireName, FALSE, FALSE, FALSE, &pidlTemp))))
    {
        // PERF: log 2 (sizeof(m_pflHfpl))
        pidlToFind = m_pflHfpl->FindPidl(pidlTemp, FALSE);
        // We will try again and this time allow for the case to not match
        if (!pidlToFind)
            pidlToFind = m_pflHfpl->FindPidl(pidlTemp, TRUE);
        ILFree(pidlTemp);
    }

    return pidlToFind;
}


LPCITEMIDLIST CXboxDir::GetPidlFromDisplayName(LPCWSTR pwzDisplayName)
{
    WIRECHAR wWireName[MAX_PATH];
    CWireEncoding * pwe = GetXboxSite()->GetCWireEncoding();

    EVAL(SUCCEEDED(pwe->UnicodeToWireBytes(NULL, pwzDisplayName, (IsUTF8Supported() ? WIREENC_USE_UTF8 : WIREENC_NONE), wWireName, ARRAYSIZE(wWireName))));
    return GetPidlFromWireName(wWireName);
}


/*****************************************************************************\
    FUNCTION: IsRoot

    DESCRIPTION:
        Returns FALSE if we are at the "XBOX Folder" root level, not
    inside an actual XBOX site.y
\*****************************************************************************/
BOOL CXboxDir::IsRoot(void)
{
    return ILIsEmpty(m_pidl);
}


typedef struct tagGETFINDDATAINFO
{
    LPCWIRESTR pwWireName;
    LPXBOX_FIND_DATA pwfd;
} GETFINDDATAINFO, * LPGETFINDDATAINFO;

HRESULT CXboxDir::_GetFindData(HINTERNET hint, HINTPROCINFO * phpi, LPVOID pv, BOOL * pfReleaseHint)
{
    LPGETFINDDATAINFO pgfdi = (LPGETFINDDATAINFO) pv;
    HRESULT hr = S_FALSE;

    // Remember, XBOX filenames are always in the ANSI character set
    // PERF: Status
    hr = XboxDoesFileExist(hint, TRUE, pgfdi->pwWireName, pgfdi->pwfd, INTERNET_NO_CALLBACK);
    if (SUCCEEDED(hr))
    {
        if (!StrCmpIA(pgfdi->pwfd->cFileName, pgfdi->pwWireName))
            hr = S_OK;        // The are equal.
        else if (!StrCmpA(pgfdi->pwfd->cFileName, SZ_DOTA))
        {
            //    Coincidence of coincidences:  If we found a ".",
            //  then the wfd already contains the description of
            //  the directory!  In other words, the wfd contains
            //  the correct information after all, save for the name.
            //  Aren't we lucky.
            //
            //  And if it isn't dot, then it's some directory with
            //  unknown attributes (so we'll use whatever's lying around).
            //  Just make sure it's a directory.
            pgfdi->pwfd->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            StrCpyNA(pgfdi->pwfd->cFileName, pgfdi->pwWireName, ARRAYSIZE(pgfdi->pwfd->cFileName));
            hr = S_OK;
        }
    }
    else
    {
#ifndef DEBUG
        // Don't display an error msg because some callers will call when they
        // know the file may not exist.  This is the case for ConfirmCopy().
        hr = S_FALSE;
#endif // DEBUG
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: GetFindData

    DESCRIPTION:
        Get the WIN32_FIND_DATA for a file, given by name.

    This is done as part of drag/drop to allow for an overwrite prompt.

    BUGBUG -- This is all a gross hack because the STAT command
    isn't supported by WinINet (as XboxGetFileAttributes).

    Not that it'd help, because Xbox.microsoft.com is OUT OF SPEC
    with regard to the STAT command.  (The first line of the output
    isn't terminated correctly, causing the client to hang.)

    Furthermore, UNIX Xbox servers implement STAT incorrectly, too,
    rendering STAT no more useful than LIST.

    HACKHACK -- There is a bug in WinINet where doing a FindFirst
    on a name which happens to be a directory returns the contents
    of the directory instead of the attributes of the directory itself.
    (This is actually a failing of most XBOX implementation, because
    they just use /bin/ls for directory listings.)

    So we compare the name that comes back against the name we ask
    for.  If they are different, then it's a folder.  We'll compare
    in a case-insensitive manner because we don't know whether the
    server is case-sensitive or not.

    Note that we can get faked out if a directory contains a file
    which has the same name as the directory.  There is nothing we
    can do about that.  Fortunately, UNIX servers always return "."
    as the first file in a subdirectory, so 99% of the time, we'll
    do the right thing.
\*****************************************************************************/
HRESULT CXboxDir::GetFindData(HWND hwnd, LPCWIRESTR pwWireName, LPXBOX_FIND_DATA pwfd, CXboxFolder * pff)
{
    GETFINDDATAINFO gfdi = {pwWireName, pwfd};
    HRESULT hr = WithHint(NULL, hwnd, _GetFindData, &gfdi, NULL, pff);

    return hr;
}


/*****************************************************************************\
    FUNCTION: GetFindDataForDisplayPath

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxDir::GetFindDataForDisplayPath(HWND hwnd, LPCWSTR pwzDisplayPath, LPXBOX_FIND_DATA pwfd, CXboxFolder * pff)
{
    CWireEncoding * pwe = GetXboxSite()->GetCWireEncoding();
    WIRECHAR wWirePath[MAX_PATH];

    EVAL(SUCCEEDED(pwe->UnicodeToWireBytes(NULL, pwzDisplayPath, (IsUTF8Supported() ? WIREENC_USE_UTF8 : WIREENC_NONE), wWirePath, ARRAYSIZE(wWirePath))));
    return GetFindData(hwnd, wWirePath, pwfd, pff);
}


/*****************************************************************************\
    FUNCTION: GetNameOf

    DESCRIPTION:
        Common worker that handles SHGDN_FORPARSING style GetDisplayNameOf's.

    Note! that since we do not support junctions (duh), we can
    safely walk down the pidl generating goop as we go, secure
    in the knowledge that we are in charge of every subpidl.

    _CHARSET_:  Since XBOX filenames are always in the ANSI character
    set, by RFC 1738, we can return ANSI display names without loss
    of fidelity.  In a general folder implementation, we should be
    using cStr to return display names, so that the UNICODE
    version of the shell extension can handle UNICODE names.
\*****************************************************************************/
HRESULT CXboxDir::GetNameOf(LPCITEMIDLIST pidl, DWORD shgno, LPSTRRET pstr)
{
    LPITEMIDLIST pidlFull = ILCombine(m_pidl, pidl);
    HRESULT hr = E_FAIL;

    if (pidlFull)
    {
        hr = StrRetFromXboxPidl(pstr, shgno, pidlFull);
        ILFree(pidlFull);
    }

    return hr;
}

/*****************************************************************************\
      FUNCTION: ChangeFolderName

      DESCRIPTION:
        A rename happened on this folder so update the szDir and m_pidl

      PARAMETERS:
        pidlXboxPath
\*****************************************************************************/
HRESULT CXboxDir::ChangeFolderName(LPCITEMIDLIST pidlXboxPath)
{
    HRESULT hr = E_FAIL;
    LPITEMIDLIST pidlNewXboxPath = NULL;

    EVAL(SUCCEEDED(m_pfs->FlushSubDirs(m_pidlXboxDir)));
    hr = XboxPidl_ReplacePath(m_pidl, pidlXboxPath, &pidlNewXboxPath);
    _SetXboxDir(m_pfs, this, pidlXboxPath);
    if (EVAL(SUCCEEDED(hr)))
    {
        Pidl_Set(&m_pidl, pidlNewXboxPath);
        ILFree(pidlNewXboxPath);
    }

    return hr;
}


/*****************************************************************************\
      FUNCTION: _CompareDirs

      DESCRIPTION:
        Check if the indicated pfd is already rooted at the indicated pidl.
\*****************************************************************************/
int CALLBACK _CompareDirs(LPVOID pvPidl, LPVOID pvXboxDir, LPARAM lParam)
{
    LPCITEMIDLIST pidl = (LPCITEMIDLIST) pvPidl;
    CXboxDir * pfd = (CXboxDir *) pvXboxDir;

    return XboxItemID_CompareIDsInt(COL_NAME, pfd->m_pidl, pidl, FCMP_NORMAL);
}


HRESULT CXboxDir::_SetXboxDir(CXboxSite * pfs, CXboxDir * pfd, LPCITEMIDLIST pidl)
{
    if (XboxID_IsServerItemID(pidl))
        pidl = _ILNext(pidl);

    // We don't want pfd->m_pidlXboxDir to include the virtual root.
    if (pfd->GetXboxSite()->HasVirtualRoot())
    {
        LPITEMIDLIST pidlIterate = (LPITEMIDLIST) pidl;
        LPITEMIDLIST pidlVRootIterate = (LPITEMIDLIST) pfd->GetXboxSite()->GetVirtualRootReference();

        ASSERT(!XboxID_IsServerItemID(pidl) && !XboxID_IsServerItemID(pidlVRootIterate));
        // Let's see if pidl starts off with 
        while (!ILIsEmpty(pidlVRootIterate) && !ILIsEmpty(pidlIterate) && 
                XboxItemID_IsEqual(pidlVRootIterate, pidlIterate))
        {
            pidlVRootIterate = _ILNext(pidlVRootIterate);
            pidlIterate = _ILNext(pidlIterate);
        }

        if (ILIsEmpty(pidlVRootIterate))
            pidl = (LPCITEMIDLIST)pidlIterate;

    }

    Pidl_Set(&pfd->m_pidlXboxDir, pidl);
    return S_OK;
}


/*****************************************************************************\
      FUNCTION: CXboxDir_Create

      DESCRIPTION:
        Create a brand new XboxDir structure.
\*****************************************************************************/
HRESULT CXboxDir_Create(CXboxSite * pfs, LPCITEMIDLIST pidl, CXboxDir ** ppfd)
{
    CXboxDir * pfd = new CXboxDir();
    HRESULT hr = E_OUTOFMEMORY;

    ASSERT(pfs);
    if (EVAL(pfd))
    {
        // WARNING: No ref held because it's a back pointer.
        //          This requires that the parent (CXboxSite) always
        //          out live this object.
        pfd->m_pfs = pfs;

        Pidl_Set(&pfd->m_pidl, pidl);
        if (EVAL(pfd->m_pidl))
            hr = pfd->_SetXboxDir(pfs, pfd, pidl);
        else
            IUnknown_Set(&pfd, NULL);
    }

    *ppfd = pfd;
    ASSERT(*ppfd ? SUCCEEDED(hr) : FAILED(hr));

    return hr;
}


/****************************************************\
    Constructor
\****************************************************/
CXboxDir::CXboxDir() : m_cRef(1)
{
    DllAddRef();

    // This needs to be allocated in Zero Inited Memory.
    // Assert that all Member Variables are inited to Zero.
    ASSERT(!m_pfs);
    ASSERT(!m_pflHfpl);
    ASSERT(!m_pfgMotd);
    ASSERT(!m_pidl);

    LEAK_ADDREF(LEAK_CXboxDir);
}


/****************************************************\
    Destructor
\****************************************************/
CXboxDir::~CXboxDir()
{
    // WARNING: m_pfs is a back pointer that doesn't have a ref.
    // m_pfs)

    IUnknown_Set(&m_pflHfpl, NULL);
    IUnknown_Set(&m_pfgMotd, NULL);
    
    if (m_pidl)         // Win95's Shell32.dll crashes with ILFree(NULL)
        ILFree(m_pidl);

    DllRelease();
    LEAK_DELREF(LEAK_CXboxDir);
}


//===========================
// *** IUnknown Interface ***
//===========================

ULONG CXboxDir::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG CXboxDir::Release()
{
    ASSERT(m_cRef > 0);
    m_cRef--;

    if (m_cRef > 0)
        return m_cRef;

    delete this;
    return 0;
}

HRESULT CXboxDir::QueryInterface(REFIID riid, void **ppvObj)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObj = SAFECAST(this, IUnknown*);
    }
    else
    {
        TraceMsg(TF_XBOXQI, "CXboxDir::QueryInterface() failed.");
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}
