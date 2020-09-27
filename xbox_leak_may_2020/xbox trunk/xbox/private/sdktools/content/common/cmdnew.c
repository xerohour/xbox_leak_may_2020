/*
 * cmdnew.c
 */
#include "cabarc.h"
#include "oldnames.h"
#include "cryptcab.h"
#include "metadata.h"

#include "pubkey.c"


#define FOLDER_THRESHOLD	LONG_MAX


/*
 * File placed function called when a file has been committed
 * to a cabinet
 */
static FNFCIFILEPLACED(file_placed)
{
	return 0;
}


/*
 * Function to obtain temporary files
 */
static FNFCIGETTEMPFILE(get_temp_file)
{
    char    *psz;

    psz = _tempnam("","xx");            // Get a name
    if ((psz != NULL) && (strlen(psz) < (unsigned)cbTempName)) {
        strcpy(pszTempName,psz);        // Copy to caller's buffer
        free(psz);                      // Free temporary name buffer
        return TRUE;                    // Success
    }
    //** Failed
    if (psz) {
        free(psz);
    }

    return FALSE;
}


/*
 * Progress function
 */
static FNFCISTATUS(progress)
{
	return 0;
}


static FNFCIGETNEXTCABINET(get_next_cabinet)
{
    if (strchr(g_cabinet_template,'%') == NULL)
    {
        printf("\n"
            "When creating cabinet sets using -d, the cabinet name should contain\n"
            "a single '*' character where the cabinet number will be inserted.\n"
            "\n"
        );

	    return -1;  /* shut it down */
    }
    else
    {
        sprintf(pccab->szCab,g_cabinet_template,pccab->iCab);

	    return TRUE;
    }
}


static FNFCIGETOPENINFO(get_open_info)
{
	BY_HANDLE_FILE_INFORMATION	finfo;
	FILETIME					filetime;
	HANDLE						handle;

	handle = CreateFile(
		pszName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE)
	{
        *err = GetLastError();
		return -1;
	}

	if (GetFileInformationByHandle(handle, &finfo) == FALSE)
	{
        *err = GetLastError();
		CloseHandle(handle);
		return -1;
	}

	FileTimeToLocalFileTime(
		&finfo.ftLastWriteTime,
		&filetime
	);

	FileTimeToDosDateTime(
		&filetime,
		pdate,
		ptime
	);

	// CABARCX: Force the timestamp to be 0, we have other plans
	*ptime = 0;

    *pattribs = (USHORT) finfo.dwFileAttributes &
        (FILE_ATTRIBUTE_READONLY |
         FILE_ATTRIBUTE_HIDDEN   |
         FILE_ATTRIBUTE_SYSTEM   |
         FILE_ATTRIBUTE_ARCHIVE);

	printf("  -- adding %s\n", pszName);

	return (INT_PTR)handle;
}

static FNFCIGETOPENINFO(get_open_info_quiet)
{
	BY_HANDLE_FILE_INFORMATION	finfo;
	FILETIME					filetime;
	HANDLE						handle;

	handle = CreateFile(
		pszName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE)
	{
        *err = GetLastError();
		return -1;
	}

	if (GetFileInformationByHandle(handle, &finfo) == FALSE)
	{
        *err = GetLastError();
		CloseHandle(handle);
		return -1;
	}

	FileTimeToLocalFileTime(
		&finfo.ftLastWriteTime,
		&filetime
	);

	FileTimeToDosDateTime(
		&filetime,
		pdate,
		ptime
	);

	// CABARCX: Force the timestamp to be 0, we have other plans
	*ptime = 0;

    *pattribs = (USHORT) finfo.dwFileAttributes &
        (FILE_ATTRIBUTE_READONLY |
         FILE_ATTRIBUTE_HIDDEN   |
         FILE_ATTRIBUTE_SYSTEM   |
         FILE_ATTRIBUTE_ARCHIVE);

	return (INT_PTR)handle;
}

static void set_cab_parameters(PCCAB cab_parms)
{
	memset(cab_parms, 0, sizeof(CCAB));

    if ((g_disk_size != LONG_MAX) && (g_set_id == 0))
    {
        srand((unsigned)time(NULL));
        g_set_id = (LONG) rand();
    }

	cab_parms->cb = g_disk_size;
	cab_parms->cbFolderThresh = FOLDER_THRESHOLD;

	cab_parms->cbReserveCFHeader = g_reserved_cfheader_space;
	cab_parms->cbReserveCFFolder = g_reserved_cffolder_space;
	cab_parms->cbReserveCFData   = 0;

	cab_parms->iCab  = 1;
	cab_parms->iDisk = 0;
	cab_parms->setID = (USHORT) g_set_id;

	strcpy(cab_parms->szDisk, "");

    sprintf(cab_parms->szCab,g_cabinet_template,cab_parms->iCab);
}


