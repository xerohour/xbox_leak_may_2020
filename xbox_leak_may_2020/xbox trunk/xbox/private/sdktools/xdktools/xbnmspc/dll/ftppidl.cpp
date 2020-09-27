    /*****************************************************************************\
    xboxpidl.cpp - Pointers to Item ID Lists

    This is the only file that knows the internal format of our IDLs.
\*****************************************************************************/

#include "priv.h"
#include "xboxpidl.h"
#include "xboxurl.h"
#include "cookie.h"

#define NOT_INITIALIZED         10
DWORD g_fNoPasswordsInAddressBar = NOT_INITIALIZED;

#define SESSIONKEY      FILETIME

// Private XboxServerID Helpers
HRESULT XboxServerID_GetServer(LPCITEMIDLIST pidl, LPTSTR szServer, DWORD cchSize);
DWORD XboxItemID_GetTypeID(LPCITEMIDLIST pidl);

// v0 never went to customers but was used in NT5 before 1799       - Shipped in: Never.
// v1 This switch was to use password cookies for a security fix.   - Shipped in: Never.
// v2 this was done to not use the IDelegate's IMalloc for non-first ItemIDs  - Shipped in: Never (5/15/98)
// v3 add extra padding to ItemIDs so their dwType matches that of ServerIDs - Shipped in: IE5b1, IE5b2, NT5b2 (5/25/98)
// v4 add wzDisplayName to XboxItemID                                - Shipped in: IE5 RTM & NT5 b3  (11/16/98)

#define PIDL_VERSION_NUMBER_UPGRADE 3
#define PIDL_VERSION_NUMBER 4



#define     SIZE_ITEMID_SIZEFIELD        (sizeof(DWORD) + sizeof(WORD))
#define     SIZE_ITEMID_TERMINATOR       (sizeof(DWORD))


/****************************************************\
    IDType

    DESCRIPTION:
        These bits go into XBOXIDLIST.dwIDType and describe
    what type of pidl it is AND which areas of the
    data structure have been verified by getting the
    data directly from the server.
\****************************************************/

#define IDTYPE_ISVALID           0x00000001    // Set if TYPE is valid
#define IDTYPE_SERVER            (0x00000002 | IDTYPE_ISVALID)    // Server
#define IDTYPE_DIR               (0x00000004 | IDTYPE_ISVALID)    // Folder/Dir
#define IDTYPE_FILE              (0x00000008 | IDTYPE_ISVALID)    // File
#define IDTYPE_FILEORDIR         (0x00000010 | IDTYPE_ISVALID)    // File or Dir.  Wasn't specified.
#define IDTYPE_FRAGMENT          (0x00000020 | IDTYPE_ISVALID)    // File Fragment (i.e. foobar.htm#SECTION_3)

// These are bits that indicate
// For Server ItemIDs
#define IDVALID_PORT_NUM         0x00000100     // Was the port number specified
#define IDVALID_USERNAME         0x00000200     // Was the login name specified
#define IDVALID_PASSWORD         0x00000400     // Was the password specified
#define IDVALID_DLTYPE           0x00000800     // Download Type is specified.
#define IDVALID_DL_ASCII         0x00001000     // Download as ASCII if set, otherwise, download as BINARY.
#define IDVALID_HIDE_PASSWORD    0x00002000     // The Password entry is invalid so use the sessionkey to look it up.

#define VALID_SERVER_BITS (IDTYPE_ISVALID|IDTYPE_SERVER|IDVALID_PORT_NUM|IDVALID_USERNAME|IDVALID_PASSWORD|IDVALID_DLTYPE|IDVALID_DL_ASCII|IDVALID_HIDE_PASSWORD)
#define IS_VALID_SERVER_ITEMID(pItemId) (!(pItemId & ~VALID_SERVER_BITS))

// For Dir/File ItemIDs
#define IDVALID_FILESIZE         0x00010000     // Did we get the file size from the server?
#define IDVALID_MOD_DATE         0x00020000     // Did we get the modification date from the server?

#define VALID_DIRORFILE_BITS (IDTYPE_ISVALID|IDTYPE_DIR|IDTYPE_FILE|IDTYPE_FILEORDIR|IDTYPE_FRAGMENT|IDVALID_FILESIZE|IDVALID_MOD_DATE)
#define IS_VALID_DIRORFILE_ITEMID(pItemId) (!(pItemId & (~VALID_DIRORFILE_BITS & ~IDTYPE_ISVALID)))


#define IS_FRAGMENT(pXboxIDList)       (IDTYPE_ISVALID != (IDTYPE_FRAGMENT & pXboxIDList->dwIDType))

///////////////////////////////////////////////////////////
// XBOX Pidl Helper Functions 
///////////////////////////////////////////////////////////


