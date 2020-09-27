/*****************************************************************************\
 *    xboxsite.cpp - Internal object that manages a single XBOX site
\*****************************************************************************/

#include "priv.h"
#include "xboxsite.h"
#include "xboxinet.h"
#include "xboxurl.h"
#include "statusbr.h"
#include "offline.h"
#include <ratings.h>
#include <wininet.h>
#include <dbgmem.h>

#ifdef DEBUG
DWORD g_dwOpenConnections = 0;      // Ref Counting Open Connections
#endif // DEBUG

/*****************************************************************************\
 *    CXboxSite
 *
 *    EEK!  RFC 1738 is really scary.  XBOX sites don't necessarily
 *    start you at the root, and RFC1738 says that xbox://foo/bar asks
 *    for the file bar in the DEFAULT directory, not the root!
\*****************************************************************************/
CXboxList * g_XboxSiteCache = NULL;                /* The list of all open XBOX sites */


void CXboxSite::FlushHint(void)
{
    HINTERNET hint = m_hint;

    m_hint = NULL;
    if (hint)
    {
        // Our caller needs to be holding the critical section
        // while we modify m_hint
        ASSERTCRITICAL;

        InternetCloseHandle(hint);
//        DEBUG_CODE(g_dwOpenConnections--;);
    }
}


void CXboxSite::FlushHintCritial(void)
{
    ASSERTNONCRITICAL;

    ENTERCRITICAL;
    FlushHint();
    LEAVECRITICAL;
}


void CXboxSite::FlushHintCB(LPVOID pvXboxSite)
{
    CXboxSite * pfs = (CXboxSite *) pvXboxSite;

    if (pfs)
    {
        pfs->FlushHint();
        pfs->Release();
    }
}


/*****************************************************************************\
 *    An InternetConnect has just completed.  Get the motd and cache it.
 *
 *    hint - the connected handle, possibly 0 if error
\*****************************************************************************/
void CXboxSite::CollectMotd(HINTERNET hint)
{
    CXboxGlob * pfg = GetXboxResponse(&m_cwe);
    remove_from_memlist(pfg);   // We will probably free this on a separate thread.

    ENTERCRITICAL;
    m_fMotd = m_pfgMotd ? TRUE : FALSE;            // We have a motd

    IUnknown_Set(&m_pfgMotd, NULL);
    m_pfgMotd = pfg;

    LEAVECRITICAL;
}


/*****************************************************************************\
    FUNCTION: ReleaseHint

    DESCRIPTION:
        An XboxDir client is finished with a handle to the XBOX site.
    Put it into the cache, and throw away what used to be there.

    We always keep the most recent handle, because that reduces the
    likelihood that the server will close the connection due to extended
    inactivity.

    The critical section around this entire procedure is important,
    else we open up all sorts of really ugly race conditions.  E.g.,
    the timeout might trigger before we're finished initializing it.
    Or somebody might ask for the handle before we're ready.
\*****************************************************************************/
void CXboxSite::ReleaseHint(LPCITEMIDLIST pidlXboxPath, HINTERNET hint)
{
    ENTERCRITICAL;

    TriggerDelayedAction(&m_hgti);    // Kick out the old one

    _SetPidl(pidlXboxPath);
    m_hint = hint;

    if (EVAL(SUCCEEDED(SetDelayedAction(FlushHintCB, (LPVOID) this, &m_hgti))))
        AddRef();   // We just gave away a ref.
    else
        FlushHint();    // Oh well, can't cache it

    LEAVECRITICAL;
}


// NT #362108: We need to set the redirect password for the CXboxSite that
// contains the server, the user name, but a blank password to be redirected
// to the CXboxSite that does have the correct password.  This way, if a user
// logs in and doesn't save the password in the URL or the secure cache, we
// then put it in the in memory password cache so it stays valid for that
// "browser" session (defined by process lifetime).  We then need to redirect
// future navigations that go to that 
HRESULT CXboxSite::_SetRedirPassword(LPCTSTR pszServer, INTERNET_PORT ipPortNum, LPCTSTR pszUser, LPCTSTR pszPassword, LPCITEMIDLIST pidlXboxPath, LPCTSTR pszFragment)
{
    TCHAR szUrl[MAX_URL_STRING];
    HRESULT hr;

    hr = UrlCreate(pszServer, pszUser, TEXT(""), TEXT(""), pszFragment, ipPortNum, NULL, szUrl, ARRAYSIZE(szUrl));
    if (EVAL(SUCCEEDED(hr)))
    {
        LPITEMIDLIST pidlServer;

        hr = CreateXboxPidlFromUrl(szUrl, GetCWireEncoding(), NULL, &pidlServer, m_pm, TRUE);
        if (EVAL(SUCCEEDED(hr)))
        {
            LPITEMIDLIST pidl = ILCombine(pidlServer, pidlXboxPath);

            if (pidl)
            {
                CXboxSite * pfsDest = NULL;

                // The user name has changed so we need to update the
                // CXboxSite with the new user name also.
                hr = SiteCache_PidlLookup(pidl, FALSE, m_pm, &pfsDest);
                if (EVAL(SUCCEEDED(hr)))
                {
                    pfsDest->SetRedirPassword(pszPassword);
                    pfsDest->Release();
                }

                ILFree(pidl);
            }

            ILFree(pidlServer);
        }
    }

    return hr;
}

