/* -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

   FILE: dlgProcs.h

	 DESCRIPTION: dialog procedures, class declarations

	 CREATED BY: michael malone (mjm)

	 HISTORY: created November 4, 1998

	 Copyright (c) 1998, All Rights Reserved

// -----------------------------------------------------------------------------
// -------------------------------------------------------------------------- */

#if !defined(_DLGPROCS_H_INCLUDED_)
#define _DLGPROCS_H_INCLUDED_

// system includes
#include <windows.h>

// maxsdk includes
#include <max.h>
#include <iparamm2.h>

// local includes
#include "globals.h"
#include "resource.h"
class BlurMgr;

#define NAMELENGTH 64
typedef TCHAR TChBuffer[NAMELENGTH];

// -----------------------
// master dialog procedure
// -----------------------
class MasterDlgProc : public ParamMap2UserDlgProc
{
	int numCheckedIn;
	bool mAllDlgsCreated;
	HWND mHWnds[numIDs];

	void placeChildren();

	// from ParamMap2UserDlgProc
	BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void SetThing(ReferenceTarget *m);
	void DeleteThis();

public:
	MasterDlgProc() : numCheckedIn(0), mAllDlgsCreated(false) { mHWnds[0] = mHWnds[1] = mHWnds[2] = NULL; }
	void showWnd(int dlgId) { ShowWindow(mHWnds[dlgId], SW_SHOW); }
	void hideWnd(int dlgId) { ShowWindow(mHWnds[dlgId], SW_HIDE); }
	void checkIn(int dlgId, HWND hDlg);
	void notifyChildrenCreated();
};

// -----------------------
// child dialog procedures
// -----------------------
class BlurDataDlgProc : public ParamMap2UserDlgProc
{
	MasterDlgProc *mpMasterDlgProc;
	void DeleteThis() { }
	BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void enableControls(TimeValue t, IParamMap2* map, HWND hWnd);

public:
	BlurDataDlgProc() : mpMasterDlgProc(NULL) { }
	BlurDataDlgProc(MasterDlgProc *mpMaster) : mpMasterDlgProc(mpMaster) { }
};

class SelDataDlgProc : public ParamMap2UserDlgProc
{
	TChBuffer mChannelStr[5];
	MasterDlgProc *mpMasterDlgProc;
	void DeleteThis() { }
	BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	SelDataDlgProc() : mpMasterDlgProc(NULL) { }
	SelDataDlgProc(MasterDlgProc *mpMaster) : mpMasterDlgProc(mpMaster) { }
};

#endif // !defined(_DLGPROCS_H_INCLUDED_)
