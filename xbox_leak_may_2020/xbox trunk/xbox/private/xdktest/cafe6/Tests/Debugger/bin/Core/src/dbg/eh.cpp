/*****************************************************************************
*
*	File:		EH.CPP
*	Project:	THREAD
*	Owner:		briancr
*
*	Description: This is a C++ file that basically throws a C++ exception.
*
*	History:
*		briancr			09/20/93	created
*
*****************************************************************************/

//#include "eh.h"

extern "C" void DoCppException(void);
void ThrowException(void);

void DoCppException(void)
{
	ThrowException();							// dbg:stack_traverse
}

void ThrowException(void)
{
	throw 5;
}												// dbg:eh_stop
