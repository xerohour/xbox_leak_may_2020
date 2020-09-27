///////////////////////////////////////////////////////////////////////////////
//	export.h (SRC export)
//
//	Created by :			Date :
//		ScotF			06/20/97
//
//	Description :
//		SRC DLL defines
//

#ifndef __SRCXPRT_H__
#define __SRCXPRT_H__

#ifdef EXPORT_SRC
	#define SRC_CLASS AFX_EXT_CLASS
	#define SRC_API   AFX_EXT_API
	#define SRC_DATA  AFX_EXT_DATA
#else
	#define SRC_CLASS __declspec(dllimport)
	#define SRC_API   __declspec(dllimport)
	#define SRC_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "srcd.lib")
	#else
		#pragma comment(lib, "src.lib")
	#endif
#endif

#endif // __SRCXPRT_H__
