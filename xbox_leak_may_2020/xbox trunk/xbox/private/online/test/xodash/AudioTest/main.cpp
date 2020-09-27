#include "std.h"

#include "XODash.h"


XOApplication  theApp;
void __cdecl main()
{
	theApp.Create();
	while (1)
	{
		theApp.Run();
	}

	theApp.Destroy();
}