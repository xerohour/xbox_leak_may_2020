//
// Scratch area for writing quick code snippets
//
//

#include "tests.h"
#include "logging.h"

void testRebootAndSetTitle()
{
	HRESULT	hr;

	//
	// Reboot box and wait 15 seconds
	//
	hr = DmReboot(DMBOOT_WAIT);

	//
	// This call should be blocked until the box's connection comes back
	// online. And thus set the launch title to dolphin classic. It appears that
	// if you call this too early, it times out after 30 seconds. Need to wait
	// till the IP address appears on the screen prior to calling this API.
	//
	Sleep(16000);		// This appears to be the time it takes the box to cold boot
	hr = DmSetTitle("e:\\samples\\dolphinclassic", "dolphinclassic.xbe", "");
	hr = hr + 1;	// dummy code to set a breakpoint on.
}