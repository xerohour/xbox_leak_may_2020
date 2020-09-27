// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//	 FILE: dlgProcs.cpp
//
//	 DESCRIPTION: dialog procedures - class definitions
//
//	 CREATED BY: michael malone (mjm)
//
//	 HISTORY: created November 4, 1998
//
//	 Copyright (c) 1998, All Rights Reserved
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// precompiled header
#include "pch.h"

// local includes
#include "pch.h"
#include "dllMain.h"
#include "dlgProcs.h"
#include "blurMgr.h"

void MasterDlgProc::checkIn(int id, HWND hDlg)
{
	mHWnds[id] = hDlg;
	numCheckedIn++;
	mAllDlgsCreated = (numCheckedIn == numIDs) ? true : false;
}

void MasterDlgProc::placeChildren()
{
	// compute locations for child dialogs
	Rect rc;
	GetWindowRect(mHWnds[idMaster], &rc);
	int originX = rc.left;
	int originY = rc.top;

	// find display area of tab control
	HWND hTabCtrl = GetDlgItem(mHWnds[idMaster], IDC_TAB);
	GetWindowRect(hTabCtrl, &rc);
	TabCtrl_AdjustRect(hTabCtrl, FALSE, &rc);

	// convert to client coordinates
	rc.left -= originX; rc.right  -= originX;
	rc.top  -= originY; rc.bottom -= originY;

	// move child dialogs into place
	MoveWindow(mHWnds[idBlurData], rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
	MoveWindow(mHWnds[idSelData],  rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
}

void MasterDlgProc::notifyChildrenCreated()
{
	assert(mAllDlgsCreated);

	// setup tab control
	HWND hTabCtrl = GetDlgItem(mHWnds[idMaster], IDC_TAB);
	TC_ITEM tci;
	tci.mask = TCIF_TEXT;
	tci.iImage = -1;
	tci.pszText = GetString(IDS_BLUR_TYPE);
	TabCtrl_InsertItem(hTabCtrl, 0, &tci);
	tci.pszText = GetString(IDS_SEL_TYPES);
	TabCtrl_InsertItem(hTabCtrl, 1, &tci);

	// position child dialogs
	placeChildren();

	// show appropriate window
	switch( TabCtrl_GetCurSel( GetDlgItem(mHWnds[idMaster], IDC_TAB) ) )
	{
		case 0:
			ShowWindow(mHWnds[idSelData],  SW_HIDE);
			ShowWindow(mHWnds[idBlurData], SW_SHOW);
			break;
		case 1:
			ShowWindow(mHWnds[idBlurData], SW_HIDE);
			ShowWindow(mHWnds[idSelData],  SW_SHOW);
			break;
	}
}

void MasterDlgProc::SetThing(ReferenceTarget *m)
{
	// parammap is being recycled - need to manually set the new blur's child paramblocks into the child parammaps
	BlurMgr *newBlur = (BlurMgr *)m;
	newBlur->pmBlurData->SetParamBlock(newBlur->pbBlurData);
	newBlur->pmSelData->SetParamBlock(newBlur->pbSelData);
}

void MasterDlgProc::DeleteThis()
{
	numCheckedIn = 0;
	mAllDlgsCreated = false;
	mHWnds[0] = mHWnds[1] = mHWnds[2] = NULL;
}

BOOL MasterDlgProc::DlgProc(TimeValue t, IParamMap2* map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);

	switch(msg)
	{
		case WM_INITDIALOG:
			checkIn(idMaster, hWnd);
			return FALSE;

		case WM_NOTIFY:
		{
			NMHDR* pnmhdr = (LPNMHDR)lParam;
			HWND hTabCtrl = GetDlgItem(hWnd, IDC_TAB);

			switch(pnmhdr->code)
			{
				case TCN_SELCHANGING:
					switch(TabCtrl_GetCurSel(hTabCtrl))
					{
					case 0:
						hideWnd(idBlurData);
						break;
					case 1:
						hideWnd(idSelData);
						break;
					}
					break;

				case TCN_SELCHANGE:
					switch(TabCtrl_GetCurSel(hTabCtrl))
					{
					case 0:
						showWnd(idBlurData);
						break;
					case 1:
						showWnd(idSelData);
						break;
					}
					break;

				default:
					return FALSE;
			}
			break;
		}

		default:
			return FALSE;
	}
	return TRUE;
}


// ---------------
// BlurTypeDlgProc
// ---------------
void BlurDataDlgProc::enableControls(TimeValue t, IParamMap2* map, HWND hWnd)
{
	int blurType;
	BOOL state;

	map->GetParamBlock()->GetValue(prmBlurType, t, blurType, FOREVER);
	switch (blurType)
	{
		case idBlurUnif:
		case idBlurDir:
			EnableWindow(GetDlgItem(hWnd, IDB_BRADIAL_CLEAR_NODE), FALSE);
			break;
		case idBlurRadial:
			state = ( IsDlgButtonChecked(hWnd, IDC_BRADIAL_USE_NODE) != BST_CHECKED );
			map->Enable( prmRadialXOrig, state );
			map->Enable( prmRadialYOrig, state );
			map->Enable( prmRadialNode, !state );
			EnableWindow(GetDlgItem(hWnd, IDB_BRADIAL_CLEAR_NODE), !state);
			break;
	}
}

BOOL BlurDataDlgProc::DlgProc(TimeValue t, IParamMap2* map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			mpMasterDlgProc->checkIn(idBlurData, hWnd);
			map->SetTooltip( prmRadialNode, TRUE, GetString(IDS_BRADIAL_NODE_PROMPT) );
			break;

		case WM_SHOWWINDOW:
			enableControls(t, map, hWnd);
			break;

		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
				case IDR_BLUR_UNIF:
				case IDR_BLUR_DIR:
					enableControls(t, map, hWnd);
					break;

				case IDR_BLUR_RADIAL:
				case IDC_BRADIAL_USE_OBJECT:
					enableControls(t, map, hWnd);
					break;

				case IDB_BRADIAL_CLEAR_NODE:
					if ( HIWORD(wParam) == BN_CLICKED )
						map->GetParamBlock()->SetValue(prmRadialNode, 0, (INode *)NULL); // not animatable -- use time = 0
					break;

				default:
					return FALSE;
			}
			break;

		case WM_DESTROY:
			GetCOREInterface()->UnRegisterDlgWnd(hWnd); // this should be handled automatically, but currently is not.
			break;

		default:
			return FALSE;
	}
	return TRUE;
}


