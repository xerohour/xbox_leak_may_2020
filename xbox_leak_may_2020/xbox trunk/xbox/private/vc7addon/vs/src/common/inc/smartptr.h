#ifndef SMARTPTR_H_INCLUDED
#define SMARTPTR_H_INCLUDED

///////////////////////////////////////////////////////////////////////////////
//
// "Smart Pointer" classes
//
// Revision History:
//   06/13/1997 AdamK - created.
//

#include <windows.h>
#include "vsmem.h"

// ignore error C4284, since we can instantiate the template 
// even when T is not a UDT, i.e. CPointer<char> pChar.  
#pragma warning(disable: 4284) // C4284 return type for 'identifier::operator –>' is not a UDT or reference to a UDT. 


////////////////////////////////////////////////////////////////////////////////
// CVoidPointer
//
// Provides core implementation of CPointer.
// Do not use this class directly -- use CPointer instead.
// 
class CVoidPointer
{
protected:
    LPVOID Address;
    CVoidPointer()
    {
        Address = 0;
    }
    ~CVoidPointer()
    {
        Free();
    }
public:
    bool Allocate(DWORD NumBytes)
    {
        LPVOID NewAddress;

        // if memory was already allocated, then reallocate, else just allocate
        if (Address)
        {
            NewAddress = VSRealloc(Address, NumBytes);

            if (NewAddress)
            {
                // if successful, release old address, since it should not
                // be freed when reallocating memory
                Release();
            }
        }
        else
        {
            NewAddress = VSAlloc(NumBytes);
        }

        if (NewAddress)
        {
            Assign(NewAddress);
            return true;
        }
        else
        {
            return false;
        }
    }
    bool Copy(LPVOID pBuffer, DWORD BufferSize)
    {
        if (Allocate(BufferSize))
        {
            CopyMemory(Address, pBuffer, BufferSize);
            return true;
        }
        else
        {
            return false;
        }
    }
    void Free()
    {
        Assign(0);
    }
    // this function releases the pointer so it may passed to another block
    // which assumes responsibility for freeing the allocated memory.
    LPVOID Release()
    {
        LPVOID ReturnValue = Address;
        Address = 0;
        return ReturnValue;
    }
    bool IsNull() const
    {
        return (Address == 0);
    }
    operator bool() const
    {
        return !IsNull();
    }
    operator !() const
    {
        return IsNull();
    }
protected:
    void Assign(LPVOID const NewAddress = 0) 
    {
        if (Address)
        {
            VSFree(Address);
        }
        Address = NewAddress;
    }
};

 
////////////////////////////////////////////////////////////////////////////////
// CPointer
//
// Implements a "smart pointer" class for a pointer to any type, which handles
// allocating and freeing memory.  
//
template<typename T>
class CPointer : public CVoidPointer
{
public:
    operator T*() const
    {
        return GetAddress();
    }
    T& operator*() const
    {
        return *GetAddress();
    }
    T* operator->() const
    {
        return GetAddress();
    }
    T* Release()
    {
        return reinterpret_cast<T*>(CVoidPointer::Release());
    }
protected:
    T* GetAddress() const
    {
        return reinterpret_cast<T*>(Address);
    }
    void Assign(T* const NewAddress = 0) 
    {
        CVoidPointer::Assign(reinterpret_cast<LPVOID>(NewAddress));
    }
};

//#ifndef CP_THREAD_ACP
//#define CP_THREAD_ACP CP_ACP
//#endif 

inline DWORD GetStringLength(LPCSTR pAnsiString)
{
    return (DWORD)(pAnsiString ? strlen(pAnsiString) : 0);
}

inline DWORD GetStringLength(LPCWSTR pUnicodeString)
{
    return (DWORD)(pUnicodeString ? wcslen(pUnicodeString) : 0);
}

inline DWORD GetStringListLength(LPCSTR pAnsiStringList)
{
    if (!pAnsiStringList)
    {
        return 0;
    }

    DWORD CurrStringLength;
    DWORD AnsiStringListLength = 0;

    for (LPCSTR pCurrString = pAnsiStringList; *pCurrString; pCurrString += CurrStringLength)
    {
        CurrStringLength = (DWORD)strlen(pCurrString) + 1;
        AnsiStringListLength += CurrStringLength;
    }
    ++AnsiStringListLength; // final NULL terminator

    return AnsiStringListLength;
}

inline DWORD GetStringListLength(LPCWSTR pUnicodeStringList)
{
    if (!pUnicodeStringList)
    {
        return 0;
    }

    DWORD CurrStringLength;
    DWORD UnicodeStringListLength = 0;

    for (LPCWSTR pCurrString = pUnicodeStringList; *pCurrString; pCurrString += CurrStringLength)
    {
        CurrStringLength = (DWORD)wcslen(pCurrString) + 1;
        UnicodeStringListLength += CurrStringLength;
    }
    ++UnicodeStringListLength; // final NULL terminator

    return UnicodeStringListLength;
}

