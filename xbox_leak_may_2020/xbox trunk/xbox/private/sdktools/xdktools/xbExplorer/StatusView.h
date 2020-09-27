#ifndef __STATUSVIEW_H__
#define __STATUSVIEW_H__

#define N_SEPARATE 2
#define ID_STATUS 100

enum {
	SELECTED_STATUS,
	TOTAL_STATUS
};

class STATUSVIEW
{
private:
	HWND hStatusWnd;
	RECT statusRect;
	int sb_size[N_SEPARATE];

private:
	void SetSeparateSize( int );

public:
	STATUSVIEW();
	~STATUSVIEW();
	HWND GethWnd();
	HWND CreateStatusView( HWND );
	HWND RedrawStatusView( HWND, WPARAM, LPARAM );
	void SetText( int, LPCTSTR );
};

#endif
