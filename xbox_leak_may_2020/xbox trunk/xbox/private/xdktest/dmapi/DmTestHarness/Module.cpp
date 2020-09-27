//
// Debug API - Module Access Functions
//

#include "tests.h"
#include "logging.h"

static void testDmWalkLoadedModules();
static void testDmCloseLoadedModules();
static void testDmWalkModuleSections();
static void testDmCloseModuleSections();
static void testDmGetModuleLongName();
static void testWalkAllModulesAndSections();

void testModulesAndSections()
{
	testDmWalkLoadedModules();
	testDmCloseLoadedModules();
	testDmWalkModuleSections();
	testDmCloseModuleSections();
	testDmGetModuleLongName();
	testWalkAllModulesAndSections();
}

void testDmWalkLoadedModules()
{
	HRESULT				hr;
	PDM_WALK_MODULES	pWalkMod = NULL;
	DMN_MODLOAD			ModLoad;
	char				buf[256];

	// Pass NULL for ppWalkMod, pModLoad

	VERIFY_HRESULT(DmWalkLoadedModules(NULL, &ModLoad), E_INVALIDARG);
	VERIFY_HRESULT(DmWalkLoadedModules(&pWalkMod, NULL), E_INVALIDARG);

	// Call function repeatedly to obtain all modules

	memset(&ModLoad, 0, sizeof ModLoad);
	hr = DmWalkLoadedModules(&pWalkMod, &ModLoad);
	while (hr == XBDM_NOERR)
	{
		wsprintf(buf, "%-20s - Addr: 0x%08x  Size: %8ul  Time: %08x  ChkSum: %08x", 
				ModLoad.Name, ModLoad.BaseAddress, 
				ModLoad.Size, ModLoad.TimeStamp, ModLoad.CheckSum);
		LogNote(buf);
		memset(&ModLoad, 0, sizeof ModLoad);
		VERIFY_HRESULT_ALT(DmWalkLoadedModules(&pWalkMod, &ModLoad), XBDM_NOERR, XBDM_ENDOFLIST);
	}

	if (pWalkMod)
		DmCloseLoadedModules(pWalkMod);
}

void testDmWalkModuleSections()
{
	HRESULT				hr;
	PDM_WALK_MODULES	pWalkMod = NULL;
	DMN_MODLOAD			ModLoad;
	DMN_SECTIONLOAD		SectLoad;
	PDM_WALK_MODSECT	pWalkModSect;

	hr = DmWalkLoadedModules(&pWalkMod, &ModLoad);
	if (hr != XBDM_NOERR)
	{
		LogNote("Cannot run DmWalkModuleSections() tests due to inability to obtain Module object");
		return;
	}

	// Pass NULL for ppWalkModSect

	VERIFY_HRESULT(DmWalkModuleSections(NULL, "", &SectLoad), E_INVALIDARG);

	// Pass NULL for pModLoad

	pWalkModSect = NULL;
	VERIFY_HRESULT(DmWalkModuleSections(&pWalkModSect, NULL, &SectLoad), XBDM_NOMODULE);
	if (pWalkModSect)
		DmCloseModuleSections(pWalkModSect);

	// Pass NULL for lzModName

	pWalkModSect = NULL;
	VERIFY_HRESULT(DmWalkModuleSections(&pWalkModSect, ModLoad.Name, NULL), E_INVALIDARG);
	if (pWalkModSect)
		DmCloseModuleSections(pWalkModSect);

	// Pass empty string for ModName

	pWalkModSect = NULL;
	VERIFY_HRESULT(DmWalkModuleSections(&pWalkModSect, "", &SectLoad), XBDM_NOMODULE);
	if (pWalkModSect)
		DmCloseModuleSections(pWalkModSect);

	// Pass valid module for ModName

	pWalkModSect = NULL;
	VERIFY_HRESULT(DmWalkModuleSections(&pWalkModSect, ModLoad.Name, &SectLoad), XBDM_NOERR);
	if (pWalkModSect)
		DmCloseModuleSections(pWalkModSect);

	// Pass invalid module for ModName

	pWalkModSect = NULL;
	VERIFY_HRESULT(DmWalkModuleSections(&pWalkModSect, "$@BogusName@$", &SectLoad), XBDM_NOMODULE);
	if (pWalkModSect)
		DmCloseModuleSections(pWalkModSect);

	// Pass valid module with mixed case for ModName
	// Pass valid module with leading/trailing spaces for ModName

	if (pWalkMod)
		DmCloseLoadedModules(pWalkMod);
}

void testDmCloseLoadedModules()
{
	HRESULT				hr;
	PDM_WALK_MODULES	pWalkMod = NULL;
	DMN_MODLOAD			ModLoad;
	
	// using Null

	VERIFY_HRESULT(DmCloseLoadedModules(NULL), E_INVALIDARG);

	// using valid DM_WALK_MODULES object

	pWalkMod = NULL;
	VERIFY_HRESULT(DmWalkLoadedModules(&pWalkMod, &ModLoad), XBDM_NOERR);
	if (pWalkMod)
		DmCloseLoadedModules(pWalkMod);
}

