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


// FILE:      library\common\dialog.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   09.05.95
//
// PURPOSE: The base class for requesters.
//
// HISTORY:

#ifndef DIALOG_H
#define DIALOG_H

#include <windows.h>
#include <commdlg.h>
#include "library\common\prelude.h"
#include "library\common\gnerrors.h"



class Slider
	{
	friend class Dialog;

	private:
		WORD value;
		WORD min, max;
		WORD lineStep, pageStep;
		HWND slider;
		int resourceID;

	public:
		Slider (HWND hDlg, int resourceID);
		Slider (void) {}

		void Init (HWND hDlg, int resourceID, WORD value,   // with implicit drawing!
		           WORD min, WORD max, WORD lineStep, WORD pageStep);

		void SetRange (WORD min, WORD max, WORD lineStep, WORD pageStep);

		void SetValue (WORD value);   // with implicit drawing!
		WORD GetValue (void) {return value;}

		int GetID (void) {return resourceID;}

		WORD Move (WPARAM wParam, LPARAM lParam);

		void Show (BOOL showIt);
	};



class Dialog
	{
	private:
		friend BOOL FAR PASCAL __loadds DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
		HBRUSH		hbrGrey;

	protected:
		HINSTANCE	hinst;

		HWND			hDlg;	   	// dialog handle
		int			resourceID;
   	BOOL			modal;

		virtual BOOL DialogProc (UINT msg, WPARAM wParam, LPARAM lParam);
			// Implemented by derived classes. Return if message was processed.
		virtual Error InitDialog(void) {GNRAISE_OK;}
		virtual Error DoneDialog(void) {GNRAISE_OK;}
		virtual Error CancelDialog(void) {GNRAISE_OK;}
		virtual Error InitMove(int xu, int yu);

      virtual Error Paint(HDC hdc) {GNRAISE_OK;}

      Error InitSlider(Slider & slider, WORD value, WORD min, WORD max,
		                                  WORD lineStep, WORD pageStep,
		                                  int resourceID);

		WORD MoveSlider(Slider & slider, WPARAM wParam, LPARAM lParam);
	public:
		Dialog(HINSTANCE	hinst, int resourceID)
			{this->hinst = hinst; this->resourceID = resourceID; hDlg = NULL; hbrGrey = NULL;}

		Error OpenDialog(void);
		Error CloseDialog(void);

		Error DoDialog(void);
	};



#endif
