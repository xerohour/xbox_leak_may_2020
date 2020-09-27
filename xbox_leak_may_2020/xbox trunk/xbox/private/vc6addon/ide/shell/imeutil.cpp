#include "stdafx.h"
#pragma hdrstop
#include <winnls32.h>
#include <string.h>
#include <ime32.h>
#include "imeutil.h"

/*
**	FUNCTION:		imeMoveConvertWin
**
**	PURPOSE:		Move the IME conversion window.
**
**	NOTES:
*/

void imeMoveConvertWin (HWND hWnd, int xp, int yp)
{
	theIME.MoveConversionWin( CWnd::FromHandle(hWnd), xp, yp );
}

/*
**	FUNCTION:		imeFont
**
**	PURPOSE:		Notify the IME of a change in font.
**
**	NOTES:
*/

void imeChangeFont (HWND hWnd, HFONT hFont)
{
	theIME.SetFont( CWnd::FromHandle(hWnd), hFont );
}


/*
**	FUNCTION:		imeInit
**
**	PURPOSE:		
**
**	NOTES:		
*/

void imeInit ()
{
	// This is a no-op
}


/*
**	FUNCTION:		imeEnableIME
**
**	PURPOSE:		Enable/Disable the IME.
**
**	NOTES:		
*/

BOOL 
imeEnableIME (HWND hWnd, BOOL bEnable)
{
	return theIME.EnableIME( bEnable );
}


/*
**	FUNCTION:		imeFlush
**
**	PURPOSE:		Flush undermined string from the IME
**
**	NOTES:		
*/
void imeFlush( HWND hWnd )
{
	theIME.Flush( CWnd::FromHandle( hWnd ) );
}

BOOL imeIsOpen()
{
	return theIME.IsOpen();
}

BOOL imeIsEnabled()
{
	return theIME.IsEnabled();
}

BOOL imeIsActive()
{
	return theIME.IsActive();
}

// CIME:
CIME theIME;

CIME::CIME()
{
	if (_getmbcp() == 932)		// Japanese code page
	{
		m_hIMEData = GlobalAlloc( GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, sizeof(IMESTRUCT) );
		m_pIMEData = (LPIMESTRUCT) GlobalLock( m_hIMEData );
		GlobalUnlock( m_hIMEData );
		m_hNLS = LoadLibrary( "USER32.DLL" );
		ASSERT( m_hNLS );

		m_lpfnSendIMEMessage = (WORD (FAR WINAPI *)(HWND, LONG))GetProcAddress(m_hNLS, "SendIMEMessageExA");
		m_lpfnEnableIME = (BOOL (FAR WINAPI *)(HWND, BOOL))GetProcAddress(m_hNLS, "WINNLSEnableIME");
		m_lpfnGetEnableStatus = (BOOL (FAR WINAPI *)(HWND))GetProcAddress(m_hNLS, "WINNLSGetEnableStatus");
	}
	else		// non-Japanese code pages may be DBCS-enabled, but are not IME-enabled
	{
		m_hIMEData = NULL;
		m_pIMEData = NULL;
		m_hNLS = NULL;
		m_lpfnSendIMEMessage = NULL;
		m_lpfnEnableIME = NULL;
		m_lpfnGetEnableStatus = NULL;
	}
}

CIME::~CIME()
{
	if (m_hNLS)
		FreeLibrary( m_hNLS);
	if (m_hIMEData)
		GlobalFree(m_hIMEData);
}

void CIME::MoveConversionWin( CWnd *pWnd, int x, int y )
{
	if (m_pIMEData != NULL)
	{
		m_pIMEData->fnc = IME_SETCONVERSIONWINDOW;
		m_pIMEData->wParam = ((x == -1) && (y == -1)) ? MCW_DEFAULT : MCW_WINDOW;
		m_pIMEData->lParam1 = MAKELONG(x, y);
		m_xPix = x;
		m_yPix = y;

		if (m_lpfnSendIMEMessage != NULL)
			(*(m_lpfnSendIMEMessage))(pWnd->GetSafeHwnd(), (LPARAM) m_hIMEData);
	}
}

void CIME::SetFont( CWnd *pWnd, HFONT hFont )
{
	LOGFONT lf;

	GetObject(hFont, sizeof(LOGFONT), &lf);
	if (lf.lfCharSet != SHIFTJIS_CHARSET)		// disable the ime
	{
		EnableIME( FALSE );
	}
	else
	{
		if (m_pIMEData != NULL)
		{
			HGLOBAL hlf;		// Need this for IME call!
			LOGFONT *plf;

			hlf = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(LOGFONT));
			plf = (LOGFONT *)GlobalLock(hlf);
			memcpy((void *)plf, (void *)&lf, sizeof(LOGFONT));
			EnableIME( TRUE );			// make sure it's enabled
			m_pIMEData->fnc = IME_SETCONVERSIONFONTEX;
			m_pIMEData->lParam1 = (LPARAM)hlf;

			if (m_lpfnSendIMEMessage != NULL)
				(*(m_lpfnSendIMEMessage))( pWnd->GetSafeHwnd(), (LPARAM)m_hIMEData );

			GlobalUnlock(hlf);
			GlobalFree(hlf);
		}
	}
}

static BOOL g_bIMEEnabled = TRUE;
BOOL CIME::EnableIME( BOOL bEnable )
{
	g_bIMEEnabled = bEnable;
	if (m_lpfnEnableIME != NULL)
		return (*(m_lpfnEnableIME))(NULL, bEnable);
	else
		return FALSE;
}

void CIME::Flush( CWnd *pWnd )
{
	if (m_pIMEData != NULL)
	{
		m_pIMEData->fnc = IME_SENDVKEY;
		m_pIMEData->wParam = VK_DBE_FLUSHSTRING;

		if (m_lpfnSendIMEMessage != NULL)
			(*(m_lpfnSendIMEMessage))(pWnd->GetSafeHwnd(), (LPARAM)m_hIMEData);
	}
}

BOOL CIME::IsOpen()
{
	if (m_pIMEData != NULL)
	{
		m_pIMEData->fnc = IME_GETOPEN;

		if (m_lpfnSendIMEMessage != NULL)
			return ((*(m_lpfnSendIMEMessage))( ::GetFocus(), (LPARAM)m_hIMEData) != 0);
	}

	return FALSE;
}

BOOL CIME::IsEnabled()
{
	if (m_lpfnGetEnableStatus != NULL)
		return g_bIMEEnabled;
	else
		return FALSE;
}

BOOL CIME::IsActive()
{
	return IsOpen() && IsEnabled();
}
