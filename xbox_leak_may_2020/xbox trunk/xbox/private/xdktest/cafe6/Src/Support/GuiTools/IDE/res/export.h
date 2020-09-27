///////////////////////////////////////////////////////////////////////////////
//	export.h (res export)
//
//	Created by :			Date :
//		ScotF			06/20/97
//
//	Description :
//		res DLL defines
//

#ifndef __RESXPRT_H__
#define __RESXPRT_H__

#ifdef EXPORT_RES
	#define RES_CLASS AFX_EXT_CLASS
	#define RES_API   AFX_EXT_API
	#define RES_DATA  AFX_EXT_DATA
#else
	#define RES_CLASS __declspec(dllimport)
	#define RES_API   __declspec(dllimport)
	#define RES_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "resd.lib")
	#else
		#pragma comment(lib, "res.lib")
	#endif
#endif

#endif // __RESXPRT_H__
