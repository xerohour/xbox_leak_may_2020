/*
 * cmdlist.c
 */
#include "cabarc.h"


static void dos_date_to_ascii(WORD dos_date, char *ascii_date)
{
	int day, month, year;

	day		= dos_date & 31;
	month	= (dos_date >> 5) & 15;
	year	= (dos_date >> 9) + 1980;

	sprintf(
		ascii_date, 
		"%04d/%02d/%02d",
		year,
		month,
		day
	);
}


static void dos_time_to_ascii(WORD dos_time, char *ascii_time)
{
	int hour, minute, second;

	second = (dos_time & 31) << 1;
	minute = (dos_time >> 5) & 63;
	hour   = (dos_time >> 11) & 31;

	sprintf(
		ascii_time,
		"%02d:%02d:%02d",
		hour,
		minute,
		second
	);
}


static void dos_attribs_to_ascii(WORD dos_attribs, char *ascii_attribs)
{
	sprintf(
		ascii_attribs,
        "%c%c%c%c",
		dos_attribs & _A_RDONLY ? 'r' : '-',
        dos_attribs & _A_ARCH ? 'a' : '-',
		dos_attribs & _A_HIDDEN ? 'h' : '-',
		dos_attribs & _A_SYSTEM ? 's' : '-'
	);
}


static FNFDINOTIFY(notification_function)
{
	switch (fdint)
	{
		case fdintCABINET_INFO: // general information about the cabinet
			return 0;

		case fdintPARTIAL_FILE: // first file in cabinet is continuation
			return 0;

		case fdintCOPY_FILE:	// file to be copied
		{
			char ascii_date[32], ascii_time[32], ascii_attribs[32];

			dos_date_to_ascii(pfdin->date, ascii_date);
			dos_time_to_ascii(pfdin->time, ascii_time);
			dos_attribs_to_ascii(pfdin->attribs, ascii_attribs);

			printf(
				"   %-29s %8d %s %s  %s\n",
				pfdin->psz1,
				pfdin->cb,
				ascii_date,
				ascii_time,
				ascii_attribs
			);

			return 0;
		}

		case fdintCLOSE_FILE_INFO:	// close the file, set relevant info

			/* here is where we would set the file attributes provided */
			callback_file_close(pfdin->hf);

			return TRUE;

		case fdintNEXT_CABINET:	// file continued to next cabinet

			return -1;
	}

	return 0;
}


bool list_cab(char *cabname)
{
	HFDI			hfdi;
	ERF				erf;
	FDICABINETINFO	fdici;
	INT_PTR			hf;
	char			*p;
	char			cabinet_name[256];
	char			cabinet_path[256];

	hfdi = FDICreate(
		callback_alloc,
		callback_free,
		callback_file_open,
		callback_file_read,
		callback_file_write,
		callback_file_close,
		callback_file_seek,
		cpu80386,
		&erf
	);

	if (hfdi == NULL)
	{
		printf("FDICreate() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper)
		);

		return FALSE;
	}


	/*
	 * Is this file really a cabinet?
	 */
	hf = callback_file_open(
		cabname,
		_O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
		0
	);

	if (hf == -1)
	{
		(void) FDIDestroy(hfdi);

		printf("Unable to open '%s' for input\n", cabname);
		return FALSE;
	}

	if (FALSE == FDIIsCabinet(
			hfdi,
			hf,
			&fdici))
	{
		/*
		 * No, it's not a cabinet!
		 */
		callback_file_close(hf);

		printf(
			"FDIIsCabinet() failed: '%s' is not a cabinet\n",
			cabname
		);

		(void) FDIDestroy(hfdi);
		return FALSE;
	}

	callback_file_close(hf);

	printf(
		"Listing of cabinet file '%s' (size %d):\n"
		"   %d file(s), %d folder(s), set ID %d, cabinet #%d\n"
		"\n",
		cabname,
		fdici.cbCabinet,
		fdici.cFiles,
		fdici.cFolders,
		fdici.setID,
		fdici.iCabinet
	);

	printf(
		"File name                      File size     Date      Time   Attrs\n"
		"-----------------------------  ---------- ---------- -------- -----\n"
	);

	p = strrchr(cabname, '\\');

	if (p == NULL)
	{
		strcpy(cabinet_name, cabname);
		strcpy(cabinet_path, "");
	}
	else
	{
		strcpy(cabinet_name, p+1);

		strncpy(cabinet_path, cabname, (int) (p-cabname)+1);
		cabinet_path[ (int) (p-cabname)+1 ] = 0;
	}

	if (TRUE != FDICopy(
		hfdi,
		cabinet_name,
		cabinet_path,
		0,
		notification_function,
		NULL,
		NULL))
	{
		printf(
			"FDICopy() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper)
		);

		(void) FDIDestroy(hfdi);
		return FALSE;
	}

	printf("\n");

	if (FDIDestroy(hfdi) != TRUE)
	{
		printf(
			"FDIDestroy() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper)
		);

		return FALSE;
	}

	return TRUE;
}
