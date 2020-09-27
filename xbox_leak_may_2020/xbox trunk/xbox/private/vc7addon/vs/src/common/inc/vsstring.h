//=--------------------------------------------------------------------------=
// VsString.H
//=--------------------------------------------------------------------------=
// Common string manipulation functions and classes.  This library has the
// following goals:
//
//    1.  Unicode / Ansi transparent.  You should be able to compile your code
//        as ansi or as unicode and all these calls should work transparently.
//
//    2.  Simple.  I want to provide a broad range of functionality without
//        creating ten thousand API calls.  Also, I want to name these
//        API's simply and consistently.
//
//=--------------------------------------------------------------------------=
// Copyright 1997 Microsoft Corporation.  All rights reserved.
// Information contained herein is proprietary and confidential.
// 
// BrianPe.  4/97
//

#ifndef _INC_VSSTRING_H
#define _INC_VSSTRING_H


//=--------------------------------------------------------------------------=
// StrConvert.  this is a very simple string utility class.  No, it does
// not, should not, will not ever become a full string class.  All it does
// is help facilitate conversions among the following datatypes:
//
// WCHAR
// CHAR
// Resource ID
//
// One problem with a class like this is error handling - it is always 
// assumed to succeed.  This class will return an empty string if it
// runs out of memory.  Not particularly robust, but it will prevent
// a crash.
//
class StrConvert
  {
  friend void VsStrSetDefaultHInstance(HINSTANCE hInst);

  public:
    StrConvert(LPSTR  psz);
    StrConvert(LPWSTR pwsz);
    StrConvert(UINT   uId, HINSTANCE hinstResource = NULL);

    ~StrConvert();

    operator LPSTR();
    operator LPWSTR();

    static void SetDefaultInstance(HINSTANCE hinstResource) { s_hinstResource = hinstResource; }

  private:
    UINT      m_fWszStatic:1;
    UINT      m_fSzStatic:1;
    LPSTR     m_psz;
    LPWSTR    m_pwsz;
    HINSTANCE m_hinst;
    UINT      m_uId;

    void _LoadString(BOOL fUnicodePreferred);

    static HINSTANCE    s_hinstResource;
    const  static WCHAR s_chEmpty;
  };


//=--------------------------------------------------------------------------=
// String copying functions.  These are overloaded here so you can just
// pass in what you've got and what you want, and they will always work.
// They always return the number of BYTES copied, and they always take the
// number of CHARACTERS to copy.
//
DWORD VsStrCpy(LPSTR pszDest,   LPSTR pszSrc,   int cch = -1);
DWORD VsStrCpy(LPWSTR pszDest,  LPSTR pszSrc,   int cch = -1);
DWORD VsStrCpy(LPSTR pszDest,   LPWSTR pszSrc,  int cch = -1);
DWORD VsStrCpy(LPWSTR pszDest,  LPWSTR pszSrc,  int cch = -1);


//=--------------------------------------------------------------------------=
// Same thing for comparisons.  We only have one compare function, but it
// takes a variety of flags.
//
#define SC_IGNORECASE   0x01
#define SC_IGNOREWIDTH  0x02

enum STRING_COMPARE
  {
  SC_LESSTHAN     = -1,
  SC_EQUAL        = 0,
  SC_GREATERTHAN  = 1
  };

STRING_COMPARE VsStrCmp(LPSTR pszStr1,  LPSTR pszStr2,  int nCompareFlags = 0);
STRING_COMPARE VsStrCmp(LPWSTR pszStr1, LPSTR pszStr2,  int nCompareFlags = 0);
STRING_COMPARE VsStrCmp(LPSTR  pszStr1, LPWSTR pszStr2, int nCompareFlags = 0);
STRING_COMPARE VsStrCmp(LPWSTR pszStr1, LPWSTR pszStr2, int nCompareFlags = 0);


#endif // _INC_VSSTRING_H
