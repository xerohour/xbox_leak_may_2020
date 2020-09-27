#include "stdafx.h"

//**
//** Methods for use by CComEnum
//**
//**
void CPidlUtils::init(LPITEMIDLIST* p)
{
    *p = NULL;
}
HRESULT CPidlUtils::copy(LPITEMIDLIST* pTo, const LPITEMIDLIST* pFrom)
{

    *pTo = CPidlUtils::Copy(*pFrom);
    if(!*pTo) return E_OUTOFMEMORY;
    return S_OK;
}

void CPidlUtils::destroy(LPITEMIDLIST* p)
{
  if(*p)
  {
    g_pShellMalloc->Free(*p);
    *p = NULL;
  }
}


//**
//** Methods for use by CXboxFolders
//**
//**

ULONG CPidlUtils::GetLength(LPCITEMIDLIST pidl)
{
    ULONG size = 2; //The size is at least the two terminating NULL.
    while(pidl->mkid.cb)
    {
        size += pidl->mkid.cb;
        pidl = AdvancePtr(pidl, pidl->mkid.cb);
    }
    return size;
}

LPITEMIDLIST CPidlUtils::Copy(LPCITEMIDLIST Pidl, UINT uExtraAllocation)
{
    ULONG pidlSize = GetLength(Pidl);
    LPITEMIDLIST pidlCopy = (LPITEMIDLIST)g_pShellMalloc->Alloc(pidlSize+uExtraAllocation);
    if(pidlCopy)
    {
        memcpy(pidlCopy, Pidl, pidlSize);
    }
    return pidlCopy;
}

void CPidlUtils::Free(LPITEMIDLIST pidl)
{
    g_pShellMalloc->Free(pidl);
}

LPITEMIDLIST CPidlUtils::LastItem(LPITEMIDLIST pidl)
{
    LPITEMIDLIST prevPidl = NULL;
    while(pidl->mkid.cb)
    {
        prevPidl = pidl;
        pidl = AdvancePtr(pidl, pidl->mkid.cb);
    }
    return prevPidl;
}

LPITEMIDLIST CPidlUtils::Concatenate(LPITEMIDLIST dest, LPCITEMIDLIST src)
{
    ULONG destSize = g_pShellMalloc->GetSize(dest);
    ULONG concatLength = GetLength(dest) - 2; //drop the termination from the dest
    ULONG sourceLength = GetLength(src);
    ULONG requiredSize = concatLength + sourceLength;
    if(requiredSize > destSize)
    {
        dest = (LPITEMIDLIST) g_pShellMalloc->Realloc((PVOID)dest, destSize);
    }
    if(dest)
    {
        memcpy((PVOID)((ULONG)dest+concatLength), (PVOID)src, sourceLength);
    }
    return dest;
}

//**
//** Debug method
//**
//**

void CPidlUtils::DumpPidl(LPCITEMIDLIST pidl)
{
    CHAR FormatBuffer[MAX_PATH];
    //CHAR FormatBuffer2[MAX_PATH];
    int bufferIndex = 0;
    LPCITEMIDLIST pidlOrg = pidl;
    OutputDebugStringA("PIDL:");
    while(pidl->mkid.cb)
    {
        int abIDLength = pidl->mkid.cb - sizeof(USHORT);
        wsprintfA(FormatBuffer, "cb=%d,abID=\'",pidl->mkid.cb);
        OutputDebugStringA(FormatBuffer);
        memcpy(FormatBuffer, pidl->mkid.abID, abIDLength);
        FormatBuffer[abIDLength] = '\'';
        FormatBuffer[abIDLength+1] = ':';
        FormatBuffer[abIDLength+2] = '\0';
        for(int i = 0; i < abIDLength; i++)
        {
            if( FormatBuffer[i] < 32 ) FormatBuffer[i] = ' ';
            if( FormatBuffer[i] > 127 ) FormatBuffer[i] = '.';
        }
        //wsprintfA(FormatBuffer2, "%s", FormatBuffer);
        OutputDebugStringA(FormatBuffer);
        pidl = AdvancePtr(pidl, pidl->mkid.cb);
    }
    OutputDebugStringA("PIDLEND\n");
}