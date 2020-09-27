// xbmemmap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "meminfo.h"

CXboxMemoryInfo XboxMemory;

LPCSTR BusyTypesString = "?s#$pV*ic@d.";

LPCSTR BusyDescString[] = {
	"Unknown",
    "Stack",
    "Virtual Page Table",
    "System Page Table",
    "Pool",
    "Virtual Mapped",
    "System Memory",
    "Image",
    "File System Cache",
    "Contiguous Memory",
    "Debugger",
	"Free"					// Steal MmMaximumUsage for free pages
};

void usage()
{
    fprintf(stderr,
        "xbmemdump version " VER_PRODUCTVERSION_STR "\n"
        "Copyright (c) Microsoft Corporation.  All rights reserved.\n\n"
        "Usage:\txbmemdump [options]\n\n"
        "\t-x name\t  IP address or name of Xbox\n"
        "\t-s\t  suppress display usage summary information\n"
        "\t-l\t  display page frame number layout in long output format\n"
        "\t-w\t  display page frame number layout in wide output format\n"
		"\t-n width  specify width of page frame number list for wide view\n\t\t  default = 64\n"
        "\t-vad\t  display virtual address descriptor *\n"
        "\n"
        "\t* = all threads will be suspended while downloading information\n\n");
    exit(1);
}

void FormatPagesWideView(ULONG StartPage, ULONG NumberOfPages, LPSTR Buffer)
{
	CHAR ch;
	ULONG c, i, MaxPages, BusyType;

	MaxPages = XboxMemory.GetPfnDatabase().GetPfnDatabaseSize();

	for (i=0, c=StartPage; i<NumberOfPages; i++, c++) {

		if (c >= MaxPages) {
			break;
		}

		if (XboxMemory.GetPfnDatabase().IsPhysicallyMappedPage(c)) {
			ch = BusyTypesString[MmContiguousUsage];
		} else if (XboxMemory.GetPfnDatabase().IsFreePage(c)) {
			ch = BusyTypesString[MmMaximumUsage];
		} else {
			BusyType = XboxMemory.GetPfnDatabase().GetBusyType(c);
			if (BusyType >= MmMaximumUsage) {
				BusyType = MmMaximumUsage;
			}
			ch = BusyTypesString[BusyType];
		}

		Buffer[i] = ch;
	}

	Buffer[i] = 0;
}

void DumpPfnMapInWideView(ULONG ColumnWidth)
{
	CHAR Buffer[260];
	ULONG c, pages;

	pages = XboxMemory.GetPfnDatabase().GetPfnDatabaseSize();
	memset(Buffer, '-', ColumnWidth);
	Buffer[ColumnWidth] = 0;

	fprintf(stdout, "%-10s %*s\n", "Page Frame", ColumnWidth/2, "Page Type");
	fprintf(stdout, "---------- %s\n", Buffer);

	for (c=0; c<pages; c+=ColumnWidth) {
		FormatPagesWideView(c, ColumnWidth, Buffer);
		fprintf(stdout, "0x%08X %s\n", c, Buffer);
	}

	fprintf(stdout, "\n\n");

	for (c=0; c<=MmMaximumUsage; c++) {
		fprintf(stdout, "%c - %-25s", BusyTypesString[c], BusyDescString[c]);
		if (c & 1) {
			fprintf(stdout, "\n");
		}
	}

	fprintf(stdout, "\n\n");
}

static ULONG PageCategory(ULONG Page)
{
	if (XboxMemory.GetPfnDatabase().IsPhysicallyMappedPage(Page)) {
		return 0;
	} else if (!XboxMemory.GetPfnDatabase().IsFreePage(Page)) {
		return 1;
	} else {
		return 2;
	}
}

void DumpMemoryUsageSummary(void)
{
    int i;
    ULONG value;

    fprintf(stdout, "Usage Summary:\n\n");

    value = XboxMemory.GetTotalNumberOfPhysicalPages();
    fprintf(stdout, "%6d  Total number of pages (%d MB)\n",
        value, value * 4096 / (1024 * 1024));

    value = XboxMemory.GetAvailablePages();
    fprintf(stdout, "%6d  Available number of pages (%d KB)\n\n",
        value, value * 4096 / 1024);

    for (i=0; i<MmMaximumUsage; i++) {
        fprintf(stdout, "%6d pages used for %s (%d KB)\n",
            XboxMemory.GetAllocatedPagesByUsage(i),  BusyDescString[i],
            XboxMemory.GetAllocatedKBytesByUsage(i));
    }

    fprintf(stdout, "\n\n");
}

