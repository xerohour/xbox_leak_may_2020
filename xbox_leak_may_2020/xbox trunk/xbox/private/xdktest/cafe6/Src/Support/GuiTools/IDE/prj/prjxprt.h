///////////////////////////////////////////////////////////////////////////////
//	prjxprt.h (project export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		Project DLL defines
//

#ifndef __PRJXPRT_H__
#define __PRJXPRT_H__

#ifdef EXPORT_PROJECT
	#define PRJ_CLASS AFX_EXT_CLASS
	#define PRJ_API   AFX_EXT_API
	#define PRJ_DATA  AFX_EXT_DATA
#else
	#define PRJ_CLASS __declspec(dllimport)
	#define PRJ_API   __declspec(dllimport)
	#define PRJ_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "prjd.lib")
	#else
		#pragma comment(lib, "prj.lib")
	#endif
#endif

#endif // __PRJXPRT_H__
