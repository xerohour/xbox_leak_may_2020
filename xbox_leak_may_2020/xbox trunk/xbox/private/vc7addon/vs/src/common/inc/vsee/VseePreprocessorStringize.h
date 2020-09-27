/*-----------------------------------------------------------------------------
Microsoft VSEE

Microsoft Confidential
Copyright 1995-2000 Microsoft Corporation. All Rights Reserved.

@doc external
@module VseePreprocessorStringize.h

@owner JayK
-----------------------------------------------------------------------------*/
#if !defined(VS_COMMON_INC_VSEE_PREPROCESSORSTRINGIZE_H_INCLUDED_) // {
#define VS_COMMON_INC_VSEE_PREPROCESSORSTRINGIZE_H_INCLUDED_
/*#pragma once ends up in .rgi, which is bad, so do not do it*/

#include "VseePreprocessorPaste.h"

#define VseePrivatePreprocessorStringize(x) # x

/*-----------------------------------------------------------------------------
Name: VseePreprocessorStringize, VseePreprocessorStringizeW
@macro
These macros simply stringize their parameter, after evaluating it;
it is evaluated so that
define A B
VseePreprocessorStringize(A) -> "B" instead of "A"
VseePreprocessorStringizeW(A) -> L"B" instead of L"A"
@owner JayK
-----------------------------------------------------------------------------*/
#define VseePreprocessorStringize(x) VseePrivatePreprocessorStringize(x)
#define VseePreprocessorStringizeW(x) VseePreprocessorPaste(L, VseePrivatePreprocessorStringize(x))

#endif // }
