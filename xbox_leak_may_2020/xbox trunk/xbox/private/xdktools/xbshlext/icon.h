/*++

Copyright (c) Microsoft Corporation


Module Name:

    xbicon.h

Abstract:
    
    Declaration of CXboxExtractIcon.

    CXboxExtractIcon is initialized in one of the following ways.
    
    a) As a root.
    b) As a console.
    c) As a volume.
    d) As a folder.
    e) As a file.

Environment:

    Windows 2000
    Uses ATL

Notes:

Revision History:

    March 2001 - created by Mitchell Dernis (mitchd)

--*/
#ifndef __XBICON_H_
#define __XBICON_H_


#include "resource.h"       // main symbols

//-----------------------------------------------------------------
//  CXboxExtractIcon - implememtns IExtractIconA and IExtractIConW
//-----------------------------------------------------------------
class ATL_NO_VTABLE CXboxExtractIcon : 
	public CComObjectRootEx<CComMultiThreadModel>,
    public IExtractIconA,
    public IExtractIconW,
    public IXboxVisitor   //For initialization.
{

public:
    CXboxExtractIcon() : 
      m_hr(S_OK),
      m_uCount(0) {}
    ~CXboxExtractIcon(){}

    /*
    **  Type of Icon to present.
    */
    enum XboxIconType
    {
        Root = 0,
        AddConsole,
        Console,
        ConsoleDefault,
        Volume,
        Folder,
        File,
        Xbe
    };
    
    /*
    ** ATL INTERFACE MAP.
    */
    DECLARE_PROTECT_FINAL_CONSTRUCT()
    BEGIN_COM_MAP(CXboxExtractIcon)
     COM_INTERFACE_ENTRY_IID(IID_IExtractIconA, IExtractIconA)
     COM_INTERFACE_ENTRY_IID(IID_IExtractIconW, IExtractIconW)
    END_COM_MAP()

 public:
    // *** IExtractIconA methods ***
    STDMETHOD(Extract)(
        LPCSTR pszFile,
        UINT nIconIndex,
        HICON *phiconLarge,
        HICON *phiconSmall,
        UINT nIconSize
        ) {return S_FALSE;}
    
    STDMETHOD(GetIconLocation)(
        UINT uFlags,
        LPSTR szIconFile,
        UINT cchMax,
        LPINT piIndex,
        UINT *pwFlags
        );
    
    // *** IExtractIconW methods ***
    STDMETHOD(Extract)(
        LPCWSTR pszFile,
        UINT nIconIndex,
        HICON *phiconLarge,
        HICON *phiconSmall,
        UINT nIconSize
        ) {return S_FALSE;}

    STDMETHOD(GetIconLocation)(
        UINT uFlags,
        LPWSTR szIconFile,
        UINT cchMax,
        LPINT piIndex,
        UINT *pwFlags
        );

  public:
    /*
    ** IXboxVisitor
    */
    virtual void VisitRoot          (IXboxVisit *pRoot,                DWORD *pdwFlags);   
    virtual void VisitAddConsole    (IXboxVisit *pAddConsole,          DWORD *pdwFlags);
    virtual void VisitConsole       (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags);   
    virtual void VisitVolume        (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags);
    virtual void VisitFileOrDir     (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags);
    virtual void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){E_NOTIMPL;};

    /*
    **  Did succeed?
    */
    HRESULT GetHResult() {if(m_uCount)return m_hr; return E_NOINTERFACE;}

  private:

    HRESULT GetIconLocationImpl(UINT uFlags, LPSTR szIconFile, UINT cchMax, LPINT piIndex, UINT *pwFlags);

    HRESULT      m_hr;
    UINT         m_uCount;
    XboxIconType m_eIconType;
    CHAR         m_szFilename[MAX_PATH];
    DWORD        m_dwAttributes;
};

#endif //__XBICON_H_