/*****************************************************************************\
    FUNCTION: UrlGetAbstractPathFromPidl

    DESCRIPTION:
        pszUrlPath will be UNEscaped and in Wire Bytes.
\*****************************************************************************/
HRESULT UrlGetAbstractPathFromPidl(LPCITEMIDLIST pidl, BOOL fDirsOnly, BOOL fInWireBytes, void * pvPath, DWORD cchUrlPathSize)
{
    HRESULT hr = S_OK;
    LPWIRESTR pwWirePath = (LPWIRESTR) pvPath;
    LPWSTR pwzDisplayPath = (LPWSTR) pvPath;

    if (!EVAL(XboxPidl_IsValid(pidl)))
        return E_INVALIDARG;

    ASSERT(pvPath && (0 < cchUrlPathSize));
    ASSERT(IsValidPIDL(pidl));

    if (fInWireBytes)
    {
        pwWirePath[0] = '/';
        pwWirePath[1] = '\0'; // Make this path absolute.
    }
    else
    {
        pwzDisplayPath[0] = L'/';
        pwzDisplayPath[1] = L'\0'; // Make this path absolute.
    }

    if (!ILIsEmpty(pidl) && XboxID_IsServerItemID(pidl))       // If it's not a server, we are screwed.
        pidl = _ILNext(pidl);   // Skip past the Server Pidl.

    for (; !ILIsEmpty(pidl); pidl = _ILNext(pidl))
    {
        if (!fDirsOnly || XboxItemID_IsDirectory(pidl, TRUE) || !ILIsEmpty(_ILNext(pidl)))
        {
            if (!XboxItemID_IsFragment(pidl))
            {
                if (fInWireBytes)
                {
                    LPCWIRESTR pwWireName = XboxItemID_GetWireNameReference(pidl);

                    if (pwWireName)
                    {
                        // The caller should never need the URL Path escaped because
                        // that will happen when it's converted into an URL.
                        WirePathAppend(pwWirePath, cchUrlPathSize, pwWireName);
                    }
                }
                else
                {
                    LPCWSTR pwzDisplayName = XboxItemID_GetDisplayNameReference(pidl);

                    if (pwzDisplayName)
                    {
                        // The caller should never need the URL Path escaped because
                        // that will happen when it's converted into an URL.
                        DisplayPathAppend(pwzDisplayPath, cchUrlPathSize, pwzDisplayName);
                    }
                }
            }
        }

        if (SUCCEEDED(hr) && XboxItemID_IsDirectory(pidl, FALSE))
        {
            if (fInWireBytes)
                WirePathAppendSlash(pwWirePath, cchUrlPathSize); // Always make sure dirs end in '/'.
            else
                DisplayPathAppendSlash(pwzDisplayPath, cchUrlPathSize); // Always make sure dirs end in '/'.
        }
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: GetDisplayPathFromPidl

    DESCRIPTION:
        pwzDisplayPath will be UNEscaped and in display unicode.
\*****************************************************************************/
HRESULT GetDisplayPathFromPidl(LPCITEMIDLIST pidl, LPWSTR pwzDisplayPath, DWORD cchUrlPathSize, BOOL fDirsOnly)
{
    return UrlGetAbstractPathFromPidl(pidl, fDirsOnly, FALSE, (void *) pwzDisplayPath, cchUrlPathSize);
}


/*****************************************************************************\
    FUNCTION: GetWirePathFromPidl

    DESCRIPTION:
        pszUrlPath will be UNEscaped and in Wire Bytes.
\*****************************************************************************/
HRESULT GetWirePathFromPidl(LPCITEMIDLIST pidl, LPWIRESTR pwWirePath, DWORD cchUrlPathSize, BOOL fDirsOnly)
{
    return UrlGetAbstractPathFromPidl(pidl, fDirsOnly, TRUE, (void *) pwWirePath, cchUrlPathSize);
}


/*****************************************************************************\
    FUNCTION: UrlGetFileNameFromPidl

    DESCRIPTION:
        pszFileName will be filled with the file name if it exists.  If it doesn't,
    S_FALSE will be returned if the PIDL only points to a directory and not
    a file.
HRESULT UrlGetFileNameFromPidl(LPCITEMIDLIST pidl, LPTSTR pszFileName, DWORD cchSize)
{
    HRESULT hr = S_FALSE;

    if (!EVAL(XboxPidl_IsValid(pidl)))
        return E_INVALIDARG;

    ASSERT(pszFileName && (0 < cchSize));
    ASSERT(IsValidPIDL(pidl));
    pszFileName[0] = TEXT('\0');

    if (EVAL(XboxID_IsServerItemID(pidl)))       // If it's not a server, we are screwed.
        pidl = _ILNext(pidl);   // Skip past the Server Pidl.

    for (; !ILIsEmpty(pidl); pidl = _ILNext(pidl))
    {
        if (!XboxItemID_IsDirectory(pidl, FALSE))
        {
            // This isn't a directory, so the only time it's valid to not
            // be the last ItemID is if this is a file ItemID and the next
            // ItemID is a Fragment.
            ASSERT(ILIsEmpty(_ILNext(pidl)) || XboxItemID_IsFragment(_ILNext(pidl)));
            SHAnsiToTChar(XboxItemID_GetFileName(pidl), pszFileName, cchSize);
            hr = S_OK;
            break;
        }
    }

    return hr;
}
\*****************************************************************************/


#ifndef UNICODE
/*****************************************************************************\
    FUNCTION: UrlCreateFromPidlW

    DESCRIPTION:
        x.
\*****************************************************************************/
HRESULT UrlCreateFromPidlW(LPCITEMIDLIST pidl, DWORD shgno, LPWSTR pwzUrl, DWORD cchSize, DWORD dwFlags, BOOL fHidePassword)
{
    HRESULT hr;
    TCHAR szUrl[MAX_URL_STRING];

    hr = UrlCreateFromPidl(pidl, shgno, szUrl, ARRAYSIZE(szUrl), dwFlags, fHidePassword);
    if (SUCCEEDED(hr))
        SHTCharToUnicode(szUrl, pwzUrl, cchSize);

    return hr;
}

#else // UNICODE

/*****************************************************************************\
    FUNCTION: UrlCreateFromPidlA

    DESCRIPTION:
        x.
\*****************************************************************************/
HRESULT UrlCreateFromPidlA(LPCITEMIDLIST pidl, DWORD shgno, LPSTR pszUrl, DWORD cchSize, DWORD dwFlags, BOOL fHidePassword)
{
    HRESULT hr;
    TCHAR szUrl[MAX_URL_STRING];

    hr = UrlCreateFromPidl(pidl, shgno, szUrl, ARRAYSIZE(szUrl), dwFlags, fHidePassword);
    if (SUCCEEDED(hr))
        SHTCharToAnsi(szUrl, pszUrl, cchSize);

    return hr;
}

#endif // UNICODE


BOOL IncludePassword(void)
{
    if (NOT_INITIALIZED == g_fNoPasswordsInAddressBar)
        g_fNoPasswordsInAddressBar = !SHRegGetBoolUSValue(SZ_REGKEY_XBOXFOLDER, SZ_REGVALUE_PASSWDSIN_ADDRBAR, FALSE, TRUE);

    return g_fNoPasswordsInAddressBar;
}


HRESULT ParseUrlCreateFromPidl(LPCITEMIDLIST pidl, LPTSTR pszUrl, DWORD cchSize, DWORD dwFlags, BOOL fHidePassword)
{
    HRESULT hr = S_OK;
    TCHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH];
    TCHAR szUrlPath[MAX_URL_STRING];
    TCHAR szUserName[INTERNET_MAX_USER_NAME_LENGTH];
    TCHAR szPassword[INTERNET_MAX_PASSWORD_LENGTH];
    TCHAR szFragment[MAX_PATH];
    TCHAR szDownloadType[MAX_PATH] = TEXT("");
    INTERNET_PORT ipPortNum = INTERNET_DEFAULT_FTP_PORT;

    if (ILIsEmpty(pidl))
    {
        ASSERT(0); // BUGBUG: Work around until we can figure out why CXboxFolder has ILIsEmpty(m_pidlHere).
        szServer[0] = szUrlPath[0] = szUserName[0] = szPassword[0] = TEXT('\0');
        hr = E_FAIL;
    }
    else
    {
        XboxPidl_GetServer(pidl, szServer, ARRAYSIZE(szServer));
        GetDisplayPathFromPidl(pidl, szUrlPath, ARRAYSIZE(szUrlPath), FALSE);
        XboxPidl_GetUserName(pidl, szUserName, ARRAYSIZE(szUserName));
        if (FAILED(XboxPidl_GetPassword(pidl, szPassword, ARRAYSIZE(szPassword), !fHidePassword)))
            szPassword[0] = 0;

        XboxPidl_GetFragment(pidl, szFragment, ARRAYSIZE(szPassword));
        XboxPidl_GetDownloadTypeStr(pidl, szDownloadType, ARRAYSIZE(szDownloadType));
        UrlPathAdd(szUrlPath, ARRAYSIZE(szUrlPath), szDownloadType);
        ipPortNum = XboxPidl_GetPortNum(pidl);
    }

    if (SUCCEEDED(hr))
    {
        TCHAR szUserNameEscaped[INTERNET_MAX_USER_NAME_LENGTH];

        szUserNameEscaped[0] = 0;
        if (szUserName[0])
            EscapeString(szUserName, szUserNameEscaped, ARRAYSIZE(szUserNameEscaped));

        hr = UrlCreateEx(szServer, NULL_FOR_EMPTYSTR(szUserNameEscaped), szPassword, szUrlPath, szFragment, ipPortNum, szDownloadType, pszUrl, cchSize, dwFlags);
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: GetFullPrettyName
    
    DESCRIPTION:
        The user wants a pretty name so these are the cases we need to worry
    about:
    URL:                                               Pretty Name:
    ----------------------------------                 ---------------------
    xbox://joe:psswd@serv/                              serv
    xbox://joe:psswd@serv/dir1/                         dir1 on serv
    xbox://joe:psswd@serv/dir1/dir2/                    dir2 on serv
    xbox://joe:psswd@serv/dir1/dir2/file.txt            file.txt on serv
\*****************************************************************************/
HRESULT GetFullPrettyName(LPCITEMIDLIST pidl, LPTSTR pszUrl, DWORD cchSize)
{
    HRESULT hr = S_OK;
    TCHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH];

    XboxPidl_GetServer(pidl, szServer, ARRAYSIZE(szServer));
    // Is there anything after the ServerItemID?
    if (!ILIsEmpty(_ILNext(pidl)))
    {
        // Yes, so let's get the name of the last item and
        // make the string "<LastItemName> on <Server>".
        LPCWSTR pwzLastItem = XboxItemID_GetDisplayNameReference(ILFindLastID(pidl));
        LPTSTR pszStrArray[] = {szServer, (LPTSTR)pwzLastItem};
        TCHAR szTemplate[MAX_PATH];
        
        EVAL(LoadString(HINST_THISDLL, IDS_PRETTYNAMEFORMAT, szTemplate, ARRAYSIZE(szTemplate)));
        EVAL(FormatMessage((FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY), (LPVOID)szTemplate,
                            0, 0, pszUrl, cchSize, (va_list*)pszStrArray));
    }
    else
    {
        // No, so we are done.
        StrCpyN(pszUrl, szServer, cchSize);
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: UrlCreateFromPidl
    
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
HRESULT UrlCreateFromPidl(LPCITEMIDLIST pidl, DWORD shgno, LPTSTR pszUrl, DWORD cchSize, DWORD dwFlags, BOOL fHidePassword)
{
    HRESULT hr = S_OK;

    pszUrl[0] = 0;
    if (!EVAL(pidl) ||
        !EVAL(IsValidPIDL(pidl)) ||
        !XboxPidl_IsValid(pidl) ||
        !XboxID_IsServerItemID(pidl) ||
        !EVAL(pszUrl && (0 < cchSize)))
    {
        return E_INVALIDARG;
    }

    if (shgno & SHGDN_INFOLDER)
    {
        // shgno & SHGDN_INFOLDER ?
        LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

        if (EVAL(pidlLast && !ILIsEmpty(pidlLast)))
        {
            hr = XboxPidl_GetDisplayName(pidlLast, pszUrl, cchSize);

            // Do they want to reparse it later?  If they do and it's
            // a server, we need to give out the scheme also.
            // (SHGDN_INFOLDER) = "ServerName"
            // (SHGDN_INFOLDER|SHGDN_FORPARSING) = "xbox://ServerName/"
            if ((shgno & SHGDN_FORPARSING) &&
                (XboxID_IsServerItemID(pidlLast)))
            {
                // Yes, so we need to add the server name.
                TCHAR szServerName[MAX_PATH];

                StrCpyN(szServerName, pszUrl, ARRAYSIZE(szServerName));
                wnsprintf(pszUrl, cchSize, TEXT("xbox://%s/"), szServerName);
            }
        }
        else
            hr = E_FAIL;
    }
    else
    {
        // Assume they want the full URL.
        if (!EVAL((shgno & SHGDN_FORPARSING) || 
               (shgno & SHGDN_FORADDRESSBAR) ||
               (shgno == SHGDN_NORMAL)))
        {
            TraceMsg(TF_ALWAYS, "UrlCreateFromPidl() shgno=%#08lx and I dont know what to do with that.", shgno);
        }

        if ((shgno & SHGDN_FORPARSING) || (shgno & SHGDN_FORADDRESSBAR))
        {
            hr = ParseUrlCreateFromPidl(pidl, pszUrl, cchSize, dwFlags, fHidePassword);
        }
        else
            hr = GetFullPrettyName(pidl, pszUrl, cchSize);
    }

//    TraceMsg(TF_XBOXURL_UTILS, "UrlCreateFromPidl() pszUrl=%ls, shgno=%#08lX", pszUrl, shgno);
    return hr;
}


/*****************************************************************************\
    FUNCTION: CreateXboxPidlFromDisplayPathHelper

    DESCRIPTION:
        The work done in CreateXboxPidlFromUrlPath requires a fair amount of
    stack space so we do most of the work in CreateXboxPidlFromDisplayPathHelper
    to prevent overflowing the stack.
\*****************************************************************************/
HRESULT CreateXboxPidlFromDisplayPathHelper(LPCWSTR pwzFullPath, CWireEncoding * pwe, ULONG *pcchEaten, LPITEMIDLIST * ppidl, BOOL fIsTypeKnown, BOOL fIsDir, LPITEMIDLIST * ppidlCurrentID, LPWSTR * ppwzRemaining)
{
    HRESULT hr = E_FAIL;
    LPITEMIDLIST pidl;
    WCHAR wzFirstItem[MAX_PATH];
    WIRECHAR wFirstWireItem[MAX_PATH];
    WCHAR wzRemaining[MAX_PATH];
    BOOL fIsCurrSegmentADir = FALSE;
    BOOL fIsCurrSegmentTypeKnown = fIsTypeKnown;
    BOOL fIsFragSeparator = FALSE;

    *ppwzRemaining = NULL;
    *ppidl = 0;

    if (pcchEaten)
        *pcchEaten = 0;     // The caller will parse the entire URL so we don't need to fill this in.

    if (L'/' == pwzFullPath[0])
        pwzFullPath = (LPWSTR) CharNextW(pwzFullPath);

    DisplayPathGetFirstSegment(pwzFullPath, wzFirstItem, ARRAYSIZE(wzFirstItem), NULL, wzRemaining, ARRAYSIZE(wzRemaining), &fIsCurrSegmentADir);
    // Is this the last segment?
    if (!wzRemaining[0])
    {
        // Yes, so if the caller knows the type of the last segment, use it now.
        if (fIsTypeKnown)
            fIsCurrSegmentADir = fIsDir;
    }
    else
    {
        // No, so we are assured that fIsDirCurrent is correct because it must have been followed
        // by a '/', or how could it be followed by another path segment?
        fIsCurrSegmentTypeKnown = TRUE;
        ASSERT(fIsCurrSegmentADir);
    }

    // NOTE: If the user entered "xbox://serv/Dir1/Dir2" fIsDir will be false for Dir2.
    //       It will be marked as ambigious. (TODO: Check for extension?)

    EVAL(SUCCEEDED(pwe->UnicodeToWireBytes(NULL, wzFirstItem, ((pwe && pwe->IsUTF8Supported()) ? WIREENC_USE_UTF8 : WIREENC_NONE), wFirstWireItem, ARRAYSIZE(wFirstWireItem))));
    hr = XboxItemID_CreateFake(wzFirstItem, wFirstWireItem, fIsCurrSegmentTypeKnown, !fIsCurrSegmentADir, FALSE, &pidl);
    ASSERT(IsValidPIDL(pidl));

    if (EVAL(SUCCEEDED(hr)))
    {
        if (wzRemaining[0])
        {
            Str_SetPtrW(ppwzRemaining, wzRemaining);
            *ppidlCurrentID = pidl;
        }
        else
            *ppidl = pidl;
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: CreateXboxPidlFromUrlPath

    DESCRIPTION:
        This function will be passed the 'Path' of the URL and will create
    each of the IDs for each path segment.  This will happen by creating an ID
    for the first path segment and then Combining that with the remaining
    IDs which are obtained by a recursive call.

    URL = "xbox://<UserName>:<Password>@<HostName>:<PortNum>/Dir1/Dir2/Dir3/file.txt[;Type=[a|b|d]]"
    Url Path = "Dir1/Dir2/Dir3/file.txt"

    pszFullPath - This URL will contain an URL Path (/Dir1/Dir2/MayBeFileOrDir).
    fIsTypeKnown - We can detect all directories w/o ambiguity because they end
                   end '/' except for the last directory.  fIsTypeKnown is used
                   if this information is known.  If TRUE, fIsDir will be used to
                   disambiguate the last item.  If FALSE, the last item will be marked
                   a directory if it doesn't have an extension.

    The incoming name is %-encoded, but if we see an illegal %-sequence,
    just leave the % alone.

    Note that we return E_FAIL when given an unparseable path,
    not E_INVALIDARG.
\*****************************************************************************/
HRESULT CreateXboxPidlFromDisplayPath(LPCWSTR pwzFullPath, CWireEncoding * pwe, ULONG *pcchEaten, LPITEMIDLIST * ppidl, BOOL fIsTypeKnown, BOOL fIsDir)
{
    HRESULT hr = E_FAIL;
    LPWSTR pwzRemaining = NULL;
    LPITEMIDLIST pidlCurrentID = NULL;

    hr = CreateXboxPidlFromDisplayPathHelper(pwzFullPath, pwe, pcchEaten, ppidl, fIsTypeKnown, fIsDir, &pidlCurrentID, &pwzRemaining);
    if (EVAL(SUCCEEDED(hr)) && pwzRemaining)
    {
        LPITEMIDLIST pidlSub;

        hr = CreateXboxPidlFromDisplayPath(pwzRemaining, pwe, pcchEaten, &pidlSub, fIsTypeKnown, fIsDir);
        if (EVAL(SUCCEEDED(hr)))
        {
            *ppidl = ILCombine(pidlCurrentID, pidlSub);
            hr = *ppidl ? S_OK : E_OUTOFMEMORY;
            ILFree(pidlSub);
        }

        ILFree(pidlCurrentID);
        Str_SetPtrW(&pwzRemaining, NULL);
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: CreateXboxPidlFromDisplayPathHelper

    DESCRIPTION:
        The work done in CreateXboxPidlFromUrlPath requires a fair amount of
    stack space so we do most of the work in CreateXboxPidlFromDisplayPathHelper
    to prevent overflowing the stack.
\*****************************************************************************/
HRESULT CreateXboxPidlFromXboxWirePathHelper(LPCWIRESTR pwXboxWirePath, CWireEncoding * pwe, ULONG *pcchEaten, LPITEMIDLIST * ppidl, BOOL fIsTypeKnown, BOOL fIsDir, LPITEMIDLIST * ppidlCurrentID, LPWIRESTR * ppwRemaining)
{
    HRESULT hr = E_FAIL;
    LPITEMIDLIST pidl;
    WIRECHAR wFirstItem[MAX_PATH];
    WCHAR wzFirstItemDisplayName[MAX_PATH];
    WIRECHAR wRemaining[MAX_PATH];
    BOOL fIsCurrSegmentADir = FALSE;
    BOOL fIsCurrSegmentTypeKnown = fIsTypeKnown;
    BOOL fIsFragSeparator = FALSE;

    *ppwRemaining = NULL;
    *ppidl = 0;

    if (pcchEaten)
        *pcchEaten = 0;     // The caller will parse the entire URL so we don't need to fill this in.

    if ('/' == pwXboxWirePath[0])
        pwXboxWirePath = (LPWIRESTR) CharNextA(pwXboxWirePath);

    WirePathGetFirstSegment(pwXboxWirePath, wFirstItem, ARRAYSIZE(wFirstItem), NULL, wRemaining, ARRAYSIZE(wRemaining), &fIsCurrSegmentADir);
    // Is this the last segment?
    if (!wRemaining[0])
    {
        // Yes, so if the caller knows the type of the last segment, use it now.
        if (fIsTypeKnown)
            fIsCurrSegmentADir = fIsDir;
    }
    else
    {
        // No, so we are assured that fIsDirCurrent is correct because it must have been followed
        // by a '/', or how could it be followed by another path segment?
        fIsCurrSegmentTypeKnown = TRUE;
        ASSERT(fIsCurrSegmentADir);
    }

    // NOTE: If the user entered "xbox://serv/Dir1/Dir2" fIsDir will be false for Dir2.
    //       It will be marked as ambigious. (TODO: Check for extension?)
    EVAL(SUCCEEDED(pwe->WireBytesToUnicode(NULL, wFirstItem, WIREENC_IMPROVE_ACCURACY, wzFirstItemDisplayName, ARRAYSIZE(wzFirstItemDisplayName))));
    hr = XboxItemID_CreateFake(wzFirstItemDisplayName, wFirstItem, fIsCurrSegmentTypeKnown, !fIsCurrSegmentADir, FALSE, &pidl);
    ASSERT(IsValidPIDL(pidl));

    if (EVAL(SUCCEEDED(hr)))
    {
        if (wRemaining[0])
        {
            Str_SetPtrA(ppwRemaining, wRemaining);
            *ppidlCurrentID = pidl;
        }
        else
            *ppidl = pidl;
    }

    return hr;
}


HRESULT CreateXboxPidlFromXboxWirePath(LPCWIRESTR pwXboxWirePath, CWireEncoding * pwe, ULONG *pcchEaten, LPITEMIDLIST * ppidl, BOOL fIsTypeKnown, BOOL fIsDir)
{
    HRESULT hr = E_FAIL;
    LPWIRESTR pwRemaining = NULL;
    LPITEMIDLIST pidlCurrentID = NULL;

    *ppidl = NULL;
    if (!pwXboxWirePath[0] || (0 == StrCmpA(pwXboxWirePath, SZ_URL_SLASHA)))
        return S_OK;

    hr = CreateXboxPidlFromXboxWirePathHelper(pwXboxWirePath, pwe, pcchEaten, ppidl, fIsTypeKnown, fIsDir, &pidlCurrentID, &pwRemaining);
    if (EVAL(SUCCEEDED(hr)) && pwRemaining)
    {
        LPITEMIDLIST pidlSub;

        hr = CreateXboxPidlFromXboxWirePath(pwRemaining, pwe, pcchEaten, &pidlSub, fIsTypeKnown, fIsDir);
        if (EVAL(SUCCEEDED(hr)))
        {
            *ppidl = ILCombine(pidlCurrentID, pidlSub);
            hr = *ppidl ? S_OK : E_OUTOFMEMORY;
            ILFree(pidlSub);
        }

        ILFree(pidlCurrentID);
        Str_SetPtrA(&pwRemaining, NULL);
    }

    return hr;
}


HRESULT CreateXboxPidlFromUrlPathAndPidl(LPCITEMIDLIST pidl, CWireEncoding * pwe, LPCWIRESTR pwXboxWirePath, LPITEMIDLIST * ppidl)
{
    HRESULT hr = E_FAIL;
    LPITEMIDLIST pidlNew = ILClone(pidl);

    if (pidlNew)
    {
        LPITEMIDLIST pidlLast = (LPITEMIDLIST) ILGetLastID(pidlNew);

        while (!XboxID_IsServerItemID(pidlLast))
        {
            pidlLast->mkid.cb = 0;  // Remove this ID.
            pidlLast = (LPITEMIDLIST) ILGetLastID(pidlNew);
        }

        LPITEMIDLIST pidlUrlPath = NULL;
        hr = CreateXboxPidlFromXboxWirePath(pwXboxWirePath, pwe, NULL, &pidlUrlPath, TRUE, TRUE);
        if (EVAL(SUCCEEDED(hr)))
        {
            *ppidl = ILCombine(pidlNew, pidlUrlPath);
        }

        if (pidlLast)
            ILFree(pidlLast);

        if (pidlUrlPath)
            ILFree(pidlUrlPath);
    }

    return hr;
}


/*****************************************************************************\
    CreateXboxPidlFromUrl

    The incoming name is %-encoded, but if we see an illegal %-sequence,
    just leave the % alone.

    Note that we return E_FAIL when given an unparseable path,
    not E_INVALIDARG.
\*****************************************************************************/
HRESULT CreateXboxPidlFromUrl(LPCTSTR pszUrl, CWireEncoding * pwe, ULONG *pcchEaten, LPITEMIDLIST * ppidl, IMalloc * pm, BOOL fHidePassword)
{
    return CreateXboxPidlFromUrlEx(pszUrl, pwe, pcchEaten, ppidl, pm, fHidePassword, FALSE, FALSE);
}


/*****************************************************************************\
    FUNCTION: CreateXboxPidlFromUrlEx

    DESCRIPTION:
    pszUrl - This URL will contain an URL Path (/Dir1/Dir2/MayBeFileOrDir).
    fIsTypeKnown - We can detect all directories w/o ambiguity because they end
                   end '/' except for the last directory.  fIsTypeKnown is used
                   if this information is known.  If TRUE, fIsDir will be used to
                   disambiguate the last item.  If FALSE, the last item will be marked
                   a directory if it doesn't have an extension.

    The incoming name is %-encoded, but if we see an illegal %-sequence,
    just leave the % alone.

    Note that we return E_FAIL when given an unparseable path,
    not E_INVALIDARG.
\*****************************************************************************/
HRESULT CreateXboxPidlFromUrlEx(LPCTSTR pszUrl, CWireEncoding * pwe, ULONG *pcchEaten, LPITEMIDLIST * ppidl, IMalloc * pm, BOOL fHidePassword, BOOL fIsTypeKnown, BOOL fIsDir)
{
    URL_COMPONENTS urlComps = {0};
    HRESULT hr = E_FAIL;

    // URL = "xbox://<UserName>:<Password>@<HostName>:<PortNum>/Dir1/Dir2/Dir3/file.txt[;Type=[a|b|d]]"
    TCHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH];
    TCHAR szUrlPath[MAX_URL_STRING];
    TCHAR szExtraInfo[MAX_PATH];    // Includes Port Number and download type (ASCII, Binary, Detect)
    TCHAR szUserName[INTERNET_MAX_USER_NAME_LENGTH];
    TCHAR szPassword[INTERNET_MAX_PASSWORD_LENGTH];

    *ppidl = 0;

    urlComps.dwStructSize = sizeof(urlComps);
    urlComps.lpszHostName = szServer;
    urlComps.dwHostNameLength = ARRAYSIZE(szServer);
    urlComps.lpszUrlPath = szUrlPath;
    urlComps.dwUrlPathLength = ARRAYSIZE(szUrlPath);

    urlComps.lpszUserName = szUserName;
    urlComps.dwUserNameLength = ARRAYSIZE(szUserName);
    urlComps.lpszPassword = szPassword;
    urlComps.dwPasswordLength = ARRAYSIZE(szPassword);
    urlComps.lpszExtraInfo = szExtraInfo;
    urlComps.dwExtraInfoLength = ARRAYSIZE(szExtraInfo);

    BOOL fResult = XboxCrackUrl(pszUrl, 0, ICU_DECODE, &urlComps);
    if (fResult && (INTERNET_SCHEME_FTP == urlComps.nScheme))
    {
        LPITEMIDLIST pidl;
        DWORD dwDownloadType = 0;   // Indicate that it hasn't yet been specified.
        BOOL fASCII;

        ASSERT(INTERNET_SCHEME_FTP == urlComps.nScheme);
        // NOTE:
        //          If the user is trying to give an NT UserName/DomainName pair, a bug will be encountered.
        //          Url in AddressBand="xbox://DomainName\UserName:Password@ServerName/"
        //          Url passed to us="xbox://DomainName/UserName:Password@ServerName/"
        //          We need to detect this case and fix it because this will cause "DomainName" to become
        //          the server name and the rest will become the UrlPath.
        // ASSERT(!StrChr(szUrlPath, TEXT(':')) && !StrChr(szUrlPath, TEXT('@')));

        if (S_OK == UrlRemoveDownloadType(szUrlPath, NULL, &fASCII))
        {
            if (fASCII)
                dwDownloadType = (IDVALID_DLTYPE | IDVALID_DL_ASCII);
            else
                dwDownloadType = IDVALID_DLTYPE;
        }

        if (!szServer[0])
        {
            TraceMsg(TF_XBOXURL_UTILS, "CreateXboxPidlFromUrl() failed because szServer=%s", szServer);
            hr = E_FAIL;    // Bad URL so fail.
        }
        else
        {
            //TraceMsg(TF_XBOXURL_UTILS, "CreateXboxPidlFromUrl() szServer=%s, szUrlPath=%s, szUserName=%s, szPassword=%s", szServer, szUrlPath, szUserName, szPassword);
            hr = XboxServerID_Create(szServer, szUserName, szPassword, dwDownloadType, urlComps.nPort, &pidl, pm, fHidePassword);
            if (EVAL(SUCCEEDED(hr)))
            {
                ASSERT(IsValidPIDL(pidl));
                if (szUrlPath[0] && StrCmp(szUrlPath, SZ_URL_SLASH))
                {
                    LPITEMIDLIST pidlSub;

                    hr = CreateXboxPidlFromDisplayPath(szUrlPath, pwe, pcchEaten, &pidlSub, fIsTypeKnown, fIsDir);
                    if (EVAL(SUCCEEDED(hr)))
                    {
                        *ppidl = ILCombine(pidl, pidlSub);
                        if (szExtraInfo[0])
                        {
                            LPITEMIDLIST pidlFragment;
                            WIRECHAR wFragment[MAX_PATH];

                            // The code page is just whatever the user is using but oh well, I don't 
                            // care about fragments.
                            SHUnicodeToAnsi(szExtraInfo, wFragment, ARRAYSIZE(wFragment));
                            // There is a fragment, so we need to add it.
                            hr = XboxItemID_CreateFake(szExtraInfo, wFragment, TRUE, FALSE, TRUE, &pidlFragment);
                            if (EVAL(SUCCEEDED(hr)))
                            {
                                LPITEMIDLIST pidlPrevious = *ppidl;

                                *ppidl = ILCombine(pidlPrevious, pidlFragment);
                                ILFree(pidlPrevious);
                                ILFree(pidlFragment);
                            }
                        }

                        hr = *ppidl ? S_OK : E_OUTOFMEMORY;
                        ILFree(pidlSub);
                    }
                    ILFree(pidl);
                }
                else
                    *ppidl = pidl;

                if (SUCCEEDED(hr))
                {
                    ASSERT(IsValidPIDL(*ppidl));
                    if (pcchEaten)
                        *pcchEaten = lstrlen(pszUrl);      // TODO: Someday we can do this recursively.
                }
            }
        }
    }
    else
        TraceMsg(TF_XBOXURL_UTILS, "CreateXboxPidlFromUrl() failed XboxCrackUrl() because pszUrl=%s, fResult=%d, urlComps.nScheme=%d", pszUrl, fResult, urlComps.nScheme);

    //TraceMsg(TF_XBOXURL_UTILS, "CreateXboxPidlFromUrl() is returning, hr=%#08lx", hr);
    return hr;
}


/*****************************************************************************\
     FUNCTION: Win32FindDataFromPidl
 
    DESCRIPTION:
        Fill in the WIN32_FIND_DATA data structure from the info in the pidl.
\*****************************************************************************/
HRESULT Win32FindDataFromPidl(LPCITEMIDLIST pidl, LPWIN32_FIND_DATAW pwfd, BOOL fFullPath, BOOL fInDisplayFormat)
{
    HRESULT hr = E_INVALIDARG;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    ASSERT(pwfd);
    if (!EVAL(XboxPidl_IsValid(pidl)))
        return E_INVALIDARG;

    // I don't want to lie when I pass out File Size and Date info.
    if ((IDVALID_FILESIZE | IDVALID_MOD_DATE) & XboxItemID_GetTypeID(pidlLast))
    {
        pwfd->nFileSizeLow = XboxItemID_GetFileSizeLo(pidlLast);
        pwfd->nFileSizeHigh = XboxItemID_GetFileSizeHi(pidlLast);
        pwfd->dwFileAttributes = XboxItemID_GetAttributes(pidlLast);

        // See the notes in priv.h on how time works.
        pwfd->ftCreationTime = XboxPidl_GetXBOXFileTime(pidlLast);
        pwfd->ftLastWriteTime = pwfd->ftCreationTime;
        pwfd->ftLastAccessTime = pwfd->ftCreationTime;

        if (fFullPath)
        {
            if (fInDisplayFormat)
                hr = GetDisplayPathFromPidl(pidl, pwfd->cFileName, ARRAYSIZE(pwfd->cFileName), FALSE);
            else
                hr = GetWirePathFromPidl(pidl, (LPWIRESTR)pwfd->cFileName, ARRAYSIZE(pwfd->cFileName), FALSE);
        }
        else
        { 
            hr = S_OK;
            if (fInDisplayFormat)
                StrCpyNW(pwfd->cFileName, XboxPidl_GetLastFileDisplayName(pidl), ARRAYSIZE(pwfd->cFileName));
            else
                StrCpyNA((LPWIRESTR)pwfd->cFileName, XboxPidl_GetLastItemWireName(pidl), ARRAYSIZE(pwfd->cFileName));
        }
    }

    return hr;
}






/****************************************************\
    XBOX Server ItemIDs
\****************************************************/

/****************************************************\
    XBOX PIDL Cooking functions
\****************************************************/

/*****************************************************************************\
    DATA STRUCTURE: XBOXIDLIST

    DESCRIPTION:
        What our private IDLIST looks like for a file, a dir, or a fragment.

    The bytes sent to an Xbox server or received from an XBOX server are
    wire bytes (could be UTF-8 or DBCS/MBCS) encoded.  We also store
    a unicode version that has already been converted after trying to guess
    the code page.

    Note that the use of any TCHAR inside an IDLIST is COMPLETELY WRONG!
    IDLISTs can be saved in a file and reloaded later.  If it were saved
    by an ANSI version of the shell extension but loaded by a UNICODE
    version, things would turn ugly real fast.
\*****************************************************************************/

/*****************************************************************************\
    XBOXSERVERIDLIST structure

    A typical full pidl looks like this:
    <Not Our ItemID> [Our ItemID]

    <The Internet>\[server,username,password,port#,downloadtype]\[subdir]\...\[file]

    The <The Internet> part is whatever the shell gives us in our
    CXboxFolder::_Initialize, telling us where in the namespace
    we are rooted.

    We are concerned only with the parts after the <The Internet> root,
    the offset to which is remembered in the CXboxFolder class
    in m_ibPidlRoot.  Ways of accessing various bits of
    information related to our full pidl are provided by our
    CXboxFolder implementation, qv.

    The first XBOX IDList entry describes the server.  The remaining
    entries describe objects (files or folders) on the server.
\*****************************************************************************/

typedef struct tagXBOXSERVERIDLIST
{
    DWORD dwIDType;                 // Server ItemID or Dir ItemID?  Which Bits are valid?
    DWORD dwVersion;                // version
    SESSIONKEY sessionKey;          // Session Key
    DWORD dwPasswordCookie;         // Password Cookie
    DWORD dwReserved1;              // for future use
    DWORD dwReserved2;              // for future use
    DWORD dwReserved3;              // for future use
    DWORD dwPortNumber;             // Port Number on server
    DWORD cchServerSize;            // StrLen of szServer
    CHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH];        // Server
    DWORD cchUserNameSize;          // StrLen of szUserName
    CHAR szUserName[INTERNET_MAX_USER_NAME_LENGTH];      // User Name for Login
    DWORD cchPasswordSize;          // StrLen of szPassword
    CHAR szPassword[INTERNET_MAX_PASSWORD_LENGTH];      // Password for Login
} XBOXSERVERIDLIST;

typedef UNALIGNED XBOXSERVERIDLIST * LPXBOXSERVERIDLIST;


LPXBOXSERVERIDLIST XboxServerID_GetData(LPCITEMIDLIST pidl)
{
    LPXBOXSERVERIDLIST pXboxServerItemId = (LPXBOXSERVERIDLIST) ProtocolIdlInnerData(pidl);

    if (!XboxPidl_IsValid(pidl) || 
        !IS_VALID_SERVER_ITEMID(pXboxServerItemId->dwIDType)) // If any other bits are sit, it's invalid.
        pXboxServerItemId = NULL;

    return pXboxServerItemId;
}


LPXBOXSERVERIDLIST XboxServerID_GetDataSafe(LPCITEMIDLIST pidl)
{
    LPXBOXSERVERIDLIST pXboxServerItemId = NULL;
    
    if (EVAL(pidl) && !ILIsEmpty(pidl))
        pXboxServerItemId = (LPXBOXSERVERIDLIST) ProtocolIdlInnerData(pidl);

    return pXboxServerItemId;
}


BOOL XboxID_IsServerItemID(LPCITEMIDLIST pidl)
{
    LPXBOXSERVERIDLIST pXboxServerItemID = XboxServerID_GetDataSafe(pidl);
    BOOL fIsServerItemID = FALSE;

    if (pXboxServerItemID && IS_VALID_SERVER_ITEMID(pXboxServerItemID->dwIDType))
        fIsServerItemID = TRUE;

    return fIsServerItemID;
}


LPCITEMIDLIST XboxID_GetLastIDReferense(LPCITEMIDLIST pidl)
{
    LPCITEMIDLIST pidlCurrent = pidl;
    LPCITEMIDLIST pidlNext = pidl;

    if (!pidl || ILIsEmpty(pidl))
        return pidl;

    for (; !ILIsEmpty(pidlNext); pidl = _ILNext(pidl))
    {
        pidlCurrent = pidlNext;
        pidlNext = _ILNext(pidlNext);
    }

    return pidlCurrent;
}


CCookieList * g_pCookieList = NULL;

CCookieList * GetCookieList(void)
{
    ENTERCRITICAL;
    if (!g_pCookieList)
        g_pCookieList = new CCookieList();
    ASSERT(g_pCookieList);
    LEAVECRITICAL;

    return g_pCookieList;
}

SESSIONKEY g_SessionKey = {-1, -1};

HRESULT PurgeSessionKey(void)
{
    GetSystemTimeAsFileTime(&g_SessionKey);

    return S_OK;
}

SESSIONKEY GetSessionKey(void)
{    
    if (-1 == g_SessionKey.dwHighDateTime)
        PurgeSessionKey();
    
    return g_SessionKey;
}

BOOL AreSessionKeysEqual(SESSIONKEY sk1, SESSIONKEY sk2)
{
    if ((sk1.dwHighDateTime == sk2.dwHighDateTime) &&
        (sk1.dwLowDateTime == sk2.dwLowDateTime))
    {
        return TRUE;
    }

    return FALSE;
}

// This is used in order to make sure Alpha machines don't get DWORD mis-aligned.
#define LENGTH_AFTER_ALIGN(nLen, nAlignSize)        (((nLen) % (nAlignSize)) ? ((nLen) + ((nAlignSize) - ((nLen) % (nAlignSize)))) : (nLen))

/****************************************************\
    FUNCTION: XboxServerID_Create

    DESCRIPTION:
        Create a Xbox Server ItemID and fill it in.
\****************************************************/
HRESULT XboxServerID_Create(LPCTSTR pszServer, LPCTSTR pszUserName, LPCTSTR pszPassword, 
                     DWORD dwFlags, INTERNET_PORT ipPortNum, LPITEMIDLIST * ppidl, IMalloc *pm, BOOL fHidePassword)
{
    HRESULT hr;
    DWORD cb;
    LPITEMIDLIST pidl;
    LPXBOXSERVERIDLIST pXboxServerID = NULL;
    DWORD cchServerLen = lstrlen(pszServer);
    DWORD cchUserNameLen = lstrlen(pszUserName);
    DWORD cchPasswordLen = lstrlen(pszPassword);

    cchServerLen = LENGTH_AFTER_ALIGN(cchServerLen + 1, sizeof(DWORD));
    cchUserNameLen = LENGTH_AFTER_ALIGN(cchUserNameLen + 1, sizeof(DWORD));
    cchPasswordLen = LENGTH_AFTER_ALIGN(cchPasswordLen + 1, sizeof(DWORD));

    if (!(EVAL(ppidl) && pszServer[0]))
        return E_FAIL;

    // Set bits in dwFlags that are appropriate
    if (pszUserName[0])
        dwFlags |= IDVALID_USERNAME;

    if (pszPassword[0])
        dwFlags |= IDVALID_PASSWORD;

    // Find lenght of XBOXSERVERIDLIST struct without the MAX_PATH strings
    cb = (sizeof(*pXboxServerID) - sizeof(pXboxServerID->szServer) - sizeof(pXboxServerID->szUserName) - sizeof(pXboxServerID->szPassword));

    // Add the size of the strings.
    cb += (cchServerLen + cchUserNameLen + cchPasswordLen);

    ASSERT(0 == (cb % sizeof(DWORD)));  // Make sure it's DWORD aligned for Alpha machines.

    pidl = (LPITEMIDLIST) pm->Alloc(cb);
    if (pidl)
    {
        LPSTR pszNext;

        pXboxServerID = XboxServerID_GetDataSafe(pidl);
        pszNext = pXboxServerID->szServer;

        ZeroMemory(pXboxServerID, cb);
        pXboxServerID->dwIDType = (dwFlags | IDTYPE_ISVALID | IDTYPE_SERVER | IDVALID_PORT_NUM);
        ASSERT(IS_VALID_SERVER_ITEMID(pXboxServerID->dwIDType));

        pXboxServerID->dwVersion = PIDL_VERSION_NUMBER;
        pXboxServerID->sessionKey = GetSessionKey();
        pXboxServerID->dwPasswordCookie = -1;
        pXboxServerID->dwPortNumber = ipPortNum;

        pXboxServerID->cchServerSize = cchServerLen;
        SHTCharToAnsi(pszServer, pszNext, pXboxServerID->cchServerSize);

        pszNext += cchServerLen; // Advance to cchUserNameSize
        *((LPDWORD) pszNext) = cchUserNameLen;  // Fill in cchUserNameSize
        pszNext = (LPSTR)(((BYTE *) pszNext) + sizeof(DWORD)); // Advance to szUserName
        SHTCharToAnsi(pszUserName, pszNext, cchUserNameLen);  // Fill in szUserName

        if (fHidePassword)
        {
            pXboxServerID->dwIDType |= IDVALID_HIDE_PASSWORD;
            if (EVAL(GetCookieList()))
                pXboxServerID->dwPasswordCookie = GetCookieList()->GetCookie(pszPassword);

            ASSERT(-1 != pXboxServerID->dwPasswordCookie);
            pszPassword = TEXT("");
        }

//        TraceMsg(TF_XBOXURL_UTILS, "XboxServerID_Create(\"xbox://%s:%s@%s/\") dwIDType=%#80lx", pszUserName, pszPassword, pszServer, pXboxServerID->dwIDType);
        pszNext += cchUserNameLen; // Advance to cchPasswordLen
        *((LPDWORD) pszNext) = cchPasswordLen;  // Fill in cchPasswordLen
        pszNext = (LPSTR)(((BYTE *) pszNext) + sizeof(DWORD)); // Advance to szPassword
        SHTCharToAnsi(pszPassword, pszNext, cchPasswordLen);  // Fill in pszPassword
    }

    *ppidl = pidl;
    hr = pidl ? S_OK : E_OUTOFMEMORY;
    ASSERT(IsValidPIDL(*ppidl));

    return hr;
}


DWORD XboxServerID_GetTypeID(LPCITEMIDLIST pidl)
{
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);

    ASSERT(XboxID_IsServerItemID(pidl));
    if (EVAL(pXboxServerID) && 
        EVAL(XboxPidl_IsValid(pidl)))
        return pXboxServerID->dwIDType;

    return 0;
}


HRESULT XboxServerID_GetServer(LPCITEMIDLIST pidl, LPTSTR pszServer, DWORD cchSize)
{
    HRESULT hr = S_OK;
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);

    if (pXboxServerID)
        SHAnsiToTChar(pXboxServerID->szServer, pszServer, cchSize);
    else
        hr = E_FAIL;

    return hr;
}


BOOL XboxServerID_ServerStrCmp(LPCITEMIDLIST pidl, LPCTSTR pszServer)
{
    BOOL fMatch = FALSE;
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);
#ifdef UNICODE
    CHAR szServerAnsi[MAX_PATH];

    SHUnicodeToAnsi(pszServer, szServerAnsi, ARRAYSIZE(szServerAnsi));
#endif // UNICODE

    if (pXboxServerID)
    {
#ifdef UNICODE
        fMatch = (0 == StrCmpA(pXboxServerID->szServer, szServerAnsi));
#else // UNICODE
        fMatch = (0 == StrCmpA(pXboxServerID->szServer, pszServer));
#endif // UNICODE
    }

    return fMatch;
}


HRESULT XboxServerID_GetUserName(LPCITEMIDLIST pidl, LPTSTR pszUserName, DWORD cchSize)
{
    HRESULT hr = E_FAIL;
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);

    if (EVAL(pXboxServerID))
    {
        LPCSTR pszSourceUserName = pXboxServerID->szServer + pXboxServerID->cchServerSize + sizeof(DWORD);

        SHAnsiToTChar(pszSourceUserName, pszUserName, cchSize);
        hr = S_OK;
    }

    return hr;
}

HRESULT XboxServerID_GetPassword(LPCITEMIDLIST pidl, LPTSTR pszPassword, DWORD cchSize, BOOL fIncludingHiddenPassword)
{
    HRESULT hr = E_FAIL;
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);

    pszPassword[0] = 0;
    if (EVAL(pXboxServerID))
    {
        // Was the password hidden?
        if (fIncludingHiddenPassword &&
            (IDVALID_HIDE_PASSWORD & pXboxServerID->dwIDType))
        {
            // Yes, so get it out of the cookie jar (list)
            if (EVAL(GetCookieList()) &&
                AreSessionKeysEqual(pXboxServerID->sessionKey, GetSessionKey()))
            {
                hr = GetCookieList()->GetString(pXboxServerID->dwPasswordCookie, pszPassword, cchSize);
            }
        }
        else
        {
            // No, so what's in the pidl is the real password.
            BYTE * pvSizeOfUserName = (BYTE *) (pXboxServerID->szServer + pXboxServerID->cchServerSize);
            DWORD dwSizeOfUserName = *(DWORD *) pvSizeOfUserName;
            LPCSTR pszSourcePassword = (LPCSTR) (pvSizeOfUserName + dwSizeOfUserName + 2*sizeof(DWORD));

            SHAnsiToTChar(pszSourcePassword, pszPassword, cchSize);
            hr = S_OK;
        }
    }

    return hr;
}

INTERNET_PORT XboxServerID_GetPortNum(LPCITEMIDLIST pidl)
{
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);

    ASSERT(XboxID_IsServerItemID(pidl));
    if (EVAL(pXboxServerID))
        return (INTERNET_PORT)pXboxServerID->dwPortNumber;

    return INTERNET_DEFAULT_FTP_PORT;
}


