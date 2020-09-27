/*===========================================================================*\
 | 
 |  FILE:	SceneExport.cpp
 |			Skeleton project and code for a Scene Exporter 
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 26-3-99
 | 
\*===========================================================================*/

#include "SceneExport.h"




/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()				{ return TRUE; }
	void *			Create( BOOL loading )	{ return new SkeletonExporter; }
	const TCHAR *	ClassName()				{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()			{ return SCENE_EXPORT_CLASS_ID; }
	Class_ID 		ClassID()				{ return SCEXP_CLASSID; }
	const TCHAR* 	Category()				{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()			{ return _T("SkeletonExporter"); }
	HINSTANCE		HInstance()				{ return hInstance; }
};

static SkeletonExportClassDesc SkeletonExportCD;
ClassDesc* GetSkeletonSceneExportDesc() {return &SkeletonExportCD;}



/*===========================================================================*\
 |	Basic implimentation of a dialog handler
\*===========================================================================*/

static BOOL CALLBACK CustomDialogHandler(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch (msg) 
	{
		case WM_INITDIALOG:
			break;
		case WM_DESTROY:
			break;
		case WM_COMMAND:
			break;
	}
	return FALSE;
}



/*===========================================================================*\
 |  Constructor/Destructor - just initialize any variables or memory
\*===========================================================================*/

SkeletonExporter::SkeletonExporter()
{
	simpleParam = 1;
}

SkeletonExporter::~SkeletonExporter()
{
}

/*===========================================================================*\
 |  Return how many extensions we support, and what they are
\*===========================================================================*/

int SkeletonExporter::ExtCount() { return 1; }

const TCHAR * SkeletonExporter::Ext(int n)
{
	switch(n) {
		case 0:
			return GetString(IDS_EXT_01);
		}
	return _T("");

}


/*===========================================================================*\
 |  Return various information about our scene exporter
\*===========================================================================*/

const TCHAR * SkeletonExporter::LongDesc()
{
	return GetString(IDS_LONGDESC);
}

const TCHAR * SkeletonExporter::ShortDesc()
{
	return GetString(IDS_SHORTDESC);
}

const TCHAR * SkeletonExporter::AuthorName()
{
	return GetString(IDS_AUTHOR);
}

const TCHAR * SkeletonExporter::CopyrightMessage()
{
	return GetString(IDS_COPYRIGHT);
}

const TCHAR * SkeletonExporter::OtherMessage1() { return _T(""); }
const TCHAR * SkeletonExporter::OtherMessage2() { return _T(""); }

// Version number = (version * 100)
unsigned int SkeletonExporter::Version()
{
	return 100;
}



/*===========================================================================*\
 |  Show about box
\*===========================================================================*/

static BOOL CALLBACK AboutDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SkeletonExporter *se = (SkeletonExporter*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!se && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) {
		case WM_INITDIALOG:	
			// Update class pointer
			se = (SkeletonExporter*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			break;

		case WM_DESTROY:
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_OK:
					EndDialog(hWnd,1);
				break;
			}
			break;

		default:
			return FALSE;
		}	
	return TRUE;
	}

void SkeletonExporter::ShowAbout(HWND hWnd)
{
	DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_ABOUT),
		hWnd,
		AboutDlgProc,
		(LPARAM)this);
}
