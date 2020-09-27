#include "std.h"
#include "ASEConv.h"


ASEConv  theApp;
void __cdecl main()
{
	theApp.Create();
	while (1)
	{
		theApp.Run();
	}

	theApp.Destroy();
}