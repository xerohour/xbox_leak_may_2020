#include "stdafx.h"

LRESULT CALLBACK ViewOptionDlg( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		if ( splashscreen ) CheckDlgButton( hDlg, IDC_SHOWSPLASH, BST_CHECKED );
		CenterDialog( hDlg );
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDOK:
			if ( IsDlgButtonChecked( hDlg, IDC_SHOWSPLASH ) == BST_CHECKED ) splashscreen = TRUE;
			else splashscreen = FALSE;
		case IDCANCEL:
			EndDialog( hDlg, TRUE );
			return TRUE;
		}
		break;
	}
	return FALSE;
}
