#ifndef __TNCONTROL_SCRIPT__
#define __TNCONTROL_SCRIPT__
//#pragma message("Defining __TNCONTROL_SCRIPT__")




//==================================================================================
// External Structures
//==================================================================================

//- - - - - - - - - - - - - - - -
// Test input data structures
//- - - - - - - - - - - - - - - -

// Script:FreeOutputVars
typedef struct tagTID_SCRIPT_FREEOUTPUTVARS
{
	DWORD	dwCaseIDSize; // size of the case ID string to match, if any
	DWORD	dwInstanceIDSize; // size of the instance ID string to match, if any
	DWORD	dwNameSize; // size of the output variable name string to match, if any
	DWORD	dwTypeSize; // size of the output variable type string to match, if any
	BOOL	fFreeData; // whether the output data can be freed as well, if no variables are left

	// Anything after this is part of the case ID, instance ID, name and type
	// strings.
} TID_SCRIPT_FREEOUTPUTVARS, * PTID_SCRIPT_FREEOUTPUTVARS;

// Script:FreeResults
typedef struct tagTID_SCRIPT_FREERESULTS
{
	DWORD	dwCaseIDSize; // size of the case ID string to match, if any
	DWORD	dwInstanceIDSize; // size of the instance ID string to match, if any

	// Anything after this is part of the case and instance ID strings.
} TID_SCRIPT_FREERESULTS, * PTID_SCRIPT_FREERESULTS;

// Script:Sleep
typedef struct tagTID_SCRIPT_SLEEP
{
	DWORD	dwMS; // milliseconds to sleep
} TID_SCRIPT_SLEEP, * PTID_SCRIPT_SLEEP;

// Script:Wait
typedef struct tagTID_SCRIPT_WAIT
{
	DWORD	dwSecs; // seconds to wait until timeout, or INFINITE
	DWORD	dwStringSize; // how big the following string is, including NULL termination.

	// Anything after this is part of the dialog box string.
} TID_SCRIPT_WAIT, * PTID_SCRIPT_WAIT;





//==================================================================================
// External Prototypes
//==================================================================================
HRESULT ScriptLoadTestTable(PTNTESTTABLEGROUP pTable);






#else //__TNCONTROL_SCRIPT__
//#pragma message("__TNCONTROL_SCRIPT__ already included!")
#endif //__TNCONTROL_SCRIPT__
