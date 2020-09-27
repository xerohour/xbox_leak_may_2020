#include <xtl.h>
#include <stdio.h>
//#include <dsound.h>
#include "draw.h"

// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...

/*extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}*/


/*
#define VERIFY_SUCCESS(xprsn) \
{\
    HRESULT hrTemp = (xprsn);\
    if(!SUCCEEDED(hrTemp))\
    {\
        DebugPrint("Call failed: %s(%d): 0x%0.8x\n", __FILE__, __LINE__, hrTemp);\
    }\
}
*/