#include "XDIFile.h"
#include <assert.h>

BOOL VerifyContents(BYTE *pbDemos, DWORD dwFileSize)
{
	LONG lCheckSum = 0;
	LONG lStoredCheckSum = ((XDIHEADER *) pbDemos)->m_lCheckSum;

	//zero out checksum for computation...
	((XDIHEADER *) pbDemos)->m_lCheckSum = 0L;

	while (dwFileSize--)
	{
		lCheckSum = lCheckSum + (((DWORD) *pbDemos + dwFileSize) % ((DWORD) *pbDemos + 1));
		pbDemos++;
	}

	return lCheckSum == lStoredCheckSum;
}

void WriteChecksum(HANDLE hFile)
{
	DWORD  dwFileSize;
	DWORD  dwNum;
	LONG   lCheckSum = 0;
	BYTE   *pbDemos;
	BYTE   *pbPtr;
	XDIHEADER *pxdi;
	
	FlushFileBuffers(hFile);
	dwFileSize = GetFileSize(hFile, NULL);
	
	pbDemos = (BYTE *) new BYTE[dwFileSize];
	if (pbDemos)
	{
		pxdi = (XDIHEADER *) pbDemos;
		
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		ReadFile(hFile, pbDemos, dwFileSize, &dwNum, NULL);
		
		if (dwFileSize == dwNum && dwFileSize >= sizeof(XDIHEADER))
		{
			//zero out checksum for computation...
			pxdi->m_lCheckSum = 0L;
			
			pbPtr = pbDemos;
			while (dwFileSize--)
			{
				lCheckSum = lCheckSum + (((DWORD) *pbPtr + dwFileSize) % ((DWORD) *pbPtr + 1));
				pbPtr++;
			}
			
			pxdi->m_lCheckSum = lCheckSum;
			
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			
			WriteFile(hFile, pxdi, sizeof(XDIHEADER), &dwNum, NULL);
		}
		
		delete pbDemos;
	}
}

#ifdef _XBOX

// MJL 7/16/2001: for fixing up pointers
#define FIXUP(pointer, offset) (*(int *)(&(pointer))) += (*(int *)(&(offset)))

//
//Change byte offsets in DIB struct to pointers to actual data in m_data section
//
static void FixupXDIBlob(void *pvDemos)
{
    DIB       *pdib;
    LONG     lNumDemos = 0;

    lNumDemos = ((XDIHEADER *) pvDemos)->m_lNumDemos;

    //
    //Point past header to first XDI entry
    //
    pdib = (DIB *) ((BYTE *) pvDemos + sizeof(XDIHEADER));

    while (lNumDemos--)
    {
        //
        //Transform size into a pointer to the next demo info block
        //
        pdib->m_pNext = lNumDemos ? (DIB *) ((BYTE *) pdib + pdib->m_dwSize) : NULL;

        FIXUP(pdib->m_pszTitle,		pdib);
        FIXUP(pdib->m_pszFolder,	pdib);
        FIXUP(pdib->m_pszXBE,		pdib);
        FIXUP(pdib->m_pszTeaser,	pdib);
        FIXUP(pdib->m_pszAuthor,	pdib);
        FIXUP(pdib->m_pszDemoType,	pdib);

        if ((DWORD) -1 != (DWORD) pdib->m_pszPersist)
        {
            FIXUP(pdib->m_pszPersist, pdib);
        }
        else
        {
            pdib->m_pszPersist = NULL;
        }

        if ((DWORD) -1 != (DWORD) pdib->m_pszAddi)
        {
            FIXUP(pdib->m_pszAddi, pdib);
        }
        else
        {
            pdib->m_pszAddi = NULL;
        }

        pdib = (DIB *) pdib->m_pNext;
    }
}

