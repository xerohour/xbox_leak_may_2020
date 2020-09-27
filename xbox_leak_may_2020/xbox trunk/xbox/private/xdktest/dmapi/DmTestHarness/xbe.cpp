#include "tests.h"
#include "logging.h"

static void testXbeGetLocalModulePath();
static void testXbeGetXbeInfo();
static void testXbeGetBuilderArgs();

void testImageFileRoutines()
{
	testXbeGetLocalModulePath();
	testXbeGetXbeInfo();
	testXbeGetBuilderArgs();
}

void testXbeGetLocalModulePath()
{
#ifndef _XBOX
	HRESULT	hr;
	char	buffer[512];

	// Pass NULL for szXBE

	VERIFY_HRESULT(XbeGetLocalModulePathA(NULL, "INIT", buffer, sizeof buffer), XBDM_NOSUCHFILE);
	//VERIFY_HRESULT(XbeGetLocalModulePathW(NULL, "INIT", buffer, sizeof buffer), XBDM_NOSUCHFILE);

	// Pass NULL for szModule

	VERIFY_HRESULT(hr = XbeGetLocalModulePathA("e:\\samples\\cartoon\\cartoon.xbe", NULL, buffer, sizeof buffer), XBDM_NOSUCHFILE);
	//VERIFY_HRESULT(hr = XbeGetLocalModulePathW(L"e:\\samples\\cartoon\\cartoon.xbe", NULL, buffer, sizeof buffer), XBDM_NOSUCHFILE);

	// pass NULL for szPath

	VERIFY_HRESULT(hr = XbeGetLocalModulePathA("e:\\samples\\cartoon\\cartoon.xbe", "INIT", NULL, sizeof buffer), E_INVALIDARG);
	//VERIFY_HRESULT(hr = XbeGetLocalModulePathW(L"e:\\samples\\cartoon\\cartoon.xbe", "INIT", NULL, sizeof buffer), E_INVALIDARG);

	// TODO: XbeGetLocalModulePath - other cases
	// Pass empty string for szXBE
	// Pass valid filename for szXBE
	// Pass valid filename with mixed case for szXBE
	// Pass valid filename with leading/trailing spaces for szXBE
	// Pass invalid filename for szXBE
	// Pass valid module for szModule
	// Pass valid module with mixed case for szModule
	// Pass valid module with leading/trailing spaces for szModule
	// Pass 0 for cchPath
	// Pass 1 for cchPath
	// Pass # < actual path size for cchPath
	// Pass # > actual path size for cchPath
/*
	TEST_BEGIN("XbeGetLocalModulePath(\"e:\\samples\\cartoon\\cartoon.xbe\", \"XTL.DLL\", buffer, sizeof buffer)");
	hr = XbeGetLocalModulePath("e:\\samples\\cartoon\\cartoon.xbe", \"XTL.DLL\", buffer, sizeof buffer);
	if (hr != E_INVALIDARG)
		LogTestResult("XbeGetLocalModulePath(\"e:\\samples\\cartoon\\cartoon.xbe\", \"XTL.DLL\", buffer, sizeof buffer)", false, "E_INVALIDARG Error should be returned");
	else
		LogTestResult("XbeGetLocalModulePath(\"e:\\samples\\cartoon\\cartoon.xbe\", \"XTL.DLL\", buffer, sizeof buffer)", true, "");
	TEST_END("XbeGetLocalModulePath(\"e:\\samples\\cartoon\\cartoon.xbe\", \"XTL.DLL\", buffer, sizeof buffer)");
*/
#endif
}

void testDmGetXbeInfo()
{
	HRESULT	hr;
	DM_XBE	xbeinfo;

	// Pass NULL for szName

	ZeroMemory(&xbeinfo, sizeof xbeinfo);
	VERIFY_HRESULT(DmGetXbeInfo(NULL, &xbeinfo), XBDM_NOSUCHFILE);
	ZeroMemory(&xbeinfo, sizeof xbeinfo);
	VERIFY_HRESULT(DmGetXbeInfo("e:\\samples\\cartoon\\cartoon.xbe", NULL), E_INVALIDARG);
	ZeroMemory(&xbeinfo, sizeof xbeinfo);
	VERIFY_HRESULT(DmGetXbeInfo("", &xbeinfo), XBDM_NOSUCHFILE);

	// TODO: DmGetXbeInfo - other cases
	// copy all new testcases from testXbeGetXbeInfo
}