HRESULT XboxServerID_SetHiddenPassword(LPITEMIDLIST pidl, LPCTSTR pszPassword)
{
    HRESULT hr = E_INVALIDARG;
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);

    ASSERT(XboxID_IsServerItemID(pidl));
    if (EVAL(pXboxServerID))
    {
        pXboxServerID->sessionKey = GetSessionKey();
        pXboxServerID->dwIDType |= IDVALID_HIDE_PASSWORD;
        if (EVAL(GetCookieList()))
            pXboxServerID->dwPasswordCookie = GetCookieList()->GetCookie(pszPassword);
        hr = S_OK;
    }

    return hr;
}


HRESULT XboxServerID_GetStrRet(LPCITEMIDLIST pidl, LPSTRRET lpName)
{
    LPXBOXSERVERIDLIST pXboxServerID = XboxServerID_GetData(pidl);

    ASSERT(XboxID_IsServerItemID(pidl));
    if (pXboxServerID)
    {
        lpName->uType = STRRET_OFFSET;
        lpName->uOffset = (DWORD) (sizeof(XBOXSERVERIDLIST) - sizeof(pXboxServerID->szServer) + (LPBYTE)pXboxServerID - (LPBYTE)pidl);
    }
    else
    {
        lpName->uType = STRRET_CSTR;
        lpName->cStr[0] = '\0';
    }

    return S_OK;
}