class CAnsiString : public CPointer<CHAR>
{
public:
    // copy from null-terminated ansi string to ansi string
    bool CopyString(LPCSTR pAnsiString) 
    {
        if (!pAnsiString)
        {
            Free();
            return true;
        }

        DWORD AnsiStringSize = ((DWORD)strlen(pAnsiString) + 1) * sizeof(CHAR); // size in bytes

        if (Allocate(AnsiStringSize))
        {
            CopyMemory(Address, pAnsiString, AnsiStringSize);
            return true;
        }
        else
        {
            return false;
        }
    }
    // copy from unicode string to an ansi string
    bool CopyString(LPCWSTR pUnicodeString, DWORD CodePage = CP_ACP, DWORD Flags = 0,
        LPCSTR pDefaultChar = NULL, LPBOOL pUsedDefaultChar = NULL)
    {
        if (!pUnicodeString)
        {
            Free();
            return true;
        }

        CAnsiString pTempAnsiString;

        // determine number of characters needed to hold ansi string
        DWORD CharsNeeded = WideCharToMultiByte(CodePage, Flags, pUnicodeString, -1, NULL, 0, 0, 0);

        // allocate memory for ansi string
        if (!pTempAnsiString.Allocate(CharsNeeded * sizeof(CHAR)))
        {
            return false;
        }

        // get ansi string
        if (!WideCharToMultiByte(CodePage, Flags, pUnicodeString, -1, 
            pTempAnsiString, CharsNeeded, pDefaultChar, pUsedDefaultChar))
        {
            return false;
        }

        // assign temp string
        Assign(pTempAnsiString);    
        pTempAnsiString.Release();  

        return true;
    }
    DWORD GetLength() const
    {
        return GetStringLength(GetAddress());
    }
};


class CUnicodeString : public CPointer<WCHAR>
{
public:
    // copy from null-terminated unicode string to unicode string
    bool CopyString(LPCWSTR pUnicodeString) // copy a null-terminated string
    {
        DWORD UnicodeStringSize = ((DWORD)wcslen(pUnicodeString) + 1) * sizeof(WCHAR); // size in bytes

        if (Allocate(UnicodeStringSize))
        {
            CopyMemory(Address, pUnicodeString, UnicodeStringSize);
            return true;
        }
        else
        {
            return false;
        }
    }
    // copy from ansi string to a unicode string
    bool CopyString(LPCSTR pAnsiString, DWORD CodePage = CP_ACP, DWORD Flags = 0)
    {
        CUnicodeString pTempUnicodeString;

        // determine number of characters needed to hold unicode string
        DWORD CharsNeeded = MultiByteToWideChar(CodePage, Flags, pAnsiString, -1, NULL, 0);

        // allocate memory for unicode string
        if (!pTempUnicodeString.Allocate(CharsNeeded * sizeof(WCHAR)))
        {
            return false;
        }

        // get unicode string
        if (!MultiByteToWideChar(CodePage, Flags, pAnsiString, -1, pTempUnicodeString, CharsNeeded))
        {
            return false;
        }

        // assign temp string
        Assign(pTempUnicodeString);
        pTempUnicodeString.Release();

        return true;
    }    
    DWORD GetLength() const
    {
        return GetStringLength(GetAddress());
    }
};

class CAnsiStringList : public CPointer<CHAR>
{
public:
    // copy from a null-terminated list of ansi strings
    bool CopyStringList(LPCSTR pAnsiStringList) 
    {
        if (!pAnsiStringList)
        {
            Free();
            return true;
        }

        DWORD AnsiStringListSize = GetStringListLength(pAnsiStringList) * sizeof(CHAR);

        if (Allocate(AnsiStringListSize))
        {
            CopyMemory(Address, pAnsiStringList, AnsiStringListSize);
            return true;
        }
        else
        {
            return false;
        }
    }
    // copy from a null-terminated list of unicode strings
    bool CopyStringList(LPCWSTR pUnicodeStringList, DWORD CodePage = CP_ACP, DWORD Flags = 0,
        LPCSTR pDefaultChar = NULL, LPBOOL pUsedDefaultChar = NULL)
    {
        if (!pUnicodeStringList)
        {
            Free();
            return true;
        }

        DWORD UnicodeStringListLength = GetStringListLength(pUnicodeStringList);

        CAnsiStringList pTempAnsiStringList;

        // determine number of bytes needed to hold ansi string list
        DWORD BytesNeeded = WideCharToMultiByte(CodePage, Flags, pUnicodeStringList, UnicodeStringListLength, NULL, 0, 0, 0);

        // allocate memory for ansi string list
        if (!pTempAnsiStringList.Allocate(BytesNeeded))
        {
            return false;
        }

        // get ansi string list
        if (!WideCharToMultiByte(CodePage, Flags, pUnicodeStringList, UnicodeStringListLength, 
            pTempAnsiStringList, BytesNeeded / sizeof(CHAR), pDefaultChar, pUsedDefaultChar))
        {
            return false;
        }

        // assign temp string
        Assign(pTempAnsiStringList);    
        pTempAnsiStringList.Release();  

        return true;
    }
    DWORD GetLength() const
    {
        return GetStringListLength(GetAddress());
    }
    bool AddString(LPCSTR pAnsiString)
    {
        // if NULL or empty, nothing to add
        if (!pAnsiString || !*pAnsiString)
        {
            return true;
        }

        // get list length
        DWORD ListLength = GetLength();

        // if list length is zero (i.e. list is NULL), then add one for final NULL terminator
        if (ListLength == 0)
        {
            ++ListLength; 
        }

        // determine bytes needed
        DWORD StringLength = (DWORD)strlen(pAnsiString) + 1;
        DWORD BytesNeeded = (ListLength + StringLength) * sizeof(CHAR);

        // reallocate
        if (!Allocate(BytesNeeded))
        {
            return false;
        }

        // copy new string onto the end of the list
        CopyMemory(GetAddress() + ListLength - 1, 
            pAnsiString, StringLength * sizeof(CHAR));

        // terminate list
        GetAddress()[StringLength + ListLength - 1] = 0;

        return true;
    }
    bool AddString(LPCWSTR pUnicodeString)
    {
        CAnsiString pAnsiString;

        // translate unicode string to ansi
        if (!pAnsiString.CopyString(pUnicodeString))
        {
            return false;
        }

        return AddString(pAnsiString);
    }
};


