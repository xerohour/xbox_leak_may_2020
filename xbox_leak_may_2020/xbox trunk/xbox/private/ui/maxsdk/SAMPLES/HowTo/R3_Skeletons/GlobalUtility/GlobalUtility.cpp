/*===========================================================================*\
 | 
 |  FILE:	GlobalUtility.cpp
 |			Skeleton project and code for a Global Utility Plugin (GUP)
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/

#include "GlobalUtility.h"


/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonGUPClassDesc:public ClassDesc {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return new SkeletonGUP; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return GUP_CLASS_ID; }
	Class_ID 		ClassID()					{ return SKGUP_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }
};

static SkeletonGUPClassDesc SkeletonGUPCD;
ClassDesc* GetGUPDesc() {return &SkeletonGUPCD;}



/*===========================================================================*\
 |	Global Utility interface (start/stop/control)
\*===========================================================================*/

// Activate and Stay Resident
// The GUPRESULT_KEEP tells MAX that we want to remain loaded in the system
// Check the SDK Help File for other returns, to change the behavior
DWORD SkeletonGUP::Start( ) {
	SkeletonViewWindow *skvw = new SkeletonViewWindow(this);
	Max()->RegisterViewWindow(skvw);
	return GUPRESULT_KEEP;
}


// We do nothing when the GUP unloads
void SkeletonGUP::Stop( ) {

}



DWORD SkeletonGUP::Control( DWORD parameter ) {
	return 0;
}