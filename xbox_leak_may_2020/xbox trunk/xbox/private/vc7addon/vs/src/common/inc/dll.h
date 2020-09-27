//---------------------------------------------------------------------------
// Microsoft Visual Studio
//
// Microsoft Confidential
// Copyright (C) 1994 - 2000 Microsoft Corporation. 
// All Rights Reserved.
//
//---------------------------------------------------------------------------
#ifndef _DLL_H_
#define _DLL_H_

#include "smartptr.h"

#pragma warning(disable: 4786)

////////////////////////////////////////////////////////////////////////////////
// CDLLHandle
//
class CDLLHandle
{
protected:
    HMODULE Handle;
public:
    CDLLHandle()
    {
        Handle = 0;
    }
    ~CDLLHandle()
    {
        Assign(0);
    }
    operator HMODULE() const
    {
        return Handle;
    }
    bool Load(LPCWSTR pFileName, HMODULE hFile, DWORD Flags)
    {
        // first try using the Unicode api
        HMODULE NewHandle = LoadLibraryExW(pFileName, hFile, Flags);

        // if the Unicode api fails, try the Ansi api
        if (!NewHandle)
        {
            CAnsiString pAnsiFileName;

            if (!pAnsiFileName.CopyString(pFileName))
            {
                return false;
            }

            NewHandle = LoadLibraryExA(pAnsiFileName, hFile, Flags);
        }
        
        if (NewHandle)
        {
            Assign(NewHandle);
            return true;
        }
        else 
        {
            return false;
        }
    }
    void Unload()
    {
        Assign(0);
    }
    HMODULE Release()
    {
        HMODULE ReturnValue = Handle;
        Handle = 0;
        return ReturnValue;
    }
    bool IsValid() const
    {
        return (Handle != 0);
    }
protected:
    void Assign(HMODULE NewHandle)
    {
        if (Handle)
        {
            FreeLibrary(Handle);
        }
        Handle = NewHandle;
    }
};

////////////////////////////////////////////////////////////////////////////////
// CGenericDLLImport
//
// Implements a wrapper for an untyped DLL import.  Used in implementation of
// type-safe CDLLImport template.  Do not use directly.
//
class CGenericDLLImport
{
protected:
    FARPROC ImportAddress;
public:
    CGenericDLLImport() 
    {
        ImportAddress = 0;
    }
    bool ResolveImportAddress(HINSTANCE hInstance, LPCSTR pImportName, bool Required = true)
    {
        FARPROC NewAddress = GetProcAddress(hInstance, pImportName);

        if (NewAddress || !Required)
        {
            Assign(NewAddress);
            return true;
        }
        else
        {
            return false;
        }
    }
    operator bool() const
    {
        return IsValid();
    }
    bool operator !() const
    {
        return !IsValid();
    }
    bool IsValid() const
    {
        return ImportAddress != 0;
    }
protected:
    void Assign(FARPROC NewImportAddress)
    {
        ImportAddress = NewImportAddress;
    }
};

////////////////////////////////////////////////////////////////////////////////
// CDLLImport
//
// Implements a type-safe encapsulation of a DLL import.
template<class IMPORT_TYPE> class CDLLImport : public CGenericDLLImport
{
public:
    operator IMPORT_TYPE() const
    {
        return GetAddress();
    }
    IMPORT_TYPE GetAddress() const
    {
        return reinterpret_cast<IMPORT_TYPE>(ImportAddress);
    }
};

////////////////////////////////////////////////////////////////////////////////
// CDLL
//

class CDLL
{
protected:
    LPCWSTR pszFileName;
    CDLLHandle hDLL;
    DWORD LoadCount;
public:
    CDLL(LPCWSTR pszNewFileName);
    virtual ~CDLL();
    LPCWSTR GetFileName();
    bool Load(DWORD Flags = 0);
    void Unload();
    bool IsLoaded();
protected:
    virtual bool GetDLLImports() = 0;
    virtual bool OnLoad(bool IsInitialLoad);
    virtual void OnUnload(bool IsFinalUnload);
};

inline CDLL::CDLL(LPCWSTR pszNewFileName) : pszFileName(pszNewFileName)
{
    LoadCount = 0;
}

inline CDLL::~CDLL()
{
    // hopefully the load count is zero, but if not, unload the dll
    if (LoadCount)
    {
        LoadCount = 1; // want to do final unload
        Unload();
    }
}

inline LPCWSTR CDLL::GetFileName()
{
    return pszFileName;
}

inline bool CDLL::OnLoad(bool IsInitialLoad)
{
    return true;
}

inline void CDLL::OnUnload(bool IsFinalUnload)
{
}

inline bool CDLL::IsLoaded()
{
    return hDLL.IsValid();
}

////////////////////////////////////////////////////////////////////////////////
// CDLLLoader
//
// Allows exception-safe loading of DLLs.
//
class CDLLLoader
{
protected:
	CDLL& DLL;
    bool Loaded;
public:
    CDLLLoader(CDLL& InitDLL) : DLL(InitDLL)
    {
        Loaded = false;
    }
    ~CDLLLoader()
    {
        if (Loaded)
        {
            DLL.Unload();
        }
    }
    bool IsLoaded()
    {
        return Loaded;
    }
    bool Load()
    {
        if (!Loaded)
        {
            if (!DLL.Load())
            {
                return false;
            }
            Loaded = true;
        }
        return true;
    }
    void Unload()
    {
        if (Loaded)
        {
            DLL.Unload();
            Loaded = false;
        }
    }
};

#endif // _DLL_H_