/****************************************************\
    XBOX File/Dir ItemIDs
\****************************************************/

typedef struct tagXBOXIDLIST
{
    DWORD dwIDType;         // Server ItemID or Dir ItemID?  Which Bits are valid?
    DWORD dwAttributes;     // What are the file/dir attributes
    ULARGE_INTEGER uliFileSize;
    FILETIME ftModified;    // Stored in Local Time Zone. (XBOX Time)
    DWORD dwUNIXPermission; // UNIX CHMOD Permissions (0x00000777, 4=Read, 2=Write, 1=Exec, <Owner><Group><All>)
    DWORD dwReserved2;              // for future use
    WIRECHAR szWireName[MAX_PATH];          // Needs to go last.
    WCHAR wzDisplayName[MAX_PATH];  // Converted to unicode to be displayed in the UI.
} XBOXIDLIST;

typedef UNALIGNED XBOXIDLIST * LPXBOXIDLIST;

HRESULT XboxItemID_Alloc(LPXBOXIDLIST pfi, LPITEMIDLIST * ppidl);


typedef struct _XBOXIDLIST_WITHHEADER
{
    USHORT  cb;             // size
    XBOXIDLIST fidListData;
    USHORT  cbTerminator;   // size of next ID (Empty)
} XBOXIDLIST_WITHHEADER;



