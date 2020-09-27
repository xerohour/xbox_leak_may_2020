///////////////////////////////////////////////////////////////////////////////
//	targxprt.h (target export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		Target DLL defines
//

#ifndef __TARGXPRT_H__
#define __TARGXPRT_H__

#ifdef EXPORT_TARGET
	#define TARGET_CLASS AFX_EXT_CLASS
	#define TARGET_API   AFX_EXT_API
	#define TARGET_DATA  AFX_EXT_DATA
#else
	#define TARGET_CLASS __declspec(dllimport)
	#define TARGET_API   __declspec(dllimport)
	#define TARGET_DATA  __declspec(dllimport)

	#ifdef _DEBUG
		#pragma comment(lib, "targetd.lib")
	#else
		#pragma comment(lib, "target.lib")
	#endif
#endif

#endif //__TARGXPRT_H__