HRESULT CXboxSite::_RedirectAndUpdate(LPCTSTR pszServer, INTERNET_PORT ipPortNum, LPCTSTR pszUser, LPCTSTR pszPassword, LPCITEMIDLIST pidlXboxPath, LPCTSTR pszFragment, IUnknown * punkSite, CXboxFolder * pff)
{
    TCHAR szUrl[MAX_URL_STRING];
    TCHAR szUser[INTERNET_MAX_USER_NAME_LENGTH];
    HRESULT hr;

    StrCpyN(szUser, pszUser, ARRAYSIZE(szUser));    // Copy because of possible reentrancy
    EscapeString(NULL, szUser, ARRAYSIZE(szUser));
    hr = UrlCreate(pszServer, szUser, pszPassword, TEXT(""), pszFragment, ipPortNum, NULL, szUrl, ARRAYSIZE(szUrl));
    if (EVAL(SUCCEEDED(hr) && pff))
    {
        LPITEMIDLIST pidlServer;

        hr = CreateXboxPidlFromUrl(szUrl, GetCWireEncoding(), NULL, &pidlServer, m_pm, TRUE);
        if (EVAL(SUCCEEDED(hr)))
        {
            LPITEMIDLIST pidl = ILCombine(pidlServer, pidlXboxPath);

            if (pidl)
            {
                // If the user changed the password, we need to setup a redirect so
                // they can return later. (NT #362108)
                if (m_pszUser && !StrCmp(m_pszUser, szUser) && StrCmp(m_pszPassword, pszPassword))
                {
                    _SetRedirPassword(pszServer, ipPortNum, szUser, pszPassword, pidlXboxPath, pszFragment);
                }

                // If the user name changed, set a redirect.
                if (!m_pszUser || StrCmp(m_pszUser, szUser))
                {
                    CXboxSite * pfsDest = NULL;

                    // The user name has changed so we need to update the
                    // CXboxSite with the new user name also.
                    hr = SiteCache_PidlLookup(pidl, FALSE, m_pm, &pfsDest);
                    if (EVAL(SUCCEEDED(hr)))
                    {
                        pfsDest->SetRedirPassword(pszPassword);
                        pfsDest->Release();
                    }
                }

                hr = _Redirect(pidl, punkSite, pff);
                ILFree(pidl);
            }

            ILFree(pidlServer);
        }
    }

    return hr;
}


