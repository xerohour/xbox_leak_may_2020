/*===========================================================================*\
 | 
 |  FILE:	ColorPicker.cpp
 |			Skeleton project and code for a plugin ColorPicker
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 27-1-99
 | 
\*===========================================================================*/

#include "ColorPicker.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class ColPickClassDesc:public ClassDesc {
	public:
	int 			IsPublic()				{ return TRUE; }
	void *			Create(BOOL loading)	{ return new SkeletonColorPicker; }
	const TCHAR *	ClassName()				{ return GetString(IDS_CLASSNAME); 	}
	SClass_ID		SuperClassID()			{ return COLPICK_CLASS_ID; }
	Class_ID 		ClassID()				{ return SKELCP_CLASSID; }
	const TCHAR* 	Category()				{ return _T("");  }
	};

ColPickClassDesc theColPickClassDesc;

ClassDesc *GetSkeletonCPDesc() { return &theColPickClassDesc; }




/*===========================================================================*\
 |	ColorPicker methods
\*===========================================================================*/


ColorPicker* SkeletonColorPicker::CreateColorPicker(HWND hwndOwner, DWORD initColor,
													IPoint2* spos, HSVCallback *pcallback,
													TCHAR *name, int objClr) 
{
	return new SkeletonModeless(hwndOwner,initColor,spos,pcallback,name,objClr);
}	


int SkeletonColorPicker::ModalColorPicker(HWND hwndOwner, DWORD *lpc, IPoint2 *spos,
										  HSVCallback *callBack, TCHAR *name) 
{
	int res = 0;
	SkeletonCPDialog SCPD(hwndOwner,*lpc,spos,callBack,name);
	res = SCPD.StartModal();

	if(lpc)		*lpc = SCPD.GetColor();
	if(spos)	*spos = SCPD.GetPosition();

	return res;
}



/*===========================================================================*\
 |	SkeletonModeless methods
\*===========================================================================*/


SkeletonModeless::SkeletonModeless(HWND hwndOwner, DWORD initColor, 
								   IPoint2* pos, HSVCallback* callback, 
								   TCHAR *name, int objClr)
{
	hwnd = NULL; SCPD = NULL;
	SCPD = new SkeletonCPDialog(hwndOwner,initColor,pos,callback,name,objClr,this);
	hwnd = SCPD->StartModeless();
}


void SkeletonModeless::Destroy() {
	if(hwnd) DestroyWindow(hwnd);
}

SkeletonModeless::~SkeletonModeless() {
	Destroy();
	if(SCPD) delete SCPD;
	hwnd = NULL; SCPD = NULL;
}


// Transactions with the dialog handler
void SkeletonModeless::SetNewColor (DWORD color, TCHAR *name)
{
	SCPD->SetNewColor(color,name);
}

void SkeletonModeless::ModifyColor (DWORD color)
{
	SCPD->ModifyColor(color);
}

void SkeletonModeless::InstallNewCB(DWORD col, HSVCallback *pcb, TCHAR *name)
{
	SCPD->InstallNewCB(col,pcb,name);
}

DWORD SkeletonModeless::GetColor()
{
	return SCPD->GetColor();
}

IPoint2 SkeletonModeless::GetPosition()
{
	return SCPD->GetPosition();
}


IPoint2 SkeletonCPDialog::initPos(354,94);
