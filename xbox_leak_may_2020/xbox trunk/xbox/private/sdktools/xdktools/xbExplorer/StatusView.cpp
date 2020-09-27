#include "stdafx.h"

STATUSVIEW::STATUSVIEW()
{
	hStatusWnd = NULL;
}

STATUSVIEW::~STATUSVIEW()
{
}

void STATUSVIEW::SetSeparateSize( int size )
{
	sb_size[1] = size;
	sb_size[0] = size / 2;
}

HWND STATUSVIEW::GethWnd()
{
	return hStatusWnd;
}

HWND STATUSVIEW::CreateStatusView( HWND hWndParent )
{
	RECT rect;

	GetClientRect( hWndParent, &rect );
	SetSeparateSize( rect.right );
	hStatusWnd = CreateStatusWindow( WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, NULL, hWndParent, ID_STATUS );
	SendMessage( hStatusWnd, SB_SETPARTS, N_SEPARATE, (LPARAM) sb_size );
	GetClientRect( hStatusWnd, &statusRect );
	if ( ! hStatusWnd ) return NULL;
	return hStatusWnd;
}

HWND STATUSVIEW::RedrawStatusView( HWND hWndParent, WPARAM wParam, LPARAM lParam )
{
	SetSeparateSize( LOWORD( lParam ) );
	SendMessage( hStatusWnd, SB_SETPARTS, N_SEPARATE, (LPARAM) sb_size );
    SendMessage( hStatusWnd, WM_SIZE, wParam, lParam );
	return hStatusWnd;
}

void STATUSVIEW::SetText( int nseparate, LPCTSTR str )
{
	SendMessage( hStatusWnd, SB_SETTEXT, nseparate, (LPARAM) str );
}
