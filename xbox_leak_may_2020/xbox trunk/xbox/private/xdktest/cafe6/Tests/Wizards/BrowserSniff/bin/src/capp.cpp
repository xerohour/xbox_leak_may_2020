
#include "browse.h"
#include "capp.h"

const LPCSTR szClassName = "BrowseApp";
const LPCSTR szAppName = "Browser Test";

LRESULT WINAPI WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	switch( uMsg ) {
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;

		default:
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

BOOL CApp::InitApplication( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int nCmdShow ) {
	m_hInstance = hInstance;
	m_hPrevInstance = hPrevInstance;
	m_szCmdLine = szCmdLine;
	m_nCmdShow = nCmdShow ;

	// register class
	WNDCLASS wndClass;

	wndClass.style = CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION); // blank
	wndClass.hCursor = LoadCursor( m_hInstance, IDC_ARROW );
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szClassName;

	return !!::RegisterClass( &wndClass );
}


BOOL CApp::InitInstance() {
	// create and show window
	HWND hWnd = CreateWindow( szClassName, 
		szAppName,
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		m_hInstance,
		0 );

	m_hWnd = hWnd;

	if( hWnd ) {
		ShowWindow( hWnd, m_nCmdShow );
		UpdateWindow( hWnd );
		return TRUE;
	}

	return FALSE;
}


int CApp::Run() {
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return msg.wParam;
}