LPXBOXIDLIST XboxItemID_GetData(LPCITEMIDLIST pidl)
{
    BYTE * pbData = (BYTE *) pidl;

    pbData += SIZE_ITEMID_SIZEFIELD;      // Skip over the size.
    LPXBOXIDLIST pXboxItemId = (LPXBOXIDLIST) pbData;

    if (!EVAL(IS_VALID_DIRORFILE_ITEMID(pXboxItemId->dwIDType))) // If any other bits are sit, it's invalid.
        pXboxItemId = NULL;

    return pXboxItemId;
}

LPCWSTR XboxItemID_GetDisplayNameReference(LPCITEMIDLIST pidl)
{
    BYTE * pbData = (BYTE *) pidl;
    LPCWSTR pwzDisplayName = NULL;
    DWORD cbWireName;

    // Is the version OK?
//    if (PIDL_VERSION_NUMBER > XboxPidl_GetVersion(pidl))
//        return NULL;

    pbData += SIZE_ITEMID_SIZEFIELD;      // Skip over the size.
    LPXBOXIDLIST pXboxItemId = (LPXBOXIDLIST) pbData;

    cbWireName = LENGTH_AFTER_ALIGN((lstrlenA(pXboxItemId->szWireName) + 1), sizeof(DWORD));
    pwzDisplayName = (LPCWSTR) ((BYTE *)(&pXboxItemId->szWireName[0]) + cbWireName);

    if (!EVAL(IS_VALID_DIRORFILE_ITEMID(pXboxItemId->dwIDType))) // If any other bits are sit, it's invalid.
        pwzDisplayName = NULL;

    return pwzDisplayName;
}

DWORD XboxItemID_GetTypeID(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxItemId = XboxItemID_GetData(pidl);

    return (pXboxItemId ? pXboxItemId->dwIDType : 0);
}


void XboxItemID_SetTypeID(LPITEMIDLIST pidl, DWORD dwNewTypeID)
{
    LPXBOXIDLIST pXboxItemId = XboxItemID_GetData(pidl);
    ASSERT(pXboxItemId);
    pXboxItemId->dwIDType = dwNewTypeID;
}


/****************************************************\
    FUNCTION: XboxItemID_Alloc

    DESCRIPTION:
        We are passed a pointer to a XBOXIDLIST data
    structure and our goal is to create a ItemID from
    it.  This mainly includes making it only big enough
    for the current string(s).
\****************************************************/
HRESULT XboxItemID_Alloc(LPXBOXIDLIST pfi, LPITEMIDLIST * ppidl)
{
    HRESULT hr;
    WORD cbTotal;
    WORD cbDataFirst;
    WORD cbData;
    BYTE * pbMemory;
    DWORD cchSizeOfName = lstrlenA(pfi->szWireName);
    DWORD cchSizeOfDispName = lstrlenW(pfi->wzDisplayName);

    ASSERT(pfi && ppidl);

    // Find lenght of XBOXIDLIST struct if the szName member only needed enought room
    // for the string, not the full MAX_PATH.
    // Size EQUALS: (Everything in the struct) - (the 2 full statusly sized strings) + (the 2 packed strings + alignment)
    cbDataFirst = (WORD)((sizeof(*pfi) - sizeof(pfi->szWireName) - sizeof(pfi->wzDisplayName)) + LENGTH_AFTER_ALIGN(cchSizeOfName + 1, sizeof(DWORD)) - sizeof(DWORD));
    cbData = cbDataFirst + (WORD) LENGTH_AFTER_ALIGN((cchSizeOfDispName + 1) * sizeof(WCHAR), sizeof(DWORD));

    ASSERT((cbData % sizeof(DWORD)) == 0);  // Verify it's DWORD aligned.
    cbTotal = (SIZE_ITEMID_SIZEFIELD + cbData + SIZE_ITEMID_TERMINATOR);

    pbMemory = (BYTE *) CoTaskMemAlloc(cbTotal);
    if (EVAL(pbMemory))
    {
        USHORT * pIDSize = (USHORT *)pbMemory;
        BYTE * pbData = (pbMemory + SIZE_ITEMID_SIZEFIELD);        // the Data starts at the second DWORD.
        USHORT * pIDTerminator = (USHORT *)(pbMemory + SIZE_ITEMID_SIZEFIELD + cbData);

        pIDSize[0] = (cbTotal - SIZE_ITEMID_TERMINATOR);      // Set the size of the ItemID (including the next ItemID as terminator)
        ASSERT(cbData <= sizeof(*pfi)); // Don't let me copy too much.
        CopyMemory(pbData, pfi, cbDataFirst);
        CopyMemory((pbData + cbDataFirst), &(pfi->wzDisplayName), ((cchSizeOfDispName + 1) * sizeof(WCHAR)));
        pIDTerminator[0] = 0;  // Terminate the next ID.

//        TraceMsg(TF_XBOXURL_UTILS, "XboxItemID_Alloc(\"%ls\") dwIDType=%#08lx, dwAttributes=%#08lx", pfi->wzDisplayName, pfi->dwIDType, pfi->dwAttributes);
    }

    *ppidl = (LPITEMIDLIST) pbMemory;
    hr = pbMemory ? S_OK : E_OUTOFMEMORY;

    ASSERT(IsValidPIDL(*ppidl));
    ASSERT_POINTER_MATCHES_HRESULT(*ppidl, hr);

    return hr;
}


