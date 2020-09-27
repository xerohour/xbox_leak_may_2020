/*
 *
 * dmfile.c
 *
 * Utility functions for dealing with filenames and 
 * files.
 *
 */

#include "dmp.h"

//-----------------------------------------------------------------------------------------------------
// Table for converting drive names to object manager names
//-----------------------------------------------------------------------------------------------------
const OCHAR EDRIVE_OBNAME[]=OTEXT("\\Device\\Harddisk0\\Partition1\\Devkit");
const OCHAR SDRIVE_OBNAME[]=OTEXT("\\Device\\Harddisk0\\Partition1\\TDATA");
const OCHAR VDRIVE_OBNAME[]=OTEXT("\\Device\\Harddisk0\\Partition1\\UDATA");
const OCHAR CDROM_OBNAME[]=OTEXT("\\Device\\Cdrom0");
const OCHAR CDDEVKIT_OBNAME[]=OTEXT("\\Device\\Cdrom0\\Devkit");
const OCHAR ILLEGAL_OBNAME[]=OTEXT("\\Badpath");
const OCHAR SYMLINK_OBNAME[]=OTEXT("\\??\\0:");

typedef struct _DRIVENAME_CONVERSION_ENTRY
{
    PCOSTR  ObjectName;
    ULONG   ObjectNameLength; //Without a terminating NULL
    OCHAR	DriveName;
    BOOL    Visible;
} DRIVENAME_TABLE_ENTRY, *PDRIVENAME_TABLE_ENTRY;

DRIVENAME_TABLE_ENTRY DriveNameConversionTable[] = 
{
    { EDRIVE_OBNAME, sizeof(EDRIVE_OBNAME)/sizeof(OCHAR)-1, 'E', 1 },
    { SDRIVE_OBNAME, sizeof(SDRIVE_OBNAME)/sizeof(OCHAR)-1, 'S', 1 },
    { VDRIVE_OBNAME, sizeof(VDRIVE_OBNAME)/sizeof(OCHAR)-1, 'V', 1 },
	{ CDROM_OBNAME, sizeof(CDROM_OBNAME)/sizeof(OCHAR)-1, 'A', 1, },
	// block the C and Y drives if they're mapped by the shell
    { ILLEGAL_OBNAME, sizeof(ILLEGAL_OBNAME)/sizeof(OCHAR)-1, 'C', 0 },
    { ILLEGAL_OBNAME, sizeof(ILLEGAL_OBNAME)/sizeof(OCHAR)-1, 'Y', 0 },
    // include a drive for the use of recovery
	{ CDDEVKIT_OBNAME, sizeof(CDDEVKIT_OBNAME)/sizeof(OCHAR)-1, 1,  },
    // and space for more
    { NULL, 0, 0, 0, },
    { NULL, 0, 0, 0, },
    { NULL, 0, 0, 0, },
    { NULL, 0, 0, 0, },
    { NULL, 0, 0, 0, },
};

void MapDebugDrive(OCHAR chDrive, PCOSTR szPrefix)
{
	PDRIVENAME_TABLE_ENTRY pdte;

	for(pdte = DriveNameConversionTable; pdte->ObjectName; ++pdte)
		if(pdte->DriveName == chDrive)
			break;

	/* Only fill it in if we're prior to the last entry */
	if((BYTE *)(pdte + 1) < (BYTE *)DriveNameConversionTable +
		sizeof DriveNameConversionTable)
	{
		pdte->ObjectName = szPrefix;
		if(szPrefix) {
			pdte->ObjectNameLength = ocslen(szPrefix);
			pdte->DriveName = chDrive;
            pdte->Visible = TRUE;
		} else if(pdte[1].ObjectName) {
			/* We've unmapped a drive in the middle of the table */
			PDRIVENAME_TABLE_ENTRY pdteLast;
			for(pdteLast = pdte; pdteLast[1].ObjectName; ++pdteLast);
			*pdte = *pdteLast;
			pdteLast->ObjectName = NULL;
		}
	}
}

VOID
FObNameToFileName(
    IN POBJECT_STRING objectName,
    OUT LPSTR pszFileName,
    int cchMax
    )
