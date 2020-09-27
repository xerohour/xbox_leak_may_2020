// first line of source file


#include <xtl.h>


LPARAM CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


int gintVar = 0;
int gintDenominator = 1;
HWND ghWnd;


void Func(void)

{	// first line of Func().

	gintVar = 1;
	while(0);	// line after expression bp hit.
}


void FuncCalledIndirectly(void)
{	// first line of FuncCalledIndirectly().
}


int InnerFunc(void)
{	// first line of InnerFunc().
	return 0;
}


void OuterFunc(int i)
{	// first line of OuterFunc().
}


void SendMessageFunc(void)
{	// first line of SendMessageFunc().
	//SendMessage(ghWnd, WM_NULL, 0, 0);
	int i = 0;
}


void __cdecl main()

{	// first line of main().

	while(0);	// second line of main().

	Func();
	while(0);	// line after call to Func().

	void (*pFuncCalledIndirectly)(void) = FuncCalledIndirectly;
	pFuncCalledIndirectly();

	OuterFunc(InnerFunc());

	_asm
	{
		push eax
		pop eax
	}

	int i = 1 / gintDenominator;

	/*
	WNDCLASS wndclass;
	wndclass.style = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "exepri1";
	RegisterClass(&wndclass);

	ghWnd = CreateWindow("exepri1", "exepri1", WS_OVERLAPPEDWINDOW, 
						 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
						 NULL, NULL, hInstance, NULL);
	*/

	SendMessageFunc();
	
	while(1);	// loop line.
	
	while(0);	// return from main().

	while(1);	// returned from main().

	return;
}


/*
LPARAM CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	
{	// first line of WndProc().
	switch(msg)
	{	
		case WM_NULL:
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
*/



/* 
START: for tests that require scrolling code out of view.












































END: for tests that require scrolling code out of view.
*/