/////////////////////////////////////////////////////////
//
/*
	VB doesn't understand DWORDs, so I have to change
	all of the params to longs. Since I have to wait
	to turn this code on, I have entered this code so
	I can compile both ways. This allows me to check in
	the code now.

	If you are just now writing to this code use longs instead
	of DWORDs.

	NOTE: To avoid forcing VS to recompile, I've left vshelp.dll using
	DWORDs. However, I've compiled a VB compatible typelib version of helpsvcs.tlb
	into helptb.tlb. The ocx imports this tlb instead of helpsvcs.tlb.
	This is hopefully only a temporary solution...
*/

//#define _VB_COMPATIBLE_PARAMS
#ifdef _VB_COMPATIBLE_PARAMS
#define INT_TYPE LONG_PTR
#define LCID_TYPE long
cpp_quote("#ifndef INT_TYPE")
cpp_quote("#define INT_TYPE LONG_PTR")
cpp_quote("#define LCID_TYPE long")
cpp_quote("#endif")
#else
#define INT_TYPE DWORD_PTR
#define LCID_TYPE DWORD
cpp_quote("#ifndef INT_TYPE")
cpp_quote("#define INT_TYPE DWORD_PTR")
cpp_quote("#define LCID_TYPE DWORD")
cpp_quote("#endif")
#endif
