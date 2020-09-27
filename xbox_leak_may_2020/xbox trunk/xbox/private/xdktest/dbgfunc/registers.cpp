// adapted from \\vsqaauto\vc6snaps\Debugger\Core\src\registers

#include "dbgtest.h"

int Foo(int nArg)
{
	return nArg*2;
}							/* Return from Foo */


//turn off divide by zero compiler warning
#pragma warning(disable: 4723)

void Test_Registers()
{
    /* "err" is one of the magic register names recognized by the */
    /* debugger.  Here we force a conflict by having a local var  */
    /* of the same name */

    int err = 0;
	int fSetFpuCtrlWord = 0;	/* set this to 1 to do exception checks */
	int FpuCtrlWord;
	int res = Foo(7);		/* First line for tests */

	double y = 1e-19;
	_asm fld y;
	
	if (fSetFpuCtrlWord)
	{
		FpuCtrlWord = 0x277;
		__asm { FLDCW FpuCtrlWord }
	}

	float f1;

	/* check Overflow exception */
	f1 = (float) 1.234567e38;
	f1 *= f1;
	f1 *= f1;	/* Exception should be here for CTRL 277 */
	f1 *= f1;

	if (fSetFpuCtrlWord)
	{
		FpuCtrlWord = 0x27B;
		__asm { FLDCW FpuCtrlWord }
	}

	/* check divide by zero exception */
	f1 = 10.;
	f1 /= 0;	/* Exception should be here for CTRL 27B */
}