/*****************************************************************************\
    FUNCTION: XboxItemID_CreateReal

    DESCRIPTION:
        Cook up a pidl based on a WIN32_FIND_DATA.

    The cFileName field is itself MAX_PATH characters long,
    so its length cannot possibly exceed MAX_PATH...
\*****************************************************************************/
HRESULT XboxItemID_CreateReal(const LPXBOX_FIND_DATA pwfd, LPCWSTR pwzDisplayName, LPITEMIDLIST * ppidl)
{
    HRESULT hr;
    XBOXIDLIST fi = {0};

    // Fill in fi.
    fi.dwIDType = (IDTYPE_ISVALID | IDVALID_FILESIZE | IDVALID_MOD_DATE);
    fi.uliFileSize.LowPart = pwfd->nFileSizeLow;
    fi.uliFileSize.HighPart = pwfd->nFileSizeHigh;
    fi.ftModified = pwfd->ftLastWriteTime;
    fi.dwAttributes = pwfd->dwFileAttributes;
    fi.dwUNIXPermission = pwfd->dwReserved0;    // Set by WININET
    StrCpyNA(fi.szWireName, pwfd->cFileName, ARRAYSIZE(fi.szWireName));
    StrCpyN(fi.wzDisplayName, pwzDisplayName, ARRAYSIZE(fi.wzDisplayName));

    if (pwfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        fi.dwIDType |= IDTYPE_DIR;
    else
        fi.dwIDType |= IDTYPE_FILE;

    hr = XboxItemID_Alloc(&fi, ppidl);
    ASSERT(IsValidPIDL(*ppidl));

    return hr;
}


/****************************************************\
    FUNCTION: XboxItemID_CreateFake

    DESCRIPTION:
        Create a ItemID but we are only setting the
    name.  We don't know the true file attributes,
    file size, or modification date yet because
    we haven't touched the server yet.  If we did,
    we would use the returned WIN32_FIND_DATA struct
    to create the ItemID by using XboxItemID_CreateReal().
\****************************************************/
HRESULT XboxItemID_CreateFake(LPCWSTR pwzDisplayName, LPCWIRESTR pwWireName, BOOL fTypeKnown, BOOL fIsFile, BOOL fIsFragment, LPITEMIDLIST * ppidl)
{
    HRESULT hr;
    DWORD dwType = IDTYPE_ISVALID;
    XBOXIDLIST fi = {0};

    // Is it unknown?
    if (!fTypeKnown)
    {
        // HACK: We will assume everything w/o a file extension is a Dir
        //    and everything w/an extension is a file.
        fTypeKnown = TRUE;
        fIsFile = (0 != *PathFindExtension(pwzDisplayName)) ? TRUE : FALSE;
    }
    if (fTypeKnown)
    {
        if (fIsFile)
            dwType |= IDTYPE_FILE;
        else if (fIsFragment)
            dwType |= IDTYPE_FRAGMENT;
        else
            dwType |= IDTYPE_DIR;
    }
    else
    {
        // You need to know if it's a fragment because there is no
        // heuristic to find out.
        ASSERT(!fIsFragment);

        dwType |= IDTYPE_FILEORDIR;
    }

    fi.dwIDType = dwType;
    fi.dwAttributes = (fIsFile ? FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_DIRECTORY);
    fi.uliFileSize.QuadPart = 0;
    StrCpyNW(fi.wzDisplayName, pwzDisplayName, ARRAYSIZE(fi.wzDisplayName));
    StrCpyNA(fi.szWireName, pwWireName, ARRAYSIZE(fi.szWireName));

    hr = XboxItemID_Alloc(&fi, ppidl);
    ASSERT(IsValidPIDL(*ppidl));
    ASSERT_POINTER_MATCHES_HRESULT(*ppidl, hr);

    return hr;
}


/*****************************************************************************\
    FUNCTION: XboxItemID_SetName

    DESCRIPTION:
        The user chose a new name for the Xbox file or dir (in unicode).  We
    now need to create the name in wire bytes and we will use the original
    wire byte name to decide how to do that (from pidl).
\*****************************************************************************/
HRESULT XboxItemID_CreateWithNewName(LPCITEMIDLIST pidl, LPCWSTR pwzDisplayName, LPCWIRESTR pwWireName, LPITEMIDLIST * ppidlOut)
{
    HRESULT hr;
    XBOXIDLIST fi;
    const XBOXIDLIST * pfi = XboxItemID_GetData(pidl);
    CWireEncoding cWireEncoding;

    CopyMemory(&fi, pfi, sizeof(XBOXIDLIST) - sizeof(fi.szWireName) - sizeof(fi.wzDisplayName));
    StrCpyNW(fi.wzDisplayName, pwzDisplayName, ARRAYSIZE(fi.wzDisplayName));
    StrCpyNA(fi.szWireName, pwWireName, ARRAYSIZE(fi.szWireName));

    hr = XboxItemID_Alloc(&fi, ppidlOut);
    ASSERT(IsValidPIDL(*ppidlOut));

    return hr;
}


HRESULT Private_GetFileInfo(SHFILEINFO *psfi, DWORD rgf, LPCTSTR pszName, DWORD dwFileAttributes)
{
    HRESULT hr = E_FAIL;

    if (SHGetFileInfo(pszName, dwFileAttributes, psfi, sizeof(*psfi), rgf | SHGFI_USEFILEATTRIBUTES))
        hr = S_OK;

    return hr;
}


/*****************************************************************************\
    FUNCTION: XboxPidl_GetFileInfo

    DESCRIPTION:
        _UNDOCUMENTED_:  We strip the Hidden and System bits so
    that SHGetFileInfo won't think that we're passing something
    that might be a junction.

    We also force the SHGFI_USEFILEATTRIBUTES bit to remind the shell
    that this isn't a file.
\*****************************************************************************/
HRESULT XboxPidl_GetFileInfo(LPCITEMIDLIST pidl, SHFILEINFO *psfi, DWORD rgf)
{
    HRESULT hr;
    TCHAR szDisplayName[MAX_PATH];

    psfi->iIcon = 0;
    psfi->hIcon = NULL;
    psfi->dwAttributes = 0;
    psfi->szDisplayName[0] = 0;
    psfi->szTypeName[0] = 0;

    ASSERT(IsValidPIDL(pidl));
    if (XboxID_IsServerItemID(pidl))
    {
        XboxServerID_GetServer(pidl, szDisplayName, ARRAYSIZE(szDisplayName));
        hr = Private_GetFileInfo(psfi, rgf, szDisplayName, FILE_ATTRIBUTE_DIRECTORY);
        if (psfi->hIcon)
            DestroyIcon(psfi->hIcon);

        psfi->hIcon = LoadIcon(HINST_THISDLL, MAKEINTRESOURCE(IDI_XBOXFOLDER));
        ASSERT(psfi->hIcon);

        // Now replace the type (szTypeName) with "XBOX Server" because
        // it could go in the Properties dialog
        EVAL(LoadString(HINST_THISDLL, IDS_ITEMTYPE_SERVER, psfi->szTypeName, ARRAYSIZE(psfi->szTypeName)));
    }
    else
    {
        LPXBOXIDLIST pfi = XboxItemID_GetData(pidl);
        XboxItemID_GetDisplayName(pidl, szDisplayName, ARRAYSIZE(szDisplayName));
        hr = Private_GetFileInfo(psfi, rgf, szDisplayName, (pfi->dwAttributes & ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)));
    }

    return hr;
}

HRESULT XboxPidl_GetFileType(LPCITEMIDLIST pidl, LPTSTR pszType, DWORD cchSize)
{
    SHFILEINFO sfi;
    HRESULT hr;

    ASSERT(IsValidPIDL(pidl));
    hr = XboxPidl_GetFileInfo(pidl, &sfi, SHGFI_TYPENAME);
    if (EVAL(SUCCEEDED(hr)))
    {
        StrCpyN(pszType, sfi.szTypeName, cchSize);
        if (sfi.hIcon)
            DestroyIcon(sfi.hIcon);
    }

    return hr;
}


HRESULT XboxPidl_GetFileTypeStrRet(LPCITEMIDLIST pidl, LPSTRRET pstr)
{
    WCHAR szType[MAX_URL_STRING];
    HRESULT hr;

    ASSERT(IsValidPIDL(pidl));
    hr = XboxPidl_GetFileType(pidl, szType, ARRAYSIZE(szType));
    if (EVAL(SUCCEEDED(hr)))
        StringToStrRetW(szType, pstr);

    return hr;
}


/*****************************************************************************\
    FUNCTION: _XboxItemID_CompareOneID

    DESCRIPTION:
        ici - attribute (column) to compare

    Note! that UNIX filenames are case-*sensitive*.

    We make two passes on the name.  If the names are different in other
    than case, we return the result of that comparison.  Otherwise,
    we return the result of a case-sensitive comparison.

    This algorithm ensures that the items sort themselves in a
    case-insensitive way, with ties broken by a case-sensitive
    comparison.  This makes Xbox folders act "mostly" like normal
    folders.

    _UNDOCUMENTED_: The documentation says that the ici parameter
    is undefined and must be zero.  In reality, it is the column
    number (defined by IShellView) for which the comparison is to
    be made.
\*****************************************************************************/
HRESULT _XboxItemID_CompareOneID(LPARAM ici, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, DWORD dwCompFlags)
{
    int iRc = 0;    // 0 means we don't know.
    HRESULT hr = S_OK;

    ASSERT(IsValidPIDL(pidl1));
    ASSERT(IsValidPIDL(pidl2));

    // Are they both the same type? (Both Dirs or both files?)
    if (!(dwCompFlags & FCMP_GROUPDIRS) || (!XboxPidl_IsDirectory(pidl1, FALSE) == !XboxPidl_IsDirectory(pidl2, FALSE)))
    {
        switch (ici & SHCIDS_COLUMNMASK)
        {
        case COL_NAME:
        {
            // Yes they are the same, so we will key off the name...
            WIRECHAR szName1[MAX_PATH];
            WIRECHAR szName2[MAX_PATH];

            szName1[0] = TEXT('\0');
            szName2[0] = TEXT('\0');

            XboxPidl_GetWireName(pidl1, szName1, ARRAYSIZE(szName1));
            XboxPidl_GetWireName(pidl2, szName2, ARRAYSIZE(szName2));

            iRc = StrCmpIA(szName1, szName2);
            if (0 == iRc)
            {
                if (!(dwCompFlags & FCMP_CASEINSENSE))
                    iRc = StrCmpA(szName1, szName2);

/*
                // They are the same name, so now lets check on the username
                // if they are Server IDs.
                if ((0 == iRc) && (XboxID_IsServerItemID(pidl1)))
                {
                    XboxPidl_GetUserName(pidl1, szName1, ARRAYSIZE(szName1));
                    XboxPidl_GetUserName(pidl2, szName2, ARRAYSIZE(szName2));
                    iRc = StrCmp(szName1, szName2);
                }
*/
            }
        }
        break;

        case COL_SIZE:
            if (XboxPidl_GetFileSize(pidl1) < XboxPidl_GetFileSize(pidl2))
                iRc = -1;
            else if (XboxPidl_GetFileSize(pidl1) > XboxPidl_GetFileSize(pidl2))
                iRc = +1;
            else
                iRc = 0;        // I don't know
            break;

        case COL_TYPE:
            if (!XboxID_IsServerItemID(pidl1) && !XboxID_IsServerItemID(pidl2))
            {
                TCHAR szType1[MAX_PATH];

                hr = XboxPidl_GetFileType(pidl1, szType1, ARRAYSIZE(szType1));
                if (EVAL(SUCCEEDED(hr)))
                {
                    TCHAR szType2[MAX_PATH];
                    hr = XboxPidl_GetFileType(pidl2, szType2, ARRAYSIZE(szType2));
                    if (EVAL(SUCCEEDED(hr)))
                        iRc = StrCmpI(szType1, szType2);
                }
            }
            break;

        case COL_MODIFIED:
        {
            FILETIME ft1 = XboxPidl_GetFileTime(pidl1);
            FILETIME ft2 = XboxPidl_GetFileTime(pidl2);
            iRc = CompareFileTime(&ft1, &ft2);
        }
            break;

        default:
            hr = E_NOTIMPL;
            break;
        }
    }
    else
    {
        // No they are different.  We want the Folder to always come first.
        // This doesn't seam right, but it forces folders to bubble to the top
        // in the most frequent case and it matches DefView's Behavior.
        if (XboxPidl_IsDirectory(pidl1, FALSE))
            iRc = -1;
        else
            iRc = 1;
    }

    if (EVAL(S_OK == hr))
        hr = HRESULT_FROM_SUCCESS_VALUE(iRc);   // encode the sort value in the return code.

    return hr;
}


/*****************************************************************************\
    FUNCTION: XboxItemID_CompareIDs
    
    DESCRIPTION:
        ici - attribute (column) to compare

    Note! that we rely on the fact that IShellFolders are
    uniform; we do not need to bind to the shell folder in
    order to compare its sub-itemids.

    _UNDOCUMENTED_: The documentation does not say whether or not
    complex pidls can be received.  In fact, they can.

    The reason why the shell asks you to handle complex pidls
    is that you can often short-circuit the comparison by walking
    the ID list directly.  (Formally speaking, you need to bind
    to each ID and then call yourself recursively.  But if your
    pidls are uniform, you can just use a loop like the one below.)
\*****************************************************************************/
HRESULT XboxItemID_CompareIDs(LPARAM ici, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, DWORD dwCompFlags)
{
    HRESULT hr;

    if (!pidl1 || ILIsEmpty(pidl1))
    {
        if (!pidl2 || ILIsEmpty(pidl2))
            hr = HRESULT_FROM_SUCCESS_VALUE(0);        // Both ID lists are empty
        else
            hr = HRESULT_FROM_SUCCESS_VALUE(-1);        // pidl1 is empty, pidl2 is nonempty
    }
    else
    {
        if (!pidl2 || ILIsEmpty(pidl2))
            hr = HRESULT_FROM_SUCCESS_VALUE(1);     // pidl1 is nonempty, pidl2 is empty
        else
        {
            ASSERT(IsValidPIDL(pidl1));
            ASSERT(IsValidPIDL(pidl2));
            hr = _XboxItemID_CompareOneID(ici, pidl1, pidl2, dwCompFlags);    // both are nonempty
        }
    }

    // If this level of ItemsIDs are equal, then we will compare the next
    // level of ItemIDs
    if ((hr == HRESULT_FROM_SUCCESS_VALUE(0)) && pidl1 && !ILIsEmpty(pidl1))
        hr = XboxItemID_CompareIDs(ici, _ILNext(pidl1), _ILNext(pidl2), dwCompFlags);

    return hr;
}


int XboxItemID_CompareIDsInt(LPARAM ici, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, DWORD dwCompFlags)
{
    HRESULT hr = XboxItemID_CompareIDs(ici, pidl1, pidl2, dwCompFlags);
    int nResult = (DWORD)(short)hr;

    return nResult;
}

DWORD XboxItemID_GetAttributes(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return NULL;

    return pXboxIDList->dwAttributes;
}

HRESULT XboxItemID_SetAttributes(LPCITEMIDLIST pidl, DWORD dwAttribs)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return E_INVALIDARG;

    pXboxIDList->dwAttributes = dwAttribs;
    return S_OK;
}


DWORD XboxItemID_GetUNIXPermissions(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return NULL;

    return pXboxIDList->dwUNIXPermission;
}


