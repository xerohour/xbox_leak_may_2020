//
//	Simple applications used for testing browser
//

#include "browse.h"
#include "capp.h"
#include "ctest.h"
#include "cpptest.h"

CApp theApp;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int nCmdShow ) {
	
	if( theApp.InitApplication( hInstance, hPrevInstance, szCmdLine, nCmdShow ) ) {
		if( theApp.InitInstance() ) {
			return theApp.Run();
		}
	}
	ctest();
	cpptest();
	return 0;
}
