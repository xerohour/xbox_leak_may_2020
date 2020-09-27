
#ifndef _SHLOBJP_H_
#define _SHLOBJP_H_
#define NO_MONIKER
#include <docobj.h>
#include <shldisp.h>
#ifndef RC_INVOKED
#include <pshpack1.h>   /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */
// HMONITOR is already declared in NT 5's windef.h.
#if !defined(HMONITOR_DECLARED) && (WINVER < 0x0500)
DECLARE_HANDLE(HMONITOR);
#define HMONITOR_DECLARED
#endif
SHSTDAPI_(void *) SHAlloc(SIZE_T cb);
SHSTDAPI_(void *) SHRealloc(void * pv, SIZE_T cbNew);
SHSTDAPI_(SIZE_T)  SHGetSize(void * pv);
SHSTDAPI_(void)   SHFree(void * pv);
#define CMICEXSIZE_NT4          (SIZEOF(CMINVOKECOMMANDINFOEX) - SIZEOF(POINT))
#define CMIC_MASK_NO_HOOKS      SEE_MASK_NO_HOOKS
#define CMIC_MASK_DATAOBJECT    0x40000000   // lpPara is IDataObject*
#define CMIC_MASK_MODAL         0x80000000

#define CMIC_VALID_SEE_FLAGS    SEE_VALID_CMIC_FLAGS
//----------------------------------------------------------------------------
// Internal helper macro
//----------------------------------------------------------------------------

#define _IOffset(class, itf)         ((UINT_PTR)&(((class *)0)->itf))
#define IToClass(class, itf, pitf)   ((class  *)(((LPSTR)pitf)-_IOffset(class, itf)))
#define IToClassN(class, itf, pitf)  IToClass(class, itf, pitf)

//
// Helper macro definitions
//
#define S_BOOL(f)   MAKE_SCODE(SEVERITY_SUCCESS, 0, f)

#ifdef DEBUG
#define ReleaseAndAssert(punk) Assert(punk->lpVtbl->Release(punk)==0)
#else
#define ReleaseAndAssert(punk) (punk->lpVtbl->Release(punk))
#endif
// Property sheet ID for Explorer->View->Options File Types property sheet
// replacement.

#define EXPPS_FILETYPES 1
//  this interface is just the IID.  return back
//  a pointer to the IDropTarget interface if the
//  object calls DAD_DragLeave/DAD_SetDragImage
//  in its IDropTarget::Drop().
//
#define IDropTargetWithDADSupport IDropTarget
//===========================================================================
//
// IShellIconOverlayManager
//
// Used to return the icon overlay information including OverlayIndex, Image Index or Priority for an IShellFolder
// object, this is only inherited by CFSFolder_IconOverlayManager, it enumerates through all IconOverlayID's and
// keep their information in an array.
//
// IShellIconOverlayManager:GetFileOverlayInfo(LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags)
//      pwszPath        full path of the file
//      dwAttrib        attribute of this file
//      pIndex          pointer to the Icon Index in the system image list
//      pOverlayIndex   pointer to the OverlayIndex in the system image list
//      pPriority       pointer to the Priority of this overlay
// IShellIconOverlayManager:GetReservedOverlayInfo(LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags, int iReservedID)
//      iReservedID     reserved icon overlay id
//  returns:
//      S_OK,  if the index of an Overlay is found
//      S_FALSE, if no Overlay exists for this file
//      E_FAIL, if lpfd is bad
// IShellIconOverlayManager:RefreshOverlayImages(DWORD dwFlags)
//      This will refresh the overlay cache, depends on the dwFlags passed in
//      It will reload the icons into the imagelist, when passed SIOM_ICONINDEX
// IShellIconOverlayManager::LoadNonloadedOverlayIdentifiers()
//      This method loads any registered overlay identifiers (handlers) that
//      are not currently loaded.
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IShellIconOverlayManager

DECLARE_INTERFACE_(IShellIconOverlayManager, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellIconOverlayManager methods ***
    STDMETHOD(GetFileOverlayInfo)(THIS_ LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags) PURE;
    STDMETHOD(GetReservedOverlayInfo)(THIS_ LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags, int iReservedID) PURE;
    STDMETHOD(RefreshOverlayImages)(THIS_ DWORD dwFlags) PURE;
    STDMETHOD(LoadNonloadedOverlayIdentifiers)(THIS) PURE;
    STDMETHOD(SetIndependentOverlay)(int iImage, int * piIndex) PURE;
};
#define SIOM_OVERLAYINDEX         1
#define SIOM_ICONINDEX            2
// #define SIOM_PRIORITY          3
#define SIOM_RESERVED_SHARED      0
#define SIOM_RESERVED_LINK        1
#define SIOM_RESERVED_SLOWFILE    2
//===========================================================================
//
// IBrowserBand
//
// IBrowserBand::GetObjectBB(REFIID riid, void **ppv)
//  Obtain a service from the web browser.
//
// IBrowserBand::SetBrowserBandInfo(DWORD dwMask, PBROWSERBANDINFO pbbi)
//  Set browser band properties based on members of browserbandinfo struct
//  specified by dwMask.  These properties include title, deskband mode, and
//  current size.
//
//===========================================================================

typedef struct {
    UINT    cbSize;
    DWORD   dwModeFlags;
    BSTR    bstrTitle;
    SIZE    sizeMin;
    SIZE    sizeMax;
    SIZE    sizeCur;
} BROWSERBANDINFO, *PBROWSERBANDINFO;

#define BBIM_MODEFLAGS  0x00000001
#define BBIM_TITLE      0x00000002
#define BBIM_SIZEMIN    0x00000004
#define BBIM_SIZEMAX    0x00000008
#define BBIM_SIZECUR    0x00000010

#undef  INTERFACE
#define INTERFACE  IBrowserBand
DECLARE_INTERFACE_(IBrowserBand, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IBrowserBand methods ***
    STDMETHOD(GetObjectBB)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD(SetBrowserBandInfo)(THIS_ DWORD dwMask, PBROWSERBANDINFO pbbi) PURE;
    STDMETHOD(GetBrowserBandInfo)(THIS_ DWORD dwMask, PBROWSERBANDINFO pbbi) PURE;
};


typedef struct _TreeWalkerStats {
    int nFiles;               // number of files we have seen
    int nFolders;             // number of folders we have seen (does not include the root)
    int nDepth;               // the current depth
    DWORD dwClusterSize;      // cluster size of the disk we are currently on
    ULONGLONG  ulTotalSize;   // total size of all files we have seen
    ULONGLONG  ulActualSize;  // total size on disk of all files, taking into account compression, sparse files, and cluster slop
} TREEWALKERSTATS,  *LPTREEWALKERSTATS;

//===========================================================================
//
// IShellTreeWalkerCallBack Interface
// The callback interface for IShellTreeWalker
//
// All callbacks receives fully-qualified paths to the item found, being
// entered, being left, etc.
//
// Return values for all methods:
// S_OK    -- Normal, continue processing files, skip the problem file in error case
// S_FALSE -- Conitnue but quit the current directory
// E_NOTIMPL -- same as S_OK
// E_* -- Stop right away.
//
//===========================================================================
#undef  INTERFACE
#define INTERFACE   IShellTreeWalkerCallBack

DECLARE_INTERFACE_(IShellTreeWalkerCallBack, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellTreeWalkerCallBack methods ***
    STDMETHOD(FoundFile)(THIS_ LPCWSTR pwszPath, LPTREEWALKERSTATS ptws, WIN32_FIND_DATAW * pwfd) PURE;
    STDMETHOD(EnterFolder)(THIS_ LPCWSTR pwszPath, LPTREEWALKERSTATS ptws, WIN32_FIND_DATAW * pwfd) PURE;
    STDMETHOD(LeaveFolder)(THIS_ LPCWSTR pwszPath, LPTREEWALKERSTATS ptws) PURE;
    STDMETHOD(HandleError)(THIS_ LPCWSTR pwszPath, LPTREEWALKERSTATS ptws, HRESULT hrError) PURE;
};

typedef IShellTreeWalkerCallBack * LPSHELLTREEWALKERCALLBACK;

#define WT_FOLDERFIRST          0x00000001 // default to files first
#define WT_MAXDEPTH             0x00000002 // specify the maximum depth we go into
#define WT_FOLDERONLY           0x00000004 // only walks directories: default is to only return files
#define WT_NOTIFYFOLDERENTER    0x00000008 // notifies whenever entering a folder :default is to notify on every file
#define WT_NOTIFYFOLDERLEAVE    0x00000010 // notifies whenever leaving a folder
#define WT_GOINTOREPARSEPOINT   0x00000020 // default is to stop at any reparse points
#define WT_EXCLUDEWALKROOT      0x00000040 // default is to EnterFolder the starting point

#define WT_ALL                  0x0000007F

//===========================================================================
//
// IShellTreeWalker Interface
// The common File System tree traversal interface in shell.
//
// IShellTreeWalker::WalkTree(DWORD dwFlags, LPCWSTR pwszWalkRoot, LPCWSTR pwszWalkSpec, int iMaxPath, IShellTreeWalkerCallBack * pstwcb)
// Parameters:
// dwFlags      -- search flags: details above
// pwszWalkRoot -- the root directory to start from
// pwszWalkSpec -- the spec used to match files
// iMaxDepth    -- maximum depth to go into
// pstwcb       -- pointer to the callback interface
//
// Return Value:
// S_OK -- succeeded
// E_*  -- failed
//===========================================================================
#undef  INTERFACE
#define INTERFACE   IShellTreeWalker

DECLARE_INTERFACE_(IShellTreeWalker, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellTreeWalker methods ***
    STDMETHOD(WalkTree)(THIS_ DWORD dwFlags, LPCWSTR pwszWalkRoot, LPCWSTR pwszWalkSpec, int iMaxPath, IShellTreeWalkerCallBack * pstwcb) PURE;
};

typedef IShellTreeWalker * LPSHELLTREEWALKER;

// NT4 Console Server included shell32\shlink.h to get structure
// definitions and mimicked shell32\shlink.c to understand the
// stream format so our stream format is fixed forever. This is
// not bad since it was designed with extension in mind. We need
// to publish (as privately as possible) the file format and
// structures needed to read the file format.
//
// The stream format is a SHELL_LINK_DATA followed by
//   if SLDF_HAS_ID_LIST an ILSaveToStream followed by
//   if SLDF_HAS_LINK_INFO a LINKINFO followed by
//   if SLDF_HAS_NAME a STREAMSTRING followed by
//   if SLDF_RELPATH a STREAMSTRING followed by
//   if SLDF_WORKINGDIR a STREAMSTRING followed by
//   if SLDF_HAS_ARGS a STREAMSTRING followed by
//   if SLDF_HAS_ICON_LOCATION a STREAMSTRING followed by
//   SHWriteDataBlockList list of signature blocks
//
// Where a STREAMSTRING is a USHORT count of characters
// followed by that many (SLDF_UNICODE ? WIDE : ANSI) characters.
//
typedef struct {        // sld
    DWORD       cbSize;                 // signature for this data structure
    CLSID       clsid;                  // our GUID
    DWORD       dwFlags;                // SHELL_LINK_DATA_FLAGS enumeration

    DWORD       dwFileAttributes;
    FILETIME    ftCreationTime;
    FILETIME    ftLastAccessTime;
    FILETIME    ftLastWriteTime;
    DWORD       nFileSizeLow;

    int         iIcon;
    int         iShowCmd;
    WORD        wHotkey;
    WORD        wUnused;
    DWORD       dwRes1;
    DWORD       dwRes2;
} SHELL_LINK_DATA, *LPSHELL_LINK_DATA;
typedef struct
{
    DWORD       cbSize;             // Size of this extra data block
    DWORD       dwSignature;        // signature of this extra data block
} EXP_HEADER, *LPEXP_HEADER;
typedef struct
{
    DWORD       cbSize;             // Size of this extra data block
    DWORD       dwSignature;        // signature of this extra data block
    BYTE        abTracker[ 1 ];     //
} EXP_TRACKER, *LPEXP_TRACKER;
#define EXP_TRACKER_SIG                0xA0000003   // LPEXP_TRACKER
// NT40 Console Control Panel Applet does a CoCreateInstance(CLSID_CShellLink),
// used the CShellLink definition to get the 'this' pointer, and passed that
// along to some private exports in shell32. This completely prevents us
// from moving the CShellLink implementation to another DLL. (If we do, or an
// ISV takes our stream format and implements a better link by pointing the
// classid to their implementation, the NT40 Console CPA will fault.)
//
// To fix this problem in future revs, CShellLink will support IShellLinkDataList
//   AddDataBlock    adds the data block
//   FindDataBloc    returns a LocalAlloc()d COPY of a data block with the
//                   signature dwSig (NOTE: this is different than SHFindDataBlock)
//   RemoveDataBlock removes a data block with signature dwSig
// Plan is for NT50's Console CPA to use this interface instead.
//

//
// IURLSearchHook Interface is called whenever the browser navigates to a
// non-standard URL. It can be used to redirect the user's request to a
// search engine or a specific web site.
//
// A non-standard URL does not have a protocol prefix (e.g.,
// "www.microsoft.com") and the protocol cannot be easily guessed by
// the browser (e.g., "home.microsoft.com").
//
// The parameters of Translate(..)
//    lpwszSearchURL -- (IN/OUT) Wide char buffer that contains the request
//                      "URL" user typed in as input and the tranlated URL
//                      as output.
//    cchBufferSize  -- (IN) size of lpwszSearchURL
//
// Return Values:
// S_OK         Search handled completely, pszResult has the full URL to
//              browse to.  Stop running any further IURLSearchHooks and
//              pass this URL back to the browser for browsing.
//
// S_FALSE      Query has been preprocessed, pszResult has the result of
//              the preprocess, further search still needed. Go on
//              executing the rest of the IURLSearchHooks.  The
//              preprocessing steps can be:
//
//                      1. replaced certain characters
//                      2. added more hints
//
// E_ABORT      Search handled completely, stop running any further
//              IURLSearchHooks, but NO BROWSING NEEDED as a result,
//              pszResult is a copy of pcszQuery.
//
//              ;BUGBUG: This is not fully implemented, yet, making IURLQualify return this
//              involves too much change.
//
// BUGBUG:: E_ABORT is currently treated as E_FAIL. It requires too much change.
//
// E_FAIL       This Hook was unsuccessful. Search not handled at all,
//              pcszQueryURL has the query string. Please go on running
//              other IURLSearchHooks.
//
// BUGBUG:  There is a potential danger in this Interface, that is people can write
//          very bad SearchHooks, but we (Microsoft) take the blame.

// History:
//  --/--/94 KurtE Created
//
// History:
//  3/4/94 kraigb Created
//

//  CGID_DefView Command Target IDs. used to persist global DefView state

enum
{
    DVID_SETASDEFAULT,          // Set the DefView state of this folder as default for all of the same class
    DVID_RESETDEFAULT           // Reset the DefView state to the default (delete subkeys)
};

// CGID_ShellDocView Command Target IDs. for shell doc view wedge
enum {
    SHDVID_FINALTITLEAVAIL,     // DEAD: variantIn bstr - sent after final OLECMDID_SETTITLE is sent
    SHDVID_MIMECSETMENUOPEN,    // mimecharset menu open commands
    SHDVID_PRINTFRAME,          // print HTML frame
    SHDVID_PUTOFFLINE,          // DEAD: The Offline property has been changed
    SHDVID_PUTSILENT,           // DEAD: The frame's Silent property has been changed
    SHDVID_GOBACK,              // Navigate Back
    SHDVID_GOFORWARD,           // Navigate Forward
    SHDVID_CANGOBACK,           // Is Back Navigation Possible?
    SHDVID_CANGOFORWARD,        // Is Forward Navigation Possible?
    SHDVID_CANACTIVATENOW,      // (down) (PICS) OK to navigate to this view now?
    SHDVID_ACTIVATEMENOW,       // (up) (PICS) Rating checks out, navigate now
    SHDVID_CANSUPPORTPICS,      // (down) variantIn I4: IOleCommandTarget to reply to
    SHDVID_PICSLABELFOUND,      // (up) variantIn bstr: PICS label
    SHDVID_NOMOREPICSLABELS,    // (up) End of document, no more PICS labels coming
    SHDVID_CANDEACTIVATENOW,    // (QS down) (in script/etc) OK to deactivate view now?
    SHDVID_DEACTIVATEMENOW,     // (EXEC up) (in script/etc) out of script, deactivate view now
    SHDVID_NODEACTIVATENOW,     // (EXEC up) (in script/etc) entering script, disable deactivate
    SHDVID_AMBIENTPROPCHANGE,   // variantIn I4: dispid of ambient property that changed
    SHDVID_GETSYSIMAGEINDEX,    // variantOut: image index for current page
    SHDVID_GETPENDINGOBJECT,    // variantOut: IUnknown of pending shellview/docobject
    SHDVID_GETPENDINGURL,       // variantOut: BSTR of URL for pending docobject
    SHDVID_SETPENDINGURL,       // variantIn: BSTR of URL passed to pending docobject
    SHDVID_ISDRAGSOURCE,        // (down) varioutOut I4: non-zero if it's initiated drag&drop
    SHDVID_DOCFAMILYCHARSET,    // variantOut: I4: windows (family) codepage
    SHDVID_DOCCHARSET,          // variantOut: I4: actual (mlang) codepage
    SHDVID_RAISE,               // vaIn:I4:DTRF_*, vaOut:NULL unless DTRF_QUERY
    SHDVID_GETTRANSITION,       // (down) vaIn: I4: TransitionEvent; vaOut BSTR (CLSID), I4 (dwSpeed)
    SHDVID_GETMIMECSETMENU,     // get menu handle for mimecharset
    SHDVID_DOCWRITEABORT,       // Abort binding but activate pending docobject
    SHDVID_SETPRINTSTATUS,      // VariantIn: BOOL, TRUE - Started printing, FALSE - Finished printing
    SHDVID_NAVIGATIONSTATUS,    // QS for tooltip text and Exec when user clicks
    SHDVID_PROGRESSSTATUS,      // QS for tooltip text and Exec when user clicks
    SHDVID_ONLINESTATUS,        // QS for tooltip text and Exec when user clicks
    SHDVID_SSLSTATUS,           // QS for tooltip text and Exec when user clicks
    SHDVID_PRINTSTATUS,         // QS for tooltip text and Exec when user clicks
    SHDVID_ZONESTATUS,          // QS for tooltip text and Exec when user clicks
    SHDVID_ONCODEPAGECHANGE,    // variantIn I4: new specified codepage
    SHDVID_SETSECURELOCK,       // set the secure icon
    SHDVID_SHOWBROWSERBAR,      // show browser bar of clsid guid
    SHDVID_NAVIGATEBB,          // navigate to pidl in browserbar.
    SHDVID_UPDATEOFFLINEDESKTOP,// put the desktop in ON-LINE mode, update and put it back in Offline mode
    SHDVID_PICSBLOCKINGUI,      // (up) In I4: pointer to "ratings nugget" for block API
    SHDVID_ONCOLORSCHANGE,      // (up) sent by mshtml to indicate color set change
    SHDVID_CANDOCOLORSCHANGE,   // (down) used to query if document supports the above
    SHDVID_QUERYMERGEDHELPMENU, // was the help menu micro-merged?
    SHDVID_QUERYOBJECTSHELPMENU,// return the object's help menu
    SHDVID_HELP,                // do help
    SHDVID_UEMLOG,              // set UEM logging vaIn:I4:UEMIND_*, vaOut:NULL
    SHDVID_GETBROWSERBAR,       // get IDeskBand for browser bar of clsid guid
    SHDVID_GETFONTMENU,
    SHDVID_FONTMENUOPEN,
    SHDVID_CLSIDTOIDM,          // get the idm for the given clsid
    SHDVID_GETDOCDIRMENU,       // get menu handle for document direction
    SHDVID_ADDMENUEXTENSIONS,   // Context Menu Extensions
    SHDVID_CLSIDTOMONIKER,      // CLSID to property page resource mapping
    SHDVID_RESETSTATUSBAR,      // set the status bar back to "normal" icon w/out text
    SHDVID_ISBROWSERBARVISIBLE, // is browser bar of clsid guid visible?
    SHDVID_GETOPTIONSHWND,      // gets hwnd for internet options prop sheet (NULL if not open)
    SHDVID_DELEGATEWINDOWOM,    // set policy for whether window OM methods should be delegated.
    SHDVID_PAGEFROMPOSTDATA,    // determines if page was generated by post data
    SHDVID_DISPLAYSCRIPTERRORS, // tells the top docobject host to display his script err dialog
};

// CGID_SearchBand Command Target IDs.
enum {
    SBID_SEARCH_NEW,            // (re)load the context-appropriate url
    SBID_SEARCH_NEXT,           // cycle between multiple providers
    SBID_SEARCH_CUSTOMIZE,      // navigate to customization url
    SBID_SEARCH_HELP,           // what it says
    SBID_GETPIDL,               // get the search pane's pidl
};

//
//  this enum is used by SHDVID_SETSECURELOCK and SBCMDID_SETSECURELOCKICON
//  when adding new values, make sure to add to both SET and SUGGEST, and
//  the code depends on a direct correlation between the level of security
//  and the value of the enum.  so keep new values in the correct order
//
enum {
    SECURELOCK_NOCHANGE        = -1,
    SECURELOCK_SET_UNSECURE    = 0,
    SECURELOCK_SET_MIXED,
    SECURELOCK_SET_SECUREUNKNOWNBIT,
    SECURELOCK_SET_SECURE40BIT,
    SECURELOCK_SET_SECURE56BIT,
    SECURELOCK_SET_FORTEZZA,
    SECURELOCK_SET_SECURE128BIT,
    SECURELOCK_FIRSTSUGGEST,
    SECURELOCK_SUGGEST_UNSECURE = SECURELOCK_FIRSTSUGGEST,
    SECURELOCK_SUGGEST_MIXED,
    SECURELOCK_SUGGEST_SECUREUNKNOWNBIT,
    SECURELOCK_SUGGEST_SECURE40BIT,
    SECURELOCK_SUGGEST_SECURE56BIT,
    SECURELOCK_SUGGEST_FORTEZZA,
    SECURELOCK_SUGGEST_SECURE128BIT,
};

#define FCIDM_DRIVELIST    (FCIDM_BROWSERFIRST + 2) //
#define FCIDM_TREE         (FCIDM_BROWSERFIRST + 3) //
#define FCIDM_TABS         (FCIDM_BROWSERFIRST + 4) //
#define FCIDM_REBAR        (FCIDM_BROWSERFIRST + 5) //
typedef FOLDERSETTINGS *PFOLDERSETTINGS;
#define FCW_VIEW        0x0004
#define FCW_BROWSER     0x0005
#define FCW_MENUBAR     0x0007
#define FCW_ADDRESSBAR  0x0009
#define FCW_TOOLBAND    0x000a
#define FCW_LINKSBAR    0x000b
// CGID_Explorer Command Target IDs
enum {
    SBCMDID_ENABLESHOWTREE          = 0,
    SBCMDID_SHOWCONTROL             = 1,        // variant vt_i4 = loword = FCW_* hiword = SBSC_*
    SBCMDID_CANCELNAVIGATION        = 2,        // cancel last navigation
    SBCMDID_MAYSAVECHANGES          = 3,        // about to close and may save changes
    SBCMDID_SETHLINKFRAME           = 4,        // variant vt_i4 = phlinkframe
    SBCMDID_ENABLESTOP              = 5,        // variant vt_bool = fEnable
    SBCMDID_OPTIONS                 = 6,        // the view.options page
    SBCMDID_EXPLORER                = 7,        // are you explorer.exe?
    SBCMDID_ADDTOFAVORITES          = 8,
    SBCMDID_ACTIVEOBJECTMENUS       = 9,
    SBCMDID_MAYSAVEVIEWSTATE        = 10,       // Should we save view stream
    SBCMDID_DOFAVORITESMENU         = 11,       // popup the favorites menu
    SBCMDID_DOMAILMENU              = 12,       // popup the mail menu
    SBCMDID_GETADDRESSBARTEXT       = 13,       // get user-typed text
    SBCMDID_ASYNCNAVIGATION         = 14,       // do an async navigation
    SBCMDID_SEARCHBAROBSOLETE       = 15,       // OBSOLETE beta-1 SEARCHBAR
    SBCMDID_FLUSHOBJECTCACHE        = 16,       // flush object cache
    SBCMDID_CREATESHORTCUT          = 17,       // create a shortcut
    SBCMDID_SETMERGEDWEBMENU        = 18,       // set pre-merged menu for HTML
    SBCMDID_REMOVELOCKICON          = 19,       // Remove lock icon
    SBCMDID_SELECTHISTPIDL          = 20,       // QS:EXEC select history pidl on navigate
    SBCMDID_WRITEHIST               = 21,       // QS only:write history sf on navigate
    SBCMDID_GETHISTPIDL             = 22,       // EXEC gets most recent history pidl
    SBCMDID_REGISTERNSCBAND         = 23,       // EXEC registers NscBand when visible
    SBCMDID_COCREATEDOCUMENT        = 24,       // Create an instance of MSHTML
    SBCMDID_SETSECURELOCKICON       = 25,       // sets the SSL lock icon for the current page
    SBCMDID_INITFILECTXMENU         = 26,       // EXEC,QS: file context menu for band item
    SBCMDID_UNREGISTERNSCBAND       = 27,       // EXEC unregisters NscBand when invisible
    SBCMDID_SEARCHBAR               = 28,       // show/hide/toggle BrowserBar search
    SBCMDID_HISTORYBAR              = 29,       // show/hide/toggle BrowserBar history
    SBCMDID_FAVORITESBAR            = 30,       // show/hide/toggle BrowserBar favorites
#ifdef ENABLE_CHANNELS
    SBCMDID_CHANNELSBAR             = 31,       // show/hide/toggle BrowserBar channels
#endif
    SBCMDID_SENDPAGE                = 32,       // sends the current page via email
    SBCMDID_SENDSHORTCUT            = 33,       // sends a shortcut to current page via email
    SBCMDID_TOOLBAREMPTY            = 34,       // toolbar has nothing visible
    SBCMDID_EXPLORERBAR             = 35,       // show/hide/toggle BrowserBar all folders
    SBCMDID_GETUSERADDRESSBARTEXT   = 36,       // get the exact text the user-entered
    SBCMDID_HISTSFOLDER             = 37,       // EXEC varOut returns cached history IShellFolder
    SBCMDID_UPDATETRAVELLOG         = 38,       // updates the travellog with the current info
    SBCMDID_MIXEDZONE               = 39,       // The current view has mixed zones
    SBCMDID_REPLACELOCATION         = 40,       // replace the current URL with this one.
    SBCMDID_GETPANE                 = 41,       // Browser asks the view for pane ordinal
    SBCMDID_FILERENAME              = 42,       // Browser forwards file->rename to explorer bar
    SBCMDID_FILEDELETE              = 43,       // Browser forwards file->delete to explorer bar
    SBCMDID_FILEPROPERTIES          = 44,       // Browser forwards file->properties to explorer bar
    SBCMDID_IESHORTCUT              = 45,       // Navigates the browser to an .url file
    SBCMDID_GETSHORTCUTPATH         = 46,       // Asks for the path to the shortcut - if it exists
    SBCMDID_DISCUSSIONBAND          = 47,       // show/hide/toggle the Office discussion band
    SBCMDID_SETADDRESSBARFOCUS      = 48,       // Set focus on address bar on WM_ACTIVATE (UNIX)
    SBCMDID_HASADDRESSBARFOCUS      = 49,       // Checks whether address bar has focus (UNIX)
    SBCMDID_MSGBAND                 = 50,       // MsgBand Message (UNIX)
    SBCMDID_ISIEMODEBROWSER         = 51,       // returns S_OK if browser is in IE mode
    SBCMDID_GETTEMPLATEMENU         = 52,       // get handle to template menu
    SBCMDID_GETCURRENTMENU          = 53,       // get handle to current menu
    SBCMDID_ERRORPAGE               = 54,       // informs addressbar mru that this is an error page
    SBCMDID_AUTOSEARCHING           = 55,       // informs addressbar mru that we are trying a new url
    SBCMDID_STARTEDFORINTERNET      = 56,       // return S_OK if window started as IE window, S_FALSE otherwise
    SBCMDID_ISBROWSERACTIVE         = 57,       // return S_OK if the browser is active, S_FALSE otherwise
    SBCMDID_SUGGESTSAVEWINPOS       = 58,       // return S_OK if browser decides to save window position, S_FALSE otherwise
    SBCMDID_CACHEINETZONEICON       = 59,       // Cache/refresh internet zone icons painted by status bar
    SBCMDID_ONVIEWMOVETOTOP         = 60,       // Notification that the view window has moved to HWND_TOP
};

#define ZONE_UNKNOWN    -1
#define ZONE_MIXED      -2
#define IS_SPECIAL_ZONE(a) (((a) == ZONE_UNKNOWN) || ((a) == ZONE_MIXED))

// SBCMDID_GETPANE
#define PANE_NONE        -1
#define PANE_ZONE        1
#define PANE_OFFLINE     2
#define PANE_PRINTER     3
#define PANE_SSL         4
#define PANE_NAVIGATION  5
#define PANE_PROGRESS    6



#if (_WIN32_IE >= 0x0400)
// CGID_AddresseditBox Command Target IDs
enum {
    AECMDID_SAVE                    = 0,        // Ask AddressEditBox to save
};
#endif

// The pvid parameter of the GetView() method of IShellView2 is normally only an OUT parameter.
// But, for SV2GV_ISEXTENDEDVIEW, it is an IN parameter.
#define SV2GV_ISEXTENDEDVIEW    ((UINT)-3)
#define STRRET_OLESTR   0x0000          // same as STRRET_WSTR
#define STRRET_OFFPTR(pidl,lpstrret) ((LPSTR)((LPBYTE)(pidl)+(lpstrret)->NAMELESS_MEMBER(uOffset)))
SHSTDAPI_(int) SHCreateDirectory(HWND hwnd, LPCTSTR pszPath);	//
#define REGSTR_PATH_INTERNET_EXPLORER   TEXT("\\SOFTWARE\\Microsoft\\Internet Explorer")
#define REGSTR_PATH_IE_MAIN             REGSTR_PATH_INTERNET_EXPLORER TEXT("\\Main")
#define REGSTR_VALUE_USER_AGENT         TEXT("UserAgent")
#define REGSTR_DEFAULT_USER_AGENT       TEXT("Mozilla/2.0 (compatible; MSIE 3.0A; Windows 95)")
                                    //  0x000c
                                    //  0x000d
                                    //  0x000e
                                    //  0x000f
#define CSIDL_FLAG_DONT_UNEXPAND        0x2000        // combine with CSIDL_ value to avoid unexpanding environment variables
SHSTDAPI_(void) SHFlushSFCache(void);
SHSTDAPI_(LPITEMIDLIST) SHCloneSpecialIDList(HWND hwnd, int csidl, BOOL fCreate);
SHFOLDERAPI SHSetFolderPathA(int csidl, HANDLE hToken, DWORD dwFlags, LPCSTR pszPath);
SHFOLDERAPI SHSetFolderPathW(int csidl, HANDLE hToken, DWORD dwFlags, LPCWSTR pszPath);
#ifdef UNICODE
#define SHSetFolderPath  SHSetFolderPathW
#else
#define SHSetFolderPath  SHSetFolderPathA
#endif // !UNICODE
#if (_WIN32_IE >= 0x0500)

#define FCS_READ                    0x00000001
#define FCS_FORCEWRITE              0x00000002
#define FCS_WRITE                   (FCS_READ | FCS_FORCEWRITE)

#define FCS_FLAG_DRAGDROP           2

// Mask which values have been retreived or being set.
#define FCSM_VIEWID                 0x00000001
#define FCSM_WEBVIEWTEMPLATE        0x00000002
#define FCSM_INFOTIP                0x00000004
#define FCSM_CLSID                  0x00000008
#define FCSM_ICONFILE               0x00000010
#define FCSM_LOGO                   0x00000020
#define FCSM_FLAGS                  0x00000040

// Used by SHGetSetFolderCustomSettingsA
typedef struct
{
    DWORD           dwSize;
    DWORD           dwMask;             // IN/OUT   Which Attributes to Get/Set
    SHELLVIEWID*    pvid;               // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // The folder's WebView template path
    LPSTR           pszWebViewTemplate;  // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchWebViewTemplate;  // IN - Specifies the size of the buffer pointed to by pszWebViewTemplate
                                        // Ignored if dwReadWrite is FCS_READ
    LPSTR           pszWebViewTemplateVersion;  // currently IN only
    // Infotip for the folder
    LPSTR           pszInfoTip;         // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchInfoTip;         // IN - Specifies the size of the buffer pointed to by pszInfoTip
                                        // Ignored if dwReadWrite is FCS_READ
    // CLSID that points to more info in the registry
    CLSID*          pclsid;             // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // Other flags for the folder. Takes FCS_FLAG_* values
    DWORD           dwFlags;            // OUT - if dwReadWrite is FCS_READ, IN - otherwise

    LPSTR           pszIconFile;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchIconFile;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                        // Ignored if dwReadWrite is FCS_READ

    int             iIconIndex;         // OUT - if dwReadWrite is FCS_READ, IN - otherwise

    LPSTR           pszLogo;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchLogo;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                        // Ignored if dwReadWrite is FCS_READ


} SHFOLDERCUSTOMSETTINGSA, *LPSHFOLDERCUSTOMSETTINGSA;
// Gets/Sets the Folder Custom Settings for pszPath based on dwReadWrite. dwReadWrite can be FCS_READ/FCS_WRITE/FCS_FORCEWRITE
HRESULT SHGetSetFolderCustomSettingsA(LPSHFOLDERCUSTOMSETTINGSA pfcs, LPCSTR pszPath, DWORD dwReadWrite);

// Used by SHGetSetFolderCustomSettingsW
typedef struct
{
    DWORD           dwSize;
    DWORD           dwMask;              // IN/OUT  Which Attributes to Get/Set
    SHELLVIEWID*    pvid;                // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // The folder's WebView template path
    LPWSTR          pszWebViewTemplate;   // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchWebViewTemplate;   // IN - Specifies the size of the buffer pointed to by pszWebViewTemplate
                                         // Ignored if dwReadWrite is FCS_READ
    LPWSTR           pszWebViewTemplateVersion;  // currently IN only
    // Infotip for the folder
    LPWSTR          pszInfoTip;          // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchInfoTip;          // IN - Specifies the size of the buffer pointed to by pszInfoTip
                                         // Ignored if dwReadWrite is FCS_READ
    // CLSID that points to more info in the registry
    CLSID*          pclsid;              // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // Other flags for the folder. Takes FCS_FLAG_* values
    DWORD           dwFlags;             // OUT - if dwReadWrite is FCS_READ, IN - otherwise


    LPWSTR           pszIconFile;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD            cchIconFile;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                         // Ignored if dwReadWrite is FCS_READ

    int              iIconIndex;         // OUT - if dwReadWrite is FCS_READ, IN - otherwise

    LPWSTR           pszLogo;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD            cchLogo;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                         // Ignored if dwReadWrite is FCS_READ
} SHFOLDERCUSTOMSETTINGSW, *LPSHFOLDERCUSTOMSETTINGSW;
// Gets/Sets the Folder Custom Settings for pszPath based on dwReadWrite. dwReadWrite can be FCS_READ/FCS_WRITE/FCS_FORCEWRITE
HRESULT SHGetSetFolderCustomSettingsW(LPSHFOLDERCUSTOMSETTINGSW pfcs, LPCWSTR pszPath, DWORD dwReadWrite);

#ifdef UNICODE
#define SHFOLDERCUSTOMSETTINGS          SHFOLDERCUSTOMSETTINGSW
#define SHGetSetFolderCustomSettings    SHGetSetFolderCustomSettingsW
#define LPSHFOLDERCUSTOMSETTINGS        LPSHFOLDERCUSTOMSETTINGSW
#else
#define SHFOLDERCUSTOMSETTINGS          SHFOLDERCUSTOMSETTINGSA
#define SHGetSetFolderCustomSettings    SHGetSetFolderCustomSettingsA
#define LPSHFOLDERCUSTOMSETTINGS        LPSHFOLDERCUSTOMSETTINGSA
#endif

#endif  // _WIN32_IE >= 0x0500
    // pszTitle can be a resource, but the hinst is assumed to be shell32.dll
    // lpszTitle can be a resource, but the hinst is assumed to be shell32.dll

//-------------------------------------------------------------------------
//
// Shell Service Object
//
//-------------------------------------------------------------------------
// Cmds for CGID_ShellServiceObject
enum {
    SSOCMDID_LOAD        = 0,
    SSOCMDID_UNLOAD      = 1,
    SSOCMDID_OPEN        = 2,
    SSOCMDID_CLOSE       = 3,
    SSOCMDID_STARTMENU   = 4,
    SSOCMDID_SAVETODOCUMENT=5,
};
#define CMDID_SAVEASTHICKET  SSOCMDID_SAVETODOCUMENT

//-------------------------------------------------------------------------
//
// Internet Shortcut Object
//
//-------------------------------------------------------------------------
// Cmds for CGID_ShortCut
enum {
    ISHCUTCMDID_DOWNLOADICON  = 0,
    ISHCUTCMDID_INTSHORTCUTCREATE = 1,
};
#define CMDID_INTSHORTCUTCREATE ISHCUTCMDID_INTSHORTCUTCREATE



//-------------------------------------------------------------------------
// STR_DISPLAY_UI_DURING_BINDING - IBindCtx Key
//-------------------------------------------------------------------------
// If displaying UI during binding is allowed, this parameter
// will be in the IBindCtx so the object binding can get the
// IUnknown* of the site in order to:
// 1. Use the site's hwnd as the parent hwnd of any UI dialogs.
//    This is done via. IOleWindow::GetWindow() or IInternetSecurityMgrSite::GetWindow()
// 2. While the dialog is being displayed, make the site modal
//    This is done via:
//       IOleInPlaceActiveObject::EnableModeless()
//       IOleInPlaceUIWindow::EnableModelessSB()
//       IInternetSecurityMgrSite::EnableModeless()

#define STR_DISPLAY_UI_DURING_BINDING      L"UI During Binding"


//  This member function should always create a new
//
//  SHCIDS_ALLFIELDS is a mask for lParam indicating that the shell folder
//  should first compare on the lParam column, and if that proves equal,
//  then perform a full comparison on all fields.  This flag is supported
//  if the IShellFolder supports IShellFolder2.
//
//  If you add more flags in the future, you need to enhance the return
//  value from SFVM_SUPPORTSIDENTITY.
//
// IShellFolderFilterSite

#undef  INTERFACE
#define INTERFACE       IShellFolderFilterSite

DECLARE_INTERFACE_(IShellFolderFilterSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderFilterSite methods ***
    STDMETHOD(SetFilter)(THIS_ IUnknown* punk) PURE;
};

// IIDListFilter

#undef  INTERFACE
#define INTERFACE       IShellFolderFilter

DECLARE_INTERFACE_(IShellFolderFilter, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderFilter methods ***
    STDMETHOD(ShouldShow)(THIS_ IShellFolder* psf, LPCITEMIDLIST pidlFolder, LPCITEMIDLIST pidlItem) PURE;
    STDMETHOD(GetEnumFlags)(THIS_ IShellFolder* psf, LPCITEMIDLIST pidlFolder, HWND *phwnd, DWORD *pgrfFlags) PURE;
};

//
// The IShellDetails interface was private in win95 and didnt have a _WIN32_IE guard,
// so it is now in shlobj.h, along with the definition of SHELLDETAILS
//


//-------------------------------------------------------------------------
//
// DELEGATEITEMID structure
//
//
// A Delegate Item ID is a standard SHITEMID with some additional
// fields defined.
//
//-------------------------------------------------------------------------

typedef struct tagDELEGATEITEMID {
    WORD cbSize;                // Size of entire item ID
    WORD wOuter;                // Private data owned by the outer folder
    WORD cbInner;               // Size of delegate's data
    BYTE rgb[1];                // Inner folder's data,
                                //   followed by outer folder's data.
} DELEGATEITEMID;

typedef const UNALIGNED DELEGATEITEMID *PCDELEGATEITEMID;
typedef UNALIGNED DELEGATEITEMID *PDELEGATEITEMID;


//-------------------------------------------------------------------------
//
// IDelegateFolder interface
//
//
// [Member functions]
//
// IDelegateFolder::SetItemAlloc(pm)
//   This function gives the object an IMalloc interface that it
//   should use to alloc and free item IDs.  These IDs are in the
//   form of DELEGATEITEMIDs.
//
//-------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IDelegateFolder

DECLARE_INTERFACE_(IDelegateFolder, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IDelegateFolder methods ***
    STDMETHOD(SetItemAlloc)(THIS_ IMalloc *pmalloc) PURE;
};

#undef  INTERFACE
#define INTERFACE   IMountedVolume

DECLARE_INTERFACE_(IMountedVolume, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IMountedVolume methods ***
    STDMETHOD(Initialize)(THIS_ LPCWSTR pcszHostFolderPath) PURE;
};
//-------------------------------------------------------------------------
// This is the interface for a browser to "subclass" the main File Cabinet
// window.  Note that only the hwnd, message, wParam, and lParam fields of
// the msg structure are used.  The browser window will get a WM_NOTIFY
// message with NULL ID, FCN_MESSAGE as the code, and a far pointer to
// FCMSG_NOTIFY as the lParam.
//
//-------------------------------------------------------------------------
typedef struct tagFCMSG_NOTIFY
{
    NMHDR   hdr;
    MSG     msg;
    LRESULT lResult;
} FCMSG_NOTIFY;

#define FCN_MESSAGE (100)


//---------------------------------------------------------------------------
// messages that can be send to the cabinet by other apps
//
// REVIEW: Do we really need to publish any of those?
//---------------------------------------------------------------------------

#define NF_INHERITVIEW 0x0000
#define NF_LOCALVIEW   0x0001

// Change the path of an existing folder.
// wParam:
//      0:              LPARAM is a pidl, handle the message immediately.
//      CSP_REPOST:     LPARAM is a pidl, copy the pidl and handle the
//                      message later.
//      CSP_NOEXECUTE:  if this path is not a folder, fail, don't shell exec
//
//
// lParam: LPITEMIDLIST of path.
//
//
#define CSP_REPOST                  0x0001
#define CSP_INITIATEDBYHLINKFRAME   0x0002
#define CSP_NOEXECUTE               0x0004
#define CSP_NONAVIGATE              0x0008
#define CWM_SETPATH                 (WM_USER + 2)

// lpsv points to the Shell View extension that requested idle processing
// uID is an app define identifier for the processor
// returns: TRUE if there is more idle processing necessary, FALSE if all done
// Note that the idle processor should do one "atomic" operation and return
// as soon as possible.
typedef BOOL (CALLBACK *FCIDLEPROC)(void *lpsv, UINT uID);

// Inform the File Cabinet that you want idle messages.
// This should ONLY be used by File Cabinet extensions.
// wParam: app define UINT (passed to FCIDLEPROC).
// lParam: pointer to an FCIDLEPROC.
// return: TRUE if successful; FALSE otherwise
//
#define CWM_WANTIDLE                (WM_USER + 3)

// get or set the FOLDERSETTINGS for a view
// wParam: BOOL TRUE -> set to view info buffer, FALSE -> get view info buffer
// lParam: LPFOLDERSETTINGS buffer to get or set view info
//
#define CWM_GETSETCURRENTINFO       (WM_USER + 4)
#define FileCabinet_GetSetCurrentInfo(_hwnd, _bSet, _lpfs) \
             SendMessage(_hwnd, CWM_GETSETCURRENTINFO, (WPARAM)(_bSet), (LPARAM)(LPFOLDERSETTINGS)_lpfs)

// selects the specified item in the current view
// wParam: SVSI_* flags
// lParam: LPCITEMIDLIST of the item ID, NULL -> all items
//
#define CWM_SELECTITEM              (WM_USER + 5)
#define FileCabinet_SelectItem(_hwnd, _sel, _item) \
            SendMessage(_hwnd, CWM_SELECTITEM, _sel, (LPARAM)(LPCITEMIDLIST)(_item))

// selects the specified path in the current view
// wParam: SVSI_* flags
// lParam: LPCSTR of the display name
//
#define CWM_SELECTPATH              (WM_USER + 6)
#define FileCabinet_SelectPath(_hwnd, _sel, _path)  \
            SendMessage(_hwnd, CWM_SELECTPATH, _sel, (LPARAM)(LPCSTR)(_path))

// Get the IShellBrowser object associated with an hwndMain
#define CWM_GETISHELLBROWSER        (WM_USER + 7)
#define FileCabinet_GetIShellBrowser(_hwnd)     \
            (IShellBrowser*)SendMessage(_hwnd, CWM_GETISHELLBROWSER, 0, 0L)

//
//  two pidls can have the same path, so we need a compare pidl message
#define CWM_COMPAREPIDL                (WM_USER + 9)
//
//  sent when the global state changes
#define CWM_GLOBALSTATECHANGE           (WM_USER + 10)
#define CWMF_GLOBALSTATE                0x0000
#define CWMF_SECURITY                   0x0001

//  sent to the desktop from a second instance
#define CWM_COMMANDLINE                 (WM_USER + 11)
// global clone your current pidl
#define CWM_CLONEPIDL                   (WM_USER + 12)
// See if the root of the instance is as specified
#define CWM_COMPAREROOT                 (WM_USER + 13)
// Tell desktop our root
#define CWM_SPECIFYCOMPARE              (WM_USER + 14)
// See if the root of the instance matches a hwnd
#define CWM_PERFORMCOMPARE              (WM_USER + 15)
// Forward SHChangeNotify events
#define CWM_FSNOTIFY                    (WM_USER + 16)
// Forward SHChangeRegistration events
#define CWM_CHANGEREGISTRATION          (WM_USER + 17)
// For AddToRecentDocs processing by desktop
#define CWM_ADDTORECENT                 (WM_USER + 18)
// For SHWaitForFile processing by desktopop
#define CWM_WAITOP                      (WM_USER + 19)

// Notify for changes to the fav's folder.
#define CWM_FAV_CHANGE                  (WM_USER + 20)
#define CWM_SHOWDRAGIMAGE               (WM_USER + 21)
#define CWM_SHOWFOLDEROPT               (WM_USER + 22)

#define CWM_FSNOTIFYSUSPENDRESUME       (WM_USER + 23)

#define CWM_RESERVEDFORCOMDLG_FIRST     (WM_USER + 100)
#define CWM_RESERVEDFORCOMDLG_LAST      (WM_USER + 199)
#define CWM_RESERVEDFORWEBBROWSER_FIRST (WM_USER + 200)
#define CWM_RESERVEDFORWEBBROWSER_LAST  (WM_USER + 299)
//   ;BUGBUG (scotth): fill this in
#define STFRF_NORMAL            DWFRF_NORMAL
#define STFRF_DELETECONFIGDATA  DWFRF_DELETECONFIGDATA
//
//   formerly IDockingWindow::SetToolbarSite
//   ;BUGBUG (scotth): what's the difference b/t punkToolbarSite
//                    and the punkSite provided by the
//                    SetToolbarSite method?

#undef  INTERFACE
#define INTERFACE   IMultiMonitorDockingSite

DECLARE_INTERFACE_(IMultiMonitorDockingSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IMultiMonitorDockingSite methods ***
    STDMETHOD(GetMonitor) (THIS_ IUnknown* punkSrc, HMONITOR * phMon) PURE;
    STDMETHOD(RequestMonitor) (THIS_ IUnknown* punkSrc, HMONITOR * phMon) PURE;
    STDMETHOD(SetMonitor) (THIS_ IUnknown* punkSrc, HMONITOR hMonNew, HMONITOR *phMonOld) PURE;
};
#define DBIMF_FIXED             0x0001  //
#define DBIMF_FIXEDBMP          0x0004   // a fixed background bitmap (if supported)   //
#define DBIMF_UNDELETEABLE      0x0010   //
#define DBIMF_USECHEVRON        0x0080  //
#define DBIMF_BREAK             0x0100  //
// flags for DBID_PUSHCHEVRON
#define DBPC_SELECTFIRST    (DWORD)-1
#define DBPC_SELECTLAST     (DWORD)-2

#ifdef ISHELLTOOLBAND_COMPAT
#undef  INTERFACE
#define INTERFACE   IShellToolband

DECLARE_INTERFACE_(IShellToolband, IDockingWindow)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

    // *** IDockingWindow methods ***
    STDMETHOD(ShowDW)         (THIS_ BOOL fShow) PURE;
    STDMETHOD(CloseDW)        (THIS_ DWORD dwReserved) PURE;
    STDMETHOD(ResizeBorderDW) (THIS_ LPCRECT   prcBorder,
                                     IUnknown* punkToolbarSite,
                                     BOOL      fReserved) PURE;
    // *** IShellToolband methods ***
    STDMETHOD(GetBandInfo)    (THIS_ DWORD fViewMode, LPPOINT pptSize,
                              LPWSTR pszTitle, int cchTitle, DWORD *dwSizeMode) PURE;
};

// IShellToolband Enum
typedef enum {
        STBE_MINSIZE = 0,
        STBE_MAXSIZE,
        STBE_INTEGRAL,
        STBE_ACTUAL,

        STBE_MAX   // how many are there?
} STBENUM;

// command target ids
enum {
        STBID_BANDINFOCHANGED,
};

#define STBBIF_VIEWMODE_NORMAL   0x0000
#define STBBIF_VIEWMODE_VERTICAL 0x0001
#define STBBIF_VIEWMODE_FLOATING 0x0002

#define STBBIF_SIZEMODE_NORMAL  0x0
#define STBBIF_SIZEMODE_FIXED   0x1
#define STBBIF_SIZEMODE_TITLE   0x2
#define STBBIF_SIZEMODE_FIXEDBMP 0x4   // a fixed background bitmap (if supported)
#define STBBIF_SIZEMODE_VARIABLEHEIGHT 0x8

#define STBBIF_DRAGMODE_UNDELETEABLE 0x10
#endif
#if (_WIN32_IE >= 0x0400)

#define TASKOWNERID     GUID
#define REFTASKOWNERID  REFGUID


// ---ITaskScheduler
// An interface for interacting with and controlling a task scheduler. This
// interface does not need to be free-threaded unless the items in the queue
// interact with the scheduler as well as the main execution thread on which the
// task scheduler was created.

// ITaskScheduler::AddTask()
//      Adds Tasks to the scheduler's background queue. The TASKOWNERID allow particular types
//      of tasks to be grouped so that they can be counted or removed. The lParam allows the task
//      to be associated with a particular item (for example an item in a listview).
// ITaskScheduler::RemoveTasks()
//      Removes tasks from the scheduler's queue. These can be sepcified in terms of their TASKOWNERID
//      or their LPARAM, or both, or neither (TOID_NULL && ITSAT_DEFAULT_LPARAM results in all tasks being
//      removed). If a task that matches is currently running and ITaskScheduler::Status() has been
//      passeed ITSSFLAG_KILL_ON_DESTROY then the scheduler will attempt to kill the current task. The
//      fWaitIfRunning parameter is then passed to IRunnableTask::Kill().
// ITaskScheduler::CountTasks()
//      Counts the tasks in the queue depending upon the TASKOWNERID and the LPARAM passed. (TOID_NULL and
//      ITSAT_DEFAULT_LPARAM will count all tasks in the queue)
// ITaskScheduler::Status()
//      This sets the ReleaseStatus for the current task and the background thread timeout. When
//      ITaskScheduler::RemoveTasks() is called and there is a task currently running that matches
//      ITSSFLAG_COMPLETE_ON_DESTROY will cause TRUE to be passed to the task's IRunnableTask::Kill().
//      The dwThreadTimeout parameter if not set to the default will cause the background thread to
//      die if no new tasks have been added to the queue in the timeout period. The Thread will be
//      recreated when the next new task is added.

////////////////////////
// Status() flags,
// wait for the current task to complete before deleting the scheduler
#define ITSSFLAG_COMPLETE_ON_DESTROY        0x0000

// kill the current task (if there is one) when the task scheduler is deleted
#define ITSSFLAG_KILL_ON_DESTROY            0x0001

#define ITSSFLAG_SUPPORTS_TERMINATE         0x0002

#define ITSSFLAG_FLAGS_MASK                 0x0003

// set the timeout for killing the thread when the object is terminated.
// this timeout can be used to stop the object from blocking the system
// indefinitely.
#define ITSSFLAG_THREAD_TERMINATE_TIMEOUT   0x0010

// set the timeout for threads that are idle in the thread pool
#define ITSSFLAG_THREAD_POOL_TIMEOUT        0x0020

// The default timeout passed to release Status to determine how long the thread
// can be asleep before the thread is expired
#define ITSS_THREAD_DESTROY_DEFAULT_TIMEOUT     (60*1000)

// default, we won't kill it...
#define ITSS_THREAD_TERMINATE_TIMEOUT           (INFINITE)

// there is no change to the thread timeout
#define ITSS_THREAD_TIMEOUT_NO_CHANGE           (INFINITE - 1)

// the LPARAM allows task to be associated with items thus all tasks owned by a
// particular item can be accessed by passing a non default value for this parameter
#define ITSAT_DEFAULT_LPARAM        0xffffffff

// Task priorities
// ---------------
// This depends on the cooperation of tasks currently under execution. New tasks will
// be inserted in the queue in priority order. If a task of a low priority is currently
// under execution when a higher priority task is added, the scheduler will attempt
// to suspend the task currently under execution. It will be resumed when the other tasks
// have been completed.
#define ITSAT_DEFAULT_PRIORITY      0x10000000
#define ITSAT_MAX_PRIORITY          0x7fffffff
#define ITSAT_MIN_PRIORITY          0x00000000

#define TOID_NULL   CLSID_NULL

#undef INTERFACE
#define INTERFACE IShellTaskScheduler

DECLARE_INTERFACE_( IShellTaskScheduler, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // *** IShellTaskScheduler methods ***
    STDMETHOD (AddTask)(THIS_ IRunnableTask * pTask,
               REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               DWORD dwPriority ) PURE;

    STDMETHOD (RemoveTasks)(THIS_ REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               BOOL fWaitIfRunning ) PURE;

    STDMETHOD_(UINT, CountTasks)(THIS_ REFTASKOWNERID rtoid) PURE;

    STDMETHOD (Status)(THIS_ DWORD dwReleaseStatus, DWORD dwThreadTimeout ) PURE;
};

typedef IShellTaskScheduler * LPSHELLTASKSCHEDULER;


// --- IExtractImage
//-------------------------------------------------------------------------
//
// IThumbnail interface
//
//
// [Member functions]
//
// IThumbnail::Init(HWND hwnd, UINT uMsg)
//   Must initialize interface before use.  The hwnd given will receive the
//   uMsg message when the bitmap is computed (cf. GetBitmap()).
//
// IThumbnail::GetBitmap(LPCWSTR pwszFile, DWORD dwItem, LONG lWidth, LONG lHeight)
//   Call this function to actually compute and return the bitmap.  pszFile is
//   the file UNC whose bitmap is to be computed.  lWidth and lHeight are the
//   width and height respectively of the rectangle containing the thumbnail,
//   i.e. the size of the resultant bitmap.  When the bitmap is computed, the
//   uMsg is sent to the hwnd (cf. Init()) where LPARAM is the HBITMAP, and
//   WPARAM is dwItem (so it's an ID to identify the bitmap).
//   NOTE: Call GetBitmap(NULL,...) to cancel any pending requests.
//
//-------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IThumbnail

DECLARE_INTERFACE_(IThumbnail, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    // *** IThumbnail specific methods ***
    STDMETHOD(Init) (THIS_ HWND hwnd, UINT uMsg) PURE;
    STDMETHOD(GetBitmap) (THIS_ LPCWSTR wszFile, DWORD dwItem, LONG lWidth, LONG lHeight) PURE;
};

#endif
#if (_WIN32_IE >= 0x0400)

// Interface used to scale and sharpen images...
// right now, always pass the palette from SHCreateShellPalette on a 256 colour machine...
#undef  INTERFACE
#define INTERFACE   IScaleAndSharpenImage2

DECLARE_INTERFACE_ (IScaleAndSharpenImage2, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // *** IScaleAndSharpenImage methods ***
    STDMETHOD( ScaleSharpen2 ) ( THIS_ BITMAPINFO * pbi,
                                void * pBits,
                                HBITMAP * phBmpThumbnail,
                                const SIZE * prgSize,
                                DWORD dwRecClrDepth,
                                HPALETTE hpal,
                                UINT uiSharpPct,
                                BOOL fOrigSize) PURE;
};

// IImageCache
// Interface for controlling and accessing an imagecache of bitmaps for icons..

// flags for the dwMask member of IMAGECACHEINFO
#define ICIFLAG_LARGE       0x0001
#define ICIFLAG_SMALL       0x0002
#define ICIFLAG_BITMAP      0x0004
#define ICIFLAG_ICON        0x0008
#define ICIFLAG_INDEX       0x0010
#define ICIFLAG_NAME        0x0020
#define ICIFLAG_FLAGS       0x0040
#define ICIFLAG_NOUSAGE     0x0080
#define ICIFLAG_DATESTAMP   0x0100
#define ICIFLAG_MIRROR      0x2000

typedef struct _tagImageCacheInfo
{
    DWORD        cbSize;
    DWORD        dwMask;
    union
    {
        HBITMAP  hBitmapLarge;
        HICON    hIconLarge;
    };
    HBITMAP      hMaskLarge;
    union
    {
        HBITMAP  hBitmapSmall;
        HICON    hIconSmall;
    };
    HBITMAP      hMaskSmall;
    LPCWSTR      pszName;
    int          iIndex;
    DWORD        dwFlags;
#if (_WIN32_IE >= 0x0500)
    FILETIME     ftDateStamp;
#endif
} IMAGECACHEINFO;

typedef IMAGECACHEINFO * LPIMAGECACHEINFO;
typedef const IMAGECACHEINFO * LPCIMAGECACHEINFO;

#define ICIIFLAG_LARGE          0x0001
#define ICIIFLAG_SMALL          0x0002

typedef struct _tagImageCacheInitInfo
{
    DWORD cbSize;
    DWORD dwMask;
    HIMAGELIST himlLarge;
    HIMAGELIST himlSmall;
    SIZEL rgSizeLarge;
    SIZEL rgSizeSmall;
    DWORD dwFlags;
    int   iStart;
    int   iGrow;
}IMAGECACHEINITINFO;

typedef IMAGECACHEINITINFO * LPIMAGECACHEINITINFO;

#undef  INTERFACE
#define INTERFACE   IImageCache

DECLARE_INTERFACE_ ( IImageCache, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // *** IImageCache methods ***
    STDMETHOD ( AddImage ) ( THIS_ LPCIMAGECACHEINFO pInfo, UINT * puIndex ) PURE;
    STDMETHOD ( FindImage ) ( THIS_ LPCIMAGECACHEINFO pInfo, UINT * puIndex ) PURE;
    STDMETHOD ( FreeImage ) ( THIS_ UINT iImageIndex ) PURE;
    STDMETHOD ( Flush )( THIS_ BOOL fRelease ) PURE;
    STDMETHOD ( ChangeImageInfo ) ( THIS_ UINT IImageIndex, LPCIMAGECACHEINFO pInfo ) PURE;
    STDMETHOD ( GetCacheSize ) ( THIS_ UINT * puSize ) PURE;
    STDMETHOD ( GetUsage ) ( THIS_ UINT uIndex, UINT * puUsage ) PURE;

    STDMETHOD( GetImageList ) ( THIS_ LPIMAGECACHEINITINFO pInfo ) PURE;
};
typedef IImageCache * LPIMAGECACHE;

#if (_WIN32_IE >= 0x0500)

// IImageCache 2

#undef  INTERFACE
#define INTERFACE  IImageCache2

DECLARE_INTERFACE_ ( IImageCache2, IImageCache )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // *** IImageCache methods ***
    STDMETHOD ( AddImage ) ( THIS_ LPCIMAGECACHEINFO pInfo, UINT * puIndex ) PURE;
    STDMETHOD ( FindImage ) ( THIS_ LPCIMAGECACHEINFO pInfo, UINT * puIndex ) PURE;
    STDMETHOD ( FreeImage ) ( THIS_ UINT iImageIndex ) PURE;
    STDMETHOD ( Flush )( THIS_ BOOL fRelease ) PURE;
    STDMETHOD ( ChangeImageInfo ) ( THIS_ UINT IImageIndex, LPCIMAGECACHEINFO pInfo ) PURE;
    STDMETHOD ( GetCacheSize ) ( THIS_ UINT * puSize ) PURE;
    STDMETHOD ( GetUsage ) ( THIS_ UINT uIndex, UINT * puUsage ) PURE;

    STDMETHOD( GetImageList ) ( THIS_ LPIMAGECACHEINITINFO pInfo ) PURE;

    // IImageCache2 methods
    STDMETHOD ( DeleteImage ) ( THIS_ UINT iImageIndex ) PURE;
    STDMETHOD ( GetImageInfo ) (THIS_ UINT iImageIndex, LPIMAGECACHEINFO pInfo ) PURE;
};

#endif

#if ( _WIN32_IE >= 0x0500 )
///////////////////////////////////////////////////////////////////////
// interface for
typedef struct _EnumImageStoreDATAtag
{
    WCHAR     szPath[MAX_PATH];
    FILETIME  ftTimeStamp;
} ENUMSHELLIMAGESTOREDATA, * PENUMSHELLIMAGESTOREDATA;

#undef  INTERFACE
#define INTERFACE   IEnumShellImageStore

DECLARE_INTERFACE_( IEnumShellImageStore, IUnknown )
{
    STDMETHOD ( QueryInterface ) ( THIS_ REFIID riid, void **ppv ) PURE;
    STDMETHOD_( ULONG, AddRef ) ( THIS ) PURE;
    STDMETHOD_( ULONG, Release ) ( THIS ) PURE;

    STDMETHOD ( Reset ) ( THIS ) PURE;
    STDMETHOD ( Next ) ( THIS_ ULONG celt, PENUMSHELLIMAGESTOREDATA * prgElt, ULONG * pceltFetched ) PURE;
    STDMETHOD ( Skip ) ( THIS_ ULONG celt ) PURE;
    STDMETHOD ( Clone ) ( THIS_ IEnumShellImageStore ** ppEnum ) PURE;
};

typedef IEnumShellImageStore * LPENUMSHELLIMAGESTORE;


// flags used to determine the capabilities of the storage for the images
#define SHIMSTCAPFLAG_LOCKABLE    0x0001       // does the store require/support locking
#define SHIMSTCAPFLAG_PURGEABLE   0x0002       // does the store require dead items purging externally ?

#undef  INTERFACE
#define INTERFACE   IShellImageStore

// this interface is used to manipulate the Image cache. It can potentially be used
// in a free threaded manner in conjunction with the Lock parameter to Open and close
DECLARE_INTERFACE_( IShellImageStore, IUnknown )
{
    STDMETHOD ( QueryInterface )( THIS_ REFIID riid, void **ppv ) PURE;
    STDMETHOD_( ULONG, AddRef ) ( THIS ) PURE;
    STDMETHOD_( ULONG, Release ) ( THIS ) PURE;

    // if the lock parameter is used, then all other calls into
    // open and/or create will block until the lock is released.
    STDMETHOD ( Open ) ( THIS_ DWORD dwMode, DWORD * pdwLock ) PURE;
    STDMETHOD ( Create ) ( THIS_ DWORD dwMode, DWORD * pdwLock ) PURE;

    // if the lock is passed to either of these two methods, it releases the lock
    // once the operation is complete.
    STDMETHOD ( ReleaseLock ) ( THIS_ DWORD const * pdwLock ) PURE;
    STDMETHOD ( Close ) ( THIS_ DWORD const * pdwLock ) PURE;
    STDMETHOD ( Commit ) ( THIS_ DWORD const * pdwLock ) PURE;
    STDMETHOD ( IsLocked ) ( THIS ) PURE;

    STDMETHOD ( GetMode ) ( THIS_ DWORD * pdwMode ) PURE;
    STDMETHOD ( GetCapabilities ) ( THIS_ DWORD * pdwCapMask ) PURE;

    STDMETHOD ( AddEntry ) ( THIS_ LPCWSTR pszName, const FILETIME * pftTimeStamp, DWORD dwMode, HBITMAP hImage ) PURE;
    STDMETHOD ( GetEntry ) ( THIS_ LPCWSTR pszName, DWORD dwMode, HBITMAP * phImage ) PURE;
    STDMETHOD ( DeleteEntry ) ( THIS_ LPCWSTR pszName ) PURE;
    STDMETHOD ( IsEntryInStore ) ( THIS_ LPCWSTR pszName, FILETIME * pftTimeStamp ) PURE;

    STDMETHOD ( Enum ) ( THIS_ LPENUMSHELLIMAGESTORE * ppEnum ) PURE;
};

typedef IShellImageStore * LPSHELLIMAGESTORE;
#endif
//
// Interface for interacting with the IE splash screen
//

#undef  INTERFACE
#define INTERFACE   ISplashScreen

DECLARE_INTERFACE_( ISplashScreen, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // *** ISplashScreen methods ***
    STDMETHOD ( Show ) ( THIS_ HINSTANCE hinst, UINT idResHi, UINT idResLow, HWND * phwnd ) PURE;
    STDMETHOD ( Dismiss ) (THIS) PURE;
};

typedef ISplashScreen * LPSPLASHSCREEN;

//
// Interface for Multiple Monitor Configurations
//

#define MD_PRIMARY  0x00000001
#define MD_ATTACHED 0x00000002
typedef struct _MonitorData
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwStatus;
    RECT rcPos;
} MonitorData, * LPMONITORDATA;
#undef  INTERFACE
#define INTERFACE   IMultiMonConfig

DECLARE_INTERFACE_( IMultiMonConfig, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // *** IMultiMonConfig methods ***
    STDMETHOD ( Initialize ) ( THIS_ HWND hwndHost, WNDPROC WndProc, DWORD dwReserved) PURE;
    STDMETHOD ( GetNumberOfMonitors ) (THIS_ int * pCMon, DWORD dwReserved) PURE;
    STDMETHOD ( GetMonitorData) (THIS_ int iMonitor, MonitorData * pmd, DWORD dwReserved) PURE;
    STDMETHOD ( Paint) (THIS_ int iMonitor, DWORD dwReserved) PURE;
};

typedef IMultiMonConfig * LPMULTIMONCONFIG;

#endif // _WIN32_IE > 0x0400

////  IRestrict
// For the pdwRestrictionResult out parameter in IRestrict::IsRestricted() for
// all pguidIDs.
typedef enum
{
    RR_ALLOW  = 1,
    RR_DISALLOW,
    RR_NOCHANGE,
} RESTRICTION_RESULT;

// IRestrict::IsRestricted() dwRestrictAction parameter values for
// the RID_RDeskBars pguidID.
typedef enum
{
    RA_DRAG  = 1,
    RA_DROP,
    RA_ADD,
    RA_CLOSE,
    RA_MOVE,
} RESTRICT_ACTIONS;

#undef INTERFACE
#define INTERFACE IRestrict

DECLARE_INTERFACE_(IRestrict, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IRestrict Methods ***
    STDMETHOD(IsRestricted) (THIS_ const GUID * pguidID, DWORD dwRestrictAction, VARIANT * pvarArgs, OUT DWORD * pdwRestrictionResult) PURE;
};


////  IAddressBand
#define OLECMD_REFRESH_TOPMOST     0x00000000
#define OLECMD_REFRESH_ENTIRELIST  0x00000001

#undef INTERFACE
#define INTERFACE IAddressBand

DECLARE_INTERFACE_(IAddressBand, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IAddressBand Methods ***
    STDMETHOD(FileSysChange) (THIS_ DWORD dwEvent, LPCITEMIDLIST * ppidl1) PURE;
    STDMETHOD(Refresh) (THIS_ VARIANT * pvarType) PURE;
};

#undef INTERFACE
#define INTERFACE IAddressEditBox

// Init Flags
#define AEB_INIT_DEFAULT     0x00000000
#define AEB_INIT_NOSHELL     0x00000001
#define AEB_INIT_NOWEB       0x00000002
#define AEB_INIT_NOFILE      0x00000004
#define AEB_INIT_AUTOEXEC    0x00000008     // Set if you want the item Navigated/ShellExeced when ENTER occurs.
#define AEB_INIT_SUBCLASS    0x00000010     // If set parent needs to pass HWND events thru IWinEventHandler::OnWinEvent()

// Parse Flags for IAddressEditBox::ParseNow()
#define SHURL_FLAGS_NONE        0x00000000
#define SHURL_FLAGS_NOUI        0x00000001      // Don't display any UI.
#define SHURL_FLAGS_NOSNS       0x00000002      // Ignore SHELL URLs. (FILE and Internet URLs still work)
#define SHURL_FLAGS_NOWEB       0x00000004      // Ignore WEB URLs. (Don't successfully parse if not found in shell name space)
#define SHURL_FLAGS_NOPATHSEARCH 0x00000008     // don't search on dos paths or app path
#define SHURL_FLAGS_AUTOCORRECT 0x00000010      // Try to autocorrect WEB URLs

// Exec Flags for IAddressEditBox::Execute()
#define SHURL_EXECFLAGS_NONE        0x00000000
#define SHURL_EXECFLAGS_SEPVDM      0x00000001      // If ShellExec(), set SEE_MASK_FLAG_SEPVDM fMask.
#define SHURL_EXECFLAGS_DONTFORCEIE 0x00000002      // If some browser other than IE is associated with HTML files, allow them to launch.


DECLARE_INTERFACE_(IAddressEditBox, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IAddressEditBox Methods ***
    STDMETHOD(Init) (THIS_ HWND hwnd, HWND hwndEditBox, DWORD dwFlags, IUnknown * punkParent) PURE;
    STDMETHOD(SetCurrentDir) (THIS_ LPCOLESTR pwzDir) PURE;
    STDMETHOD(ParseNow) (THIS_ DWORD dwParseFlags) PURE;
    STDMETHOD(Execute) (THIS_ DWORD dwExecFlags) PURE;
    STDMETHOD(Save) (THIS_ DWORD dwReserved) PURE;
};


#if (_WIN32_IE >= 0x0400)

////  IShellFolderBand

// Field mask
#define ISFB_MASK_STATE          0x00000001 // TRUE if dwStateMask and dwState is valid
#define ISFB_MASK_BKCOLOR        0x00000002 // TRUE if crBkgnd field is valid
#define ISFB_MASK_VIEWMODE       0x00000004 // TRUE if wViewMode field is valid
#define ISFB_MASK_SHELLFOLDER    0x00000008
#define ISFB_MASK_IDLIST         0x00000010
#define ISFB_MASK_COLORS         0x00000020 // TRUE if crXXXX fields are valid (except bkgnd)
#define ISFB_MASK_ALL            0x0000003F         //
#define ISFB_MASK_INVALID        (~(ISFB_MASK_ALL)) //

#define ISFB_STATE_DEFAULT       0x00000000
#define ISFB_STATE_DEBOSSED      0x00000001
#define ISFB_STATE_ALLOWRENAME   0x00000002
#define ISFB_STATE_NOSHOWTEXT    0x00000004 // TRUE if _fNoShowText
//#define ISFB_STATE_NOTITLE       0x00000008 // obsolete! (use cbs::SetBandState)
#define ISFB_STATE_CHANNELBAR    0x00000010 // TRUE if we want NavigateTarget support
#define ISFB_STATE_QLINKSMODE    0x00000020 // TRUE if we want to turn off drag & drop onto content items
#define ISFB_STATE_FULLOPEN      0x00000040 // TRUE if band should maximize when opened
#define ISFB_STATE_NONAMESORT    0x00000080 // TRUE if band should _not_ sort icons by name
#define ISFB_STATE_BTNMINSIZE    0x00000100 // TRUE if band should report min thickness of button
#define ISFB_STATE_ALL           0x000001FF //

#define ISFBVIEWMODE_SMALLICONS   0x0001
#define ISFBVIEWMODE_LARGEICONS   0x0002
#define ISFBVIEWMODE_LOGOS        0x0003

typedef struct {
    DWORD       dwMask;       // [in] ISFB_MASK mask of valid fields from crBkgnd on
    DWORD       dwStateMask;  // [in] ISFB_STATE mask of dwState bits being set/queried
    DWORD       dwState;      // [in/out] ISFB_STATE bits
    COLORREF    crBkgnd;      // [in/out]
    COLORREF    crBtnLt;      // [in/out]
    COLORREF    crBtnDk;      // [in/out]
    WORD        wViewMode;    // [in/out]
    WORD        wAlign;       // not used (yet)
    IShellFolder * psf;       // [out]
    LPITEMIDLIST   pidl;      // [out]
} BANDINFOSFB, *PBANDINFOSFB;

#undef INTERFACE
#define INTERFACE IShellFolderBand

DECLARE_INTERFACE_(IShellFolderBand, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderBand Methods ***
    STDMETHOD(InitializeSFB)(THIS_ IShellFolder *psf, LPCITEMIDLIST pidl) PURE;
    STDMETHOD(SetBandInfoSFB)(THIS_ PBANDINFOSFB pbi) PURE;
    STDMETHOD(GetBandInfoSFB)(THIS_ PBANDINFOSFB pbi) PURE;
};

// Command Target IDs
enum {
    SFBID_PIDLCHANGED,
};

//-------------------------------------------------------------------------
//
// IWinEventHandler interface
//
//   An object that wants to send messages to another object (typically
// a sub-object) QI's for the sub-object's IWinEventHandler interface.
// Once the object determines the hwnd is owned by the sub-object (via
// IsWindowOwner), the object calls OnWinEvent for send the message.
//
//
// [Member functions]
//
// IWinEventHandler::IsWindowOwner(hwnd)
//   Returns S_OK if the hwnd is owned by the object, S_FALSE if not.
//
// IWinEventHandler::OnWinEvent(hwnd, dwMsg, wParam, lParam, plRet)
//   Sends a message to the object.  The Win32 return value (i.e., the
//   value returned by DefWindowProc) is returned in *plRet.  Return
//   S_OK if the message is handled, otherwise S_FALSE.
//
//-------------------------------------------------------------------------


#undef INTERFACE
#define INTERFACE IWinEventHandler

DECLARE_INTERFACE_(IWinEventHandler, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IWinEventHandler Methods ***
    STDMETHOD(OnWinEvent) (THIS_ HWND hwnd, UINT dwMsg, WPARAM wParam, LPARAM lParam, LRESULT* plres) PURE;
    STDMETHOD(IsWindowOwner) (THIS_ HWND hwnd) PURE;
};


////  IDeskBarClient

#undef  INTERFACE
#define INTERFACE   IDeskBarClient

DECLARE_INTERFACE_(IDeskBarClient, IOleWindow)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

    // *** IDeskBarClient methods ***
    STDMETHOD(SetDeskBarSite)   (THIS_ IUnknown* punkSite) PURE;
    STDMETHOD(SetModeDBC)          (THIS_ DWORD dwMode) PURE;
    STDMETHOD(UIActivateDBC)    (THIS_ DWORD dwState) PURE;
    STDMETHOD(GetSize)    (THIS_ DWORD dwWhich, LPRECT prc) PURE;
};

#define DBC_GS_IDEAL    0  // get the ideal size

#define DBC_HIDE        0 // Band is hidden (being destroyed)
#define DBC_SHOW        1 // Band is visible
#define DBC_SHOWOBSCURE 2 // Band is completely obscured


enum {
    DBCID_EMPTY = 0,        // bandsite is empty
    DBCID_ONDRAG = 1,       // (down)DragMoveEnter/Leave vaIn:I4:eDrag
    DBCID_CLSIDOFBAR = 2,   // clsid of bar inside
    DBCID_RESIZE = 3,		// resize from keyboard
};

///// IDeskBar

#undef  INTERFACE
#define INTERFACE   IDeskBar

DECLARE_INTERFACE_(IDeskBar, IOleWindow)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

    // *** IDeskBarClient methods ***
    STDMETHOD(SetClient)          (THIS_ IUnknown* punkClient) PURE;
    STDMETHOD(GetClient)          (THIS_ IUnknown** ppunkClient) PURE;
    STDMETHOD(OnPosRectChangeDB)(THIS_ LPRECT prc) PURE;
};


//-------------------------------------------------------------------------
//
// IBandSite interface
//
//   This interface provides methods to get or set bandsite information.
//
// [Member functions]
//
// IBandSite::AddBand(punk)
//   Add a band to the bandsite.  Returns the band ID in ShortFromResult(hres).
//
// IBandSite::EnumBands(uBand, *pdwBandID)
//   Enumerate the bands. If uBand is -1, pdwBandID is ignored and this
//   method returns the count of bands in the bandsite.  Call this method
//   with uBand starting at 0 to begin enumerating.  Returns S_OK and the
//   band ID in *pdwBandID of the next band.
//
// IBandSite::QueryBand(dwBandID, ppstb, pdwState, pszName, cchName)
//   Get info about a band.
//
// IBandSite::SetBandState(dwBandID, dwState)
//   Set the band's state.
//   ;BUGBUG (scotth): no one implements this right now!
//
// IBandSite::RemoveBand(dwBandID)
//   Remove the band.
//
// IBandSite::GetBandObject(dwBandID, riid, ppv)
//   Get an object that support riid for the band.
//
// IBandSite::GetBandSiteInfo(pbsinfo)
//   Get info about the bandsite.
//
// IBandSite::SetBandSiteInfo(pbsinfo)
//   Set info about the bandsite.
//
//-------------------------------------------------------------------------

// Field mask
#define BSIM_STATE          0x00000001
#define BSIM_STYLE          0x00000002

// State flags
#define BSSF_VISIBLE        0x00000001
#define BSSF_NOTITLE        0x00000002
#define BSSF_UNDELETEABLE   0x00001000  // OBSOLETE

// Style flags
#define BSIS_AUTOGRIPPER    0x00000000
#define BSIS_NOGRIPPER      0x00000001
#define BSIS_ALWAYSGRIPPER  0x00000002
#define BSIS_LEFTALIGN      0x00000004
#define BSIS_SINGLECLICK    0x00000008
#define BSIS_NOCONTEXTMENU  0x00000010
#define BSIS_NODROPTARGET   0x00000020

typedef struct {
    DWORD       dwMask;         // BSIM_* flags
    DWORD       dwState;        // BSSF_* flags
    DWORD       dwStyle;        // BSIS_* flags
} BANDSITEINFO;

// Passed to parent and child context menus in
// lpParameters field of CMINVOKECOMMANDINFO
typedef struct {
    DWORD       dwID;
    IUnknown *  punkSite;
} BANDSITEINVOKEPARAM;

#undef  INTERFACE
#define INTERFACE   IBandSite

DECLARE_INTERFACE_(IBandSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IBandSite methods ***
    STDMETHOD(AddBand)          (THIS_ IUnknown* punk) PURE;
    STDMETHOD(EnumBands)        (THIS_ UINT uBand, DWORD* pdwBandID) PURE;
    STDMETHOD(QueryBand)        (THIS_ DWORD dwBandID, IDeskBand** ppstb, DWORD* pdwState, LPWSTR pszName, int cchName) PURE;
    STDMETHOD(SetBandState)     (THIS_ DWORD dwBandID, DWORD dwMask, DWORD dwState) PURE;
    STDMETHOD(RemoveBand)       (THIS_ DWORD dwBandID) PURE;
    STDMETHOD(GetBandObject)    (THIS_ DWORD dwBandID, REFIID riid, void **ppv) PURE;
    STDMETHOD(SetBandSiteInfo)  (THIS_ const BANDSITEINFO * pbsinfo) PURE;
    STDMETHOD(GetBandSiteInfo)  (THIS_ BANDSITEINFO * pbsinfo) PURE;
};

// BSID_BANDREMOVED means removed *or* hidden
enum {
    BSID_BANDADDED,
    BSID_BANDREMOVED,
};
#endif // _WIN32_IE > 0x0400


//==========================================================================
// IExplorerToolbar interfaces
//
//  This is an interface to the shell toolbar which allows shell views
// to add buttons and process.
// See http://ohserv/users/satona/toolbar_extension.htm for spec.
//
//==========================================================================

// Bitmap types
#define BITMAP_NORMAL         0x1
#define BITMAP_HOT            0x2
#define BITMAP_DISABLED       0x3

// Band Visibility Flags
#define VBF_TOOLS               0x00000001
#define VBF_ADDRESS             0x00000002
#define VBF_LINKS               0x00000004
#define VBF_BRAND               0x00000008
#define VBF_MENU                0x00000010

#define VBF_ONELINETEXT         0x00000020
#define VBF_TWOLINESTEXT        0x00000040
#define VBF_NOCUSTOMIZE         0x00000080

#define VBF_EXTERNALBANDS       0xFFFF0000

#define VBF_VALID               (VBF_TOOLS | VBF_ADDRESS | VBF_LINKS | VBF_BRAND | VBF_MENU | VBF_EXTERNALBANDS)

#undef  INTERFACE
#define INTERFACE   IExplorerToolbar

typedef enum {
    ETCMDID_GETBUTTONS          = 0x0001,
    ETCMDID_NEWCOMMANDTARGET    = 0x0002,   // sent to the previous command target when a new one is coming in
#if (_WIN32_IE >= 0x500)
    ETCMDID_RELOADBUTTONS       = 0x0003,   // command target needs to call AddButtons again
#endif
} ENUM_ETCMDID;

DECLARE_INTERFACE_(IExplorerToolbar, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IExplorerToolbar methods ***
    STDMETHOD(SetCommandTarget) (THIS_ IUnknown* punkCmdTarget, const GUID* pguidCmdGrp, DWORD dwFlags) PURE;
    STDMETHOD(AddStdBrowserButtons) (THIS) PURE;

    // Wrap of the TB_* messages the messages that are implmented
    // are just the ones to add delete and modify the buttons, and not
    // the ones to manipulate the toolbar.
    STDMETHOD(AddButtons)       (THIS_ const GUID * pguidButtonGroup, UINT nButtons, const TBBUTTON * lpButtons) PURE;
    STDMETHOD(AddString)        (THIS_ const GUID * pguidButtonGroup, HINSTANCE hInst, UINT_PTR uiResID, LRESULT * pOffset) PURE;
    STDMETHOD(GetButton)        (THIS_ const GUID * pguidButtonGroup, UINT uiCommand, LPTBBUTTON lpButton) PURE;
    STDMETHOD(GetState)         (THIS_ const GUID * pguidButtonGroup, UINT uiCommand, UINT * pfState) PURE;
    STDMETHOD(SetState)         (THIS_ const GUID * pguidButtonGroup, UINT uiCommand, UINT fState) PURE;
    STDMETHOD(AddBitmap)        (THIS_ const GUID * pguidButtonGroup, UINT uiBMPType, UINT uiCount, TBADDBITMAP * ptb,
                                                        LRESULT * pOffset, COLORREF rgbMask) PURE;
    STDMETHOD(GetBitmapSize)    (THIS_ UINT * uiID) PURE;
    STDMETHOD(SendToolbarMsg)   (THIS_ const GUID * pguidButtonGroup, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT * plRes) PURE;

    STDMETHOD(SetImageList) (THIS_ const GUID* pguidCmdGroup, HIMAGELIST himlNormal, HIMAGELIST himlHot, HIMAGELIST himlDisabled) PURE;
    STDMETHOD(ModifyButton)     (THIS_ const GUID * pguidButtonGroup, UINT uiCommand, LPTBBUTTON lpButton) PURE;
};

//==========================================================================
// DefView supports a private service id for extended views.
// DefView's IOleCommandTarget supports the below command group and ids too.
//==========================================================================
// SID_DefView and CGID_DefView are defined in shlguid.w
//
#define DVCMDID_GETTHISDIRPATH 1 // pvaOut = path to this directory (if in filesystem)
#define DVCMDID_GETTHISDIRNAME 2 // pvaOut = friendly name of this directory
#define DVCMDID_GETTEMPLATEDIRNAME 3 // pvaOut = webview template (.htt) dir
#define IS_ADJUSTLISTVIEW       0x80000000
#define IS_INTERNALDUMMYBIT     0x40000000
#define IS_VALIDINTERNALBITS    (IS_ADJUSTLISTVIEW | IS_INTERNALDUMMYBIT)

// Flags for SetSafeMode
#define SSM_CLEAR   0x0000
#define SSM_SET     0x0001
#define SSM_REFRESH 0x0002
#define SSM_UPDATE  0x0004

// Flags for Set/GetScheme
#define SCHEME_DISPLAY          0x0001
#define SCHEME_EDIT             0x0002
#define SCHEME_LOCAL            0x0004
#define SCHEME_GLOBAL           0x0008
#define SCHEME_REFRESH          0x0010
#define SCHEME_UPDATE           0x0020
#define SCHEME_DONOTUSE 0x0040 // used to be SCHEME_ENUMERATE; no longer supported
#define SCHEME_CREATE           0x0080

#undef INTERFACE
#define INTERFACE IActiveDesktopP

DECLARE_INTERFACE_( IActiveDesktopP, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // IActiveDesktopP methods
    STDMETHOD (SetSafeMode)(THIS_ DWORD dwFlags) PURE;
    STDMETHOD (EnsureUpdateHTML)(THIS) PURE;
    STDMETHOD (SetScheme)(THIS_ LPCWSTR pwszSchemeName, DWORD dwFlags) PURE;
    STDMETHOD (GetScheme)(THIS_ LPWSTR pwszSchemeName, DWORD *lpdwcchBuffer, DWORD dwFlags) PURE;
    //
};

typedef IActiveDesktopP * LPACTIVEDESKTOPP;

//Flags for GetObjectFlags
#define GADOF_DIRTY    0x00000001

#undef INTERFACE
#define INTERFACE IADesktopP2

DECLARE_INTERFACE_( IADesktopP2, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // IADesktopP2 methods
    STDMETHOD (ReReadWallpaper)(THIS) PURE;
    STDMETHOD (GetADObjectFlags)(THIS_ DWORD *lpdwFlags, DWORD dwMask) PURE;
    STDMETHOD (UpdateAllDesktopSubscriptions)(THIS) PURE;
    STDMETHOD (MakeDynamicChanges)(THIS_ IOleObject *pOleObj) PURE;
};

typedef IADesktopP2 * LPADESKTOPP2;

#define CFSTR_SHELLIDLISTP      TEXT("Shell IDLData Private")
#define CFSTR_SHELLCOPYDATA     TEXT("Shell Copy Data")
//
// Win 3.1 style HDROP
//
//  Notes: Our API works only if pFiles == sizeof(DROPFILES16)
//
typedef struct _DROPFILES16 {
    WORD pFiles;                // offset to double null list of files
    POINTS pt;                  // drop point (client coords)
    WORD fNC;                   // is it on non client area
                                // and pt is in screen coords
} DROPFILES16, * LPDROPFILES16;

//
// format of CF_SHELLCOPYDATA
//

typedef struct _SHELLCOPYDATA {
    DWORD dwEffect;                 // Intended effect
} SHELLCOPYDATA;
//------ See shelldll\fsnotify.c for function descriptions. ----------

//
//  Definition of the function type to be called by the notification
//  service when a file the client has registered to monitor changes.
//

typedef struct _SHChangeNotifyEntry
{
    LPCITEMIDLIST pidl;
    BOOL   fRecursive;
} SHChangeNotifyEntry;
#define SHCNRF_InterruptLevel      0x0001
#define SHCNRF_ShellLevel          0x0002

// If the caller passes SHCNRF_RecursiveInterrupt (meaning that they want interrupt events on the whole
// subtree), then they must also pass SHCNRF_InterruptLevel flag (duh), and they must be a recursive (eg
// fRecursive == TRUE) type notification.
#define SHCNRF_RecursiveInterrupt  0x1000
#define SHCNRF_ResumeThread        0x2000
#define SHCNRF_CreateSuspended     0x4000
#define SHCNRF_NewDelivery         0x8000

// !! WARNING !!
// If you change/add any new SHCNE_ events, you will need
// to possibly modify the SHCNE_XXXXEVENTS masks that are
// defined below as well.
// NOTE: SHCNE_GLOBALEVENTS are typically used to pass
// non-pidl information (packed into pidl format). We cannot
// extend SHCNE_GLOBALEVENTS and expect clients of the new
// events to work on a down-level shell32. In fact, they
// may very well fault during psfDesktop->CompareIDs.
// So be careful when adding to this bitfield!
//
// Win95 and NT4 and IE4 shipped with 0x0C0581E0L.
// We use this event mask to tell if we need to possibly throw out a SHCNE_UPDATEDIR that was
// generated by the filesystem because we have a more specific event already in our queue.
// NOTE: only non-interrupt events could cause us to throw out a interrupt generated UPDATEDIR event.
#define SHCNE_UPDATEDIR_OVERRIDE_EVENTS 0x00023818L
// This event was defined early on in IE4 for the Theme Switcher to
// notify the shell that a system-wide update needed to happen.
// This has been removed but I don't want to re-use the id just yet...

// BUGBUG (scotth) 6/11/98: can we remove this yet??

//#define SHCNEE_THEMECHANGED       1L  // The theme changed
#define SHCNEE_WININETCHANGED       3L  // dwItem2 contains CACHE_NOTIFY_* bits
#define SHCNEE_PROMOTEDITEM         6L  // pidl2 is the pidl of the Promoted item
#define SHCNEE_DEMOTEDITEM          7L  // pidl2 is the pidl of the demoted item
#define SHCNEE_ALIASINUSE           8L  // some alias style folder is now being used
// Update types for the UpdateEntryList api
#define SHCNNU_SET        1   // Set the notify list to passed in list
#define SHCNNU_ADD        2   // Add the items to the current list
#define SHCNNU_REMOVE     3   // Remove the items from the current list
#define SHCNF_PRINTJOBA   0x0004        // dwItem1: printer name
                                        // dwItem2: SHCNF_PRINTJOB_DATA
#define SHCNF_PRINTJOBW   0x0007        // dwItem1: printer name
                                        // dwItem2: SHCNF_PRINTJOB_DATA
#define SHCNF_INSTRUMENT  0x0080        // dwItem1: LPSHCNF_INSTRUMENT
#define SHCNF_NONOTIFYINTERNALS     0x4000 // means don't do shell notify internals.  see comments in code
#define SHCNF_ONLYNOTIFYINTERNALS   0x8000 // only notify internal clients
#ifdef UNICODE
#define SHCNF_PRINTJOB  SHCNF_PRINTJOBW
#else
#define SHCNF_PRINTJOB  SHCNF_PRINTJOBA
#endif

typedef struct tagSHCNF_PRINTJOB_DATA {
    DWORD JobId;
    DWORD Status;
    DWORD TotalPages;
    DWORD Size;
    DWORD PagesPrinted;
} SHCNF_PRINTJOB_DATA, *LPSHCNF_PRINTJOB_DATA;

//
// This is all the INSTRUMENTation stuff...
// make this look like an ITEMIDLIST (uOffset points to 0 uTerm)
#include <pshpack1.h>
typedef struct tagSHCNF_INSTRUMENT {
    USHORT uOffset;
    USHORT uAlign;
    DWORD dwEventType;
    DWORD dwEventStructure;
    SYSTEMTIME st;
    union tagEvents {
        struct tagSTRING {
            TCHAR sz[32];
        } string;
        struct tagHOTKEY {
            WPARAM wParam;
        } hotkey;
        struct tagWNDPROC {
            HWND hwnd;
            UINT uMsg;
            WPARAM wParam;
            LPARAM lParam;
        } wndproc;
        struct tagCOMMAND {
            HWND hwnd;
            UINT idCmd;
        } command;
        struct tagDROP {
            HWND hwnd;
            UINT idCmd;
//          TCHAR sz[32]; // convert pDataObject into something we can log
        } drop;
    } e;
    USHORT uTerm;
} SHCNF_INSTRUMENT_INFO, * LPSHCNF_INSTRUMENT_INFO;
#include <poppack.h>

#define SHCNFI_EVENT_STATECHANGE          0   // dwEventType
#define SHCNFI_EVENT_STRING               1   // e.string
#define SHCNFI_EVENT_HOTKEY               2   // e.hotkey
#define SHCNFI_EVENT_WNDPROC              3   // e.wndproc
#define SHCNFI_EVENT_WNDPROC_HOOK         4   // e.wndproc
#define SHCNFI_EVENT_ONCOMMAND            5   // e.command
#define SHCNFI_EVENT_INVOKECOMMAND        6   // e.command
#define SHCNFI_EVENT_TRACKPOPUPMENU       7   // e.command
#define SHCNFI_EVENT_DROP                 8   // e.drop
#define SHCNFI_EVENT_MAX                  9

#define SHCNFI_STRING_SHOWEXTVIEW         0

#define SHCNFI_STATE_KEYBOARDACTIVE         0   // _KEYBOARDACTIVE or _MOUSEACTIVE
#define SHCNFI_STATE_MOUSEACTIVE            1   // _KEYBOARDACTIVE or _MOUSEACTIVE
#define SHCNFI_STATE_ACCEL_TRAY             2   // _ACCEL_TRAY or _ACCEL_DESKTOP
#define SHCNFI_STATE_ACCEL_DESKTOP          3   // _ACCEL_TRAY or _ACCEL_DESKTOP
#define SHCNFI_STATE_START_DOWN             4   // _START_DOWN or _START_UP
#define SHCNFI_STATE_START_UP               5   // _START_DOWN or _START_UP
#define SHCNFI_STATE_TRAY_CONTEXT           6
#define SHCNFI_STATE_TRAY_CONTEXT_CLOCK     7
#define SHCNFI_STATE_TRAY_CONTEXT_START     8
#define SHCNFI_STATE_DEFVIEWX_ALT_DBLCLK    9
#define SHCNFI_STATE_DEFVIEWX_SHIFT_DBLCLK 10
#define SHCNFI_STATE_DEFVIEWX_DBLCLK       11

#define SHCNFI_GLOBALHOTKEY               0

#define SHCNFI_CABINET_WNDPROC            0
#define SHCNFI_DESKTOP_WNDPROC            1
#define SHCNFI_PROXYDESKTOP_WNDPROC       2
#define SHCNFI_TRAY_WNDPROC               3
#define SHCNFI_DRIVES_WNDPROC             4
#define SHCNFI_ONETREE_WNDPROC            5
#define SHCNFI_MAIN_WNDPROC               6
#define SHCNFI_FOLDEROPTIONS_DLGPROC      7
#define SHCNFI_VIEWOPTIONS_DLGPROC        8
#define SHCNFI_FT_DLGPROC                 9
#define SHCNFI_FTEdit_DLGPROC            10
#define SHCNFI_FTCmd_DLGPROC             11
#define SHCNFI_TASKMAN_DLGPROC           12
#define SHCNFI_TRAYVIEWOPTIONS_DLGPROC   13
#define SHCNFI_INITSTARTMENU_DLGPROC     14
#define SHCNFI_PRINTERQUEUE_DLGPROC      15

#define SHCNFI_CABINET_ONCOMMAND          0
#define SHCNFI_TRAYCOMMAND                1

#define SHCNFI_BITBUCKET_DFM_INVOKE       0
#define SHCNFI_BITBUCKET_FNV_INVOKE       1
#define SHCNFI_BITBUCKET_INVOKE           2
#define SHCNFI_BITBUCKETBG_DFM_INVOKE     3
#define SHCNFI_CONTROLS_DFM_INVOKE        4
#define SHCNFI_CONTROLS_FNV_INVOKE        5
#define SHCNFI_CONTROLSBG_DFM_INVOKE      6
#define SHCNFI_DEFFOLDER_DFM_INVOKE       7
#define SHCNFI_DEFFOLDER_INVOKE           8
#define SHCNFI_FINDEXT_INVOKE             9
#define SHCNFI_DEFFOLDER_FNV_INVOKE      10
#define SHCNFI_DRIVESBG_DFM_INVOKE       11
#define SHCNFI_DRIVES_FNV_INVOKE         12
#define SHCNFI_DRIVES_DFM_INVOKE         13
#define SHCNFI_FOLDERBG_DFM_INVOKE       14
#define SHCNFI_FOLDER_FNV_INVOKE         15
#define SHCNFI_FOLDER_DFM_INVOKE         16
#define SHCNFI_NETWORKBG_DFM_INVOKE      17
#define SHCNFI_NETWORK_FNV_INVOKE        18
#define SHCNFI_NETWORK_DFM_INVOKE        19
#define SHCNFI_NETWORKPRINTER_DFM_INVOKE 20
#define SHCNFI_DESKTOPBG_DFM_INVOKE      21
#define SHCNFI_DESKTOP_DFM_INVOKE        22
#define SHCNFI_DESKTOP_FNV_INVOKE        23
#define SHCNFI_PRINTERS_DFM_INVOKE       24
#define SHCNFI_PRINTERSBG_DFM_INVOKE     25
#define SHCNFI_PRINTERS_FNV_INVOKE       26
#define SHCNFI_DEFVIEWX_INVOKE           27

#define SHCNFI_FOLDER_DROP                0
#define SHCNFI_PRINTQUEUE_DROP            1
#define SHCNFI_DEFVIEWX_TPM               2
#define SHCNFI_DROP_EXE_TPM               3
#define SHCNFI_IDLDT_TPM                  4

#define SHCNFI_DROP_BITBUCKET             0
#define SHCNFI_DROP_PRINTFOLDER           1
#define SHCNFI_DROP_PRINTER               2
#define SHCNFI_DROP_RUN                   3
#define SHCNFI_DROP_SHELLLINK             4
#define SHCNFI_DROP_DRIVES                5
#define SHCNFI_DROP_FS                    6
#define SHCNFI_DROP_EXE                   7
#define SHCNFI_DROP_NETROOT               8
#define SHCNFI_DROP_PRINTQUEUE            9
#define SHCNFI_DROP_BRIEFCASE            10

#ifdef WANT_SHELL_INSTRUMENTATION
#define INSTRUMENT_STATECHANGE(t)                               \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_STATECHANGE;                \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_STRING(t,p)                                  \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_STRING;                     \
    lstrcpyn(s.e.string.sz,(p),ARRAYSIZE(s.e.string.sz));       \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_HOTKEY(t,w)                                  \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_HOTKEY;                     \
    s.e.hotkey.wParam=(w);                                      \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_WNDPROC(t,h,u,w,l)                           \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_WNDPROC;                    \
    s.e.wndproc.hwnd=(h);                                       \
    s.e.wndproc.uMsg=(u);                                       \
    s.e.wndproc.wParam=(w);                                     \
    s.e.wndproc.lParam=(l);                                     \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_WNDPROC_HOOK(h,u,w,l)                        \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=0;                                            \
    s.dwEventStructure=SHCNFI_EVENT_WNDPROC_HOOK;               \
    s.e.wndproc.hwnd=(h);                                       \
    s.e.wndproc.uMsg=(u);                                       \
    s.e.wndproc.wParam=(w);                                     \
    s.e.wndproc.lParam=(l);                                     \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_ONCOMMAND(t,h,u)                             \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_ONCOMMAND;                  \
    s.e.command.hwnd=(h);                                       \
    s.e.command.idCmd=(u);                                      \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_INVOKECOMMAND(t,h,u)                         \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_INVOKECOMMAND;              \
    s.e.command.hwnd=(h);                                       \
    s.e.command.idCmd=(u);                                      \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_TRACKPOPUPMENU(t,h,u)                        \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_TRACKPOPUPMENU;             \
    s.e.command.hwnd=(h);                                       \
    s.e.command.idCmd=(u);                                      \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#define INSTRUMENT_DROP(t,h,u,p)                                \
{                                                               \
    SHCNF_INSTRUMENT_INFO s;                                    \
    s.dwEventType=(t);                                          \
    s.dwEventStructure=SHCNFI_EVENT_DROP;                       \
    s.e.drop.hwnd=(h);                                          \
    s.e.drop.idCmd=(u);                                         \
    SHChangeNotify(SHCNE_INSTRUMENT,SHCNF_INSTRUMENT,&s,NULL);  \
}
#else
#define INSTRUMENT_STATECHANGE(t)
#define INSTRUMENT_STRING(t,p)
#define INSTRUMENT_HOTKEY(t,w)
#define INSTRUMENT_WNDPROC(t,h,u,w,l)
#define INSTRUMENT_WNDPROC_HOOK(h,u,w,l)
#define INSTRUMENT_ONCOMMAND(t,h,u)
#define INSTRUMENT_INVOKECOMMAND(t,h,u)
#define INSTRUMENT_TRACKPOPUPMENU(t,h,u)
#define INSTRUMENT_DROP(t,h,u,p)
#endif //WANT_SHELL_INSTRUMENTATION
//
//
// IQueryCodePage
//
#undef  INTERFACE
#define INTERFACE  IQueryCodePage

DECLARE_INTERFACE_(IQueryCodePage, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IQueryCodePage methods ***
    STDMETHOD(GetCodePage)(THIS_ UINT * puiCodePage) PURE;
    STDMETHOD(SetCodePage)(THIS_ UINT uiCodePage) PURE;
} ;
SHSTDAPI_(void) ReceiveAddToRecentDocs(HANDLE hARD, DWORD dwProcId);

SHSTDAPI_(void) SHWaitOp_Operate(HANDLE hWaitOp, DWORD dwProcId);

/// THESE ARE INTERNAL ....

#define SHCR_CMD_REGISTER   1
#define SHCR_CMD_DEREGISTER 2

typedef struct _SHChangeRegistration {
    UINT    uCmd;
    ULONG   ulID;
    DWORD   hwnd;
    UINT    uMsg;
    DWORD   fSources;
    LONG    lEvents;
    BOOL    fRecursive;
    UINT    uidlRegister;
} SHChangeRegistration, *LPSHChangeRegistration;

typedef struct _SHChangeNotification {
    DWORD   dwSize;
    LONG    lEvent;
    UINT    uFlags;
    UINT    cRef;
    DWORD   dwEventTime;
    UINT    uidlMain;
    UINT    uidlExtra;
} SHChangeNotification, *LPSHChangeNotification;

typedef struct _SHChangeNotificationLock {
    LPITEMIDLIST            pidlMain;
    LPITEMIDLIST            pidlExtra;
    LPSHChangeNotification  pshcn;
#ifdef DEBUG
    DWORD                   dwSignature;
#endif
} SHChangeNotificationLock, * LPSHChangeNotificationLock;

typedef struct _SHChangeDWORDAsIDList {
    USHORT   cb;
    DWORD    dwItem1;
    DWORD    dwItem2;
    USHORT   cbZero;
} SHChangeDWORDAsIDList, *LPSHChangeDWORDAsIDList;



#if (_WIN32_IE >= 0x0400)

// Use this instead of SHChangeDWORDAsIDList for SHCNEE_ORDERCHANGED
// SHCNEE_PROMOTEDITEM and SHCNEE_DEMOTEDITEM
//
// When filling in the pSender, cast LPVOID directly to INT64.
// Do not cast through DWORD_PTR because that will not sign extend
// properly on Win32.

typedef struct _SHChangeMenuAsIDList {
    USHORT  cb;
    DWORD   dwItem1;        // SHCNEE_EXTENDED_EVENT requires this
    // pSender must appear immediately after dwItem1 for IE5.0 compat
    INT64 pSender;          // address of sender (64-bit for Win64)
    DWORD   dwProcessID;    // pid of sender
    USHORT  cbZero;
} SHChangeMenuAsIDList, * LPSHChangeMenuAsIDList;

typedef struct _SHChangeUpdateImageIDList {
    USHORT cb;
    int iIconIndex;
    int iCurIndex;
    UINT uFlags;
    DWORD dwProcessID;
    WCHAR szName[MAX_PATH];
    USHORT cbZero;
} SHChangeUpdateImageIDList, * LPSHChangeUpdateImageIDList;

SHSTDAPI_(int)  SHHandleUpdateImage( LPCITEMIDLIST pidlExtra );			
#endif /* _WIN32_IE */
#define SHChangeNotifyHandleEvents() SHChangeNotify(0, SHCNF_FLUSH, NULL, NULL)
SHSTDAPI_(ULONG) SHChangeNotifyRegister(HWND hwnd, int fSources, LONG fEvents, UINT wMsg, int cEntries, SHChangeNotifyEntry *pshcne);
#define SHChangeNotifyRegisterORD 2
SHSTDAPI_(BOOL) SHChangeNotifyDeregister(unsigned long ulID);
#define SHChangeNotifyDeregisterORD 4

SHSTDAPI_(BOOL) SHChangeNotifyUpdateEntryList(unsigned long ulID, int iUpdateType, int cEntries, SHChangeNotifyEntry *pshcne);

SHSTDAPI_(void)   SHChangeNotifyReceive(LONG lEvent, UINT uFlags, LPCITEMIDLIST pidl, LPCITEMIDLIST pidlExtra);
SHSTDAPI_(HANDLE) SHChangeNotification_Create(LONG lEvent, UINT uFlags, LPCITEMIDLIST pidlMain, LPCITEMIDLIST pidlExtra, DWORD dwProcessId, DWORD dwEventTime);
SHSTDAPI_(ULONG)  SHChangeNotification_Release(HANDLE hChangeNotification, DWORD dwProcessId);
SHSTDAPI_(LPSHChangeNotificationLock) SHChangeNotification_Lock(HANDLE hChangeNotification, DWORD dwProcessId, LPITEMIDLIST **pppidl, LONG *plEvent);
SHSTDAPI_(BOOL) SHChangeNotification_Unlock(LPSHChangeNotificationLock pshcnl);
SHSTDAPI_(BOOL) SHChangeRegistrationReceive(HANDLE hChangeNotification, DWORD dwProcId);
SHSTDAPI_(void) SHChangeNotifyDeregisterWindow(HWND hwnd);
SHSTDAPI_(LRESULT) SHChangeNotifySuspendResumeReceive(WPARAM wParam, LPARAM lParam);
SHSTDAPI_(BOOL) SHChangeNotifySuspendResume(BOOL bSuspend, LPITEMIDLIST pidlSuspend, BOOL bRescursive, DWORD dwReserved);

#if (_WIN32_IE >= 0x0400)
// The pidls that are given to the view via the ChangeNotifyEvents are simple Pidls,
// SHGetRealIDL() will convert them to true PIDLs.
SHSTDAPI SHGetRealIDL(IShellFolder *psf, LPCITEMIDLIST pidlSimple, LPITEMIDLIST * ppidlReal);
#endif // _WIN32_IE >= 0x0400
#ifndef NOOCHOST // {

//
// ====== OCHOST CONTROL ==========================================
//
// The following are all definations of structures, windows messages needed for OCHOST,
// the window control you can use to host an OC.
// To use OCHOST, you must specify the window class as OCHOST_CLASS or "OCHOST"
// -- To Create and initialize OCHOST:
// 1. CreateWindow with window title = the string version of class id or the OC
// 2. CreateWindow with a pointer to the OCHINITSTRUCT structure as the last parameter
//    you must allocate and initialize the OCHINITSTRUCT structure
// 3. CreateWindow with no title and the last param == NULL, then use the OCM_INITIALIZE message
//    or the OCHost_InitOC macro to initialize it.
// 4. Use OCM_QUERYINTERFACE to QI an interface pointer from the OC.
// 5. Simply destroy the window when you exit.

// SHDRC -- Shell Doc Register Classes

typedef struct _SHDRC {
    DWORD cbSize;   // size of this structure in bytes
    DWORD dwFlags;  // SHDRCF_ flags
} SHDRC;

#define SHDRCF_OCHOST   0x0001 // Register the OCHOST_CLASS window
#define SHDRCF_ALL      0x0001 //

SHDOCAPI_(BOOL)     DllRegisterWindowClasses(const SHDRC * pshdrc);
BROWSEUIAPI_(BOOL)  InitOCHostClass(const SHDRC *pshdrc);

#define OCHOST_CLASSA   "OCHost"
#define OCHOST_CLASSW   L"OCHost"

#ifdef UNICODE
#define OCHOST_CLASS    OCHOST_CLASSW
#else
#define OCHOST_CLASS    OCHOST_CLASSA
#endif

// ------------------- Window messages for OC Host --------------------

// IUnknown::QueryInterface the hosted OC
typedef struct _QIMSG {
    const IID * qiid;
    void **ppvObject;
} QIMSG, *LPQIMSG;

// ................. Query Interface Message ..........
#define OCM_QUERYINTERFACE      (WM_USER+0)

#ifdef __cplusplus
inline HRESULT OCHost_QueryInterface(HWND hwndOCH, REFIID riid, void **ppv) \
{ QIMSG qimsg = {&riid, ppv}; \
  return (HRESULT)SNDMSG((hwndOCH), OCM_QUERYINTERFACE, (WPARAM)sizeof(qimsg), (LPARAM)&qimsg); \
}
#else
#define OCHost_QueryInterface(hwndOCH, riid, ppv) \
{ QIMSG qimsg = {&riid, ppv}; \
  SNDMSG((hwndOCH), OCM_QUERYINTERFACE, (WPARAM)sizeof(qimsg), (LPARAM)&qimsg); \
}
#endif


// ------------------- OC Initialization Structure ---------------------
// This is intended to be passed in as the lParam of the OCM_INITOC message
typedef struct _OCHINITSTRUCT {
    UINT cbSize;                    // size of this structure
    CLSID clsidOC;                  // class ID of the OC
    IUnknown * punkOwner;           // owner of OCHOST
} OCHINITSTRUCT, *LPOCHINITSTRUCT;
//--------------------------------------------------------------------

// ................ Initialize and activate the OC ...............
#define OCM_INITIALIZE      (WM_USER+1)
#define OCM_INITOC          OCM_INITIALIZE
#define OCHost_InitOC(hwndOCH, lpOCS) \
  (HRESULT)SNDMSG((hwndOCH), OCM_INITOC, 0, (LPARAM)lpOCS)


// ............... give ochost a parent IUnknown .......
#define OCM_SETOWNER            (WM_USER+2)
#define OCHost_SetOwner(hwndOC, punk) \
  (HRESULT)SNDMSG((hwndOC), OCM_SETOWNER, 0, (LPARAM)(IUnknown*)(punk))

// ............... DoVerb the OC .......
// n.b. iVerb is technically a long, WPARAM might truncate it
#define OCM_DOVERB              (WM_USER+3)
#define OCHost_DoVerb(hwndOC, iVerb, lpMsg) \
  (HRESULT)SNDMSG((hwndOC), OCM_DOVERB, (WPARAM)iVerb, (LPARAM)lpMsg)

//.................Enable/Disable OC event notifications
#define OCM_ENABLEEVENTS       (WM_USER+4)
//  WPARAM: TRUE to enable event notifications, FALSE to disable.
//  returns: TRUE if successful, otherwise FALSE.
//  Event notification is in the form of a WM_NOTIFY with
//  code OCN_OCEVENT and an NMOCEVENT block.

#define OCHost_EnableEvents(hwndOC, bEnable) \
    (BOOL)SNDMSG((hwndOC), OCM_ENABLEEVENTS, (WPARAM)bEnable, 0L)

// ------------------ Window Notify messages from OC Host --------------

#define OCN_FIRST               0x1300
#define OCN_COCREATEINSTANCE    (OCN_FIRST + 1)

typedef struct _OCNCOCREATEMSG {
    NMHDR nmhdr;
    CLSID clsidOC;
    IUnknown ** ppunk;
} OCNCOCREATEMSG, *LPOCNCOCREATEMSG;

// NOTE: return values are defined as the following
// If the handler of OCN_COCREATEINSTANCE Notify message returns OCNCOCREATE_ALREADYCREATED,
// on return the (*ppvObj) is assumed to have the value of the OC's IUnkown pointer
#define OCNCOCREATE_CONTINUE       0
#define OCNCOCREATE_HANDLED       -1


#define OCN_PERSISTINIT         (OCN_FIRST + 2)
// NOTE: return values are defined as the following
// If the handler of OCN_PERSISTINIT Notify message returns OCNPERSIST_ABORT,
// the OCHOST will abort IPersist's initialization.
#define OCNPERSISTINIT_CONTINUE    0
#define OCNPERSISTINIT_HANDLED    -1

// The return value on the following notify messages are ignored.
#define OCN_ACTIVATE            (OCN_FIRST + 3)
#define OCN_DEACTIVATE          (OCN_FIRST + 4)
#define OCN_EXIT                (OCN_FIRST + 5)
#define OCN_ONPOSRECTCHANGE     (OCN_FIRST + 6)

typedef struct _OCNONPOSRECTCHANGEMSG {
    NMHDR nmhdr;
    LPCRECT prcPosRect;
} OCNONPOSRECTCHANGEMSG, *LPOCNONPOSRECTCHANGEMSG;

#define OCN_ONUIACTIVATE        (OCN_FIRST + 7)
typedef struct _OCNONUIACTIVATEMSG {
    NMHDR nmhdr;
    IUnknown *punk;
} OCNONUIACTIVATEMSG, *LPOCNONUIACTIVATEMSG;

#define OCNONUIACTIVATE_HANDLED       -1

#define OCN_ONSETSTATUSTEXT     (OCN_FIRST + 8)
typedef struct _OCNONSETSTATUSTEXT {
    NMHDR nmhdr;
    LPCOLESTR pwszStatusText;
} OCNONSETSTATUSTEXTMSG, *LPOCNONSETSTATUSTEXTMSG;


#define OCN_OCEVENT    (OCN_FIRST + 9)
//  OC event notification block.
//  Recipient should return nonzero if any outbound parameters were modified
//  and should be returned to the ActiveX control object; otherwse zero.
typedef struct tagNMOCEVENT
{
    IN NMHDR           hdr ;
    IN DISPID          dispID ;
    IN IID             iid ;
    IN LCID            lcid ;
    IN WORD            wFlags ;
    IN OUT DISPPARAMS  *pDispParams ;
    OUT VARIANT        *pVarResult ;
    OUT EXCEPINFO      *pExepInfo ;
    OUT UINT           *puArgErr ;
} NMOCEVENT, *PNMOCEVENT, *LPNMOCEVENT ;


#endif // NOOCHOST // }

//
// Access to shdocvw's active session count
//
enum SessionOp {
    SESSION_QUERY = 0,
    SESSION_INCREMENT,
    SESSION_DECREMENT,
	SESSION_INCREMENT_NODEFAULTBROWSERCHECK
};

long SetQueryNetSessionCount(enum SessionOp Op);
void IEWriteErrorLog(const EXCEPTION_RECORD* pexr);

//
// Internal APIs Follow.  NOT FOR PUBLIC CONSUMPTION.
//

// Errors shared in the shell

#define E_FILE_NOT_FOUND        MAKE_SCODE(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND)
#define E_PATH_NOT_FOUND        MAKE_SCODE(SEVERITY_ERROR, FACILITY_WIN32, ERROR_PATH_NOT_FOUND)


// Useful macros

#define ResultFromShort(i)      MAKE_HRESULT(SEVERITY_SUCCESS, 0, (USHORT)(i))
#define ShortFromResult(r)      (short)HRESULT_CODE(r)


#if (defined(UNICODE) && !defined(_X86_)) // all non-x86 systems require alignment
#ifndef ALIGNMENT_SCENARIO
#define ALIGNMENT_SCENARIO
#endif
#endif

#ifndef LPNTSTR_DEFINED
#define LPNTSTR_DEFINED
typedef UNALIGNED const WCHAR * LPNCWSTR;
typedef UNALIGNED WCHAR *       LPNWSTR;
#ifdef UNICODE
#define LPNCTSTR        LPNCWSTR
#define LPNTSTR         LPNWSTR
#else
#define LPNCTSTR        LPCSTR
#define LPNTSTR         LPSTR
#endif
#endif // LPNTSTR_DEFINED

#define RFN_FIRST       (0U-510U) // run file dialog notify
#define RFN_LAST        (0U-519U)


//=======================================================================
// String constants for
//  1. Registration database keywords       (prefix STRREG_)
//  2. Exported functions from handler dlls (prefix STREXP_)
//  3. .INI file keywords                   (prefix STRINI_)
//  4. Others                               (prefix STR_)
//=======================================================================
#define STRREG_SHELLUI          TEXT("ShellUIHandler")
#define STRREG_SHELL            TEXT("Shell")
#define STRREG_DEFICON          TEXT("DefaultIcon")
#define STRREG_SHEX             TEXT("shellex")
#define STRREG_SHEX_PROPSHEET   STRREG_SHEX TEXT("\\PropertySheetHandlers")
#define STRREG_SHEX_DDHANDLER   STRREG_SHEX TEXT("\\DragDropHandlers")
#define STRREG_SHEX_MENUHANDLER STRREG_SHEX TEXT("\\ContextMenuHandlers")
#define STRREG_SHEX_COPYHOOK    TEXT("Directory\\") STRREG_SHEX TEXT("\\CopyHookHandlers")
#define STRREG_SHEX_PRNCOPYHOOK TEXT("Printers\\") STRREG_SHEX TEXT("\\CopyHookHandlers")
#define STRREG_STARTMENU TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MenuOrder\\Start Menu")
#define STRREG_FAVORITES TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MenuOrder\\Favorites")
#define STRREG_DISCARDABLE      TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable")
#define STRREG_POSTSETUP        TEXT("\\PostSetup")

#define STREXP_CANUNLOAD        "DllCanUnloadNow"       // From OLE 2.0

#define STRINI_CLASSINFO        TEXT(".ShellClassInfo")       // secton name
#define STRINI_SHELLUI          TEXT("ShellUIHandler")
#define STRINI_OPENDIRICON      TEXT("OpenDirIcon")
#define STRINI_DIRICON          TEXT("DirIcon")

#define STR_DESKTOPINI          TEXT("desktop.ini")
#define STR_DESKTOPINIA         "desktop.ini"

#define DRIVEID(path)   ((*path - 'A') & 31)

#define PATH_CCH_EXT    64
// PathResolve flags
#define PRF_VERIFYEXISTS            0x0001
#define PRF_TRYPROGRAMEXTENSIONS    (0x0002 | PRF_VERIFYEXISTS)
#define PRF_FIRSTDIRDEF             0x0004
#define PRF_DONTFINDLNK             0x0008      // if PRF_TRYPROGRAMEXTENSIONS is specified

#ifndef NOUSER

typedef struct {
    NMHDR  hdr;
    CHAR   szCmd[MAX_PATH*2];
    DWORD  dwHotKey;
    HMONITOR hMonitor;
} NMVIEWFOLDERA, *LPNMVIEWFOLDERA;
typedef struct {
    NMHDR  hdr;
    WCHAR  szCmd[MAX_PATH*2];
    DWORD  dwHotKey;
    HMONITOR hMonitor;
} NMVIEWFOLDERW, *LPNMVIEWFOLDERW;
#ifdef UNICODE
typedef NMVIEWFOLDERW NMVIEWFOLDER;
typedef LPNMVIEWFOLDERW LPNMVIEWFOLDER;
#else
typedef NMVIEWFOLDERA NMVIEWFOLDER;
typedef LPNMVIEWFOLDERA LPNMVIEWFOLDER;
#endif // UNICODE

#endif

//
//  DDE related APIs
//
STDAPI_(void) ShellDDEInit(BOOL fInit);
STDAPI_(BOOL) DDEHandleViewFolderNotify(IShellBrowser* psb, HWND hwnd, LPNMVIEWFOLDER lpnm);
STDAPI_(LPNMVIEWFOLDER) DDECreatePostNotify(LPNMVIEWFOLDER lpnm);

SHSTDAPI_(int) RestartDialog(HWND hwnd, LPCTSTR lpPrompt, DWORD dwReturn);

SHSTDAPI SHCoCreateInstance(LPCTSTR pszCLSID, const CLSID * lpclsid, IUnknown *pUnkOuter, REFIID riid, void **ppv);

SHSTDAPI CIDLData_CreateFromIDArray(LPCITEMIDLIST pidlFolder, UINT cidl, LPCITEMIDLIST apidl[], IDataObject ** ppdtobj);


// Common strings
#define STR_DESKTOPCLASS        "Progman"

//
// Storage name of a scrap/bookmark item
//
#define WSTR_SCRAPITEM L"\003ITEM000"


// For CallCPLEntry16
//
DECLARE_HANDLE(FARPROC16);
STDAPI_(LRESULT) CallCPLEntry16(HINSTANCE hinst, FARPROC16 lpfnEntry, HWND hwndCPL, UINT msg, LPARAM lParam1, LPARAM lParam2);

#ifdef RFN_FIRST
#define RFN_EXECUTE             (RFN_FIRST - 0)
typedef struct {
    NMHDR hdr;
    LPCSTR   lpszCmd;
    LPCSTR   lpszWorkingDir;
    int nShowCmd;
} NMRUNFILEA, *LPNMRUNFILEA;
typedef struct {
    NMHDR hdr;
    LPCWSTR  lpszCmd;
    LPCWSTR  lpszWorkingDir;
    int nShowCmd;
} NMRUNFILEW, *LPNMRUNFILEW;
#ifdef UNICODE
typedef NMRUNFILEW NMRUNFILE;
typedef LPNMRUNFILEW LPNMRUNFILE;
#else
typedef NMRUNFILEA NMRUNFILE;
typedef LPNMRUNFILEA LPNMRUNFILE;
#endif // UNICODE

#endif


SHSTDAPI SHCreateStdEnumFmtEtc(UINT cfmt, const FORMATETC afmt[], IEnumFORMATETC **ppenumFormatEtc);


// Shell create link API
#define SHCL_USETEMPLATE        0x0001
#define SHCL_USEDESKTOP         0x0002
#define SHCL_CONFIRM            0x0004
#define SHCL_MAKEFOLDERSHORTCUT 0x0008

SHSTDAPI SHCreateLinks(HWND hwnd, LPCTSTR pszDir, IDataObject *pDataObj, UINT fFlags, LPITEMIDLIST* ppidl);

SHSTDAPI SHRegisterDragDrop(HWND hwnd, IDropTarget *pdtgt);
SHSTDAPI SHRevokeDragDrop(HWND hwnd);
SHSTDAPI SHDoDragDrop(HWND hwnd, IDataObject *pdata, IDropSource *pdsrc, DWORD dwEffect, DWORD *pdwEffect);

//===========================================================================
// office 9 pluggable UI
//===========================================================================

#define PUI_OFFICE_COMMAND     (WM_USER + 0x901)
#define PLUGUI_CMD_SHUTDOWN    0 // wParam value
#define PLUGUI_CMD_QUERY       1 // wParam value
#define OFFICE_VERSION_9       9 // standardized value to return for Office 9 apps

typedef struct _PLUGUI_INFO
{
    unsigned uMajorVersion : 8; // Used to indicate App's major version number
    unsigned uOleServer : 1;    // BOOL, TRUE if this is an OLE process
    unsigned uUnused : 23;      // not used
} PLUGUI_INFO;

typedef union _PLUGUI_QUERY
{
    UINT uQueryVal;
    PLUGUI_INFO PlugUIInfo;
} PLUGUI_QUERY;

//===========================================================================
// Image dragging API (definitely private)
//===========================================================================

// stuff for doing auto scrolling
#define NUM_POINTS      3
typedef struct {        // asd
    int iNextSample;
    DWORD dwLastScroll;
    BOOL bFull;
    POINT pts[NUM_POINTS];
    DWORD dwTimes[NUM_POINTS];
} AUTO_SCROLL_DATA;

//Contains the drag context header.
typedef struct {
    BOOL  fImage;
    BOOL  fLayered;
    POINT ptOffset;
} DragContextHeader;

#define DAD_InitScrollData(pad) (pad)->bFull = FALSE, (pad)->iNextSample = 0, (pad)->dwLastScroll = 0

SHSTDAPI_(BOOL) DAD_SetDragImage(HIMAGELIST him, POINT * pptOffset);
SHSTDAPI_(BOOL) DAD_DragEnter(HWND hwndTarget);
SHSTDAPI_(BOOL) DAD_DragEnterEx(HWND hwndTarget, const POINT ptStart);
SHSTDAPI_(BOOL) DAD_DragEnterEx2(HWND hwndTarget, const POINT ptStart, IDataObject *pdtObject);
SHSTDAPI_(BOOL) DAD_ShowDragImage(BOOL fShow);
SHSTDAPI_(BOOL) DAD_DragMove(POINT pt);
SHSTDAPI_(BOOL) DAD_DragLeave(void);
SHSTDAPI_(BOOL) DAD_AutoScroll(HWND hwnd, AUTO_SCROLL_DATA *pad, const POINT *pptNow);
SHSTDAPI_(BOOL) DAD_SetDragImageFromListView(HWND hwndLV, POINT ptOffset);
SHSTDAPI_(void) DAD_InvalidateCursors(void);



//----------------------------------------------------------------------------
// CABINETSTATE holds the global configuration for the Explorer and its cohorts.
//
// Originally the cLength was an 'int', it is now two words, allowing us to
// specify a version number.
//----------------------------------------------------------------------------

typedef struct {
    WORD cLength;
    WORD nVersion;

    BOOL fFullPathTitle            : 1;
    BOOL fSaveLocalView            : 1;
    BOOL fNotShell                 : 1;
    BOOL fSimpleDefault            : 1;
    BOOL fDontShowDescBar          : 1;
    BOOL fNewWindowMode            : 1;
    BOOL fShowCompColor            : 1;  // NT: Show compressed volumes in a different colour
    BOOL fDontPrettyNames          : 1;  // NT: Do 8.3 name conversion, or not!
    BOOL fAdminsCreateCommonGroups : 1;  // NT: Administrators create comon groups
    UINT fUnusedFlags : 7;

    UINT fMenuEnumFilter;

} CABINETSTATE, * LPCABINETSTATE;

#define CABINETSTATE_VERSION 2

// APIs for reading and writing the cabinet state.
SHSTDAPI_(BOOL) ReadCabinetState( LPCABINETSTATE lpState, int iSize );
SHSTDAPI_(BOOL) WriteCabinetState( LPCABINETSTATE lpState );


// RUN FILE RETURN values from notify message
#define RFR_NOTHANDLED 0
#define RFR_SUCCESS 1
#define RFR_FAILURE 2

#define PathGetExtensionORD     158

SHSTDAPI_(LPTSTR) PathGetExtension(LPCTSTR pszPath, LPTSTR pszExtension, int cchExt);
SHSTDAPI_(BOOL) PathMakeUniqueName(LPTSTR pszUniqueName, UINT cchMax, LPCTSTR pszTemplate, LPCTSTR pszLongPlate, LPCTSTR pszDir);
SHSTDAPI_(BOOL) PathGetShortName(LPCTSTR pszLongName, LPTSTR pszShortName, UINT cbShortName);
SHSTDAPI_(BOOL) PathGetLongName(LPCTSTR pszShortName, LPTSTR pszLongName, UINT cbLongName);
SHSTDAPI_(BOOL) PathDirectoryExists(LPCTSTR pszDir);
SHSTDAPI_(void) PathQualify(LPTSTR psz);
SHSTDAPI_(LPTSTR) PathGetNextComponent(LPCTSTR pszPath, LPTSTR pszComponent);
SHSTDAPI_(BOOL) PathIsExe(LPCTSTR pszPath);

SHSTDAPI_(BOOL) PathIsSlowA(LPCSTR pszFile, DWORD dwAttr);
SHSTDAPI_(BOOL) PathIsSlowW(LPCWSTR pszFile, DWORD dwAttr);
#ifdef UNICODE
#define PathIsSlow  PathIsSlowW
#else
#define PathIsSlow  PathIsSlowA
#endif // !UNICODE
SHSTDAPI_(BOOL) PathIsTemporaryA(LPCSTR pszPath);
SHSTDAPI_(BOOL) PathIsTemporaryW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsTemporary  PathIsTemporaryW
#else
#define PathIsTemporary  PathIsTemporaryA
#endif // !UNICODE

//
//  Return codes from PathCleanupSpec.  Negative return values are
//  unrecoverable errors
//
#define PCS_FATAL           0x80000000
#define PCS_REPLACEDCHAR    0x00000001
#define PCS_REMOVEDCHAR     0x00000002
#define PCS_TRUNCATED       0x00000004
#define PCS_PATHTOOLONG     0x00000008  // Always combined with FATAL

SHSTDAPI_(int) PathCleanupSpec(LPCTSTR pszDir, LPTSTR pszSpec);
SHSTDAPI_(int) PathCleanupSpecEx(LPCTSTR pszDir, LPTSTR pszSpec);
SHSTDAPI_(int) PathResolve(LPTSTR pszPath, LPCTSTR dirs[], UINT fFlags);
SHSTDAPI_(BOOL) ParseField(LPCTSTR szData, int n, LPTSTR szBuf, int iBufLen);

// Needed for RunFileDlg
#define RFD_NOBROWSE            0x00000001
#define RFD_NODEFFILE           0x00000002
#define RFD_USEFULLPATHDIR      0x00000004
#define RFD_NOSHOWOPEN          0x00000008
#define RFD_WOW_APP             0x00000010
#define RFD_NOSEPMEMORY_BOX     0x00000020


SHSTDAPI_(int) RunFileDlg(HWND hwnd, HICON hIcon, LPCTSTR pszWorkingDir, LPCTSTR pszTitle,
                          LPCTSTR pszPrompt, DWORD dwFlags);

SHSTDAPI_(BOOL) GetFileNameFromBrowse(HWND hwnd, LPTSTR pszFilePath, UINT cbFilePath,
                                      LPCTSTR pszWorkingDir, LPCTSTR pszDefExt, LPCTSTR pszFilters, LPCTSTR pszTitle);

SHSTDAPI_(int) DriveType(int iDrive);
SHSTDAPI_(int) RealDriveTypeFlags(int iDrive, BOOL fOKToHitNet);
SHSTDAPI_(int) RealDriveType(int iDrive, BOOL fOKToHitNet);
SHSTDAPI_(void) InvalidateDriveType(int iDrive);
SHSTDAPI_(int) IsNetDrive(int iDrive);

//
// Constants used for dwNumFmtFlags argument in Int64ToString and LargeIntegerToString.
//
#define NUMFMT_IDIGITS    0x00000001
#define NUMFMT_ILZERO     0x00000002
#define NUMFMT_SGROUPING  0x00000004
#define NUMFMT_SDECIMAL   0x00000008
#define NUMFMT_STHOUSAND  0x00000010
#define NUMFMT_INEGNUMBER 0x00000020
#define NUMFMT_ALL        0xFFFFFFFF

SHSTDAPI_(int) LargeIntegerToString(LARGE_INTEGER *pN, LPTSTR szOutStr, UINT nSize, BOOL bFormat, NUMBERFMT *pFmt, DWORD dwNumFmtFlags);
SHSTDAPI_(int) Int64ToString(_int64 n, LPTSTR szOutStr, UINT nSize, BOOL bFormat, NUMBERFMT *pFmt, DWORD dwNumFmtFlags);

//-------- drive type identification --------------
// iDrive      drive index (0=A, 1=B, ...)
//
#define DRIVE_CDROM     5           // extended DriveType() types
#define DRIVE_RAMDRIVE  6
#define DRIVE_TYPE      0x000F      // type masek
#define DRIVE_SLOW      0x0010      // drive is on a slow link
#define DRIVE_LFN       0x0020      // drive supports LFNs
#define DRIVE_AUTORUN   0x0040      // drive has AutoRun.inf in root.
#define DRIVE_AUDIOCD   0x0080      // drive is a AudioCD
#define DRIVE_AUTOOPEN  0x0100      // should *always* auto open on insert
#define DRIVE_NETUNAVAIL 0x0200     // Network drive that is not available
#define DRIVE_SHELLOPEN  0x0400     // should auto open on insert, if shell has focus
#define DRIVE_SECURITY   0x0800     // Supports ACLs
#define DRIVE_COMPRESSED 0x1000     // Root of volume is compressed
#define DRIVE_ISCOMPRESSIBLE 0x2000 // Drive supports compression (not nescesarrily compressed)
#define DRIVE_DVD       0x4000      // drive is a DVD

#define DriveTypeFlags(iDrive)      DriveType('A' + (iDrive))
#define DriveIsSlow(iDrive)         (RealDriveTypeFlags(iDrive, FALSE) & DRIVE_SLOW)
#define DriveIsLFN(iDrive)          (RealDriveTypeFlags(iDrive, TRUE)  & DRIVE_LFN)
#define DriveIsAutoRun(iDrive)      (RealDriveTypeFlags(iDrive, FALSE) & DRIVE_AUTORUN)
#define DriveIsAutoOpen(iDrive)     (RealDriveTypeFlags(iDrive, FALSE) & DRIVE_AUTOOPEN)
#define DriveIsShellOpen(iDrive)    (RealDriveTypeFlags(iDrive, FALSE) & DRIVE_SHELLOPEN)
#define DriveIsAudioCD(iDrive)      (RealDriveTypeFlags(iDrive, FALSE) & DRIVE_AUDIOCD)
#define DriveIsNetUnAvail(iDrive)   (RealDriveTypeFlags(iDrive, FALSE) & DRIVE_NETUNAVAIL)
#define DriveIsSecure(iDrive)       (RealDriveTypeFlags(iDrive, TRUE)  & DRIVE_SECURITY)
#define DriveIsCompressed(iDrive)   (RealDriveTypeFlags(iDrive, TRUE)  & DRIVE_COMPRESSED)
#define DriveIsCompressible(iDrive) (RealDriveTypeFlags(iDrive, TRUE)  & DRIVE_ISCOMPRESSIBLE)
#define DriveIsDVD(iDrive)          (RealDriveTypeFlags(iDrive, FALSE) & DRIVE_DVD)

#define IsCDRomDrive(iDrive)        (RealDriveType(iDrive, FALSE) == DRIVE_CDROM)
#define IsRamDrive(iDrive)          (RealDriveType(iDrive, FALSE) == DRIVE_RAMDRIVE)
#define IsRemovableDrive(iDrive)    (RealDriveType(iDrive, FALSE) == DRIVE_REMOVABLE)
#define IsRemoteDrive(iDrive)       (RealDriveType(iDrive, FALSE) == DRIVE_REMOTE)


//
// For SHCreateDefClassObject
//
typedef HRESULT (CALLBACK *LPFNCREATEINSTANCE)(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);

SHSTDAPI SHCreateDefClassObject(REFIID riid, void **ppv, LPFNCREATEINSTANCE lpfn, UINT *pcRefDll, REFIID riidInstance);


// Flags for Shell_MergeMenus
#define MM_ADDSEPARATOR         0x00000001L
#define MM_SUBMENUSHAVEIDS      0x00000002L
#define MM_DONTREMOVESEPS       0x00000004L

SHSTDAPI_(UINT) Shell_MergeMenus(HMENU hmDst, HMENU hmSrc, UINT uInsert, UINT uIDAdjust, UINT uIDAdjustMax, ULONG uFlags);


typedef HANDLE HNRES;           // for SHGetNetResource

SHSTDAPI_(UINT) SHGetNetResource(HNRES hnres, UINT iItem, LPNETRESOURCE pnres, UINT cbMax);


/*
 * The SHObjectProperties API provides an easy way to invoke
 *   the Properties context menu command on shell objects.
 *
 *   PARAMETERS
 *
 *     hwnd    The window handle of the window which will own the dialog
 *     dwType       A SHOP_ value as defined below
 *     lpObject     Name of the object, see SHOP_ values below
 *     lpPage       The name of the property sheet page to open to or NULL.
 *
 *   RETURN
 *
 *     TRUE if the Properties command was invoked
 */
SHSTDAPI_(BOOL) SHObjectProperties(HWND hwnd, DWORD dwType, LPCTSTR lpObject, LPCTSTR lpPage);

#define SHOP_PRINTERNAME 0x00000001  // lpObject points to a printer friendly name
#define SHOP_FILEPATH    0x00000002  // lpObject points to a fully qualified path+file name
#define SHOP_VOLUMEGUID  0x00000004  // lpObject points to a Volume GUID
#define SHOP_TYPEMASK	 0x00000007
#define SHOP_MODAL       0x80000000


/*
 * The SHFormatDrive API provides access to the Shell
 *   format dialog. This allows apps which want to format disks
 *   to bring up the same dialog that the Shell does to do it.
 *
 *   This dialog is not sub-classable. You cannot put custom
 *   controls in it. If you want this ability, you will have
 *   to write your own front end for the DMaint_FormatDrive
 *   engine.
 *
 *   NOTE that the user can format as many diskettes in the specified
 *   drive, or as many times, as he/she wishes to. There is no way to
 *   force any specififc number of disks to format. If you want this
 *   ability, you will have to write your own front end for the
 *   DMaint_FormatDrive engine.
 *
 *   NOTE also that the format will not start till the user pushes the
 *   start button in the dialog. There is no way to do auto start. If
 *   you want this ability, you will have to write your own front end
 *   for the DMaint_FormatDrive engine.
 *
 *   PARAMETERS
 *
 *     hwnd    = The window handle of the window which will own the dialog
 *               NOTE that unlike SHCheckDrive, hwnd == NULL does not cause
 *               this dialog to come up as a "top level application" window.
 *               This parameter should always be non-null, this dialog is
 *               only designed to be the child of another window, not a
 *               stand-alone application.
 *     drive   = The 0 based (A: == 0) drive number of the drive to format
 *     fmtID   = The ID of the physical format to format the disk with
 *               NOTE: The special value SHFMT_ID_DEFAULT means "use the
 *                     default format specified by the DMaint_FormatDrive
 *                     engine". If you want to FORCE a particular format
 *                     ID "up front" you will have to call
 *                     DMaint_GetFormatOptions yourself before calling
 *                     this to obtain the valid list of phys format IDs
 *                     (contents of the PhysFmtIDList array in the
 *                     FMTINFOSTRUCT).
 *     options = There is currently only two option bits defined
 *
 *                SHFMT_OPT_FULL
 *                SHFMT_OPT_SYSONLY
 *
 *               The normal defualt in the Shell format dialog is
 *               "Quick Format", setting this option bit indicates that
 *               the caller wants to start with FULL format selected
 *               (this is useful for folks detecting "unformatted" disks
 *               and wanting to bring up the format dialog).
 *
 *               The SHFMT_OPT_SYSONLY initializes the dialog to
 *               default to just sys the disk.
 *
 *               All other bits are reserved for future expansion and
 *               must be 0.
 *
 *               Please note that this is a bit field and not a value
 *               and treat it accordingly.
 *
 *   RETURN
 *      The return is either one of the SHFMT_* values, or if the
 *      returned DWORD value is not == to one of these values, then
 *      the return is the physical format ID of the last succesful
 *      format. The LOWORD of this value can be passed on subsequent
 *      calls as the fmtID parameter to "format the same type you did
 *      last time".
 *
 */
SHSTDAPI_(DWORD) SHFormatDrive(HWND hwnd, UINT drive, UINT fmtID, UINT options);

SHSTDAPI_(DWORD) SHChkDskDrive(HWND hwnd, UINT drive);

//
// Special value of fmtID which means "use the default format"
//
#define SHFMT_ID_DEFAULT    0xFFFF

//
// Option bits for options parameter
//
#define SHFMT_OPT_FULL     0x0001
#define SHFMT_OPT_SYSONLY  0x0002

//
// Special return values. PLEASE NOTE that these are DWORD values.
//
#define SHFMT_ERROR     0xFFFFFFFFL     // Error on last format, drive may be formatable
#define SHFMT_CANCEL    0xFFFFFFFEL     // Last format was canceled
#define SHFMT_NOFORMAT  0xFFFFFFFDL     // Drive is not formatable

//====== SEMI-PRIVATE API ===============================
#ifndef HPSXA_DEFINED
#define HPSXA_DEFINED
DECLARE_HANDLE( HPSXA );
#endif
WINSHELLAPI HPSXA SHCreatePropSheetExtArray( HKEY hKey, LPCTSTR pszSubKey, UINT max_iface );
WINSHELLAPI void SHDestroyPropSheetExtArray( HPSXA hpsxa );
WINSHELLAPI UINT SHAddFromPropSheetExtArray( HPSXA hpsxa, LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam );
WINSHELLAPI UINT SHReplaceFromPropSheetExtArray( HPSXA hpsxa, UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam );
WINSHELLAPI HPSXA SHCreatePropSheetExtArrayEx( HKEY hKey, LPCTSTR pszSubKey, UINT max_iface, IDataObject * pdo );

//====== SEMI-PRIVATE API ORDINALS ===============================
// This is the list of semi-private ordinals we semi-publish.
#define SHGetNetResourceORD                      69
#define SHObjectPropertiesORD                   178
#define SHAddFromPropSheetExtArrayORD           167
#define SHCreatePropSheetExtArrayORD            168
#define SHDestroyPropSheetExtArrayORD           169
#define SHReplaceFromPropSheetExtArrayORD       170
#define SHCreateDefClassObjectORD                70

#define SHEXP_SHGETNETRESOURCE                  MAKEINTRESOURCE(SHGetNetResourceORD)

#define SHEXP_SHADDFROMPROPSHEETEXTARRAY        MAKEINTRESOURCE(SHAddFromPropSheetExtArrayORD)
#define SHEXP_SHCREATEPROPSHEETEXTARRAY         MAKEINTRESOURCE(SHCreatePropSheetExtArrayORD)
#define SHEXP_SHDESTROYPROPSHEETEXTARRAY        MAKEINTRESOURCE(SHDestroyPropSheetExtArrayORD)
#define SHEXP_SHREPLACEFROMPROPSHEETEXTARRAY    MAKEINTRESOURCE(SHReplaceFromPropSheetExtArrayORD)
#define SHEXP_SHCREATEDEFCLASSOBJECT            MAKEINTRESOURCE(SHCreateDefClassObjectORD)


// This avoids duplicate definition problems in the net\config project
// which defines *some* of its own PIDL utility functions, but depends on
// us for others (ILClone())
SHSTDAPI_(LPITEMIDLIST) ILClone(LPCITEMIDLIST pidl);

#ifndef AVOID_NET_CONFIG_DUPLICATES
//===========================================================================
// ITEMIDLIST
//===========================================================================

// flags for ILGetDisplayNameEx
#define ILGDN_FULLNAME  0
#define ILGDN_ITEMONLY  1
#define ILGDN_INFOLDER  2

SHSTDAPI_(LPITEMIDLIST) ILGetNext(LPCITEMIDLIST pidl);
SHSTDAPI_(UINT)         ILGetSize(LPCITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) ILFindLastID(LPCITEMIDLIST pidl);
SHSTDAPI_(BOOL)         ILRemoveLastID(LPITEMIDLIST pidl);

#define ILIsEmpty(pidl)     ((pidl) == NULL || (pidl)->mkid.cb==0)

SHSTDAPI_(LPITEMIDLIST) ILCreate(void);
SHSTDAPI_(LPITEMIDLIST) ILAppendID(LPITEMIDLIST pidl, LPCSHITEMID pmkid, BOOL fAppend);
SHSTDAPI_(void)         ILFree(LPITEMIDLIST pidl);
SHSTDAPI_(void)         ILGlobalFree(LPITEMIDLIST pidl);
SHSTDAPI_(BOOL)         ILGetDisplayName(LPCITEMIDLIST pidl, LPTSTR pszName);
SHSTDAPI_(BOOL)         ILGetDisplayNameEx(IShellFolder *psfRoot, LPCITEMIDLIST pidl, LPTSTR pszName, int fType);
SHSTDAPI_(BOOL)         ILGetPseudoNameA(LPCITEMIDLIST pidl, LPCITEMIDLIST pidlBase, LPSTR pszName, int fType);
SHSTDAPI_(BOOL)         ILGetPseudoNameW(LPCITEMIDLIST pidl, LPCITEMIDLIST pidlBase, LPWSTR pszName, int fType);
#ifdef UNICODE
#define ILGetPseudoName  ILGetPseudoNameW
#else
#define ILGetPseudoName  ILGetPseudoNameA
#endif // !UNICODE

SHSTDAPI_(LPITEMIDLIST) ILCloneFirst(LPCITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) ILGlobalClone(LPCITEMIDLIST pidl);
SHSTDAPI_(BOOL)         ILIsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
SHSTDAPI_(BOOL)         ILIsParent(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, BOOL fImmediate);
SHSTDAPI_(LPITEMIDLIST) ILFindChild(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlChild);
SHSTDAPI_(LPITEMIDLIST) ILCombine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
SHSTDAPI                ILLoadFromStream(IStream *pstm, LPITEMIDLIST *pidl);
SHSTDAPI                ILSaveToStream(IStream *pstm, LPCITEMIDLIST pidl);
SHSTDAPI                ILLoadFromFile(HFILE hfile, LPITEMIDLIST *pidl);
SHSTDAPI                ILSaveToFile(HFILE hfile, LPCITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) _ILCreate(UINT cbSize);

#if (_WIN32_IE >= 0x0400)
SHSTDAPI_(LPITEMIDLIST) ILCreateFromPathA(LPCSTR pszPath);
SHSTDAPI_(LPITEMIDLIST) ILCreateFromPathW(LPCWSTR pszPath);
#ifdef UNICODE
#define ILCreateFromPath  ILCreateFromPathW
#else
#define ILCreateFromPath  ILCreateFromPathA
#endif // !UNICODE
#endif
SHSTDAPI SHILCreateFromPath(LPCTSTR szPath, LPITEMIDLIST *ppidl, DWORD *rgfInOut);

// helper macros
#define ILCreateFromID(pmkid)   ILAppendID(NULL, pmkid, TRUE)

#endif // AVOID_NET_CONFIG_DUPLICATES

//
#undef  INTERFACE
#define INTERFACE   IDefViewFrame
DECLARE_INTERFACE_(IDefViewFrame, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IDefViewFrame methods ***
    STDMETHOD(GetWindowLV) (THIS_ HWND * phwnd) PURE;
    STDMETHOD(ReleaseWindowLV) (THIS) PURE;
    STDMETHOD(GetShellFolder)(THIS_ IShellFolder **ppsf) PURE;

    // Warning!  Do not change or add any methods to this interface
    // because IE4 shell32.dll uses it, so changing the interface
    // breaks IE4 interop.

};

// Warning!  IE4's shell32.dll does not support IDefViewFrame2, so make
//           sure you can survive its absence.

#undef  INTERFACE
#define INTERFACE   IDefViewFrame2
DECLARE_INTERFACE_(IDefViewFrame2, IDefViewFrame)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IDefViewFrame methods ***
    STDMETHOD(GetWindowLV) (THIS_ HWND * phwnd) PURE;
    STDMETHOD(ReleaseWindowLV) (THIS) PURE;
    STDMETHOD(GetShellFolder)(THIS_ IShellFolder **ppsf) PURE;

    // *** IDefViewFrame2 methods ***
    STDMETHOD(GetWindowLV2) (THIS_ HWND * phwnd, IUnknown * punk) PURE;
    STDMETHOD(AutoAutoArrange)(THIS_ DWORD dwReserved) PURE;
};

#undef  INTERFACE
#define INTERFACE   IWebViewOCWinMan
DECLARE_INTERFACE_(IWebViewOCWinMan, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IWebViewOCWinMan methods ***
    STDMETHOD(SwapWindow) (THIS_ HWND hwndLV, IWebViewOCWinMan **pocWinMan) PURE;
};

//===========================================================================
// Shell restrictions. (Parameter for SHRestricted)
typedef enum
{
    // Classic Win95 policy bits.  Do NOT add any more bits in
    // this range.  See the ranges below to add new policies.
    //          |                        |
    //          v                        v
    REST_NONE                       = 0x00000000,
    REST_NORUN                      = 0x00000001,
    REST_NOCLOSE                    = 0x00000002,
    REST_NOSAVESET                  = 0x00000004,
    REST_NOFILEMENU                 = 0x00000008,
    REST_NOSETFOLDERS               = 0x00000010,
    REST_NOSETTASKBAR               = 0x00000020,
    REST_NODESKTOP                  = 0x00000040,
    REST_NOFIND                     = 0x00000080,
    REST_NODRIVES                   = 0x00000100,
    REST_NODRIVEAUTORUN             = 0x00000200,
    REST_NODRIVETYPEAUTORUN         = 0x00000400,
    REST_NONETHOOD                  = 0x00000800,
    REST_STARTBANNER                = 0x00001000,
    REST_RESTRICTRUN                = 0x00002000,
    REST_NOPRINTERTABS              = 0x00004000,
    REST_NOPRINTERDELETE            = 0x00008000,
    REST_NOPRINTERADD               = 0x00010000,
    REST_NOSTARTMENUSUBFOLDERS      = 0x00020000,
    REST_MYDOCSONNET                = 0x00040000,
    REST_NOEXITTODOS                = 0x00080000,
    REST_ENFORCESHELLEXTSECURITY    = 0x00100000,
    REST_LINKRESOLVEIGNORELINKINFO  = 0x00200000,
    REST_NOCOMMONGROUPS             = 0x00400000,
    REST_SEPARATEDESKTOPPROCESS     = 0x00800000,
    REST_NOWEB                      = 0x01000000,
    REST_NOTRAYCONTEXTMENU          = 0x02000000,
    REST_NOVIEWCONTEXTMENU          = 0x04000000,
    REST_NONETCONNECTDISCONNECT     = 0x08000000,
    REST_STARTMENULOGOFF            = 0x10000000,
    REST_NOSETTINGSASSIST           = 0x20000000,

    // Values starting at 4000001 are treated as raw INTs.  New
    // *shell-specific* policies (NOT browser policies) should
    // be added in this range.  Use SHRestricted() with these bits.
    //          |                        |
    //          v                        v

    REST_NOINTERNETICON             = 0x40000001,
    REST_NORECENTDOCSHISTORY        = 0x40000002,
    REST_NORECENTDOCSMENU           = 0x40000003,
    REST_NOACTIVEDESKTOP            = 0x40000004,
    REST_NOACTIVEDESKTOPCHANGES     = 0x40000005,
    REST_NOFAVORITESMENU            = 0x40000006,
    REST_CLEARRECENTDOCSONEXIT      = 0x40000007,
    REST_CLASSICSHELL               = 0x40000008,
    REST_NOCUSTOMIZEWEBVIEW         = 0x40000009,
    REST_NOHTMLWALLPAPER            = 0x40000010,
    REST_NOCHANGINGWALLPAPER        = 0x40000011,
    REST_NODESKCOMP                 = 0x40000012,
    REST_NOADDDESKCOMP              = 0x40000013,
    REST_NODELDESKCOMP              = 0x40000014,
    REST_NOCLOSEDESKCOMP            = 0x40000015,
    REST_NOCLOSE_DRAGDROPBAND       = 0x40000016,   // Disable Close and Drag & Drop on ALL Bands
    REST_NOMOVINGBAND               = 0x40000017,   // Disable Moving ALL Bands
    REST_NOEDITDESKCOMP             = 0x40000018,
    REST_NORESOLVESEARCH            = 0x40000019,
    REST_NORESOLVETRACK             = 0x4000001A,
    REST_FORCECOPYACLWITHFILE       = 0X4000001B,
    REST_NOLOGO3CHANNELNOTIFY       = 0x4000001C,
    REST_NOFORGETSOFTWAREUPDATE     = 0x4000001D,
    REST_NOSETACTIVEDESKTOP         = 0x4000001E,   // No Active desktop on Settings Menu
    REST_NOUPDATEWINDOWS            = 0x4000001F,   // No Windows Update on Settings Menu
    REST_NOCHANGESTARMENU           = 0x40000020,   // No Context menu or Drag and Drop on Start menu
    REST_NOFOLDEROPTIONS            = 0x40000021,   // No Folder Options on Settings Menu
    REST_HASFINDCOMPUTERS           = 0x40000022,   // Show Start/Search/Computers
    REST_INTELLIMENUS               = 0x40000023,
    REST_RUNDLGMEMCHECKBOX          = 0x40000024,
    REST_ARP_ShowPostSetup          = 0x40000025,   // ARP: Show Post-Setup page
    REST_NOCSC                      = 0x40000026,   // Disable the ClientSide caching on SM
    REST_NOCONTROLPANEL             = 0x40000027,   // Remove the Control Panel only from SM|Settings
    REST_ENUMWORKGROUP              = 0x40000028,   // Enumerate workgroup in root of nethood
    REST_ARP_NOARP                  = 0x40000029,   // ARP: Don't Allow ARP to come up at all
    REST_ARP_NOREMOVEPAGE           = 0x4000002A,   // ARP: Don't allow Remove page
    REST_ARP_NOADDPAGE              = 0x4000002B,   // ARP: Don't allow Add page
    REST_ARP_NOWINSETUPPAGE         = 0x4000002C,   // ARP: Don't allow opt components page
    REST_GREYMSIADS                 = 0x4000002D,    // SM: Allow the greying of Darwin Ads in SM
    REST_NOCHANGEMAPPEDDRIVELABEL   = 0x4000002E,   // Don't enable the UI which allows users to rename mapped drive labels
    REST_NOCHANGEMAPPEDDRIVECOMMENT = 0x4000002F,   // Don't enable the UI which allows users to change mapped drive comments
    REST_MaxRecentDocs              = 0x40000030,
    REST_NONETWORKCONNECTIONS       = 0x40000031,   // No Start Menu | Settings |Network Connections
    REST_FORCESTARTMENULOGOFF       = 0x40000032,   // Force logoff on the Start Menu
    REST_NOWEBVIEW                  = 0x40000033,   // Disable Web View
    REST_NOCUSTOMIZETHISFOLDER      = 0x40000034,   // Disable Customize This Folder
    REST_NOENCRYPTION               = 0x40000035,   // Don't allow file encryption
    REST_ALLOWFRENCRYPTION          = 0x40000036,   // Allow encryption in France (Do not publish)
    REST_DONTSHOWSUPERHIDDEN        = 0x40000037,   // don't show super hidden files
    REST_NOSHELLSEARCHBUTTON        = 0x40000038,
    REST_NOHARDWARETAB              = 0x40000039,   // No Hardware tab on Drives or in control panel
    REST_NORUNASINSTALLPROMPT       = 0x4000003A,   // Don't bring up "Run As" prompt for install programs
    REST_PROMPTRUNASINSTALLNETPATH  = 0x4000003B,   // Force the  "Run As" prompt for install programs on unc/network shares
    REST_NOMANAGEMYCOMPUTERVERB     = 0x4000003C,   // No Manage verb on My Computer
    REST_NORECENTDOCSNETHOOD        = 0x4000003D,   // dont add the recent docs shares to nethood
    REST_DISALLOWRUN                = 0x4000003E,   // don't allow certain apps to be run
    REST_NOWELCOMESCREEN            = 0x4000003F,   // don't allow the welcome screen to be displayed.
    REST_RESTRICTCPL                = 0x40000040,   // only allow certain cpls to be run
    REST_DISALLOWCPL                = 0x40000041,   // don't allow certain cpls to be run
    REST_NOSMBALLOONTIP             = 0x40000042,   // No Start Menu Balloon Tip
    REST_NOSMHELP                   = 0x40000043,   // No Help on the Start Menu
    REST_NOWINKEYS                  = 0x40000044,   // No Windows-X Hot keys
    REST_NOENCRYPTONMOVE            = 0x40000045,   // Don't automatically try to encrypt files that are moved to encryped directories
    REST_NOLOCALMACHINERUN          = 0x40000046,   // ignore HKLM\sw\ms\win\cv\Run and all of it's sub keys
    REST_NOCURRENTUSERRUN           = 0x40000047,   // ignore HKCU\sw\ms\win\cv\Run and all of it's sub keys
    REST_NOLOCALMACHINERUNONCE      = 0x40000048,   // ignore HKLM\sw\ms\win\cv\RunOnce and all of it's sub keys
    REST_NOCURRENTUSERRUNONCE       = 0x40000049,   // ignore HKCU\sw\ms\win\cv\RunOnce and all of it's sub keys
    REST_FORCEACTIVEDESKTOPON       = 0x4000004A,   // Force ActiveDesktop to be turned ON all the time.
    REST_NOCOMPUTERSNEARME          = 0x4000004B,   // removes the "Computers near me" link
    REST_NOVIEWONDRIVE              = 0x4000004C,   // disallows CreateViewObject() on specified drives (CFSFolder only)

// BUGBUG: Space is taken by Millennium changes
    REST_NOSMMYDOCS                 = 0x4000004F,   // Don't show the My Documents item on the Start Menu.
#ifdef WINNT // hydra specific ids
    REST_NODISCONNECT               = 0x41000001,   // No Disconnect option in Start menu
    REST_NOSECURITY                 = 0x41000002,   // No Security option in start menu
    REST_NOFILEASSOCIATE            = 0x41000003,   // Do not allow user to change file association
#endif
} RESTRICTIONS;


// Browser restrictions (parameter for SHRestricted2)
//
// The browser restrictions are divided into two ranges, "explorer restrictions" and
// "infodelivery restrictions".  Explorer restrictions are kept under "Software\\Microsoft\\
// Windows\\CurrentVersion\\Policies\\Explorer," while infodelivery restrictions are kept
// under "Software\\Policies\\Microsoft\\Internet Explorer\\Infodelivery\\Restrictions."
//
// **NOTE**: SHRestricted2 assumes each range is continuous.
typedef enum {
    // explorer restrictions
    REST_BROWSER_NONE               = 0x00000000,   // REST_EXPLORER_FIRST
    REST_NOTOOLBARCUSTOMIZE         = 0x00000001,
    REST_NOBANDCUSTOMIZE            = 0x00000002,
    REST_SMALLICONS                 = 0x00000003,
    REST_LOCKICONSIZE               = 0x00000004,
    REST_SPECIFYDEFAULTBUTTONS      = 0x00000005,
    REST_BTN_BACK                   = 0x00000006,
    REST_BTN_FORWARD                = 0x00000007,
    REST_BTN_STOPDOWNLOAD           = 0x00000008,
    REST_BTN_REFRESH                = 0x00000009,
    REST_BTN_HOME                   = 0x0000000A,
    REST_BTN_SEARCH                 = 0x0000000B,
    REST_BTN_HISTORY                = 0x0000000C,
    REST_BTN_FAVORITES              = 0x0000000D,
    REST_BTN_ALLFOLDERS             = 0x0000000E,
    REST_BTN_THEATER                = 0x0000000F,
    REST_BTN_TOOLS                  = 0x00000010,
    REST_BTN_MAIL                   = 0x00000011,
    REST_BTN_FONTS                  = 0x00000012,
    REST_BTN_PRINT                  = 0x00000013,
    REST_BTN_EDIT                   = 0x00000014,
    REST_BTN_DISCUSSIONS            = 0x00000015,
    REST_BTN_CUT                    = 0x00000016,
    REST_BTN_COPY                   = 0x00000017,
    REST_BTN_PASTE                  = 0x00000018,
    REST_BTN_ENCODING               = 0x00000019,
    REST_NoUserAssist               = 0x0000001A,
    REST_NoWindowsUpdate            = 0x0000001B,
    REST_NoExpandedNewMenu          = 0x0000001C,
    REST_NOFILEURL                  = 0x0000001D,   // REST_EXPLORER_LAST

    // infodelivery restrictions
    REST_NoChannelUI                = 0x50000001,   // REST_INFO_FIRST
    REST_NoAddingChannels           = 0x50000002,
    REST_NoEditingChannels          = 0x50000003,
    REST_NoRemovingChannels         = 0x50000004,
    REST_NoAddingSubscriptions      = 0x50000005,
    REST_NoEditingSubscriptions     = 0x50000006,
    REST_NoRemovingSubscriptions    = 0x50000007,
    REST_NoChannelLogging           = 0x50000008,
    REST_NoManualUpdates            = 0x50000009,
    REST_NoScheduledUpdates         = 0x5000000A,
    REST_NoUnattendedDialing        = 0x5000000B,
    REST_NoChannelContent           = 0x5000000C,
    REST_NoSubscriptionContent      = 0x5000000D,
    REST_NoEditingScheduleGroups    = 0x5000000E,
    REST_MaxChannelSize             = 0x5000000F,
    REST_MaxSubscriptionSize        = 0x50000010,
    REST_MaxChannelCount            = 0x50000011,
    REST_MaxSubscriptionCount       = 0x50000012,
    REST_MinUpdateInterval          = 0x50000013,
    REST_UpdateExcludeBegin         = 0x50000014,
    REST_UpdateExcludeEnd           = 0x50000015,
    REST_UpdateInNewProcess         = 0x50000016,
    REST_MaxWebcrawlLevels          = 0x50000017,
    REST_MaxChannelLevels           = 0x50000018,
    REST_NoSubscriptionPasswords    = 0x50000019,
    REST_NoBrowserSaveWebComplete   = 0x5000001A,
    REST_NoSearchCustomization      = 0x5000001B,
    REST_NoSplash                   = 0x5000001C,  // REST_INFO_LAST

    // restrictions ported from SP2
    REST_NoFileOpen                 = 0x60000001,  // REST_BROWSER_FIRST
    REST_NoFileNew                  = 0x60000002,
    REST_NoBrowserSaveAs            = 0x60000003,
    REST_NoBrowserOptions           = 0x60000004,
    REST_NoFavorites                = 0x60000005,
    REST_NoSelectDownloadDir        = 0x60000006,
    REST_NoBrowserContextMenu       = 0x60000007,
    REST_NoBrowserClose             = 0x60000008,
    REST_NoOpeninNewWnd             = 0x60000009,
    REST_NoTheaterMode              = 0x6000000A,
    REST_NoFindFiles                = 0x6000000B,
    REST_NoViewSource               = 0x6000000C,
    REST_GoMenu                     = 0x6000000D,
    REST_NoToolbarOptions           = 0x6000000E,

    REST_NoHelpItem_TipOfTheDay     = 0x6000000F,
    REST_NoHelpItem_NetscapeHelp    = 0x60000010,
    REST_NoHelpItem_Tutorial        = 0x60000011,
    REST_NoHelpItem_SendFeedback    = 0x60000012,
    REST_AlwaysPromptWhenDownload   = 0x60000013,

    REST_NoNavButtons               = 0x60000014,
    REST_NoHelpMenu                 = 0x60000015,
    REST_NoBrowserBars              = 0x60000016,
    REST_NoToolBar                  = 0x60000017,
    REST_NoAddressBar               = 0x60000018,
    REST_NoLinksBar                 = 0x60000019,  // REST_BROWSER_LAST

} BROWSER_RESTRICTIONS;

#define REST_EXPLORER_FIRST     REST_BROWSER_NONE
#define REST_EXPLORER_LAST      REST_NOFILEURL

#define REST_INFO_FIRST         REST_NoChannelUI
#define REST_INFO_LAST          REST_NoSplash

#define REST_BROWSER_FIRST      REST_NoFileOpen
#define REST_BROWSER_LAST       REST_NoLinksBar

// codes for REST_BTN policies
#define RESTOPT_BTN_STATE_DEFAULT       0   // must be zero, else break browseui\itbar.cpp assumption
#define RESTOPT_BTN_STATE_VISIBLE       1
#define RESTOPT_BTN_STATE_HIDDEN        2

// REST_INTELLIMENUS
#define RESTOPT_INTELLIMENUS_USER       0
#define RESTOPT_INTELLIMENUS_DISABLED   1       // Match Restriction assumption: 1 == Off
#define RESTOPT_INTELLIMENUS_ENABLED    2


// FTP Exports
STDAPI IsIEDefautlFTPClient(void);
STDAPI MakeIEDefautlFTPClient(void);
STDAPI RestoreFTPClient(void);


SHSTDAPI_(void) SHSettingsChanged(WPARAM wParam, LPARAM lParam);
SHSTDAPI_(BOOL) SHIsBadInterfacePtr(LPCVOID pv, UINT cbVtbl);
SHSTDAPI_(IStream *) OpenRegStream(HKEY hkey, LPCTSTR pszSubkey, LPCTSTR pszValue, DWORD grfMode);
SHSTDAPI_(void) SHHandleDiskFull(HWND hwnd, int idDrive);

SHSTDAPI_(BOOL) SHFindFiles(LPCITEMIDLIST pidlFolder, LPCITEMIDLIST pidlSaveFile);
SHSTDAPI_(BOOL) SHFindComputer(LPCITEMIDLIST pidlFolder, LPCITEMIDLIST pidlSaveFile);

SHSTDAPI_(void) PathGetShortPath(LPTSTR pszLongPath);
SHSTDAPI_(BOOL) PathYetAnotherMakeUniqueName(LPTSTR  pszUniqueName, LPCTSTR pszPath, LPCTSTR pszShort, LPCTSTR pszFileSpec);

SHSTDAPI_(BOOL) Win32CreateDirectory(LPCTSTR pszPath, SECURITY_ATTRIBUTES *psa);
SHSTDAPI_(BOOL) Win32RemoveDirectory(LPCTSTR pszPath);
SHSTDAPI_(BOOL) Win32DeleteFile(LPCTSTR pszPath);

SHSTDAPI_(void) SHUpdateRecycleBinIcon();

//
// Path processing function
//

#define PPCF_ADDQUOTES               0x00000001        // return a quoted name if required
#define PPCF_ADDARGUMENTS            0x00000003        // appends arguments (and wraps in quotes if required)
#define PPCF_NODIRECTORIES           0x00000010        // don't match to directories
#define PPCF_NORELATIVEOBJECTQUALIFY 0x00000020        // don't return fully qualified relative objects
#define PPCF_FORCEQUALIFY            0x00000040        // qualify even non-relative names
#define PPCF_LONGESTPOSSIBLE         0x00000080        // always find the longest possible name

SHSTDAPI_(LONG) PathProcessCommand(LPCTSTR lpSrc, LPTSTR lpDest, int iMax, DWORD dwFlags);
SHSTDAPI_(LPITEMIDLIST) SHLogILFromFSIL(LPCITEMIDLIST pidlFS);
SHSTDAPI_(BOOL) StrRetToStrN(LPTSTR szOut, UINT uszOut, STRRET *pStrRet, LPCITEMIDLIST pidl);
SHSTDAPI_(DWORD) SHWaitForFileToOpen(LPCITEMIDLIST pidl, UINT uOptions, DWORD dwtimeout);
SHSTDAPI_(void) SetAppStartingCursor(HWND hwnd, BOOL fSet);
SHSTDAPI_(DWORD) SHRestricted(RESTRICTIONS rest);
SHSTDAPI_(void *) SHGetHandlerEntry(LPCTSTR szHandler, LPCSTR szProcName, HINSTANCE *lpModule);


SHSTDAPI_(BOOL) SignalFileOpen(LPCITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) SHSimpleIDListFromPath(LPCTSTR pszPath);

SHSTDAPI SHCreatePropertyBag(REFIID riid, void **ppv);

SHSTDAPI_(DWORD) SHNetConnectionDialog(HWND hwnd, LPTSTR pszRemoteName, DWORD dwType);
SHSTDAPI SHLoadOLE(LPARAM lParam);
SHSTDAPI_(void) Desktop_UpdateBriefcaseOnEvent(HWND hwnd, UINT uEvent);


SHSTDAPI SHStartNetConnectionDialogA(HWND hwnd, LPCSTR pszRemoteName, DWORD dwType);
SHSTDAPI SHStartNetConnectionDialogW(HWND hwnd, LPCWSTR pszRemoteName, DWORD dwType);
#ifdef UNICODE
#define SHStartNetConnectionDialog  SHStartNetConnectionDialogW
#else
#define SHStartNetConnectionDialog  SHStartNetConnectionDialogA
#endif // !UNICODE
SHSTDAPI SHDefExtractIconA(LPCSTR pszIconFile, int iIndex, UINT uFlags,
                           HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize);
SHSTDAPI SHDefExtractIconW(LPCWSTR pszIconFile, int iIndex, UINT uFlags,
                           HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize);
#ifdef UNICODE
#define SHDefExtractIcon  SHDefExtractIconW
#else
#define SHDefExtractIcon  SHDefExtractIconA
#endif // !UNICODE
SHSTDAPI_(int) SHLookupIconIndexA(LPCSTR pszFile, int iIconIndex, UINT uFlags);
SHSTDAPI_(int) SHLookupIconIndexW(LPCWSTR pszFile, int iIconIndex, UINT uFlags);
#ifdef UNICODE
#define SHLookupIconIndex  SHLookupIconIndexW
#else
#define SHLookupIconIndex  SHLookupIconIndexA
#endif // !UNICODE
SHSTDAPI_(UINT) SHExtractIconsA(LPCSTR pszFileName, int nIconIndex, int cxIcon, int cyIcon,
                                HICON *phicon, UINT *piconid, UINT nIcons, UINT flags);
SHSTDAPI_(UINT) SHExtractIconsW(LPCWSTR pszFileName, int nIconIndex, int cxIcon, int cyIcon,
                                HICON *phicon, UINT *piconid, UINT nIcons, UINT flags);
#ifdef UNICODE
#define SHExtractIcons  SHExtractIconsW
#else
#define SHExtractIcons  SHExtractIconsA
#endif // !UNICODE
SHSTDAPI SHGetAssociations(LPCITEMIDLIST pidl, void **ppvQueryAssociations);

#define SHLookupIconIndexAORD   7
#define SHLookupIconIndexWORD   8
#ifdef UNICODE
#define SHLookupIconIndexORD    SHLookupIconIndexWORD
#else
#define SHDefExtractIcon    SHDefExtractIconA
#endif


// BUGBUG (scotth): temporary.  move to shdocvw.
// OpenAsInfo flags
#define OAIF_ALLOW_REGISTRATION     0x00000001      // enable the "always use this file" checkbox (NOTE if you dont pass this, it will be disabled)
#define OAIF_REGISTER_EXT           0x00000002      // do the registration after the user hits "ok"
#define OAIF_EXEC                   0x00000004      // execute file after registering
#define OAIF_FORCE_REGISTRATION     0x00000008      // force the "always use this file" checkbox to be checked (normally, you wont use the OAIF_ALLOW_REGISTRATION wen you pass this)
#define OAIF_ALL                    (OAIF_ALLOW_REGISTRATION | OAIF_REGISTER_EXT | OAIF_EXEC | OAIF_FORCE_REGISTRATION)  //

typedef struct _openasinfo
{
    LPCTSTR pcszFile;           // [in] file name
    LPCTSTR pcszClass;          // [in] file class description.  NULL means
                                //      use pcszFile's extension
    DWORD dwInFlags;            // [in] input flags from OAIF_*

    TCHAR szApp[MAX_PATH];      // [out] application selected
} OPENASINFO, * POPENASINFO;

SHSTDAPI OpenAsDialog(HWND hwnd, POPENASINFO poainfo);

//
// Interface pointer validation
//
#define IsBadInterfacePtr(pitf, ITF)  SHIsBadInterfacePtr(pitf, sizeof(ITF##Vtbl))

//===========================================================================
// Another block of private API
//===========================================================================

// indexes into the shell image lists (Shell_GetImageList) for default images
// If you add to this list, you also need to update II_LASTSYSICON!

#define II_DOCNOASSOC         0  // document (blank page) (not associated)
#define II_DOCUMENT           1  // document (with stuff on the page)
#define II_APPLICATION        2  // application (exe, com, bat)
#define II_FOLDER             3  // folder (plain)
#define II_FOLDEROPEN         4  // folder (open)
#define II_DRIVE525           5
#define II_DRIVE35            6
#define II_DRIVEREMOVE        7
#define II_DRIVEFIXED         8
#define II_DRIVENET           9
#define II_DRIVENETDISABLED  10
#define II_DRIVECD           11
#define II_DRIVERAM          12
#define II_WORLD             13
#define II_NETWORK           14
#define II_SERVER            15
#define II_PRINTER           16
#define II_MYNETWORK         17
#define II_GROUP             18
// Startmenu images.
#define II_STPROGS           19
#define II_STDOCS            20
#define II_STSETNGS          21
#define II_STFIND            22
#define II_STHELP            23
#define II_STRUN             24
#define II_STSUSPEND         25
#define II_STEJECT           26
#define II_STSHUTD           27

#define II_SHARE             28
#define II_LINK              29
#define II_SLOWFILE          30
#define II_RECYCLER          31
#define II_RECYCLERFULL      32
#define II_RNA               33
#define II_DESKTOP           34

// More startmenu image.
#define II_STCPANEL          35
#define II_STSPROGS          36
#define II_STPRNTRS          37
#define II_STFONTS           38
#define II_STTASKBR          39

#define II_CDAUDIO           40
#define II_TREE              41
#define II_STCPROGS          42
#define II_STFAVORITES       43
#define II_STLOGOFF          44
#define II_STFLDRPROP        45
#define II_WINUPDATE         46

#define II_MU_STSECURITY     47
#define II_MU_STDISCONN      48

#ifdef WINNT // hydra specific id
#define II_LASTSYSICON       II_MU_STDISCONN
#else
// Last system image list icon index - used by icon cache manager
#define II_LASTSYSICON       II_WINUPDATE
#endif

// Overlay indexes
#define II_OVERLAYFIRST      II_SHARE
#define II_OVERLAYLAST       II_SLOWFILE

#define II_NDSCONTAINER      72
#define II_SERVERSHARE       73

SHSTDAPI_(BOOL) FileIconInit( BOOL fRestoreCache );

SHSTDAPI_(BOOL) Shell_GetImageLists(HIMAGELIST *phiml, HIMAGELIST *phimlSmall);
SHSTDAPI_(void) Shell_SysColorChange(void);
SHSTDAPI_(int)  Shell_GetCachedImageIndex(LPCTSTR pszIconPath, int iIconIndex, UINT uIconFlags);


// A usefull function in Defview for mapping idlist into index into system
// image list.  Optionally it can also look up the index of the selected
// icon.
SHSTDAPI_(int) SHMapPIDLToSystemImageListIndex(IShellFolder *pshf, LPCITEMIDLIST pidl, int *piIndexSel);
//
// OLE string
//
SHSTDAPI_(int) OleStrToStrN(LPTSTR, int, LPCOLESTR, int);
SHSTDAPI_(int) StrToOleStrN(LPOLESTR, int, LPCTSTR, int);
SHSTDAPI_(int) OleStrToStr(LPTSTR, LPCOLESTR);
SHSTDAPI_(int) StrToOleStr(LPOLESTR, LPCTSTR);


// Tray CopyData Messages
#define TCDM_APPBAR     0x00000000
#define TCDM_NOTIFY     0x00000001
#define TCDM_LOADINPROC 0x00000002

//
//  new navigation stack interfaces

#undef  INTERFACE
#define INTERFACE   ITravelEntry

DECLARE_INTERFACE_(ITravelEntry, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** ITravelEntry specific methods
    STDMETHOD(Invoke)(THIS_ IUnknown *punk) PURE;
    STDMETHOD(Update)(THIS_ IUnknown *punk, BOOL fIsLocalAnchor) PURE;
    STDMETHOD(GetPidl)(THIS_ LPITEMIDLIST *ppidl) PURE;
};

#undef  INTERFACE
#define INTERFACE   ITravelLog

#define TLOG_BACK   -1
#define TLOG_FORE   1

#define TLMENUF_INCLUDECURRENT                  0x00000001
#define TLMENUF_CHECKCURRENT                    (TLMENUF_INCLUDECURRENT | 0x00000002)
#define TLMENUF_BACK                            0x00000010  // Default
#define TLMENUF_FORE                            0x00000020
#define TLMENUF_BACKANDFORTH                    (TLMENUF_BACK | TLMENUF_FORE | TLMENUF_INCLUDECURRENT)

DECLARE_INTERFACE_(ITravelLog, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** ITravelLog specific methods
    STDMETHOD(AddEntry)(THIS_ IUnknown *punk, BOOL fIsLocalAnchor) PURE;
    STDMETHOD(UpdateEntry)(THIS_ IUnknown *punk, BOOL fIsLocalAnchor) PURE;
    STDMETHOD(UpdateExternal)(THIS_ IUnknown *punk, IUnknown *punkHLBrowseContext) PURE;
    STDMETHOD(Travel)(THIS_ IUnknown *punk, int iOffset) PURE;
    STDMETHOD(GetTravelEntry)(THIS_ IUnknown *punk, int iOffset, ITravelEntry **ppte) PURE;
    STDMETHOD(FindTravelEntry)(THIS_ IUnknown *punk, LPCITEMIDLIST pidl, ITravelEntry **ppte) PURE;
    STDMETHOD(GetToolTipText)(THIS_ IUnknown *punk, int iOffset, int idsTemplate, LPWSTR pwzText, DWORD cchText) PURE;
    STDMETHOD(InsertMenuEntries)(THIS_ IUnknown *punk, HMENU hmenu, int nPos, int idFirst, int idLast, DWORD dwFlags) PURE;
    STDMETHOD(Clone)(THIS_ ITravelLog **pptl) PURE;
    STDMETHOD_(DWORD, CountEntries)(THIS_ IUnknown *punk) PURE;
    STDMETHOD(Revert)(void) PURE;
};


//
// Private QueryContextMenuFlag passed from Bands
//
#define CMF_BANDCMD      0x00020000     // Install Band Context menu commands

//
// Functions to help the cabinets sync to each other
//  uOptions parameter to SHWaitForFileOpen
//
#define WFFO_WAITTIME 10000L

#define WFFO_ADD        0x0001
#define WFFO_REMOVE     0x0002
#define WFFO_WAIT       0x0004
#define WFFO_SIGNAL     0x0008



//
// NOTES: IShellService is used when we share a service component
//  (which implements a certain interface) among multiple clients
//  (such as IE 3.0 and Explorer). The client always CoCreateInstance
//  it and call SetOwner(this). When the client is going away
//  (typically when the window is closed), it calls SetOwner(NULL)
//  to let the service object releases the reference to the owner
//  object.
//
#undef  INTERFACE
#define INTERFACE   IShellService

DECLARE_INTERFACE_(IShellService, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellService specific methods ***
    STDMETHOD(SetOwner)(THIS_ struct IUnknown* punkOwner) PURE;
};


//
// NOTES: IHistSFPrivate is used when manipulating a history IShellFolder object
//      It includes methods for pointing the shell folder at the correct virtual
//      directory.  QIing for IHistSFPrivate also guarantees the pidl format can
//      safely be delved into to access the INTERNET_CACHE_ENTRY_INFO structure
//
#undef  INTERFACE
#define INTERFACE   IHistSFPrivate

DECLARE_INTERFACE_(IHistSFPrivate, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IHistPrivate specific methods ***
    STDMETHOD(SetCachePrefix)(THIS_ LPCWSTR pszCachePrefix) PURE;
    STDMETHOD(SetDomain)(THIS_ LPCWSTR pszDomain) PURE;
    STDMETHOD(WriteHistory)(THIS_ LPCWSTR pszPrefixedUrl, FILETIME ftExpires, FILETIME ftModified, LPITEMIDLIST * ppidlSelect) PURE;
    STDMETHOD(ClearHistory) (THIS) PURE;
};

//
// NOTES:  IShellFolderViewType lets a shell folder support different "views" on
//      its contents (meaning different hierarchical layouts of its data) with
//      the default "view" being the one the shell folder displays normally.
//      This enumerator returns pidls that are special hidden folders at the
//      top level of the shell folder (which are not otherwise enumerated).
#undef  INTERFACE
#define INTERFACE   IShellFolderViewType
DECLARE_INTERFACE_(IShellFolderViewType, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)  (THIS) PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderViewType Methods ***

    // NOTE: "Views" are seen by the user as hidden folders off the
    //       root (represented by pidls).  Whenever appropriate, the
    //       default view (coming off the root folder) is represented
    //       as the NULL *or* empty pidl.

    // EnumViews:
    //   Return an enumerator which will give out one pidl for every extended view.
    STDMETHOD(EnumViews)(THIS_ ULONG grfFlags, IEnumIDList **ppenum) PURE;

    // GetDefaultViewName:
    //   Return the name of the default view.  The names of the other views
    //   can be retrieved by calling GetDisplayNameOf.
    STDMETHOD(GetDefaultViewName)(THIS_ DWORD  uFlags, LPWSTR *ppwszName)      PURE;
    STDMETHOD(GetViewTypeProperties)(THIS_ LPCITEMIDLIST pidl, DWORD *pdwFlags)  PURE;

    // TranslateViewPidl:
    //   Attempt to take a pidl represented in one heirarchical representation of
    //   the shell folder, and find it in a different representation.
    //   pidl should be relative to the root folder.
    //   Remember to ILFree ppidlOut
    STDMETHOD(TranslateViewPidl)(THIS_ LPCITEMIDLIST pidl, LPCITEMIDLIST pidlView, LPITEMIDLIST *ppidlOut) PURE;
};

#define SFVTFLAG_NOTIFY_CREATE  0x00000001
#define SFVTFLAG_NOTIFY_RESORT  0x00000002

//
// NOTES: IShellFolderSearchableCallback allows the searcher to provide
//          callback routines that moniter the search process
//
#undef  INTERFACE
#define INTERFACE IShellFolderSearchableCallback
DECLARE_INTERFACE_(IShellFolderSearchableCallback, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)  (THIS) PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderSearchableCallback Methods ***

    // NOTE: Caller may pass NULL for pVar or pdwFlags as a legal value
    STDMETHOD(RunBegin)(DWORD dwReserved) PURE;
    STDMETHOD(RunEnd)(DWORD dwReserved) PURE;
};

//
// NOTES: IShellFolderSearchable allows a shell extension to provide a searchable
//        namespace.
#undef  INTERFACE
#define INTERFACE IShellFolderSearchable
DECLARE_INTERFACE_(IShellFolderSearchable, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)  (THIS) PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderSearchable methods ***

    // FindString -
    //  The returned shell folder's enumerator will have any
    //   search hits for the given search string.
    //  As no flags are currently defined, we suggest you pass
    //    NULL as lpdword
    //  punkOnAsyncSearch will be QI'd for IShellFolderSearchableCallback
    STDMETHOD(FindString)(THIS_ LPCWSTR pwszTarget, DWORD *pdwFlags,
                          IUnknown *punkOnAsyncSearch, LPITEMIDLIST *ppidlOut)   PURE;
    // CancelAsyncSearch -
    //   Begins the process of cancelling  any pending
    //    asynchronous search from this pidl.
    //    When the search is actually cancelled, RunEnd will be called
    //   Returns: S_OK => cancelling, S_FALSE => not running
    STDMETHOD(CancelAsyncSearch) (THIS_ LPCITEMIDLIST pidlSearch, DWORD *pdwFlags) PURE;

    // InvalidateSearch -
    //   Makes this pidl no longer a valid portion of the shell folder
    //    also does some cleanup of any databases used in the search and
    //    will cause the eventual release of the IRunHook callback
    //   May cause async search to be cancelled
    STDMETHOD(InvalidateSearch)  (THIS_ LPCITEMIDLIST pidlSearch, DWORD *pdwFlags) PURE;
};

//
// NOTES: IBandSiteHelper is used to let explorer's BandSite implementation,
//  which aggregates shdocvw's BandSite, provide callback hooks for shdocvw
//  to call.
//
#undef  INTERFACE
#define INTERFACE   IBandSiteHelper

DECLARE_INTERFACE_(IBandSiteHelper, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IBandSiteHelper specific methods ***
    STDMETHOD(LoadFromStreamBS)(THIS_ struct IStream* pstm, REFIID riid, void **ppv) PURE;
    STDMETHOD(SaveToStreamBS)(THIS_ struct IUnknown* punk, struct IStream* pstm) PURE;
};

// This private interface is added such that some of the support features that is in
// SHDocvw's Internet explorer frame automation code, that is not part of publicly
// defined interfaces can be used in the Explorer code...
//
// Note: FindCIE4ConnectionPoint was a hack for IE4.  New code should use
//       shlwapi helper functions like IConnectionPoint_Invoke instead.
//

#ifdef __cplusplus
class CIE4ConnectionPoint;
#else
typedef struct CIE4ConnectionPoint CIE4ConnectionPoint;
#endif

#undef  INTERFACE
#define INTERFACE   IExpDispSupport

DECLARE_INTERFACE_(IExpDispSupport, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IExpDispSupport specific methods ***
    STDMETHOD(FindCIE4ConnectionPoint)(THIS_ REFIID riid, CIE4ConnectionPoint **ppccp) PURE;
    STDMETHOD(OnTranslateAccelerator)(MSG  *pMsg, DWORD grfModifiers) PURE;
    STDMETHOD(OnInvoke)(THIS_ DISPID dispidMember, REFIID iid, LCID lcid, WORD wFlags, DISPPARAMS *pdispparams,
                        VARIANT *pVarResult, EXCEPINFO *pexcepinfo, UINT *puArgErr) PURE;
};

//
// This interface is related to the above. It contains functions
// that would only be implemented on the WebBrowserOC version if IExpDispSupport.
//
#undef  INTERFACE
#define INTERFACE   IExpDispSupportOC
DECLARE_INTERFACE_(IExpDispSupportOC, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IExpDispSupportOC specific methods ***
    STDMETHOD(OnOnControlInfoChanged)() PURE;
    STDMETHOD(GetDoVerbMSG)(MSG *pMsg) PURE;
};



//===========================================================================
// IConnectionPointCB interface
#undef  INTERFACE
#define INTERFACE  IConnectionPointCB
DECLARE_INTERFACE_(IConnectionPointCB, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IConnectionPointCB Methods ***
    STDMETHOD(OnAdvise) (REFIID iid, DWORD cSinks, ULONG_PTR dwCookie) PURE;
    STDMETHOD(OnUnadvise) (REFIID iid, DWORD cSinks, ULONG_PTR dwCookie) PURE;

};

//===========================================================================
// IDelegateDropTargetCB interface
//
// This interface is used by CDelegateDropTarget implementation in shdocvw
// to generate an IDropTarget implementation that delegates to different
// IDropTarget implementations depending on the point currently over. The
// implementor of IDelegateDropTargetCB can handle feedback, scrolling,
// hit testing, and returning interfaces for ids returned from hit testing.
//
// GetWindows returns the hwnd to lock and the hwnd to scroll (not necesarily
//            the same window, as you often want to lock the parent of the window
//            you want to scroll)
//
// HitTest    should return an id of the object over as well as
//            uptate UI (highlights, etc). if ppt is NULL
//            that means remove any UI as the drag is terminating.
//            this should be able to handle hittesting on points not
//            even inside hwndScroll.
//
// GetObject  returns an interface for an id returned from HitTest
//
// OnDrop     gives the CB a chance to take action on the drop.
//            returning S_FALSE from this function prevents
//            CDelegateDropTarget from calling pdt->Drop(...).
//
#undef  INTERFACE
#define INTERFACE  IDelegateDropTargetCB
DECLARE_INTERFACE_(IDelegateDropTargetCB, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IDelegateDropTargetCB Methods ***
    STDMETHOD(GetWindows) (THIS_ HWND * phwndLock, HWND * phwndScroll) PURE;
    STDMETHOD(HitTest) (THIS_ LPPOINT ppt, DWORD * pdwId) PURE;
    STDMETHOD(GetObject) (THIS_ DWORD dwId, REFIID riid, void **ppv) PURE;
    STDMETHOD(OnDrop) (THIS_ IDropTarget *pdt, IDataObject *pdtobj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) PURE;

};


#ifdef __COMMCTRL_DA_DEFINED__ // we need HDPAs for this interface
//
// IOrderList - for ordering info in favorites/channels
//
// Typical usage is: GetOrderList, AllocOrderItem, insert into correct
// position, SetOrderList, and then FreeOrderList.
//
typedef struct
{
    LPITEMIDLIST    pidl;       // IDlist for this item
    int             nOrder;     // Ordinal indicating user preference
    DWORD           lParam;     // store custom order info.
} ORDERITEM, * PORDERITEM;

// Values for SortOrderList
#define OI_SORTBYNAME       0
#define OI_SORTBYORDINAL    1
#define OI_MERGEBYNAME      2

#undef  INTERFACE
#define INTERFACE  IOrderList
DECLARE_INTERFACE_(IOrderList, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IOrderList Methods ***
    STDMETHOD(GetOrderList)(THIS_ HDPA * phdpa) PURE;
    STDMETHOD(SetOrderList)(THIS_ HDPA hdpa, IShellFolder *psf) PURE;
    STDMETHOD(FreeOrderList)(THIS_ HDPA hdpa) PURE;
    STDMETHOD(SortOrderList)(THIS_ HDPA hdpa, DWORD dw) PURE;
    STDMETHOD(AllocOrderItem)(THIS_ PORDERITEM * ppoi, LPCITEMIDLIST pidl) PURE;
    STDMETHOD(FreeOrderItem)(THIS_ PORDERITEM poi) PURE;
};

#undef  INTERFACE
#define INTERFACE  IOrderList2
DECLARE_INTERFACE_(IOrderList2, IOrderList)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IOrderList Methods ***
    STDMETHOD(GetOrderList)(THIS_ HDPA * phdpa) PURE;           // BUGBUG (lamadio): This assumes Favorites Menu
    STDMETHOD(SetOrderList)(THIS_ HDPA hdpa, IShellFolder *psf) PURE; // BUGBUG (lamadio): This assumes Favorites Menu
    STDMETHOD(FreeOrderList)(THIS_ HDPA hdpa) PURE;
    STDMETHOD(SortOrderList)(THIS_ HDPA hdpa, DWORD dw) PURE;
    STDMETHOD(AllocOrderItem)(THIS_ PORDERITEM * ppoi, LPCITEMIDLIST pidl) PURE;
    STDMETHOD(FreeOrderItem)(THIS_ PORDERITEM poi) PURE;

    // *** IOrderList2
    STDMETHOD(LoadFromStream)(THIS_ IStream* pstm, HDPA* phdpa, IShellFolder* psf) PURE;
    STDMETHOD(SaveToStream)(THIS_ IStream* pstm, HDPA hdpa) PURE;
};

#endif


//===========================================================================
// IShellHTMLWindowSupport private COmWindow interface. This should never be exposed.
#undef  INTERFACE
#define INTERFACE  IShellHTMLWindowSupport
DECLARE_INTERFACE_(IShellHTMLWindowSupport, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    STDMETHOD(ViewReleased)() PURE;
    STDMETHOD(ViewActivated)() PURE;
    STDMETHOD(ReadyStateChangedTo)( long, IShellView* ) PURE;
    STDMETHOD(CanNavigate)() PURE;

};



//===========================================================================
// IBandProxy private COmWindow interface. This should never be exposed.
#undef INTERFACE
#define INTERFACE IBandProxy
DECLARE_INTERFACE_(IBandProxy, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IBandProxy Methods ***
    STDMETHOD(SetSite) (THIS_ IUnknown* punkSite) PURE;
    STDMETHOD(CreateNewWindow) (THIS_ IUnknown** ppunk) PURE;
    STDMETHOD(GetBrowserWindow) (THIS_ IUnknown** ppunk) PURE;
    STDMETHOD(IsConnected) (THIS) PURE;
    STDMETHOD(NavigateToPIDL) (THIS_ LPCITEMIDLIST pidl) PURE;
    STDMETHOD(NavigateToURL) (THIS_ LPCWSTR wzUrl, VARIANT * Flags) PURE;
};



//===========================================================================
//
// Make it sure that we include ole2x.h and shlobj.h
//
#if defined(__IOleInPlaceSite_FWD_DEFINED__) && defined(FCIDM_SHVIEWFIRST)

#ifdef HLINK_H

#ifndef RC_INVOKED
#include <pshpack8.h>
#endif /* !RC_INVOKED */

typedef struct {
    HWND _hwnd;
    ITravelLog  *_ptl;
    IHlinkFrame *_phlf;
    IWebBrowser2    *_pautoWB2; // use this to reference _pauto's IWebBrowser2 functions
    IExpDispSupport *_pautoEDS; // use this to reference _pauto's IExpDispSupport functions
    IShellService   *_pautoSS;  // use this to reference _pauto's IShellService functions
    int _eSecureLockIcon;
    DWORD _fCreatingViewWindow :1;
    UINT _uActivateState;   // this is the state we should use when we go active..
                            // this is here so that derived classes can set us for UI or non-UI active

    // The following pidl is used in CBaseBrowser2::GetViewStateStream because
    // at that time, neither _pidlCur nor _pidlPending are initialized.
    LPCITEMIDLIST  _pidlNewShellView;

    IOleCommandTarget* _pctView;

    LPITEMIDLIST _pidlCur;
    IShellView *_psv;
    IShellFolder *_psf; // IShellFolder for _psv (used in CVOCBrowser)
    HWND        _hwndView;
    LPWSTR      _pszTitleCur;

    LPITEMIDLIST _pidlPending;
    IShellView *_psvPending;
    IShellFolder *_psfPending;
    HWND        _hwndViewPending;
    LPWSTR      _pszTitlePending;

    BOOL _fIsViewMSHTML;

} BASEBROWSERDATA, *LPBASEBROWSERDATA;
typedef const BASEBROWSERDATA *LPCBASEBROWSERDATA;

#ifndef RC_INVOKED
#include <poppack.h>
#endif /* !RC_INVOKED */

#else
// so (unref'ed) ifaces will compile (?)
typedef LPVOID BASEBROWSERDATA;
typedef LPCVOID *LPBASEBROWSERDATA;     // BUGBUG LPVOID?
typedef LPCVOID *LPCBASEBROWSERDATA;

#endif // hlink_h

typedef struct _travellog * PTRAVELLOG;

typedef enum
{
    BNS_NORMAL = 0,     // Normal state that we are in
    BNS_BEGIN_NAVIGATE, // A Begin navigate event has happened.
    BNS_NAVIGATE       // A Navigate event has happened...
} BNSTATE;             // The navigate state...


#undef  INTERFACE
#define INTERFACE   IBrowserService

DECLARE_INTERFACE_(IBrowserService, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IBrowserService specific methods ***
    STDMETHOD(GetParentSite)(THIS_ struct IOleInPlaceSite** ppipsite) PURE;
    STDMETHOD(SetTitle)(THIS_ IShellView* psv, LPCWSTR pszName) PURE;
    STDMETHOD(GetTitle)(THIS_ IShellView* psv, LPWSTR pszName, DWORD cchName) PURE;
    STDMETHOD(GetOleObject)(THIS_ struct IOleObject** ppobjv) PURE;

    // think about this one.. I'm not sure we want to expose this -- Chee
    // BUGBUG:: Yep soon we should have interface instead.
    // My impression is that we won't document this whole interface???
    STDMETHOD(GetTravelLog)(THIS_ ITravelLog** pptl) PURE;

    STDMETHOD(ShowControlWindow)(THIS_ UINT id, BOOL fShow) PURE;
    STDMETHOD(IsControlWindowShown)(THIS_ UINT id, BOOL *pfShown) PURE;
    STDMETHOD(IEGetDisplayName)(THIS_ LPCITEMIDLIST pidl, LPWSTR pwszName, UINT uFlags) PURE;
    STDMETHOD(IEParseDisplayName)(THIS_ UINT uiCP, LPCWSTR pwszPath, LPITEMIDLIST * ppidlOut) PURE;
    STDMETHOD(DisplayParseError)(THIS_ HRESULT hres, LPCWSTR pwszPath) PURE;
    STDMETHOD(NavigateToPidl)(THIS_ LPCITEMIDLIST pidl, DWORD grfHLNF) PURE;

    STDMETHOD (SetNavigateState)(THIS_ BNSTATE bnstate) PURE;
    STDMETHOD (GetNavigateState) (THIS_ BNSTATE *pbnstate) PURE;

    STDMETHOD (NotifyRedirect) (THIS_ struct IShellView* psv, LPCITEMIDLIST pidl, BOOL *pfDidBrowse) PURE;
    STDMETHOD (UpdateWindowList) (THIS) PURE;

    STDMETHOD (UpdateBackForwardState) (THIS) PURE;

    STDMETHOD(SetFlags)(THIS_ DWORD dwFlags, DWORD dwFlagMask) PURE;
    STDMETHOD(GetFlags)(THIS_ DWORD *pdwFlags) PURE;

    // Tells if it can navigate now or not.
    STDMETHOD (CanNavigateNow) (THIS) PURE;

    STDMETHOD (GetPidl) (THIS_ LPITEMIDLIST *ppidl) PURE;
    STDMETHOD (SetReferrer) (THIS_ LPITEMIDLIST pidl) PURE;
    STDMETHOD_(DWORD, GetBrowserIndex)(THIS) PURE;
    STDMETHOD (GetBrowserByIndex)(THIS_ DWORD dwID, IUnknown **ppunk) PURE;
    STDMETHOD (GetHistoryObject)(THIS_ IOleObject **ppole, IStream **pstm, IBindCtx **ppbc) PURE;
    STDMETHOD (SetHistoryObject)(THIS_ IOleObject *pole, BOOL fIsLocalAnchor) PURE;

    STDMETHOD (CacheOLEServer)(THIS_ IOleObject *pole) PURE;

    STDMETHOD (GetSetCodePage)(THIS_ VARIANT* pvarIn, VARIANT* pvarOut) PURE;
    STDMETHOD (OnHttpEquiv)(THIS_ IShellView* psv, BOOL fDone, VARIANT* pvarargIn, VARIANT* pvarargOut) PURE;

    STDMETHOD (GetPalette)( THIS_ HPALETTE * hpal ) PURE;

    STDMETHOD (RegisterWindow)(THIS_ BOOL fUnregister, int swc) PURE;

    // Warning!  Do not add any new methods to this interface
    // because IE4 shell32.dll uses it, so changing the interface
    // breaks IE4 interop.
};

#define BSF_REGISTERASDROPTARGET 0x00000001
#define BSF_THEATERMODE          0x00000002
#define BSF_NOLOCALFILEWARNING   0x00000010
#define BSF_UISETBYAUTOMATION    0x00000100
#define BSF_RESIZABLE            0x00000200

#ifndef RC_INVOKED
#include <pshpack8.h>
#endif /* !RC_INVOKED */

typedef struct SToolbarItem {
    IDockingWindow * ptbar;
    BORDERWIDTHS    rcBorderTool;
    LPWSTR          pwszItem;
    BOOL            fShow;
    HMONITOR        hMon;
} TOOLBARITEM, *LPTOOLBARITEM;
#define ITB_VIEW        ((UINT)-1)      // view

#ifndef RC_INVOKED
#include <poppack.h>   /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

struct tagFolderSetData;

// TEMPORARY.  this is so that we don't have to keep writing dummy subs in basesb as we're
// building basesb2
DECLARE_INTERFACE_(IBrowserService2, IBrowserService)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IBrowserService specific methods ***
    // TODO: break into 3 sections (outer, inner, and both/inherit)
    STDMETHOD(GetParentSite)(THIS_ struct IOleInPlaceSite** ppipsite) PURE;
    STDMETHOD(SetTitle)(THIS_ IShellView* psv, LPCWSTR pszName) PURE;
    STDMETHOD(GetTitle)(THIS_ IShellView* psv, LPWSTR pszName, DWORD cchName) PURE;
    STDMETHOD(GetOleObject)(THIS_ struct IOleObject** ppobjv) PURE;

    // think about this one.. I'm not sure we want to expose this -- Chee
    // BUGBUG:: Yep soon we should have interface instead.
    // My impression is that we won't document this whole interface???
    STDMETHOD(GetTravelLog)(THIS_ ITravelLog** pptl) PURE;

    STDMETHOD(ShowControlWindow)(THIS_ UINT id, BOOL fShow) PURE;
    STDMETHOD(IsControlWindowShown)(THIS_ UINT id, BOOL *pfShown) PURE;
    STDMETHOD(IEGetDisplayName)(THIS_ LPCITEMIDLIST pidl, LPWSTR pwszName, UINT uFlags) PURE;
    STDMETHOD(IEParseDisplayName)(THIS_ UINT uiCP, LPCWSTR pwszPath, LPITEMIDLIST * ppidlOut) PURE;
    STDMETHOD(DisplayParseError)(THIS_ HRESULT hres, LPCWSTR pwszPath) PURE;
    STDMETHOD(NavigateToPidl)(THIS_ LPCITEMIDLIST pidl, DWORD grfHLNF) PURE;

    STDMETHOD (SetNavigateState)(THIS_ BNSTATE bnstate) PURE;
    STDMETHOD (GetNavigateState) (THIS_ BNSTATE *pbnstate) PURE;

    STDMETHOD (NotifyRedirect) (THIS_ struct IShellView* psv, LPCITEMIDLIST pidl, BOOL *pfDidBrowse) PURE;
    STDMETHOD (UpdateWindowList) (THIS) PURE;

    STDMETHOD (UpdateBackForwardState) (THIS) PURE;

    STDMETHOD(SetFlags)(THIS_ DWORD dwFlags, DWORD dwFlagMask) PURE;
    STDMETHOD(GetFlags)(THIS_ DWORD *pdwFlags) PURE;

    // Tells if it can navigate now or not.
    STDMETHOD (CanNavigateNow) (THIS) PURE;

    STDMETHOD (GetPidl) (THIS_ LPITEMIDLIST *ppidl) PURE;
    STDMETHOD (SetReferrer) (THIS_ LPITEMIDLIST pidl) PURE;
    STDMETHOD_(DWORD, GetBrowserIndex)(THIS) PURE;
    STDMETHOD (GetBrowserByIndex)(THIS_ DWORD dwID, IUnknown **ppunk) PURE;
    STDMETHOD (GetHistoryObject)(THIS_ IOleObject **ppole, IStream **pstm, IBindCtx **ppbc) PURE;
    STDMETHOD (SetHistoryObject)(THIS_ IOleObject *pole, BOOL fIsLocalAnchor) PURE;

    STDMETHOD (CacheOLEServer)(THIS_ IOleObject *pole) PURE;

    STDMETHOD (GetSetCodePage)(THIS_ VARIANT* pvarIn, VARIANT* pvarOut) PURE;
    STDMETHOD (OnHttpEquiv)(THIS_ IShellView* psv, BOOL fDone, VARIANT* pvarargIn, VARIANT* pvarargOut) PURE;

    STDMETHOD (GetPalette)( THIS_ HPALETTE * hpal ) PURE;

    STDMETHOD (RegisterWindow)(THIS_ BOOL fUnregister, int swc) PURE;

    // Stuff added for shbrowse->shbrows2 split
    // These remove "friend" functions and classes
    //
    STDMETHOD_(LRESULT, WndProcBS)(THIS_ HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) PURE;

    STDMETHOD (SetAsDefFolderSettings)(THIS) PURE;
    STDMETHOD (GetViewRect)(THIS_ RECT * prc) PURE;

    STDMETHOD (OnSize)(THIS_ WPARAM wParam) PURE;
    STDMETHOD (OnCreate)(THIS_ LPCREATESTRUCT pcs) PURE;
    STDMETHOD_(LRESULT, OnCommand)(THIS_ WPARAM wParam, LPARAM lParam) PURE;
    STDMETHOD (OnDestroy)(THIS ) PURE;
    STDMETHOD_(LRESULT, OnNotify)(THIS_ NMHDR * pnm) PURE;
    STDMETHOD (OnSetFocus)(THIS ) PURE;
    STDMETHOD (OnFrameWindowActivateBS)(THIS_ BOOL fActive) PURE;

    STDMETHOD (ReleaseShellView)(THIS ) PURE;
    STDMETHOD (ActivatePendingView)(THIS ) PURE;
    STDMETHOD (CreateViewWindow)(THIS_ IShellView* psvNew, IShellView* psvOld, LPRECT prcView, HWND* phwnd) PURE;
    STDMETHOD (CreateBrowserPropSheetExt)(THIS_ REFIID riid, void **ppv) PURE;

    // these could be base browser only interfaces
    // they are not overridden by anyone, and perhaps only there for the aggregators to get info from the
    // aggregatee. NOTE: where basesb calls these,
    // it does NOT go through _pbsOuter
    STDMETHOD (GetViewWindow)(THIS_ HWND * phwndView) PURE;
    STDMETHOD (GetBaseBrowserData)(THIS_ LPCBASEBROWSERDATA * pbbd) PURE;
    STDMETHOD_(LPBASEBROWSERDATA, PutBaseBrowserData)(THIS) PURE;
    STDMETHOD (InitializeTravelLog)(THIS_ ITravelLog* ptl, DWORD dw) PURE;
    STDMETHOD (SetTopBrowser)(THIS) PURE;
    STDMETHOD (Offline)(THIS_ int iCmd) PURE;
    STDMETHOD (AllowViewResize)(THIS_ BOOL f) PURE;
    STDMETHOD (SetActivateState)(THIS_ UINT u) PURE;
    STDMETHOD (UpdateSecureLockIcon)(THIS_ int eSecureLock) PURE;
    STDMETHOD (InitializeDownloadManager)(THIS) PURE;
    STDMETHOD (InitializeTransitionSite)(THIS) PURE;
    STDMETHOD (_Initialize)(THIS_ HWND hwnd, IUnknown *pauto) PURE;


    // BEGIN REVIEW:  review names and need of each.
    //
    // this first set could be basebrowser only members.  no one overrides
    // NOTE: where basesb calls these, it does NOT go throug _pbsOuter
    STDMETHOD (_CancelPendingNavigationAsync)(THIS) PURE;
    STDMETHOD (_CancelPendingView)(THIS) PURE;
    STDMETHOD (_MaySaveChanges)(THIS) PURE;
    STDMETHOD (_PauseOrResumeView)(THIS_ BOOL fPaused) PURE;
    STDMETHOD (_DisableModeless)(THIS) PURE;

    // rethink these... are all of these necessary?
    STDMETHOD (_NavigateToPidl)(THIS_ LPCITEMIDLIST pidl, DWORD grfHLNF, DWORD dwFlags)PURE;
    STDMETHOD (_TryShell2Rename)(THIS_ IShellView* psv, LPCITEMIDLIST pidlNew)PURE;
    STDMETHOD (_SwitchActivationNow)(THIS )PURE;

    // this set is overridden and called thru _pbsOuter (and SUPER:_pbsInner)
    STDMETHOD (_ExecChildren)(IUnknown *punkBar, BOOL fBroadcast,
                              const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt,
                              VARIANTARG *pvarargIn, VARIANTARG *pvarargOut)PURE;
    STDMETHOD (_SendChildren)(HWND hwndBar, BOOL fBroadcast,
        UINT uMsg, WPARAM wParam, LPARAM lParam) PURE;

    //END REVIEW:

    // querying from the outer.
    STDMETHOD (GetFolderSetData)(THIS_ struct tagFolderSetData* pfsd) PURE;

    // Toolbar stuff is here temporarily. Eventually it will move out
    // of basesb into commonsb, so I'm keeping the underscore "_"
    // Except those noted below, none of these are overridden by
    // outer guys, so no calls are made through _pbsOuter
    STDMETHOD (_OnFocusChange)(UINT itb) PURE;
    STDMETHOD (v_ShowHideChildWindows)(BOOL fChildOnly) PURE; // NOTE: overridden and called thru _pbsOuter
    STDMETHOD_(UINT,_get_itbLastFocus)() PURE;
    STDMETHOD (_put_itbLastFocus)(UINT itbLastFocus) PURE;
    STDMETHOD (_UIActivateView)(UINT uState) PURE;

    // Check the following functions
    STDMETHOD (_GetViewBorderRect)(RECT* prc) PURE;
    STDMETHOD (_UpdateViewRectSize)() PURE;
    STDMETHOD (_ResizeNextBorder)(UINT itb) PURE;
    STDMETHOD (_ResizeView)() PURE;

    STDMETHOD (_GetEffectiveClientArea)(LPRECT lprectBorder, HMONITOR hmon) PURE;

    // Desktop needs to override this from commonsb
    STDMETHOD_(IStream*,v_GetViewStream)(LPCITEMIDLIST pidl, DWORD grfMode, LPCWSTR pwszName) PURE;

    // Desktop needs access to these commonsb functions, they are not overridden:
    STDMETHOD_(LRESULT,ForwardViewMsg)(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam) PURE;
    STDMETHOD (SetAcceleratorMenu)(THIS_ HACCEL hacc) PURE;
    STDMETHOD_(int,_GetToolbarCount)(THIS) PURE;
    STDMETHOD_(LPTOOLBARITEM,_GetToolbarItem)(THIS_ int itb) PURE;
    STDMETHOD (_SaveToolbars)(IStream* pstm) PURE;
    STDMETHOD (_LoadToolbars)(IStream* pstm) PURE;
    STDMETHOD (_CloseAndReleaseToolbars)(BOOL fClose) PURE;
    STDMETHOD (v_MayGetNextToolbarFocus)(LPMSG lpMsg, UINT itbNext, int citb, LPTOOLBARITEM * pptbi, HWND * phwnd) PURE;
    STDMETHOD (_ResizeNextBorderHelper)(UINT itb, BOOL bUseHmonitor) PURE;
    STDMETHOD_(UINT,_FindTBar)(IUnknown* punkSrc) PURE;
    STDMETHOD (_SetFocus)(LPTOOLBARITEM ptbi, HWND hwnd, LPMSG lpMsg) PURE;
    STDMETHOD (v_MayTranslateAccelerator)(MSG* pmsg) PURE;
    STDMETHOD (_GetBorderDWHelper)(IUnknown* punkSrc, LPRECT lprectBorder, BOOL bUseHmonitor) PURE;

    // Shell browser needs to override this from basesb.

    STDMETHOD (v_CheckZoneCrossing)(LPCITEMIDLIST pidl) PURE;
};

// these MUST be listed in  the same order a c_szPropNames in deskbar.cpp
typedef enum {
    PROPDATA_SIDE = 0,
    PROPDATA_MODE,
    PROPDATA_LEFT,
    PROPDATA_TOP,
    PROPDATA_RIGHT,
    PROPDATA_BOTTOM,
    PROPDATA_DELETEABLE,
    PROPDATA_X,
    PROPDATA_Y,
    PROPDATA_CX,
    PROPDATA_CY,

    PROPDATA_COUNT
} ENUMPROPDATA ;

// PROPDATA_MODE values:
//  (aka. WBM_* -- webbar modes (_eMode))
#define WBM_BOTTOMMOST  ((UINT) 0x0)      // bottommost
#define WBM_TOPMOST     ((UINT) 0x1)      // topmost
#define WBM_FLOATING    ((UINT) 0x2)      // floating
#define WBM_NIL         ((UINT) 0x7)      // nil

#undef  INTERFACE
#define INTERFACE  IDockingBarPropertyBagInit
DECLARE_INTERFACE_(IDockingBarPropertyBagInit, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IDockingBarPropertyBag ***
    STDMETHOD(SetDataDWORD)(THIS_ ENUMPROPDATA e, DWORD dwData) PURE;
} ;

#if (_WIN32_IE) >= 0x0400
//===========================================================================
// IAddressList private COmWindow interface. This should never be exposed.
#undef INTERFACE
#define INTERFACE IAddressList
DECLARE_INTERFACE_(IAddressList, IWinEventHandler)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IWinEventHandler Methods ***
    STDMETHOD(OnWinEvent) (THIS_ HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plres) PURE;
    STDMETHOD(IsWindowOwner) (THIS_ HWND hwnd) PURE;

    // *** IAddressList Methods ***
    STDMETHOD(Connect) (THIS_ BOOL fConnect, HWND hwnd, IBrowserService* pbs, IBandProxy* pbp, IAutoComplete * pac) PURE;
    STDMETHOD(NavigationComplete) (THIS_ void *pvCShellUrl) PURE;
    STDMETHOD(Refresh) (THIS_ DWORD dwType) PURE;
    STDMETHOD(Load) (THIS) PURE;
    STDMETHOD(Save) (THIS) PURE;
    STDMETHOD(SetToListIndex) (THIS_ int nIndex, void *pvShelLUrl) PURE;
    STDMETHOD(FileSysChangeAL) (THIS_ DWORD dw, LPCITEMIDLIST *ppidl) PURE;
};
#endif


//
// IDocViewSite
//
#undef  INTERFACE
#define INTERFACE  IDocViewSite
DECLARE_INTERFACE_(IDocViewSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IDocViewSite methods ***
    STDMETHOD(OnSetTitle) (THIS_ VARIANTARG *pvTitle) PURE;

} ;

#endif

//===========================================================================
// INSCTree: This is a private interface
// CreateTree() dwStyles - these are normal window styles.
// Initialize() grfFlags - these are SHCONTF_* flags defined in <shlobj.h>.
//              dwFlags - these are NSCTree Behavior flags defined below
//                   NSS_DROPTARGET - Enable drag and drop
//                   NSS_BROWSERSELECT - With this on, a selection is different than the standard
//                      tree view selection in that a box is drawn around the sub heirarchy selected
//                      and the buttons are turned off.
// SetNscMode() nMode - these are modes that inherit general behavior.  Defined below

// NSCTree Behavior flags: (INSCTree::Initialize(..., dwFlags))
#define NSS_DROPTARGET          0x0001      // register as a drop target
#define NSS_BROWSERSELECT       0x0002      // Use the browser style selection (see above)
#define NSS_NOHISTSELECT        0x0004      // Do not select the history entry on navigations.

// NSCTree Modes: (INSCTree::SetNscMode(nMode))
typedef enum
{
    MODE_NORMAL           = 0x0000,             // Act like a normal tree view
    MODE_CONTROL          = 0x0001,             // Have behaviors specific the being hosted in a control
    MODE_HISTORY          = 0x0002,             // Have behaviors specific to the History pane.
    MODE_FAVORITES        = 0x0004,             // Have behaviors specific to the Favorites pane.
} nscTreeMode;


#undef INTERFACE
#define INTERFACE INSCTree
DECLARE_INTERFACE_(INSCTree, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** INSCTree Methods ***
    STDMETHOD(CreateTree) (THIS_ HWND hwndParent, DWORD dwStyles, HWND *phwnd) PURE;
    STDMETHOD(Initialize) (THIS_ LPCITEMIDLIST pidlRoot, DWORD grfFlags, DWORD dwFlags) PURE;
    STDMETHOD(ShowWindow) (THIS_ BOOL fShow) PURE;
    STDMETHOD(Refresh) (THIS) PURE;
    STDMETHOD(GetSelectedItem) (THIS_ LPITEMIDLIST * ppidl, int nItem) PURE;
    STDMETHOD(SetSelectedItem) (THIS_ LPCITEMIDLIST pidl, BOOL fCreate, BOOL fReinsert, int nItem) PURE;
    STDMETHOD(GetNscMode) (THIS_ UINT * pnMode) PURE;
    STDMETHOD(SetNscMode) (THIS_ UINT nMode) PURE;
    STDMETHOD(GetSelectedItemName) (THIS_ LPWSTR pszName, DWORD cchName) PURE;
    STDMETHOD(BindToSelectedItemParent) (THIS_ REFIID riid, void **ppv, LPITEMIDLIST *ppidl) PURE;
    STDMETHOD_(BOOL, InLabelEdit) (THIS) PURE;
};


//===========================================================================
// IDwnCodePage: This is a private interface to pass codepage info.
#undef INTERFACE
#define INTERFACE IDwnCodePage

DECLARE_INTERFACE_(IDwnCodePage, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IDwnCodePage Methods ***
    STDMETHOD_(UINT,GetCodePage) (THIS) PURE;
    STDMETHOD(SetCodePage) (THIS_ UINT uiCP) PURE;
};

//===========================================================================
// Helper functions for pidl allocation using the task allocator.
//
SHSTDAPI SHILClone(LPCITEMIDLIST pidl, LPITEMIDLIST * ppidlOut);
SHSTDAPI SHILCombine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, LPITEMIDLIST * ppidlOut);
#define SHILFree(pidl)  SHFree(pidl)

SHSTDAPI SHDllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv);

//===========================================================================

//----------------------------------------------------------------------------
#define IsLFNDriveORD           119
SHSTDAPI_(int) SHOutOfMemoryMessageBox(HWND hwndOwner, LPTSTR pszTitle, UINT fuStyle);
SHSTDAPI_(BOOL) SHWinHelp(HWND hwndMain, LPCTSTR pszHelp, UINT usCommand, ULONG_PTR ulData);

SHSTDAPI_(BOOL) RLBuildListOfPaths(void);

#ifdef WINNT
SHSTDAPI_(BOOL) RegenerateUserEnvironment(void **pPrevEnv, BOOL bSetCurrentEnv);
#endif

#define SHValidateUNCORD        173

#define VALIDATEUNC_NOUI        0x0002          // don't bring up stinking UI!
#define VALIDATEUNC_CONNECT     0x0001          // connect a drive letter
#define VALIDATEUNC_PRINT       0x0004          // validate as print share instead of disk share
#define VALIDATEUNC_VALID       0x0007          // valid flags


SHSTDAPI_(BOOL) SHValidateUNC(HWND hwndOwner, LPTSTR pszFile, UINT fConnect);

//----------------------------------------------------------------------------
#define OleStrToStrNORD                         78
#define SHCloneSpecialIDListORD                 89
#define SHDllGetClassObjectORD                 128
#define SHLogILFromFSILORD                      95
#define SHMapPIDLToSystemImageListIndexORD      77
#define SHShellFolderView_MessageORD            73
#define Shell_GetImageListsORD                  71
#define SHGetSpecialFolderPathORD              175
#define StrToOleStrNORD                         79

#define ILCloneORD                              18
#define ILCloneFirstORD                         19
#define ILCombineORD                            25
#define ILCreateFromPathORD                     157
#define ILFindChildORD                          24
#define ILFreeORD                               155
#define ILGetNextORD                            153
#define ILGetSizeORD                            152
#define ILIsEqualORD                            21
#define ILRemoveLastIDORD                       17
#define PathAddBackslashORD                     32
#define PathCombineORD                          37
#define PathIsExeORD                            43
#define PathMatchSpecORD                        46
#define SHGetSetSettingsORD                     68
#define SHILCreateFromPathORD                   28

#define SHFreeORD                               195

#define CheckWinIniForAssocsORD                 711

SHSTDAPI_(HANDLE) PifMgr_OpenProperties(LPCTSTR pszApp, LPCTSTR pszPIF, UINT hInf, UINT flOpt);
SHSTDAPI_(int)    PifMgr_GetProperties(HANDLE hProps, LPCSTR pszGroup, void *lpProps, int cbProps, UINT flOpt);
SHSTDAPI_(int)    PifMgr_SetProperties(HANDLE hProps, LPCSTR pszGroup, const VOID *lpProps, int cbProps, UINT flOpt);
SHSTDAPI_(HANDLE) PifMgr_CloseProperties(HANDLE hProps, UINT flOpt);

SHSTDAPI_(void) SHSetInstanceExplorer(IUnknown *punk);

#ifndef WINNT
// Always usr TerminateThreadEx.
BOOL APIENTRY TerminateThreadEx(HANDLE hThread, DWORD dwExitCode, BOOL bCleanupFlag);
#define TerminateThread(hThread, dwExitCode) TerminateThreadEx(hThread, dwExitCode, TRUE)
#endif

SHSTDAPI_(BOOL) IsUserAnAdmin(void);

// Sortof Gross but if we pass this flag in with STGM_CREATE we will fail if file already exists
// with ERROR_ALREADY_EXISTS
#define CSOF_FAILIFTHERE         0x80000000L


//===========================================================================
// Structure for covert communication
// between shdocvw's CShellBrowser::Exec()
// and shell32's DefView WM_COMMAND handler
struct _DFVCMDDATA
{
    HWND        hwnd;               // Browser hwnd
    VARIANTARG  *pva;               // Args passed to Exec()
    DWORD       nCmdIDTranslated;   // Replacement OLECMDID_* value
};
typedef struct _DFVCMDDATA DFVCMDDATA;
typedef struct _DFVCMDDATA *LPDFVCMDDATA;
typedef const struct _DFVCMDDATA *LPCDFVCMDDATA;


SHSTDAPI_(DWORD) SHGetProcessDword(DWORD idProcess, LONG iIndex);
SHSTDAPI_(BOOL)  SHSetShellWindowEx(HWND hwnd, HWND hwndChild);

#define CCH_MENUMAX     80          // DOC: max size of a menu string


// WinEventHandler message ranges
#define MBHM_FIRST          (0x00000001)    // MenuBandHandler
#define MBHM_LAST           (0x000000ff)

#define MSFM_FIRST          (0x00000100)    // MenuShellFolder
#define MSFM_LAST           (0x000001ff)

#define FSFM_FIRST          (0x00000200)    // FileShellFolder
#define FSFM_LAST           (0x000002ff)



//-------------------------------------------------------------------------
//
// SID_SMenuBandHandler
//
//  The menuband calls QueryService(SID_SMenuBandHandler, IID_IWinEventHandler)
//  on its shellfolder.  If the shellfolder supports this service, the
//  menuband will call the event handler on certain menu events.  These
//  events and messages are as follows:
//
//    MBHM_INITMENUPOPUP    sent when a drop-down menu or submenu is
//                          about to become active.
//
//
//   File-system ShellFolder events:
//
//    FSFM_COMMAND          sent when a file-system item is executed
//
//-------------------------------------------------------------------------

typedef struct tagMBINITMENUPOPUP
{
    DWORD           dwMask;     // MBIMP_*
    IShellFolder *  psf;
    LPCITEMIDLIST   pidl;
    HMENU           hmenu;
    int             iPos;
} MBINITMENUPOPUP;

#define MBIMP_HMENU     0x00000001      // hmenu and iPos are set


// Messages sent to the WinEventHandler         wParam      lParam      *plRet
//                                              ------      ------      ---
#define MBHM_INITMENUPOPUP  (MBHM_FIRST+0)  //  0           MBINITMENUPOPUP *


// Messages sent to the WinEventHandler         wParam      lParam      *plRet
//                                              ------      ------      ------
#define FSFM_COMMAND        (FSFM_FIRST+0)  //  cmd ID      pidl


//-------------------------------------------------------------------------
//
// IAugmentedShellFolder interface
//
//   This provides methods to add multiple Shell Folder objects to a
//  single augmented object (CLSID_AugmentedShellFolder), which will
//  enumerate them as if they were in a single namespace.
//
//
// [Member functions]
//
// IAugmentedShellFolder::AddNameSpace(rguidObject, psf, pidl)
//   Add the object represented by the IShellFolder interface to
//   the augmented list.  The rguidObject is used to identify the
//   given object.  It may be NULL.  The optional pidl refers to
//   the location of the psf.
//
// IAugmentedShellFolder::GetNameSpaceID(pidl, rguidOut)
//   This method returns the rguidObject associated with the given
//   pidl.  This is how a caller can identify which namespace the
//   pidl resides, given the list of IShellFolder objects that were
//   added to this augmented object.
//
// IAugmentedShellFolder::QueryNameSpace(dwNameSpaceID, pguidOut, ppsf)
//   Returns the shellfolder and guid associated with the given ID.
//
// IAugmentedShellFolder::EnumNameSpace(uNameSpace, pdwNameSpaceID)
//   Enumerates the namespace shellfolders.  If uNameSpace is -1,
//   pdwNameSpaceID is ignored and this method returns the count of
//   shellfolders in the augmented namespace.  Call this method with
//   uNameSpace starting at 0 to begin enumerating.  Returns S_OK and
//   and namespace ID in *pdwNameSpaceID.
//
//-------------------------------------------------------------------------

//  IAugmentedShellFolder::AddNameSpace flags
enum
{
    ASFF_DEFAULT                   = 0x00000000, // There are no applicable Flags
    ASFF_SORTDOWN                  = 0x00000001, // Sort the items in this ISF to the bottom.
    ASFF_MERGE                     = 0x00000002, // Merge the 2 namespaces
    // the following should all be collapsed to one ASFF_DEFNAMESPACE
    ASFF_DEFNAMESPACE_BINDSTG      = 0x00000100, // The namespace is the default handler for BindToStorage() for merged child items.
    ASFF_DEFNAMESPACE_COMPARE      = 0x00000200, // The namespace is the default handler for CompareIDs() for merged child items.
    ASFF_DEFNAMESPACE_VIEWOBJ      = 0x00000400, // The namespace is the default handler for CreateViewObject() for merged child items.
    ASFF_DEFNAMESPACE_ATTRIB       = 0x00001800, // The namespace is the default handler for GetAttributesOf() for merged child items.
    ASFF_DEFNAMESPACE_DISPLAYNAME  = 0x00001000, // The namespace is the default handler for GetDisplayNameOf(), SetNameOf() and ParseDisplayName() for merged child items.
    ASFF_DEFNAMESPACE_UIOBJ        = 0x00002000, // The namespace is the default handler for GetUIObjectOf() for merged child items.
    ASFF_DEFNAMESPACE_ITEMDATA     = 0x00004000, // The namespace is the default handler for GetItemData() for merged child items.
    ASFF_DEFNAMESPACE_ALL          = 0x0000FF00  // The namespace is the primary handler for all IShellFolder operations on merged child items.
};

#undef  INTERFACE
#define INTERFACE   IAugmentedShellFolder

DECLARE_INTERFACE_(IAugmentedShellFolder, IShellFolder)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellFolder methods ***
    STDMETHOD(ParseDisplayName) (THIS_ HWND hwndOwner,LPBC pbc, LPOLESTR pszDisplayName,
                                 ULONG * pchEaten, LPITEMIDLIST * ppidl, ULONG *pdwAttributes) PURE;
    STDMETHOD(EnumObjects)      (THIS_ HWND hwndOwner, DWORD grfFlags, IEnumIDList ** ppenumIDList) PURE;
    STDMETHOD(BindToObject)     (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(BindToStorage)    (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(CompareIDs)       (THIS_ LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) PURE;
    STDMETHOD(CreateViewObject) (THIS_ HWND hwndOwner, REFIID riid, void **ppv) PURE;
    STDMETHOD(GetAttributesOf)  (THIS_ UINT cidl, LPCITEMIDLIST * apidl, ULONG * rgfInOut) PURE;
    STDMETHOD(GetUIObjectOf)    (THIS_ HWND hwndOwner, UINT cidl, LPCITEMIDLIST * apidl,
                                 REFIID riid, UINT * prgfInOut, void **ppv) PURE;
    STDMETHOD(GetDisplayNameOf) (THIS_ LPCITEMIDLIST pidl, DWORD uFlags, LPSTRRET lpName) PURE;
    STDMETHOD(SetNameOf)        (THIS_ HWND hwndOwner, LPCITEMIDLIST pidl,
                                 LPCOLESTR pszName, DWORD uFlags,
                                 LPITEMIDLIST * ppidlOut) PURE;

    // *** IAugmentedShellFolder methods ***
    STDMETHOD(AddNameSpace)     (THIS_ const GUID * pguidObject, IShellFolder * psf,
                                 LPCITEMIDLIST pidl, DWORD dwFlags) PURE;
    STDMETHOD(GetNameSpaceID)   (THIS_ LPCITEMIDLIST pidl, GUID * pguidOut) PURE;
    STDMETHOD(QueryNameSpace)   (THIS_ DWORD dwID, GUID * pguidOut, IShellFolder ** ppsf) PURE;
    STDMETHOD(EnumNameSpace)    (THIS_ DWORD uNameSpace, DWORD * pdwID) PURE;
};

#undef  INTERFACE
#define INTERFACE   IAugmentedShellFolder2

DECLARE_INTERFACE_(IAugmentedShellFolder2, IAugmentedShellFolder)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellFolder methods ***
    STDMETHOD(ParseDisplayName) (THIS_ HWND hwndOwner, LPBC pbc, LPOLESTR pszDisplayName,
                                 ULONG * pchEaten, LPITEMIDLIST * ppidl, ULONG *pdwAttributes) PURE;
    STDMETHOD(EnumObjects)      (THIS_ HWND hwndOwner, DWORD grfFlags, IEnumIDList ** ppenumIDList) PURE;
    STDMETHOD(BindToObject)     (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(BindToStorage)    (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(CompareIDs)       (THIS_ LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) PURE;
    STDMETHOD(CreateViewObject) (THIS_ HWND hwndOwner, REFIID riid, void **ppv) PURE;
    STDMETHOD(GetAttributesOf)  (THIS_ UINT cidl, LPCITEMIDLIST * apidl,
                                 ULONG * rgfInOut) PURE;
    STDMETHOD(GetUIObjectOf)    (THIS_ HWND hwndOwner, UINT cidl, LPCITEMIDLIST * apidl,
                                 REFIID riid, UINT * prgfInOut, void **ppv) PURE;
    STDMETHOD(GetDisplayNameOf) (THIS_ LPCITEMIDLIST pidl, DWORD uFlags, LPSTRRET lpName) PURE;
    STDMETHOD(SetNameOf)        (THIS_ HWND hwndOwner, LPCITEMIDLIST pidl,
                                 LPCOLESTR pszName, DWORD uFlags,
                                 LPITEMIDLIST * ppidlOut) PURE;

    // *** IAugmentedShellFolder methods ***
    STDMETHOD(AddNameSpace)     (THIS_ const GUID * pguidObject, IShellFolder * psf,
                                 LPCITEMIDLIST pidl, DWORD dwFlags) PURE;
    STDMETHOD(GetNameSpaceID)   (THIS_ LPCITEMIDLIST pidl, GUID * pguidOut) PURE;
    STDMETHOD(QueryNameSpace)   (THIS_ DWORD dwID, GUID * pguidOut, IShellFolder ** ppsf) PURE;
    STDMETHOD(EnumNameSpace)    (THIS_ DWORD uNameSpace, DWORD * pdwID) PURE;

    // *** IAugmentedShellFolder2 methods ***
    //where are these used?
    //STDMETHOD(GetNameSpaceCount)  (THIS_ OUT LONG * pcNamespaces) PURE ;
    //STDMETHOD(GetIDListWrapCount) (THIS_ LPCITEMIDLIST pidlWrap, OUT LONG * pcPidls) PURE ;
    STDMETHOD(UnWrapIDList)       (THIS_ LPCITEMIDLIST pidlWrap, LONG cPidls, IShellFolder ** apsf, LPITEMIDLIST * apidlFolder, LPITEMIDLIST * apidlItems, LONG * pcFetched ) PURE ;
};


//-------------------------------------------------------------------------
//
// IProxyShellFolder interface
//
//   This provides methods to set a proxy object which can respond
//  to GetUIObjectOf instead of or in addition to the hosted object
//  the supports IShellFolder.  All other methods are forwarded onto
//  the hosted object.  The CLSID_HostProxyShellFolder object implements
//  this interface.
//
//   The Win95 shell does not support aggregation, so this object
//  must be extra careful that it follows the rules of COM.
//
//   This interface is implemented by an object that wishes to provide
//  more support for GetUIObjectOf on behalf of another object that
//  implements an original IShellFolder.
//
// [Member functions]
//
// IProxyShellFolder::InitHostProxy(psf, pidl, punk, dwFlags)
//   Set the object that implements IProxyShellFolder.  Depending
//   on dwFlags, this object will be called instead of or in addition
//   to the host's method.
//
//      SPF_PRIORITY            - call the proxy's method instead of
//                              the host's method.
//      SPF_SECONDARY           - call the proxy's method only if the
//                              host's method doesn't support it.
//      SPF_INHERIT             - create a new object of this class and
//                              hand it back for BindToObject.
//      SPF_FOLDERPRECEDENCE    - have the proxy give folders priority
//                              over files in a call to CompareIDs.
//      SPF_HAVECALLBACK        - have the proxy call the owner's
//                              IOleCommandTarget on each enumerated
//                              object, for filtering.
//
//   Also sets as the host the given object which fully implements
//   IShellFolder.  pidl refers to the shell folder.
//
// IProxyShellFolder::CloneProxyPSF(riid, ppv)
//   Tells the object to clone itself.  The host proxy will call this
//   whenever IShellFolder::BindToObject is called.
//
// IProxyShellFolder::GetUIObjectOfPSF(hwndOwner, cidl, apidl, riid, prgfInOut, ppv)
//   Called by CLSID_HostProxyShellFolder to allow the proxy object
//   an opportunity to respond.
//
// IProxyShellFolder::CreateViewObjectPSF(hwndOwner, riid, ppv)
//   Called by CLSID_HostProxyShellFolder to allow the proxy object
//   an opportunity to respond.
//
//-------------------------------------------------------------------------

// SetProxyObject flags
#define SPF_PRIORITY            0x00000001       // This is mutually exclusive w/ SPF_SECONDARY
#define SPF_SECONDARY           0x00000002
#define SPF_INHERIT             0x00000004
#define SPF_FOLDERPRECEDENCE    0x00000008
#define SPF_HAVECALLBACK        0x00000010

//For filtering of Pidls
#define PHID_FilterOutPidl  0


#undef  INTERFACE
#define INTERFACE   IProxyShellFolder

DECLARE_INTERFACE_(IProxyShellFolder, IShellFolder)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellFolder methods ***
    STDMETHOD(ParseDisplayName) (THIS_ HWND hwndOwner, LPBC pbc, LPOLESTR pszDisplayName,
                                 ULONG * pchEaten, LPITEMIDLIST * ppidl, ULONG *pdwAttributes) PURE;
    STDMETHOD(EnumObjects)      (THIS_ HWND hwndOwner, DWORD grfFlags, IEnumIDList ** ppenumIDList) PURE;
    STDMETHOD(BindToObject)     (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(BindToStorage)    (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(CompareIDs)       (THIS_ LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) PURE;
    STDMETHOD(CreateViewObject) (THIS_ HWND hwndOwner, REFIID riid, void **ppv) PURE;
    STDMETHOD(GetAttributesOf)  (THIS_ UINT cidl, LPCITEMIDLIST * apidl,
                                 ULONG * rgfInOut) PURE;
    STDMETHOD(GetUIObjectOf)    (THIS_ HWND hwndOwner, UINT cidl, LPCITEMIDLIST * apidl,
                                 REFIID riid, UINT * prgfInOut, void **ppv) PURE;
    STDMETHOD(GetDisplayNameOf) (THIS_ LPCITEMIDLIST pidl, DWORD uFlags, LPSTRRET lpName) PURE;
    STDMETHOD(SetNameOf)        (THIS_ HWND hwndOwner, LPCITEMIDLIST pidl,
                                 LPCOLESTR pszName, DWORD uFlags,
                                 LPITEMIDLIST * ppidlOut) PURE;

    // *** IProxyShellFolder methods ***
    STDMETHOD(InitHostProxy)     (THIS_ IShellFolder * psf, LPCITEMIDLIST pidl, DWORD dwFlags) PURE;
    STDMETHOD(CloneProxyPSF)     (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD(GetUIObjectOfPSF) (THIS_ HWND hwndOwner, UINT cidl, LPCITEMIDLIST * apidl,
                                 REFIID riid, UINT * prgfInOut, void **ppv) PURE;
    STDMETHOD(CreateViewObjectPSF) (THIS_ HWND hwndOwner, REFIID riid, void **ppv) PURE;

};


//-------------------------------------------------------------------------
//
// BUGBUG (scotth): I don't think we're using this anymore.
//
// ISetWinHandler interface
//
// [Member functions]
//
// ISetWinHandler::SetWindow(hwndOwner)
//   Specifies the window owner.  This may be NULL.
//
// ISetWinHandler::SetWinHandler(punk)
//   Gives a pointer to an object which supports IWinEventHandler.  The
//   hwnd will be the window handle set by the SetWindow method.  The
//   handler should return S_OK if the message is handled, otherwise S_FALSE.
//
//-------------------------------------------------------------------------

#undef  INTERFACE
#define INTERFACE   ISetWinHandler

DECLARE_INTERFACE_(ISetWinHandler, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** ISetWinHandler methods ***
    STDMETHOD(SetWindow)        (THIS_ HWND hwndOwner) PURE;
    STDMETHOD(SetWinHandler)    (THIS_ IUnknown * punk) PURE;
};


//-------------------------------------------------------------------------
//
// IMenuShellFolder interface
//
//   This wraps a static HMENU with an IShellFolder interface.
//
//
// [Member functions]
//
// IMenuShellFolder::SetMenu(hmenu)
//   Specifies the hmenu to wrap.  This may be NULL.
//
//   BUGBUG (scotth): does this copy or not?
//
// IMenuShellFolder::GetMenu(phmenu)
//   Returns the current menu in *phmenu.  Returns S_FALSE if no menu is
//   set (*phmenu will be NULL).
//
// IMenuShellFolder::SetWindow(hwndOwner)
//   Specifies the window owner.  This may be NULL.
//
//-------------------------------------------------------------------------

// Messages sent to the WinEventHandler         wParam      lParam      *plRet
//                                              ------      ------      ---
#define MSFM_COMMAND        (MSFM_FIRST+0)  //  cmd ID      0
#define MSFM_ISDROPTARGET   (MSFM_FIRST+1)  //  id          0           1 if yes


#undef  INTERFACE
#define INTERFACE   IMenuShellFolder

DECLARE_INTERFACE_(IMenuShellFolder, IShellFolder)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellFolder methods ***
    STDMETHOD(ParseDisplayName) (THIS_ HWND hwndOwner, LPBC pbc, LPOLESTR pszDisplayName,
                                 ULONG * pchEaten, LPITEMIDLIST * ppidl, ULONG *pdwAttributes) PURE;
    STDMETHOD(EnumObjects)      (THIS_ HWND hwndOwner, DWORD grfFlags, IEnumIDList ** ppenumIDList) PURE;
    STDMETHOD(BindToObject)     (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(BindToStorage)    (THIS_ LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv) PURE;
    STDMETHOD(CompareIDs)       (THIS_ LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) PURE;
    STDMETHOD(CreateViewObject) (THIS_ HWND hwndOwner, REFIID riid, void **ppv) PURE;
    STDMETHOD(GetAttributesOf)  (THIS_ UINT cidl, LPCITEMIDLIST * apidl,
                                 ULONG * rgfInOut) PURE;
    STDMETHOD(GetUIObjectOf)    (THIS_ HWND hwndOwner, UINT cidl, LPCITEMIDLIST * apidl,
                                 REFIID riid, UINT * prgfInOut, void **ppv) PURE;
    STDMETHOD(GetDisplayNameOf) (THIS_ LPCITEMIDLIST pidl, DWORD uFlags, LPSTRRET lpName) PURE;
    STDMETHOD(SetNameOf)        (THIS_ HWND hwndOwner, LPCITEMIDLIST pidl,
                                 LPCOLESTR pszName, DWORD uFlags,
                                 LPITEMIDLIST * ppidlOut) PURE;

    // *** IMenuShellFolder methods ***
    STDMETHOD(SetMenu)          (THIS_ HMENU hmenu) PURE;
    STDMETHOD(GetMenu)          (THIS_ HMENU * phmenu) PURE;
    STDMETHOD(SetWindow)        (THIS_ HWND hwndOwner) PURE;
    STDMETHOD(GetWindow)        (THIS_ HWND *phwndOwner) PURE;
    STDMETHOD(GetIDPosition)    (THIS_ LPCITEMIDLIST pidl, int * pnPos) PURE;
};


//-------------------------------------------------------------------------
//
// ITranslateShellChangeNotify interface
//
//   This interface allows an object to translate the pidls that are
// passed to it.
//
// Note:  IE4 had an interface of the same name but with a different IID,
//        so there is no conflict.
//
// [Member functions]
//
// ITranslateShellChangeNotify::TranslateIDs(&lEvent, pidl1, pidl2, ppidlOut1, ppidlOut2)
//   Return converted pidls given the event. it also can change the event if appropriate
//
//-------------------------------------------------------------------------

#undef  INTERFACE
#define INTERFACE   ITranslateShellChangeNotify

DECLARE_INTERFACE_(ITranslateShellChangeNotify, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** ITranslateShellChangeNotify methods ***
    STDMETHOD(TranslateIDs) (THIS_ LONG *plEvent , LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, LPITEMIDLIST * ppidlOut1, LPITEMIDLIST * ppidlOut2,
                                   LONG *plEvent2, LPITEMIDLIST * ppidlOut1Event2, LPITEMIDLIST * ppidlOut2Event2) PURE;
    STDMETHOD(IsChildID)(THIS_ LPCITEMIDLIST pidlKid, BOOL fImmediate) PURE;
    STDMETHOD(IsEqualID)(THIS_ LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) PURE;
    STDMETHOD(Register)(THIS_ HWND hwnd, UINT uMsg, long lEvents) PURE;
    STDMETHOD(Unregister)(THIS) PURE;
};

////////////////////////////////////////////////////
//
// IRegTreeOptions
//
typedef enum tagBUTTON_STATES
{
    IDCHECKED,
    IDUNCHECKED,
    IDRADIOON,
    IDRADIOOFF,
    IDUNKNOWN
} BUTTON_STATES;

//
// Avoid conflicts in the windows\shell project
//
#ifndef NO_SHELL_TREE_TYPE
typedef enum tagTREE_TYPE
{
    TREE_CHECKBOX,
    TREE_RADIO,
    TREE_GROUP,
    TREE_UNKNOWN
} TREE_TYPE;
#endif // NO_SHELL_TREE_TYPE

typedef enum tagWALK_TREE_CMD
{
    WALK_TREE_SAVE,
    WALK_TREE_DELETE,
    WALK_TREE_RESTORE,
    WALK_TREE_REFRESH
} WALK_TREE_CMD;


enum REG_CMD
{
    REG_SET,
    REG_GET,
    REG_GETDEFAULT
};

#undef  INTERFACE
#define INTERFACE   IRegTreeOptions

DECLARE_INTERFACE_(IRegTreeOptions, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IRegTreeOptions specific methods ***
    STDMETHOD(InitTree)(THIS_ HWND hwndTree, HKEY hkeyRoot, LPCSTR pszRegKey, LPCSTR pszParam) PURE;
    STDMETHOD(WalkTree)(THIS_ WALK_TREE_CMD cmd ) PURE;
    STDMETHOD(ToggleItem)(THIS_ HTREEITEM hti ) PURE;
    STDMETHOD(ShowHelp)(THIS_ HTREEITEM hti , DWORD dwFlags ) PURE;
};



//
// Interface: IShellHotKey
//

#undef  INTERFACE
#define INTERFACE   IShellHotKey

DECLARE_INTERFACE_(IShellHotKey, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellHotKey methods ***
    STDMETHOD(RegisterHotKey)(THIS_ IShellFolder * psf, LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidl) PURE;
};


//
// Interface: ITrayPriv
//
// Purpose: Talks to the new Start Menu code in Shdocvw from explorer.exe
//
// For flags on ShowFolder see inc\IETHREAD.H
//

#undef  INTERFACE
#define INTERFACE   ITrayPriv

DECLARE_INTERFACE_(ITrayPriv, IOleWindow)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

    // *** ITrayPriv methods ***
    STDMETHOD(ExecItem)(THIS_ IShellFolder* psf, LPCITEMIDLIST pidl) PURE;
    STDMETHOD(GetFindCM)(THIS_ HMENU hmenu, UINT idFirst, UINT idLast, IContextMenu** ppcmFind) PURE;
    STDMETHOD(GetStaticStartMenu)(THIS_ HMENU* phmenu) PURE;
};


#undef  INTERFACE
#define INTERFACE   IInitializeObject

DECLARE_INTERFACE_(IInitializeObject, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IInitializeObject methods
    STDMETHOD(Initialize)(THIS) PURE;
};

enum
{
    BMICON_LARGE = 0,
    BMICON_SMALL
};

#undef  INTERFACE
#define INTERFACE   IBanneredBar

DECLARE_INTERFACE_(IBanneredBar, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IBanneredBar methods ***
    STDMETHOD(SetIconSize)(THIS_ DWORD iIcon) PURE;
    STDMETHOD(GetIconSize)(THIS_ DWORD* piIcon) PURE;
    STDMETHOD(SetBitmap)(THIS_ HBITMAP hBitmap) PURE;
    STDMETHOD(GetBitmap)(THIS_ HBITMAP* phBitmap) PURE;

};

//
// Interface: IFolderShortcutConvert
//
// Purpose: Converts between FolderShortcuts and links
//

#undef  INTERFACE
#define INTERFACE   IFolderShortcutConvert

DECLARE_INTERFACE_(IFolderShortcutConvert, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    //*** IFolderShortcutConvert methods ***
    STDMETHOD(ConvertToLink)(THIS_ LPCOLESTR pszFSPath, DWORD fFlags) PURE;
    STDMETHOD(ConvertToFolderShortcut)(THIS_ LPCOLESTR pszLinkPath, DWORD fFlags) PURE;
};

//
// Interface: IShellFolderTask
//
// Purpose: Initializes a task that does something by enumerating a shellfolder
//

#undef  INTERFACE
#define INTERFACE   IShellFolderTask

DECLARE_INTERFACE_(IShellFolderTask, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IShellFolderTask methods ***
    STDMETHOD(InitTaskSFT)(THIS_ IShellFolder *psfParent, LPITEMIDLIST pidlFull,
                           LONG nMaxRecursionLevel, DWORD dwFlags, DWORD dwTaskPriority) PURE;
};

// Flags for InitTaskSFT
#define ITSFT_RECURSE   0x00000001      // recurse into subfolders


//
// Interface: IStartMenuTask
//
// Purpose: Initializes a task that does something for the start menu
//

#undef  INTERFACE
#define INTERFACE   IStartMenuTask

DECLARE_INTERFACE_(IStartMenuTask, IShellFolderTask)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IShellFolderTask methods ***
    STDMETHOD(InitTaskSFT)(THIS_ IShellFolder *psfParent, LPITEMIDLIST pidlFull,
                           LONG nMaxRecursionLevel, DWORD dwFlags, DWORD dwTaskPriority) PURE;

    // *** IStartMenuTask methods ***
    STDMETHOD(InitTaskSMT)(THIS_ IShellHotKey * photkey, int iThreadPriority) PURE;
};

//
// Interface: IContextMenuCB
//
// Purpose: A call defview context menu callback object
//

#undef  INTERFACE
#define INTERFACE   IContextMenuCB

DECLARE_INTERFACE_(IContextMenuCB, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IContextMenuCB methods ***
    STDMETHOD(CallBack) (THIS_  IShellFolder *psf, HWND hwndOwner, IDataObject *pdtobj, UINT uMsg,
                                WPARAM wParam, LPARAM lParam) PURE;
};

// let the context menu handler call up to it's site to let the site implement certain functions
// BUGBUG: may need to add QueryRename(), QueryCut(), etc.

DECLARE_INTERFACE_(IContextMenuSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IContextMenuSite methods ***
    STDMETHOD(DoRename)() PURE;
    STDMETHOD(DoCut)() PURE;
};

#define SID_SContextMenuSite IID_IContextMenuSite

//
// Possible underline settings for shell icons.
//
enum
{
    ICON_YES,
    ICON_NO,
    ICON_HOVER,
    ICON_IE         // Use IE hyperlink settings
};

//-----------------------------------------------
// IOleCommand Target Command IDs

// CGID_BandHandler
    //  Get the order stream from parent.
#define BHCMDID_GetOrderStream      0x00000001

// CGID_Persist
    // Set the storage key.
#define MCBID_SetKey                0x00000001

// CGID_MenuBandItem
#define MBICMDID_IsVisible          0x00000001

// CGID_MenuBand
#define MBANDCID_REFRESH            0x10000000


//
// Interface: IShellMallocSpy
//
// Purpose: Facilitate IMallocSpy in the shell
//

#undef  INTERFACE
#define INTERFACE  IShellMallocSpy
DECLARE_INTERFACE_(IShellMallocSpy, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellMallocSpy methods ***
    STDMETHOD(RegisterSpy) (THIS) PURE;
    STDMETHOD(RevokeSpy) (THIS) PURE;
    STDMETHOD(SetTracking) (THIS_ BOOL bTrack) PURE;
    STDMETHOD(AddToList) (THIS_ void *pv, SIZE_T cb) PURE;
    STDMETHOD(RemoveFromList) (THIS_ void *pv) PURE;
};

#undef  INTERFACE
#define INTERFACE   ISearchProvider

DECLARE_INTERFACE_(ISearchProvider, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** ISearchObject methods ***
    STDMETHOD(GetSearchGUID)(THIS_ LPGUID lpGuid) PURE;
};

#undef  INTERFACE
#define INTERFACE   ISearchItems

DECLARE_INTERFACE_(ISearchItems, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** ISearchItems methods ***
    //used by CSearchBand
    STDMETHOD(GetDefaultSearchUrl) (THIS_ LPWSTR pwzUrl, UINT cch) PURE;
};

#undef  INTERFACE
#define INTERFACE   ISearchBandTBHelper

DECLARE_INTERFACE_(ISearchBandTBHelper, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** ISearchBandTBHelper methods ***
    // implemented by CSearchBand
    STDMETHOD(AddNextMenuItem) (THIS_ LPCWSTR pwszText, int idItem) PURE;
    STDMETHOD(SetOCCallback) (THIS_ IOleCommandTarget *pOleCmdTarget) PURE;
    STDMETHOD(ResetNextMenu) (THIS) PURE;
};

#undef  INTERFACE
#define INTERFACE   IEnumUrlSearch

typedef struct
{
    GUID  guid;
    WCHAR wszName[80];
    WCHAR wszUrl[2048];
}URLSEARCH, *LPURLSEARCH;

DECLARE_INTERFACE_(IEnumUrlSearch, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IEnumUrlSearch methods ***
    STDMETHOD(Next)(THIS_ ULONG celt, LPURLSEARCH rgelt, ULONG *pceltFetched) PURE;
    STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
    STDMETHOD(Reset)(THIS) PURE;
    STDMETHOD(Clone)(THIS_ IEnumUrlSearch **ppenum) PURE;
};

#undef  INTERFACE
#define INTERFACE   IFolderSearches

DECLARE_INTERFACE_(IFolderSearches, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IFolderSearches methods ***
    STDMETHOD(EnumSearches) (THIS_ IEnumUrlSearch **ppenum) PURE;
    STDMETHOD(DefaultSearch) (THIS_ GUID *pguid) PURE;
};
#undef  INTERFACE
#define INTERFACE   IUserAssist

typedef struct
{
    DWORD   cbSize;     // SIZEOF
    DWORD   dwMask;     // INOUT requested/given (UEIM_*)
    int     cHit;       // profile count
    DWORD   dwAttrs;    // attributes (UEIA_*)
    FILETIME ftExecute; // Last execute filetime
} UEMINFO, *LPUEMINFO;

#define UEIM_HIT        0x01
#define UEIM_FILETIME   0x02

DECLARE_INTERFACE_(IUserAssist, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IUserAssist methods ***
    STDMETHOD(FireEvent)(const GUID *pguidGrp, int eCmd, DWORD dwFlags, WPARAM wParam, LPARAM lParam) PURE;
    STDMETHOD(QueryEvent)(const GUID *pguidGrp, int eCmd, WPARAM wParam, LPARAM lParam, LPUEMINFO pui) PURE;
    STDMETHOD(SetEvent)(const GUID *pguidGrp, int eCmd, WPARAM wParam, LPARAM lParam, LPUEMINFO pui) PURE;
};



// This exists for the Internet Control Panel to be able to toggle who is the
// currently installed handler for FTP for the browser.
DECLARE_INTERFACE_(IFtpInstaller, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IFtpInstaller methods ***
    STDMETHOD(IsIEDefautlFTPClient)(THIS) PURE;
    STDMETHOD(RestoreFTPClient)(THIS) PURE;
    STDMETHOD(MakeIEDefautlFTPClient)(THIS) PURE;
};


//
// Private QueryContextMenuFlag passed from DefView
//
#define CMF_DVFILE       0x00010000     // "File" pulldown

SHSTDAPI_(LRESULT) SHShellFolderView_Message(HWND hwndMain, UINT uMsg, LPARAM lParam);


//
// Callback interface for the IShellFolderView
//
#undef  INTERFACE
#define INTERFACE   IShellFolderViewCB

DECLARE_INTERFACE_(IShellFolderViewCB, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderViewCB methods ***
    STDMETHOD(MessageSFVCB)(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam) PURE;
};

// StringFromGUID and its constants
BOOL _StringFromGUID(GUID const* pid, LPTSTR pszString, UINT cb, UINT idString);

// Note:  The following are used by _StringFromGUID and/or are indexes into the
// c_szExtViewUIRegKeys string array in sfvext.cpp.
#define ID_EXTVIEWSTRFIRST          0
#define ID_EXTVIEWNAME              0 // name of view in menu
#define ID_EXTVIEWHELPTEXT          1 // help text for this view
#define ID_EXTVIEWTTTEXT            2 // tooltip text for this view
#define ID_EXTVIEWICONAREAIMAGE     3 // url of background bitmap for WebViewFolderContents OC
#define ID_EXTVIEWSTRLAST           3

#define ID_EXTVIEWCOLORSFIRST       4
#define ID_EXTVIEWTEXTBACKGROUND    4 // background text color for WebViewFolderContents OC
#define ID_EXTVIEWTEXT              5 // foreground text color for WebViewFolderContents OC
#define ID_EXTVIEWCOLORSLAST        5

#define ID_EXTVIEWMAX               5
#define ID_EXTVIEWSTRCOUNT  (ID_EXTVIEWSTRLAST - ID_EXTVIEWSTRFIRST + 1)
#define ID_EXTVIEWCOLORCOUNT    (ID_EXTVIEWCOLORSLAST - ID_EXTVIEWCOLORSFIRST + 1)
#define ID_EXTVIEWUICOUNT           (ID_EXTVIEWMAX + 1)

#define CLR_MYINVALID 0x8fffffff       // I need to use a special value since CLR_INVALID == CLR_NONE!!!!
#define ISVALIDCOLOR(a) ((a) != CLR_MYINVALID)

// The following are used as indexes into the crCustomColors array, and must be in the same
// order as their ID_EXTVIEW* counterparts above.  They are also used in an analogous array in CDefView.
#define CRID_CUSTOMTEXTBACKGROUND   ID_EXTVIEWTEXTBACKGROUND - ID_EXTVIEWCOLORSFIRST
#define CRID_CUSTOMTEXT             ID_EXTVIEWTEXT - ID_EXTVIEWCOLORSFIRST
#define CRID_COLORCOUNT             ID_EXTVIEWCOLORSLAST - ID_EXTVIEWCOLORSFIRST + 1

typedef struct _CUSTOMVIEWSDATA
{
    DWORD      cchSizeOfBlock;  // size of lpDataBlock in WCHARs
    LPWSTR     lpDataBlock;     // Pointer to the Strings data block, allocated by the task allocator (SHAlloc)

    // The following are offsets (in WCHARs) to be added to lpDataBlock.
    // If the offset is -1 => That data is not present for this view.
    int        acchOffExtViewUIstr[ID_EXTVIEWSTRCOUNT];
    COLORREF   crCustomColors[CRID_COLORCOUNT];

    // Once IE4 ships, we can extend this structure by adding new SFVF_ flags
    // in the main structure that say "customviewsdata contains xxx info".

} CUSTOMVIEWSDATA;

//
// Interface for enumerating "external" views for the ShellFolderView
//
typedef struct _SFVVIEWSDATA
{
    // idView specifies the view. Unless specified in pCustomData, look up
    // HKLM\CLSID\Software\Microsoft\Windows\CurrentVersion\ExtShellFolderViews\idView
    // to find the menu name, help text, tt text.
    //
    // the shell's IShellFolderView implementations get these as
    // subkeys of the shellex\ExtShellFolderViews key under the folder's
    // clsid type (or progid type, depending on the IShellFolderView impl).
    //
    // In desktop.ini, these are keys read from the [ExtShellFolderViews]
    // section, in the form of "{idView}" or "{idView}={idExtShellView}"
    //
    GUID idView;    // This is the GUID that specifies the View.

    // idExtShellView specifies the IDefViewExtInit IShellView extension
    // to use for this view.
    //
    // the shell's IShellFolderView implementations get this from
    // the "ISV" value under the shellex\ExtShellFolderViews\{idView}
    // key in the registry.
    //
    // In desktop.ini, this is read from the [ExtShellFolderViews]
    // section, in the form of "{idView}={idExtShellView}"
    //
    GUID idExtShellView; // [optional] This is the GUID for the extended IShellView

    // dwFlags - see the SFVF_ flags below.
    //
    // the shell's IShellFolderView implementations get this from
    // the "Attributes" value under the shellex\ExtShellFolderViews\{idView}
    // key in the registry.
    //
    // In desktop.ini, this is read from the [{idView}] section,
    // from the "Attributes=" line.
    //
    DWORD dwFlags;

    // lParam is passed to IShellView view extensions
    //
    // the shell's IShellFolderView implementations get this from
    // the "lparam" value under the shellex\ExtShellFolderViews\{idView}
    // key in the registry.
    //
    // In desktop.ini, this is read from the [{idView}] section,
    // from the "LParam=" line.
    //
    DWORD lParam;

    // wszMoniker is used to either bind to a docobject extended view,
    // or as the data for an idExtShellView view.
    //
    // the shell's IShellFolderView implementations get this from
    // the "PersistMoniker" value under the shellex\ExtShellFolderViews\{idView}
    // key in the registry.
    //
    // In desktop.ini, this is read from the [{idView}] section,
    // from the "PersistMoniker=" line.
    //
    WCHAR wszMoniker[MAX_PATH];

    // pCustomData overrides/provides menu and other ui options
    // for this view. If a WebViewFolderContents OC appears in the
    // docobject extended view, it pays attention to color and
    // background bitmaps.
    //
    // the shell's IShellFolderView implementations do not fill out this structure.
    //
    // In desktop.ini, this is read from the [{idView}] section,
    // from the "IconArea_Image", "IconArea_TextBackground", and "IconArea_Text".
    //
    CUSTOMVIEWSDATA *pCustomData; //This is valid only if SFVF_CUSTOMIZEDVIEW is set.

} SFVVIEWSDATA;

//
// SFVF_CUSTOMIZEDVIEW - This flag is set only for those views whose
// data are picked up from the Desktop.ini.
//
#define SFVF_CUSTOMIZEDVIEW  0x80000000

//
// SFVF_TREATASNORMAL - This flag is set for those views that wish to
// be treated as normal views, this means they will show up in the file-open/SaveAs
// Dialogs.
//
#define SFVF_TREATASNORMAL   0x40000000

//
// SFVF_NOWEBVIEWFOLDERCONTENTS - This is used to detect those views that
// never host the WebViewFolderContents OC and so cannot toggle on and off,
// ie, they are exclusive views such as Thumbnail View.
//
#define SFVF_NOWEBVIEWFOLDERCONTENTS   0x20000000


#undef  INTERFACE
#define INTERFACE   IEnumSFVViews

DECLARE_INTERFACE_(IEnumSFVViews, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IEnumSFVViews methods ***
    STDMETHOD(Next)  (THIS_ ULONG celt,
                      SFVVIEWSDATA **ppData,
                      ULONG *pceltFetched) PURE;
    STDMETHOD(Skip)  (THIS_ ULONG celt) PURE;
    STDMETHOD(Reset) (THIS) PURE;
    STDMETHOD(Clone) (THIS_ IEnumSFVViews **ppenum) PURE;
};


#undef  INTERFACE
#define INTERFACE   IPersistString

DECLARE_INTERFACE_(IPersistString, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IPersist methods ***
    STDMETHOD(GetClassID) (THIS_ LPCLSID lpClassID) PURE;

    // *** IPersistString methods ***
    STDMETHOD(Initialize) (THIS_ LPCWSTR pwszInit) PURE;
};

#pragma warning (disable: 4200)
#pragma warning (disable: 4510)
#pragma warning (disable: 4512)
#pragma warning (disable: 4610)
// From sdkinc\platform.h
// We can't simply include that file because it breaks other projects
// that include platform.h.
#undef UNSIZED_ARRAY
#undef EMPTY_SIZE
#   ifdef UNIX
        // Unix specific compiler problems
#       define UNSIZED_ARRAY 1
#   else
#       define UNSIZED_ARRAY
#   endif // UNIX
#define EMPTY_SIZE UNSIZED_ARRAY
#define QCMINFO_PLACE_BEFORE    0
#define QCMINFO_PLACE_AFTER     1
typedef struct _QCMINFO_IDMAP_PLACEMENT
{
    UINT id;
    UINT fFlags;
} QCMINFO_IDMAP_PLACEMENT;

typedef struct _QCMINFO_IDMAP
{
    UINT    nMaxIds;
    const QCMINFO_IDMAP_PLACEMENT   pIdList[EMPTY_SIZE];
} QCMINFO_IDMAP;

typedef struct _QCMINFO
{
    HMENU       hmenu;          // in
    UINT        indexMenu;      // in
    UINT        idCmdFirst;     // in/out
    UINT        idCmdLast;      // in
    const QCMINFO_IDMAP*  pIdMap;      //out
} QCMINFO;
typedef QCMINFO * LPQCMINFO;

// TBINFO flags
#define TBIF_APPEND     0
#define TBIF_PREPEND    1
#define TBIF_REPLACE    2
#define TBIF_DEFAULT      0x00000000
#define TBIF_INTERNETBAR  0x00010000
#define TBIF_STANDARDTOOLBAR   0x00020000
#define TBIF_NOTOOLBAR  0x00030000

typedef struct _TBINFO
{
    UINT        cbuttons;       // out
    UINT        uFlags;         // out (one of TBIF_ flags)
} TBINFO;


typedef struct _SFVM_SELCHANGE_DATA
{
    UINT uOldState;
    UINT uNewState;
    LPARAM lParamItem;
} SFVM_SELCHANGE_DATA;


typedef struct _COPYHOOKINFO
{
    HWND hwnd;
    DWORD wFunc;
    DWORD wFlags;
    LPCTSTR pszSrcFile;
    DWORD dwSrcAttribs;
    LPCTSTR pszDestFile;
    DWORD dwDestAttribs;
} COPYHOOKINFO;

typedef struct _DETAILSINFO
{
    LPCITEMIDLIST pidl;
    int fmt;
    int cxChar;
    STRRET str;
    int iImage;
} DETAILSINFO;

typedef struct _SFVM_PROPPAGE_DATA
{
    DWORD                dwReserved;
    LPFNADDPROPSHEETPAGE pfn;
    LPARAM               lParam;
} SFVM_PROPPAGE_DATA;

typedef struct _SFVM_HELPTOPIC_DATA
{
    WCHAR wszHelpFile[MAX_PATH];
    WCHAR wszHelpTopic[MAX_PATH];
} SFVM_HELPTOPIC_DATA;


// SFVM_GETNOTIFY: The shell will use the pidl returned from this message to synch
//                 change notifies.  Folders in Folder Shortcuts should return pidlTarget
//                 because that is where the change notifies are sent.
//
//                                uMsg    wParam             lParam
#define SFVM_MERGEMENU            1    // 0                  LPQCMINFO
#define SFVM_INVOKECOMMAND        2    // idCmd              0
#define SFVM_GETHELPTEXT          3    // idCmd,cchMax       pszText - Ansi/unicode (yup, old Tchar version)
#define SFVM_GETTOOLTIPTEXT       4    // idCmd,cchMax       pszText - Ansi/unicode (yup, old tchar version)
#define SFVM_GETBUTTONINFO        5    // 0                  LPTBINFO
#define SFVM_GETBUTTONS           6    // idCmdFirst,cbtnMax LPTBBUTTON
#define SFVM_INITMENUPOPUP        7    // idCmdFirst,nIndex  hmenu
#define SFVM_SELCHANGE            8    // idCmdFirst,nItem   SFVM_SELCHANGE_DATA*
#define SFVM_DRAWITEM             9    // idCmdFirst         DRAWITEMSTRUCT*
#define SFVM_MEASUREITEM         10    // idCmdFirst         MEASUREITEMSTRUCT*
#define SFVM_EXITMENULOOP        11    // -                  -
#define SFVM_PRERELEASE          12    // -                  -
#define SFVM_GETCCHMAX           13    // LPCITEMIDLIST      pcchMax
#define SFVM_FSNOTIFY            14    // LPCITEMIDLIST*     lEvent
#define SFVM_WINDOWCREATED       15    // hwnd               -
#define SFVM_WINDOWDESTROY       16    // hwnd               -
#define SFVM_REFRESH             17    // BOOL fPreOrPost    -                          NB: this may be called multiple times in a row
#define SFVM_SETFOCUS            18    // -                  -
#define SFVM_QUERYCOPYHOOK       20    // -                  -
#define SFVM_NOTIFYCOPYHOOK      21    // -                  COPYHOOKINFO*
#define SFVM_GETDETAILSOF        23    // iColumn            DETAILSINFO*
#define SFVM_COLUMNCLICK         24    // iColumn            -
#define SFVM_QUERYFSNOTIFY       25    // -                  SHChangeNotifyEntry *
#define SFVM_DEFITEMCOUNT        26    // -                  UINT*
#define SFVM_DEFVIEWMODE         27    // -                  FOLDERVIEWMODE*
#define SFVM_UNMERGEMENU         28    // -                  hmenu
#define SFVM_INSERTITEM          29    // pidl               -
#define SFVM_DELETEITEM          30    // pidl               -
#define SFVM_UPDATESTATUSBAR     31    // fInitialize        -
#define SFVM_BACKGROUNDENUM      32    // -                  -
#define SFVM_GETWORKINGDIR       33    // uMax               pszDir
#define SFVM_GETCOLSAVESTREAM    34    // flags              IStream **
#define SFVM_SELECTALL           35    // -                  -
#define SFVM_DIDDRAGDROP         36    // dwEffect           IDataObject *
#define SFVM_SUPPORTSIDENTITY    37    // -                  -
#define SFVM_FOLDERISPARENT      38    // -                  pidlChild
#define SFVM_SETISFV             39    // -                  IShellFolderView*
#define SFVM_GETVIEWS            40    // SHELLVIEWID*       IEnumSFVViews **
#define SFVM_THISIDLIST          41    // -                  LPITMIDLIST*
#define SFVM_GETITEMIDLIST       42    // iItem              LPITMIDLIST*
#define SFVM_SETITEMIDLIST       43    // iItem              LPITEMIDLIST
#define SFVM_INDEXOFITEMIDLIST   44    // *iItem             LPITEMIDLIST
#define SFVM_ODFINDITEM          45    // *iItem             NM_FINDITEM*
#define SFVM_HWNDMAIN            46    //                    hwndMain
#define SFVM_ADDPROPERTYPAGES    47    // -                  SFVM_PROPPAGE_DATA *
#define SFVM_BACKGROUNDENUMDONE  48    // -                  -
#define SFVM_GETNOTIFY           49    // LPITEMIDLIST*      LONG*
#define SFVM_ARRANGE             50    // -                  lParamSort
#define SFVM_QUERYSTANDARDVIEWS  51    // -                  BOOL *
#define SFVM_QUERYREUSEEXTVIEW   52    // -                  BOOL *
#define SFVM_GETSORTDEFAULTS     53    // iDirection         iParamSort
#define SFVM_GETEMPTYTEXT        54    // cchMax             pszText
#define SFVM_GETITEMICONINDEX    55    // iItem              int *piIcon
#define SFVM_DONTCUSTOMIZE       56    // -                  BOOL *pbDontCustomize
#define SFVM_SIZE                57    // resizing flag      cx, cy
#define SFVM_GETZONE             58    // -                  DWORD*
#define SFVM_GETPANE             59    // Pane ID            DWORD*
#define SFVM_ISOWNERDATA         60    // ISOWNERDATA        BOOL *
#define SFVM_GETODRANGEOBJECT    61    // iWhich             ILVRange **
#define SFVM_ODCACHEHINT         62    // -                  NMLVCACHEHINT *
#define SFVM_GETHELPTOPIC        63    // 0                  SFVM_HELPTOPIC_DATA *
#define SFVM_OVERRIDEITEMCOUNT   64    // -                  UINT*
#define SFVM_GETHELPTEXTW        65    // idCmd,cchMax       pszText - unicode
#define SFVM_GETTOOLTIPTEXTW     66    // idCmd,cchMax       pszText - unicode
#define SFVM_GETIPERSISTHISTORY  67    //                    IPersistHistory **
#define SFVM_GETANIMATION        68    // HINSTANCE *        WCHAR *
#define SFVM_GETHELPTEXTA        69    // idCmd,cchMax       pszText - ansi
#define SFVM_GETTOOLTIPTEXTA     70    // idCmd,cchMax       pszText - ansi

// The below two messages are only used in ownder data case, i.e. when SFVM_ISOWNERDATA returns TRUE
#define SFVM_GETICONOVERLAY      71    // iItem              int iOverlayIndex
#define SFVM_SETICONOVERLAY      72    // iItem              int * piOverlayIndex

#define SFVM_ALTERDROPEFFECT     73    // DWORD*             IDataObject*

// All of these macros are compatible with HANDLE_MSG in WINDOWSX.H

#define HANDLE_SFVM_MERGEMENU(pv, wP, lP, fn) \
    ((fn)((pv), (QCMINFO*)(lP)))

#define HANDLE_SFVM_INVOKECOMMAND(pv, wP, lP, fn) \
    ((fn)((pv), (UINT)(wP)))

#define HANDLE_SFVM_GETHELPTEXT(pv, wP, lP, fn) \
    ((fn)((pv), LOWORD(wP), HIWORD(wP), (LPTSTR)(lP)))

#define HANDLE_SFVM_GETTOOLTIPTEXT(pv, wP, lP, fn) \
    ((fn)((pv), LOWORD(wP), HIWORD(wP), (LPTSTR)(lP)))

#define HANDLE_SFVM_GETBUTTONINFO(pv, wP, lP, fn) \
    ((fn)((pv), (TBINFO*)(lP)))

#define HANDLE_SFVM_GETBUTTONS(pv, wP, lP, fn) \
    ((fn)((pv), LOWORD(wP), HIWORD(wP), (TBBUTTON*)(lP)))

#define HANDLE_SFVM_INITMENUPOPUP(pv, wP, lP, fn) \
    ((fn)((pv), LOWORD(wP), HIWORD(wP), (HMENU)(lP)))

#define HANDLE_SFVM_SELCHANGE(pv, wP, lP, fn) \
    ((fn)((pv), LOWORD(wP), HIWORD(wP), (SFVM_SELCHANGE_DATA*)(lP)))

#define HANDLE_SFVM_DRAWITEM(pv, wP, lP, fn) \
    ((fn)((pv), (UINT)(wP), (DRAWITEMSTRUCT*)(lP)))

#define HANDLE_SFVM_MEASUREITEM(pv, wP, lP, fn) \
    ((fn)((pv), (UINT)(wP), (MEASUREITEMSTRUCT*)(lP)))

#define HANDLE_SFVM_EXITMENULOOP(pv, wP, lP, fn) \
    ((fn)(pv))

#define HANDLE_SFVM_PRERELEASE(pv, wP, lP, fn) \
    ((fn)(pv))

#define HANDLE_SFVM_GETCCHMAX(pv, wP, lP, fn) \
    ((fn)((pv), (LPCITEMIDLIST)(wP), (UINT*)(lP)))

#define HANDLE_SFVM_FSNOTIFY(pv, wP, lP, fn) \
    ((fn)((pv), (LPCITEMIDLIST*)(wP), (lP)))

#define HANDLE_SFVM_WINDOWCREATED(pv, wP, lP, fn) \
    ((fn)((pv), (HWND)(wP)))

#define HANDLE_SFVM_WINDOWDESTROY(pv, wP, lP, fn) \
    ((fn)((pv), (HWND)(wP)))

#define HANDLE_SFVM_REFRESH(pv, wP, lP, fn) \
    ((fn)((pv), (BOOL) wP))

#define HANDLE_SFVM_SETFOCUS(pv, wP, lP, fn) \
    ((fn)(pv))

#define HANDLE_SFVM_QUERYCOPYHOOK(pv, wP, lP, fn) \
    ((fn)(pv))

#define HANDLE_SFVM_NOTIFYCOPYHOOK(pv, wP, lP, fn) \
    ((fn)((pv), (COPYHOOKINFO*)(lP)))

#define HANDLE_SFVM_GETDETAILSOF(pv, wP, lP, fn) \
    ((fn)((pv), (UINT)(wP), (DETAILSINFO*)(lP)))

#define HANDLE_SFVM_COLUMNCLICK(pv, wP, lP, fn) \
    ((fn)((pv), (UINT)(wP)))

#define HANDLE_SFVM_QUERYFSNOTIFY(pv, wP, lP, fn) \
    ((fn)((pv), (SHChangeNotifyEntry*)(lP)))

#define HANDLE_SFVM_DEFITEMCOUNT(pv, wP, lP, fn) \
    ((fn)((pv), (UINT*)(lP)))

#define HANDLE_SFVM_OVERRIDEITEMCOUNT(pv, wP, lP, fn) \
    ((fn)((pv), (UINT*)(lP)))

#define HANDLE_SFVM_DEFVIEWMODE(pv, wP, lP, fn) \
    ((fn)((pv), (FOLDERVIEWMODE*)(lP)))

#define HANDLE_SFVM_UNMERGEMENU(pv, wP, lP, fn) \
    ((fn)((pv), (HMENU)(lP)))

#define HANDLE_SFVM_INSERTITEM(pv, wP, lP, fn) \
    ((fn)((pv), (LPCITEMIDLIST)(lP)))

#define HANDLE_SFVM_DELETEITEM(pv, wP, lP, fn) \
    ((fn)((pv), (LPCITEMIDLIST)(lP)))

#define HANDLE_SFVM_UPDATESTATUSBAR(pv, wP, lP, fn) \
    ((fn)((pv), (BOOL)(wP)))

#define HANDLE_SFVM_BACKGROUNDENUM(pv, wP, lP, fn) \
    ((fn)(pv))

#define HANDLE_SFVM_GETWORKINGDIR(pv, wP, lP, fn) \
    ((fn)((pv), (UINT)(wP), (LPTSTR)(lP)))

#define HANDLE_SFVM_GETCOLSAVESTREAM(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (IStream**)(lP)))

#define HANDLE_SFVM_SELECTALL(pv, wP, lP, fn) \
    ((fn)(pv))

#define HANDLE_SFVM_DIDDRAGDROP(pv, wP, lP, fn) \
    ((fn)((pv), (DWORD)(wP), (IDataObject*)(lP)))

#define HANDLE_SFVM_HWNDMAIN(pv, wP, lP, fn) \
    ((fn)((pv), (HWND)(lP)))

#define HANDLE_SFVM_GETNOTIFY(pv, wP, lP, fn) \
    ((fn)((pv), (LPITEMIDLIST*)(wP), (LONG*)(lP)))

#define HANDLE_SFVM_SETISFV(pv, wP, lP, fn) \
    ((fn)((pv), (IShellFolderView*)(lP)))

#define HANDLE_SFVM_GETVIEWS(pv, wP, lP, fn) \
    ((fn)((pv), (SHELLVIEWID*)(wP), (IEnumSFVViews**)(lP)))

#define HANDLE_SFVM_THISIDLIST(pv, wP, lP, fn) \
    ((fn)((pv), (LPITEMIDLIST*)(lP)))

#define HANDLE_SFVM_GETITEMIDLIST(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (LPITEMIDLIST*)(lP)))

#define HANDLE_SFVM_SETITEMIDLIST(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (LPITEMIDLIST)(lP)))

#define HANDLE_SFVM_INDEXOFITEMIDLIST(pv, wP, lP, fn) \
    ((fn)((pv), (int*)(wP), (LPITEMIDLIST)(lP)))

#define HANDLE_SFVM_ODFINDITEM(pv, wP, lP, fn) \
    ((fn)((pv), (int*)(wP), (NM_FINDITEM*)(lP)))

#define HANDLE_SFVM_ADDPROPERTYPAGES(pv, wP, lP, fn) \
    ((fn)((pv), (SFVM_PROPPAGE_DATA *)(lP)))

#define HANDLE_SFVM_FOLDERISPARENT(pv, wP, lP, fn) \
    ((fn)((pv), (LPITEMIDLIST)(lP)))

#define HANDLE_SFVM_ARRANGE(pv, wP, lP, fn) \
    ((fn)((pv), (LPARAM)(lP)))

#define HANDLE_SFVM_QUERYSTANDARDVIEWS(pv, wP, lP, fn) \
    ((fn)((pv), (BOOL*)(lP)))

#define HANDLE_SFVM_QUERYREUSEEXTVIEW(pv, wP, lP, fn) \
    ((fn)((pv), (BOOL*)(lP)))

#define HANDLE_SFVM_GETEMPTYTEXT(pv, wP, lP, fn) \
    ((fn)((pv), (UINT)(wP), (LPTSTR)(lP)))

#define HANDLE_SFVM_GETITEMICONINDEX(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (int*)(lP)))

#define HANDLE_SFVM_SIZE(pv, wP, lP, fn) \
    ((fn)((pv), LOWORD(lP), HIWORD(lP)))

#define HANDLE_SFVM_GETZONE(pv, wP, lP, fn) \
    ((fn)((pv), (DWORD *)(lP)))

#define HANDLE_SFVM_GETPANE(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (DWORD *)(lP)))

#define HANDLE_SFVM_ISOWNERDATA(pv, wP, lP, fn) \
    ((fn)((pv), (BOOL*)(lP)))

#define HANDLE_SFVM_GETODRANGEOBJECT(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (ILVRange **)(lP)))

#define HANDLE_SFVM_ODCACHEHINT(pv, wP, lP, fn) \
    ((fn)((pv), (NMLVCACHEHINT*)(lP)))

#define HANDLE_SFVM_GETHELPTOPIC(pv, wP, lP, fn) \
    ((fn)((pv), (SFVM_HELPTOPIC_DATA *)(lP)))

#define HANDLE_SFVM_GETIPERSISTHISTORY(pv, wP, lP, fn) \
    ((fn)((pv), (IPersistHistory **)(lP)))

#define HANDLE_SFVM_SETICONOVERLAY(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (int)(lP)))

#define HANDLE_SFVM_GETICONOVERLAY(pv, wP, lP, fn) \
    ((fn)((pv), (wP), (int *)(lP)))

#define HANDLE_SFVM_ALTERDROPEFFECT(pv, wP, lP, fn) \
	((fn)((pv), (DWORD *)(wP), (IDataObject *)(lP)))

//
// Return values for SFVM_GETICONOVERLAY:
// If the icon overlay is not set and you want the shell to set it for you.
// return SFVOVERLAY_UNSET. This will cause the shell to retrieve the overlay and
// send SFVM_SETICONOVERLAY.
// If there is no overlay for this item at all, the correct return value is
// SFVOVERLAY_DEFAULT
// If you return SFVOVERLAY_UNSET every time for SFVM_GETICONOVERLAY, the
// shell will keep setting it indefinitely, so there is a danger of infinite loop
//
#define SFV_ICONOVERLAY_DEFAULT    0
#define SFV_ICONOVERLAY_UNSET      0xFFFFFFFF

//
// What you can return from SFVM_UPDATESTATUSBAR:
//
//  E_NOTIMPL - Message not handled; DefView should manage status bar
//
// Otherwise, update the status bar yourself and return a bitmask of the
// following codes indicating what you want DefView to do for you.
//
#define SFVUSB_HANDLED      0x0000  // Client handled completely - DefView does nothing
#define SFVUSB_INITED       0x0001  // Client initialized parts  - DefView will set text
#define SFVUSB_ALL          0x0001

// IShellFolderView interface for getting the SFV to do things
//

typedef struct _ITEMSPACING
{
    int cxSmall;
    int cySmall;
    int cxLarge;
    int cyLarge;
} ITEMSPACING;


// Define OPtions for SetObjectCount
#define SFVSOC_INVALIDATE_ALL   0x00000001  // Assumed to reset only what is neccessary...
#define SFVSOC_NOSCROLL         LVSICF_NOSCROLL

// defines for IShellFolderView::SelectItems()
#define SFVS_SELECT_NONE        0x0 // unselect all
#define SFVS_SELECT_ALLITEMS    0x1 // select all
#define SFVS_SELECT_INVERT      0x2 // Inver the selection

// defines for IShellFolderView::QuerySupport()
// allows a view to be queried to see if it supports various operations (useful for
// enabling menu items)
#define SFVQS_AUTO_ARRANGE      0x0001
#define SFVQS_ARRANGE_GRID      0x0002
#define SFVQS_SELECT_ALL        0x0004
#define SFVQS_SELECT_NONE       0x0008
#define SFVQS_SELECT_INVERT     0x0010

#undef  INTERFACE
#define INTERFACE   IShellFolderView

DECLARE_INTERFACE_(IShellFolderView, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellFolderView methods ***
    STDMETHOD(Rearrange) (THIS_ LPARAM lParamSort) PURE;
    STDMETHOD(GetArrangeParam) (THIS_ LPARAM *plParamSort) PURE;
    STDMETHOD(ArrangeGrid) (THIS) PURE;
    STDMETHOD(AutoArrange) (THIS) PURE;
    STDMETHOD(GetAutoArrange) (THIS) PURE;
    STDMETHOD(AddObject) (THIS_ LPITEMIDLIST pidl, UINT *puItem) PURE;
    STDMETHOD(GetObject) (THIS_ LPITEMIDLIST *ppidl, UINT uItem) PURE;
    STDMETHOD(RemoveObject) (THIS_ LPITEMIDLIST pidl, UINT *puItem) PURE;
    STDMETHOD(GetObjectCount) (THIS_ UINT *puCount) PURE;
    STDMETHOD(SetObjectCount) (THIS_ UINT uCount, UINT dwFlags) PURE;
    STDMETHOD(UpdateObject) (THIS_ LPITEMIDLIST pidlOld, LPITEMIDLIST pidlNew, UINT *puItem) PURE;
    STDMETHOD(RefreshObject) (THIS_ LPITEMIDLIST pidl, UINT *puItem) PURE;
    STDMETHOD(SetRedraw) (THIS_ BOOL bRedraw) PURE;
    STDMETHOD(GetSelectedCount) (THIS_ UINT *puSelected) PURE;
    STDMETHOD(GetSelectedObjects) (THIS_ LPCITEMIDLIST **pppidl, UINT *puItems) PURE;
    STDMETHOD(IsDropOnSource) (THIS_ IDropTarget *pDropTarget) PURE;
    STDMETHOD(GetDragPoint) (THIS_ POINT *ppt) PURE;
    STDMETHOD(GetDropPoint) (THIS_ POINT *ppt) PURE;
    STDMETHOD(MoveIcons) (THIS_ IDataObject *pDataObject) PURE;
    STDMETHOD(SetItemPos) (THIS_ LPCITEMIDLIST pidl, POINT *ppt) PURE;
    STDMETHOD(IsBkDropTarget) (THIS_ IDropTarget *pDropTarget) PURE;
    STDMETHOD(SetClipboard) (THIS_ BOOL bMove) PURE;
    STDMETHOD(SetPoints) (THIS_ IDataObject *pDataObject) PURE;
    STDMETHOD(GetItemSpacing) (THIS_ ITEMSPACING *pSpacing) PURE;
    STDMETHOD(SetCallback) (THIS_ IShellFolderViewCB* pNewCB, IShellFolderViewCB** ppOldCB) PURE;
    STDMETHOD(Select) ( THIS_  UINT dwFlags ) PURE;
    STDMETHOD(QuerySupport) (THIS_ UINT * pdwSupport ) PURE;
    STDMETHOD(SetAutomationObject)(THIS_ IDispatch* pdisp) PURE;
} ;

// CDTURNER start
////////////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_( IDefViewExtInit, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IDefViewExtInit methods ***
    STDMETHOD(SetOwnerDetails)(THIS_ IShellFolder * pSF, DWORD lParam ) PURE;
};

////////////////////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_( IDefViewExtInit2, IDefViewExtInit )
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IDefViewExtInit methods ***
    STDMETHOD(SetOwnerDetails)(THIS_ IShellFolder * pSF, DWORD lParam ) PURE;

    // *** IDefViewExtInit2 methods ***
    STDMETHOD( SetViewWindowStyle )( THIS_ DWORD dwBits, DWORD dwVal) PURE;
    STDMETHOD( SetViewWindowBkImage )( THIS_ LPCWSTR pszImage) PURE;
    STDMETHOD( SetViewWindowColors )( THIS_ COLORREF clrText, COLORREF clrTextBk, COLORREF clrWindow) PURE;

    STDMETHOD( IsModal )( THIS ) PURE;

    STDMETHOD( AutoAutoArrange )( THIS_ DWORD dwReserved ) PURE;
    STDMETHOD(SetStatusText)( THIS_ LPCWSTR pwszStatusText) PURE;
};
// CDTURNER end

// SHCreateShellFolderView struct
#include <pshpack8.h>
typedef struct _SFV_CREATE
{
    UINT            cbSize;
    IShellFolder*   pshf;
    IShellView*     psvOuter;
    IShellFolderViewCB* psfvcb; // No callback if NULL
} SFV_CREATE;
#include <poppack.h>

SHSTDAPI SHCreateShellFolderView(const SFV_CREATE* pcsfv, IShellView **ppsv);
#define SHCreateShellFolderViewORD      256

SHSTDAPI_(IShellFolderViewCB*) SHGetShellFolderViewCB(HWND hwndMain);
#define SHGetShellFolderViewCBORD   257


//===========================================================================
// Defview APIs to make name space author lives somewhat easier

typedef HRESULT (CALLBACK * LPFNDFMCALLBACK)(IShellFolder *psf, HWND hwnd,
                                             IDataObject *pdtobj, UINT uMsg, WPARAM wParam, LPARAM lParam);

SHSTDAPI CDefFolderMenu_Create2(LPCITEMIDLIST pidlFolder, HWND hwnd,
                                UINT cidl, LPCITEMIDLIST *apidl,
                                IShellFolder *psf, LPFNDFMCALLBACK lpfn,
                                UINT nKeys, const HKEY *ahkeyClsKeys,
                                IContextMenu **ppcm);

SHSTDAPI_(BOOL) SHOpenPropSheetA(LPCSTR pszCaption, HKEY ahkeys[], UINT cikeys,
                                 const CLSID * pclsidDefault, IDataObject *pdtobj,
                                 IShellBrowser *psb, LPCSTR pStartPage);
SHSTDAPI_(BOOL) SHOpenPropSheetW(LPCWSTR pszCaption, HKEY ahkeys[], UINT cikeys,
                                 const CLSID * pclsidDefault, IDataObject *pdtobj,
                                 IShellBrowser *psb, LPCWSTR pStartPage);
#ifdef UNICODE
#define SHOpenPropSheet  SHOpenPropSheetW
#else
#define SHOpenPropSheet  SHOpenPropSheetA
#endif // !UNICODE


// structure for lParam of DFM_INFOKECOMMANDEX
typedef struct
{
    DWORD  cbSize;

    DWORD  fMask;   // CMIC_MASK_ values for the invoke
    LPARAM lParam;  // same as lParam of DFM_INFOKECOMMAND
    UINT idCmdFirst;
    UINT idDefMax;
    LPCMINVOKECOMMANDINFO pici; // the whole thing so you can re-invoke on a child
} DFMICS, *PDFMICS;

//                                  uMsg       wParam       lParam
#define DFM_MERGECONTEXTMENU         1      // uFlags       LPQCMINFO
#define DFM_INVOKECOMMAND            2      // idCmd        pszArgs
#define DFM_ADDREF                   3      // 0            0
#define DFM_RELEASE                  4      // 0            0
#define DFM_GETHELPTEXT              5      // idCmd,cchMax pszText -Ansi
#define DFM_WM_MEASUREITEM           6      // ---from the message---
#define DFM_WM_DRAWITEM              7      // ---from the message---
#define DFM_WM_INITMENUPOPUP         8      // ---from the message---
#define DFM_VALIDATECMD              9      // idCmd        0
#define DFM_MERGECONTEXTMENU_TOP     10     // uFlags       LPQCMINFO
#define DFM_GETHELPTEXTW             11     // idCmd,cchMax pszText -Unicode
#define DFM_INVOKECOMMANDEX          12     // idCmd        PDFMICS
#define DFM_MAPCOMMANDNAME           13     // idCmd *      pszCommandName
#define DFM_GETDEFSTATICID           14     // idCmd *      0
#define DFM_GETVERBW                 15     // idCmd,cchMax pszText -Unicode
#define DFM_GETVERBA                 16     // idCmd,cchMax pszText -Ansi


// Commands from DFM_INVOKECOMMAND when strings are passed in
#define DFM_CMD_DELETE          ((WPARAM)-1)
#define DFM_CMD_MOVE            ((WPARAM)-2)
#define DFM_CMD_COPY            ((WPARAM)-3)
#define DFM_CMD_LINK            ((WPARAM)-4)
#define DFM_CMD_PROPERTIES      ((WPARAM)-5)
#define DFM_CMD_NEWFOLDER       ((WPARAM)-6)
#define DFM_CMD_PASTE           ((WPARAM)-7)
#define DFM_CMD_VIEWLIST        ((WPARAM)-8)
#define DFM_CMD_VIEWDETAILS     ((WPARAM)-9)
#define DFM_CMD_PASTELINK       ((WPARAM)-10)
#define DFM_CMD_PASTESPECIAL    ((WPARAM)-11)
#define DFM_CMD_MODALPROP       ((WPARAM)-12)
#define DFM_CMD_RENAME          ((WPARAM)-13)

STDAPI CDefFolderMenu_Create(LPCITEMIDLIST pidlFolder,
                             HWND hwndOwner,
                             UINT cidl, LPCITEMIDLIST * apidl,
                             IShellFolder *psf,
                             LPFNDFMCALLBACK lpfn,
                             HKEY hkeyProgID, HKEY hkeyBaseProgID,
                             IContextMenu ** ppcm);



STDAPI_(void) CDefFolderMenu_MergeMenu(HINSTANCE hinst, UINT idMainMerge, UINT idPopupMerge,
        LPQCMINFO pqcm);
STDAPI_(void) Def_InitFileCommands(ULONG dwAttr, HMENU hmInit, UINT idCmdFirst,
        BOOL bContext);
STDAPI_(void) Def_InitEditCommands(ULONG dwAttr, HMENU hmInit, UINT idCmdFirst,
        IDropTarget *pdtgt, UINT fContext);

// indeces in bitmap strip
#define VIEW_MOVETO     24
#define VIEW_COPYTO     25
#define VIEW_OPTIONS    26

//===========================================================================
// Default IShellView for IShellFolder
//===========================================================================

// Menu ID's
#define SFVIDM_FIRST            (FCIDM_SHVIEWLAST-0x0fff)
#define SFVIDM_LAST             (FCIDM_SHVIEWLAST)

// Hardcoded message values => This range can be used for messages that we
// pass from shdoc401 to shell32
#define SFVIDM_HARDCODED_FIRST  (SFVIDM_LAST-0x0010)
#define SFVIDM_HARDCODED_LAST   (SFVIDM_LAST)

#define SFVIDM_MISC_SETWEBVIEW  (SFVIDM_HARDCODED_FIRST)

// Popup menu ID's used in merging menus
#define SFVIDM_MENU_ARRANGE     (SFVIDM_FIRST + 0x0001)
#define SFVIDM_MENU_VIEW        (SFVIDM_FIRST + 0x0002)
#define SFVIDM_MENU_SELECT      (SFVIDM_FIRST + 0x0003)

#define SHARED_FILE_FIRST               0x0010
#define SHARED_FILE_LINK                (SHARED_FILE_FIRST + 0x0000)
#define SHARED_FILE_DELETE              (SHARED_FILE_FIRST + 0x0001)
#define SHARED_FILE_RENAME              (SHARED_FILE_FIRST + 0x0002)
#define SHARED_FILE_PROPERTIES          (SHARED_FILE_FIRST + 0x0003)

#define SHARED_EDIT_FIRST               0x0018
#define SHARED_EDIT_CUT                 (SHARED_EDIT_FIRST + 0x0000)
#define SHARED_EDIT_COPY                (SHARED_EDIT_FIRST + 0x0001)
#define SHARED_EDIT_PASTE               (SHARED_EDIT_FIRST + 0x0002)
#define SHARED_EDIT_UNDO                (SHARED_EDIT_FIRST + 0x0003)
#define SHARED_EDIT_PASTELINK           (SHARED_EDIT_FIRST + 0x0004)
#define SHARED_EDIT_PASTESPECIAL        (SHARED_EDIT_FIRST + 0x0005)
#define SHARED_EDIT_COPYTO              (SHARED_EDIT_FIRST + 0x0006)
#define SHARED_EDIT_MOVETO              (SHARED_EDIT_FIRST + 0x0007)

#define SFVIDM_FILE_FIRST               (SFVIDM_FIRST + SHARED_FILE_FIRST)
#define SFVIDM_FILE_LINK                (SFVIDM_FIRST + SHARED_FILE_LINK)
#define SFVIDM_FILE_DELETE              (SFVIDM_FIRST + SHARED_FILE_DELETE)
#define SFVIDM_FILE_RENAME              (SFVIDM_FIRST + SHARED_FILE_RENAME)
#define SFVIDM_FILE_PROPERTIES          (SFVIDM_FIRST + SHARED_FILE_PROPERTIES)

#define SFVIDM_EDIT_FIRST               (SFVIDM_FIRST + SHARED_EDIT_FIRST)
#define SFVIDM_EDIT_CUT                 (SFVIDM_FIRST + SHARED_EDIT_CUT)
#define SFVIDM_EDIT_COPY                (SFVIDM_FIRST + SHARED_EDIT_COPY)
#define SFVIDM_EDIT_PASTE               (SFVIDM_FIRST + SHARED_EDIT_PASTE)
#define SFVIDM_EDIT_UNDO                (SFVIDM_FIRST + SHARED_EDIT_UNDO)
#define SFVIDM_EDIT_PASTELINK           (SFVIDM_FIRST + SHARED_EDIT_PASTELINK)
#define SFVIDM_EDIT_PASTESPECIAL        (SFVIDM_FIRST + SHARED_EDIT_PASTESPECIAL)
#define SFVIDM_EDIT_COPYTO              (SFVIDM_FIRST + SHARED_EDIT_COPYTO)
#define SFVIDM_EDIT_MOVETO              (SFVIDM_FIRST + SHARED_EDIT_MOVETO)

#define SFVIDM_SELECT_FIRST             (SFVIDM_FIRST + 0x0020)
#define SFVIDM_SELECT_ALL               (SFVIDM_SELECT_FIRST + 0x0001)
#define SFVIDM_SELECT_INVERT            (SFVIDM_SELECT_FIRST + 0x0002)
#define SFVIDM_DESELECT_ALL             (SFVIDM_SELECT_FIRST + 0x0003)

#define SFVIDM_VIEW_FIRST               (SFVIDM_FIRST + 0x0028)
#define SFVIDM_VIEW_ICON                (SFVIDM_VIEW_FIRST + 0x0001)
#define SFVIDM_VIEW_SMALLICON           (SFVIDM_VIEW_FIRST + 0x0002)
#define SFVIDM_VIEW_LIST                (SFVIDM_VIEW_FIRST + 0x0003)
#define SFVIDM_VIEW_DETAILS             (SFVIDM_VIEW_FIRST + 0x0004)
#define SFVIDM_VIEW_FIRSTVIEW           (SFVIDM_VIEW_ICON)
#define SFVIDM_VIEW_LASTVIEW            (SFVIDM_VIEW_DETAILS)
#define SFVIDM_VIEW_OPTIONS             (SFVIDM_VIEW_FIRST + 0x0005)
#define SFVIDM_VIEW_VIEWMENU            (SFVIDM_VIEW_FIRST + 0x0006)
#define SFVIDM_VIEW_CUSTOMWIZARD        (SFVIDM_VIEW_FIRST + 0x0007)
#define SFVIDM_VIEW_COLSETTINGS         (SFVIDM_VIEW_FIRST + 0x0008)

#define SFVIDM_VIEW_EXTFIRST            (SFVIDM_VIEW_FIRST + 0x0009)
#define SFVIDM_VIEW_EXTLAST             (SFVIDM_VIEW_EXTFIRST + 0x0017)
#define SFVIDM_VIEW_SVEXTFIRST          (SFVIDM_VIEW_EXTFIRST)
#define SFVIDM_VIEW_SVEXTLAST           (SFVIDM_VIEW_EXTFIRST + 0x000F)
#define SFVIDM_VIEW_EXTENDEDFIRST       (SFVIDM_VIEW_EXTFIRST + 0x0010)
#define SFVIDM_VIEW_EXTENDEDLAST        (SFVIDM_VIEW_EXTLAST)

#define SFVIDM_ARRANGE_FIRST            (SFVIDM_FIRST + 0x0050)
#define SFVIDM_ARRANGE_AUTO             (SFVIDM_ARRANGE_FIRST + 0x0001)
#define SFVIDM_ARRANGE_GRID             (SFVIDM_ARRANGE_FIRST + 0x0002)
#define SFVIDM_ARRANGE_DISPLAYICONS     (SFVIDM_ARRANGE_FIRST + 0x0003)

#define SFVIDM_TOOL_FIRST               (SFVIDM_FIRST + 0x0060)
#define SFVIDM_TOOL_CONNECT             (SFVIDM_TOOL_FIRST + 0x0001)
#define SFVIDM_TOOL_DISCONNECT          (SFVIDM_TOOL_FIRST + 0x0002)
#define SFVIDM_TOOL_OPTIONS             (SFVIDM_TOOL_FIRST + 0x0003)

#define SFVIDM_HELP_FIRST               (SFVIDM_FIRST + 0x0070)
#define SFVIDM_HELP_TOPIC               (SFVIDM_HELP_FIRST + 0x0001)

#define SFVIDM_MISC_FIRST               (SFVIDM_FIRST + 0x0100)
#define SFVIDM_MISC_REFRESH             (SFVIDM_MISC_FIRST + 0x0003)

// Range for the client's additional menus
#define SFVIDM_CLIENT_FIRST             (SFVIDM_FIRST + 0x0200)
#define SFVIDM_CLIENT_LAST              (SFVIDM_FIRST + 0x02ff)

#define SFVIDM_DESKTOP_FIRST            (SFVIDM_FIRST + 0x0400)
#define SFVIDM_DESKTOPHTML_CUSTOMIZE    (SFVIDM_DESKTOP_FIRST + 0x0000)
#define SFVIDM_DESKTOPHTML_NEWITEM      (SFVIDM_DESKTOP_FIRST + 0x0001)
#define SFVIDM_DESKTOPHTML_WEBCONTENT   (SFVIDM_DESKTOP_FIRST + 0x0002)
#define SFVIDM_DESKTOPHTML_ICONS        (SFVIDM_DESKTOP_FIRST + 0x0003)
#define SFVIDM_DESKTOPHTML_LOCK         (SFVIDM_DESKTOP_FIRST + 0x0004)
#define SFVIDM_DESKTOPHTML_SYNCHRONIZE  (SFVIDM_DESKTOP_FIRST + 0x0005)
#define SFVIDM_DESKTOPHTML_ADDSEPARATOR (SFVIDM_DESKTOP_FIRST + 0x0010)
// The values after ADDSEPARATOR are reserved for each item dynamically added to the menu
#define SFVIDM_DESKTOP_LAST             (SFVIDM_FIRST + 0x04ff)

#define SFVIDM_COLUMN_FIRST             (SFVIDM_FIRST + 0x0500)
#define SFVIDM_COLUMN_LAST              (SFVIDM_FIRST + 0x05ff)


// Range for context menu id's
#define SFVIDM_CONTEXT_FIRST            (SFVIDM_FIRST + 0x0800)
#define SFVIDM_CONTEXT_LAST             (SFVIDM_FIRST + 0x0900)
#define SFVIDM_BACK_CONTEXT_FIRST       (SFVIDM_FIRST + 0x0901)
#define SFVIDM_BACK_CONTEXT_LAST        (SFVIDM_FIRST + 0x09ff)



typedef TBINFO * LPTBINFO;

typedef COPYHOOKINFO *LPCOPYHOOKINFO;

typedef DETAILSINFO *PDETAILSINFO;

//                              uMsg                     wParam         lParam
#define DVM_MERGEMENU           SFVM_MERGEMENU        // uFlags             LPQCMINFO
#define DVM_INVOKECOMMAND       SFVM_INVOKECOMMAND    // idCmd              0
#define DVM_GETHELPTEXT         SFVM_GETHELPTEXT      // idCmd,cchMax       pszText - Ansi
#define DVM_GETTOOLTIPTEXT      SFVM_GETTOOLTIPTEXT   // idCmd,cchMax       pszText
#define DVM_GETBUTTONINFO       SFVM_GETBUTTONINFO    // 0                  LPTBINFO
#define DVM_GETBUTTONS          SFVM_GETBUTTONS       // idCmdFirst,cbtnMax LPTBBUTTON
#define DVM_INITMENUPOPUP       SFVM_INITMENUPOPUP    // idCmdFirst,nIndex  hmenu
#define DVM_SELCHANGE           SFVM_SELCHANGE        // idCmdFirst,nItem   PDVSELCHANGEINFO
#define DVM_DRAWITEM            SFVM_DRAWITEM         // idCmdFirst         pdis
#define DVM_MEASUREITEM         SFVM_MEASUREITEM      // idCmdFirst         pmis
#define DVM_EXITMENULOOP        SFVM_EXITMENULOOP     // -                  -
#define DVM_RELEASE             SFVM_PRERELEASE       // -                  lSelChangeInfo (ShellFolder private)
#define DVM_GETCCHMAX           SFVM_GETCCHMAX        // pidlItem           pcchMax
#define DVM_FSNOTIFY            SFVM_FSNOTIFY         // LPITEMIDLIST*      lEvent
#define DVM_WINDOWCREATED       SFVM_WINDOWCREATED    // hwnd               PDVSELCHANGEINFO
#define DVM_WINDOWDESTROY       SFVM_WINDOWDESTROY    // hwnd               PDVSELCHANGEINFO
#define DVM_REFRESH             SFVM_REFRESH          // -                  lSelChangeInfo
#define DVM_SETFOCUS            SFVM_SETFOCUS         // -                  lSelChangeInfo
#define DVM_KILLFOCUS           19                    // unused
#define DVM_QUERYCOPYHOOK       SFVM_QUERYCOPYHOOK    // -                  -
#define DVM_NOTIFYCOPYHOOK      SFVM_NOTIFYCOPYHOOK   // -                  LPCOPYHOOKINFO
#define DVM_NOTIFY              SFVM_NOTIFY           // idFrom             LPNOTIFY
#define DVM_GETDETAILSOF        SFVM_GETDETAILSOF     // iColumn            PDETAILSINFO
#define DVM_COLUMNCLICK         SFVM_COLUMNCLICK      // iColumn            -
#define DVM_QUERYFSNOTIFY       SFVM_QUERYFSNOTIFY    // -                  FSNotifyEntry *
#define DVM_DEFITEMCOUNT        SFVM_DEFITEMCOUNT     // -                  PINT
#define DVM_DEFVIEWMODE         SFVM_DEFVIEWMODE      // -                  PFOLDERVIEWMODE
#define DVM_UNMERGEMENU         SFVM_UNMERGEMENU      // uFlags
#define DVM_INSERTITEM          SFVM_INSERTITEM       // pidl               PDVSELCHANGEINFO
#define DVM_DELETEITEM          SFVM_DELETEITEM       // pidl               PDVSELCHANGEINFO
#define DVM_UPDATESTATUSBAR     SFVM_UPDATESTATUSBAR  // -                  lSelChangeInfo
#define DVM_BACKGROUNDENUM      SFVM_BACKGROUNDENUM   //
#define DVM_GETWORKINGDIR       SFVM_GETWORKINGDIR    //
#define DVM_GETCOLSAVESTREAM    SFVM_GETCOLSAVESTREAM // flags              IStream **
#define DVM_SELECTALL           SFVM_SELECTALL        //                    lSelChangeInfo
#define DVM_DIDDRAGDROP         SFVM_DIDDRAGDROP      // dwEffect           IDataObject *
#define DVM_SUPPORTSIDENTIFY    SFVM_SUPPORTSIDENTITY // -                  -
#define DVM_FOLDERISPARENT      SFVM_FOLDERISPARENT   // -                  pidlChild

// max length for guid strings
#define GUIDSTR_MAX (1+ 8 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 12 + 1 + 1)

typedef struct _DVSELCHANGEINFO {
    UINT uOldState;
    UINT uNewState;
    LPARAM lParamItem;
    LPARAM* plParam;
} DVSELCHANGEINFO, *PDVSELCHANGEINFO;

typedef HRESULT (CALLBACK * LPFNVIEWCALLBACK)(IShellView *psvOuter,
                                                IShellFolder *psf,
                                                HWND hwndMain,
                                                UINT uMsg,
                                                WPARAM wParam,
                                                LPARAM lParam);

// SHCreateShellFolderViewEx struct
typedef struct _CSFV
{
    UINT            cbSize;
    IShellFolder *  pshf;
    IShellView *    psvOuter;
    LPCITEMIDLIST   pidl;
    LONG            lEvents;
    LPFNVIEWCALLBACK pfnCallback;       // No callback if NULL
    FOLDERVIEWMODE  fvm;
} CSFV, * LPCSFV;

// Tell the FolderView to rearrange.  The lParam will be passed to
// IShellFolder::CompareIDs
#define SFVM_REARRANGE          0x00000001
#define ShellFolderView_ReArrange(_hwnd, _lparam) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_REARRANGE, _lparam)

// Get the last sorting parameter given to FolderView
#define SFVM_GETARRANGEPARAM    0x00000002
#define ShellFolderView_GetArrangeParam(_hwnd) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_GETARRANGEPARAM, 0L)

// Add an OBJECT into the view (May need to add insert also)
#define SFVM_ADDOBJECT         0x00000003
#define ShellFolderView_AddObject(_hwnd, _pidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_ADDOBJECT, (LPARAM)_pidl)

// Gets the count of objects in the view
#define SFVM_GETOBJECTCOUNT         0x00000004
#define ShellFolderView_GetObjectCount(_hwnd) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_GETOBJECTCOUNT, (LPARAM)0)

// Returns a pointer to the Idlist associated with the specified index
// Returns NULL if at end of list.
#define SFVM_GETOBJECT         0x00000005
#define ShellFolderView_GetObject(_hwnd, _iObject) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_GETOBJECT, _iObject)

// Remove an OBJECT into the view (This works by pidl, may need index also);
#define SFVM_REMOVEOBJECT         0x00000006
#define ShellFolderView_RemoveObject(_hwnd, _pidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_REMOVEOBJECT, (LPARAM)_pidl)

// updates an object by passing in pointer to two PIDLS, the first
// is the old pidl, the second one is the one with update information.
//
// _ppidl[1] must be a *copy* of a pidl, as control over the lifetime
// of the pidl belongs to the view after successful completion of
// this call.  (Unsuccessful completion (a -1 return) implies failure
// and the caller must free the memory.)  Win95 waits a while before
// freeing the pidl, IE4 frees the pidl immediately.
// IShellFolderView::UpdateObject does not suffer from this problem.
//
#define SFVM_UPDATEOBJECT         0x00000007
#define ShellFolderView_UpdateObject(_hwnd, _ppidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_UPDATEOBJECT, (LPARAM)_ppidl)

// Sets the redraw mode for the window that is displaying the information
#define SFVM_SETREDRAW           0x00000008
#define ShellFolderView_SetRedraw(_hwnd, fRedraw) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_SETREDRAW, (LPARAM)fRedraw)

// Returns an array of the selected IDS to the caller.
//     lparam is a pointer to receive the idlists into
//     return value is the count of items in the array.
#define SFVM_GETSELECTEDOBJECTS 0x00000009
#define ShellFolderView_GetSelectedObjects(_hwnd, ppidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_GETSELECTEDOBJECTS, (LPARAM)ppidl)

// Checks if the current drop is on the view window
//     lparam is unused
//     return value is TRUE if the current drop is upon the background of the
//         view window, FALSE otherwise
#define SFVM_ISDROPONSOURCE     0x0000000a
#define ShellFolderView_IsDropOnSource(_hwnd, _pdtgt) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_ISDROPONSOURCE, (LPARAM)_pdtgt)

// Moves the selected icons in the listview
//     lparam is a pointer to a drop target
//     return value is unused
#define SFVM_MOVEICONS          0x0000000b
#define ShellFolderView_MoveIcons(_hwnd, _pdt) \
        (void)SHShellFolderView_Message(_hwnd, SFVM_MOVEICONS, (LPARAM)(LPDROPTARGET)_pdt)

// Gets the start point of a drag-drop
//     lparam is a pointer to a point
//     return value is unused
#define SFVM_GETDRAGPOINT       0x0000000c
#define ShellFolderView_GetDragPoint(_hwnd, _ppt) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_GETDRAGPOINT, (LPARAM)(LPPOINT)_ppt)

// Gets the end point of a drag-drop
//     lparam is a pointer to a point
//     return value is unused
#define SFVM_GETDROPPOINT       0x0000000d
#define ShellFolderView_GetDropPoint(_hwnd, _ppt) \
        SHShellFolderView_Message(_hwnd, SFVM_GETDROPPOINT, (LPARAM)(LPPOINT)_ppt)

#define ShellFolderView_GetAnchorPoint(_hwnd, _fStart, _ppt) \
        (BOOL)((_fStart) ? ShellFolderView_GetDragPoint(_hwnd, _ppt) : ShellFolderView_GetDropPoint(_hwnd, _ppt))

typedef struct _SFV_SETITEMPOS
{
        LPCITEMIDLIST pidl;
        POINT pt;
} SFV_SETITEMPOS, *LPSFV_SETITEMPOS;

// Sets the position of an item in the viewer
//     lparam is a pointer to a SVF_SETITEMPOS
//     return value is unused
#define SFVM_SETITEMPOS         0x0000000e
#define ShellFolderView_SetItemPos(_hwnd, _pidl, _x, _y) \
{       SFV_SETITEMPOS _sip = {_pidl, {_x, _y}}; \
        SHShellFolderView_Message(_hwnd, SFVM_SETITEMPOS, (LPARAM)(LPSFV_SETITEMPOS)&_sip);}

// Determines if a given drop target interface is the one being used for
// the background of the ShellFolderView (as opposed to an object in the
// view)
//     lparam is a pointer to a drop target interface
//     return value is TRUE if it is the background drop target, FALSE otherwise
#define SFVM_ISBKDROPTARGET     0x0000000f
#define ShellFolderView_IsBkDropTarget(_hwnd, _pdptgt) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_ISBKDROPTARGET, (LPARAM)(LPDROPTARGET)_pdptgt)

//  Notifies a ShellView when one of its objects get put on the clipboard
//  as a result of a menu command.
//
//  called by defcm.c when it does a copy/cut
//
//     lparam is the dwEffect (DROPEFFECT_MOVE, DROPEFFECT_COPY)
//     return value is void.
#define SFVM_SETCLIPBOARD       0x00000010
#define ShellFolderView_SetClipboard(_hwnd, _dwEffect) \
        (void)SHShellFolderView_Message(_hwnd, SFVM_SETCLIPBOARD, (LPARAM)(DWORD)(_dwEffect))


// sets auto arrange
#define SFVM_AUTOARRANGE        0x00000011
#define ShellFolderView_AutoArrange(_hwnd) \
        (void)SHShellFolderView_Message(_hwnd, SFVM_AUTOARRANGE, 0)

// sets snap to grid
#define SFVM_ARRANGEGRID        0x00000012
#define ShellFolderView_ArrangeGrid(_hwnd) \
        (void)SHShellFolderView_Message(_hwnd, SFVM_ARRANGEGRID, 0)

#define SFVM_GETAUTOARRANGE     0x00000013
#define ShellFolderView_GetAutoArrange(_hwnd) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_GETAUTOARRANGE, 0)

#define SFVM_GETSELECTEDCOUNT     0x00000014
#define ShellFolderView_GetSelectedCount(_hwnd) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_GETSELECTEDCOUNT, 0)

typedef ITEMSPACING *LPITEMSPACING;

#define SFVM_GETITEMSPACING     0x00000015
#define ShellFolderView_GetItemSpacing(_hwnd, lpis) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_GETITEMSPACING, (LPARAM)lpis)

// Causes an object to be repainted
#define SFVM_REFRESHOBJECT      0x00000016
#define ShellFolderView_RefreshObject(_hwnd, _ppidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_REFRESHOBJECT, (LPARAM)_ppidl)

// Causes the the whole view to be refreshed
#define ShellFolderView_RefreshAll(_hwnd) \
        (LPARAM)PostMessage(_hwnd, WM_KEYDOWN, (WPARAM)VK_F5, (LPARAM)0);

#define SFVM_SETPOINTS           0x00000017
#define ShellFolderView_SetPoints(_hwnd, _pdtobj) \
        (void)SHShellFolderView_Message(_hwnd, SFVM_SETPOINTS, (LPARAM)_pdtobj)

// SVM_SELECTANDPOSITIONITEM lParam
typedef struct
{
        LPCITEMIDLIST pidl;     // relative pidl to the view
        UINT  uSelectFlags;     // select flags
        BOOL fMove; // if true, we should also move it to point pt
        POINT pt;
} SFM_SAP;

// shell view messages
#define SVM_SELECTITEM                  (WM_USER + 1)
#define SVM_MOVESELECTEDITEMS           (WM_USER + 2)
#define SVM_GETANCHORPOINT              (WM_USER + 3)
#define SVM_GETITEMPOSITION             (WM_USER + 4)
#define SVM_SELECTANDPOSITIONITEM       (WM_USER + 5)

//===========================================================================
// CDefShellFolder members (for easy subclassing)
//===========================================================================

// Default implementation (no dependencies to the instance data)
STDMETHODIMP CDefShellFolder_QueryInterface(IShellFolder *psf, REFIID riid, void **ppv);
STDMETHODIMP CDefShellFolder_BindToStorage(IShellFolder *psf, LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv);
STDMETHODIMP CDefShellFolder_BindToObject(IShellFolder *psf, LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv);
STDMETHODIMP CDefShellFolder_GetAttributesOf(IShellFolder *psf, UINT cidl, LPCITEMIDLIST * apidl, ULONG * rgfOut);
STDMETHODIMP CDefShellFolder_SetNameOf(IShellFolder *psf, HWND hwndOwner, LPCITEMIDLIST pidl, LPCOLESTR pszName, DWORD dwReserved, LPITEMIDLIST * ppidlOut);

// File Search APIS
SHSTDAPI_(IContextMenu *) SHFind_InitMenuPopup(HMENU hmenu, HWND hwndOwner, UINT idCmdFirst, UINT idCmdLast);

SHSTDAPI_(void) Control_RunDLL(HWND hwndStub, HINSTANCE hAppInstance, LPSTR pszCmdLine, int nCmdShow);
SHSTDAPI_(void) Control_RunDLLW(HWND hwndStub, HINSTANCE hAppInstance, LPWSTR pszCmdLine, int nCmdShow);
SHSTDAPI_(void) Control_RunDLLAsUserW(HWND hwndStub, HINSTANCE hAppInstance, LPWSTR lpwszCmdLine, int nCmdShow);


// to add 16 bit pages to 32bit things.  hGlobal can be NULL
SHSTDAPI_(UINT) SHAddPages16(HGLOBAL hGlobal, LPCTSTR pszDllEntry, LPFNADDPROPSHEETPAGE pfnAddPage, LPARAM lParam);

SHSTDAPI SHCreateShellFolderViewEx(LPCSFV pcsfv, IShellView ** ppsv);


// Access to MSHMTL's ditherer

/* Definition of interface: IIntDitherer */
#undef INTERFACE
#define INTERFACE IIntDitherer

DECLARE_INTERFACE_(IIntDitherer, IUnknown)
{
#ifndef NO_BASEINTERFACE_FUNCS
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
#endif

    // *** IIntDitherer methods ***
    STDMETHOD(DitherTo8bpp)(THIS_ BYTE * pDestBits, LONG nDestPitch,
                    BYTE * pSrcBits, LONG nSrcPitch, REFGUID bfidSrc,
                    RGBQUAD * prgbDestColors, RGBQUAD * prgbSrcColors,
                    BYTE * pbDestInvMap,
                    LONG x, LONG y, LONG cx, LONG cy,
                    LONG lDestTrans, LONG lSrcTrans) PURE;
};

#ifdef COBJMACROS


#define IntDitherer_QueryInterface(This,riid,ppvObject) \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IntDitherer_AddRef(This)    \
    (This)->lpVtbl -> AddRef(This)

#define IntDitherer_Release(This)   \
    (This)->lpVtbl -> Release(This)

#define IntDitherer_DitherTo8bpp(This, pDestBits, nDestPitch, \
                    pSrcBits, nSrcPitch, bfidSrc, \
                    prgbDestColors, prgbSrcColors, \
                    pbDestInvMap, x, y, cx, cy, \
                    lDestTrans, lSrcTrans)  \
    (This)->lpVtbl -> DitherTo8bpp(This, pDestBits, nDestPitch, pSrcBits, nSrcPitch, bfidSrc, \
                    prgbDestColors, prgbSrcColors, pbDestInvMap, x, y, cx, cy, lDestTrans, lSrcTrans)

#endif

//      PID_IS_SCHEME       [VT_UI4]      Scheme value
#define PID_IS_FIRST         2
#define PID_IS_SCHEME        3
#define PID_IS_IDLIST        14
#define PID_IS_LAST          14
//      PID_INTSITE_FLAGS        [VT_UI4]      PIDISF_ flags
//      PID_INTSITE_CONTENTLEN   [VT_UI4]      Content length
//      PID_INTSITE_CONTENTCODE  [VT_UI8]      Content code
//      PID_INTSITE_FRAGMENT     [VT_NULL]     Fragments
#define PID_INTSITE_FIRST         2
#define PID_INTSITE_FRAGMENT      17
#define PID_INTSITE_LAST          21
  //Is URL entered in IE history bucket?
#define PIDISF_HISTORY          0x10000000
// Athena needs this to determine whether to double click or not.
//
// NOTE: if user uninstalls IE4, the Win95 code will
// realize that the size has changed (even though it's not
// in this struct, it is saved) and drop back to default state.
//
// Since we're exporting this so ISVs can get access to our flags,
// let's avoid the need for a thunk and export the beginning section
// of this structure ONLY. Here are the A and W versions for internal use:
//
typedef struct {
    // These were in Win95 //
    BOOL fShowAllObjects : 1;
    BOOL fShowExtensions : 1;
    BOOL fNoConfirmRecycle : 1;

    // The below were added for IE4 //
    BOOL fShowSysFiles : 1;
    BOOL fShowCompColor : 1;
    BOOL fDoubleClickInWebView : 1;
    BOOL fDesktopHTML : 1;
    BOOL fWin95Classic : 1;
    BOOL fDontPrettyPath : 1;
    BOOL fShowAttribCol : 1; // No longer used, dead bit
    BOOL fMapNetDrvBtn : 1;
    BOOL fShowInfoTip : 1;
    BOOL fHideIcons : 1;
    BOOL fWebView : 1;
    BOOL fFilter : 1;
    BOOL fShowSuperHidden : 1;

    // These were in Win95 Gold
    LPSTR pszHiddenFileExts;
    UINT cbHiddenFileExts;

    // These must have been an OSR release, as they aren't on \\guilo\slmadd\src\dev\inc16\shsemip.h
    LPARAM lParamSort;
    int    iSortDirection;

    // This was added for IE4
    UINT   version;

    // new for win2k. need notUsed var to calc the right size of ie4 struct
    // FIELD_OFFSET does not work on bit fields
    UINT uNotUsed; // feel free to rename and use
    BOOL fSepProcess: 1;
    // If you need a new flag, steal a bit from from fSpareFlags.
    UINT fSpareFlags : 15;

    //
    // If you add any fields to this structure, you
    // must also add upgrade code to shell32\util.cpp.
    //
} SHELLSTATEA, * LPSHELLSTATEA;

typedef struct {
    BOOL fShowAllObjects : 1;
    BOOL fShowExtensions : 1;
    BOOL fNoConfirmRecycle : 1;
    BOOL fShowSysFiles : 1;
    BOOL fShowCompColor : 1;
    BOOL fDoubleClickInWebView : 1;
    BOOL fDesktopHTML : 1;
    BOOL fWin95Classic : 1;
    BOOL fDontPrettyPath : 1;
    BOOL fShowAttribCol : 1;
    BOOL fMapNetDrvBtn : 1;
    BOOL fShowInfoTip : 1;
    BOOL fHideIcons : 1;
    BOOL fWebView : 1;
    BOOL fFilter : 1;
    BOOL fShowSuperHidden : 1;

    LPWSTR pszHiddenFileExts;
    UINT   cbHiddenFileExts;

    LPARAM lParamSort;
    int    iSortDirection;
    UINT   version;

    // new for win2k. need notUsed var to calc the right size of ie4 struct
    // FIELD_OFFSET does not work on bit fields
    UINT uNotUsed; // feel free to rename and use
    BOOL fSepProcess: 1;
    // If you need a new flag, steal a bit from from fSpareFlags.
    UINT fSpareFlags : 15;

    //
    // If you add any fields to this structure, you
    // must also add upgrade code to shell32\util.cpp.
    //
} SHELLSTATEW, * LPSHELLSTATEW;

#define SHELLSTATEVERSION_IE4 9
#define SHELLSTATEVERSION 10 // rev if defaults change but size doesn't //

#ifdef UNICODE
#define SHELLSTATE   SHELLSTATEW
#define LPSHELLSTATE LPSHELLSTATEW
#else
#define SHELLSTATE   SHELLSTATEA
#define LPSHELLSTATE LPSHELLSTATEA
#endif

#define SHELLSTATE_SIZE_WIN95 FIELD_OFFSET(SHELLSTATE,lParamSort)
#define SHELLSTATE_SIZE_NT4   FIELD_OFFSET(SHELLSTATE,version)
#define SHELLSTATE_SIZE_IE4   FIELD_OFFSET(SHELLSTATE,uNotUsed)
#define SHELLSTATE_SIZE_WIN2K sizeof(SHELLSTATE)

SHSTDAPI_(void) SHGetSetSettings(LPSHELLSTATE lpss, DWORD dwMask, BOOL bSet);

// And here's the beginning portion for public use:
#define SSF_HIDDENFILEEXTS          0x00000004  //
#define SSF_SORTCOLUMNS             0x00000010  //
#define SSF_FILTER                  0x00010000  //
#define SSF_WEBVIEW                 0x00020000  //
#define SSF_SHOWSUPERHIDDEN         0x00040000  //
#define SSF_SEPPROCESS              0x00080000  //

//--------------------------------------------------------------------------
//
// Interface used for exposing the INI file methods on a shortcut file
//
//
//--------------------------------------------------------------------------
#undef  INTERFACE
#define INTERFACE  INamedPropertyBag
DECLARE_INTERFACE_(INamedPropertyBag, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;
    // *** INamedPropertyBag methods ***
    STDMETHOD(ReadPropertyNPB) (THIS_  /* [in] */ LPCOLESTR pszBagname,
                            /* [in] */ LPCOLESTR pszPropName,
                            /* [out][in] */ PROPVARIANT *pVar) PURE;

    STDMETHOD(WritePropertyNPB)(THIS_ /* [in] */ LPCOLESTR pszBagname,
                           /* [in] */ LPCOLESTR pszPropName,
                           /* [in] */ PROPVARIANT  *pVar) PURE;

    STDMETHOD(RemovePropertyNPB)(THIS_ /* [in] */ LPCOLESTR pszBagname,
                          /* [in] */ LPCOLESTR pszPropName) PURE;
};
//-------------------------------------------------------------------------
//
// IShellMenuCallback interface
//
//
// [Member functions]
//
//  TBD
//
//-------------------------------------------------------------------------

#ifndef RC_INVOKED
#include <pshpack8.h>   /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

typedef struct _tagSMDATA
{
    DWORD   dwMask;             // SMDM_* values
    DWORD   dwFlags;            // Not used
    HMENU   hmenu;              // Static HMENU portion.
    HWND    hwnd;               // HWND owning the HMENU
    UINT    uId;                // Id of the item in the menu (-1 for menu itself)
    UINT    uIdParent;          // Id of the item spawning this menu
    UINT    uIdAncestor;        // Id of the very top item in the chain of ShellFolders
    IUnknown*    punk;          // IUnkown of the menuband
    LPITEMIDLIST   pidlFolder;  // pidl of the ShellFolder portion
    LPITEMIDLIST   pidlItem;    // pidl of the item in the ShellFolder portion
    IShellFolder*   psf;        // IShellFolder for the shell folder portion

    void*   pvUserData;         // User defined Data associated with a pane.

} SMDATA, *LPSMDATA;

#ifndef RC_INVOKED
#include <poppack.h>   /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

// Mask
#define SMDM_SHELLFOLDER               0x00000001  // This is for an item in the band
#define SMDM_HMENU                     0x00000002  // This is for the Band itself

// Flags (bitmask)
typedef struct
{
    DWORD   dwMask;     // SMIM_*
    DWORD   dwType;     // SMIT_*
    DWORD   dwFlags;    // SMIF_*
    int     iIcon;
} SMINFO, * PSMINFO;

typedef struct
{
    LONG            lEvent;     // Change notify Event
    LPCITEMIDLIST   pidl1;      // Pidl 1 Passed in from the Change notify
    LPCITEMIDLIST   pidl2;      // Pidl 2 Passed in from the Change notify
} SMCSHCHANGENOTIFYSTRUCT, *PSMCSHCHANGENOTIFYSTRUCT;

// Mask flags
enum
{
    SMIM_TYPE           = 0x00000001,
    SMIM_FLAGS          = 0x00000002,
    SMIM_ICON           = 0x00000004
};

// Types for mbiinfo.dwType
enum
{
    SMIT_SEPARATOR      = 0x00000001,
    SMIT_STRING         = 0x00000002
};

// Flags for mbiinfo.dwFlags
enum
{
    SMIF_ICON           = 0x00000001,       // Show an icon
    SMIF_ACCELERATOR    = 0x00000002,       // Underline the character marked w/ '&'
    SMIF_DROPTARGET     = 0x00000004,       // Item is a drop target
    SMIF_SUBMENU        = 0x00000008,       // Item has a submenu
    SMIF_VOLATILE       = 0x00000010,       // Obsolete.
    SMIF_CHECKED        = 0x00000020,       // Item has a Checkmark
    SMIF_DROPCASCADE    = 0x00000040,       // Item can cascade out during drag/drop
    SMIF_HIDDEN         = 0x00000080,       // Don't display item
    SMIF_DISABLED       = 0x00000100,       // Should be unselectable. Grey.
    SMIF_TRACKPOPUP     = 0x00000200,       // Should be unselectable. Grey.
    SMIF_DEMOTED        = 0x00000400,       // Display item in "Demoted" state.
    SMIF_ALTSTATE       = 0x00000800,       // Displayed in "Altered State"
};

// Messages
//
//  SMC_INITMENU        Sent when a menu is shown.
//
//
//  SMC_GETOBJECT
//    wParam = riid
//    lParam = ppv
//      Sent to get an object that supports riid.
//
#define SMC_INITMENU            0x00000001  // The callback is called to init a menuband
#define SMC_CREATE              0x00000002
#define SMC_EXITMENU            0x00000003  // The callback is called when menu is collapsing
#define SMC_EXEC                0x00000004  // The callback is called to execute an item
#define SMC_GETINFO             0x00000005  // The callback is called to return DWORD values
#define SMC_GETSFINFO           0x00000006  // The callback is called to return DWORD values
#define SMC_GETOBJECT           0x00000007  // The callback is called to get some object
#define SMC_GETSFOBJECT         0x00000008  // The callback is called to get some object
#define SMC_SFEXEC              0x00000009  // The callback is called to execute an shell folder item
#define SMC_SFSELECTITEM        0x0000000A  // The callback is called when an item is selected
#define SMC_SELECTITEM          0x0000000B  // The callback is called when an item is selected
#define SMC_GETSFINFOTIP        0x0000000C  // The callback is called to get some object
#define SMC_GETINFOTIP          0x0000000D  // The callback is called to get some object
#define SMC_INSERTINDEX         0x0000000E  // New item insert index
#define SMC_POPUP               0x0000000F  // InitMenu/InitMenuPopup (sort of)
#define SMC_REFRESH             0x00000010  // Menus have completely refreshed. Reset your state.
#define SMC_DEMOTE              0x00000011  // Demote an item
#define SMC_PROMOTE             0x00000012  // Promote an item
#define SMC_BEGINENUM           0x00000013  // tell callback that we are beginning to ENUM the indicated parent
#define SMC_ENDENUM             0x00000014  // tell callback that we are ending the ENUM of the indicated paren
#define SMC_MAPACCELERATOR      0x00000015  // Called when processing an accelerator.
#define SMC_DEFAULTICON         0x00000016  // Returns Default icon location in wParam, index in lParam
#define SMC_NEWITEM             0x00000017  // Notifies item is not in the order stream.
#define SMC_GETMINPROMOTED      0x00000018  // Returns the minimum number of promoted items
#define SMC_CHEVRONEXPAND       0x00000019  // Notifies of a expansion via the chevron
#define SMC_DISPLAYCHEVRONTIP   0x0000002A  // S_OK display, S_FALSE not.
#define SMC_DESTROY             0x0000002B  // Called when a pane is being destroyed.
#define SMC_SETOBJECT           0x0000002C  // Called to save the passed object
#define SMC_SETSFOBJECT         0x0000002D  // Called to save the passed object
#define SMC_SHCHANGENOTIFY      0x0000002E	// Called when a Change notify is received. lParam points to SMCSHCHANGENOTIFYSTRUCT
#define SMC_CHEVRONGETTIP       0x0000002F  // Called to get the chevron tip text. wParam = Tip title, Lparam = TipText Both MAX_PATH
#define SMC_SFDDRESTRICTED      0x00000030  // Called requesting if it's ok to drop. wParam = IDropTarget.
#define SMC_FILTERPIDL          0x10000000  // The callback is called to see if an item is visible
#define SMC_CALLBACKMASK        0xF0000000  // Mask of comutationally intense messages


#undef  INTERFACE
#define INTERFACE   IShellMenuCallback

DECLARE_INTERFACE_(IShellMenuCallback, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS)  PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellMenuCallback methods ***
    STDMETHOD(CallbackSM)         (THIS_ LPSMDATA psmd, UINT uMsg, WPARAM wParam, LPARAM lParam) PURE;
};

//-------------------------------------------------------------------------
//
// IMenuBand interface
//
//   This interface provides methods the menuband (CLSID_MenuBand)
//  to receive pertinent messages.
//
// [Member functions]
//
// IMenuBand::IsMenuMessage(pmsg)
//   A message pump calls this function to see if any messages need
//   to be redirected to this object.  If this returns S_OK, the
//   message loop should not call TranslateMessage or DispatchMessage.
//   If this returns E_FAIL, the menu has exited menu mode and is ready
//   to be destroyed.
//
// IMenuBand::TranslateMenuMessage(pmsg, plRet)
//   Offers the object an opportunity to translate messages.  The
//   parent window proc must call this method for every message (not
//   the message pump).  The message, wParam, and lParam should be
//   delivered to this method in *pmsg.  This method may change the
//   values of pmsg->wParam or pmsg->lParam, in which case these changes
//   should be forwarded on.
//
//   This method is required because some modal message pumps (like the one
//   in TrackPopupMenu) do not give an opportunity to call a custom
//   TranslateAccelerator method like IInputObject::TranslateAcceleratorIO.
//
//   TranslateMenuMessage returns S_OK if the message was handled and
//   should be eaten.  *plRet is not touched if this returns S_FALSE.
//
//
//-------------------------------------------------------------------------

// CmdIDs for the IOleCommandTarget Group: CGID_MenuBandHandler (defined in shguidp.h)
#define MBHANDCID_PIDLSELECT    0           // A PIDL from a menuband was selected

#undef  INTERFACE
#define INTERFACE   IMenuBand

DECLARE_INTERFACE_(IMenuBand, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS)  PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IMenuBand methods ***
    STDMETHOD(IsMenuMessage)    (THIS_ MSG * pmsg) PURE;
    STDMETHOD(TranslateMenuMessage) (THIS_ MSG * pmsg, LRESULT * plRet) PURE;
};



//-------------------------------------------------------------------------
//
// IShellMenu interface
//
//-------------------------------------------------------------------------

#define SMINIT_DEFAULT              0x00000000  // No Options
#define SMINIT_RESTRICT_CONTEXTMENU 0x00000001  // Don't allow Context Menus
#define SMINIT_RESTRICT_DRAGDROP    0x00000002  // Don't allow Drag and Drop
#define SMINIT_TOPLEVEL             0x00000004  // This is the top band.
#define SMINIT_DEFAULTTOTRACKPOPUP  0x00000008  // When no callback is specified,
                                                // default all HMENU items to Trackpopup
#define SMINIT_CACHED               0x00000010
#define SMINIT_USEMESSAGEFILTER     0x00000020
#define SMINIT_LEGACYMENU           0x00000040  // Old Menu behaviour.

// Internal Menuband setting
#define SMINIT_NOSETSITE            0x00010000  // Internal setting

// These are mutually Exclusive
#define SMINIT_VERTICAL             0x10000000  // This is a vertical menu
#define SMINIT_HORIZONTAL           0x20000000  // This is a horizontal menu    (does not inherit)
#define SMINIT_MULTICOLUMN          0x40000000  // this is a multi column menu

#define ANCESTORDEFAULT      (UINT)-1

#define SMSET_TOP                   0x10000000    // Bias this namespace to the top of the menu
#define SMSET_BOTTOM                0x20000000    // Bias this namespace to the bottom of the menu
#define SMSET_DONTOWN               0x00000001    // The Menuband doesn't own the non-ref counted object
                                                  // and should not destroy it.
#define SMSET_MERGE                 0x00000002
#define SMSET_NOEMPTY               0x00000004   // Dont show (Empty) on shell folder
#define SMSET_USEBKICONEXTRACTION   0x00000008   // Use the background icon extractor
#define SMSET_HASEXPANDABLEFOLDERS  0x00000010   // Need to call SHIsExpandableFolder
#define SMSET_DONTREGISTERCHANGENOTIFY 0x00000020 // ShellFolder is a discontiguous child of a parent shell folder
#define SMSET_COLLAPSEONEMPTY       0x00000040   // When Empty, causes a menus to collapse

#define SMINV_REFRESH        0x00000001
#define SMINV_ICON           0x00000002
#define SMINV_POSITION       0x00000004
#define SMINV_ID             0x00000008
#define SMINV_NEXTSHOW       0x00000010       // Does Invalidates on next show.
#define SMINV_PROMOTE        0x00000020       // Does Invalidates on next show.
#define SMINV_DEMOTE         0x00000040       // Does Invalidates on next show.
#define SMINV_FORCE          0x00000080
#define SMINV_NOCALLBACK     0x00000100       // Invalidates, but does not call the callback.

#undef  INTERFACE
#define INTERFACE   IShellMenu

DECLARE_INTERFACE_(IShellMenu, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS)  PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellMenu methods ***
    STDMETHOD(Initialize)(THIS_ IShellMenuCallback* psmc, UINT uId, UINT uIdAncestor, DWORD dwFlags) PURE;
    STDMETHOD(GetMenuInfo)(THIS_ IShellMenuCallback** ppsmc, UINT* puId, UINT* puIdAncestor, DWORD* pdwFlags) PURE;
    STDMETHOD(SetShellFolder)(THIS_ IShellFolder* psf, LPCITEMIDLIST pidlFolder, HKEY hKey, DWORD dwFlags) PURE;
    STDMETHOD(GetShellFolder)(THIS_ DWORD* pdwFlags, LPITEMIDLIST* ppidl, REFIID riid, void** ppv) PURE;
    STDMETHOD(SetMenu)(THIS_ HMENU hmenu, HWND hwnd, DWORD dwFlags) PURE;
    STDMETHOD(GetMenu)(THIS_ HMENU* phmenu, HWND* phwnd, DWORD* pdwFlags) PURE;
    STDMETHOD(InvalidateItem)(THIS_ LPSMDATA psmd, DWORD dwFlags) PURE;
    STDMETHOD(GetState)(THIS_ LPSMDATA psmd) PURE;
    STDMETHOD(SetMenuToolbar)(THIS_ IUnknown* punk, DWORD dwFlags) PURE;
};



#undef  INTERFACE
#define INTERFACE   ITrackShellMenu

DECLARE_INTERFACE_(ITrackShellMenu, IShellMenu)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS)  PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IShellMenu methods ***
    STDMETHOD(Initialize)(THIS_ IShellMenuCallback* psmc, UINT uId, UINT uIdAncestor, DWORD dwFlags) PURE;
    STDMETHOD(GetMenuInfo)(THIS_ IShellMenuCallback** ppsmc, UINT* puId, UINT* puIdAncestor, DWORD* pdwFlags) PURE;
    STDMETHOD(SetShellFolder)(THIS_ IShellFolder* psf, LPCITEMIDLIST pidlFolder, HKEY hKey, DWORD dwFlags) PURE;
    STDMETHOD(GetShellFolder)(THIS_ DWORD* pdwFlags, LPITEMIDLIST* ppidl, REFIID riid, void** ppv) PURE;
    STDMETHOD(SetMenu)(THIS_ HMENU hmenu, HWND hwnd, DWORD dwFlags) PURE;
    STDMETHOD(GetMenu)(THIS_ HMENU* phmenu, HWND* phwnd, DWORD* pdwFlags) PURE;
    STDMETHOD(InvalidateItem)(THIS_ LPSMDATA psmd, DWORD dwFlags) PURE;
    STDMETHOD(GetState)(THIS_ LPSMDATA psmd) PURE;
    STDMETHOD(SetMenuToolbar)(THIS_ IUnknown* punk, DWORD dwFlags) PURE;

    // *** ITrackShellMenu methods ***
    STDMETHOD(SetObscured)(THIS_ HWND hwndTB, IUnknown* punkBand, DWORD dwSMSetFlags) PURE;
    STDMETHOD(Popup)(THIS_ HWND hwnd, POINTL *ppt, RECTL *prcExclude, DWORD dwFlags) PURE;
};

//-------------------------------------------------------------------------
//
// IMenuPopup interface
//
//   This interface provides methods to navigate thru a menu.
//
// [Member functions]
//
// IMenuPopup::Popup(ppt, prcExclude, dwFlags)
//   Invoke the menu, located at the point *ppt (in screen coordinates).
//   The optional prcExclude points to the rectangle to exclude when
//   positioning the menu, otherwise it should be NULL.  dwFlags may be:
//
//      MDBPU_SETFOCUS: the menu can take the focus.
//
//   Returns S_OK if the object implements the popup menu as a modeless
//   menu.  Otherwise it returns S_FALSE, and the menu is finished.
//
// IMenuPopup::OnSelect(dwSelectType)
//   This method handles selection notifications.
//
// IMenuPopup::SetSubMenu(pmp, fSet)
//   Sets the given menu bar interface to be the submenu of this
//   object's interface.  Set fSet == FALSE to remove the submenu.
//
//-------------------------------------------------------------------------

// Type values for IMenuPopup::OnSelect
enum
{
    MPOS_EXECUTE = 0,           // Execute the selected menu item
    MPOS_FULLCANCEL,            // Cancel the entire menu
    MPOS_CANCELLEVEL,           // Cancel the current cascaded menu
    MPOS_SELECTLEFT,            // select one to the left of the cur selection
    MPOS_SELECTRIGHT,           // select one to the right of the cur selection
    MPOS_CHILDTRACKING          // the child got a tracking select (mouse moved over)
};

// Flags for IMenuPopup::Popup
enum
{
    MPPF_SETFOCUS       = 0x00000001,    // Menu can take the focus
    MPPF_INITIALSELECT  = 0x00000002,    // Select the first item
    MPPF_NOANIMATE      = 0x00000004,    // Do not animate this show
    MPPF_KEYBOARD       = 0x00000010,    // The menu is activated by keyboard
    MPPF_REPOSITION     = 0x00000020,    // Resposition the displayed bar.
    MPPF_FORCEZORDER    = 0x00000040,    // internal: Tells menubar to ignore Submenu positions
    MPPF_FINALSELECT    = 0x00000080,    // Select the last item
    MPPF_TOP            = 0x20000000,    // Popup menu up from point
    MPPF_LEFT           = 0x40000000,    // Popup menu left from point
    MPPF_RIGHT          = 0x60000000,    // Popup menu right from point
    MPPF_BOTTOM         = 0x80000000,    // Popup menu below point
    MPPF_POS_MASK       = 0xE0000000     // Menu Position Mask
};

#if (_WIN32_IE) >= 0x0400
#undef  INTERFACE
#define INTERFACE   IMenuPopup

DECLARE_INTERFACE_(IMenuPopup, IDeskBar)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS)  PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow)        (THIS_ HWND * lphwnd) PURE;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

    // *** IDeskBarClient methods ***
    STDMETHOD(SetClient)        (THIS_ IUnknown* punkClient) PURE;
    STDMETHOD(GetClient)        (THIS_ IUnknown** ppunkClient) PURE;
    STDMETHOD(OnPosRectChangeDB)(THIS_ LPRECT prc) PURE;

    // *** IMenuPopup methods ***
    STDMETHOD(Popup)            (THIS_ POINTL *ppt, RECTL *prcExclude, DWORD dwFlags) PURE;
    STDMETHOD(OnSelect)         (THIS_ DWORD dwSelectType) PURE;
    STDMETHOD(SetSubMenu)       (THIS_ IMenuPopup* pmp, BOOL fSet) PURE;
};
#endif


//
// Net Access Wizard - called from NetID and WinLogon.
//

#define NAW_NETID              0x00          // called from netid tab
#define NAW_PSWORKGROUP        0x01          // post setup (joined workgroup)
#define NAW_PSDOMAINJOINED     0x02          // post setup (joined domain)
#define NAW_PSDOMAINJOINFAILED 0x03          // post setup (domain joined failed)

STDAPI NetAccessWizard(HWND hwnd, UINT uType, BOOL *pfReboot);
typedef HRESULT (CALLBACK *LPNETACCESSWIZARD)(HWND hwnd, UINT uType, BOOL *pfReboot);

STDAPI ClearAutoLogon(VOID);

//
// Net Places API's called from mpr.
//

typedef enum
{
    NETPLACES_WIZARD_MAPDRIVE = 0,
    NETPLACES_WIZARD_ADDPLACE,
} NETPLACESWIZARDTYPE;

STDAPI_(DWORD)
NetPlacesWizardDoModal(
    LPCONNECTDLGSTRUCTW lpConnDlgStruct,
    NETPLACESWIZARDTYPE npwt,
    BOOL                fIsRoPath
    );

// Definition for the exported function types (for GetProcAddress)
typedef DWORD (STDAPICALLTYPE*NetPlacesWizardDoModal_t)(LPCONNECTDLGSTRUCTW lpConnDlgStruct, NETPLACESWIZARDTYPE npwt, BOOL fIsROPath);

//
// moved here from nettarg.h (shell\inc)
//

// nCmdID
#define CFCID_SETENUMTYPE       0

//nCmdExecOpt
#define CFCOPT_ENUMINCOMING     1
#define CFCOPT_ENUMOUTGOING     2
#define CFCOPT_ENUMALL          3


/****************************************************
 Items to display in the shutdown dialog (dwItems)
 -------------------------------------------------

  These flags can be |'d when passed in. A single
  value will be returned, with NONE indicating an
  error or the user clicked cancel.
  -dsheldon
****************************************************/
#define SHTDN_NONE                      0x000000000
#define SHTDN_LOGOFF                    0x000000001
#define SHTDN_SHUTDOWN                  0x000000002
#define SHTDN_RESTART                   0x000000004
#define SHTDN_RESTART_DOS               0x000000008
#define SHTDN_SLEEP                     0x000000010
#define SHTDN_SLEEP2                    0x000000020
#define SHTDN_HIBERNATE                 0x000000040
#define SHTDN_DISCONNECT                0x000000080

// Shutdown dialog function exported from MSGINA.dll

STDAPI_(DWORD) ShellShutdownDialog(HWND hwndParent, LPCTSTR szUsername, DWORD dwExcludeItems);
typedef DWORD (STDAPICALLTYPE*PFNSHELLSHUTDOWNDIALOG)(HWND hwndParent, LPCTSTR szUsername, DWORD dwExcludeItems);


// Define a clipboard format for a data object to pass a user's SID
// A data object supporting this format is passed to IShellExtInit::Initialize
// before calling IShellPropSheetExt::AddPages.
#define CFSTR_USERPROPPAGESSID TEXT("UserPropertyPagesSid")


// Registry paths where extra IShellPropSheetExt handlers can be put so that
// extended pages are added to the CPL or User Properties
#define REGSTR_USERSANDPASSWORDS_CPL \
/* HKLM\\ */ TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\Users and Passwords")
// CLSIDs go in HKLM\Software\Microsoft\Windows\CurrentVersion\Control Panel\Users and Passwords\shellex\PropertySheetHandlers

#define REGSTR_USERPROPERTIES_SHEET \
/* HKLM\\ */ TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\Users and Passwords\\User Properties")
// CLSIDs go in HKLM\Software\Microsoft\Windows\CurrentVersion\Control Panel\Users and Passwords\User Properties\shellex\PropertySheetHandlers

//  SHStgOpenStorage()
//  Wrap of StgOpenStorage for binding to file-based storage.
//
//  pszPath,     // storage path
//  grfMode,     // STGM_ flags.  Note: if requesting a stream or property set, use STGM_DIRECT|STGM_SHARE_EXCLUSIVE.
//  grfAttr,     // reserved for future use.
//  grfFileAttr, // win32 file attributes, if available; otherwise zero.
//  riid,        // interface requested
//  ppvObj       // receives address of storage object
//
STDAPI SHStgOpenStorageA(IN LPCSTR pszPath, IN OPTIONAL DWORD grfMode, IN OPTIONAL DWORD grfAttr, IN OPTIONAL DWORD grfFileAttr, REFIID riid, OUT void ** ppvObj);
//  riid,        // interface requested
//  ppvObj       // receives address of storage object
//
STDAPI SHStgOpenStorageW(IN LPCWSTR pszPath, IN OPTIONAL DWORD grfMode, IN OPTIONAL DWORD grfAttr, IN OPTIONAL DWORD grfFileAttr, REFIID riid, OUT void ** ppvObj);
#ifdef UNICODE
#define SHStgOpenStorage  SHStgOpenStorageW
#else
#define SHStgOpenStorage  SHStgOpenStorageA
#endif // !UNICODE


//  SHPropStgCreate()
//  Wrap of IPropertySetStorage::Open/Create
//
//  This function ensures proper handling of code page retrieval/assignment
//  for the requested property set operation.
//
//  psstg,          //  Address of IPropertySetStorage vtable
//  fmtid,          //  property set ID
//  pclsid,         //  class ID associated with the set. This can be NULL
//  grfFlags,       //  PROPSETFLAG_xxx.  All sets containing ansi bytes should be created with
                    //  PROPSETFLAG_ANSI, otherwise PROPSETFLAG_DEFAULT.
//  grfMode,        //  STGM_ flags.  Must contain STGM_DIRECT|STGM_EXCLUSIVE.
//  dwDisposition,  //  OPEN_EXISTING. OPEN_ALWAYS, CREATE_NEW, or CREATE_ALWAYS
//  IPropertyStorage** ppstg,  // Address to receive requested vtable
//  puCodePage      //  Optional address to receive the code page ID for the set.
//
STDAPI SHPropStgCreate( IPropertySetStorage* psstg, REFFMTID fmtid, CLSID* pclsid, DWORD grfFlags, DWORD grfMode, DWORD dwDisposition, OUT IPropertyStorage** ppstg, OUT OPTIONAL UINT* puCodePage );


//  SHPropStgReadMultiple()
//  IPropertyStorage::ReadMultiple wrap
//
//  The wrap ensures ANSI/UNICODE translations are handled properly for
//  legacy property sets.
//
//  pps,       // address of IPropertyStorage vtable.
//  uCodePage, //Code page value retrieved from SHCreatePropertySet
//  cpspec,    //Count of properties being read
//  rgpspec,   //Array of the properties to be read
//  rgvar      //Array of PROPVARIANTs containing the property values on return
//
STDAPI SHPropStgReadMultiple( IPropertyStorage* pps, UINT uCodePage, ULONG cpspec, PROPSPEC const rgpspec[], PROPVARIANT rgvar[] );


//  SHPropStgWriteMultiple()
//  IPropertyStorage::WriteMultiple wrap
//
//  The wrap ensures ANSI/UNICODE translations are handled properly for
//  legacy property sets.
//
//  pps,        // address of IPropertyStorage vtable.
//  puCodePage, // (in, out) address of code page value retrieved from SHCreatePropertySet.
//  cpspec,     // The number of properties being set
//  rgpspec,    // Property specifiers
//  rgvar,      // Array of PROPVARIANT values
//  propidNameFirst // Minimum value for property identifiers. This value should be >= PID_FIRST_USABLE
//
STDAPI SHPropStgWriteMultiple( IPropertyStorage* pps, UINT* puCodePage, ULONG cpspec, PROPSPEC const rgpspec[], PROPVARIANT rgvar[], PROPID propidNameFirst );

//  SHIsLegacyAnsiProperty()
//
//  Determine whether the property is a legacy ANSI property, and if so,
//  compute a conversion type for the property.
//
//  fmtid,     // property set identifier
//  propid,    // property identifier
//  pvt,       // optional:  if non-null, contains on input the VARTYPE for the property; on output,
               //            the converted type.  (e.g., VT_LPSTR <--> VT_LPWSTR).

STDAPI_(BOOL) SHIsLegacyAnsiProperty( REFFMTID fmtid, PROPID propid, IN OUT OPTIONAL VARTYPE* pvt );



//-------------------------------------------------------------------------//
//  linkwnd.h - declaration of LinkWindow control
//
//  LinkWindow supports HTML-like embedded links in the caption text.
//  (e.g. "<a>Click Here</a> to see something cool"
//
//  An unlimited number of embedded links are supported.   When the user clicks
//  on a link, an WM_NOTIFY - LWN_CLICK notification message is sent to the
//  parent window.   The message data includes the zero-based index
//  (left to right orientation) of the link that was clicked.
//
//  (BUGBUG: need to add link-identifier support to accommodate right-to-left
//  captions, <a id:Foo>Click Here</a> ).
//
//  scotthan: author/owner
//  dsheldon: moved this to shlobjp.h. Eventually destined for comctl32.

EXTERN_C BOOL WINAPI LinkWindow_RegisterClass() ;
EXTERN_C BOOL WINAPI LinkWindow_UnregisterClass( HINSTANCE ) ;

#define INVALID_LINK_INDEX  (-1)
#define MAX_LINKID_TEXT     48
#define LINKWINDOW_CLASS    TEXT("Link Window")

#define LWIF_ITEMINDEX  0x00000001
#define LWIF_STATE      0x00000002
#define LWIF_ITEMID     0x00000004
#define LWIF_URL        0x00000008

#define LWIS_FOCUSED    0x0001
#define LWIS_ENABLED    0x0002
#define LWIS_VISITED    0x0004

//  BUGBUG: we've got to change the following definition to
//  wininet.h::INTERNET_MAX_URL_LENGTH.
//  Currently, this breaks various dependents.
#define LW_MAX_URL_LENGTH   (2048 + 32 + sizeof("://"))

//  LWITEM
typedef struct tagLWITEMA {
    UINT        mask ;
    int         iLink ;
    UINT        state ;
    UINT        stateMask ;
    CHAR        szID[MAX_LINKID_TEXT] ;
    CHAR        szUrl[LW_MAX_URL_LENGTH] ;   //bugbug: INTERNET_MAX_URL_LENGTH
} LWITEMA, FAR* LPLWITEMA ;

typedef struct tagLWITEMW {
    UINT        mask ;
    int         iLink ;
    UINT        state ;
    UINT        stateMask ;
    WCHAR       szID[MAX_LINKID_TEXT] ;
    WCHAR       szUrl[LW_MAX_URL_LENGTH] ;  //bugbug: INTERNET_MAX_URL_LENGTH
} LWITEMW, FAR* LPLWITEMW ;

//  LWHITTESTINFO
typedef struct tagLWHITTESTINFOA {
    POINT       pt ;
    LWITEMA     item ;
} LWHITTESTINFOA, FAR* LPLWHITTESTINFOA ;

//  LWHITTESTINFO
typedef struct tagLWHITTESTINFOW {
    POINT       pt ;
    LWITEMW     item ;
} LWHITTESTINFOW, FAR* LPLWHITTESTINFOW ;

//  NMLINKWND
typedef struct tagNMLINKWNDA {
    NMHDR       hdr;
    LWITEMA     item ;
} NMLINKWNDA, FAR *LPNMLINKWNDA;

typedef struct tagNMLINKWNDW {
    NMHDR       hdr;
    LWITEMW     item ;
} NMLINKWNDW, FAR *LPNMLINKWNDW;

#ifdef UNICODE
#define LWITEM          LWITEMW
#define LPLWITEM        LPLWITEMW
#define LWHITTESTINFO   LWHITTESTINFOW
#define LPLWHITTESTINFO LPLWHITTESTINFOW
#define NMLINKWND       NMLINKWNDW
#define LPNMLINKWND     LPNMLINKWNDW
#else //UNICODE
#define LWITEM          LWITEMA
#define LPLWITEM        LPLWITEMA
#define LWHITTESTINFO   LWHITTESTINFOA
#define LPLWHITTESTINFO LPLWHITTESTINFOA
#define NMLINKWND       NMLINKWNDA
#define LPNMLINKWND     LPNMLINKWNDA
#endif //UNICODE

//  LinkWindow notifications
//  NM_CLICK   // wParam: n/a, lParam: LPLWITEM, ret: ignored.

//  LinkWindow messages
#define LWM_HITTEST         (WM_USER+0x300)  // wParam: n/a, lparam: LPLWHITTESTINFO, ret: BOOL
#define LWM_GETIDEALHEIGHT  (WM_USER+0x301)  // wParam: n/a, lparam: n/a, ret: cy
#define LWM_SETITEM        (WM_USER+0x302)  // wParam: n/a, lparam: LWITEM*, ret: BOOL
#define LWM_GETITEM        (WM_USER+0x303)  // wParam: n/a, lparam: LWITEM*, ret: BOOL
//-------------------------------------------------------------------------//

//-------------------------------------------------------------------------//
//  grpbtn.h - declaration of GroupButton control
//
//  GroupButton is a variation of a group box control but supports a link-like
//  caption bar, and optionally, can be buddied to a containee window for
//  auto-positioning behavior.
//
//  scotthan: author/owner
//            (Eventually destined for comctl32.)

#define GROUPBUTTON_CLASS  TEXT("GroupButton")

EXTERN_C ATOM WINAPI GroupButton_RegisterClass() ;
EXTERN_C BOOL WINAPI GroupButton_UnregisterClass() ;

typedef struct tagGBPLACEMENT
{
    LONG x ;            // in: top left; -1 to ignore
    LONG y ;            // in: top right; -1 to ignore
    LONG cx ;           // in: width to assign; -1 to ignore.
                        //     If cx != -1, cxContain and cxMax are ignored.
    LONG cy ;           // in: height to assign; -1 to ignore.
                        //     If cy != -1, cyContain and cyMax are ignored.
    LONG cxBuddy ;      // in: width of buddy area; -1 to ignore
    LONG cyBuddy ;      // in: height of buddy area; -1 to ignore
    LONG cxBuddyMargin ;// in: width of buddy margin; -1 to ignore
    LONG cyBuddyMargin ;// in: height of buddy margin; -1 to ignore
    RECT rcBuddy ;      // out: new buddy rect in parent coords.
    RECT rcWindow ;     // out: new window rect in parent coords.
    HDWP hdwp ;         // optional in: Non-NULL => DeferWindowPos,
                        //              NULL => SetWindowPos
} GBPLACEMENT, *PGBPLACEMENT, *LPGBPLACEMENT ;

//  Buddy flags
#define GBBF_HRESIZE   0x00000001
#define GBBF_VRESIZE   0x00000002
#define GBBF_HSCROLL   0x00000004
#define GBBF_VSCROLL   0x00000008
#define GBBF_HSLAVE    0x00000010
#define GBBF_VSLAVE    0x00000020

//  Group button messages
#define GBM_FIRST        (WM_USER+0x400) // arbitrary
#define GBM_SETPLACEMENT (GBM_FIRST+0)   // WPARAM: n/a, LPARAM: PGBPLACEMENT, return: BOOL
#define GBM_SETBUDDY     (GBM_FIRST+1)   // WPARAM: HWND hwndBuddy, LPARAM: dwBuddyFlags, return: BOOL
#define GBM_GETBUDDY     (GBM_FIRST+2)   // WPARAM: n/a, LPARAM: n/a, return: HWND
#define GBM_SETDROPSTATE (GBM_FIRST+3)   // WPARAM: BOOL fDropped, LPARAM: n/a, return: BOOL
#define GBM_GETDROPSTATE (GBM_FIRST+4)   // WPARAM: n/a, LPARAM: n/a, return: BOOL fDropped
#define GBM_LAST         GBM_GETDROPSTATE

typedef struct tagGBNQUERYBUDDYSIZE
{
    NMHDR   hdr;
    LONG    cx ;
    LONG    cy ;
} GBNQUERYBUDDYSIZE, *LPGBNQUERYBUDDYSIZE, *PGBNQUERYBUDDYSIZE ;

//  Group button notifications
#define GBN_FIRST               2000U
#define GBN_LAST                2020U
#define GBN_QUERYBUDDYHEIGHT    (GBN_FIRST+0)   // LPARAM: LPGBNQUERYBUDDYSIZE
#define GBN_QUERYBUDDYWIDTH     (GBN_FIRST+1)   // LPARAM: LPGBNQUERYBUDDYSIZE.

//-------------------------------------------------------------------------//
#ifdef __cplusplus
}

#endif  /* __cplusplus */

#ifndef RC_INVOKED
#include <poppack.h>
#endif  /* !RC_INVOKED */
#endif // _SHLOBJP_H_
