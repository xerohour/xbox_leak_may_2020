#include <xtl.h> //xbox #include <windows.h>

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

void __cdecl main()
	{
	using namespace n1;
	func();
	if (j==1) {
		j += 2;
	}
	else {
		j *= 10; 
	}
	return;		// line after n1::j changed
	}	
