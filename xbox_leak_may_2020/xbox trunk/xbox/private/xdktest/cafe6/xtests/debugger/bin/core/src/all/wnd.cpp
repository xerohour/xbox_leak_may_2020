#include <windows.h>

LPARAM CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
	
	{ 				// first line of WinMain()
	MSG msg;
	HWND hwnd;

	if(!hinstPrev)

		{
		WNDCLASS wndclass;
	    wndclass.style = 0;
	    wndclass.lpfnWndProc = WndProc;
	    wndclass.cbClsExtra = 0;
	    wndclass.cbWndExtra = 0;
	    wndclass.hInstance = hinst;
	    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	    wndclass.lpszMenuName = NULL;
	    wndclass.lpszClassName = "Wnd";

	    if(!RegisterClass(&wndclass))
	    	return 0;
		}

	hwnd = CreateWindow("Wnd", "Wnd", WS_OVERLAPPEDWINDOW | 
						WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
						CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hinst, NULL);

	while(GetMessage(&msg, NULL, 0, 0))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	return msg.wParam;
	}


LPARAM CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	
	{
	switch(msg)
		{	
		case WM_PAINT:
			{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			return 0L;
			}
		case WM_DESTROY:
			{
			PostQuitMessage(0);
			while(1);
			break;
			}
		}

	return DefWindowProc(hwnd, msg, wParam, lParam);
	}

