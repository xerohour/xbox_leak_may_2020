/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       floatmath.cpp
 *  Content:    Private impementation of floating-point math routines.
 *
 ***************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

    // Converts a floating point value to a long.
    _declspec(naked) long FloatToLong(float f)
    {
        _asm
        {
            // Note that this does a truncate, not a floor:

            cvttss2si eax, [esp+4]
            ret 4
        }
    }

    // Cosine.
    _declspec(naked) float Cos(float e)
    {
        _asm
        {
            fld [esp+4]
            fcos
            ret 4
        }
    }

    // Exponent
    float Exp(float e)
    {
        WORD istat;
        WORD fstat;

        _asm 
        {
            fld [e]

	        xor	ch,ch		    ; result is always positive
	        fldl2e
	        fmul			    ; convert log base e to log base 2

	        fld	st(0)			; copy TOS
	        frndint 			; near round to integer
	        ftst
            fstsw   [istat]     ; save integer part status
	        fwait
	        fxch				; NOS gets integer part
	        fsub	st,st(1)		; TOS gets fraction
	        ftst
            fstsw   [fstat]     ; save fraction part status
	        fabs
	        f2xm1
        
            fld1
	        fadd
	        test	[fstat+1],1	; if fraction > 0 (TOS > 0)
	        jz	ExpNoInvert	    ;	 bypass 2^x invert

	        fld1
	        fdivrp	st(1),st(0)

        ExpNoInvert:
	        test	[istat+1],040h 	    ; if integer part was zero
	        jnz	ExpScaled	    ;	 bypass scaling to avoid bug
	        fscale			    ; now TOS = 2^x

        ExpScaled:
	        or	ch,ch		    ; check for negate flag
	        jz	expret
	        fchs			    ; negate result (negreal ^ odd integer)

        expret:
	        fxch
            fstp st(0)
        }
    }

    // Log
    _declspec(naked) float Log(float e)
    {
        if (DBG_CHECK(e <= 0.0f))
        {
            DXGRIP("Log - fast log doesn't handle zero or negative values.");
        }

        _asm
        {
            fldln2
            fld [esp+4]
            fyl2x
            ret 4
        }
    }

} // end namespace
