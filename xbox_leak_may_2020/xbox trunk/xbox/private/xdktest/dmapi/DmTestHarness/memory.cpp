//
// Scratch area for writing quick code snippets
//
//

#include "tests.h"
#include "logging.h"
#include <stdio.h>

// retail xbox units have 64Mb of memory
// address range is 0x00000000 - 0x03FFFFFF
// development kits have 128 Mb
// address range is 0x04000000 - 0x07FFFFFF

#define VALID_ADDR_LO 0x00040000
#define VALID_ADDR_HI 0x04040000

void VerifyMemory(const char *buf, unsigned int len, char expected, char *errhdr)
{
	char sztmp[160];
	for (unsigned int i = 0; i < len; i++)
		if (buf[i] != expected)
		{
			sprintf(sztmp, "at %03d expected 0x%02x, found 0x%02x", i, expected, buf[i]);
			LogTestResult(errhdr, false, sztmp);
		}
}

void testMemory()
{
	HRESULT	hr;
	char	buffer[512];
	DWORD	size;

	/* unmapped section of memory */
	VERIFY_HRESULT(DmGetMemory(0, sizeof buffer, buffer, &size), XBDM_NOERR);	// size = 0
	VERIFY_HRESULT(DmGetMemory(0, 10, buffer, &size), XBDM_NOERR);				// size = 0
	VERIFY_HRESULT(DmGetMemory(0, sizeof buffer, buffer, NULL), XBDM_MEMUNMAPPED);
	VERIFY_HRESULT(DmGetMemory(0, 1, buffer, &size), XBDM_NOERR);				// size = 0
	VERIFY_HRESULT(DmGetMemory(0, 1, buffer, NULL), XBDM_MEMUNMAPPED);
	VERIFY_HRESULT(DmGetMemory(0, 0, buffer, &size), XBDM_NOERR);
	VERIFY_HRESULT(DmGetMemory(0, 0, buffer, NULL), XBDM_NOERR);
	VERIFY_HRESULT(DmGetMemory(0, 10, NULL, &size), E_INVALIDARG);
	VERIFY_HRESULT(DmGetMemory(0, 10, NULL, NULL), E_INVALIDARG);

	/* mapped section of memory */
	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_LO, sizeof buffer, buffer, &size), XBDM_NOERR);
	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_LO, sizeof buffer, NULL, &size), E_INVALIDARG);
	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_LO, sizeof buffer, NULL, NULL), E_INVALIDARG);

	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_HI, sizeof buffer, buffer, &size), XBDM_NOERR);
	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_HI, sizeof buffer, NULL, &size), E_INVALIDARG);
	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_HI, sizeof buffer, NULL, NULL), E_INVALIDARG);

	FillMemory(buffer, sizeof buffer, 0);

	/* unmapped section of memory */
	VERIFY_HRESULT(DmSetMemory(0, sizeof buffer, buffer, &size), XBDM_MEMUNMAPPED);
	VERIFY_HRESULT(DmSetMemory(0, 10, buffer, &size), XBDM_MEMUNMAPPED);
	VERIFY_HRESULT(DmSetMemory(0, sizeof buffer, buffer, NULL), XBDM_MEMSETINCOMPLETE);
	VERIFY_HRESULT(DmSetMemory(0, 1, buffer, &size), XBDM_MEMUNMAPPED);
	VERIFY_HRESULT(DmSetMemory(0, 1, buffer, NULL), XBDM_MEMSETINCOMPLETE);
	VERIFY_HRESULT(DmSetMemory(0, 0, buffer, &size), XBDM_NOERR);
	VERIFY_HRESULT(DmSetMemory(0, 0, buffer, NULL), XBDM_NOERR);
	VERIFY_HRESULT(DmSetMemory(0, 10, NULL, &size), E_INVALIDARG);
	VERIFY_HRESULT(DmSetMemory(0, 10, NULL, NULL), E_INVALIDARG);

	/* mapped section of memory */
	VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_LO, sizeof buffer, buffer, &size), XBDM_NOERR);
	VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_LO, sizeof buffer, NULL, &size), E_INVALIDARG);
	VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_LO, sizeof buffer, NULL, NULL), E_INVALIDARG);

	//VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_HI, sizeof buffer, buffer, &size), XBDM_NOERR);
	VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_HI, sizeof buffer, NULL, &size), E_INVALIDARG);
	VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_HI, sizeof buffer, NULL, NULL), E_INVALIDARG);

	/* check memory transfers */

	// put 0x2d in one buffer-sized block, and immediately get it
	FillMemory(buffer, sizeof buffer, 0x2d);
	VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_LO, sizeof buffer, buffer, &size), XBDM_NOERR);
	ZeroMemory(buffer, sizeof buffer);
	VERIFY_HRESULT(DmGetMemory(( void *) VALID_ADDR_LO, sizeof buffer, buffer, &size), XBDM_NOERR);
	VerifyMemory(buffer, sizeof buffer, 0x2d, "DmSetMemory/DmGetMemory");

	// put 0x3c in one buffer-sized block, and above it put 0x6f's
	FillMemory(buffer, sizeof buffer, 0x3c);
	VERIFY_HRESULT(DmSetMemory(( void *) VALID_ADDR_LO, sizeof buffer, buffer, &size), XBDM_NOERR);
	FillMemory(buffer, sizeof buffer, 0x6f);
	VERIFY_HRESULT(DmSetMemory(( void *) (VALID_ADDR_LO + sizeof buffer), sizeof buffer, buffer, &size), XBDM_NOERR);

	// get half the 0x3c's: we expect the buffer to be half 0x03c, half 0x6f's
	// because we didn't clear the local buffer from the previous exercise
	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_LO, sizeof buffer / 2, buffer, &size), XBDM_NOERR);
	VerifyMemory(buffer, (sizeof buffer) / 2, 0x3c, "DmGetMemory [split block, lower half]");
	VerifyMemory(buffer + ((sizeof buffer) / 2), (sizeof buffer) / 2, 0x6f, "DmGetMemory [split block, upper half]");

	// get all the 0x3c's
	ZeroMemory(buffer, sizeof buffer);
	VERIFY_HRESULT(DmGetMemory((const void *) VALID_ADDR_LO, sizeof buffer, buffer, &size), XBDM_NOERR);
	VerifyMemory(buffer, sizeof buffer, 0x3c, "DmGetMemory [lower block]");

	// get all the 0x6f's
	ZeroMemory(buffer, sizeof buffer);
	VERIFY_HRESULT(DmGetMemory((const void *) (VALID_ADDR_LO + sizeof buffer), sizeof buffer, buffer, &size), XBDM_NOERR);
	VerifyMemory(buffer, sizeof buffer, 0x6f, "DmGetMemory [upper block]");
}
