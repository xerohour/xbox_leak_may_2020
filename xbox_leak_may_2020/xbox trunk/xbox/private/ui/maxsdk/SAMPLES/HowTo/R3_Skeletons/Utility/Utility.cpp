/*===========================================================================*\
 | 
 |  FILE:	Utility.cpp
 |			Skeleton project and code for a Utility
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 9-2-99
 | 
\*===========================================================================*/

#include "Utility.h"


/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonUtilClassDesc:public ClassDesc {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return &theSkeletonUtility; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return UTILITY_CLASS_ID; }
	Class_ID 		ClassID()					{ return SKUTIL_CLASSID; }
	const TCHAR* 	Category()					{ return GetString(IDS_CATEGORY);  }
	void ResetClassParams (BOOL fileReset);
};

static SkeletonUtilClassDesc SkeletonUtilCD;
ClassDesc* GetSkeletonUtilDesc() {return &SkeletonUtilCD;}

// Reset all the utility values on File/Reset
void SkeletonUtilClassDesc::ResetClassParams (BOOL fileReset) 
{
}



/*===========================================================================*\
 |	Dialog Handler for Utility
\*===========================================================================*/

static BOOL CALLBACK DefaultDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theSkeletonUtility.Init(hWnd);
			break;

		case WM_DESTROY:
			theSkeletonUtility.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CLOSE:
					theSkeletonUtility.iu->CloseUtility();
					break;

				case IDC_BTN1:
					MessageBeep(MB_OK);
					break;
			}
			break;


		default:
			return FALSE;
	}
	return TRUE;
}



/*===========================================================================*\
 |  Utility implimentations
\*===========================================================================*/

SkeletonUtility::SkeletonUtility()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
}

SkeletonUtility::~SkeletonUtility()
{

}


void SkeletonUtility::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;

	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_SKELUTIL),
		DefaultDlgProc,
		GetString(IDS_PARAMETERS),
		0);
}
	
void SkeletonUtility::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}


void SkeletonUtility::Init(HWND hWnd)
{

}

void SkeletonUtility::Destroy(HWND hWnd)
{

}

