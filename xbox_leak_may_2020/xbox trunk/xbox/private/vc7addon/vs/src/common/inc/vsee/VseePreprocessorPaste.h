/*-----------------------------------------------------------------------------
Microsoft VSEE

Microsoft Confidential
Copyright 1995-2000 Microsoft Corporation. All Rights Reserved.

@doc external
@module VseePreprocessorPaste.h

@owner JayK
-----------------------------------------------------------------------------*/
#if !defined(VS_COMMON_INC_VSEE_PREPROCESSORPASTE_H_INCLUDED_) // {
#define VS_COMMON_INC_VSEE_PREPROCESSORPASTE_H_INCLUDED_
/*#pragma once ends up in .rgi, which is bad, so do not do it*/

#define VseePrivatePreprocessorPaste(x, y) x ## y

/*-----------------------------------------------------------------------------
Name: VseePreprocessorPaste2, VseePreprocessorPaste3, etc.
@macro
These macros paste together n tokens, where n is in the name of the macro.
A level of evaluation is inserted as well.

define A 1
define B 2

VseePreprocessorPaste2(A, B) -> 12
@owner JayK
-----------------------------------------------------------------------------*/

// These are synonyms.
#define VseePreprocessorPaste(x, y)  VseePrivatePreprocessorPaste(x, y)
#define VseePreprocessorPaste2(x, y) VseePrivatePreprocessorPaste(x, y)

#define VseePreprocessorPaste3(x, y, z) VseePreprocessorPaste(VseePreprocessorPaste(x, y), z)
#define VseePreprocessorPaste4(w, x, y, z) VseePreprocessorPaste(VseePreprocessorPaste3(w, x, y), z)
#define VseePreprocessorPaste5(v, w, x, y, z) VseePreprocessorPaste(VseePreprocessorPaste4(v, w, x, y), z)
#define VseePreprocessorPaste6(u, v, w, x, y, z) VseePreprocessorPaste(VseePreprocessorPaste5(u, v, w, x, y), z)

#define VseePreprocessorPaste15(a1,a2,a3,a4,a5,a6,a7,a8,a9,a,b,c,d,e,f) \
	VseePreprocessorPaste3 \
	( \
		VseePreprocessorPaste5(a1,a2,a3,a4,a5), \
		VseePreprocessorPaste5(a6,a7,a8,a9,a), \
		VseePreprocessorPaste5(b,c,d,e,f) \
	)

#endif // }
