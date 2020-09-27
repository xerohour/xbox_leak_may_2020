#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

#include "dbgtest.h"
#include "bounce.h"

#ifdef _XBOX
void __cdecl main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    CBounce bounce;
	int choice = 0;

	while(1)
	{
        #ifdef _XBOX
        bounce.DrawStuff();
		#endif

		if (!choice)
			continue;

        Test_Disasm_MMX();
        Test_Disasm_Katmai();

        // debug multiple threads
        Test_Threads();

        // test FPU exception handling
		Test_Registers();

        // test register view of MMX and SSE registers
        Test_MMX_Registers();
        Test_SSE_Registers();

        // test call stack 
        Test_Stack();

        // test tracing through /opt:icf-combined functions
        Test_ICF();

        // some interesting breakpoint testcases
        Test_Breakpoints();
	}
	
#ifndef _XBOX
    return 0;
#endif
}
