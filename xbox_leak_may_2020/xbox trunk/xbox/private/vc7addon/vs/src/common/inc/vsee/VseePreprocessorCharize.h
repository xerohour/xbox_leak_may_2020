/*-----------------------------------------------------------------------------
Microsoft VSEE

Microsoft Confidential
Copyright 1995-2000 Microsoft Corporation. All Rights Reserved.

@doc external
@module VseePreprocessorCharize.h

@owner JayK
-----------------------------------------------------------------------------*/
#if !defined(VS_COMMON_INC_VSEE_PREPROCESSORCHARIZE_H_INCLUDED_) // {
#define VS_COMMON_INC_VSEE_PREPROCESSORCHARIZE_H_INCLUDED_
/*#pragma once ends up in .rgi, which is bad, so do not do it*/

#include "VseePreprocessorPaste.h"

#define VseePrivatePreprocessorCharize(x) #@ x

/*-----------------------------------------------------------------------------
Name: VseePreprocessorCharize, VseePreprocessorCharizeW
@macro
These macros simply charize their parameter, after evaluating it;
it is evaluated so that
define A B
VseePreprocessorCharize(A) -> 'B' instead of 'A'
VseePreprocessorCharizeW(A) -> 'B' instead of L'A'
@owner JayK
-----------------------------------------------------------------------------*/
#define VseePreprocessorCharize(x) VseePrivatePreprocessorCharize(x)
#define VseePreprocessorCharizeW(x) VseePreprocessorPaste(L, VseePrivatePreprocessorCharize(x))

#endif // }
