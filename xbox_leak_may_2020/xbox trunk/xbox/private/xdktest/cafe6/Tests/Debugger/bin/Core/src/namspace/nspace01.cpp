#include <windows.h>

int i = 0;
int j = 0;

namespace n1
	
	{
	int i = 1;
	int j = 1;

	void func(void)
		{				// first line of n1::func()
		int local = 3;
		}

	namespace n2
		{
		int i = 2;
		int j = 2;
		}
	}

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
	{
	using namespace n1;
	func();
	if (j==1) {
		j += 2;
	}
	else {
		j *= 10; 
	}
	return 0;		// line after n1::j changed
	}	
