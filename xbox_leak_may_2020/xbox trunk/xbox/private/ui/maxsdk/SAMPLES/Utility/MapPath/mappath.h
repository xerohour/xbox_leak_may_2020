/**********************************************************************
 *<
	FILE: RefCheck.h

	DESCRIPTION:	Template Utility

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#ifndef __REFCHECK__H
#define __REFCHECK__H

#include "Max.h"
#include "resource.h"
#include "utilapi.h"
#include "istdplug.h"
#include "stdmat.h"
#include "bmmlib.h"
#include <io.h>

#define REFCHECK_CLASS_ID	Class_ID(0xa7d423ed, 0x64de98f9)

extern ClassDesc*	GetRefCheckDesc();
extern HINSTANCE	hInstance;
extern TCHAR*		GetString(int id);

#define COPYWARN_YES		0x00
#define COPYWARN_YESTOALL	0x01
#define COPYWARN_NO			0x02
#define COPYWARN_NOTOALL	0x03

class RefCheck : public UtilityObj {
	public:
		IUtil *iu;
		Interface *ip;
		HWND hPanel;
		HWND hDialog;

		RefCheck();
		~RefCheck();

		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);
		void DeleteThis() {}

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);

		// Main dialog resize functions
		void ResizeWindow(int x, int y);
		void SetMinDialogSize(int w, int h)	{ minDlgWidth = w; minDlgHeight = h; }
		int	 GetMinDialogWidth()			{ return minDlgWidth; }
		int  GetMinDialogHeight()			{ return minDlgHeight; }

		void CheckDependencies();
		void Update();
		void StripSelected();
		void StripAll();
		void DoDialog();
		void DoSelection();
		void SetPath(TCHAR* path, BitmapTex* map);
		void StripMapName(TCHAR* path);
		void BrowseDirectory();
		BOOL ChooseDir(TCHAR *title, TCHAR *dir);
		void EnableEntry(HWND hWnd, BOOL bEnable, int numSel);
		void ShowInfo();
		void HandleInfoDlg(HWND dlg);

		BOOL GetIncludeMatLib();
		BOOL GetIncludeMedit();

		int  CopyWarningPrompt(HWND hParent, TCHAR* filename);
		void CopyMaps();
		void SetActualPath();
		void SelectMissing();

		BOOL FindMap(TCHAR* mapName, TCHAR* newName);

		BOOL GetCopyQuitFlag()	{ return bCopyQuitFlag; }
		void SetCopyQuitFlag(BOOL bStatus)	{ bCopyQuitFlag = bStatus; }
		void SetInfoTex(BitmapTex* b) { infoTex = b; }
		BitmapTex* GetInfoTex() { return infoTex; }

	private:
		BOOL		bCopyQuitFlag;
		BitmapTex*	infoTex;
		int			minDlgWidth;
		int			minDlgHeight;
		//PBITMAPINFO	pDib;
};

#endif // __REFCHECK__H
