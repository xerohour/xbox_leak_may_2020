// Nothing.cpp : Defines the entry point for the dll application.
//

#include "Precomp.h"
#include "Nothing.h"
#include "shared.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch( ul_reason_for_call ) 
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
NOTHING_API int nNothing=0;

// This is an example of an exported function.
NOTHING_API int fnNothing(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see Nothing.h for the class definition
CNothing::CNothing()
{ 
	return; 
}