/*++
    Routine Description:
        Uses the conversion table to subsitute the object manager
        path with a common drive letter exposed by the debugger.
        
        This routine also converts OBJECT_STRING (OCHAR) to CHAR.
--*/
{
    PCOSTR pocSource;
    CHAR  *pcDestination = pszFileName;
    ULONG ulCount;
    PDRIVENAME_TABLE_ENTRY pDriveNameEntry = DriveNameConversionTable;

    cchMax--;  //save room for the NULL termination
    while(pDriveNameEntry->ObjectName)
    {
        if(objectName->Length >= pDriveNameEntry->ObjectNameLength * sizeof(OCHAR)) 
        {
            if(0 == _ocsnicmp(pDriveNameEntry->ObjectName, objectName->Buffer,
				pDriveNameEntry->ObjectNameLength))
            {
			   *pcDestination++ = (char)pDriveNameEntry->DriveName;
			   *pcDestination++ = ':';
			   cchMax -= 2;
               ulCount = objectName->Length-pDriveNameEntry->ObjectNameLength;
               pocSource = objectName->Buffer+pDriveNameEntry->ObjectNameLength;
               while(ulCount-- && (cchMax-- > 0)) *pcDestination++ = (CHAR)*pocSource++;
               *pcDestination = 0; //NULL terminate
               return;
            }
        }
        pDriveNameEntry++;
    }
    //
    //  If we haven't converted it, just copy it.
    //
    ulCount = objectName->Length;
    pocSource = objectName->Buffer;
    while(ulCount-- && (cchMax-- > 0)) *pcDestination++ = (CHAR)*pocSource++;
    *pcDestination = 0; //NULL terminate
    return;
}

BOOL FFileNameToObName(LPCSTR sz, OCHAR *osz, int cchMax)
{
	int ich;
	OCHAR chDrive;
	PCOSTR oszPrefix;
	PDRIVENAME_TABLE_ENTRY pdte;

    if(!sz)
        return FALSE;

	/* The file name must start with drive letter, then ':' then '\' for us
	 * to consider it valid.  If we have a filename of the form "XC:\...",
	 * we'll accept that as well */
	if(sz[0] == 'x' || sz[0] == 'X' && sz[1] != ':')
		++sz;
	if(sz[1] != ':' || sz[2] != '\\')
		return FALSE;
	chDrive = sz[0];
	if(chDrive >= 'a' && chDrive <= 'z')
		chDrive -= 'a' - 'A';
	/* Look in the table to find a map for this drive letter */
	oszPrefix = NULL;
	for(pdte = DriveNameConversionTable; pdte->ObjectName; ++pdte)
		if(chDrive == pdte->DriveName) {
			oszPrefix = pdte->ObjectName;
			chDrive = 0;
			break;
		}
	
	/* If we didn't find a match, we use DosDevices, as long as it's a valid
     * drive */
    if(!oszPrefix) {
	    if(chDrive < 'A' || chDrive > 'Z')
		    return FALSE;
		oszPrefix = SYMLINK_OBNAME;
    }

	for(ich = 0; oszPrefix[ich]; ++ich)
		osz[ich] = oszPrefix[ich];
	if(chDrive)
		osz[ich - 2] = chDrive;
	sz += 2;
	while(*sz && ich < cchMax)
		osz[ich++] = *sz++;
	osz[ich] = 0;
	return TRUE;
}


NTSTATUS
FCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN LPCSTR FileName,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions
    )
/*++
    Routine Description:
        Wraps CreateFile to use ANSI names passed over the debugger protocol.

        1) Do any name conversion to the Object Manager Name.
        2) Initialize OBJECT_ATTRIBUTES structure.
        3) Call NtCreateFile
--*/
{
    OBJECT_STRING objectName;
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    OCHAR oszObName[MAX_OBJ_PATH+1];
    NTSTATUS status;

    if(!FFileNameToObName(FileName, oszObName, sizeof(oszObName)/sizeof(OCHAR)))
    {
        status = STATUS_NO_SUCH_FILE;
    } else
    {
	    RtlInitObjectString(&objectName, oszObName);
        InitializeObjectAttributes(
            &objectAttributes,
            &objectName,
            OBJ_CASE_INSENSITIVE,
            NULL,
		    NULL
            );
        status = NtCreateFile(
                    FileHandle,
                    DesiredAccess,
                    &objectAttributes,
                    &ioStatusBlock,
                    AllocationSize,
                    FileAttributes,
                    ShareAccess,
                    CreateDisposition,
                    CreateOptions);
    }
    return status;
}