void testXbeGetXbeInfo()
{
#ifndef _XBOX
	HRESULT	hr;
	DM_XBE	xbeinfo;

	// Pass NULL for szName

	memset(&xbeinfo, 0, sizeof xbeinfo);
	VERIFY_HRESULT(XbeGetXbeInfoA(NULL, &xbeinfo), XBDM_NOSUCHFILE);
	//VERIFY_HRESULT(XbeGetXbeInfoW(NULL, &xbeinfo), XBDM_NOSUCHFILE);

	// Pass NULL for pXbeInfo

	memset(&xbeinfo, 0, sizeof xbeinfo);
	VERIFY_HRESULT(XbeGetXbeInfoA("e:\\samples\\cartoon\\cartoon.xbe", NULL), E_INVALIDARG);
	//VERIFY_HRESULT(XbeGetXbeInfoW(L"e:\\samples\\cartoon\\cartoon.xbe", NULL), E_INVALIDARG);

	// Pass empty string for szName

	memset(&xbeinfo, 0, sizeof xbeinfo);
	VERIFY_HRESULT(XbeGetXbeInfoA("", &xbeinfo), XBDM_NOSUCHFILE);
	//VERIFY_HRESULT(XbeGetXbeInfoW(L"", &xbeinfo), XBDM_NOSUCHFILE);

	// Call when no XBE is running

	memset(&xbeinfo, 0, sizeof xbeinfo);
	hr = DmReboot(DMBOOT_WARM);
	Sleep(10000);
	VERIFY_HRESULT(XbeGetXbeInfoA("e:\\samples\\cartoon\\cartoon.xbe", &xbeinfo), XBDM_NOERR);
	/*
	memset(&xbeinfo, 0, sizeof xbeinfo);
	hr = DmReboot(DMBOOT_WARM);
	Sleep(10000);
	VERIFY_HRESULT(XbeGetXbeInfoA(L"e:\\samples\\cartoon\\cartoon.xbe", &xbeinfo), XBDM_NOERR);
	*/

	// TODO: XbeGetXbeInfo - other cases
	// Pass valid szName with mixed case
	// Pass valid szName with leading/trailing spaces
	// Pass full path for szName
	// Pass long filename for szName
	// Call when XBE already running
#endif
}

void testXbeGetBuilderArgs()
{
#ifndef _XBOX
	HRESULT	hr;
	char	szArgs[512];
	DWORD	dwSize;

#define VALID_LOCAL_XBE ""

	ZeroMemory(szArgs, sizeof szArgs);
	dwSize = sizeof szArgs;
	VERIFY_HRESULT(XbeGetBuilderArgsA(VALID_LOCAL_XBE, szArgs, &dwSize), XBDM_NOSUCHFILE);
	//VERIFY_HRESULT(XbeGetBuilderArgsW(VALID_LOCAL_XBE, szArgs, &dwSize), XBDM_NOSUCHFILE);

	VERIFY_HRESULT(XbeGetBuilderArgsA(NULL, NULL, &dwSize), E_INVALIDARG);
	//VERIFY_HRESULT(XbeGetBuilderArgsW(NULL, NULL, &dwSize), E_INVALIDARG);

	VERIFY_HRESULT(XbeGetBuilderArgsA(NULL, szArgs, NULL), E_INVALIDARG);
	//VERIFY_HRESULT(XbeGetBuilderArgsW(NULL, szArgs, NULL), E_INVALIDARG);

	dwSize = 0;
	VERIFY_HRESULT(XbeGetBuilderArgsA(VALID_LOCAL_XBE, szArgs, &dwSize), XBDM_NOSUCHFILE);
	//VERIFY_HRESULT(XbeGetBuilderArgsW(VALID_LOCAL_XBE, szArgs, &dwSize), XBDM_NOSUCHFILE);

	// TODO:XbeGetBuilderArgs - other cases
#endif
}
