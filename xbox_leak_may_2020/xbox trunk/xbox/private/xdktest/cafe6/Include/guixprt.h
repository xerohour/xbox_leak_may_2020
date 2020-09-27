///////////////////////////////////////////////////////////////////////////////
//	guixprt.h (gui export)
//
//	Created by :			Date :
//		BrianCr				01/03/95
//
//	Description :
//		GUI DLL defines
//

#ifndef __GUIXPRT_H__
#define __GUIXPRT_H__

#ifdef EXPORT_GUI
	#define GUI_CLASS AFX_EXT_CLASS
	#define GUI_API   AFX_EXT_API
	#define GUI_DATA  AFX_EXT_DATA
#else
	#define GUI_CLASS __declspec(dllimport)
	#define GUI_API   __declspec(dllimport)
	#define GUI_DATA  __declspec(dllimport)

	#ifdef _DEBUG 
		#pragma comment(lib, "guid.lib")
	#else
		#pragma comment(lib, "gui.lib")
	#endif
#endif

#endif // __GUIXPRT_H__
