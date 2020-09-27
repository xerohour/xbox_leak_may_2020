/*++

Copyright (c) Microsoft Corporation

Module Name:

    xboxpidl.h

Abstract:

    Declares class to manage PIDL's for the Xbox Development Kit
    namespace extension.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    02-05-2000 : created

--*/
#ifndef _XBOXPIDL_H
#define _XBOXPIDL_H


//-----------------------------------------------------------------------
// AdvancePtr cleans up the really ugly casting necessary to walk arrays
// of variable lengths structures, such as pidls.  Being a template, it
// works with any type of structure.
//-----------------------------------------------------------------------
template <class T>
inline T *AdvancePtr(T *pT, ULONG cb)
{
    return (T *)(((ULONG_PTR)pT) + cb);
}

//-----------------------------------------------------------------------------------
//  class CXboxPidl
//
//    Can be initialized as the Xbox root, as a random opaque pidl, or
//    with a display name relative to another CXboxPidl object.
//
//    The instance can then return a the friendly display name, or a name suitable
//    for use with the xboxdbg.dll API to access the hardware.
//
//    CXboxPidl never talks to the hardware itself and has no way of verifing
//    PIDLs other than by syntax.
//
//    The class also has a number of static members that are useful for parsing PIDL's
//    in general.
//     
//-----------------------------------------------------------------------------------
class CXboxPidl
{
  public:
    //c'tor
    CXboxPidl() : 
        m_dwFirstXboxIdOffset(0),
        m_Pidl(NULL),
        m_usItemType(0)
        {RefClass();} //default c'tor
    //d'tor
    ~CXboxPidl();
    
    //Initialization Routines (didn't use c'tors or assignment operators, because these
    //can fail.)
    HRESULT InitAsRoot();
    HRESULT InitWithPidl(LPCITEMIDLIST pidl, DWORD dwXboxIdOffset);
    HRESULT InitWithDisplayName(CXboxPidl *pidl, LPCOLESTR pwszDisplayName);
    
    //The assignment operator doesn't require prior initialization
    CXboxPidl& operator=(const CXboxPidl& rhs);  //assign from CXboxPidl
    
    //Append a PIDL onto the end.    
    HRESULT Append(const CXboxPidl& rhs); //concatenation with another PIDL
    
    //gets a copy of the real PIDL
    LPITEMIDLIST  GetPidl() const; //Makes a copy of the PIDL. The caller owns it.

    //get information from PIDL (The signature is designed to mesh with IShellFolder
    //GetDisplayNameOf), the flags are the same flags.
    HRESULT GetDisplayName(DWORD uFlags, LPSTRRET lpName) const;

    // get information necessary to communicate with an Xbox
    HRESULT GetXboxName(LPSTR pMachineName, DWORD *pdwLength);
    HRESULT GetXboxFilePath(LPSTR pFilePath, DWORD *pdwLength);
    
    //get size of the current pidl
    inline ULONG GetPidlSize() const {return GetPidlSize(m_Pidl);}

  private:

    // Used internally, to check that the generated PIDL was valid
    bool IsValid() const;
    USHORT GetItemIdType() const {return m_usItemType;}

    //The data associated with a PIDL
    DWORD m_dwFirstXboxIdOffset;
    LPITEMIDLIST m_Pidl;
    USHORT m_usItemType;
    
    //Static Members, includes some PIDL helper
    //functions that are not Xbox specific
  
  public:
    static ULONG GetPidlSize(LPCITEMIDLIST pidl);
    inline static LPCITEMIDLIST GetNextItemId(LPCITEMIDLIST pidl);
    inline static LPITEMIDLIST GetNextItemId(LPITEMIDLIST pidl);
    static LPCITEMIDLIST GetLastItemId(LPCITEMIDLIST pidl);
    static LPITEMIDLIST CopyFirstItemId(LPCITEMIDLIST pidl);
    static ULONG   RefClass()
                   { 
                     LONG refCount = InterlockedIncrement(&sm_lClassReference);
                     if(1==refCount) InitClass(); //possible sync bug
                     return refCount;
                   }
    static ULONG DerefClass()
                 {
                     LONG refCount = InterlockedDecrement(&sm_lClassReference);
                     if(0==refCount) DestroyClass();
                     return refCount;
                 }
  private:
      
    static HRESULT PreprocessDisplayName(
                    LPCOLESTR pwszDisplayName,
                    LPSTR     pszBuffer,
                    DWORD    *pdwLength,
                    DWORD    *pdwTokenCount,
                    BOOL     *pfAbsolute);

    static HRESULT InitClass();
    static void DestroyClass();
    static IMalloc *sm_pMalloc;
    static LONG    sm_lClassReference;

  private:
    CXboxPidl(const CXboxPidl&);  //copy constructor is private to prevent its
                                  //inadvertant use

};

#endif // _XBOXPIDL_H

