/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			MAX Script SDK Example - adding a new system variable
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 1-4-99
 | 
\*===========================================================================*/

#include "MAXScrpt.h"

// Various MAX and MXS includes
#include "Numbers.h"
#include "MAXclses.h"
#include "Streams.h"
#include "MSTime.h"
#include "MAXObj.h"
#include "Parser.h"

#include "max.h"

// define the new primitives using macros from SDK
#include "definsfn.h"



/*===========================================================================*\
 |	The new get/set functions for our system variable
 |	We will use the DLLDirectory methods to see how many plugins are loaded in MAX
\*===========================================================================*/

Value*
get_numplugs()
{
	// Get a pointer to the interface
	Interface *ip = MAXScript_interface;

	// Get the DLLDir, and ask for a count
	DllDir *dl = ip->GetDllDirectory();
	return Integer::intern(dl->Count());
}

Value*
set_numplugs(Value* val)
{
	// We can't SET the number of loaded plugins.. so return false
	return &false_value;
}



/*===========================================================================*\
 |	MAXScript Plugin Initialization
 |	Ask MAX Script to regester us a new system variable
\*===========================================================================*/

__declspec( dllexport ) void
LibInit() { 
	define_system_global("NumPlugins", get_numplugs, set_numplugs);
}
