///////////////////////////////////////////////////////////////////////////////
//	edsxprt.h (editors export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		Editors DLL defines
//

#ifndef __EDSXPRT_H__
#define __EDSXPRT_H__

#ifdef EXPORT_EDITORS
	#define EDS_CLASS AFX_EXT_CLASS
	#define EDS_API   AFX_EXT_API
	#define EDS_DATA  AFX_EXT_DATA
#else
	#define EDS_CLASS __declspec(dllimport)
	#define EDS_API   __declspec(dllimport)
	#define EDS_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "edsd.lib")
	#else
		#pragma comment(lib, "eds.lib")
	#endif
#endif

#endif // __EDSXPRT_H__