static void get_filename_to_store_in_cab(char *filename, char *stripped_name)
{
	struct cab_file_link	*search;

	if (g_preserve_path_names)
	{
		char *p;

		/*
		 * Skip directory info (do not allow drive letters to be stored)
		 */
		p = strrchr(filename, ':');

		if (p)
			filename = p+1;

		/*
		 * Skip leading slash
		 */
		if (*filename == SLASH_CHARACTER)
			filename++;

		/*
		 * Strip prefixes defined by the user
		 */
		for (search = g_strip_path_list.flh_head; search; search = search->fl_next)
		{
			if (!strnicmp(filename, search->fl_filename, strlen(search->fl_filename)))
			{
				if (strlen(filename) > strlen(search->fl_filename))
				{
					strcpy(stripped_name, &filename[ strlen(search->fl_filename) ]);
					return;
				}
			}
		}

		/*
		 * No prefies to strip
		 */
		strcpy(stripped_name, filename);
	}
	else
	{
		strip_path(filename, stripped_name);
	}
}

bool create_new_cab(char *cabname, cab_file_list_header *files_to_add)
{
	HFCI			hfci = NULL;
	ERF				erf;
	CCAB			cab_parameters;
	struct cab_file_link *item;
	char			compression_mode[80];
    int             cab_path_length;
    char           *psz;

	HRESULT		hr = S_OK;
	HCRYPTPROV	hCrypto = 0;
	HCRYPTKEY	hKeyPublic = 0;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	BYTE		rgbSymKey[XONLINE_KEY_LENGTH];
	CHAR		szRawCab[MAX_PATHNAME_LENGTH];
	CHAR		szTempMetadata[MAX_PATHNAME_LENGTH];
	BOOL		fRet = true;
	TCOMP		tcomp;

	*szTempMetadata = '\0';
    strip_path(cabname, g_cabinet_template);

    cab_path_length = strlen(cabname) - strlen(g_cabinet_template);

    for (psz = g_cabinet_template; *psz != '\0'; psz++)
    {
        if (*psz == '*')
        {
            memmove(psz + 1, psz, strlen(psz) + 1);

            *psz++ = '%';
            *psz = 'd';

            break;
        }
    }

	set_cab_parameters(&cab_parameters);

	strncpy(
		cab_parameters.szCabPath,
		cabname,
		cab_path_length
	);

	cab_parameters.szCabPath[ cab_path_length ] = 0;

	// Initialize our crypto provider and establish public key
	hr = InitializeCrypto(
				NULL, 0,
				rgbPublicKey, sizeof(rgbPublicKey),
				&hCrypto, &hKeyPublic, NULL, 0);
	if (FAILED(hr))
	{
		printf("Unable to initialize crypto provider, error %u\n", hr);
		return(false);
	}

	hfci = FCICreate(
		&erf,
		file_placed,
        callback_fci_alloc,
        callback_fci_free,
        callback_fci_file_open,
        callback_fci_file_read,
        callback_fci_file_write,
        callback_fci_file_close,
        callback_fci_file_seek,
        callback_fci_file_delete,
		get_temp_file,
        &cab_parameters,
        NULL /* pv */
	);

	switch (g_compression_type)
	{
		default:
			strcpy(compression_mode, "UNKNOWN");
			tcomp = tcompTYPE_NONE;
			break;

		case COMPRESSION_TYPE_NONE:
			strcpy(compression_mode, "NONE");
			tcomp = tcompTYPE_NONE;
			break;

		case COMPRESSION_TYPE_MSZIP:
			strcpy(compression_mode, "MSZIP");
			tcomp = tcompTYPE_MSZIP;
			break;

		case COMPRESSION_TYPE_LZX:
			sprintf(compression_mode, "LZX:%d", g_compression_memory);
			tcomp = (TCOMP)TCOMPfromLZXWindow(g_compression_memory);
			break;
	}

	printf(
		"Creating new cabinet '%s%s' with compression '%s':\n",
		cab_parameters.szCabPath, cab_parameters.szCab,
		compression_mode
	);

	if (hfci == NULL)
	{
		printf("FCICreate() failed: code %d [%s]\n",
			erf.erfOper, return_fci_error_string(erf.erfOper)
		);

		return false;
	}

	// Create a new symmetric key
	hr = GenerateSymmetricKey(hCrypto, rgbSymKey);
	if (FAILED(hr))
	{
		printf("Unable to create package parameters, error %u\n", hr);
		goto Error;
	}

	strcpy(szRawCab, cabname);
	strcat(szRawCab, ".cab");
	for (item = files_to_add->flh_head; item; item = item->fl_next)
	{
		char	stripped_name[MAX_PATHNAME_LENGTH];

        if (!strcmp(item->fl_filename, FOLDER_SEPARATOR_STRING))
		{
			if (FALSE == FCIFlushFolder(
				hfci,
				get_next_cabinet,
                progress))
			{
				printf("FCIFlushFolder() failed: code %d [%s]\n",
					erf.erfOper, return_fci_error_string(erf.erfOper)
				);
				goto Error;
			}

			continue;
		}

		/*
		 * Don't allow a cabinet to be added to itself
         * (This won't save you if cabinet name is a template)
		 */
		if (!stricmp(cabname, item->fl_filename))
			continue;

        if (item->fl_logical_filename != NULL)
        {
            // the user already supplied a logical filename
            strcpy(stripped_name, item->fl_logical_filename);
        }
        else
        {
            /*
             * Don't store the exact path name in the cabinet file!
             */
            get_filename_to_store_in_cab(item->fl_filename, stripped_name);
        }

		if (FALSE == FCIAddFile(
			hfci,
			item->fl_filename, /* file to add */
			stripped_name, /* file name in cabinet file */
			FALSE, /* file is not executable */
			get_next_cabinet,
			progress,
			get_open_info,
            tcomp))
		{
			printf("FCIAddFile() failed: code %d [%s]\n",
				erf.erfOper, return_fci_error_string(erf.erfOper)
			);
			goto Error;
		}
	}		

	printf("\n");

	/*
	 * This will automatically flush the folder first
	 */
	if (FALSE == FCIFlushCabinet(
		hfci,
		FALSE,
		get_next_cabinet,
        progress))
	{
		printf("FCIFlushCabinet() failed: code %d [%s]\n",
			erf.erfOper, return_fci_error_string(erf.erfOper)
		);
		goto Error;
	}

    if (FCIDestroy(hfci) != TRUE)
	{
		printf("FCIDestroy() failed: code %d [%s]\n",
			erf.erfOper, return_fci_error_string(erf.erfOper)
		);
		fRet = false;
		goto Exit;
	}
	hfci = NULL;

	// If we are in debug mode, save a copy of the raw cab file
	if (g_debug_mode)
	{
		DeleteFile(szRawCab);
		if (!CopyFile(cabname, szRawCab, TRUE))
		{
			printf("Cannot output raw cabinet %s, error %u\n", 
						szRawCab, GetLastError());
			goto Abort;
		}
	}

	// Re-open the cabinet
	hFile = CreateFile(
				cabname,
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Cannot post-process cabinet %s, error %u\n", 
					cabname, GetLastError());
		goto Abort;
	}

	// Ok, now we can encrypt the cabinet
	hr = EncryptCabinetBody(hFile, hCrypto, rgbSymKey, XONLINE_KEY_LENGTH);
	if (FAILED(hr))
	{
		printf("Unable to build package (1), error %u\n", hr);
		goto Abort;
	}

	hr = EncryptCabinetHeader(hFile, hCrypto, rgbSymKey, XONLINE_KEY_LENGTH);
	if (FAILED(hr))
	{
		printf("Unable to build package (2), error %u\n", hr);
		goto Abort;
	}

	if (!g_offline)
	{
		// Now encrypt the public key and embed into the signature slot
		hr = EncloseSymmetricKey(hFile, hKeyPublic, rgbSymKey, XONLINE_KEY_LENGTH);
		if (FAILED(hr))
		{
			printf("Unable to seal package, error %u\n", hr);
			goto Abort;
		}

		// We will parse the metadata file to build the full metadata file
		// Figure out a temp filename
		strcpy(szTempMetadata, g_metadata_file_path);
		strcat(szTempMetadata, ".$$$");
		hr = BuildPackedFileFromMetadataFile(
					g_metadata_file_path, szTempMetadata);
		if (FAILED(hr))
		{
			printf("%s\n", MetadataGetErrorString());
			goto Abort;
		}
		
		// Finally, we append an encrypted version of the metadata file to the
		// end of the package
		hr = AppendEncryptedFile(hFile, szTempMetadata, 
					hCrypto, rgbSymKey, XONLINE_KEY_LENGTH);
		if (FAILED(hr))
		{
			printf("Unable to process metadata file, error %u\n", hr);
			goto Abort;
		}
	}
	else
	{
	}

		printf("Completed successfully\n");

Exit:

	if (*szTempMetadata != '\0')
		DeleteFile(szTempMetadata);

	return(fRet);

Abort:
	DeleteFile(cabname);
	if (g_debug_mode)
		DeleteFile(szRawCab);
	
Error:	
	if (hfci)
	    (void) FCIDestroy(hfci);
    fRet = false;
    goto Exit;
}

