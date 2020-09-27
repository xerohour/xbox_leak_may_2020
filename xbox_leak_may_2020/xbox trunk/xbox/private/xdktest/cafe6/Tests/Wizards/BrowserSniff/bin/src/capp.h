//
// class CApp 
//

class CApp {
public:
	BOOL InitApplication( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int nCmdShow );
	BOOL InitInstance();
	int Run();

private:
	HINSTANCE m_hInstance;
	HINSTANCE m_hPrevInstance;
	LPTSTR m_szCmdLine;
	int m_nCmdShow ;
	HWND m_hWnd;
};