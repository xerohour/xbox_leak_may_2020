///////////////////////////////////////////////////////////////////////////////
//	toolxprt.h (tools export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		Toolset DLL defines
//

#ifndef __TOOLXPRT_H__
#define __TOOLXPRT_H__

#ifdef EXPORT_TOOLSET
	#define TOOLSET_CLASS AFX_EXT_CLASS
//	#define TOOLSET_API   AFX_EXT_API

	#define TOOLSET_API   __declspec(dllexport)
  
  #define TOOLSET_DATA  AFX_EXT_DATA
#else
	#define TOOLSET_CLASS __declspec(dllimport)
	#define TOOLSET_API   __declspec(dllimport)
	#define TOOLSET_DATA  __declspec(dllimport)

#ifndef __TARGET_H__
	#ifdef _DEBUG
		#pragma comment(lib, "toolsetd.lib")
	#else
		#pragma comment(lib, "toolset.lib")
	#endif
#endif
#endif

#endif //__TOOLXPRT_H__
