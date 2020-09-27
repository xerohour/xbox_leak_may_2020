//
// javadbg.cpp
//
// Miscellaneous Java debugger support

#include "stdafx.h"
#pragma hdrstop

#include "javadbg.h"

size_t LOADDS PASCAL JavaGetDebuggeeBytes(HTM hTM, size_t cb, void FAR* bBuff, PFRAME pFrame)
{
	// in Java, the notion of debuggee bytes is really the value of the expression,
	// so we'll just use the EE to evaluate the expression and give us the result

	// get the value of the expression in the TM from the EE
	RTMI rtmi = { 0 };
	rtmi.fValue = TRUE;
	rtmi.fSzBytes = TRUE;
	HTI hti;

	if (EEvaluateTM (&hTM, pFrame, EEHORIZONTAL) == EENOERROR &&
		EEInfoFromTM(&hTM, &rtmi, &hti) == EENOERROR)
	{
		PTI pti = (PTI)BMLock(hti);

		// copy the value from the TM info to the buffer
		memset(bBuff, 0, cb);
		memcpy(bBuff, pti->Value, pti->cbValue);

		cb = pti->cbValue;

		BMUnlock(hti);
		EEFreeTI(&hti);
	}
	else {
		cb = 0;
	}

	return cb;
}
