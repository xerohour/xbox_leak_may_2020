///////////////////////////////////////////////////////////////////////////////
//	shlxprt.h (shell export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		SHL DLL defines
//

#ifndef __SHLXPRT_H__
#define __SHLXPRT_H__

#ifdef EXPORT_SHELL
	#define SHL_CLASS AFX_EXT_CLASS
	#define SHL_API   AFX_EXT_API
	#define SHL_DATA  AFX_EXT_DATA
#else
	#define SHL_CLASS __declspec(dllimport)
	#define SHL_API   __declspec(dllimport)
	#define SHL_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "shld.lib")
	#else
		#pragma comment(lib, "shl.lib")
	#endif
#endif

#endif // __GUIXPRT_H__
