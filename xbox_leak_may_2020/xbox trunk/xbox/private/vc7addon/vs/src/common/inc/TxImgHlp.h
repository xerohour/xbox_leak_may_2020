// TxImgHlp.h - IVsTextImage helpers

#pragma once
#include "dbgout.h"

inline LONG WINAPI TextImage_GetSize (IVsTextImage * pText)
{
    LONG cch;
    VERIFY(SUCCEEDED(pText->GetCharSize(&cch)));
    return cch;
}

inline LONG WINAPI TextImage_GetLineCount (IVsTextImage * pText)
{
    LONG cLines;
    VERIFY(SUCCEEDED(pText->GetLineSize(&cLines)));
    return cLines;
}

inline LONG WINAPI TextImage_GetLineLength (IVsTextImage * pText, LONG iLine)
{
    LONG len = 0; 
    VERIFY(SUCCEEDED(pText->GetLineLength(iLine, &len)));
    return len;
}

inline LONG WINAPI TextImage_GetSpanLength (IVsTextImage * pText, const TextSpan * pSpan)
{
    LONG len = 0;
    VERIFY(SUCCEEDED(pText->GetSpanLength(pSpan, &len)));
    return len;
}

inline HRESULT WINAPI TextImage_GetLine (IVsTextImage * pText, LONG iLine, DWORD glde, LINEDATAEX & ld)
{
    return pText->GetLine (glde, iLine, 0, 0, &ld);
}

inline void WINAPI TextImage_ReleaseLine (IVsTextImage * pText, LINEDATAEX & ld)
{
    VERIFY(SUCCEEDED(pText->ReleaseLine(&ld)));
}

// Increment/Decrement Address
//
// TRUE  = changed and new coordinate is valid
// FALSE = reached edge of document - not changed
//
// These move by single WCHARs only
BOOL WINAPI TextImage_IncrementAddress (IVsTextImage * pImage, CTextAddress & ta);
BOOL WINAPI TextImage_DecrementAddress (IVsTextImage * pImage, CTextAddress & ta);

// These honor composite character boundaries (surrogates, combining characters)
BOOL WINAPI TextImage_IncrementAddressComposite (IVsTextImage * pImage, CTextAddress & ta);
BOOL WINAPI TextImage_DecrementAddressComposite (IVsTextImage * pImage, CTextAddress & ta);

// Get the type of line ending used in the image.
// Be aware that an image can contain multiple types of line endings.
// This helper samples only one or a few lines.
EOLTYPE WINAPI TextImage_GetEolType(IVsTextImage * pImage);
