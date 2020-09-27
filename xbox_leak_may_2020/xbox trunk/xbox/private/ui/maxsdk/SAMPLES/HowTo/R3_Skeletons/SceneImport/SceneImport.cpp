/*===========================================================================*\
 | 
 |  FILE:	SceneImport.cpp
 |			Skeleton project and code for a Scene Importer 
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 29-3-99
 | 
\*===========================================================================*/

#include "SceneImport.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonImportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()				{ return TRUE; }
	void *			Create( BOOL loading )	{ return new SkeletonImporter; }
	const TCHAR *	ClassName()				{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()			{ return SCENE_IMPORT_CLASS_ID; }
	Class_ID 		ClassID()				{ return SCIMP_CLASSID; }
	const TCHAR* 	Category()				{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()			{ return _T("SkeletonImporter"); }
	HINSTANCE		HInstance()				{ return hInstance; }
};

static SkeletonImportClassDesc SkeletonImportCD;
ClassDesc* GetSkeletonSceneImportDesc() {return &SkeletonImportCD;}



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

SkeletonImporter::SkeletonImporter()
{
	simpleParam = 1;
}

SkeletonImporter::~SkeletonImporter()
{
}

/*===========================================================================*\
 |  Return how many extensions we support, and what they are
\*===========================================================================*/

int SkeletonImporter::ExtCount() { return 1; }

const TCHAR * SkeletonImporter::Ext(int n)
{
	switch(n) {
		case 0:
			return GetString(IDS_EXT_01);
		}
	return _T("");

}


/*===========================================================================*\
 |  Return various information about our scene importer
\*===========================================================================*/

const TCHAR * SkeletonImporter::LongDesc()
{
	return GetString(IDS_LONGDESC);
}

const TCHAR * SkeletonImporter::ShortDesc()
{
	return GetString(IDS_SHORTDESC);
}

const TCHAR * SkeletonImporter::AuthorName()
{
	return GetString(IDS_AUTHOR);
}

const TCHAR * SkeletonImporter::CopyrightMessage()
{
	return GetString(IDS_COPYRIGHT);
}

const TCHAR * SkeletonImporter::OtherMessage1() { return _T(""); }
const TCHAR * SkeletonImporter::OtherMessage2() { return _T(""); }

// Version number = (version * 100)
unsigned int SkeletonImporter::Version()
{
	return 100;
}



/*===========================================================================*\
 |  Show about box
\*===========================================================================*/

static BOOL CALLBACK AboutDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SkeletonImporter *si = (SkeletonImporter*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!si && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) {
		case WM_INITDIALOG:	
			// Update class pointer
			si = (SkeletonImporter*)lParam;
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

void SkeletonImporter::ShowAbout(HWND hWnd)
{
	DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_ABOUT),
		hWnd,
		AboutDlgProc,
		(LPARAM)this);
}
