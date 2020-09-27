/*********************************************/
/* testgo.c                                  */
/*********************************************/
/* CVM : g : Go command                      */
/*********************************************/
#include <xtl.h>

void LoopLevel2(int level1)
{
	char buf[64];
	int local_count = 0;

	Sleep(1000);
	for (local_count = 0; local_count < 100; local_count++)
	{
		wsprintf(buf, "      LoopLevel2: %-5d - %-3d\n", level1, local_count+1);
		OutputDebugString(buf);
	}
	Sleep(10);
}

int global_count;
int LoopLevel1()
{
	char buf[64];
	global_count = 0; /* inside LoopLevel1 */

	for (global_count = 0; global_count < 10000; global_count++)
	{
		wsprintf(buf, "  LoopLevel1: %-5d\n", global_count+1);
		OutputDebugString(buf);
		LoopLevel2(global_count);
	}
	return 0;
}

void __cdecl main()
{
	OutputDebugString("Test code has started\n");
	LoopLevel1();
	OutputDebugString("Test code has ended\n");
	while(1);
	return;
}