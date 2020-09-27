// W4Char.inl
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1999 Microsoft Corporation.  All Rights Reserved.
//
// July 24, 1999 [paulde] Paul Chase Dempsey
//
//-----------------------------------------------------------------

inline int WINAPI W4StrLen (PCW4STR pw4Str)
{
  if (!pw4Str) return 0;
  PCW4STR pch = pw4Str;
  while (*pch)
    ++pch;
  return (LONG32)(pch - pw4Str);
}

//-----------------------------------------------------------------
// W4CharRange
//
// Returns:
// -1  Out of UTF-16 (Unicode) range
//  0  Basic Unicode, i.e. BMP, plane 1
//  1  Surrogate
//
inline int WINAPI W4CharRange (W4CHAR ch)
{
  if (ch <= 0x0000FFFF) return 0;
  if (ch <= 0x0010FFFF) return 1;
  return -1;
}

//-----------------------------------------------------------------
inline int WINAPI WLengthOfW4Char (W4CHAR ch)
{
  if (ch <= 0x0000FFFF) return 1;
  if (ch <= 0x0010FFFF) return 2;
  return 1;
}

//-----------------------------------------------------------------
inline W4CHAR WINAPI MakeW4Char (WCHAR chHi, WCHAR chLo)
{
  UASSERT(IsHighSurrogate(chHi) && IsLowSurrogate(chLo));
  return ((W4CHAR)(chHi - UCH_HI_SURROGATE_FIRST) << 10) + 
         ((W4CHAR)(chLo - UCH_LO_SURROGATE_FIRST) + 0x00100000);

}

//-----------------------------------------------------------------
inline PWSTR WINAPI MakeSurrogatePair (PWSTR pch, W4CHAR ch)
{
  UASSERT(pch);
  UASSERT(ch > 0x0000FFFF && ch <= 0x0010FFFF);
  ch -= 0x0010000;
  *pch++ = (WCHAR)((ch >> 10) + UCH_HI_SURROGATE_FIRST);
  *pch++ = (WCHAR)((ch & 0x000003FF) + UCH_LO_SURROGATE_FIRST);
  return pch;
}

