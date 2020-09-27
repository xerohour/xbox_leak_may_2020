///////////////////////////////////////////////////////////////////////////////
//	dbgxprt.h (debugger export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		Debugger DLL defines
//

#ifndef __DBGXPRT_H__
#define __DBGXPRT_H__

#ifdef EXPORT_DEBUGGER
	#define DBG_CLASS AFX_EXT_CLASS
	#define DBG_API   AFX_EXT_API
	#define DBG_DATA  AFX_EXT_DATA
#else
	#define DBG_CLASS __declspec(dllimport)
	#define DBG_API   __declspec(dllimport)
	#define DBG_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "dbgd.lib")
	#else
		#pragma comment(lib, "dbg.lib")
	#endif
#endif

#endif // __GUIXPRT_H__
