///////////////////////////////////////////////////////////////////////////////
//	dllmain.h
//
//	Created by :			Date :
//		BrianCr				12/29/94
//
//	Description :
//		Exports for each subsuite DLL
//		This file must be included once in each subsuite DLL
//
//		Notes: originally, DllMain was included in subsuite.lib.
//		Apparently, exports from a lib are not exported from a DLL unless
//		they're called.
//

#ifndef __DLLMAIN_H__
#define __DLLMAIN_H__

#ifndef _AFXDLL

#error Only include this file in once in a SubSuite DLL. (_AFXDLL not defined.)

#else // _AFXDLL

///////////////////////////////////////////////////////////////////////////////
//	Debug/Retail mismatch identification
//  We want to be able to identify debug and retail builds of subsuite DLLs

extern "C" __declspec(dllexport) BOOL IsDebugBuild(void)
{
	#ifdef _DEBUG
		return TRUE;
	#else
		return FALSE;
	#endif
}
#endif // _AFXDLL

#endif // __DLLMAIN_H__