class CUnicodeStringList : public CPointer<WCHAR>
{
public:
    // copy from a null-terminated list of unicode strings
    bool CopyStringList(LPCWSTR pUnicodeStringList) 
    {
        if (!pUnicodeStringList)
        {
            Free();
            return true;
        }

        DWORD UnicodeStringListSize = GetStringListLength(pUnicodeStringList) * sizeof(WCHAR);

        if (Allocate(UnicodeStringListSize))
        {
            CopyMemory(Address, pUnicodeStringList, UnicodeStringListSize);
            return true;
        }
        else
        {
            return false;
        }
    }
    // copy from a null-terminated list of ansi strings
    bool CopyStringList(LPCSTR pAnsiStringList, DWORD CodePage = CP_ACP, DWORD Flags = 0)
    {
        if (!pAnsiStringList)
        {
            Free();
            return true;
        }

        DWORD CurrStringLength;
        DWORD AnsiStringListLength = 0;

        // calculate length (in characters) of ansi string list
        for (LPCSTR pCurrString = pAnsiStringList; *pCurrString; pCurrString += CurrStringLength)
        {
            CurrStringLength = (DWORD)strlen(pCurrString) + 1;
            AnsiStringListLength += CurrStringLength;
        }
        ++AnsiStringListLength; // include NULL terminator

        CUnicodeStringList pTempUnicodeStringList;

        // determine number of bytes needed to hold unicode string list
        DWORD BytesNeeded = MultiByteToWideChar(CodePage, Flags, pAnsiStringList, AnsiStringListLength, NULL, 0);

        // allocate memory for unicode string list
        if (!pTempUnicodeStringList.Allocate(BytesNeeded))
        {
            return false;
        }

        // get unicode string list
        if (!MultiByteToWideChar(CodePage, Flags, pAnsiStringList, AnsiStringListLength, 
            pTempUnicodeStringList, BytesNeeded / sizeof(CHAR)))
        {
            return false;
        }

        // assign temp string
        Assign(pTempUnicodeStringList);    
        pTempUnicodeStringList.Release();  

        return true;
    }
    DWORD GetLength() const
    {
        return GetStringListLength(GetAddress());
    }
    bool AddString(LPCWSTR pUnicodeString)
    {
        // if NULL or empty, nothing to add
        if (!pUnicodeString || !*pUnicodeString)
        {
            return true;
        }

        // determine list length
        DWORD ListLength = GetLength();

        // if list length is zero (i.e. list is NULL), then add one for final NULL terminator
        if (ListLength == 0)
        {
            ++ListLength; 
        }

        // determine bytes needed
        DWORD StringLength = (DWORD)wcslen(pUnicodeString) + 1;
        DWORD BytesNeeded = (ListLength + StringLength) * sizeof(WCHAR);

        // reallocate
        if (!Allocate(BytesNeeded))
        {
            return false;
        }

        // copy new string onto the end of the list
        CopyMemory(GetAddress() + ListLength - 1, pUnicodeString, StringLength * sizeof(WCHAR));

        // terminate list
        GetAddress()[StringLength + ListLength - 1] = 0;

        return true;
    }
    bool AddString(LPCSTR pAnsiString)
    {
        CUnicodeString pUnicodeString;

        // translate ansi string to unicode
        if (!pUnicodeString.CopyString(pAnsiString))
        {
            return false;
        }

        return AddString(pUnicodeString);
    }
};

#ifdef UNICODE
#define CVSString CUnicodeString
#define CVSStringList CUnicodeStringList
#else
#define CVSString CAnsiString
#define CVSStringList CAnsiStringList
#endif


#endif // SMARTPTR_H_INCLUDED
