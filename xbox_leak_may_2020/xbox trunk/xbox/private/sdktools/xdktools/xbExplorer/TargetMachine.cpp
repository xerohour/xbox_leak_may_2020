#include "stdafx.h"

LRESULT CALLBACK TargetDlg( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		SetDlgItemText( hDlg, IDC_TARGETNAME, g_TargetXbox );
		CenterDialog( hDlg );
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDOK:
			GetDlgItemText( hDlg, IDC_TARGETNAME, g_TargetXbox, sizeof(g_TargetXbox) );
		case IDCANCEL:
			EndDialog( hDlg, LOWORD( wParam ) );
			return TRUE;
		}
		break;
	}
	return FALSE;
}