HRESULT XboxItemID_SetUNIXPermissions(LPCITEMIDLIST pidl, DWORD dwUNIXPermission)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return E_INVALIDARG;

    pXboxIDList->dwUNIXPermission = dwUNIXPermission;
    return S_OK;
}


ULONGLONG XboxItemID_GetFileSize(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return NULL;


    ASSERT(IsFlagSet(pXboxIDList->dwIDType, IDVALID_FILESIZE));
    return pXboxIDList->uliFileSize.QuadPart;
}

void XboxItemID_SetFileSize(LPCITEMIDLIST pidl, ULARGE_INTEGER uliFileSize)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return;

    pXboxIDList->uliFileSize = uliFileSize;
    pXboxIDList->dwIDType |= IDVALID_FILESIZE;
}

DWORD XboxItemID_GetFileSizeLo(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return NULL;

    ASSERT(IsFlagSet(pXboxIDList->dwIDType, IDVALID_FILESIZE));
    return pXboxIDList->uliFileSize.LowPart;
}

DWORD XboxItemID_GetFileSizeHi(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return NULL;

    ASSERT(IsFlagSet(pXboxIDList->dwIDType, IDVALID_FILESIZE));
    return pXboxIDList->uliFileSize.HighPart;
}


// Return value is in Local Time Zone.
FILETIME XboxItemID_GetFileTime(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
    {
        FILETIME ftEmpty = {0};
        return ftEmpty;
    }

    ASSERT(IsFlagSet(pXboxIDList->dwIDType, IDVALID_MOD_DATE));
    return pXboxIDList->ftModified;
}


LPCWIRESTR XboxItemID_GetWireNameReference(LPCITEMIDLIST pidl)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList) || IS_FRAGMENT(pXboxIDList))
        return NULL;

    return pXboxIDList->szWireName;
}


HRESULT XboxItemID_GetDisplayName(LPCITEMIDLIST pidl, LPWSTR pwzName, DWORD cchSize)
{
    HRESULT hr = S_OK;
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (pXboxIDList && !IS_FRAGMENT(pXboxIDList))
    {
        // The display name wasn't stored in v3
        StrCpyN(pwzName, XboxItemID_GetDisplayNameReference(pidl), cchSize);
    }
    else 
    {
        pwzName[0] = TEXT('\0');
        hr = E_FAIL;
    }

    return hr;
}


HRESULT XboxItemID_GetWireName(LPCITEMIDLIST pidl, LPWIRESTR pszName, DWORD cchSize)
{
    HRESULT hr = S_OK;
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (pXboxIDList && !IS_FRAGMENT(pXboxIDList))
        StrCpyNA(pszName, pXboxIDList->szWireName, cchSize);
    else 
    {
        pszName[0] = TEXT('\0');
        hr = E_FAIL;
    }

    return hr;
}


HRESULT XboxItemID_GetFragment(LPCITEMIDLIST pidl, LPWSTR pwzFragmentStr, DWORD cchSize)
{
    HRESULT hr = S_OK;
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (pXboxIDList && IS_FRAGMENT(pXboxIDList))
        StrCpyNW(pwzFragmentStr, XboxItemID_GetDisplayNameReference(pidl), cchSize);
    else 
    {
        pwzFragmentStr[0] = TEXT('\0');
        hr = E_FAIL;
    }

    return hr;
}


BOOL XboxItemID_IsFragment(LPCITEMIDLIST pidl)
{
    BOOL fIsFrag = FALSE;
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (pXboxIDList && IS_FRAGMENT(pXboxIDList))
        fIsFrag = TRUE;

    return fIsFrag;
}


// fileTime In UTC
void XboxItemID_SetFileTime(LPCITEMIDLIST pidl, FILETIME fileTime)
{
    FILETIME fileTimeLocal;
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return;

    FileTimeToLocalFileTime(&fileTime, &fileTimeLocal);
    pXboxIDList->ftModified = fileTimeLocal;
    pXboxIDList->dwIDType |= IDVALID_MOD_DATE;
}

BOOL XboxItemID_IsDirectory(LPCITEMIDLIST pidl, BOOL fAssumeDirForUnknown)
{
    LPXBOXIDLIST pXboxIDList = XboxItemID_GetData(pidl);

    if (!EVAL(pXboxIDList))
        return NULL;

    BOOL fIsDir = (IsFlagSet(pXboxIDList->dwIDType, IDTYPE_DIR));
    
    if (fAssumeDirForUnknown && (IDTYPE_FILEORDIR == pXboxIDList->dwIDType))
    {
//        TraceMsg(TF_XBOXURL_UTILS, "XboxItemID_IsDirectory() IDTYPE_FILEORDIR is set, so we assume %s", (fAssumeDirForUnknown ? TEXT("DIR") : TEXT("FILE")));
        fIsDir = TRUE;
    }
    else
    {
//        TraceMsg(TF_XBOXURL_UTILS, "XboxItemID_IsDirectory() It is known to be a %s", (fIsDir ? TEXT("DIR") : TEXT("FILE")));
    }

    return fIsDir;
}




/****************************************************\
    Functions to work on an entire XBOX PIDLs
\****************************************************/
#define SZ_ASCII_DOWNLOAD_TYPE       TEXT("a")
#define SZ_BINARY_DOWNLOAD_TYPE      TEXT("b")

HRESULT XboxPidl_GetDownloadTypeStr(LPCITEMIDLIST pidl, LPTSTR szDownloadType, DWORD cchTypeStrSize)
{
    HRESULT hr = S_FALSE;   // We may not have a type.
    DWORD dwTypeID = XboxServerID_GetTypeID(pidl);

    szDownloadType[0] = TEXT('\0');
    if (IDVALID_DLTYPE & dwTypeID)
    {
        hr = S_OK;
        StrCpyN(szDownloadType, SZ_XBOX_URL_TYPE, cchTypeStrSize);

        if (IDVALID_DL_ASCII & dwTypeID)
            StrCatBuff(szDownloadType, SZ_ASCII_DOWNLOAD_TYPE, cchTypeStrSize);
        else
            StrCatBuff(szDownloadType, SZ_BINARY_DOWNLOAD_TYPE, cchTypeStrSize);
    }

    return hr;
}

DWORD XboxPidl_GetDownloadType(LPCITEMIDLIST pidl)
{
    DWORD dwAttribs = FTP_TRANSFER_TYPE_UNKNOWN;
    DWORD dwTypeID = XboxServerID_GetTypeID(pidl);

    ASSERT(XboxID_IsServerItemID(pidl));
    if (IDVALID_DLTYPE & dwTypeID)
    {
        if (IDVALID_DL_ASCII & dwTypeID)
            dwAttribs = FTP_TRANSFER_TYPE_ASCII;
        else
            dwAttribs = FTP_TRANSFER_TYPE_BINARY;
    }

    return dwAttribs;
}


INTERNET_PORT XboxPidl_GetPortNum(LPCITEMIDLIST pidl)
{
    ASSERT(XboxID_IsServerItemID(pidl));

    return XboxServerID_GetPortNum(pidl);
}


BOOL XboxPidl_IsDirectory(LPCITEMIDLIST pidl, BOOL fAssumeDirForUnknown)
{
    BOOL fIsDir = FALSE;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (!XboxID_IsServerItemID(pidlLast))
        fIsDir = XboxItemID_IsDirectory(pidlLast, fAssumeDirForUnknown);

    return fIsDir;
}


BOOL XboxPidl_IsAnonymous(LPCITEMIDLIST pidl)
{
    BOOL fIsAnonymous = TRUE;

    if (IDVALID_USERNAME & XboxServerID_GetTypeID(pidl))
        fIsAnonymous = FALSE;

    return fIsAnonymous;
}


HRESULT XboxPidl_GetServer(LPCITEMIDLIST pidl, LPTSTR pszServer, DWORD cchSize)
{
    if (!XboxID_IsServerItemID(pidl)) // Will fail if we are handed a non-server ID.
        return E_FAIL;

    return XboxServerID_GetServer(pidl, pszServer, cchSize);
}


BOOL XboxPidl_IsDNSServerName(LPCITEMIDLIST pidl)
{
    BOOL fIsDNSServer = FALSE;
    TCHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH];

    if (EVAL(SUCCEEDED(XboxPidl_GetServer(pidl, szServer, ARRAYSIZE(szServer)))))
        fIsDNSServer = !IsIPAddressStr(szServer);

    return fIsDNSServer;
}


HRESULT XboxPidl_GetUserName(LPCITEMIDLIST pidl, LPTSTR pszUserName, DWORD cchSize)
{
    ASSERT(XboxID_IsServerItemID(pidl));
    return XboxServerID_GetUserName(pidl, pszUserName, cchSize);
}

HRESULT XboxPidl_GetPassword(LPCITEMIDLIST pidl, LPTSTR pszPassword, DWORD cchSize, BOOL fIncludingHiddenPassword)
{
    ASSERT(XboxID_IsServerItemID(pidl));
    return XboxServerID_GetPassword(pidl, pszPassword, cchSize, fIncludingHiddenPassword);
}


ULONGLONG XboxPidl_GetFileSize(LPCITEMIDLIST pidl)
{
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);
    ULONGLONG ullFileSize;
    ullFileSize = 0;

    if (!XboxID_IsServerItemID(pidlLast))
        ullFileSize = XboxItemID_GetFileSize(pidlLast);

    return ullFileSize;
}


HRESULT XboxPidl_SetFileSize(LPCITEMIDLIST pidl, DWORD dwSizeHigh, DWORD dwSizeLow)
{
    HRESULT hr = E_INVALIDARG;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (!XboxID_IsServerItemID(pidlLast))
    {
        ULARGE_INTEGER uliFileSize;

        uliFileSize.HighPart = dwSizeHigh;
        uliFileSize.LowPart = dwSizeLow;
        XboxItemID_SetFileSize(pidlLast, uliFileSize);
        hr = S_OK;
    }

    return hr;
}

// Return value in UTC time.
FILETIME XboxPidl_GetFileTime(LPCITEMIDLIST pidl)
{
    FILETIME fileTimeXBOX = XboxPidl_GetXBOXFileTime(pidl);   // This is what servers will be.
    FILETIME fileTime;

    EVAL(LocalFileTimeToFileTime(&fileTimeXBOX, &fileTime));

    return fileTime;
}


// Return value is in Local Time Zone.
FILETIME XboxPidl_GetXBOXFileTime(LPCITEMIDLIST pidl)
{
    FILETIME fileTime = {0};   // This is what servers will be.

    if (!XboxID_IsServerItemID(pidl))
        fileTime = XboxItemID_GetFileTime(pidl);

    return fileTime;
}


HRESULT XboxPidl_GetDisplayName(LPCITEMIDLIST pidl, LPWSTR pwzName, DWORD cchSize)
{
    HRESULT hr = E_INVALIDARG;

    if (pidl)
    {
        if (XboxID_IsServerItemID(pidl))
            hr = XboxServerID_GetServer(pidl, pwzName, cchSize);
        else
            hr = XboxItemID_GetDisplayName(pidl, pwzName, cchSize);
    }

    return hr;
}


HRESULT XboxPidl_GetWireName(LPCITEMIDLIST pidl, LPWIRESTR pwName, DWORD cchSize)
{
    HRESULT hr = E_INVALIDARG;

    if (pidl)
    {
        if (XboxID_IsServerItemID(pidl))
        {
            WCHAR wzServerName[INTERNET_MAX_HOST_NAME_LENGTH];

            // It's a good thing Server Names need to be in US ANSI
            hr = XboxServerID_GetServer(pidl, wzServerName, ARRAYSIZE(wzServerName));
            SHUnicodeToAnsi(wzServerName, pwName, cchSize);
        }
        else
            hr = XboxItemID_GetWireName(pidl, pwName, cchSize);
    }

    return hr;
}


HRESULT XboxPidl_GetFragment(LPCITEMIDLIST pidl, LPTSTR pszFragment, DWORD cchSize)
{
    HRESULT hr = E_INVALIDARG;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (!XboxID_IsServerItemID(pidlLast))
        hr = XboxItemID_GetFragment(pidlLast, pszFragment, cchSize);
    else
    {
        pszFragment[0] = 0;
    }

    return hr;
}


DWORD XboxPidl_GetAttributes(LPCITEMIDLIST pidl)
{
    DWORD dwAttribs = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_TEMPORARY;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (!XboxID_IsServerItemID(pidlLast))
        dwAttribs = XboxItemID_GetAttributes(pidlLast);
    else
        dwAttribs = FILE_ATTRIBUTE_DIRECTORY;

    return dwAttribs;
}


HRESULT XboxPidl_SetAttributes(LPCITEMIDLIST pidl, DWORD dwAttribs)
{
    HRESULT hr = E_INVALIDARG;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (!XboxID_IsServerItemID(pidlLast))
        hr = XboxItemID_SetAttributes(pidlLast, dwAttribs);

    return hr;
}