HRESULT CXboxSite::_Redirect(LPITEMIDLIST pidl, IUnknown * punkSite, CXboxFolder * pff)
{
    LPITEMIDLIST pidlFull = pff->CreateFullPublicPidl(pidl);
    HRESULT hr = E_INVALIDARG;

    if (EVAL(pidlFull))
    {
        hr = IUnknown_PidlNavigate(punkSite, pidlFull, FALSE);

        ASSERT(SUCCEEDED(hr));
        ILFree(pidlFull);
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: _SetDirectory

    DESCRIPTION:
        When the caller wants a handle to the server, they often want a different
    directory than what's in the cache.  This function needs to change into
    the new directory.
\*****************************************************************************/
HRESULT CXboxSite::_SetDirectory(HINTERNET hint, HWND hwnd, LPCITEMIDLIST pidlNewDir, CStatusBar * psb, int * pnTriesLeft)
{
    HRESULT hr = S_OK;

    if (pidlNewDir && XboxID_IsServerItemID(pidlNewDir))
        pidlNewDir = _ILNext(pidlNewDir);   // Skip the server.

    ASSERT(m_pidl);
    // NT #300889: I would like to cache the dir but sometimes it gets
    //             out of wack and m_pidl doesn't match the HINTERNET's
    //             cwd.  PERF: This could be fixed in the future but
    //             this perf tweak isn't work the work now (small gain).
//  if (m_pidl && !XboxPidl_IsPathEqual(_ILNext(m_pidl), pidlNewDir))
    {
        LPITEMIDLIST pidlWithVirtualRoot;

        if (psb)
        {
            WCHAR wzDisplayPath[MAX_PATH];  // For Statusbar.
            
            if (pidlNewDir && SUCCEEDED(GetDisplayPathFromPidl(pidlNewDir, wzDisplayPath, ARRAYSIZE(wzDisplayPath), TRUE)))
                psb->SetStatusMessage(IDS_CHDIR, wzDisplayPath);
            else
                psb->SetStatusMessage(IDS_CHDIR, L"\\");
        }

        hr = PidlInsertVirtualRoot(pidlNewDir, &pidlWithVirtualRoot);
        if (EVAL(SUCCEEDED(hr)))
        {
            hr = XboxSetCurrentDirectoryPidlWrap(hint, TRUE, pidlWithVirtualRoot, TRUE, TRUE);
            if (SUCCEEDED(hr))  // Ok if failed. (No Access?)
            {
                hr = _SetPidl(pidlNewDir);
            }
            else
            {

                ReleaseHint(NULL, hint); // Nowhere
                if (hr == HRESULT_FROM_WIN32(ERROR_FTP_DROPPED))
                    FlushHintCritial(); // Don't cache dead hint
                else
                {
                    DisplayWininetError(hwnd, TRUE, HRESULT_CODE(hr), IDS_XBOXERR_TITLE_ERROR, IDS_XBOXERR_CHANGEDIR, IDS_XBOXERR_WININET, MB_OK, NULL);
                    *pnTriesLeft = 0;   // Make sure we don't keep display UI.
                    hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                }

                hint = 0;
            }

            ILFree(pidlWithVirtualRoot);
        }

        if (psb)
            psb->SetStatusMessage(IDS_EMPTY, 0);
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: _LoginToTheServer

    DESCRIPTION:
        We want an HINTERNET to do some XBOX operation but we don't have one
    cached.  So, login to create it.

    WARNING: This function will be called in a critical section and needs to 
             return in one.  However, it may leave the critical section for a
             while.
\*****************************************************************************/
HRESULT CXboxSite::_LoginToTheServer(HWND hwnd, HINTERNET hintDll, HINTERNET * phint, LPCITEMIDLIST pidlXboxPath, CStatusBar * psb, IUnknown * punkSite, CXboxFolder * pff)
{
    HRESULT hr = S_OK;

    ASSERTCRITICAL;
    BOOL fKeepTryingToLogin = FALSE;
    BOOL fTryOldPassword = TRUE;

    LEAVECRITICALNOASSERT;
    TCHAR szUser[INTERNET_MAX_USER_NAME_LENGTH];
    TCHAR szPassword[INTERNET_MAX_PASSWORD_LENGTH];

    StrCpyN(szUser, m_pszUser, ARRAYSIZE(szUser));
    StrCpyN(szPassword, m_pszPassword, ARRAYSIZE(szPassword));

    ASSERT(m_pszServer);
    if (psb)
        psb->SetStatusMessage(IDS_CONNECTING, m_pszServer);

    do
    {
        hr = InternetConnectWrap(hintDll, TRUE, HANDLE_NULLSTR(m_pszServer), m_ipPortNum, NULL_FOR_EMPTYSTR(szUser), NULL_FOR_EMPTYSTR(szPassword), INTERNET_SERVICE_FTP, 0, 0, phint);
        if (*phint)
            fKeepTryingToLogin = FALSE; // Move up.
        else
        {
            BOOL fSkipLoginDialog = FALSE;

            // Display Login dialog to get new user name/password to try again or cancel login.
            // fKeepTryingToLogin = TRUE if Dialog said [LOGIN].
            if (((ERROR_INTERNET_LOGIN_FAILURE == HRESULT_CODE(hr)) ||
                (ERROR_INTERNET_INCORRECT_USER_NAME == HRESULT_CODE(hr)) ||
                (ERROR_INTERNET_INCORRECT_PASSWORD == HRESULT_CODE(hr))) && hwnd)
            {
                BOOL fIsAnonymous = (!szUser[0] || !StrCmpI(szUser, TEXT("anonymous")) ? TRUE : FALSE);
                DWORD dwLoginFlags = (fIsAnonymous ? LOGINFLAGS_ANON_LOGINJUSTFAILED : LOGINFLAGS_USER_LOGINJUSTFAILED);

                if (fTryOldPassword)
                {
                    hr = m_cAccount.GetUserName(HANDLE_NULLSTR(m_pszServer), szUser, ARRAYSIZE(szUser));
                    if (S_OK == hr)
                    {
                        hr = m_cAccount.GetPassword(HANDLE_NULLSTR(m_pszServer), szUser, szPassword, ARRAYSIZE(szPassword));
                        if (S_OK == hr)
                        {
                            fKeepTryingToLogin = TRUE;
                            fSkipLoginDialog = TRUE;
                        }
                    }
                }
            
                if (!fSkipLoginDialog)
                {
                    // If the user tried to log in anonymously and failed, we want to try
                    // logging in with a password.  If the user tried logging in with a password
                    // and failed, we want to keep trying to log in with a password.
                    // 
                    // DisplayLoginDialog returns S_OK for OK pressed, S_FALSE for Cancel button, and
                    //       FAILED() for something is really messed up.
                    hr = m_cAccount.DisplayLoginDialog(hwnd, dwLoginFlags, HANDLE_NULLSTR(m_pszServer),
                                szUser, ARRAYSIZE(szUser), szPassword, ARRAYSIZE(szPassword));
                }

                // S_FALSE means the user cancelled out of the Login dialog.
                // We need to turn this into an error value so the caller,
                // CXboxDir::WithHint() won't call the callback.
                if (S_FALSE == hr)
                    hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);

                fKeepTryingToLogin = (SUCCEEDED(hr) ? TRUE : FALSE);
                if (fKeepTryingToLogin)
                {
                    // We need to set the cancelled error so we don't display the
                    // error message after this.
                    hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                }

                fTryOldPassword = FALSE;
            }
            else
                fKeepTryingToLogin = FALSE;
        }
    }
    while (fKeepTryingToLogin);

    if (!*phint)
    {
        ASSERT(2 != HRESULT_CODE(hr));        // error 2 = wininet not configured


#ifdef DEBUG
        // Gee, I wonder why I couldn't connect, let's find out.
        TCHAR szBuff[1500];
        InternetGetLastResponseInfoDisplayWrap(FALSE, NULL, szBuff, ARRAYSIZE(szBuff));
        // This may happen if the server has too many connections.  We may want to sniff
        // for this and offer to keep trying.  These are the response from the various
        // XBOX Servers in this case:
        // IIS v5: 421 Too many people are connected.  Please come back when the server is less busy.
        // UNIX: ???
#endif // DEBUG
    }
    // Was a different login name or password needed in order to login successfully?
    else
    {
        LPITEMIDLIST pidlVirtualDir;

        CollectMotd(*phint);
        _QueryServerFeatures(*phint);
        // Ref Count the open connections.
//                  DEBUG_CODE(g_dwOpenConnections++;);

        // Is it a VMS Server?
        if (m_fIsServerVMS)
        {
            // Yes, so skip getting pidlVirtualDir because wininet gives us
            // garbage for XboxGetCurrentDirectoryA().
        }
        else
        {
            // NOTE: If the connection isn't annonymous, the server may put the user
            //   into a sub directory called a virtual root.  We need to squirel that
            //   directory away because it may be needed when going into sub directories
            //   relative to this virtual root.
            //     Example: xbox://user1:password@server/ puts you into /users/user1/
            //     Then: xbox://user1:password@server/dir1 really should be /users/user1/dir1/
            hr = XboxGetCurrentDirectoryPidlWrap(*phint, TRUE, GetCWireEncoding(), &pidlVirtualDir);
            if (SUCCEEDED(hr))
            {
                // Are we rooted at '/'? (Meaning no virtual root)
                Pidl_Set(&m_pidlVirtualDir, pidlVirtualDir);
                ILFree(pidlVirtualDir);
            }
        }

        //DEBUG_CODE(TraceMsg(TF_WININET_DEBUG, "CXboxSite::GetHint() XboxGetCurrentDirectory() returned %#08lx", hr));
        if (StrCmp(HANDLE_NULLSTR(m_pszUser), szUser) || StrCmp(HANDLE_NULLSTR(m_pszPassword), szPassword))
        {
            // Yes, so redirect so the AddressBand and User Status Bar pane update.
            // We normally log in with m_pidl because normally we login with
            // a default directory ('\') and then change directories to the final location.
            // we do this so isolate access denied to the server and access denied to the
            // directory.
            //
            // We pass pidlXboxPath instead in this case because it will tell the browser
            // to re-direct and we won't get a chance to do the ChangeDir later.

            Str_SetPtr(&m_pszRedirPassword, szPassword);

            _RedirectAndUpdate(m_pszServer, m_ipPortNum, szUser, szPassword, pidlXboxPath, m_pszFragment, punkSite, pff);
            hr = HRESULT_FROM_WIN32(ERROR_NETWORK_ACCESS_DENIED);
        }
    }

    // Can we assume annonymous logins don't use virtual roots?
    ASSERT(FAILED(hr) || (m_pidlVirtualDir && szUser[0]) || !(m_pidlVirtualDir && szUser[0]));

    if (psb)
        psb->SetStatusMessage(IDS_EMPTY, NULL);
    ENTERCRITICALNOASSERT;

    // The directory is empty.
    _SetPidl(NULL);

    return hr;
}


/*****************************************************************************\
    FUNCTION: GetHint

    DESCRIPTION:
        An IShellFolder client wants a handle to the XBOX site.
    Pull it from the cache if possible.

    The caller should have marked the IShellFolder as busy.

    EEK!  RFC 1738 is really scary.  XBOX sites don't necessarily
    start you at the root, and RFC1738 says that xbox://foo/bar asks
    for the file bar in the DEFAULT directory, not the root!
\*****************************************************************************/
HRESULT CXboxSite::GetHint(HWND hwnd, LPCITEMIDLIST pidlXboxPath, CStatusBar * psb, HINTERNET * phint, IUnknown * punkSite, CXboxFolder * pff)
{
    HINTERNET hint = NULL;
    HINTERNET hintDll = GetWininetSessionHandle();
    HRESULT hr = S_OK;

    if (!hintDll)
    {
        // No point in retrying if we can't init Wininet
        hr = HRESULT_FROM_WIN32(GetLastError());    // Save error code
    }
    else
    {
        int cTriesLeft = 1; // This is a feature that would be cool to implement.
        hr = AssureNetConnection(NULL, hwnd, m_pszServer, NULL, TRUE);

        if (ILIsEmpty(pidlXboxPath))
            pidlXboxPath = NULL;

        if (SUCCEEDED(hr))
        {
            // BUGBUG -- I don't remember exactly what the CS is protecting
            ASSERTNONCRITICAL;
            ENTERCRITICALNOASSERT;

            do
            {
                BOOL fReuseExistingConnection = FALSE;
                hr = E_FAIL;    // We don't have our hint yet...

                ASSERTCRITICAL;
                hint = (HINTERNET) InterlockedExchangePointer(&m_hint, 0);
                if (hint)
                {
                    HINTERNET hintResponse;

                    TriggerDelayedAction(&m_hgti);      // Nothing will happen
                    fReuseExistingConnection = TRUE;    // We will need to change it for the current user.

                    // We want (S_OK == hr) if our login session is still good.  Else, we want to
                    // re-login.
                    hr = XboxCommandWrap(hint, FALSE, FALSE, FTP_TRANSFER_TYPE_ASCII, XBOX_CMD_NO_OP, NULL, &hintResponse);
                    if (SUCCEEDED(hr))
                    {
                        TraceMsg(TF_XBOXOPERATION, "CXboxSite::GetHint() We are going to use a cached HINTERNET.");
                        InternetCloseHandleWrap(hintResponse, TRUE);
                    }
                    else
                    {
                        TraceMsg(TF_XBOXOPERATION, "CXboxSite::GetHint() Can't used cached HINTERNET because server didn't respond to NOOP.");
                        InternetCloseHandleWrap(hint, TRUE);
                    }
                }
                
                if (FAILED(hr))
                {
                    hr = _LoginToTheServer(hwnd, hintDll, &hint, pidlXboxPath, psb, punkSite, pff);
                    TraceMsg(TF_XBOXOPERATION, "CXboxSite::GetHint() We had to login because we didn't have a cached HINTERNET.");
                }

                ASSERTCRITICAL;
                // BUGBUG -- is it safe to do this outside the crst?
                LEAVECRITICALNOASSERT;

                // Do we need to CD into a specific directory?  Yes, if...
                // 1. We succeeded above, AND
                // 2. We are already using a connection so the dir may be incorrect, OR
                // 3. We need a non-default dir.
                if (SUCCEEDED(hr) && (fReuseExistingConnection || pidlXboxPath))   // pidlXboxPath may be NULL.
                    hr = _SetDirectory(hint, hwnd, pidlXboxPath, psb, &cTriesLeft);

                ENTERCRITICALNOASSERT;
                ASSERTCRITICAL;
            }
            while (hr == HRESULT_FROM_WIN32(ERROR_FTP_DROPPED) && --cTriesLeft);

            LEAVECRITICALNOASSERT;
        }
    }

    *phint = hint;
    return hr;
}


HRESULT CXboxSite::_CheckToEnableCHMOD(LPCWIRESTR pwResponse)
{
    HRESULT hr = S_FALSE;
    // TODO: We should probably be more restictive in how we parse the
    //       response.  We should probably verify there is some kind of
    //       white space before and after the command.
    LPCWIRESTR pwCommand = StrStrIA(pwResponse, XBOX_UNIXCMD_CHMODA);

    // Does this XBOX server support the "SITE CHMOD" command?
    if (pwCommand)
    {
        // Yes, so we may want to use it later.
        m_fIsCHMODSupported = TRUE;

        // We can later respond with:
        // "SITE chmod xyz FileName.txt"
        // x is for Owner, (4=Read, 2=Write, 1=Execute)
        // y is for Owner, (4=Read, 2=Write, 1=Execute)
        // z is for Owner, (4=Read, 2=Write, 1=Execute)
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: _QueryServerFeatures

    DESCRIPTION:
        Find out what the server is and isn't capable of.  Information we could
    use:
        SITE: Find out OS specific commands that may be useful.  "chmod" is one
              of them.
        HELP SITE: Find out what the OS supports.
        SYST: Find out the OS type.
        NOOP: See if the connection is still alive.
        MLST: Unambiguous Directory listing with dates in UTC.
        MLSD: 
        FEAT: Features supported. UTF8 is the one we care about. 

    Response to "SITE HELP" for these servers:
        UNIX Type: L8 Version: BSD-199506
        UNIX Type: L8
           UMASK   CHMOD   GROUP   NEWER   INDEX   ALIAS   GROUPS 
           IDLE    HELP    GPASS   MINFO   EXEC    CDPATH 

        Windows_NT version 4.0
           CKM DIRSTYLE HELP STATS    
\*****************************************************************************/
HRESULT CXboxSite::_QueryServerFeatures(HINTERNET hint)
{
    HRESULT hr = E_FAIL;
    HINTERNET hintResponse;

    // Can we turn on 'UTF8' encoding?
    if (SUCCEEDED(XboxCommandWrap(hint, FALSE, FALSE, FTP_TRANSFER_TYPE_ASCII, XBOX_CMD_UTF8, NULL, &hintResponse)))
    {
        m_fInUTF8Mode = TRUE;
        m_cwe.SetUTF8Support(TRUE);
        TraceMsg(TF_XBOX_OTHER, "_QueryServerFeatures() in UTF8 Mode");

        InternetCloseHandleWrap(hintResponse, TRUE);
    }
    else
    {
        TraceMsg(TF_XBOX_OTHER, "_QueryServerFeatures() NOT in UTF8 Mode");
        m_fInUTF8Mode = FALSE;
    }

    if (!m_fFeaturesQueried)
    {
        // Is type of server software is running?  We want to know if we are running
        // on VMS, because in that case we want to fall back to HTML view (URLMON).
        // This is because the wininet guys don't want to support it.
        if (SUCCEEDED(XboxCommandWrap(hint, FALSE, FALSE, FTP_TRANSFER_TYPE_ASCII, XBOX_CMD_SYSTEM, NULL, &hintResponse)))
        {
            DWORD dwError;
            WIRECHAR wResponse[MAX_URL_STRING];
            DWORD cchSize = ARRAYSIZE(wResponse);

            if (SUCCEEDED(InternetGetLastResponseInfoWrap(TRUE, &dwError, wResponse, &cchSize)))
            {
                // Is this a VMS server?
                if (StrStrIA(wResponse, XBOX_SYST_VMS))
                    m_fIsServerVMS = TRUE;

                TraceMsg(TF_XBOX_OTHER, "_QueryServerFeatures() SYSTM returned %hs.", wResponse);
            }

            InternetCloseHandleWrap(hintResponse, TRUE);
        }


#ifdef FEATURE_CHANGE_PERMISSIONS
        // Is the server capable of supporting the UNIX "chmod" command
        // to change permissions on the file?
        if (SUCCEEDED(XboxCommandWrap(hint, FALSE, FALSE, FTP_TRANSFER_TYPE_ASCII, XBOX_CMD_SITE_HELP, NULL, &hintResponse)))
        {
            DWORD dwError;
            WIRECHAR wResponse[MAX_URL_STRING];
            DWORD cchSize = ARRAYSIZE(wResponse);

            if (SUCCEEDED(InternetGetLastResponseInfoWrap(TRUE, &dwError, wResponse, &cchSize)))
            {
                _CheckToEnableCHMOD(wResponse);
//                TraceMsg(TF_XBOX_OTHER, "_QueryServerFeatures() SITE HELP returned success");
            }

            InternetCloseHandleWrap(hintResponse, TRUE);
        }
#endif // FEATURE_CHANGE_PERMISSIONS

/*
        // Is the server capable of supporting the UNIX "chmod" command
        // to change permissions on the file?
        if (SUCCEEDED(XboxCommandWrap(hint, FALSE, FALSE, FTP_TRANSFER_TYPE_ASCII, XBOX_CMD_SITE, NULL, &hintResponse)))
        {
            DWORD dwError;
            WIRECHAR wResponse[MAX_URL_STRING];
            DWORD cchSize = ARRAYSIZE(wResponse);

            if (SUCCEEDED(InternetGetLastResponseInfoWrap(TRUE, &dwError, wResponse, &cchSize)))
            {
                TraceMsg(TF_XBOX_OTHER, "_QueryServerFeatures() SITE returned succeess");
            }

            InternetCloseHandleWrap(hintResponse, TRUE);
        }
*/
    
        m_fFeaturesQueried = TRUE;
    }

    return S_OK;    // This shouldn't fail.
}


LPITEMIDLIST CXboxSite::GetPidl(void)
{
    return ILClone(m_pidl);
}


/*****************************************************************************\
    FUNCTION: _SetPidl

    DESCRIPTION:
        m_pidl contains the ServerID and the ItemIDs making up the path of where
    m_hint is currently located.  This function will take a new path in pidlXboxPath
    and update m_pidl so it still has the server.
\*****************************************************************************/
HRESULT CXboxSite::_SetPidl(LPCITEMIDLIST pidlXboxPath)
{
    HRESULT hr = E_FAIL;
    LPITEMIDLIST pidlServer = XboxCloneServerID(m_pidl);

    if (pidlServer)
    {
        LPITEMIDLIST pidlNew = ILCombine(pidlServer, pidlXboxPath);

        if (pidlNew)
        {
            ILFree(m_pidl);
            m_pidl = pidlNew;

            hr = S_OK;
        }

        ILFree(pidlServer);
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: QueryMotd

    DESCRIPTION:
        Determine whether there is a motd at all.
\*****************************************************************************/
BOOL CXboxSite::QueryMotd(void)
{
    return m_fMotd;
}


HRESULT CXboxSite::GetVirtualRoot(LPITEMIDLIST * ppidl)
{
    HRESULT hr = S_FALSE;
    *ppidl = NULL;

    if (m_pidlVirtualDir)
    {
        *ppidl = ILClone(m_pidlVirtualDir);
        hr = S_OK;
    }

    return S_OK;
}


HRESULT CXboxSite::PidlInsertVirtualRoot(LPCITEMIDLIST pidlXboxPath, LPITEMIDLIST * ppidl)
{
    HRESULT hr = S_OK;

    if (!m_pidlVirtualDir)
        *ppidl = ILClone(pidlXboxPath);
    else
    {
        LPITEMIDLIST pidlTemp = NULL;

        if (pidlXboxPath && XboxID_IsServerItemID(pidlXboxPath))
        {
            pidlTemp = XboxCloneServerID(pidlXboxPath);
            pidlXboxPath = _ILNext(pidlXboxPath);
        }

        LPITEMIDLIST pidlWithVRoot = ILCombine(pidlTemp, m_pidlVirtualDir);
        if (pidlWithVRoot)
        {
            *ppidl = ILCombine(pidlWithVRoot, pidlXboxPath);
            ILFree(pidlWithVRoot);
        }
        
        ILFree(pidlTemp);
    }

    return S_OK;
}


BOOL CXboxSite::HasVirtualRoot(void)
{
    return (m_pidlVirtualDir ? TRUE : FALSE);
}


/*****************************************************************************\
      GetMotd
  
      Returns the HFGLOB that babysits the motd.  The refcount has been
      incremented.
\*****************************************************************************/
CXboxGlob * CXboxSite::GetMotd(void)
{
    if (m_pfgMotd)
        m_pfgMotd->AddRef();

    return m_pfgMotd;
}


/*****************************************************************************\
      GetCXboxList
  
      Return the CXboxList * that remembers which folders live in this CXboxSite *.
  
      WARNING!  The caller must own the critical section when calling
      this routine, because the returned CXboxList * is not refcounted!
\*****************************************************************************/
CXboxList * CXboxSite::GetCXboxList(void)
{
    return m_XboxDirList;
}


/*****************************************************************************\
      _CompareSites
  
      Callback during SiteCache_PrivSearch to see if the site is already in the
      list.
\*****************************************************************************/
int CALLBACK _CompareSites(LPVOID pvStrSite, LPVOID pvXboxSite, LPARAM lParam)
{
    CXboxSite * pfs = (CXboxSite *) pvXboxSite;
    LPCTSTR pszLookupStrNew = (LPCTSTR) pvStrSite;
    LPCTSTR pszLookupStr = (pfs->m_pszLookupStr ? pfs->m_pszLookupStr : TEXT(""));

    ASSERT(pszLookupStr && pszLookupStr);
    return StrCmpI(pszLookupStr, pszLookupStrNew);
}


/*****************************************************************************\
    FUNCTION: SiteCache_PrivSearch

    DESCRIPTION:
        We cache information about an XBOX Server to prevent hitting the net all
    the time.  This state is stored in CXboxSite objects and we use 'lookup strings'
    to find them.  This is what makes one server different from another.  Since
    we store password state in a CXboxSite object, we need to have one per
    user/password combo.
\*****************************************************************************/
HRESULT SiteCache_PrivSearch(LPCTSTR pszLookup, LPCITEMIDLIST pidl, IMalloc * pm, CXboxSite ** ppfs)
{
    CXboxSite * pfs = NULL;
    HRESULT hr = S_OK;

    ENTERCRITICAL;

    // CXboxSite_Init() can fail in low memory
    if (SUCCEEDED(CXboxSite_Init()))
    {
        pfs = (CXboxSite *) g_XboxSiteCache->Find(_CompareSites, (LPVOID)pszLookup);   // Add CXboxSite:: ?
        if (!pfs)
        {
            //  We need to hold the critical section while setting up
            //  the new CXboxSite structure, lest somebody else come in
            //  and try to create the same CXboxSite while we are busy.
            hr = CXboxSite_Create(pidl, pszLookup, pm, &pfs);
            if (EVAL(SUCCEEDED(hr)))
            {
                hr = g_XboxSiteCache->AppendItem(pfs);
                if (!(EVAL(SUCCEEDED(hr))))
                    IUnknown_Set(&pfs, NULL);
            }
        }
    }

    LEAVECRITICAL;
    *ppfs = pfs;
    if (pfs)
        pfs->AddRef();

    ASSERT_POINTER_MATCHES_HRESULT(*ppfs, hr);
    return hr;
}



/*****************************************************************************\
    FUNCTION: SiteCache_PidlLookupPrivHelper

    DESCRIPTION:
        We cache information about an XBOX Server to prevent hitting the net all
    the time.  This state is stored in CXboxSite objects and we use 'lookup strings'
    to find them.  This is what makes one server different from another.  Since
    we store password state in a CXboxSite object, we need to have one per
    user/password combo.
    
        SiteCache_PidlLookup() does the high level work of deciding if we want
    to do a password redirect.  This function just wraps the creating of the
    lookup string and the fetching of the site.
\*****************************************************************************/
HRESULT SiteCache_PidlLookupPrivHelper(LPCITEMIDLIST pidl, IMalloc * pm, CXboxSite ** ppfs)
{
    HRESULT hr = E_FAIL;
    TCHAR szLookup[MAX_PATH];

    *ppfs = NULL;
    hr = PidlGenerateSiteLookupStr(pidl, szLookup, ARRAYSIZE(szLookup));
    // May fail w/Outofmemory

    if (SUCCEEDED(hr))
        hr = SiteCache_PrivSearch((pidl ? szLookup : TEXT('\0')), pidl, pm, ppfs);

    ASSERT_POINTER_MATCHES_HRESULT(*ppfs, hr);
    return hr;
}


/*****************************************************************************\
    FUNCTION: SiteCache_PidlLookupPrivHelper

    DESCRIPTION:
        We cache information about an XBOX Server to prevent hitting the net all
    the time.  This state is stored in CXboxSite objects and we use 'lookup strings'
    to find them.  This is what makes one server different from another.  Since
    we store password state in a CXboxSite object, we need to have one per
    user/password combo.
\*****************************************************************************/
HRESULT SiteCache_PidlLookup(LPCITEMIDLIST pidl, BOOL fPasswordRedir, IMalloc * pm, CXboxSite ** ppfs)
{
    HRESULT hr = E_FAIL;

    if (pidl && !ILIsEmpty(pidl))
    {
        hr = SiteCache_PidlLookupPrivHelper(pidl, pm, ppfs);

        // Okay, we found a site but we may need to redirect to another site
        // because the password is wrong.  This happens if a user goes to
        // ServerA w/UserA and PasswordA but PasswordA is invalid.  So,
        // PasswordB is entered and the navigation completes successfully.
        // Now either the navigation occurs again with PasswordA or w/o
        // a password (because the addrbar removes it), then we need to
        // look it up again and get it.
        if (SUCCEEDED(hr) && (*ppfs)->m_pszRedirPassword && fPasswordRedir)
        {
            LPITEMIDLIST pidlNew;   // with new (redirected) password

            if (XboxPidl_IsAnonymous(pidl))
            {
                pidlNew = ILClone(pidl);
                if (!pidlNew)
                    hr = E_OUTOFMEMORY;
            }
            else
            {
                // We need to redirect to get that CXboxSite.
                hr = PidlReplaceUserPassword(pidl, &pidlNew, pm, NULL, (*ppfs)->m_pszRedirPassword);
            }

            (*ppfs)->Release();
            *ppfs = NULL;
            if (SUCCEEDED(hr))
            {
                hr = SiteCache_PidlLookupPrivHelper(pidlNew, pm, ppfs);
                ILFree(pidlNew);
            }
        }
    }

    ASSERT_POINTER_MATCHES_HRESULT(*ppfs, hr);
    return hr;
}


/*****************************************************************************\
     FUNCTION: UpdateHiddenPassword

     DESCRIPTION:
        Since our IShellFolder::GetDisplayNameOf() will hide the password in some
     cases, we need to 'patch' display names that come thru our
     IShellFolder::GetDisplayName().  If a display name is coming in, we will
     see if the CXboxSite has a m_pszRedirPassword.  If it did, then the user entered
     a password via the 'Login As...' dialog in place of the empty password,
     which made it hidden.  If this is the case, we then have IShellFolder::ParseDisplayName()
     patch back in the password.
\*****************************************************************************/
HRESULT CXboxSite::UpdateHiddenPassword(LPITEMIDLIST pidl)
{
    HRESULT hr = S_FALSE;
    TCHAR szUserName[INTERNET_MAX_USER_NAME_LENGTH];
    TCHAR szPassword[INTERNET_MAX_PASSWORD_LENGTH];

    // Is it a candidate to a password to be inserted?
    if (m_pszPassword && 
        EVAL(SUCCEEDED(XboxPidl_GetUserName(pidl, szUserName, ARRAYSIZE(szUserName)))) &&
        szUserName[0] &&
        SUCCEEDED(XboxPidl_GetPassword(pidl, szPassword, ARRAYSIZE(szPassword), TRUE)) &&
        !szPassword[0]) 
    {
        // Yes...
        hr = XboxServerID_SetHiddenPassword(pidl, m_pszPassword);
    }

    return hr;
}


/*****************************************************************************\
     CXboxSite::GetXboxDir
\*****************************************************************************/
HRESULT CXboxSite::GetXboxDir(LPCTSTR pszServer, LPCWSTR pszUrlPath, CXboxDir ** ppfd)
{
    HRESULT hr = S_OK;
    TCHAR szUrl[MAX_URL_STRING];

    *ppfd = NULL;
    hr = UrlCreate(pszServer, NULL, NULL, pszUrlPath, NULL, INTERNET_DEFAULT_FTP_PORT, NULL, szUrl, ARRAYSIZE(szUrl));
    if (EVAL(SUCCEEDED(hr)))
    {
        LPITEMIDLIST pidl;

        // We know this is a path.
        hr = CreateXboxPidlFromUrlEx(szUrl, GetCWireEncoding(), NULL, &pidl, m_pm, FALSE, TRUE, TRUE);
        if (EVAL(SUCCEEDED(hr)))
        {
            hr = GetXboxDir(pidl, ppfd);
            ILFree(pidl);
        }
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: GetXboxDir

    DESCRIPTION:
        Obtain the XboxDir structure for an XBOX site, creating one if
    necessary.  It is the caller's responsibility to Release the
    XboxDir when finished.
\*****************************************************************************/
HRESULT CXboxSite::GetXboxDir(LPCITEMIDLIST pidl, CXboxDir ** ppfd)
{
    HRESULT hr = S_OK;
    CXboxDir * pfd = NULL;

    ENTERCRITICAL;
    ASSERT(ppfd && m_XboxDirList);

    pfd = (CXboxDir *) m_XboxDirList->Find(_CompareDirs, (LPVOID) pidl);
    if (!pfd)
    {
        // We need to hold the critical section while setting up
        // the new XboxDir structure, lest somebody else come in
        // and try to create the same XboxDir while we are busy.
        hr = CXboxDir_Create(this, pidl, &pfd);
        if (EVAL(SUCCEEDED(hr)))
        {
            // NOTE: REF-COUNTING
            //      Note that CXboxDir has a pointer (m_pfs) to a CXboxSite.
            //      We just added a back pointer in CXboxSite's list of CXboxDir(s),
            //      so it's necessary for that back pointer to not have a ref.
            //      This will not be a problem because the back pointers will
            //      always be valid because: 1) CXboxDir's destructor removes the backpointer,
            //      and 2) CXboxDir holds a ref on CXboxSite, so it won't go away until
            //      all the CXboxDir(s) are good and ready.  -BryanSt
            hr = m_XboxDirList->AppendItem(pfd);
            if (FAILED(hr))
                IUnknown_Set(&pfd, NULL);
        }
    }
    LEAVECRITICAL;

    *ppfd = pfd;
    if (pfd)
        pfd->AddRef();

    return hr;
}


/*****************************************************************************\
    FUNCTION: FlushSubDirs

    DESCRIPTION:
        Every subdir of pidl is no longer valid so flush them.  This is done
    because the parent dir may have changed names so they are invalid.

    PARAMETERS:
        pidl: Path of ItemIDs (no-ServerID) that includes the full path w/o
              the virtual root.  This matches CXboxDir::m_pidlXboxDir
\*****************************************************************************/
HRESULT CXboxSite::FlushSubDirs(LPCITEMIDLIST pidl)
{
    HRESULT hr = S_OK;
    CXboxDir * pfd = NULL;
    int nIndex;

    ENTERCRITICAL;

    // Count down so deleting items won't screw up the indicies.
    for (nIndex = (m_XboxDirList->GetCount() - 1); nIndex >= 0; nIndex--)
    {
        pfd = (CXboxDir *) m_XboxDirList->GetItemPtr(nIndex);
        if (pfd)
        {
            // Is this a child?
            if (XboxItemID_IsParent(pidl, pfd->GetPathPidlReference()))
            {
                // Yes, pfd is a child of pidl so delete it.
                m_XboxDirList->DeletePtrByIndex(nIndex);
                pfd->Release();
            }
        }
    }
    LEAVECRITICAL;

    return hr;
}


BOOL CXboxSite::IsSiteBlockedByRatings(HWND hwndDialogOwner)
{
    if (!m_fRatingsChecked)
    {
        void * pvRatingDetails = NULL;
        TCHAR szUrl[MAX_URL_STRING];
        CHAR szUrlAnsi[MAX_URL_STRING];
        HRESULT hr = S_OK;  // Assume allowed (in case no ratings)

        EVAL(SUCCEEDED(UrlCreateFromPidlW(m_pidl, SHGDN_FORPARSING, szUrl, ARRAYSIZE(szUrl), (ICU_ESCAPE | ICU_USERNAME), FALSE)));
        SHTCharToAnsi(szUrl, szUrlAnsi, ARRAYSIZE(szUrlAnsi));

        if (IS_RATINGS_ENABLED())
        {
            // S_OK - Allowed, S_FALSE - Not Allowed, FAILED() - not rated.
            hr = RatingCheckUserAccess(NULL, szUrlAnsi, NULL, NULL, 0, &pvRatingDetails);
            if (S_OK != hr)    // Does user want to override with parent password in dialog?
                hr = RatingAccessDeniedDialog2(hwndDialogOwner, NULL, pvRatingDetails);

            if (pvRatingDetails)
                RatingFreeDetails(pvRatingDetails);
        }

        if (S_OK == hr)     // It's off by default.
            m_fRatingsAllow = TRUE;

        m_fRatingsChecked = TRUE;
    }

    return !m_fRatingsAllow;
}


/*****************************************************************************\
      CXboxSite_Init
  
      Initialize the global list of XBOX sites.
  
      Note that the DLL refcount is decremented after this is created,
      so that this internal list doesn't prevent us from unloading.
\*****************************************************************************/
HRESULT CXboxSite_Init(void)
{
    HRESULT hr = S_OK;

    if (!g_XboxSiteCache)
        hr = CXboxList_Create(10, NULL, 10, &g_XboxSiteCache);

    return hr;
}


/*****************************************************************************\
      XboxSitePurge_CallBack
  
      Purge the global list of XBOX sites.
\*****************************************************************************/
int XboxSitePurge_CallBack(LPVOID pvPunk, LPVOID pv)
{
    IUnknown * punk = (IUnknown *) pvPunk;

    if (punk)
        punk->Release();

    return 1;
}


/*****************************************************************************\
      CXboxPunkList_Purge
  
      Purge the global list of XBOX sites.
\*****************************************************************************/
HRESULT CXboxPunkList_Purge(CXboxList ** pfl)
{
    TraceMsg(TF_XBOX_DLLLOADING, "CXboxPunkList_Purge() Purging our cache.");
    if (*pfl)
    {
        (*pfl)->Enum(XboxSitePurge_CallBack, NULL);
        IUnknown_Set(pfl, NULL);
    }

    return S_OK;
}


/*****************************************************************************\
      CXboxSite_Create
  
      Create a brand new CXboxSite given a name.
\*****************************************************************************/
HRESULT CXboxSite_Create(LPCITEMIDLIST pidl, LPCTSTR pszLookupStr, IMalloc * pm, CXboxSite ** ppfs)
{
    CXboxSite * pfs = new CXboxSite();
    HRESULT hr = E_OUTOFMEMORY;

    ASSERT(pidl && pszLookupStr && ppfs);
    *ppfs = NULL;
    if (EVAL(pfs))
    {
        Str_SetPtr(&pfs->m_pszLookupStr, pszLookupStr);

        IUnknown_Set((IUnknown **) &(pfs->m_pm), pm);
        hr = CXboxList_Create(10, NULL, 10, &pfs->m_XboxDirList);
        if (EVAL(SUCCEEDED(hr)))
        {
            // Did someone give us an empty URL?
            if (EVAL(pidl) && EVAL(XboxPidl_IsValid(pidl)))
            {
                TCHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH];
                TCHAR szUser[INTERNET_MAX_USER_NAME_LENGTH];
                TCHAR szPassword[INTERNET_MAX_PASSWORD_LENGTH];
                TCHAR szFragment[INTERNET_MAX_PASSWORD_LENGTH];

                EVAL(SUCCEEDED(XboxPidl_GetServer(pidl, szServer, ARRAYSIZE(szServer))));
                Str_SetPtr(&pfs->m_pszServer, szServer);

                Pidl_Set(&pfs->m_pidl, pidl);

                EVAL(SUCCEEDED(XboxPidl_GetUserName(pidl, szUser, ARRAYSIZE(szUser))));
                Str_SetPtr(&pfs->m_pszUser, szUser);
                
                if (FAILED(XboxPidl_GetPassword(pidl, szPassword, ARRAYSIZE(szPassword), TRUE)))
                {
                       // Password expired
                    szPassword[0] = 0;
                }

                Str_SetPtr(&pfs->m_pszPassword, szPassword);
                XboxPidl_GetFragment(pidl, szFragment, ARRAYSIZE(szFragment));
                Str_SetPtr(&pfs->m_pszFragment, szFragment);

                pfs->m_ipPortNum = XboxPidl_GetPortNum(pidl);

                switch (XboxPidl_GetDownloadType(pidl))
                {
                case FTP_TRANSFER_TYPE_UNKNOWN:
                    pfs->m_fDLTypeSpecified = FALSE;
                    pfs->m_fASCIIDownload = FALSE;
                    break;
                case FTP_TRANSFER_TYPE_ASCII:
                    pfs->m_fDLTypeSpecified = TRUE;
                    pfs->m_fASCIIDownload = TRUE;
                    break;
                case FTP_TRANSFER_TYPE_BINARY:
                    pfs->m_fDLTypeSpecified = TRUE;
                    pfs->m_fASCIIDownload = FALSE;
                    break;
                default:
                    ASSERT(0);
                }
            }
            else
            {
                Str_SetPtr(&pfs->m_pszServer, NULL);
                Str_SetPtr(&pfs->m_pszUser, NULL);
                Str_SetPtr(&pfs->m_pszPassword, NULL);
                Str_SetPtr(&pfs->m_pszFragment, NULL);

                Pidl_Set(&pfs->m_pidl, NULL);
                pfs->m_fDLTypeSpecified = FALSE;
            }
            *ppfs = pfs;
        }
        else
        {
            hr = E_FAIL;
            pfs->Release();
        }
    }

    return hr;
}



/****************************************************\
    Constructor
\****************************************************/
CXboxSite::CXboxSite() : m_cRef(1)
{
    DllAddRef();

    // This needs to be allocated in Zero Inited Memory.
    // Assert that all Member Variables are inited to Zero.
    ASSERT(!m_pszServer);
    ASSERT(!m_pidl);
    ASSERT(!m_pszUser);
    ASSERT(!m_pszPassword);
    ASSERT(!m_pszFragment);
    ASSERT(!m_pszLookupStr);
    ASSERT(!m_pidlVirtualDir);

    ASSERT(!m_fMotd);
    ASSERT(!m_hint);
    ASSERT(!m_hgti);
    ASSERT(!m_XboxDirList);
    ASSERT(!m_fRatingsChecked);
    ASSERT(!m_fRatingsAllow);

    LEAK_ADDREF(LEAK_CXboxSite);
}


/****************************************************\
    Destructor
\****************************************************/
CXboxSite::~CXboxSite()
{
    FlushHint();        // Frees m_hgti

    Str_SetPtr(&m_pszServer, NULL);
    Str_SetPtr(&m_pszUser, NULL);
    Str_SetPtr(&m_pszPassword, NULL);
    Str_SetPtr(&m_pszFragment, NULL);
    Str_SetPtr(&m_pszLookupStr, NULL);
    Str_SetPtr(&m_pszRedirPassword, NULL);

    Pidl_Set(&m_pidlVirtualDir, NULL);
    Pidl_Set(&m_pidl, NULL);

    IUnknown_Set(&m_pfgMotd, NULL);

    ASSERTCRITICAL;

    CXboxPunkList_Purge(&m_XboxDirList);

    TriggerDelayedAction(&m_hgti);    // Out goes the cached handle
    ASSERT(m_hint == 0);        // Make sure he's gone
    ATOMICRELEASE(m_pm);

    DllRelease();
    LEAK_DELREF(LEAK_CXboxSite);
}


//===========================
// *** IUnknown Interface ***
ULONG CXboxSite::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG CXboxSite::Release()
{
    ASSERT(m_cRef > 0);
    m_cRef--;

    if (m_cRef > 0)
        return m_cRef;

    delete this;
    return 0;
}


HRESULT CXboxSite::QueryInterface(REFIID riid, void **ppvObj)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObj = SAFECAST(this, IUnknown*);
    }
    else
    {
        TraceMsg(TF_XBOXQI, "CXboxSite::QueryInterface() failed.");
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}
