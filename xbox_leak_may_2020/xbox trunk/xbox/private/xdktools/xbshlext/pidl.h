/*++

Copyright (c) Microsoft Corporation

Module Name:

    pidl.h

Abstract:

    A number of utility methods for using pidls.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    02-05-2000 : created

--*/
#ifndef __PIDL_H__
#define __PIDL_H__


//-----------------------------------------------------------------------
// AdvancePtr cleans up the really ugly casting necessary to walk arrays
// of variable lengths structures, such as pidls.  Being a template, it
// works with any type of structure.
//-----------------------------------------------------------------------
template <class T>
inline T *AdvancePtr(T *pT, ULONG cb)
{
    return (T *)(((ULONG)pT) + cb);
}

class CPidlUtils
{
  public:

    enum PIDLTYPE
    {
        PidlTypeSimple,  // Single SHITEMID
        PidlTypeRoot,    // Root of Namespace Extension (i.e. Xbox Root)
        PidlTypeAbsolute // Root of System Namespace (i.e. Desktop)
    };
    //Methods specifically intended for use with the ATL CComEnum class.
    //Unlike the methods below which take a point to IMalloc, these use
    //SHGetMalloc.
    static void init(LPITEMIDLIST* p);
    static HRESULT copy(LPITEMIDLIST* pTo, const LPITEMIDLIST* pFrom);
    static void destroy(LPITEMIDLIST* p);

    //Utility Functions for PIDLs
    static ULONG         GetLength(LPCITEMIDLIST pidl);
    static LPITEMIDLIST  Copy(LPCITEMIDLIST pidl, UINT uExtraAllocation = 0);
    static void          Free(LPITEMIDLIST pidl);
    static LPITEMIDLIST  NextItem(LPITEMIDLIST pidl) {return AdvancePtr(pidl, pidl->mkid.cb);}
    static LPITEMIDLIST  LastItem(LPITEMIDLIST pidl);
    static LPITEMIDLIST  Concatenate(LPITEMIDLIST dest, LPCITEMIDLIST src);
    static void          DumpPidl(LPCITEMIDLIST pidl);
};

#ifdef NEVER
#define DEBUG_DUMP_PIDL(pidl) CPidlUtils::DumpPidl(pidl)
#else
#define DEBUG_DUMP_PIDL(pidl)
#endif


#endif //__PIDL_H__