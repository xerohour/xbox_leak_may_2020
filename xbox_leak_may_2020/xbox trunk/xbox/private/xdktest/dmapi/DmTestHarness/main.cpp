// Main.cpp : Defines the entry point for the console application.
//

#include "tests.h"

void __cdecl main(int argc, char* argv[])
{
	testPerfCounters();
	return;

	// notification
	testNotification();

	// breakpoint routines
	testBreakpoint();
	testExecution();

	// reboot
	testDmReboot();

	// memory
	testMemory();

	// thread routines
	testThreadSupport();

	// xtl data, modules and sections, xbe info
	testDmGetXtlData();
	testModulesAndSections();
	testDmGetXbeInfo();

	// socket-level commands
	testSocketCommands();

	// filesystem
	testFileSystem();

	// time
	testDmGetSystemTime();

	// config and name functions
	testConfig();

	// management functions
	testSecurity();

	// title to launch

	// screenshot/snapshot

	// start/stop profile data collection

	// performance counters
	testPerfCounters();

	// drive info
	testDmGetDriveList();
	testDmGetDiskFreeSpace();

	// error codes
	testDmTranslateError();

	// image file routines
	testImageFileRoutines();
}