////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// FILE:      library\common\pspages.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   07.11.96
//
// PURPOSE:   Class for common Property Sheet Pages and such for use in Device Manager (Control Panel)
//
// HISTORY:


// Remarks:
//
// You need to add two include paths: 1) to the "inc16" directory of the Windows 95 DDK
//                                    2) to the "include" directory of the Win32 SDK (in that order)
//
// Do not use the standard MSVC include paths!
//
// You also must link the following two lib files from the Windows 95 DDK (or add them to the linker settings):
//
//     setupx.lib
//     commctrl.lib
//
// The constructors need not only the resource ID of the dialog template, but also a special "titleResID".
// For property sheet pages of "PropertySheetPage" type, it is the ID of the small (16x16) icon to appear in
// the page's title.
// For "DevManPropertySheetPage"s it is the first of three consecutive resource IDs:
//
//     1) IDI_TITLE (like for "PropertySheetPage")
//     2) IDI_CLASS_ICON = IDI_TITLE + 1 (ID of icon in the dialog template that identifies the class type)
//     3) IDC_DEVICE_NAME = IDI_TITLE + 2 (ID of device name text in the dialog template).


#ifndef PSPAGES_H
#define PSPAGES_H

#define STRICT

#define WINVER 0x0400

#include <windows.h>
#include <windowsx.h>
#include <winerror.h>		// Comes from the Win32SDK!
extern "C"						// Necessary to correctly link setupx functions.
{
#include <setupx.h>
}

// Prevents inclusion of <stdlib.h>
#include "library\common\gnerrors.h"

class PropertySheetPage
	{
	private:
		friend BOOL FAR PASCAL __loadds DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
		friend UINT FAR PASCAL __loadds PageCallback(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE page);
	protected:
		HINSTANCE		hinst;
		HPROPSHEETPAGE	hPage;
		int				count;
		int				resourceID;
		int				titleResID;
		HWND				hDlg;				// Handle that is sent with the DialogProc(), should be accessed in the message
												// and notification handlers only.

		virtual BOOL	DialogProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

		// Message handlers
		virtual LRESULT OnInitDialog(void) {return 0;}
		virtual LRESULT OnCommand(int idItem, HWND hWndCtrl, WORD notifyCode) {return 0;}
		virtual LRESULT OnNotify(int idFrom, LPNMHDR lpnmhdr);
		virtual LRESULT OnClose() {return 0;}
		virtual LRESULT OnDestroy() {return 0;}
		virtual LRESULT OnVScroll(WORD code, HWND hWndCtrl, int pos) {return 0;}
		virtual LRESULT OnHScroll(WORD code, HWND hWndCtrl, int pos) {return 0;}

		// Property Sheet Page Notification handlers
		virtual LRESULT OnPSNSetActive(LPNMHDR lpnmhdr) {return 0;}
		virtual LRESULT OnPSNKillActive(LPNMHDR lpnmhdr) {return 0;}
		virtual LRESULT OnPSNQueryCancel(LPNMHDR lpnmhdr) {return FALSE;}
		virtual LRESULT OnPSNApply(LPNMHDR lpnmhdr) {return PSNRET_NOERROR;}
		virtual LRESULT OnPSNReset(LPNMHDR lpnmhdr) {return FALSE;}
		virtual LRESULT OnPSNHelp(LPNMHDR lpnmhdr) {return FALSE;}

	public:
		PropertySheetPage(HINSTANCE hinst, int resourceID, int titleResID)
			{this->hinst = hinst; this->resourceID = resourceID; this->titleResID = titleResID; hPage = NULL; count = 0;}

      virtual ~PropertySheetPage(void) {}

		Error CreatePage(void);
		Error DestroyPage(void);
	};

class DevManPropertySheetPage : public PropertySheetPage
	{
	private:
		LPFNADDPROPSHEETPAGE	AddPageProc;
		LPARAM 					lParam;

		HICON						classIcon;

	protected:
		LPDEVICE_INFO			deviceInfo;

		LRESULT OnInitDialog(void);						// Call this from any successor if you want to get the class icon
																	// and device name automatically (IDI_CLASS_ICON and IDC_DEVICE_NAME).
	public:
		DevManPropertySheetPage(HINSTANCE				hinst,
										int 						resourceID,
										int						titleResID,
										LPDEVICE_INFO			deviceInfo,
		 								LPFNADDPROPSHEETPAGE	AddPageProc,
		 								LPARAM					lParam);

		Error AddPage(void);
	};

#endif
