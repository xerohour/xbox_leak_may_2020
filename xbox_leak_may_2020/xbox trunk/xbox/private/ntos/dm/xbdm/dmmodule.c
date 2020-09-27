/*
 *
 * dmmodule.c
 *
 * Get information about loaded modules and about memory
 *
 */

#include "dmp.h"

/* We can't allow the Ex/Dm pool defines here */
#undef ExAllocatePool
#undef ExAllocatePoolWithTag
#undef ExFreePool

NTSTATUS
StLoadImage(
	PCOSTR oszName,
	PCOSTR oszFor,
	PLDR_DATA_TABLE_ENTRY *ppldte
	);

void
UnloadReferencedModules(
	PLDR_DATA_TABLE_ENTRY pldte
	);

RTL_CRITICAL_SECTION csLoader;

void InitLoader(void)
{
	RtlInitializeCriticalSection(&csLoader);
}

struct {
	PVOID pfnImport;
	PVOID pfnSub;
} rgfsub[] = {
	{ NULL, DmAllocatePool },
	{ NULL, DmAllocatePoolWithTag },
	{ NULL, DmFreePool },
};

/* In certain cases, we may want to call the functions we've overridden.  Here
 * are the stubs */
void DmExFreePool(PVOID p)
{
	void (*pfnExFreePool)(PVOID) = rgfsub[2].pfnImport;
	pfnExFreePool(p);
}

PLDR_DATA_TABLE_ENTRY PldteGetModule(LPCSTR sz, BOOL fMatchExt)
{
	PLDR_DATA_TABLE_ENTRY pldte = NULL;
	PLIST_ENTRY ple;
	WCHAR *wz;
	int cch;
	const char *pch;

	ple = g_dmi.LoadedModuleList->Flink;
	while(ple != g_dmi.LoadedModuleList) {
		pldte = CONTAINING_RECORD(ple, LDR_DATA_TABLE_ENTRY,
			InLoadOrderLinks);
		ple = ple->Flink;
		wz = pldte->BaseDllName.Buffer;
		cch = pldte->BaseDllName.Length >> 1;
		pch = sz;
		while(cch) {
			char ch1, ch2;
			ch1 = (char )*wz++;
			if(ch1 >= 'a' && ch1 <= 'z')
				ch1 -= 'a' - 'A';
			ch2 = *pch++;
			if(ch2 >= 'a' && ch2 <= 'z')
				ch2 -= 'a' - 'A';
			if(ch1 != ch2)
				break;
			--cch;
		}
		if(cch == 0) {
			if(*pch == 0)
				/* We've matched the whole name */
				break;
		} else if(!fMatchExt && pch[-1] == 0 && wz[-1] == '.')
				/* We've matched the basename */
				break;
		pldte = NULL;
	}
	return pldte;
}

HRESULT DmGetMemory(LPCVOID pb, DWORD cb, LPVOID lpbBuf, LPDWORD pcbRet)
{
	DWORD cbRet;
	BOOL fIsPageValid = TRUE;
	DWORD dwPageBase = (DWORD) pb + 0x1000;

	if (!lpbBuf)
		return E_INVALIDARG;

	for(cbRet = 0; cb-- && fIsPageValid; ++(DWORD)pb) {
		if((dwPageBase ^ (DWORD)pb) & 0xfffff000) {
			dwPageBase = (DWORD)pb & 0xfffff000;
			fIsPageValid = MmIsAddressValid((BYTE *)pb);
		}
		if(fIsPageValid)
			fIsPageValid = FGetMemory((BYTE *)pb, lpbBuf);
		if(fIsPageValid)
			++cbRet, ++(BYTE *)lpbBuf;
	}

	if(pcbRet)
		*pcbRet = cbRet;

	return !cb || pcbRet ? XBDM_NOERR : XBDM_MEMUNMAPPED;
}

HRESULT DmSetMemory(LPVOID pb, DWORD cb, LPCVOID lpbBuf, LPDWORD pcbRet)
{
	DWORD cbRet;
	BOOL fIsPageValid = TRUE;
	DWORD dwPageBase = (DWORD) pb + 0x1000;

	if (!lpbBuf)
		return E_INVALIDARG;

	for(cbRet = 0; cb-- && fIsPageValid; ++(DWORD)pb) {
		if((dwPageBase ^ (DWORD)pb) & 0xfffff000) {
			dwPageBase = (DWORD)pb & 0xfffff000;
			fIsPageValid = MmIsAddressValid(pb);
		}
		if(fIsPageValid)
			fIsPageValid = FSetMemory(pb, *(BYTE *)lpbBuf);
		if(fIsPageValid)
			++cbRet, ++(BYTE *)lpbBuf;
	}

	if(pcbRet)
		*pcbRet = cbRet;

	return !cb || pcbRet ? XBDM_NOERR : XBDM_MEMUNMAPPED;
}

