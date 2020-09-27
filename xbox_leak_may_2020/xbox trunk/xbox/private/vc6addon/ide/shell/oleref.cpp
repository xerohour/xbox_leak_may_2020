#include "stdafx.h"

/*-----------------------------------------------------------------------------
Name:	FreeOlePtr

Description:
Free a pointer allocated through IMalloc
-----------------------------------------------------------------------------*/
void FreeOlePtr(void * pv)
{
	ASSERT(pv);

	COleRef<IMalloc>	srpMalloc;

	::CoGetMalloc(MEMCTX_TASK, &srpMalloc);
	ASSERT(srpMalloc != NULL);

	srpMalloc->Free(pv);
}


