// UniEsc - escape/unescape non-ASCII in W<->A strings 
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1999 Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------
// This is a suitable way to convert a Unicode string to ASCII (NOT ANSI) 
// and back, preserving all Unicode code points.
//
// By using ASCII (char < 128), the string is transparent to any ANSI/UNICODE/UTF-8 
// conversions it may suffer during it's lifetime, and the original Unicode text
// can be regenerated.
//
// When converting from W to A, all '\' become '\\', and all chars > 127 become
// \u#### where #### is 4 hex digits specifying the unicode code point.
//
// When converting from A to W, '\u####' #### can be 1-4 contiguous hex digits.
// scanning stops at the first non-hex digit. '\u' not followed by a hex digit
// becomes 'u'.
//
// This is a good way to save a Unicode string to the registry as REG_SZ using ANSI
// registry APIs on all platforms.
//
//----------------------------------------------------------------
#pragma once

int   WINAPI GetUnicodeEscapeStringLength   (PCWSTR pszSrc);
int   WINAPI UnicodeEscapeString            (PCWSTR pszSrc, int cch, PSTR  pszDst);
int   WINAPI GetUnicodeUnescapeStringLength (PCSTR  pszSrc);
int   WINAPI UnicodeUnescapeString          (PCSTR  pszSrc, int cch, PWSTR pszDst);

PSTR  WINAPI GetUnicodeEscapeString   (PCWSTR pszSrc); // VSFree result
PWSTR WINAPI GetUnicodeUnescapeString (PCSTR pszSrc);  // VSFree result
BSTR  WINAPI GetUnicodeUnescapeBSTR   (PCSTR pszSrc);
