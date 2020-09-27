// oleutil.cpp : implementation file
//

#include "stdafx.h"
#include "oleutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_LINE 1024      			// Never used; came from class wizard

/* OleEntryPoints NEAR oleEntryPoints;	// will be initialized to NULL

HINSTANCE hOle2, hMFCAnsi;

BOOL bOleInitWorked = FALSE;
BOOL bOleVersionOK = FALSE;

static char BASED_CODE szOle2[] = "OLE32.DLL";
static char BASED_CODE szMFCAnsi[] = "MFCANS32.DLL";

BOOL PASCAL IsOleInstalled()
{
	OFSTRUCT ofs;
	return OpenFile(szOle2, &ofs, OF_EXIST) != HFILE_ERROR
		&& OpenFile(szMFCAnsi, &ofs, OF_EXIST) != HFILE_ERROR;
}

BOOL PASCAL CheckOleVersion()
{
#ifndef BOGUSGUID
	ASSERT(IsOleInstalled());
#endif //BOGUSGUID
	
	// if we haven't tried initialize OLE then do it here	
	if (IsOleInstalled())
		LoadOleEntryPoints(FALSE);
		
	return bOleVersionOK;
}

void PASCAL LoadOleEntryPoints(BOOL bNeedTypeInfo)
{
	// attempt to load all the OLE entry points
	//	(note: load by name, since the ordinals aren't guaranteed to stay fixed)
	UINT uErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
	if (hOle2 == NULL)
	{	
		hOle2 = LoadLibrary(szOle2);
		if (hOle2 != NULL)
		{
			(FARPROC&)oleEntryPoints.lpCoCreateGuid = 
				GetProcAddress(hOle2, "CoCreateGuid");
			(FARPROC&)oleEntryPoints.lpOleInitialize = 
				GetProcAddress(hOle2, "OleInitialize");
			(FARPROC&)oleEntryPoints.lpOleUninitialize = 
				GetProcAddress(hOle2, "OleUninitialize");
		}
	}
	if (hMFCAnsi == NULL)
	{
		hMFCAnsi = LoadLibrary(szMFCAnsi);
		if (hMFCAnsi != NULL)
		{
			(FARPROC&)oleEntryPoints.lpCLSIDFromString =
				GetProcAddress(hMFCAnsi, "CLSIDFromString");			
		}

	}	
	::SetErrorMode(uErrorMode);

	if (oleEntryPoints.lpCoCreateGuid 		!= NULL
		&& oleEntryPoints.lpCLSIDFromString != NULL
		&& oleEntryPoints.lpOleInitialize 	!= NULL
		&& oleEntryPoints.lpOleUninitialize != NULL)
	{
		bOleVersionOK = TRUE;
	}
	else
		bOleVersionOK = FALSE;

	if (!IsOleLoaded())
	{
		// otherwise don't need them if don't have all of them
		UnloadOleEntryPoints();
		return;
	}	
		
	// initialize OLE since it was successfully loaded
	if (!bOleInitWorked)
	{
		ASSERT(oleEntryPoints.lpOleInitialize != NULL);
		if (!FAILED(GetScode(oleEntryPoints.lpOleInitialize(NULL))))
			bOleInitWorked = TRUE;
	}		
}

void PASCAL UnloadOleEntryPoints()
{
	// uninitialize OLE
	if (bOleInitWorked)
	{
		ASSERT(oleEntryPoints.lpOleUninitialize != NULL);
		oleEntryPoints.lpOleUninitialize();
		bOleInitWorked = FALSE;
	}

	if (hOle2 != NULL)
	{
		FreeLibrary(hOle2);
		hOle2 = NULL;
	}
	if (hMFCAnsi != NULL)
	{
		FreeLibrary(hMFCAnsi);
		hMFCAnsi = NULL;
	}

	// entry points are now invalid -- so NULL them out
	memset(&oleEntryPoints, 0, sizeof oleEntryPoints);
}

BOOL PASCAL IsOleLoaded()
{
	// return TRUE if non-typelib entry points available
	return oleEntryPoints.lpCoCreateGuid 	!= NULL
		&& oleEntryPoints.lpCLSIDFromString != NULL
		&& oleEntryPoints.lpOleInitialize 	!= NULL
		&& oleEntryPoints.lpOleUninitialize != NULL;
}

*/

#ifdef BOGUSGUID
void GetGuidFromList(GUID FAR& guid, UINT nGuid)
{
	// Create guid from range given to us.

	// nGuid ranges from 1 to 0x100, to reserve 0 for when .ini entry
	//  for next guid isn't an int.  Thus, we subtract 1 fron nGuid
	
	guid.Data1=0x00027800 + (nGuid-1);
	guid.Data2=0;
	guid.Data3=0;
	guid.Data4[0]=0xC0;
	guid.Data4[1]=0;
	guid.Data4[2]=0;
	guid.Data4[3]=0;
	guid.Data4[4]=0;
	guid.Data4[5]=0;
	guid.Data4[6]=0;
	guid.Data4[7]=0x46;
}
#endif //BOGUSGUID

BOOL PASCAL GetValidGuid(GUID FAR& guid)
{
	//LoadOleEntryPoints(FALSE);

	// start by filling the GUID with 0
	memset((void*) &guid, 0, sizeof(GUID));
	
	// Retain a zeroed guid to compare to later
	GUID zero;
	memset((void*) &zero, 0, sizeof(GUID));

/*	if (IsOleLoaded())
	{
	*/
		// then call OLE api to get a random GUID
		// Deliberately ignore CoCreateGuid's return value.  It calls UuidCreate
		//  which can sometimes succeed but return a success-value unrecognized
		//  by CoCreateGuid-- in this case CoCreateGuid will fail, even
		//  though we've just gotten a perfectly valid guid.
		// Instead, we'll just see if guid is still zeroed out-- THAT will
		//  indicate whether we should fail.
		CoCreateGuid(&guid);
		if (!memcmp((void*) &guid, (void*) &zero, sizeof(GUID)))
			return FALSE;		// The new guid is still zeroed-- no guid created
		else
			return TRUE;		// The guid has changed-- we're ok
/*	}
	else
#ifndef BOGUSGUID	
		return FALSE;
#else
	{
		// The code below generated guids from a range of 256, w/out using OLE
		#define GUID_MAX 0x100
		CWinApp* pApp = AfxGetApp();
		UINT nGuid = pApp->GetProfileInt("guid generation", "next guid", GUID_MAX + 1);
		if (nGuid == GUID_MAX + 1 || nGuid == 0)
		{
			// If entry not found or cycled past end...
			pApp->WriteProfileInt("guid generation", "next guid", 2);
			nGuid = 1;
		}
		else
			pApp->WriteProfileInt("guid generation", "next guid", nGuid+1);
		GetGuidFromList(guid, nGuid);
		return TRUE;
	}
#endif //BOGUSGUID*/
}

