/*****************************************************************************\
    FILE: xboxfldr.h

    DESCRIPTION:
        This class inherits from CBaseFolder for a base ShellFolder implementation
    of IShellFolder and overrides methods to give Xbox Specific features.
\*****************************************************************************/

#ifndef _XBOXFOLDER_H
#define _XBOXFOLDER_H

#include "isf.h"
#include "xboxdir.h"
#include "xboxsite.h"
#include "xboxlist.h"
#include "xboxglob.h"
#include "ftppidl.h"
#include "xboxpidl.h"
#include "cowsite.h"
#include "util.h"

class CXboxDir;
class CXboxSite;


/*****************************************************************************\
     CXboxFolder
 
     The stuff that tracks the state of a folder.
 
     The cBusy field tracks how many sub-objects have been created
     (e.g., IEnumIDList) which still contain references to this
     folder's identity.  You cannot change the folder's identity
     (via IPersistFolder::Initialize) while there are outstanding
     subobjects.
 
     The number of cBusy's never exceeds the number of cRef's, because
     each subobject that requires the folder identity must retain a
     reference to the folder itself.  That way, the folder won't be
     Release()d while the identity is still needed.
\*****************************************************************************/

class CXboxFolder  : public CBaseFolder
                   , public IShellIcon
                   , public IShellIconOverlay
                   , public IDelegateFolder
                   , public IShellPropSheetExt
                   , public IBrowserFrameOptions
{
public:
    //////////////////////////////////////////////////////
    // Public Interfaces
    //////////////////////////////////////////////////////
    
    // *** IUnknown ***
    virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
    virtual STDMETHODIMP_(ULONG) AddRef(void) {return CBaseFolder::AddRef();};
    virtual STDMETHODIMP_(ULONG) Release(void) {return CBaseFolder::Release();};
    
    // *** IShellFolder ***
    virtual STDMETHODIMP ParseDisplayName(HWND hwndOwner, LPBC pbcReserved, LPOLESTR lpszDisplayName,
                                            ULONG * pchEaten, LPITEMIDLIST * ppidl, ULONG *pdwAttributes);
    virtual STDMETHODIMP EnumObjects(HWND hwndOwner, DWORD grfFlags, LPENUMIDLIST * ppenumIDList);
    virtual STDMETHODIMP BindToObject(LPCITEMIDLIST pidl, LPBC pbcReserved, REFIID riid, LPVOID * ppvOut);
    virtual STDMETHODIMP BindToStorage(LPCITEMIDLIST pidl, LPBC pbcReserved, REFIID riid, LPVOID * ppvObj);
    virtual STDMETHODIMP CompareIDs(LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
    virtual STDMETHODIMP CreateViewObject(HWND hwndOwner, REFIID riid, LPVOID * ppvOut);
    virtual STDMETHODIMP GetAttributesOf(UINT cidl, LPCITEMIDLIST * apidl, ULONG * rgfInOut);
    virtual STDMETHODIMP GetUIObjectOf(HWND hwndOwner, UINT cidl, LPCITEMIDLIST * apidl, REFIID riid, UINT * prgfInOut, LPVOID * ppvOut);
    virtual STDMETHODIMP GetDisplayNameOf(LPCITEMIDLIST pidl, DWORD uFlags, LPSTRRET lpName);
    virtual STDMETHODIMP SetNameOf(HWND hwndOwner, LPCITEMIDLIST pidl, LPCOLESTR lpszName, DWORD uFlags, LPITEMIDLIST * ppidlOut);
    
    // *** IShellFolder2 ***
    virtual STDMETHODIMP GetDetailsEx(LPCITEMIDLIST pidl, const SHCOLUMNID *pscid, VARIANT *pv);

    // *** IPersistFolder ***
    virtual STDMETHODIMP Initialize(LPCITEMIDLIST pidl);        // Override default behavior

    // *** IShellIcon ***
    virtual STDMETHODIMP GetIconOf(LPCITEMIDLIST pidl, UINT flags, LPINT lpIconIndex);

    // *** IShellIconOverlay ***
    virtual STDMETHODIMP GetOverlayIndex(LPCITEMIDLIST pidl, int * pIndex) {return GetOverlayIndexHelper(pidl, pIndex, SIOM_OVERLAYINDEX);};
    virtual STDMETHODIMP GetOverlayIconIndex(LPCITEMIDLIST pidl, int * pIconIndex) {return GetOverlayIndexHelper(pidl, pIconIndex, SIOM_ICONINDEX);};

    // *** IDelegateFolder ***
    virtual STDMETHODIMP SetItemAlloc(IMalloc *pm);

    // *** IShellPropSheetExt ***
    virtual STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE pfnAddPage, LPARAM lParam) {return AddXBOXPropertyPages(pfnAddPage, lParam, &m_hinstInetCpl, _punkSite);};
    virtual STDMETHODIMP ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam) {return E_NOTIMPL;};

    // *** IBrowserFrameOptions ***
    virtual STDMETHODIMP GetFrameOptions(IN BROWSERFRAMEOPTIONS dwMask, OUT BROWSERFRAMEOPTIONS * pdwOptions);


