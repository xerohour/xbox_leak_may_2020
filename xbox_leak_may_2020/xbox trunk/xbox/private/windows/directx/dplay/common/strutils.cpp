/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       StrUtils.cpp
 *  Content:    Implements the string utils
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *   02/12/2000	rmt		Created
 *   08/28/2000	masonb	Voice Merge: Added check of return code of MultiByteToWideChar in STR_jkAnsiToWide
 *   09/16/2000 aarono  fix STR_AllocAndConvertToANSI, ANSI doesn't mean 1 byte per DBCS character so we
 *                       must allow up to 2 bytes per char when allocating buffer (B#43286)
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "commoni.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON



#undef DPF_MODNAME
#define DPF_MODNAME "UnicodeToMultiByteN"

HRESULT
UnicodeToMultiByteN(
    OUT PCH MultiByteString,
    IN ULONG MaxBytesInMultiByteString,
    IN PWCH UnicodeString,
    IN ULONG BytesInUnicodeString)

/*++

Routine Description:

    This functions converts the specified unicode source string into an
    ansi string. The translation is done with respect to the
    ANSI Code Page (ACP) loaded at boot time.

Arguments:

    MultiByteString - Returns an ansi string that is equivalent to the
        unicode source string.  If the translation can not be done,
        an error is returned.

    MaxBytesInMultiByteString - Supplies the maximum number of bytes to be
        written to MultiByteString.  If this causes MultiByteString to be a
        truncated equivalent of UnicodeString, no error condition results.

    BytesInMultiByteString - Returns the number of bytes in the returned
        ansi string pointed to by MultiByteString.

    UnicodeString - Supplies the unicode source string that is to be
        converted to ansi.

    BytesInUnicodeString - The number of bytes in the the string pointed to by
        UnicodeString.

Return Value:

    SUCCESS - The conversion was successful

--*/

{
    ULONG LoopCount;
    ULONG CharsInUnicodeString;

    CharsInUnicodeString = BytesInUnicodeString / sizeof(WCHAR);

    LoopCount = (CharsInUnicodeString < MaxBytesInMultiByteString) ?
                 CharsInUnicodeString : MaxBytesInMultiByteString;

    while (LoopCount) {

        *MultiByteString = (*UnicodeString < 256) ? (UCHAR)*UnicodeString : '?';

        UnicodeString++;
        MultiByteString++;
        LoopCount--;
    }

    return DPN_OK;
}


#undef DPF_MODNAME
#define DPF_MODNAME "MultiByteToUnicodeN"

HRESULT
MultiByteToUnicodeN(
    OUT PWCH UnicodeString,
    IN ULONG MaxBytesInUnicodeString,
    IN PCH MultiByteString,
    IN ULONG BytesInMultiByteString)

/*++

Routine Description:

    This functions converts the specified ansi source string into a
    Unicode string. The translation is done with respect to the
    ANSI Code Page (ACP) installed at boot time.  Single byte characters
    in the range 0x00 - 0x7f are simply zero extended as a performance
    enhancement.  In some far eastern code pages 0x5c is defined as the
    Yen sign.  For system translation we always want to consider 0x5c
    to be the backslash character.  We get this for free by zero extending.

    NOTE: This routine only supports precomposed Unicode characters.

Arguments:

    UnicodeString - Returns a unicode string that is equivalent to
        the ansi source string.

    MaxBytesInUnicodeString - Supplies the maximum number of bytes to be
        written to UnicodeString.  If this causes UnicodeString to be a
        truncated equivalent of MultiByteString, no error condition results.

    BytesInUnicodeString - Returns the number of bytes in the returned
        unicode string pointed to by UnicodeString.

    MultiByteString - Supplies the ansi source string that is to be
        converted to unicode.  For single-byte character sets, this address
        CAN be the same as UnicodeString.

    BytesInMultiByteString - The number of bytes in the string pointed to
        by MultiByteString.

Return Value:

    SUCCESS - The conversion was successful.


--*/

{
    ULONG LoopCount;
    ULONG MaxCharsInUnicodeString;

    MaxCharsInUnicodeString = MaxBytesInUnicodeString / sizeof(WCHAR);

    LoopCount = (MaxCharsInUnicodeString < BytesInMultiByteString) ?
                 MaxCharsInUnicodeString : BytesInMultiByteString;

    while (LoopCount) {

        *UnicodeString = (WCHAR)(UCHAR)(*MultiByteString);

        UnicodeString++;
        MultiByteString++;
        LoopCount--;
    }

    return DPN_OK;
}


/*
 ** WideToAnsi
 *
 *  CALLED BY:	everywhere
 *
 *  PARAMETERS: lpStr - destination string
 *				lpWStr - string to convert
 *				cchStr - size of dest buffer
 *
 *  DESCRIPTION:
 *				converts unicode lpWStr to ansi lpStr.
 *				fills in unconvertable chars w/ DPLAY_DEFAULT_CHAR "-"
 *				
 *
 *  RETURNS:  if cchStr is 0, returns the size required to hold the string
 *				otherwise, returns the number of chars converted
 *
 */
#undef DPF_MODNAME
#define DPF_MODNAME "STR_jkWideToAnsi"
HRESULT STR_jkWideToAnsi(LPSTR lpStr,LPCWSTR lpWStr,int cchStr)
{
	if (!lpWStr && cchStr)
	{
		// can't call us w/ null pointer & non-zero cch
		DNASSERT(FALSE);
		return DPNERR_INVALIDPARAM;
	}
	
	UnicodeToMultiByteN( lpStr, cchStr, (PWCH) lpWStr, cchStr*sizeof(WCHAR));
	
	return DPN_OK;

} // WideToAnsi




#undef DPF_MODNAME
#define DPF_MODNAME "STR_jkAnsiToWide"
/*
 ** AnsiToWide
 *
 *  CALLED BY: everywhere
 *
 *  PARAMETERS: lpWStr - dest string
 *				lpStr  - string to convert
 *				cchWstr - size of dest buffer
 *
 *  DESCRIPTION: converts Ansi lpStr to Unicode lpWstr
 *
 *
 *  RETURNS:  if cchStr is 0, returns the size required to hold the string
 *				otherwise, returns the number of chars converted
 *
 */
HRESULT STR_jkAnsiToWide(LPWSTR lpWStr,LPCSTR lpStr,int cchWStr)
{
	if (!lpStr && cchWStr)
	{
		// can't call us w/ null pointer & non-zero cch
		DNASSERT(FALSE);
		return DPNERR_INVALIDPOINTER;
	}

	MultiByteToUnicodeN( lpWStr, cchWStr*sizeof(WCHAR), (CHAR *) lpStr, cchWStr );

	return DPN_OK;
}  // AnsiToWide


