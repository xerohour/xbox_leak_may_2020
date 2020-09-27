//
// Test cases - sorted according to API appearance in xboxdbg.h/xbdm.h
//
#ifdef _XBOX
#include <xtl.h>
#include <xbdm.h>
#else
#include <windows.h>
#include <xboxdbg.h>
#endif

// notification
void testNotification();

// breakpoint routines
void testBreakpoint();
void testExecution();

// reboot
void testDmReboot();

// memory
void testMemory();

// thread routines
void testThreadSupport();

// xtl data, modules and sections, xbe info
void testDmGetXtlData();
void testModulesAndSections();
void testDmGetXbeInfo();

// socket-level commands
void testSocketCommands();

// filesystem
void testFileSystem();

// time
void testDmGetSystemTime();

// config and name functions
void testConfig();

// management functions
void testSecurity();

// title to launch

// screenshot/snapshot

// start/stop profile data collection

// performance counters
void testPerfCounters();

// drive info
void testDmGetDriveList();
void testDmGetDiskFreeSpace();

// error codes
void testDmTranslateError();

// image file routines
void testImageFileRoutines();
