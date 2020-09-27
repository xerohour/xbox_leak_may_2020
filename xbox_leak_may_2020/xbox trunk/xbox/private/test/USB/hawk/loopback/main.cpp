#include "loopback.h"

// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

//------------------------------------------------------------------------------
//  Forward Declations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------

void __cdecl main()
{
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    CHawkUnits hawkUnits;
    CDraw draw;
    hawkUnits.Run(draw);
}