void FixupBistroImage(void)
{
    /* VTune Call Graph needs to be able to modify the code pages in the image,
     * so we need to detect whether we're running an instrumented image.  We
     * look for a section marked .bistro.  If we see one, we go through all of
     * the section flags and mark them as writable */
    PXBEIMAGE_SECTION pxsh;
    int cxsh;

    cxsh = XeImageHeader()->NumberOfSections;
    pxsh = XeImageHeader()->SectionHeaders;

    for(; cxsh--; ++pxsh) {
        if(0 == memcmp(pxsh->SectionName, ".bistro", 8)) {
            /* We found it */
            cxsh = XeImageHeader()->NumberOfSections;
            pxsh = XeImageHeader()->SectionHeaders;
            for(; cxsh--; ++pxsh)
                pxsh->SectionFlags |= XBEIMAGE_SECTION_WRITEABLE;
            return;
        }
    }
}

DWORD DwXmhFlags(PDMN_MODLOAD pdmml)
{
	DWORD dwFlags = DMN_MODFLAG_XBE;
    PIMAGE_DOS_HEADER pdosh;
    PIMAGE_NT_HEADERS pnth;

	if (XeImageHeader()->TlsDirectory)
		dwFlags |= DMN_MODFLAG_TLS;

    /* See if we appear to have PE headers here */
    do {
        pdosh = pdmml->BaseAddress;
        if(pdosh->e_magic != IMAGE_DOS_SIGNATURE)
            break;
        pnth = (PIMAGE_NT_HEADERS)((ULONG)pdosh + pdosh->e_lfanew);
        if(pnth->Signature != IMAGE_NT_SIGNATURE)
            break;
        if(pnth->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    		dwFlags |= DMN_MODFLAG_PEHEADER;
    } while (0);

	return dwFlags;
}

struct _DM_WALK_MODULES {
	PLIST_ENTRY ple;
};

HRESULT DmWalkLoadedModules(PDM_WALK_MODULES *ppdmwm, DMN_MODLOAD *pdmml)
{
	PDM_WALK_MODULES pdmwm;
	PLDR_DATA_TABLE_ENTRY pldte;
	PIMAGE_NT_HEADERS pnth;

	if(!ppdmwm || !pdmml)
		return E_INVALIDARG;

	pdmwm = *ppdmwm;
	if(!pdmwm) {

		/* Set up our list */
		pdmwm = DmAllocatePoolWithTag(sizeof *pdmwm, 'mwmd');
		if(!pdmwm)
			return E_OUTOFMEMORY;
		pdmwm->ple = g_dmi.LoadedModuleList->Flink;
		*ppdmwm = pdmwm;
	}

	for(;;) {

		if(pdmwm->ple == g_dmi.LoadedModuleList)
			break;

		pldte = CONTAINING_RECORD(pdmwm->ple, LDR_DATA_TABLE_ENTRY,
			InLoadOrderLinks);
		pdmwm->ple = pdmwm->ple->Flink;
		FillSzFromWz(pdmml->Name, sizeof pdmml->Name,
			pldte->BaseDllName.Buffer, pldte->BaseDllName.Length >> 1);
		pdmml->BaseAddress = pldte->DllBase;
		if(!(pldte->Flags & LDRP_ENTRY_XE_IMAGE)) {
			pdmml->Size = pldte->SizeOfImage;
			pnth = RtlImageNtHeader(pldte->DllBase);
			if(pnth) {
				pdmml->CheckSum = (ULONG)pnth->OptionalHeader.CheckSum;
				pdmml->TimeStamp = (ULONG)pnth->FileHeader.TimeDateStamp;
			} else {
				pdmml->CheckSum = 0;
				pdmml->TimeStamp = 0;
			}
			pdmml->Flags = 0;
			return XBDM_NOERR;
		} else {
			pdmml->Size = XeImageHeader()->NtSizeOfImage;
			pdmml->TimeStamp = XeImageHeader()->NtTimeDateStamp;
			pdmml->CheckSum = XeImageHeader()->NtCheckSum;
			pdmml->Flags = DwXmhFlags(pdmml);
			return XBDM_NOERR;
		}
	}

	/* We didn't find anything to return, so say end of list */
	return XBDM_ENDOFLIST;
}

HRESULT DmCloseLoadedModules(PDM_WALK_MODULES pdmwm)
{
	if (!pdmwm)
		DmFreePool(pdmwm);
	return XBDM_NOERR;
}

struct _DM_WALK_MODSECT {
	PLDR_DATA_TABLE_ENTRY pldte;
	union {
		struct {
			/* XE module */
			PXBEIMAGE_SECTION pxsh;
			int cxsh;
			int ixsh;
		};
		struct {
			/* kernel module */
			int ish;
			PIMAGE_NT_HEADERS pnth;
			PIMAGE_SECTION_HEADER psh;
			int csh;
		};
	};
};

void RewindDmws(PDM_WALK_MODSECT pdmws)
{
	if(pdmws->pldte->Flags & LDRP_ENTRY_XE_IMAGE) {
		pdmws->cxsh = XeImageHeader()->NumberOfSections;
		pdmws->pxsh = XeImageHeader()->SectionHeaders;
		pdmws->ixsh = 0;
	} else {
		pdmws->pnth = RtlImageNtHeader(pdmws->pldte->DllBase);
		if(pdmws->pnth) {
			pdmws->psh = IMAGE_FIRST_SECTION(pdmws->pnth);
			pdmws->csh = pdmws->pnth->FileHeader.NumberOfSections;
			pdmws->ish = 0;
		}
	}
}

void DmslFromXsh(PXBEIMAGE_SECTION pxsh, PDMN_SECTIONLOAD pdmsl)
{
	strncpy(pdmsl->Name, pxsh->SectionName, MAX_PATH);
	pdmsl->Name[MAX_PATH-1] = 0;
	pdmsl->BaseAddress = (PVOID)pxsh->VirtualAddress;
	pdmsl->Size = pxsh->VirtualSize;
    pdmsl->Index = pxsh - XeImageHeader()->SectionHeaders;
	pdmsl->Flags = pxsh->SectionReferenceCount ? DMN_SECFLAG_LOADED : 0;
}

HRESULT DmWalkModuleSections(PDM_WALK_MODSECT *ppdmws, LPCSTR szModule,
	PDMN_SECTIONLOAD pdmsl)
{
	PDM_WALK_MODSECT pdmws;

	if (!ppdmws || !pdmsl)
		return E_INVALIDARG;

	pdmws = *ppdmws;
	if(!pdmws) {
		PLDR_DATA_TABLE_ENTRY pldte;

		if (!szModule)
			return E_INVALIDARG;

		/* First find the module */
		pldte = PldteGetModule(szModule, TRUE);
		if(!pldte)
			/* Never found a match */
			return XBDM_NOMODULE;

		/* Now we can set things up */
		pdmws = DmAllocatePoolWithTag(sizeof *pdmws, 'swmd');
		if(!pdmws)
			return E_OUTOFMEMORY;
		*ppdmws = pdmws;
		pdmws->pldte = pldte;
		RewindDmws(pdmws);
	}

	for(;;) {
		if(pdmws->pldte->Flags & LDRP_ENTRY_XE_IMAGE) {
			/* This is an XE section */
			PXBEIMAGE_SECTION pxsh;

			if(!pdmws->cxsh)
				/* Nothing left */
				break;

			pxsh = pdmws->pxsh++;
			--pdmws->cxsh;

			/* We want to report this section */
			DmslFromXsh(pxsh, pdmsl);
            ++pdmws->ixsh;
			return XBDM_NOERR;
		} else {
			/* Make sure we actually have headers */
			if(!pdmws->pnth)
				return XBDM_NOMODULE;
			if(pdmws->csh) {
				/* We want to report this section */
				strcpy(pdmsl->Name, pdmws->psh->Name);
				pdmsl->Name[8] = 0;
				pdmsl->BaseAddress = (PVOID)((PBYTE)pdmws->pldte->DllBase +
					pdmws->psh->PointerToRawData);
				pdmsl->Size = pdmws->psh->Misc.VirtualSize;
				pdmsl->Index = ++pdmws->ish;
				pdmsl->Flags = 0;
				++pdmws->psh;
				--pdmws->csh;
				return XBDM_NOERR;
			}
			break;
		}
	}
	return XBDM_ENDOFLIST;
}

HRESULT DmCloseModuleSections(PDM_WALK_MODSECT pdmws)
{
	if (pdmws)
		DmFreePool(pdmws);
	return XBDM_NOERR;
}

HRESULT DmGetModuleLongName(LPCSTR szShort, LPSTR szLong, LPDWORD pcchLong)
{
	PLDR_DATA_TABLE_ENTRY pldte;
	LPCSTR sz;
	DWORD cch;
	DWORD cchMax;

	if (!szShort || !szLong || !pcchLong)
		return E_INVALIDARG;

	/* First find the module */
	pldte = PldteGetModule(szShort, TRUE);

	if(!pldte)
		return XBDM_NOMODULE;

	/* No XE means no data */
	if(!(pldte->Flags & LDRP_ENTRY_XE_IMAGE))
		return E_FAIL;

	/* Look for the long name */
	sz = XeImageHeader()->DebugPathName;
	if(!sz)
		return E_FAIL;

	cchMax = *pcchLong - 1;
	for(cch = 0; *sz && cch < cchMax; ++cch)
		*szLong++ = *sz++;
	*szLong = 0;
	*pcchLong = cch;
	return XBDM_NOERR;
}

HRESULT DmGetXbeInfo(LPCSTR szName, PDM_XBE pxbe)
{
	NTSTATUS st;
	HANDLE h;
	IO_STATUS_BLOCK iosb;
	POBJECT_STRING objectName;
	XBEIMAGE_HEADER xh;
	PXBEIMAGE_HEADER pxh;
	HRESULT hr;

	if (!pxbe)
		return E_INVALIDARG;

	/* If we already have an XBE in memory, we'll use its information.
		report the name it came from */
	pxh = XeImageHeader();

	if(MmDbgReadCheck(pxh) && !(g_dmi.Flags & DMIFLAG_RUNSHELL))
	{
		//
		// Grab the image name out the	LDR_CURRENT_IMAGE structure.
		//
		objectName = XeImageFileName;
		FObNameToFileName(objectName, pxbe->LaunchPath, sizeof(pxbe->LaunchPath));
		pxbe->TimeStamp = pxh->TimeDateStamp;
		pxbe->CheckSum = 0;
		pxbe->StackSize = pxh->SizeOfStackCommit;
		return XBDM_NOERR;
	}

	st = FCreateFile(&h, GENERIC_READ | SYNCHRONIZE, szName, NULL, 0, 0,
		 FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);
	if(!NT_SUCCESS(st))
		return XBDM_NOSUCHFILE;
	/* Make sure we can read an XE header out of this thing */
	st = NtReadFile(h, NULL, NULL, NULL, &iosb, &xh, sizeof xh, NULL);
	if(NT_SUCCESS(st) && xh.Signature == XBEIMAGE_SIGNATURE) {
		int ich;
		for(ich = 0; *szName && ich < sizeof pxbe->LaunchPath - 1; ++ich)
			pxbe->LaunchPath[ich] = *szName++;
		pxbe->LaunchPath[ich] = 0;
		pxbe->TimeStamp = xh.TimeDateStamp;
		pxbe->CheckSum = 0;
		pxbe->StackSize = xh.SizeOfStackCommit;
		hr = XBDM_NOERR;
	} else
		hr = XBDM_NOSUCHFILE;
	NtClose(h);
	return hr;
}

BOOL FResolveImport(PVOID pvBase, PIMAGE_EXPORT_DIRECTORY ped, ULONG cbExports,
	LPCSTR szName, PVOID *ppvRet)
{
	ULONG *rgichNames;
	USHORT *rgwOrdinals;
	ULONG *rgpvFunctions;
	ULONG iMic, iMid, iMac;
	ULONG iSym;

	if(((ULONG)szName & 0xffff0000) == 0)
		/* ordinal */
		iSym = (ULONG)szName - ped->Base;
	else {
		rgichNames = (PVOID)((PBYTE)pvBase + ped->AddressOfNames);
		rgwOrdinals = (PVOID)((PBYTE)pvBase + ped->AddressOfNameOrdinals);

		//
		// Lookup the import name in the name table using a binary search.
		//

		iMic = 0;
		iMid = 0;
		iMac = ped->NumberOfNames;

		while (iMac > iMic) {
			int sgn;

			//
			// Compute the next probe index and compare the import name
			// with the export name entry.
			//

			iMid = (iMic + iMac) >> 1;
			sgn = strcmp(szName, (PCHAR)((PCHAR)pvBase + rgichNames[iMid]));

			if (sgn < 0) {
				iMac = iMid;

			} else if (sgn > 0) {
				iMic = iMid + 1;

			} else {
				break;
			}
		}

		//
		// If the iMac index is less than the iMic index, then a matching
		// table entry was not found. Otherwise, get the ordinal number
		// from the ordinal table.
		//

		if (iMac <= iMic)
			return FALSE;
		else
			iSym = rgwOrdinals[iMid];
	}

	if(iSym >= ped->NumberOfFunctions)
		return FALSE;

	rgpvFunctions = (PVOID)((PBYTE)pvBase + ped->AddressOfFunctions);
	*ppvRet = (PBYTE)pvBase + rgpvFunctions[iSym];

	if(*ppvRet > (PVOID)ped && *ppvRet < (PVOID)((PBYTE)ped + cbExports)) {
		/* This is a forwarder */
		int cchDll;
		PSTR szSym;
		char szDll[64];
		PLDR_DATA_TABLE_ENTRY pldteF;

		szSym = strchr(*ppvRet, '.');
		if(!szSym)
			return FALSE;
		cchDll = szSym++ - (PCHAR)*ppvRet;
		if(cchDll > sizeof szDll - 1)
			return FALSE;
		memcpy(szDll, *ppvRet, cchDll);
		szDll[cchDll] = 0;
		pldteF = PldteGetModule(szDll, FALSE);
		if(!pldteF)
			return FALSE;
		return DmGetProcAddress((HANDLE)pldteF, szSym, ppvRet);
	} else {
		/* Check to see whether we need to do import substitution on this
		 * guy */
		int ifsub = sizeof rgfsub / sizeof rgfsub[0];
		while(ifsub--) {
			if(*ppvRet == rgfsub[ifsub].pfnImport) {
				*ppvRet = rgfsub[ifsub].pfnSub;
				break;
			}
		}
	}

	return TRUE;
}

BOOL FResolveImageImports(PLDR_DATA_TABLE_ENTRY pldteImage, PCOSTR oszName,
	PLIST_ENTRY LoadedModuleList)
{
	PIMAGE_IMPORT_DESCRIPTOR pid;
	PIMAGE_EXPORT_DIRECTORY ped;
	ULONG cbImports;
	ULONG cbExports;
	PLDR_DATA_TABLE_ENTRY pldte;
	PIMAGE_THUNK_DATA pthILT;
	PIMAGE_THUNK_DATA pthIAT;
	PBYTE pbImageBase = pldteImage->DllBase;
	int ipldte;
	BOOL fLoadFailed = FALSE;

	/* Loop through all of the imports of this library and fix them up, loading
	 * the supporting libs as necessary */
	pid = (PIMAGE_IMPORT_DESCRIPTOR)RtlImageDirectoryEntryToData(pbImageBase,
		TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &cbImports);
	if(!pid)
		return FALSE;

	if(!LoadedModuleList)
		LoadedModuleList = g_dmi.LoadedModuleList;

	if(g_dmi.LoadedModuleList) {
		/* Build a list of imported libraries */
		PIMAGE_IMPORT_DESCRIPTOR pidT = pid;
		for(ipldte = 0; pidT->Name && pidT->FirstThunk; ++ipldte, ++pidT);
		pldteImage->LoadedImports = DmAllocatePoolWithTag((ipldte + 1) *
			sizeof(PVOID), 'TDmM');
		if(!pldteImage)
			return FALSE;
		RtlZeroMemory(pldteImage->LoadedImports, (ipldte + 1) * sizeof(PVOID));
		*(int *)pldteImage->LoadedImports = ipldte;
	} else
		/* This is not an unloadable module */
		pldteImage->LoadedImports = NULL;

	ipldte = 1;
	while(pid->Name && pid->FirstThunk) {
		/* Get the loader entry for this library */
		if(g_dmi.LoadedModuleList) {
			char *szName = pbImageBase + pid->Name;
			RtlEnterCriticalSection(&csLoader);
			/* See if this guy is already loaded */
			_asm {
				pushfd
				cli
			}
			pldte = PldteGetModule(szName, TRUE);
			if(pldte)
				++pldte->LoadCount;
			_asm popfd
			if(!pldte) {
				/* Not already loaded, so we need to load it */
				if(!NT_SUCCESS(StLoadImage(szName, oszName, &pldte)))
					pldte = NULL;
			}
			RtlLeaveCriticalSection(&csLoader);
		} else {
			/* We're relocating ourselves, so we need to look up the
			 * entry.  But we should safely assume that the first entry in
			 * the list is the kernel */
			pldte = CONTAINING_RECORD(LoadedModuleList->Flink,
				LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		}
		if(!pldte)
			return FALSE;
		if(pldteImage->LoadedImports)
			((PVOID *)pldteImage->LoadedImports)[ipldte++] = pldte;

		/* Get the export directory from the library we've referenced */
		ped = (PIMAGE_EXPORT_DIRECTORY)
			RtlImageDirectoryEntryToData(pldte->DllBase, TRUE,
			IMAGE_DIRECTORY_ENTRY_EXPORT, &cbExports);
		if(!ped) {
			UnloadReferencedModules(pldteImage);
			return FALSE;
		}

		/* Now loop over all of the import thunks and fix them up */
		pthILT = (PIMAGE_THUNK_DATA)(pbImageBase + pid->OriginalFirstThunk);
		pthIAT = (PIMAGE_THUNK_DATA)(pbImageBase + pid->FirstThunk);
		while(pthILT->u1.AddressOfData) {
			char *szName;
			if(IMAGE_SNAP_BY_ORDINAL(pthILT->u1.Ordinal))
				szName = (char *)IMAGE_ORDINAL(pthILT->u1.Ordinal);
			else {
				PIMAGE_IMPORT_BY_NAME pibn = (PIMAGE_IMPORT_BY_NAME)
					(pbImageBase + pthILT->u1.AddressOfData);
				szName = pibn->Name;
			}
			if(!FResolveImport(pldte->DllBase, ped, cbExports, szName,
				(PVOID *)&pthIAT->u1.Function))
			{
#if DBG
				char szError[256];
				char szFn[32];
				STRING st, *pst;

				if(((ULONG)szName & 0xffff0000) == 0) {
					sprintf(szFn, "ordinal %d", szName);
					szName = szFn;
				}
				sprintf(szError, "missing %s in %s\n", szName,
					pbImageBase + pid->Name);
				st.Buffer = szError;
				st.MaximumLength = sizeof szError - 1;
				st.Length = (USHORT)strlen(szError);
				pst = &st;
				_asm {
					mov eax, BREAKPOINT_PRINT
					mov ecx, pst
					int 2dh
					int 3
				}
#endif // DBG
				fLoadFailed = TRUE;
			}
			++pthILT;
			++pthIAT;
		}
		if(fLoadFailed) {
			UnloadReferencedModules(pldteImage);
			return FALSE;
		}
		++pid;
	}
	return TRUE;
}

BOOL FFinishImageLoad(PLDR_DATA_TABLE_ENTRY pldteT, LPCSTR szName,
	PLDR_DATA_TABLE_ENTRY *ppldteOut)
{
	PLDR_DATA_TABLE_ENTRY pldte;
	UNICODE_STRING ustName;
	UNICODE_STRING ustFullName;
	ANSI_STRING ast;
	NTSTATUS st;
	PBYTE pbBase = pldteT->DllBase;
	PIMAGE_NT_HEADERS pnth;
	const char *pch, *pchBase;
	ULONG ul;

	for(pch = pchBase = szName; *pch; ++pch)
		if(*pch == '\\')
			pchBase = pch + 1;

	RtlInitAnsiString(&ast, pchBase);
	ustName.MaximumLength = RtlAnsiStringToUnicodeSize(&ast);
	ustName.Buffer = DmAllocatePool(ustName.MaximumLength);
	if(!ustName.Buffer)
		return FALSE;
	st = RtlAnsiStringToUnicodeString(&ustName, &ast, FALSE);
	if(!NT_SUCCESS(st))
		goto RetFree1;

	RtlInitAnsiString(&ast, szName);
	ustFullName.MaximumLength = RtlAnsiStringToUnicodeSize(&ast);
	ustFullName.Buffer = DmAllocatePool(ustFullName.MaximumLength);
	if(!ustFullName.Buffer)
		goto RetFree1;
	st = RtlAnsiStringToUnicodeString(&ustFullName, &ast, FALSE);
	if(!NT_SUCCESS(st))
		goto RetFree2;

	pldte = DmAllocatePoolWithTag(sizeof *pldte, 'dLmM');
	if(!pldte) {
RetFree2:
		DmFreePool(ustFullName.Buffer);
RetFree1:
		DmFreePool(ustName.Buffer);
		return FALSE;
	}

	RtlZeroMemory(pldte, sizeof *pldte);
	pldte->BaseDllName = ustName;
	pldte->FullDllName = ustFullName;
	pldte->DllBase = pldteT->DllBase;
	pnth = RtlImageNtHeader(pbBase);
	pldte->EntryPoint = pbBase + pnth->OptionalHeader.AddressOfEntryPoint;
	pldte->SizeOfImage = pnth->OptionalHeader.SizeOfImage;
	pldte->CheckSum = pnth->OptionalHeader.CheckSum;
	pldte->Flags = LDRP_ENTRY_PROCESSED | LDRP_SYSTEM_MAPPED;
	pldte->LoadCount = 1;
	pldte->LoadedImports = pldteT->LoadedImports;
	ExInterlockedInsertTailList(g_dmi.LoadedModuleList, &pldte->InLoadOrderLinks);
	if(RtlImageDirectoryEntryToData(pldte->DllBase, TRUE,
		IMAGE_DIRECTORY_ENTRY_DEBUG, &ul))
	{
		DbgLoadImageSymbols(&ast, pldte->DllBase, (ULONG_PTR)-1);
		pldte->Flags |= LDRP_DEBUG_SYMBOLS_LOADED;
	}
	if(ppldteOut)
		*ppldteOut = pldte;
	return TRUE;
}

BOOL FFixupXbdm(PVOID pvBase, PDMINIT pdmi)
{
	LDR_DATA_TABLE_ENTRY ldte;
	PVOID *rgppv[3];
	int ifsub;

	ldte.DllBase = pvBase;
	/* Find our kernel imports */
	if(!FResolveImageImports(&ldte, NULL, pdmi->LoadedModuleList))
		return FALSE;
	/* Initialize the pool */
	InitPool();
	ldte.LoadedImports = (PVOID)-1;
	g_dmi.LoadedModuleList = pdmi->LoadedModuleList;
	if(!FFinishImageLoad(&ldte, "xbdm.dll", NULL))
		return FALSE;

	/* With the module loaded, we now need to set up the kernel routine
	 * substitution list, and then thunk ourselves */
	_asm {
		lea eax, ExAllocatePool
		lea ecx, ExAllocatePoolWithTag
		lea edx, ExFreePool
		mov rgppv, eax
		mov rgppv+4, ecx
		mov rgppv+8, edx
	}
	for(ifsub = 0; ifsub < 3; ++ifsub) {
		rgfsub[ifsub].pfnImport = *rgppv[ifsub];
		*rgppv[ifsub] = rgfsub[ifsub].pfnSub;
	}
}

void UnloadReferencedModules(PLDR_DATA_TABLE_ENTRY pldte)
{
	if(pldte->LoadedImports) {
		PLDR_DATA_TABLE_ENTRY *rgpldte = pldte->LoadedImports;
		int i;
		int c = (int)rgpldte[0];
		for(i = c; i; --i) {
			HANDLE h = (HANDLE)rgpldte[i];
			if(h)
				DmUnloadExtension(h);
		}
		DmFreePool(rgpldte);
		pldte->LoadedImports = NULL;
	}
}

HRESULT DmUnloadExtension(HANDLE hModule)
{
	PLDR_DATA_TABLE_ENTRY pldte = (PLDR_DATA_TABLE_ENTRY)hModule;

	if(pldte->LoadedImports == (PVOID)-1)
		/* This module can't be unloaded */
		return XBDM_NOERR;

	_asm {
		pushfd
		cli
	}
	if(--pldte->LoadCount == 0) {
		RemoveEntryList(&pldte->InLoadOrderLinks);
	} else
		pldte = NULL;
	_asm popfd
	if(pldte) {
		ANSI_STRING ast;
		char sz[256];

		ast.Length = 0;
		ast.MaximumLength = sizeof sz;
		ast.Buffer = sz;
		RtlUnicodeStringToAnsiString(&ast, &pldte->FullDllName, FALSE);
		DbgUnLoadImageSymbols(&ast, pldte->DllBase, (ULONG_PTR)-1);

		UnloadReferencedModules(pldte);

		MmDbgFreeMemory(pldte->DllBase, 0);
		DmFreePool(pldte->BaseDllName.Buffer);
		DmFreePool(pldte->FullDllName.Buffer);
		DmFreePool(pldte);
	}

	return XBDM_NOERR;
}

NTSTATUS StLoadImage(PCOSTR oszName, PCOSTR oszFor,
	PLDR_DATA_TABLE_ENTRY *ppldte)
{
	NTSTATUS st;
	HANDLE h;
	IO_STATUS_BLOCK iosb;
	OBJECT_ATTRIBUTES oa;
	OBJECT_STRING ost;
	FILE_NETWORK_OPEN_INFORMATION fni;
	LDR_DATA_TABLE_ENTRY ldte;

	*ppldte = NULL;
	if(oszFor) {
		const OCHAR *poch, *pochDir;
		int cchBase;
		int cchTot;

		for(poch = pochDir = oszFor; *poch; ++poch)
			if(*poch == '\\')
				pochDir = poch;
		cchBase = strlen(oszName);
		cchTot = (pochDir - oszFor) + cchBase + 1;
		poch = oszName;
		oszName = DmAllocatePool(cchTot + 1);
		if(!poch)
			return STATUS_INSUFFICIENT_RESOURCES;
		memcpy((POCHAR)oszName, oszFor, (pochDir - oszFor) + 1);
		memcpy((POCHAR)oszName + (cchTot - cchBase), poch, cchBase);
		((POCHAR)oszName)[cchTot] = 0;
	}

	RtlInitObjectString(&ost, oszName);
	InitializeObjectAttributes(&oa, &ost, OBJ_CASE_INSENSITIVE, NULL,
		NULL);
	st = NtOpenFile(&h, FILE_EXECUTE, &oa, &iosb, 0,
		FILE_SYNCHRONOUS_IO_NONALERT);
	if(!NT_SUCCESS(st))
		goto ErrRet;
	st = NtQueryInformationFile(h, &iosb, &fni, sizeof fni,
		FileNetworkOpenInformation);
	if(!NT_SUCCESS(st))
		goto ErrClose;
	ldte.DllBase = MmDbgAllocateMemory(fni.EndOfFile.LowPart, PAGE_READWRITE);
	if(ldte.DllBase == NULL) {
		st = STATUS_NO_MEMORY;
		goto ErrClose;
	}
	st = NtReadFile(h, NULL, NULL, NULL, &iosb, ldte.DllBase,
		fni.EndOfFile.LowPart, NULL);
	if(!NT_SUCCESS(st))
		goto ErrClose;
	try {
		/* Now that the image is loaded, we first relocate it */
		st = LdrRelocateImage(ldte.DllBase, "xbdm", STATUS_SUCCESS,
			STATUS_CONFLICTING_ADDRESSES, STATUS_INVALID_IMAGE_FORMAT);
		if(NT_SUCCESS(st)) {
			/* Now we resolve its imports */
			st = FResolveImageImports(&ldte, oszName, NULL) ? STATUS_SUCCESS :
				STATUS_DRIVER_ENTRYPOINT_NOT_FOUND;
			if(NT_SUCCESS(st) && !FFinishImageLoad(&ldte, oszName, ppldte)) {
				st = STATUS_INSUFFICIENT_RESOURCES;
				UnloadReferencedModules(&ldte);
			}
			if(NT_SUCCESS(st)) {
				BOOL fUnload = FALSE;
				/* We need to call this function's entry point.  Since we
				 * don't have any faith that the calling convention of the
				 * callee is sane, we do this via an asm thunk */
				CallDxtEntry((*ppldte)->EntryPoint, &fUnload);
				if(fUnload)
					st = 0x80001000;
			}
		}
	} except(EXCEPTION_EXECUTE_HANDLER) {
		st = GetExceptionCode();
	}
	if(!NT_SUCCESS(st)) {
		if(*ppldte) {
			/* We failed after registering the module.  Need to pull it from
			 * the list */
			DmUnloadExtension((HANDLE)*ppldte);
			*ppldte = NULL;
		} else
			MmDbgFreeMemory(ldte.DllBase, fni.EndOfFile.LowPart);
	}

ErrClose:
	NtClose(h);

ErrRet:
	if(oszFor)
		DmFreePool((PVOID)oszName);

	return st;
}

HRESULT DmLoadExtension(LPCSTR szName, PHANDLE phModule, PVOID *ppvBase)
{
	OCHAR oszName[256];
	NTSTATUS st;
	OBJECT_STRING ost;
	const char *pch, *pchBase;
	LDR_DATA_TABLE_ENTRY *pldte;

	if (!szName)
		return E_INVALIDARG;

	st = FFileNameToObName(szName, oszName, sizeof(oszName)/sizeof(OCHAR)) ?
		STATUS_SUCCESS : STATUS_OBJECT_PATH_INVALID;

	RtlEnterCriticalSection(&csLoader);
	/* See if this basename is already loaded */
	for(pchBase = NULL, pch = szName; *pch; ++pch)
		if(*pch == '\\')
			pchBase = pch + 1;
	_asm {
		pushfd
		cli
	}
	pldte = PldteGetModule(pchBase ? pchBase : szName, TRUE);
	if(pldte) {
		if(pchBase && NT_SUCCESS(st)) {
			/* The basename is there; if the full filenames are different,
			 * then we'll generate an error */
			WCHAR *wz;
			int cch;
			const OCHAR *poch;

			wz = pldte->FullDllName.Buffer;
			cch = pldte->FullDllName.Length >> 1;
			poch = szName;
			while(cch) {
				char ch1, ch2;
				ch1 = (char )*wz++;
				if(ch1 >= 'a' && ch1 <= 'z')
					ch1 -= 'a' - 'A';
				ch2 = (char )*pch++;
				if(ch2 >= 'a' && ch2 <= 'z')
					ch2 -= 'a' - 'A';
				if(ch1 != ch2)
					break;
				--cch;
			}
			if(cch || *pch)
				/* The names differ */
				st = STATUS_OBJECT_NAME_COLLISION;
		}
		if(NT_SUCCESS(st))
			/* We've got a match; increment the refcount and we're done */
			++pldte->LoadCount;
	}
	_asm popfd

	if(!pldte && NT_SUCCESS(st))
		/* No already-loaded module, so let's try to load it */
		st = StLoadImage(oszName, NULL, &pldte);
	RtlLeaveCriticalSection(&csLoader);

	switch(st) {
	case STATUS_OBJECT_NAME_NOT_FOUND:
	case STATUS_OBJECT_PATH_NOT_FOUND:
	case STATUS_OBJECT_PATH_INVALID:
	case STATUS_OBJECT_NAME_INVALID:
		return XBDM_NOSUCHFILE;
	default:
		if(!NT_SUCCESS(st))
			return E_FAIL;
		break;
	}
	if(phModule)
		*phModule = (HANDLE)pldte;
	if(ppvBase)
		*ppvBase = pldte->DllBase;
	return XBDM_NOERR;
}

HRESULT DmGetProcAddress(HANDLE hModule, LPCSTR szName, PVOID *ppvRet)
{
	PLDR_DATA_TABLE_ENTRY pldte = (PLDR_DATA_TABLE_ENTRY)hModule;
	PVOID pvBase = pldte->DllBase;
	PIMAGE_EXPORT_DIRECTORY ped;
	ULONG cbExports;

	if (!szName || !ppvRet)
		return E_INVALIDARG;

	/* Find the module's export directory */
	ped = (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(pvBase, TRUE,
		IMAGE_DIRECTORY_ENTRY_EXPORT, &cbExports);
	if(!ped)
		return XBDM_NOSUCHFILE;

	return FResolveImport(pvBase, ped, cbExports, szName, ppvRet) ?
		XBDM_NOERR : XBDM_NOSUCHFILE;
}