void DumpPfnMapInLongView(void)
{
	ULONG c, pages;
	ULONG RegionBegin;
	ULONG CurrentCategory, LastCategory;
    char* CachingType;
	const MMPTE* Pte;

	pages = XboxMemory.GetPfnDatabase().GetPfnDatabaseSize();
	RegionBegin = 0;
	LastCategory = PageCategory(0);

	fprintf(stdout, "%-17s  %7s  %-18s\n", "Page Frame Range", "Size", "Usage");
	fprintf(stdout, "-----------------  -------  ------------------\n");

    for (c=1; c<=pages; c++) {

        if (c == pages) {
            goto display;
        }

		CurrentCategory = PageCategory(c);

		if (LastCategory == CurrentCategory) {

			if (CurrentCategory == 2) {

				continue;

			} else if (CurrentCategory == 1) {

                if (XboxMemory.GetPfnDatabase().GetBusyType(RegionBegin) == \
                    XboxMemory.GetPfnDatabase().GetBusyType(c)) {
					continue;
				}

			} else {

				Pte = XboxMemory.GetPfnDatabase().GetPte(RegionBegin);

                if ((Pte->Long & 0x5FF) != \
                    (XboxMemory.GetPfnDatabase().GetPte(c)->Long & 0x5FF)) {
                    goto display;
                }

                //
                // Display this block if we found end-of-allocation mark
                //

                if (XboxMemory.GetPfnDatabase().GetPte(c)->Hard.GuardOrEndOfAllocation == 1) {
                    CurrentCategory = PageCategory(++c);
                    goto display;
                }

                if ((Pte->Long & 0x7FF) == \
                    (XboxMemory.GetPfnDatabase().GetPte(c)->Long & 0x7FF)) {
					continue;
				}
			}
		}

display:

		switch (LastCategory) {

		case 0:

			Pte = XboxMemory.GetPfnDatabase().GetPte(RegionBegin);

			if (Pte->Hard.CacheDisable != 0) {
				CachingType = "UC";
			} else if (Pte->Hard.WriteThrough == 1) {
				CachingType = "WC";
			} else {
				CachingType = "WB";
			}

			printf("0x%05X - 0x%05X  %6uK  %-18s  %s  %s  %s\n",
				RegionBegin, c-1, (c-RegionBegin) * 4, "Physical Mapped",
                CachingType, Pte->Hard.Write ? "R/W" : "R/O",
				Pte->Hard.PersistAllocation ? "Persist" : "");
			break;

		case 1:
		    fprintf(stdout, "0x%05X - 0x%05X  %6uK  %-18s\n",
				RegionBegin, c-1, (c-RegionBegin) * 4,
				BusyDescString[XboxMemory.GetPfnDatabase().GetBusyType(RegionBegin)]);
			break;

		default:
			fprintf(stdout, "0x%05X - 0x%05X  %6uK  %-18s\n", RegionBegin, c-1,
				(c-RegionBegin) * 4, "Free Page");
		}

		LastCategory = CurrentCategory;
		RegionBegin = c;
	}

    fprintf(stdout, "\n\n");
}