void testDmCloseModuleSections()
{
	HRESULT				hr;
	PDM_WALK_MODULES	pWalkMod = NULL;
	DMN_MODLOAD			ModLoad;
	DMN_SECTIONLOAD		SectLoad;
	PDM_WALK_MODSECT	pWalkModSect;
	
	// using Null

	VERIFY_HRESULT(DmCloseModuleSections(NULL), E_INVALIDARG);

	// using valid DM_WALK_MODULES object

	pWalkMod = NULL;
	hr = DmWalkLoadedModules(&pWalkMod, &ModLoad);
	if (hr != XBDM_NOERR)
	{
		// Error code
		LogTestResult("DmCloseModuleSections(pWalkModSect)", false, "Unable to obtain valid LoadModule to use");
		return;
	}

	pWalkModSect = NULL;
	VERIFY_HRESULT(DmWalkModuleSections(&pWalkModSect, ModLoad.Name, &SectLoad), XBDM_NOERR);
	if (hr != XBDM_NOERR)
		DmCloseModuleSections(pWalkModSect);
}

void testDmGetModuleLongName()
{
	HRESULT				hr;
	PDM_WALK_MODULES	pWalkMod = NULL;
	DMN_MODLOAD			ModLoad;
	char				LongName[512];
	DWORD				size;

	hr = DmWalkLoadedModules(&pWalkMod, &ModLoad);
	if (hr != XBDM_NOERR)
	{
		LogNote("Cannot run DmGetModuleLongName() tests due to inability to obtain Module object");
		return;
	}
/*
	// Pass NULL for ShortName

	TEST_BEGIN("DmGetModuleLongName(NULL, LongName, &size)");
	size = sizeof LongName;
	memset(LongName, 0, size);
	hr = DmGetModuleLongName(NULL, LongName, &size);
	if (hr != XBDM_NOMODULE)
		LogTestResult("DmGetModuleLongName(NULL, LongName, &size)", false, "E_INVALIDARG error should have been returned");
	else
		LogTestResult("DmGetModuleLongName(NULL, LongName, &size)", true, "");
	TEST_END("DmGetModuleLongName(NULL, LongName, &size)");

	// Pass NULL for LongName

	TEST_BEGIN("DmGetModuleLongName(ModLoad.Name, NULL, &size)");
	size = sizeof LongName;
	hr = DmGetModuleLongName(ModLoad.Name, NULL, &size);
	if (hr != E_INVALIDARG)
		LogTestResult("DmGetModuleLongName(ModLoad.Name, NULL, &size)", false, "E_INVALIDARG error should have been returned");
	else
		LogTestResult("DmGetModuleLongName(ModLoad.Name, NULL, &size)", true, "");
	TEST_END("DmGetModuleLongName(ModLoad.Name, NULL, &size)");

	// Pass NULL for pcch

	TEST_BEGIN("DmGetModuleLongName(ModLoad.Name, LongName, NULL)");
	size = sizeof LongName;
	memset(LongName, 0, size);
	hr = DmGetModuleLongName(ModLoad.Name, LongName, NULL);
	if (hr != E_INVALIDARG)
		LogTestResult("DmGetModuleLongName(ModLoad.Name, LongName, NULL)", false, "E_INVALIDARG error should have been returned");
	else
		LogTestResult("DmGetModuleLongName(ModLoad.Name, LongName, NULL)", true, "");
	TEST_END("DmGetModuleLongName(ModLoad.Name, LongName, NULL)");
*/
	// Pass valid ShortName

	size = sizeof LongName;
	memset(LongName, 0, size);
	VERIFY_HRESULT(DmGetModuleLongName(ModLoad.Name, LongName, &size), XBDM_NOERR);

	// Pass invalid ShortName

	size = sizeof LongName;
	memset(LongName, 0, size);
	VERIFY_HRESULT(DmGetModuleLongName("$@BogusName@$", LongName, &size), XBDM_NOMODULE);

	// Pass valid ShortName with mixed case
	// Pass valid ShortName with leading/trailing spaces
	// Pass pcch value of 0
	// Pass pcch value of 1
	// Pass pcch value < incoming data size
	// Pass pcch value > incoming data size
	// Verify LongName only returns if /debug used

	if (pWalkMod)
		DmCloseLoadedModules(pWalkMod);
}

void testWalkAllModulesAndSections()
{
	HRESULT				hr;
	PDM_WALK_MODULES	pWalkMod = NULL;
	DMN_MODLOAD			ModLoad;
	DMN_SECTIONLOAD		SectLoad;
	PDM_WALK_MODSECT	pWalkModSect;
	char				buf[256];

	//
	// Walk Modules currently running on Xbox
	//
	hr = DmWalkLoadedModules(&pWalkMod, &ModLoad);
	while (hr == XBDM_NOERR)	// && hr != XBDM_ENDOFLIST)
	{
		wsprintf(buf, "%-25s - Addr: 0x%08x  Size: %8ul  Time: %08x  ChkSum: %08x\n", 
				ModLoad.Name, ModLoad.BaseAddress, 
				ModLoad.Size, ModLoad.TimeStamp, ModLoad.CheckSum);
		OutputDebugString(buf);
		//
		// Walk Sections of Current module
		//
		pWalkModSect = NULL;
		hr = DmWalkModuleSections(&pWalkModSect, ModLoad.Name, &SectLoad);
		while (hr == XBDM_NOERR)
		{
			wsprintf(buf, "     %-20s - Addr: 0x%08x  Size: %8ul\n", 
					SectLoad.Name, SectLoad.BaseAddress, 
					SectLoad.Size);
			OutputDebugString(buf);
			hr = DmWalkModuleSections(&pWalkModSect, ModLoad.Name, &SectLoad);
		}
		if (hr == XBDM_ENDOFLIST || pWalkModSect)
			DmCloseModuleSections(pWalkModSect);

		hr = DmWalkLoadedModules(&pWalkMod, &ModLoad);
	}
	if (hr == XBDM_ENDOFLIST || pWalkMod)	
		DmCloseLoadedModules(pWalkMod);
}
