/*===========================================================================*\
 | 
 |  FILE:	ColorPicker.h
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

#ifndef __COLPSKEL__H
#define __COLPSKEL__H

#include "max.h"
#include <bmmlib.h>
#include <math.h>
#include "gamma.h"
#include "gport.h"
#include "hsv.h"

#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SKELCP_CLASSID		Class_ID(0x7d209b8, 0x41210287)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonCPDesc();

class SkeletonModeless;



/*===========================================================================*\
 |	SkeletonCPDialog class
\*===========================================================================*/

class SkeletonCPDialog
{

public:
	HWND	hwOwner;
	HWND	hwPanel;
	
	DWORD	origRGB;
    DWORD	currentRGB;

	IPoint2 curPos;
	TSTR	colName;

	HSVCallback *callback;
	static IPoint2 initPos;
	BOOL	objColor;
	SkeletonModeless *sm;


	// Modeless and Modal support
	BOOL	Modeless;
	int		StartModal();
	HWND	StartModeless();


	// Construct and initialize
	SkeletonCPDialog(HWND hOwner, DWORD col,  IPoint2* pos,
			HSVCallback *cb, TCHAR *name, int objClr = 0, SkeletonModeless *smp = NULL);
	~SkeletonCPDialog();
	void DoPaint(HWND hWnd);


	// Modify the dialog's settings
	void SetNewColor (DWORD color, TCHAR *name);
	void ModifyColor (DWORD color);
	DWORD GetColor();
	IPoint2 GetPosition();
	void InstallNewCB(DWORD col, HSVCallback *pcb, TCHAR *name);
};

// The dialog proc
BOOL CALLBACK SkelDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*===========================================================================*\
 |	SkeletonColorPicker class definition
\*===========================================================================*/


class SkeletonColorPicker: public ColPick 
{
public:

	SkeletonColorPicker() {}
	~SkeletonColorPicker() {}

	// Do Modal dialog
	int ModalColorPicker(
		HWND hwndOwner, 		// owning window
		DWORD *lpc,				// pointer to color to be edited
	    IPoint2 *spos, 			// starting position, set to ending position
	    HSVCallback *callBack,	// called when color changes
		TCHAR *name				// name of color being edited
	    );

	// Create Modeless dialog.
	ColorPicker *CreateColorPicker(
		HWND hwndOwner,   
		DWORD initColor,  
		IPoint2* spos,    
		HSVCallback *pcallback,
		TCHAR *name, 	  
		BOOL isObjectColor);


	const TCHAR *	ClassName()		{ return GetString(IDS_CLASSNAME);	}
	Class_ID 		ClassID()		{ return SKELCP_CLASSID; }

	void DeleteThis() {	delete this; }

	int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) {return 0; } 
};



/*===========================================================================*\
 |	SkeletonModeless class definition
\*===========================================================================*/

class SkeletonModeless: public ColorPicker 
{

	HWND hwnd;
	SkeletonCPDialog *SCPD;

public:
		SkeletonModeless(HWND hwndOwner, DWORD initColor, IPoint2* pos, HSVCallback* callback, TCHAR *name, int objClr);

		~SkeletonModeless();
		void DeleteThis() { delete this; }

		void SetNewColor (DWORD color, TCHAR *name);
		void ModifyColor (DWORD color);
		void InstallNewCB(DWORD col, HSVCallback *pcb, TCHAR *name);

		DWORD GetColor();
		IPoint2 GetPosition();
		void Destroy();
};


#endif
