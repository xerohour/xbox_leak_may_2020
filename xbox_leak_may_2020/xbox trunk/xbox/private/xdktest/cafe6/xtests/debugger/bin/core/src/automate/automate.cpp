#include <windows.h>


void func(void)
	{
START_OF_FUNC:
	return;
	}


void DebuggerObjectTest(void)
	{
	int i = 0;
AFTER_RUN_TO_CURSOR:
	func();
AFTER_STEP_OVER:
	func();
AFTER_STEP_OUT:
	return;
	}


void BreakpointsCollectionTest(void)
	{
	return;
	}


void BreakpointItemTest(void)
	{
	return;
	}


void MacroRecordingTest(void)
	{
	return;
	}


int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
	{
	DebuggerObjectTest();
	BreakpointsCollectionTest();
	BreakpointItemTest();
	MacroRecordingTest();
	return 0;		
	}	