// ftTimeDate In UTC
HRESULT XboxPidl_SetFileTime(LPCITEMIDLIST pidl, FILETIME ftTimeDate)
{
    HRESULT hr = E_INVALIDARG;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (!XboxID_IsServerItemID(pidlLast))
    {
        XboxItemID_SetFileTime(pidlLast, ftTimeDate);
        hr = S_OK;
    }

    return hr;
}


/****************************************************\
    FUNCTION: XboxPidl_GetFileWireName

    DESCRIPTION:
        Get the file name.
\****************************************************/
LPCWIRESTR XboxPidl_GetFileWireName(LPCITEMIDLIST pidl)
{
    if (EVAL(!XboxID_IsServerItemID(pidl)) &&
        !XboxItemID_IsFragment(pidl))
    {
        return XboxItemID_GetWireNameReference(pidl);
    }

    return NULL;
}


/****************************************************\
    FUNCTION: XboxPidl_GetFileDisplayName

    DESCRIPTION:
        Get the file name.
\****************************************************/
LPCWSTR XboxPidl_GetFileDisplayName(LPCITEMIDLIST pidl)
{
    if (EVAL(!XboxID_IsServerItemID(pidl)) &&
        !XboxItemID_IsFragment(pidl))
    {
        return XboxItemID_GetDisplayNameReference(pidl);
    }

    return NULL;
}


/****************************************************\
    FUNCTION: XboxPidl_GetLastItemDisplayName

    DESCRIPTION:
        This will get the last item name, even if that
    last item is a fragment.
\****************************************************/
LPCWSTR XboxPidl_GetLastItemDisplayName(LPCITEMIDLIST pidl)
{
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (EVAL(!XboxID_IsServerItemID(pidlLast)))
        return XboxItemID_GetDisplayNameReference(pidlLast);

    return NULL;
}


/****************************************************\
    FUNCTION: XboxPidl_GetLastItemWireName

    DESCRIPTION:
        This will get the last item name, even if that
    last item is a fragment.
\****************************************************/
LPCWIRESTR XboxPidl_GetLastItemWireName(LPCITEMIDLIST pidl)
{
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (XboxItemID_IsFragment(pidlLast) && (pidlLast != pidl))
    {
        // Oops, we went to far.  Step back one.
        LPCITEMIDLIST pidlFrag = pidlLast;

        pidlLast = pidl;    // Start back at the beginning.
        while (!XboxItemID_IsEqual(_ILNext(pidlLast), pidlFrag))
        {
            if (ILIsEmpty(pidlLast))
                return NULL;    // Break infinite loop.

            pidlLast = _ILNext(pidlLast);
        }
    }

    return XboxPidl_GetFileWireName(pidlLast);
}


/****************************************************\
    FUNCTION: XboxPidl_GetLastFileDisplayName

    DESCRIPTION:
        This will get the last item name, even if that
    last item is a fragment.
\****************************************************/
LPCWSTR XboxPidl_GetLastFileDisplayName(LPCITEMIDLIST pidl)
{
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    if (XboxItemID_IsFragment(pidlLast) && (pidlLast != pidl))
    {
        // Oops, we went to far.  Step back one.
        LPCITEMIDLIST pidlFrag = pidlLast;

        pidlLast = pidl;    // Start back at the beginning.
        while (!XboxItemID_IsEqual(_ILNext(pidlLast), pidlFrag))
        {
            if (ILIsEmpty(pidlLast))
                return NULL;    // Break infinite loop.

            pidlLast = _ILNext(pidlLast);
        }
    }

    if (EVAL(!XboxID_IsServerItemID(pidlLast)))
        return XboxItemID_GetDisplayNameReference(pidlLast);

    return NULL;
}


/****************************************************\
    FUNCTION: XboxPidl_InsertVirtualRoot

    DESCRIPTION:
        This function will insert the virtual root path
    (pidlVirtualRoot) into pidlXboxPath.

    PARAMETERS:
        pidlVirtualRoot: Does not have a ServerID
        pidlXboxPath: Can have a ServerID
        *ppidl: Will be pidlVirtualRoot with item ItemIDs from
            pidlXboxPath behind it. (No ServerID)
\****************************************************/
HRESULT XboxPidl_InsertVirtualRoot(LPCITEMIDLIST pidlVirtualRoot, LPCITEMIDLIST pidlXboxPath, LPITEMIDLIST * ppidl)
{
    HRESULT hr = E_OUTOFMEMORY;

    if (XboxID_IsServerItemID(pidlXboxPath))
        pidlXboxPath = _ILNext(pidlXboxPath);

    *ppidl = ILCombine(pidlVirtualRoot, pidlXboxPath);
    if (*ppidl)
        hr = S_OK;

    return hr;
}


DWORD XboxPidl_GetVersion(LPCITEMIDLIST pidl)
{
    if (!EVAL(XboxID_IsServerItemID(pidl)))
        return 0;

    LPXBOXSERVERIDLIST pXboxServerItemId = (LPXBOXSERVERIDLIST) ProtocolIdlInnerData(pidl);
    return pXboxServerItemId->dwVersion;
}


BOOL XboxPidl_IsValid(LPCITEMIDLIST pidl)
{
    if (!EVAL(IsValidPIDL(pidl)))
        return FALSE;

    return TRUE;
}


BOOL XboxPidl_IsValidFull(LPCITEMIDLIST pidl)
{
    if (!EVAL(XboxID_IsServerItemID(pidl)))
        return FALSE;

    if (!EVAL(XboxPidl_IsValid(pidl)))
        return FALSE;

    // We consider anything older than PIDL_VERSION_NUMBER_UPGRADE
    // to be invalid.
    return ((PIDL_VERSION_NUMBER_UPGRADE - 1) < XboxPidl_GetVersion(pidl));
}


BOOL XboxPidl_IsValidRelative(LPCITEMIDLIST pidl)
{
    if (!EVAL(!XboxID_IsServerItemID(pidl)))
        return FALSE;       // This is a server item id which is not relative.

    return XboxPidl_IsValid(pidl);
}


LPITEMIDLIST ILCloneFirstItemID(LPITEMIDLIST pidl)
{
    LPITEMIDLIST pidlCopy = ILClone(pidl);

    if (pidlCopy && pidlCopy->mkid.cb)
    {
        LPITEMIDLIST pSecondID = (LPITEMIDLIST)_ILNext(pidlCopy);

        ASSERT(pSecondID);
        // Remove the last one
        pSecondID->mkid.cb = 0; // null-terminator
    }

    return pidlCopy;
}

BOOL XboxPidl_HasPath(LPCITEMIDLIST pidl)
{
    BOOL fResult = TRUE;
    
    if (!XboxPidl_IsValid(pidl) || !EVAL(XboxID_IsServerItemID(pidl)))
        return FALSE;

    if (!ILIsEmpty(pidl) && ILIsEmpty(_ILNext(pidl)))
        fResult = FALSE;

    return fResult;
}


HRESULT XboxPidl_SetFileItemType(LPITEMIDLIST pidl, BOOL fIsDir)
{
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);
    HRESULT hr = E_INVALIDARG;
        
    if (EVAL(XboxPidl_IsValid(pidl)) && EVAL(!XboxID_IsServerItemID(pidlLast)))
    {
        DWORD dwIDType = XboxItemID_GetTypeID(pidlLast);

        ClearFlag(dwIDType, (IDTYPE_FILEORDIR | IDTYPE_DIR | IDTYPE_FILE));
        SetFlag(dwIDType, (fIsDir ? IDTYPE_DIR : IDTYPE_FILE));
        XboxItemID_SetTypeID((LPITEMIDLIST) pidlLast, dwIDType);

        hr = S_OK;
    }

    return hr;
}


BOOL IsXboxPidlQuestionable(LPCITEMIDLIST pidl)
{
    BOOL fIsQuestionable = FALSE;
    LPCITEMIDLIST pidlLast = ILGetLastID(pidl);

    // Is it a Server Pidl? (All Server pidls aren't questionable)
    if (XboxPidl_IsValid(pidl) && !XboxID_IsServerItemID(pidlLast))
    {
        // No, so it might be questionable.

        // Does it have "File or Dir" bit set?
        if (IsFlagSet(XboxItemID_GetTypeID(pidlLast), IDTYPE_FILEORDIR))
            fIsQuestionable = TRUE;
    }
    
    return fIsQuestionable;
}


LPITEMIDLIST XboxCloneServerID(LPCITEMIDLIST pidl)
{
    LPITEMIDLIST pidlResult = NULL;

    if (EVAL(XboxID_IsServerItemID(pidl)))
    {
        pidlResult = ILClone(pidl);

        while (!ILIsEmpty(_ILNext(pidlResult)))
            ILRemoveLastID(pidlResult);
    }

    return pidlResult;
}


/*****************************************************************************\
    FUNCTION: XboxPidl_ReplacePath

    DESCRIPTION:
        This function will fill in *ppidlOut with a pidl that contains the
    XboxServerID from pidlServer and the XboxItemIDs from pidlXboxPath.
\*****************************************************************************/
HRESULT XboxPidl_ReplacePath(LPCITEMIDLIST pidlServer, LPCITEMIDLIST pidlXboxPath, LPITEMIDLIST * ppidlOut)
{
    HRESULT hr = E_INVALIDARG;

    *ppidlOut = NULL;
    if (EVAL(XboxID_IsServerItemID(pidlServer)))
    {
        LPITEMIDLIST pidlServerOnly = XboxCloneServerID(pidlServer);

        if (pidlServerOnly)
        {
            if (XboxID_IsServerItemID(pidlXboxPath))
                pidlXboxPath = _ILNext(pidlXboxPath);

            *ppidlOut = ILCombine(pidlServerOnly, pidlXboxPath);
            if (*ppidlOut)
                hr = S_OK;
            else
                hr = E_FAIL;

            ILFree(pidlServerOnly);
        }
    }

    ASSERT_POINTER_MATCHES_HRESULT(*ppidlOut, hr);
    return hr;
}


BOOL XboxItemID_IsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    // Don't repeat recursively.
    return (S_OK == _XboxItemID_CompareOneID(COL_NAME, pidl1, pidl2, FALSE));
}


BOOL XboxPidl_IsPathEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    // This works recursively.
    return ((0 == XboxItemID_CompareIDsInt(COL_NAME, pidl1, pidl2, FCMP_NORMAL)) ? TRUE : FALSE);
}


// is pidlChild a child of pidlParent
BOOL XboxItemID_IsParent(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlChild)
{
    BOOL fIsChild = TRUE;

    if (pidlChild)
    {
        LPITEMIDLIST pidl1Iterate = (LPITEMIDLIST) pidlParent;
        LPITEMIDLIST pidl2Iterate = (LPITEMIDLIST) pidlChild;

        ASSERT(!XboxID_IsServerItemID(pidl1Iterate) && pidlParent && !XboxID_IsServerItemID(pidl2Iterate));

        // Let's see if pidl starts off with 
        while (fIsChild && pidl1Iterate && !ILIsEmpty(pidl1Iterate) &&
                pidl2Iterate && !ILIsEmpty(pidl2Iterate) && 
                XboxItemID_IsEqual(pidl1Iterate, pidl2Iterate))
        {
            fIsChild = XboxItemID_IsEqual(pidl1Iterate, pidl2Iterate);

            pidl1Iterate = _ILNext(pidl1Iterate);
            pidl2Iterate = _ILNext(pidl2Iterate);
        }

        if (!(ILIsEmpty(pidl1Iterate) && !ILIsEmpty(pidl2Iterate)))
            fIsChild = FALSE;
    }
    else
        fIsChild = FALSE;

    return fIsChild;
}


// is pidlChild a child of pidlParent, so all the itemIDs in
// pidlParent are in pidlChild, but pidlChild has more.
// This will return a pointer to those itemIDs.
LPCITEMIDLIST XboxItemID_FindDifference(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlChild)
{
    LPCITEMIDLIST pidlDiff = (LPITEMIDLIST) pidlChild;

    if (pidlChild)
    {
        LPITEMIDLIST pidl1Iterate = (LPITEMIDLIST) pidlParent;

        if (XboxID_IsServerItemID(pidl1Iterate))
            pidl1Iterate = _ILNext(pidl1Iterate);

        if (XboxID_IsServerItemID(pidlDiff))
            pidlDiff = _ILNext(pidlDiff);

        // Let's see if pidl starts off with 
        while (pidl1Iterate && !ILIsEmpty(pidl1Iterate) &&
                pidlDiff && !ILIsEmpty(pidlDiff) && 
                XboxItemID_IsEqual(pidl1Iterate, pidlDiff))
        {
            pidlDiff = _ILNext(pidlDiff);
            pidl1Iterate = _ILNext(pidl1Iterate);
        }
    }

    return pidlDiff;
}