DWORD XDIReadFile(const char *pszXDIFile, void *pvDemos, DWORD dwFileSize)
{
    HANDLE hFile;
    DWORD  dwNumRead;
    LONG   dwRc = -1;

    assert(pszXDIFile);
    assert(pvDemos || (dwFileSize==0));

	hFile = CreateFile(pszXDIFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
					   OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (INVALID_HANDLE_VALUE != hFile)
    {
		// MJL 7/16/2001
		// Helper to get file size
		if (pvDemos == NULL)
		{
			dwRc = GetFileSize(hFile, NULL);
	        CloseHandle(hFile);
			return dwRc;
		}

        if (ReadFile(hFile, pvDemos, dwFileSize, &dwNumRead, NULL))
        {
			if (dwFileSize == dwNumRead && VerifyContents((BYTE *) pvDemos, dwFileSize))
			{
	            dwRc = dwNumRead;
	            FixupXDIBlob(pvDemos);
			}
        }

        CloseHandle(hFile);
    }

    return dwRc;
}

#else

//
//Functions in here are called from the XDIGen Visual Basic app...
//

#include "resource.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

_declspec(dllexport) long __stdcall XDIGetDllVersion(void)
{
    return MAKELONG(VERMINOR, VERMAJOR);
}

//
//Get the lIdx-th demo info
//
_declspec(dllexport) long __stdcall XDIGetDemoInfo(char *pszXDIFile, DIB *pdib, LONG lIdx)
{
    HANDLE hFile;
    DWORD  dwNumRead;
    DWORD  dwSize;
    BOOL   fOK;
    DIB    *pReadDib = NULL;
    XDIHEADER xdi;

    assert(pszXDIFile);
    assert(pdib);

	hFile = CreateFile(pszXDIFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
					   OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (fOK = (INVALID_HANDLE_VALUE != hFile))
    {
        //
        //Read past header...
        //
        fOK = ReadFile(hFile, (XDIHEADER *) &xdi, sizeof(XDIHEADER), &dwNumRead, NULL);

        //
        //Loop while no read errors and until we have the desired demo information block
        //
        while (fOK && lIdx--)
        {
            fOK = FALSE;

            //
            //If we allocated memory last loop, free it before allocating new after ReadFile below
            //
            if (pReadDib)
            {
                delete pReadDib;
                pReadDib = NULL;
            }

            //
            //Read just the size of the demo info
            //
            if (ReadFile(hFile, &dwSize, sizeof(DWORD), &dwNumRead, NULL))
            {
                assert(sizeof(DWORD) == dwNumRead);

                //
                //Now allocate a new DIB that is the size specified
                //
                pReadDib = (DIB *) new BYTE[dwSize];

                //
                //Read the demo information as it was written to file, don't read the size because the file
                //seek pointer is already set just beyond that because of read above
                //
                if (pReadDib && ReadFile(hFile, (BYTE *) pReadDib + sizeof(DWORD), dwSize - sizeof(DWORD), &dwNumRead, NULL))
                {
                    assert(dwSize - sizeof(DWORD) == dwNumRead);

                    fOK = TRUE;
                }
            }
        }

        if (fOK)
        {
            pdib->m_dwSize = dwSize;
            pdib->m_dwFlags = pReadDib->m_dwFlags;
            pdib->m_dwPriority = pReadDib->m_dwPriority;

            //
            //Memory was preallocated in Visual Basic for the pdib strings.  Copy the relevant strings
            //from the m_data blob into the stucture for passing back to VB.
            //
            lstrcpy(pdib->m_pszTitle, (char *) pReadDib + (DWORD) pReadDib->m_pszTitle);
            lstrcpy(pdib->m_pszFolder, (char *) pReadDib + (DWORD) pReadDib->m_pszFolder);
            lstrcpy(pdib->m_pszXBE, (char *) pReadDib + (DWORD) pReadDib->m_pszXBE);
            lstrcpy(pdib->m_pszTeaser, (char *) pReadDib + (DWORD) pReadDib->m_pszTeaser);
            lstrcpy(pdib->m_pszAuthor, (char *) pReadDib + (DWORD) pReadDib->m_pszAuthor);

            if ((DWORD) -1 != (DWORD) pReadDib->m_pszPersist)
            {
                lstrcpy(pdib->m_pszPersist, (char *) pReadDib + (DWORD) pReadDib->m_pszPersist);
            }
            else
            {
                lstrcpy(pdib->m_pszPersist, "");
            }

            if ((DWORD) -1 != (DWORD) pReadDib->m_pszAddi)
            {
                lstrcpy(pdib->m_pszAddi, (char *) pReadDib + (DWORD) pReadDib->m_pszAddi);
            }
            else
            {
                lstrcpy(pdib->m_pszAddi, "");
            }
        }

        if (pReadDib)
        {
            delete pReadDib;
        }

        CloseHandle(hFile);
    }

    return fOK;
}

_declspec(dllexport) long __stdcall XDIGetDemoHeader(char *pszXDIFile, XDIHEADER *pxdi)
{
    HANDLE hFile;
    DWORD dwNumRead;
	DWORD dwFileSize;
    XDIHEADER *pxdiRead;
    BOOL  fRc = FALSE;

    assert(pszXDIFile);
    assert(pxdi);

	hFile = CreateFile(pszXDIFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
					   OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (INVALID_HANDLE_VALUE != hFile)
    {
		dwFileSize = GetFileSize(hFile, NULL);
		BYTE *pbDemos = (BYTE *) new BYTE[dwFileSize];

        fRc = pbDemos && ReadFile(hFile, pbDemos, dwFileSize, &dwNumRead, NULL);

		if (fRc && dwFileSize == dwNumRead && VerifyContents(pbDemos, dwFileSize))
		{
			pxdiRead = (XDIHEADER *) pbDemos;

			pxdi->m_lNumDemos = pxdiRead->m_lNumDemos;
			pxdi->m_fKiosk = pxdiRead->m_fKiosk;
			pxdi->m_lVerMajor = pxdiRead->m_lVerMajor;
			pxdi->m_lVerMinor = pxdiRead->m_lVerMinor;
			lstrcpy(pxdi->m_szGameName, pxdiRead->m_szGameName);
			lstrcpy(pxdi->m_szPersist, pxdiRead->m_szPersist);
		}
		else
		{
			fRc = FALSE;
		}

		delete pbDemos;
        CloseHandle(hFile);
    }

    return (LONG) (INVALID_HANDLE_VALUE != hFile && fRc);
}

//
//Write the header info with the number of demos that is passed in from visual basic
//
_declspec(dllexport) long __stdcall XDIWriteHeader(LPSTR pszXDIFile, 
                                                   LONG lNumDemos, 
                                                   LONG lKiosk,
                                                   char *pszCallingXBE, 
                                                   char *pszPersist)
{
    HANDLE hFile;
    XDIHEADER xdi;
    DWORD dwNumWritten;
    BOOL  fRc = FALSE;

    xdi.m_lNumDemos = lNumDemos;
    xdi.m_fKiosk = lKiosk;
    xdi.m_lVerMajor = VERMAJOR;
    xdi.m_lVerMinor = VERMINOR;
    lstrcpy(xdi.m_szGameName, pszCallingXBE);
    lstrcpy(xdi.m_szPersist, pszPersist);

    //
    //Always recreate the file when writing the header
    //
	hFile = CreateFile(pszXDIFile, GENERIC_READ|GENERIC_WRITE, 0, NULL, 
					   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE != hFile)
    {
        fRc = WriteFile(hFile, &xdi, sizeof(XDIHEADER), &dwNumWritten, NULL);

        if (sizeof(XDIHEADER) == dwNumWritten)
		{
			WriteChecksum(hFile);
		}

        CloseHandle(hFile);
    }

    return (LONG) (INVALID_HANDLE_VALUE != hFile && fRc);
}

//
//Write the next demo info, appended to the end of the file
//
_declspec(dllexport) long __stdcall XDIWriteDemoInfo(char *pszXDIFile, DIB *pdib)
{
    HANDLE hFile;
    DWORD dwNumWritten;
    DIB   *pNewDib;
    char  szDemoType[nMAX_STRINGLEN];

    //
    //Find the handle to this dll (XDIDll.Dll) and load the descriptive demo type string from the string table
    //
    HMODULE hDll = GetModuleHandle("XDIDll");
    LoadString(hDll, pdib->m_dwFlags & DIBFLAGS_MOVIE ? IDS_MOVIE : IDS_DEMO, szDemoType, nMAX_STRINGLEN);

    //
    //Calculate size of this demo information block, include space for trailing zeros
    //
    pdib->m_dwSize = sizeof(DIB) + lstrlen(pdib->m_pszTitle) + 1 +
                                   lstrlen(pdib->m_pszFolder) + 1 +
                                   lstrlen(pdib->m_pszXBE) + 1 + 
                                   lstrlen(pdib->m_pszTeaser) + 1 +
                                   lstrlen(pdib->m_pszPersist) + 1 +
                                   lstrlen(pdib->m_pszAddi) + 1 +
                                   lstrlen(pdib->m_pszAuthor) + 1 +
                                   lstrlen(szDemoType) + 1;

    //
    //Allocate space for the DIB, and copy the parameter dib into it
    //
    pNewDib = (DIB *) new BYTE[pdib->m_dwSize];
	if (NULL == pNewDib)
	{
		return 0L;
	}

    CopyMemory(pNewDib, pdib, sizeof(DIB));

    char *pPtr = (char *) &pNewDib->m_data;
    ZeroMemory(pPtr, pNewDib->m_dwSize - sizeof(DIB));

    lstrcpy(pPtr, pNewDib->m_pszTitle);
    pNewDib->m_pszTitle = (char *) (pPtr - (char *) pNewDib);
    pPtr += lstrlen(pPtr) + 1;

    lstrcpy(pPtr, pNewDib->m_pszFolder);
    pNewDib->m_pszFolder = (char *) (pPtr - (char *) pNewDib);
    pPtr += lstrlen(pPtr) + 1;

    lstrcpy(pPtr, pNewDib->m_pszXBE);
    pNewDib->m_pszXBE = (char *) (pPtr - (char *) pNewDib);
    pPtr += lstrlen(pPtr) + 1;

    lstrcpy(pPtr, pNewDib->m_pszTeaser);
    pNewDib->m_pszTeaser = (char *) (pPtr - (char *) pNewDib);
    pPtr += lstrlen(pPtr) + 1;

    lstrcpy(pPtr, pNewDib->m_pszPersist);
    pNewDib->m_pszPersist = *pNewDib->m_pszPersist ? (char *) (pPtr - (char *) pNewDib) : (char *) -1;
    pPtr += lstrlen(pPtr) + 1;

    lstrcpy(pPtr, pNewDib->m_pszAddi);
    pNewDib->m_pszAddi = *pNewDib->m_pszAddi ? (char *) (pPtr - (char *) pNewDib) : (char *) -1;
    pPtr += lstrlen(pPtr) + 1;

    lstrcpy(pPtr, pNewDib->m_pszAuthor);
    pNewDib->m_pszAuthor = (char *) (pPtr - (char *) pNewDib);
    pPtr += lstrlen(pPtr) + 1;

    lstrcpy(pPtr, szDemoType);
    pNewDib->m_pszDemoType = (char *) (pPtr - (char *) pNewDib);

    //
    //We OPEN_EXISTING, because a call to XDIWriteHeader took place before this call
    //
	hFile = CreateFile(pszXDIFile, GENERIC_READ|GENERIC_WRITE, 0, NULL, 
					   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE != hFile)
    {
        SetFilePointer(hFile, 0, NULL, FILE_END);
        WriteFile(hFile, pNewDib, pNewDib->m_dwSize, &dwNumWritten, NULL);

        if (pNewDib->m_dwSize == dwNumWritten)
		{
			WriteChecksum(hFile);
		}

        CloseHandle(hFile);
    }

	delete pNewDib;
    return (LONG) (INVALID_HANDLE_VALUE != hFile);
}

#endif