public:
    CXboxFolder();
    ~CXboxFolder(void);

    // Public Member Functions
    void InvalidateCache(void);
    CXboxDir * GetXboxDir(void);
    CXboxDir * GetXboxDirFromPidl(LPCITEMIDLIST pidl);
    CXboxDir * GetXboxDirFromUrl(LPCTSTR pszUrl);

    BOOL IsRoot(void) { return ILIsEmpty(GetPrivatePidlReference()); };
    BOOL IsUTF8Supported(void);
    BOOL _IsValidPidlParameter(LPCITEMIDLIST pidl);
    HRESULT GetItemAllocator(IMalloc **ppm);
    IMalloc * GetItemAllocatorDirect(void) {ASSERT(m_pm); return m_pm;};
    HRESULT GetUIObjectOfHfpl(HWND hwndOwner, CXboxPidlList * pflHfpl, REFIID riid, LPVOID * ppvObj, BOOL fFromCreateViewObject);
    HRESULT CreateSubViewObject(HWND hwndOwner, CXboxPidlList * pflHfpl, REFIID riid, LPVOID * ppvObj);
    HRESULT _PidlToMoniker(LPCITEMIDLIST pidl, IMoniker ** ppmk);
    HRESULT _CreateShellView(HWND hwndOwner, void ** ppvObj);
    CXboxGlob * GetSiteMotd(void);
    CWireEncoding * GetCWireEncoding(void);
    HRESULT _InitXboxSite(void);
    IMalloc * GetIMalloc(void);
    HRESULT _Initialize(LPCITEMIDLIST pidlTarget, LPCITEMIDLIST pidlRoot, int nBytesToPrivate);
    HRESULT _BindToObject_OriginalXboxSupport(LPCITEMIDLIST pidl, REFIID riid, LPVOID * ppvObj);
    HRESULT _FilterBadInput(LPCTSTR pszUrl, LPITEMIDLIST * ppidl);
    HRESULT _BindToObject(LPCITEMIDLIST pidl, LPCITEMIDLIST pidlFull, IBindCtx * pbc, REFIID riid, LPVOID * ppvObj);
//    HRESULT AddToUrlHistory(LPCTSTR pszUrl);
    HRESULT AddToUrlHistory(LPCITEMIDLIST pidl);
    HRESULT AddToUrlHistory(void) {return AddToUrlHistory(this->GetPrivatePidlReference());};
    HRESULT GetOverlayIndexHelper(LPCITEMIDLIST pidl, int * pIndex, DWORD dwFlags);
    HRESULT _GetUIObjectOf(HWND hwndOwner, UINT cidl, LPCITEMIDLIST * apidl, REFIID riid, UINT * prgfInOut, LPVOID * ppvOut, BOOL fFromCreateViewObject);

    static HRESULT FolderCompareIDs(LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
    {
        return XboxItemID_CompareIDs(lParam, pidl1, pidl2, FCMP_GROUPDIRS);
    };

    // Public Member Variables
    CXboxSite *             m_pfs;
    IMalloc *               m_pm;           // today's itemid allocator
    IUrlHistoryStg *        m_puhs;         // Used to add to the history list.
    HINSTANCE               m_hinstInetCpl; // HANDLE to Internet Control panel for View.Options.
    IShellIconOverlayManager * m_psiom;     // Used to get default icon overlays like shortcut cue.

    // Friend Functions
    friend HRESULT CXboxFolder_Create(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, int ib, REFIID riid, LPVOID * ppvObj);
    friend HRESULT CXboxFolder_Create(REFIID riid, LPVOID * ppvObj);

protected:
    HRESULT _CreateViewObject(HWND hwndOwner, REFIID riid, LPVOID * ppvObj);
    HRESULT _AddToUrlHistory(LPCWSTR pwzUrl);
    HRESULT _GetCachedPidlFromDisplayName(LPCTSTR pszDisplayName, LPITEMIDLIST * ppidl);
    HRESULT _ForPopulateAndEnum(CXboxDir * pfd, LPCITEMIDLIST pidlBaseDir, LPCTSTR pszUrl, LPCWIRESTR pwLastDir, LPITEMIDLIST * ppidl);
    HRESULT _InitLegacyShellFolder(IShellFolder * psfLegacy, LPCITEMIDLIST pidlInit);
	HRESULT _GetBindCtx(IBindCtx ** ppbc);
private:
    
    CXboxPidl *m_pidlRoot; //The absolute PIDL to Xbox root, we don't know or care how to parse it.
    CXboxPidl *m_pidl;     //The PIDL from Xbox root to this instance.

};

#endif // _XBOXFOLDER_H
