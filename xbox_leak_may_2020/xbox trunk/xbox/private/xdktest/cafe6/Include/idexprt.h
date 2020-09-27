///////////////////////////////////////////////////////////////////////////////
//	idexprt.h (ide export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		IDE DLL defines
//

#ifndef __IDEXPRT_H__
#define __IDEXPRT_H__

#ifdef EXPORT_IDE
	#define IDE_CLASS AFX_EXT_CLASS
	#define IDE_API   AFX_EXT_API
	#define IDE_DATA  AFX_EXT_DATA
#else
	#define IDE_CLASS __declspec(dllimport)
	#define IDE_API   __declspec(dllimport)
	#define IDE_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "ided.lib")
	#else
		#pragma comment(lib, "ide.lib")
	#endif
#endif

#endif // __GUIXPRT_H__
