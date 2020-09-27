/*****************************************************************************
 *    xboxicon.h
 *****************************************************************************/

#ifndef _XBOXICON_H
#define _XBOXICON_H


INT GetXboxIcon(UINT uFlags, BOOL fIsRoot);

/*****************************************************************************
    CXboxIcon

    The stuff that tells the shell which icon to use.
    Just plain annoying.  No real work is happening.
    Fortunately, the shell does most of the real work.

    Again, note that the szName is a plain char and not a TCHAR,
    because UNIX filenames are always ASCII.

    Extract() returning S_FALSE means "Could you do it for me?  Thanks."
 *****************************************************************************/

class CXboxIcon          : public IExtractIconW
                        , public IExtractIconA
                        , public IQueryInfo
{
public:
    //////////////////////////////////////////////////////
    // Public Interfaces
    //////////////////////////////////////////////////////
    
    // *** IUnknown ***
    virtual STDMETHODIMP_(ULONG) AddRef(void);
    virtual STDMETHODIMP_(ULONG) Release(void);
    virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
    
    // *** IExtractIconA ***
    virtual STDMETHODIMP GetIconLocation(UINT uFlags, LPSTR szIconFile, UINT cchMax, int * piIndex, UINT * pwFlags);
    virtual STDMETHODIMP Extract(LPCSTR pszFile, UINT nIconIndex, HICON * phiconLarge, HICON * phiconSmall, UINT nIconSize) {return S_FALSE;};
    
    // *** IExtractIconW ***
    virtual STDMETHODIMP GetIconLocation(UINT uFlags, LPWSTR szIconFile, UINT cchMax, int * piIndex, UINT * pwFlags);
    virtual STDMETHODIMP Extract(LPCWSTR pszFile, UINT nIconIndex, HICON * phiconLarge, HICON * phiconSmall, UINT nIconSize) {return S_FALSE;};

    // *** IQueryInfo ***
    virtual STDMETHODIMP GetInfoTip(DWORD dwFlags, WCHAR **ppwszTip);
    virtual STDMETHODIMP GetInfoFlags(DWORD *pdwFlags);


public:
    CXboxIcon();
    ~CXboxIcon(void);
    // Friend Functions
    friend HRESULT CXboxIcon_Create(CXboxFolder * pff, CXboxPidlList * pflHfpl, REFIID riid, LPVOID * ppv);
    friend HRESULT CXboxIcon_Create(CXboxFolder * pff, CXboxPidlList * pflHfpl, CXboxIcon ** ppfm);

protected:
    // Private Member Variables
    int                     m_cRef;

    CXboxPidlList *          m_pflHfpl;      // XboxDir in which our pidls live
    int                     m_nRoot;        // Gross HACKHACK (see CXboxIcon_Create)
    SINGLE_THREADED_MEMBER_VARIABLE;

    // Private Member Functions
    int ParseIconLocation(LPSTR pszIconFile);
    void GetDefaultIcon(LPSTR szIconFile, UINT cchMax, HKEY hk);
    HRESULT GetIconLocHkey(LPSTR szIconFile, UINT cchMax, LPINT pi, HKEY hk);
};

#endif // _XBOXICON_H
