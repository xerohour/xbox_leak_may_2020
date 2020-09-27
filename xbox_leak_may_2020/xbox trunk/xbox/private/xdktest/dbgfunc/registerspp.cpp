// \\vsqaauto\vc6snaps\Debugger\Core\src\Registers PP\Registers PP.cpp

#include "dbgtest.h"

void Test_MMX_Registers()
{
	// The following instructions will modify the contents of the MMX
    // registers; step through the instructions and make sure that the
    // register window reflects those changes as they are made.
	
	__int64 regVal = 0;

	regVal = 100;
	__asm { MOVQ MM0, regVal }

	regVal = 200;
	__asm { MOVQ MM1, regVal }

	regVal = 300;
	__asm { MOVQ MM2, regVal }

	regVal = 400;
	__asm { MOVQ MM3, regVal }

	regVal = 500;
	__asm { MOVQ MM4, regVal }

	regVal = 600;
	__asm { MOVQ MM5, regVal }

	regVal = 700;
	__asm { MOVQ MM6, regVal }

	regVal = 800;
	__asm { MOVQ MM7, regVal }

	regVal = 0;
	regVal = 1;
}

void Test_SSE_Registers()
{
	// The following instructions will modify the contents of the PIII
    // SIMD floating point registers; step through the instructions 
    // and make sure that the register window reflects those changes 
    // as they are made.
	
    // Each register is represented in the register window in its
    // entirety as well as its four subparts.  for example, the value
    // of XMM0 is shown, as well as XMM00, XMM01, XMM02, XMM03.  A
    // change to XMM0 should change one or more of its subparts.

	float fVal = 0;

	fVal = 1.0;
	__asm { MOVSS XMM0, fVal }

	fVal = 2.0;
	__asm { MOVSS XMM1, fVal }

	fVal = 3.0;
	__asm { MOVSS XMM2, fVal }

	fVal = 4.0;
	__asm { MOVSS XMM3, fVal }

	fVal = 5.0;
	__asm { MOVSS XMM4, fVal }

	fVal = 6.0;
	__asm { MOVSS XMM5, fVal }

	fVal = 7.0;
	__asm { MOVSS XMM6, fVal }

	fVal = 8.0;
	__asm { MOVSS XMM7, fVal }


	fVal = 1.0;
	fVal = 2.0;
	fVal = 3.0;
}