void DumpVadList(void)
{
    size_t vad;

    fprintf(stdout, "Virtual Address Descriptor List:\n\n");
    fprintf(stdout, "%-19s  %s\n", "Virtual Address", "Protection");
    fprintf(stdout, "-------------------  ----------\n");

    for (vad=0; vad<XboxMemory.GetVad().GetArraySize(); vad++) {

        ULONG Protection = XboxMemory.GetVad().GetAllocationProtecion(vad);
        PVOID StartingVa = MI_VPN_TO_VA(XboxMemory.GetVad().GetStartingVpn(vad));
        PVOID EndingVa = MI_VPN_TO_VA(XboxMemory.GetVad().GetEndingVpn(vad));

        fprintf(stdout, "%p - %p  0x%08X  %s%s%s%s%s%s%s%s%s%s%s %s\n",
            StartingVa, EndingVa, Protection,
            Protection & PAGE_NOACCESS ? "PAGE_NOACCESS " : "",
            Protection & PAGE_READONLY ? "PAGE_READONLY " : "",
            Protection & PAGE_READWRITE ? "PAGE_READWRITE " : "",
            Protection & PAGE_WRITECOPY ? "PAGE_WRITECOPY " : "",
            Protection & PAGE_EXECUTE ? "PAGE_EXECUTE " : "",
            Protection & PAGE_EXECUTE_READ ? "PAGE_EXECUTE_READ " : "",
            Protection & PAGE_EXECUTE_READWRITE ? "PAGE_EXECUTE_READWRITE " : "",
            Protection & PAGE_EXECUTE_WRITECOPY ? "PAGE_EXECUTE_WRITECOPY " : "",
            Protection & PAGE_GUARD ? "PAGE_GUARD " : "",
            Protection & PAGE_NOCACHE ? "PAGE_NOCACHE " : "",
            Protection & PAGE_WRITECOMBINE ? "PAGE_WRITECOMBINE " : "",
            XboxMemory.IsValidHeap(StartingVa) ? "(Heap)" : "");
    }

    fprintf(stdout, "\n\n");
}

int __cdecl main(int argc, char* argv[])
{
	int i;
	ULONG size;
	HRESULT hr = S_OK;
	PCSTR pszXboxName, arg;
	bool PfnLongView = false;
	bool PfnWideView = false;
    bool SummaryView = true;
    bool VadView = false;
	CHAR Buffer[260];
	ULONG ColumnWidth = 0;

	//
	// Process and validate command line
	//

    pszXboxName = getenv("XBOXIP");

    if (pszXboxName) {
        XboxMemory.SetXboxNameNoRegister(pszXboxName);
        pszXboxName = NULL;
    }

    for (i=1; i<argc; i++) {
        arg = argv[i];
        if (*arg == '/' || *arg == '-') {
            arg++;
            if (*arg == 'x' || *arg == 'X') {
                pszXboxName = (++i < argc ? argv[i] : NULL);
			} else if (*arg == 'n' || *arg == 'N') {
				ColumnWidth = (++i < argc ? atoi(argv[i]) : 0);
            } else if (*arg == 'l' || *arg == 'L') {
                PfnLongView = *(arg+1) == '-' ? false : true;
            } else if (*arg == 'w' || *arg == 'W') {
                PfnWideView = *(arg+1) == '-' ? false : true;
            } else if (*arg == 's' || *arg == 'S') {
                SummaryView  = false;
            } else if (_stricmp(arg, "vad") == 0) {
                VadView = true;
            } else {
				usage();
			}
        } else {
            usage();
        }
    }

	if (!SummaryView && !PfnLongView && !PfnWideView) {
		usage();
	}

	if (ColumnWidth == 0 || ColumnWidth > 256) {
		ColumnWidth = 64;
	}

	//
	// Set and register new Xbox machine name if needed
	//

    if (pszXboxName) {
        hr = XboxMemory.SetXboxName(pszXboxName);
    }

	//
	// Download PFN database and all memory manager global
	//

	if (SUCCEEDED(hr)) {
		size = sizeof(Buffer);
		hr = XboxMemory.GetXboxName(Buffer, &size);
	}

	fprintf(stderr, "Downloading information from '%s'\n\n", Buffer);

    if (SUCCEEDED(hr) && SummaryView) {
        hr = XboxMemory.DownloadMmGlobal();
    }

	if (SUCCEEDED(hr) && (PfnLongView || PfnWideView)) {
		hr = XboxMemory.DownloadPfnDatabase();
	}

    if (SUCCEEDED(hr) && VadView) {
        hr = XboxMemory.DownloadVad();
    }

	//
	// Display the memory map and other information
	//

	if (SUCCEEDED(hr)) {

        if (SummaryView) {
            DumpMemoryUsageSummary();
        }

		if (PfnWideView) {
			DumpPfnMapInWideView(ColumnWidth);
		}

		if (PfnLongView) {
			DumpPfnMapInLongView();
		}

        if (VadView) {
            DumpVadList();
        }
	}

	//
	// Display error message if there is an error
	//

	if (FAILED(hr)) {
		XboxMemory.TranslateError(hr, Buffer, sizeof(Buffer));
		fprintf(stderr, "%s\n", Buffer);
		return 1;
	}

	return 0;
}