// --------------
// SelTypeDlgProc
// --------------
#define NUMCHANNELS 5

BOOL SelDataDlgProc::DlgProc(TimeValue t, IParamMap2* map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{
			mpMasterDlgProc->checkIn(idSelData, hWnd);

			// initialize strings for combo boxes
			_tcscpy( mChannelStr[0], GetString(IDS_RED) );
			_tcscpy( mChannelStr[1], GetString(IDS_GREEN));
			_tcscpy( mChannelStr[2], GetString(IDS_BLUE));
			_tcscpy( mChannelStr[3], GetString(IDS_ALPHA));
			_tcscpy( mChannelStr[4], GetString(IDS_LUMINANCE));

			// insert strings into listbox
			HWND hwndMap = GetDlgItem(hWnd, IDCB_SMASK_CHANNEL);  
			for (int index=0; index<NUMCHANNELS; index++)
				SendMessage( hwndMap, CB_ADDSTRING, 0, (LPARAM)mChannelStr[index] );
			break;
		}

		case WM_SHOWWINDOW:
		{
			int curIndex;
			map->GetParamBlock()->GetValue(prmMaskChannel, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDCB_SMASK_CHANNEL), CB_SETCURSEL, (WPARAM)curIndex, 0);

			// comboboxes not supported by parammap2 so manually set enable/disable for this control
			BOOL checked = ( SendMessage(GetDlgItem(hWnd, IDC_SMASK_ACTIVE), BM_GETCHECK, 0, 0) == BST_CHECKED ) ? TRUE : FALSE;
			EnableWindow(GetDlgItem(hWnd, IDCB_SMASK_CHANNEL), checked);
			break;
		}

		case WM_COMMAND:
			if ( (LOWORD(wParam) == IDC_SMASK_ACTIVE) && (HIWORD(wParam) == BN_CLICKED) )
			{
				BOOL checked = ( SendMessage(GetDlgItem(hWnd, IDC_SMASK_ACTIVE), BM_GETCHECK, 0, 0) == BST_CHECKED ) ? TRUE : FALSE;
				EnableWindow(GetDlgItem(hWnd, IDCB_SMASK_CHANNEL), checked);
				break;
			}

			else if ( (LOWORD(wParam) == IDCB_SMASK_CHANNEL) && (HIWORD(wParam) == CBN_SELCHANGE) )
			{
				HWND hListBox = (HWND)lParam;
				int index = SendMessage(hListBox, CB_GETCURSEL, 0, 0);
				map->GetParamBlock()->SetValue(prmMaskChannel, GetCOREInterface()->GetTime(), index);
				break;
			}

			else
				return FALSE;

		case WM_DESTROY:
			GetCOREInterface()->UnRegisterDlgWnd(hWnd); // this should be handled automatically, but currently is not.
			break;

		default:
			return FALSE;
	}
	return TRUE;
}
