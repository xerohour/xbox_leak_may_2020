#include "std.h"
#include "XODash.h"


XOApplication  theApp;
void __cdecl main()
{
	HRESULT hr = theApp.Create();
	if(FAILED(hr))
	{
		DbgPrint("main - fail to Initialize the app, hr = 0x%x\n. Aborting execution",hr);
		theApp.Destroy();
		return;
	}
	while (1)
	{
		theApp.Run();
	}

	theApp.Destroy();
}