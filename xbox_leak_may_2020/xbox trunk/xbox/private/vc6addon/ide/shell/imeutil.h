
/*
**	FILE:	imeutil.h
*/

#ifndef __IMEUTIL_H
#define __IMEUTIL_H

#include "ime32.h"

// Prototypes
void imeInit (void);

// Class
class CIME
{
// Construction
public:
	CIME();
	~CIME();

// Attributes
protected:
	HANDLE m_hIMEData;
	LPIMESTRUCT m_pIMEData;
	HINSTANCE m_hNLS;
	WORD (FAR WINAPI *m_lpfnSendIMEMessage)(HWND, LONG);
	BOOL (FAR WINAPI *m_lpfnEnableIME)(HWND, BOOL);
	BOOL (FAR WINAPI *m_lpfnGetEnableStatus)(HWND);
public:
	int m_xPix;
	int m_yPix;

// Operations
public:
	void MoveConversionWin( CWnd *pWnd, int x, int y );
	void SetFont( CWnd *pWnd, HFONT hFont );
	BOOL EnableIME( BOOL );
	void Flush( CWnd *pWnd );
	BOOL IsOpen();
	BOOL IsEnabled();
	BOOL IsActive();
};

extern CIME theIME;

#endif
