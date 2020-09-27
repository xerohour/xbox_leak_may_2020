///////////////////////////////////////////////////////////////////////////////
//	export.h (WRK export)
//
//	Created by :			Date :
//		ScotF				06/20/97
//
//	Description :
//		WRK DLL defines
//

#ifndef __WRKXPRT_H__
#define __WRKXPRT_H__

#ifdef EXPORT_WRK
	#define WRK_CLASS AFX_EXT_CLASS
	#define WRK_API   AFX_EXT_API
	#define WRK_DATA  AFX_EXT_DATA
#else
	#define WRK_CLASS __declspec(dllimport)
	#define WRK_API   __declspec(dllimport)
	#define WRK_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "wrkd.lib")
	#else
		#pragma comment(lib, "wrk.lib")
	#endif
#endif

#endif // __WRKXPRT_H__
