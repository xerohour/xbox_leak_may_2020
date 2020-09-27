/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing Autoupdate

Module Name:

    autoupd.c

--*/

#include "xonp.h"
#include "xonver.h"

#ifdef XONLINE_FEATURE_XBOX

#include <xboxp.h>
#include <xlaunch.h>
#include <shahmac.h>

//
// Define the version of the offline dash
//
#define AUTOUPD_OFFLINE_DASH_VERSION	((DWORD)0x00000001)

//
// Define some hard paths for Autoupdate
//
#define AUTOUPD_DVD_DASH_SOURCE_PATH	"d:\\$u\\%08x\\$\\"
#define AUTOUPD_DVD_DASH_SOURCE_ROOT	"d:\\$u\\%08x\\"
#define AUTOUPD_DVD_TITLE_SOURCE_PATH	"d:\\$u\\%08x\\"
#define AUTOUPD_DVD_TITLE_ROOT_PATH		"t:\\"
#define AUTOUPD_DVD_DASH_ROOT_PATH		"y:\\"
#define AUTOUPD_UPDATE_XBE_NAME			"default.xbe"
#define AUTOUPD_DASH_CONFIG_NAME		"boot.ini"

#ifdef DEVKIT
#define AUTOUPD_DASH_UPDATE_XBE_NAME	"xshell.xbe"
#else
#define AUTOUPD_DASH_UPDATE_XBE_NAME	"xboxdash.xbe"
#endif

#define AUTOUPD_DASH_SWITCHER_XBE_NAME	"switcher.xbe"

#define AUTOUPD_DASH_UPDATE_VERSION_NAME "version.xbx"

//
// Define the path to the dash partition
//
const OCHAR DashPartition[]  = OTEXT("\\Device\\Harddisk0\\Partition2\\");
const OBJECT_STRING YDrive = CONSTANT_OBJECT_STRING( OTEXT("\\??\\Y:") );


//
// Define the full reboot paths
//
const OCHAR TitleRebootPath[] = 
		OTEXT("\\Device\\Harddisk0\\Partition1\\TDATA\\%08x\\$u");
const OCHAR DashRebootPath[] = 
		OTEXT("\\Device\\Harddisk0\\Partition2\\$%c");

//
// Strings for mounting devices
//
COBJECT_STRING DDrive      = CONSTANT_OBJECT_STRING( OTEXT("\\??\\D:") );
COBJECT_STRING CdDevice    = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\CdRom0") );
COBJECT_STRING FDrive      = CONSTANT_OBJECT_STRING( OTEXT("\\??\\F:") );
COBJECT_STRING TitleData   = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition1\\TDATA") );

//
// DVD Autoupdate (Dash and Titles) states
//
typedef enum
{
	dvdupdCountFiles = 0,
	dvdupdWipeDestination,
	dvdupdCreateTarget,
	dvdupdCopyFiles,
	dvdupdWriteConfig,
	dvdupdCleanup,
	dvdupdDone,

} AUTOUPD_DVD_STATES;

//
// DVD Autoupdate (Dash and Titles) task context
//
struct XONLINETASK_AUTOUPD_DVD
{
	XONLINETASK_CONTEXT			xontask;	// Master task handle
	HRESULT						hr;			// HRESULT
	AUTOUPD_DVD_STATES			dwState;	// Current State
	DWORD						dwTitleID;	// Title being updated
	WORD						wVersionHi;	// Current major version before update
	WORD						wVersionLo;	// Current minor version before update
	DWORD						dwFlags;	// Flags

	XONLINETASK_DIROPS			dirops;		// Directory operations context
	XONLINETASK_DIRCOPY			dircopy;	// Directory crawl context

	AUTOUPD_PROGRESS_CONTEXT	progress;	// Progress struct

	// String buffers
	CHAR						szPath[MAX_PATH];
	CHAR						szTarget[MAX_PATH];
	CHAR						szDir[MAX_PATH];
	CHAR						szTemp[MAX_PATH];

};

//
// Define the set of flags
//
#define XONLINEAUTOUPD_FLAG_DASH_UPDATE		(0x80000000)
#define XONLINEAUTOUPD_FLAG_DASH_0			(0x00000001)

#define IS_UPDATING_DASH(pautoupd) \
		(((pautoupd)->dwFlags & XONLINEAUTOUPD_FLAG_DASH_UPDATE)?TRUE:FALSE)
#define GET_UPDATE_DASH_ID(pautoupd) \
		(((pautoupd)->dwFlags & XONLINEAUTOUPD_FLAG_DASH_0)?0:1)
#define GET_DIRTYPE(pautoupd) \
		(IS_UPDATING_DASH(pautoupd)? \
				((GET_UPDATE_DASH_ID(pautoupd)==1)?dirtypeDashUpdate1:dirtypeDashUpdate0): \
				dirtypeTitleUpdate)

//
// Helper function to get the unmunged title key for the given title ID
//
HRESULT XoUpdateGetRawTitleKey(
			DWORD	dwTitleId,
			PBYTE	pbTitleKey,
			DWORD	*pcbTitleKey
			)
{
	PXBEIMAGE_CERTIFICATE	Certificate;
	DWORD					i;

    Assert(*pcbTitleKey == XBOX_KEY_LENGTH);

	// Get the header cert of the cerrent title
    Certificate = XeImageHeader()->Certificate;

    // First check to see if the current title is already it
    if (dwTitleId == Certificate->TitleID)
    {
    	memcpy(pbTitleKey, Certificate->SignatureKey, XBOX_KEY_LENGTH);
    	return(S_OK);
    }
    else
    {
    	// Now see if it's one of our alternate titles
	    for (i = 0; i < XBEIMAGE_ALTERNATE_TITLE_ID_COUNT; i++) 
	    {
	    	if (Certificate->AlternateTitleIDs[i] == dwTitleId)
	    	{
	    		memcpy(pbTitleKey, 
    					Certificate->AlternateSignatureKeys[i], XBOX_KEY_LENGTH);
	    		return(S_OK);
	    	}
	    }
    }

    return(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
}

//
// This function mounts the dash partition as Y: drive. 
//
HRESULT XoUpdateMountDashPartition()
{
	HRESULT			hr = S_OK;
    NTSTATUS		Status;
	OBJECT_STRING	VolString;
	OBJECT_STRING	DriveString;

	// Initialize the volume string
	RtlInitObjectString(&VolString, DashPartition);

	// The DriveString should not end in a backslash, so init from the same
	// string, but subtract a character on the Length member.
	RtlInitObjectString(&DriveString, DashPartition);
	DriveString.Length -= sizeof(OCHAR);

	// Just validate the partition for good measure
	Status = XapiValidateDiskPartition(&VolString);
	if (NT_SUCCESS(Status))
	{
	    // Assign it to drive y:
	    Status = IoCreateSymbolicLink((POBJECT_STRING) &YDrive, &DriveString);
	}

	if (!NT_SUCCESS(Status))
		hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));

	return(hr);
}

//
// Function to mount a title's TDATA to the specified drive
//
// Call this with caution and make sure the drive to mount does 
// not conflict with pre-defined drive-letters
//
// Note: This is a simplified RIP from pathmisc.c and xapiinit.c
// because we don't want a dependency on xapilibp
//
HRESULT XoMountTitleTDataToDriveLetter(
			PCOBJECT_STRING pcDriveString,
			PCOBJECT_STRING pcPathString,
			DWORD			dwTitleId
			)
{
	HRESULT	hr = S_OK;
    NTSTATUS Status;
    OCHAR szPathTemp[MAX_PATH];
    OBJECT_ATTRIBUTES Obja;
    IO_STATUS_BLOCK IoStatusBlock;
    OBJECT_STRING PerTitlePathString;
    HANDLE DirHandle;
    OCHAR szTitleId[CCHMAX_HEX_DWORD];

    Assert(pcDriveString && pcPathString);

    DwordToStringO(dwTitleId, szTitleId);

    InitializeObjectAttributes(
        &Obja,
        (POBJECT_STRING)&TitleData,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Assert(ocslen(szTitleId) < CCHMAX_HEX_DWORD);
    Assert(pcPathString->Length < (sizeof(szPathTemp) - (CCHMAX_HEX_DWORD * sizeof(OCHAR))));
    ocscpy(szPathTemp, pcPathString->Buffer);
    //
    //  Usually, pcDriveString usually does not have a '\\' on the end, the exception
    //  is on mounting MU's.  So here we check for '\\' and only append if necessary.
    //
    if(szPathTemp[(pcPathString->Length / sizeof(OCHAR))-1] != OTEXT('\\'))
    {
        szPathTemp[pcPathString->Length / sizeof(OCHAR)] = OTEXT('\\');
    } else
    {
        pcPathString->Length--;
    }
    ocscpy(&(szPathTemp[(pcPathString->Length + sizeof(OCHAR)) / sizeof(OCHAR)]), szTitleId);

    RtlInitObjectString(&PerTitlePathString, szPathTemp);

    InitializeObjectAttributes(
        &Obja,
        &PerTitlePathString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtCreateFile(
                &DirHandle,
                FILE_LIST_DIRECTORY | SYNCHRONIZE | FILE_GENERIC_WRITE,
                &Obja,
                &IoStatusBlock,
                NULL,
                FILE_ATTRIBUTE_NORMAL,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_OPEN,
                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                );
    if (NT_SUCCESS(Status))
    {
        Status = IoCreateSymbolicLink((POBJECT_STRING) pcDriveString, &PerTitlePathString);
    }
    
    if (!NT_SUCCESS(Status))
    	goto Error;

Exit:
    return(hr);

Error:    
	hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
	goto Exit;
}

//
// Drive specific to hide details
//
HRESULT XoUpdateMountTitleTDataToPrivateDrive(
			DWORD			dwTitleId
			)
{
	return(XoMountTitleTDataToDriveLetter(&FDrive, &TitleData, dwTitleId));
}

//
// Function to unmount a drive previously mounted with
// XoMountTitleTdataToDriveLetter. Since this does not do any
// routine cleanup, don't call this function to unmpa any drives
// that is not mapped with XoMountTitleTdataToDriveLetter.
// 
HRESULT XoUpdateUnmountTitleTData(
			OCHAR chDrive
			)
{
    NTSTATUS Status;
    OCHAR szDosDevice[MAX_PATH];
    OBJECT_STRING DosDevice;

    // Removing the 0x20 bit will make lower case characters uppercase
    chDrive &= (~0x20);

    soprintf(szDosDevice, OTEXT("\\??\\%c:"), chDrive);

    RtlInitObjectString(&DosDevice, szDosDevice);

	// Need to unmount the FS here as well
    Status = IoDeleteSymbolicLink(&DosDevice);

    return(RtlNtStatusToDosError(Status));
}

//
// Drive-inspecific function to hide the details
//
HRESULT XoUpdateUnmountPrivateDrive()
{
	return(XoUpdateUnmountTitleTData('F'));
}

//
// This function formats the dash partition.
//
HRESULT XoUpdateFormatDashPartition()
{
	HRESULT			hr = S_OK;
    NTSTATUS		Status;
	BOOL			fRet;
	OBJECT_STRING	VolString;
	OBJECT_STRING	DriveString;

	// Initialize the volume string
	RtlInitObjectString(&VolString, DashPartition);

	// The DriveString should not end in a backslash, so init from the same
	// string, but subtract a character on the Length member.
	RtlInitObjectString(&DriveString, DashPartition);
	DriveString.Length -= sizeof(OCHAR);

	// Just simple as that!
	fRet = XapiFormatFATVolume(&DriveString);
	if (fRet)
	{
		// Validate the partition for good measure
		Status = XapiValidateDiskPartition(&VolString);
		if (!NT_SUCCESS(Status))
			hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
	}
	else
		hr = HRESULT_FROM_WIN32(GetLastError());

	return(hr);
}

//
// Function to read the current dash boot directory 
// from the config file
//
HRESULT XoUpdateReadDashConfig(
			PAUTOUPD_DASH_CONFIG	pConfig,
			BOOL					fFailIfNotFound
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	CHAR	szConfig[MAX_PATH];
	DWORD	cbConfig;
	DWORD	dwError;

	// Figure out where to find the config file
	cbConfig = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0,
				AUTOUPD_DASH_CONFIG_NAME,
				dirtypeDashRoot,
				&cbConfig, szConfig);
	Assert(SUCCEEDED(hr));
	
	hFile = CreateFile(szConfig,
				GENERIC_READ, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		if ((dwError == ERROR_FILE_NOT_FOUND) ||
			(dwError == ERROR_PATH_NOT_FOUND))
		{
			if (fFailIfNotFound)
			{
				hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
				goto Error;
			}
			goto Default;
		}
		
		hr = HRESULT_FROM_WIN32(dwError);
		goto Error;
	}
	if (!ReadFile(hFile, (PBYTE)pConfig, 
				sizeof(AUTOUPD_DASH_CONFIG), &cbConfig, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// If the struct doesn't look right, then fall back to default
	if (cbConfig != sizeof(AUTOUPD_DASH_CONFIG))
		goto Default;

Error:	
Exit:
	CloseHandle(hFile);
	return(hr);

Default:
	// Default active to 0
	pConfig->dwActiveDirectory = 0;
	goto Exit;
}

//
// Function to write the current dash boot directory 
// to the config file
//
HRESULT XoUpdateWriteDashConfig(
			PAUTOUPD_DASH_CONFIG	pConfig
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	CHAR	szConfig[MAX_PATH];
	DWORD	cbConfig;
	
	// Figure out where to find the config file
	cbConfig = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0,
				AUTOUPD_DASH_CONFIG_NAME,
				dirtypeDashRoot,
				&cbConfig, szConfig);
	Assert(SUCCEEDED(hr));
	
	hFile = CreateFile(szConfig,
				GENERIC_WRITE, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}
	if (!WriteFile(hFile, (PBYTE)pConfig, 
				sizeof(AUTOUPD_DASH_CONFIG), &cbConfig, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}
	SetEndOfFile(hFile);

Error:	
	CloseHandle(hFile);
	return(hr);
}

//
// Function to read the current dash update version
// from the config file
//
HRESULT XoUpdateReadDashUpdateVersion(
			PDWORD	pdwVersion
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	CHAR	szConfig[MAX_PATH];
	DWORD	cbConfig;

	// Figure out where to find the config file
	sprintf(szConfig, AUTOUPD_DVD_DASH_SOURCE_ROOT, AUTOUPD_DASH_TITLE_ID);
	strcat(szConfig, AUTOUPD_DASH_UPDATE_VERSION_NAME);

	hFile = CreateFile(szConfig,
				GENERIC_READ, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}
	if (!ReadFile(hFile, (PBYTE)pdwVersion, sizeof(DWORD), &cbConfig, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

Error:	
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	return(hr);
}

//
// Function to replace the original dash XBE with the switcher
//
HRESULT XoUpdateInstallSwitcher(
			XONLINEDIR_TYPE	dirtype
			)
{
	HRESULT	hr = S_OK;
	CHAR	szSource[MAX_PATH];
	CHAR	szTarget[MAX_PATH];
	DWORD	cbPath;

	cbPath = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0,
				AUTOUPD_DASH_SWITCHER_XBE_NAME,
				dirtype,
				&cbPath, szSource);
	if (FAILED(hr))
		return(hr);

	// Copy the switcher over, overwrite if exists
	strcpy(szTarget, AUTOUPD_DVD_DASH_ROOT_PATH);
	strcat(szTarget, AUTOUPD_DASH_UPDATE_XBE_NAME);
	if (!CopyFile(szSource, szTarget, FALSE))
		return(HRESULT_FROM_WIN32(GetLastError()));
	return(S_OK);
}

//
// Per-file task handler to count the number of files and total size
//
HRESULT CXo::UpdateDVDCountFilesContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	ULARGE_INTEGER				uliAlignedSize;
	PXONLINETASK_DIRCOPY		pdircopy;
	PXONLINETASK_AUTOUPD_DVD	pautoupd;
	PAUTOUPD_PROGRESS_CONTEXT	papc;
	WIN32_FIND_DATA				*pwfd;

	Assert(hTask != NULL);

	// Locate the containing object
	pdircopy = CONTAINING_RECORD(hTask, XONLINETASK_DIRCOPY, xontaskOnFile);
	pautoupd = CONTAINING_RECORD(pdircopy, XONLINETASK_AUTOUPD_DVD, dircopy);
	papc = &pautoupd->progress;
	pwfd = &(pdircopy->dircrawl.wfd);

	// Get the context (which is a progress context) and add the values
	(papc->dwFilesTotal)++;
	uliAlignedSize.u.LowPart = pwfd->nFileSizeLow;
	uliAlignedSize.u.HighPart = pwfd->nFileSizeHigh;
	papc->uliBytesTotal.QuadPart += uliAlignedSize.QuadPart;
	
	uliAlignedSize.QuadPart += (XBOX_CLUSTER_SIZE - 1);
	uliAlignedSize.QuadPart /= XBOX_CLUSTER_SIZE;
	uliAlignedSize.QuadPart *= XBOX_CLUSTER_SIZE;
	papc->uliActualBytesRequired.QuadPart += uliAlignedSize.QuadPart;

	return(XONLINETASK_S_SUCCESS);
}

//
// Callback function used to provide progress for update process
//
HRESULT FileCopyProgressHandler(
			PVOID			pvContext,
			WIN32_FIND_DATA	*pwfd
			)
{
	HRESULT						hr = S_OK;
	PAUTOUPD_PROGRESS_CONTEXT	papc = (PAUTOUPD_PROGRESS_CONTEXT)pvContext;

	Assert(pvContext != NULL);
	Assert(pwfd != NULL);

	// Get the context (which is a progress context) and add the values
	(papc->dwFilesDone)++;
	papc->uliBytesProcessed.u.LowPart = pwfd->nFileSizeLow;
	papc->uliBytesProcessed.u.HighPart = pwfd->nFileSizeHigh;

	return(S_OK);
}

//
// State handler function for dvdupdCountFiles
//
HRESULT CXo::dvdupdCountFilesHandler(
			PXONLINETASK_AUTOUPD_DVD	pautoupd
			)
{
	HRESULT			hr = S_OK;

	// Keep pumping until the task is done
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->dircopy));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// See if it succeeded
		if (FAILED(hr))
			goto Error;

		// Just to be safe, we will wipe the destination clean
		// Remove directory alters the directory string, so we will
		// pass in a copy instead
		strcpy(pautoupd->szTemp, pautoupd->szTarget);
		hr = RemoveDirectoryInitializeContext(
					pautoupd->szTemp,
					strlen(pautoupd->szTemp),
					MAX_PATH, pautoupd->szDir, 0,
					pautoupd->xontask.hEventWorkAvailable,
					&(pautoupd->dirops));
		if (FAILED(hr))
			goto Error;

		// OK, now we know how many files/bytes we need to copy, we can 
		// now clean up the target
		pautoupd->dwState = dvdupdWipeDestination;
	}

Exit:
	return(S_OK);

Error:
	// Something failed, we will go to cleanup
	pautoupd->dwState = dvdupdCleanup;
	pautoupd->hr = hr;
	goto Exit;
}

//
// State handler function for dvdupdWipeDestination
//
HRESULT CXo::dvdupdWipeDestinationHandler(
			PXONLINETASK_AUTOUPD_DVD	pautoupd
			)
{
	HRESULT			hr = S_OK;
	PSTR			szDriveString = AUTOUPD_DVD_TITLE_ROOT_PATH;
	ULARGE_INTEGER	uliAvailable;
	ULARGE_INTEGER	uliTotal;
	DWORD			cbTemp;

	// We'll just do a format if we're updating the dash
	if (IS_UPDATING_DASH(pautoupd))
	{
		// We will format the dash partition. This does not unmount
		// the y: drive
		hr = XoUpdateFormatDashPartition();
		if (FAILED(hr))
			goto Error;

		// Create the appropriate root directory
		strcpy(pautoupd->szTemp, pautoupd->szTarget);
		cbTemp = strlen(pautoupd->szTemp);
		pautoupd->szTemp[--cbTemp] = '\0';
		if (!CreateDirectory(pautoupd->szTemp, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		// Now we will check to make sure we have enough disk space
		szDriveString = AUTOUPD_DVD_DASH_ROOT_PATH;
		goto CheckSpace;
	}

	// We will pump until the target directory is removed
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->dirops));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// See if it succeeded
		if (FAILED(hr))
			goto Error;

		// OK, now the destination is clean, we can then make sure the 
		// target directory is created. Create directory also does not like
		// trailing backslashes.
		strcpy(pautoupd->szTemp, pautoupd->szTarget);
		cbTemp = strlen(pautoupd->szTemp);
		pautoupd->szTemp[--cbTemp] = '\0';
		hr = CreateDirectoryInitializeContext(
					pautoupd->szTemp,
					cbTemp, 0,
					pautoupd->xontask.hEventWorkAvailable,
					&(pautoupd->dirops));
		if (FAILED(hr))
			goto Error;

CheckSpace:

		// Determine if we have sufficient space for the update. 
		if (!GetDiskFreeSpaceEx(szDriveString,
					&uliAvailable, &uliTotal, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		if (uliAvailable.QuadPart < pautoupd->progress.uliActualBytesRequired.QuadPart)
		{
			// Oooops, not enough disk space
			hr = HRESULT_FROM_WIN32(ERROR_DISK_FULL);
			goto Error;
		}
		
		// Next state is to create the destination
		pautoupd->dwState = dvdupdCreateTarget;
	}

Exit:
	return(S_OK);

Error:
	// Something failed, we will go to cleanup
	pautoupd->dwState = dvdupdCleanup;
	pautoupd->hr = hr;
	goto Exit;
}

//
// State handler function for dvdupdCreateTarget
//
HRESULT CXo::dvdupdCreateTargetHandler(
			PXONLINETASK_AUTOUPD_DVD	pautoupd
			)
{
	HRESULT	hr = S_OK;

	// If it's the dash we can start copying immediately
	if (IS_UPDATING_DASH(pautoupd))
		goto StartCopy;

	// Keep pumping until the task is done
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->dirops));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// See if it succeeded
		if (FAILED(hr))
			goto Error;

StartCopy:

		// OK, everything is set up, now start the update copying 
		// process
		hr = CopyDirectoryInitializeContext(
					pautoupd->szPath, 
					pautoupd->szTarget, MAX_PATH, 0,
					pautoupd->xontask.hEventWorkAvailable,
					&(pautoupd->dircopy));
		if (FAILED(hr))
			goto Error;

		// Next state is to create the destination
		pautoupd->dwState = dvdupdCopyFiles;
	}

Exit:
	return(S_OK);

Error:
	// Something failed, we will go to cleanup
	pautoupd->dwState = dvdupdCleanup;
	pautoupd->hr = hr;
	goto Exit;
}

//
// State handler function for dvdupdCopyFiles
//
HRESULT CXo::dvdupdCopyFilesHandler(
			PXONLINETASK_AUTOUPD_DVD	pautoupd
			)
{
	HRESULT	hr = S_OK;

	// Keep pumping until the task is done
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->dircopy));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// See if it succeeded
		if (FAILED(hr))
			goto Error;

		// Next state is to write the configuration
		hr = S_OK;
		pautoupd->dwState = dvdupdWriteConfig;
	}

Exit:
	return(S_OK);

Error:
	// Something failed, we will go to cleanup
	pautoupd->dwState = dvdupdCleanup;
	pautoupd->hr = hr;
	goto Exit;
}

//
// State handler function for dvdupdWriteConfig
//
HRESULT CXo::dvdupdWriteConfigHandler(
			PXONLINETASK_AUTOUPD_DVD	pautoupd
			)
{
	HRESULT			hr = S_OK;
	DGSTFILE_HEADER	dfh;
	LARGE_INTEGER	liSize;
	HANDLE			hSig = INVALID_HANDLE_VALUE;
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	BYTE			rgbTitleKey[XBOX_KEY_LENGTH];
	BYTE			rgbSignature[XCALCSIG_SIGNATURE_SIZE];
	BYTE			rgbVerify[XBOX_HD_SECTOR_SIZE * 2];
	PBYTE			pbBuffer;
	PSTR			szTarget;
	DWORD			cbTarget;
	DWORD			dwDashId;
	DWORD			cbSize;
	DWORD			cbRead;
	
	XSHAHMAC_CONTEXT 			ShaHmac;
	
	XONLINETASK_CONTENT_VERIFY	verify;
	DGSTFILE_IDENTIFIER			dfi;

	// First load up the digest
	szTarget = pautoupd->szTemp;
	strcpy(szTarget, pautoupd->szTarget);
	cbTarget = strlen(szTarget);
    Assert(cbTarget + strlen(XONLINECONTENT_MANIFEST_FILE_NAME) < MAX_PATH);
	strcpy(szTarget + cbTarget, XONLINECONTENT_MANIFEST_FILE_NAME);
	hFile = CreateFile(szTarget,
				GENERIC_READ | GENERIC_WRITE, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
				FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	if (!GetFileSizeEx(hFile, &liSize))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}
	Assert(liSize.u.HighPart == 0);

	// Get the title key that is used to sign the digest
	cbSize = XBOX_KEY_LENGTH;
	hr = XoUpdateGetRawTitleKey(pautoupd->dwTitleID, rgbTitleKey, &cbSize);
	if (FAILED(hr))
		goto Error;
	Assert(cbSize == XBOX_KEY_LENGTH);

	// Set up our dgstfile struct
	dfi.dwTitleId = pautoupd->dwTitleID;
	dfi.OfferingId = 0;
	dfi.wTitleVersionHi = pautoupd->wVersionHi;
	dfi.wTitleVersionLo = pautoupd->wVersionLo;

	// Use our tried-and-true API to verify and re-sign the update
	hr = ContentVerifyInitializeContext(hFile, 
				liSize.u.LowPart,
				rgbVerify, sizeof(rgbVerify),
				rgbTitleKey,
				XONLINECONTENT_VERIFY_FLAG_SIGN |
				XONLINECONTENT_VERIFY_FLAG_ALLOW_SAME_VERSION,
				&dfi,
				GET_DIRTYPE(pautoupd),
				NULL,
				&verify);
	if (FAILED(hr))
		goto Error;

	// Kick it off!
	hr = ContentVerifyBegin(&verify);
	if (FAILED(hr))
		goto Error;

	// Just keep it synchronous for now
	do
	{
		hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&verify);
	
	} while (!XONLINETASK_STATUS_AVAILABLE(hr));

	if (FAILED(hr))
		goto Error;
	hr = S_OK;

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	// If this is a title update we're done!
	if (IS_UPDATING_DASH(pautoupd))
	{
		AUTOUPD_DASH_CONFIG	adc;
		
		// Finally, we clobber the original default.xbe with our switcher
		hr = XoUpdateInstallSwitcher((GET_UPDATE_DASH_ID(pautoupd)==1)?
					dirtypeDashUpdate1:dirtypeDashUpdate0);
		if (FAILED(hr))
			goto Error;

		// Then we write the configuration file to indicate the target
		// Dash directory to boot from
		adc.dwActiveDirectory = GET_UPDATE_DASH_ID(pautoupd);
		hr = XoUpdateWriteDashConfig(&adc);
		if (FAILED(hr))
			goto Error;
	}

	// Next state is to cleanup
	pautoupd->dwState = dvdupdCleanup;

Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	if (hSig != INVALID_HANDLE_VALUE)
		XCalculateSignatureEnd(hSig, (PXCALCSIG_SIGNATURE)rgbSignature);

	return(S_OK);

Error:
	// Something failed, we will go to cleanup
	pautoupd->dwState = dvdupdCleanup;
	pautoupd->hr = hr;
	goto Exit;
}

//
// State handler function for dvdupdCleanup
//
HRESULT CXo::dvdupdCleanupHandler(
			PXONLINETASK_AUTOUPD_DVD	pautoupd
			)
{
	// Just clean up the dirops and dircopy contexts
	DiropsCleanup(&pautoupd->dirops);	
	DircrawlCleanup(&pautoupd->dircopy.dircrawl);

	// We're done!
	pautoupd->dwState = dvdupdDone;

	return(S_OK);
}

//
// Array of state handler functions for DVD update
//
typedef HRESULT (CXo::*PFNAUTOUPD_DVD_HANDLER)(PXONLINETASK_AUTOUPD_DVD	pautoupd);

const PFNAUTOUPD_DVD_HANDLER CXo::s_rgpfndvdupdHandlers[] =
{
    CXo::dvdupdCountFilesHandler,
    CXo::dvdupdWipeDestinationHandler,
    CXo::dvdupdCreateTargetHandler,
    CXo::dvdupdCopyFilesHandler,
    CXo::dvdupdWriteConfigHandler,
    CXo::dvdupdCleanupHandler
};

//
// Main do work function for DVD update
//
HRESULT CXo::UpdateFromDVDContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_AUTOUPD_DVD	pautoupd = (PXONLINETASK_AUTOUPD_DVD)hTask;

	Assert(hTask != NULL);

	// Always signal the event. If we actually do any async work, we 
	// will reset the event. This makes sure we don't stall
	if (pautoupd->xontask.hEventWorkAvailable)
		SetEvent(pautoupd->xontask.hEventWorkAvailable);

	// Call the function to take care of this
	if (pautoupd->dwState < dvdupdDone)
	{
		// Call our handler to do more work
		hr = (this->*s_rgpfndvdupdHandlers[pautoupd->dwState])(pautoupd);
	}
	else if (pautoupd->dwState != dvdupdDone)
	{
		AssertSz(FALSE, "UpdateFromDVDContinue: Bad state");
		pautoupd->dwState = dvdupdDone;
		hr = E_NOTIMPL;
	}
		
	// When the next state is done, we set the flag so we don't continue.
	if (pautoupd->dwState == dvdupdDone)
	{
		// Indicate that we are done and return the final code
		hr = pautoupd->hr;
		XONLINETASK_SET_COMPLETION_STATUS(hr);
	}

	return(hr);
}

//
// Function to close the task handle
//
VOID CXo::UpdateFromDVDClose(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_AUTOUPD_DVD	pautoupd = (PXONLINETASK_AUTOUPD_DVD)hTask;
	
	dvdupdCleanupHandler(pautoupd);
	SysFree(pautoupd);
}

//
// Function to kick off an asynchronous DVD update (Dash or title)
//
HRESULT CXo::UpdateFromDVD(
			DWORD				dwTitleID,
			WORD				wVersionHi,
			WORD				wVersionLo,
			DWORD				dwFlags,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_AUTOUPD_DVD	pautoupd = NULL;
	DWORD						dwTemp;

	Assert(phTask != NULL);

	// Allocate the task context
	pautoupd = (PXONLINETASK_AUTOUPD_DVD)SysAlloc(sizeof(XONLINETASK_AUTOUPD_DVD), PTAG_XONLINETASK_AUTOUPD_DVD);
	if (!pautoupd)
		return(E_OUTOFMEMORY);

	// Initialization
	ZeroMemory(pautoupd, sizeof(XONLINETASK_AUTOUPD_DVD));
	TaskInitializeContext(&pautoupd->xontask);
	pautoupd->xontask.pfnContinue = UpdateFromDVDContinue;
	pautoupd->xontask.pfnClose = UpdateFromDVDClose;
	pautoupd->xontask.hEventWorkAvailable = hWorkEvent;
	pautoupd->hr = S_OK;
	pautoupd->dwState = dvdupdCountFiles;
	pautoupd->dwTitleID = dwTitleID;
	pautoupd->dwFlags = dwFlags;
	pautoupd->wVersionHi = wVersionHi;
	pautoupd->wVersionLo = wVersionLo;

	// Set up the subtask to count files
	TaskInitializeContext(&pautoupd->dircopy.xontaskOnFile);
	pautoupd->dircopy.xontaskOnFile.pfnContinue = UpdateDVDCountFilesContinue;
	pautoupd->dircopy.xontaskOnFile.hEventWorkAvailable = hWorkEvent;

	// Compute the source and destination paths
	if (IS_UPDATING_DASH(pautoupd))
	{
		dwTemp = sprintf(pautoupd->szPath, 
					AUTOUPD_DVD_DASH_SOURCE_PATH, AUTOUPD_DASH_TITLE_ID);
		dwTemp = sprintf(pautoupd->szTarget, 
					AUTOUPD_DASH_TARGET_PATH,
					GET_UPDATE_DASH_ID(pautoupd) + '0');
	}
	else
	{
        dwTemp = sprintf(pautoupd->szPath, AUTOUPD_DVD_TITLE_SOURCE_PATH, m_dwTitleId);
		strcpy(pautoupd->szTarget, AUTOUPD_TITLE_TARGET_PATH);
	}

	// Set up the dircrawl context to count the number of files to install
	hr = DircrawlInitializeContext(
				pautoupd->szPath, NULL, MAX_PATH, 0, NULL, 
				(XONLINETASK_HANDLE)&(pautoupd->dircopy.xontaskOnFile),
				hWorkEvent,
				&(pautoupd->dircopy.dircrawl));
	if (FAILED(hr))
		goto Error;

	// Return the task handle
	*phTask = (XONLINETASK_HANDLE)pautoupd;

Exit:
	return(hr);

Error:
	if (pautoupd)
		SysFree(pautoupd);
	goto Exit;
}

//
// Function to update dash from DVD
//
HRESULT CXo::UpdateDashFromDVD(
			WORD				wVersionHi,
			WORD				wVersionLo,
			DWORD				dwDashId,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
    Assert((dwDashId == 0) || (dwDashId == 1));
	//Assert(dwVersion > 0);
	Assert(phTask != NULL);

	// Make sure the dash partition is mounted
	XoUpdateMountDashPartition();

	// Also indicate that this is a dash update
	dwDashId |= XONLINEAUTOUPD_FLAG_DASH_UPDATE;
	return(UpdateFromDVD(AUTOUPD_DASH_TITLE_ID, 
				wVersionHi, wVersionLo, dwDashId, hWorkEvent, phTask));
}

//
// Function to update title from DVD
//
HRESULT CXo::UpdateTitleFromDVD(
			DWORD				dwTitleID,
			WORD				wVersionHi,
			WORD				wVersionLo,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
	//Assert(dwVersion > 0);
	Assert(phTask != NULL);
	
	return(UpdateFromDVD(dwTitleID, 
				wVersionHi, wVersionLo, 0, hWorkEvent, phTask));
}

//
// Function to return progress for DVD auotupdate
//
HRESULT CXo::UpdateFromDVDGetProgress(
			XONLINETASK_HANDLE	hTask,
			DWORD				*pdwPercentDone,
			ULARGE_INTEGER		*puliNumerator,
			ULARGE_INTEGER		*puliDenominator
			)
{
	PXONLINETASK_AUTOUPD_DVD	pautoupd = (PXONLINETASK_AUTOUPD_DVD)hTask;
	PAUTOUPD_PROGRESS_CONTEXT	pprogress = &(pautoupd->progress);
	
	Assert(hTask != NULL);

	// We need to grab the processed count from pdircopy
	pprogress->uliBytesProcessed.QuadPart = pautoupd->dircopy.uliBytesCopied.QuadPart;
	
	if (puliNumerator)
		puliNumerator->QuadPart = pprogress->uliBytesProcessed.QuadPart;
	if (puliDenominator)
		puliDenominator->QuadPart = pprogress->uliBytesTotal.QuadPart;
	if (pdwPercentDone)
	{
		if (pprogress->uliBytesTotal.QuadPart != 0)
		{
			*pdwPercentDone = 
				(DWORD)((pprogress->uliBytesProcessed.QuadPart * 100)/
					pprogress->uliBytesTotal.QuadPart);
			if (*pdwPercentDone > 100)
				*pdwPercentDone = 100;
		}
		else
			*pdwPercentDone = 0;
	}
	return(S_OK);
}

//
// Function to reboot into the update for the specified dirtype
//
VOID XoUpdateReboot(
			DWORD			dwTitleId,
			XONLINEDIR_TYPE	dirtype
			)
{
	DWORD	dwError;
	DWORD	dwType;
	CHAR	szPath[MAX_PATH];
	CHAR	chDir;

	LAUNCH_DATA	ld;
	PLAUNCH_DATA pld = &ld;

	dwError = XGetLaunchInfo(&dwType, &ld);
	if (dwError != ERROR_SUCCESS)
	{
		dwType = LDT_NONE;
		pld = NULL;
	}

	switch (dirtype)
	{
	case dirtypeTitleUpdate:

        Assert(dwTitleId != AUTOUPD_DASH_TITLE_ID);
		sprintf(szPath, TitleRebootPath, dwTitleId);

		XWriteTitleInfoAndReboot(
				AUTOUPD_UPDATE_XBE_NAME, szPath, dwType, dwTitleId, pld);
		break;

	case dirtypeDashUpdate0:
	case dirtypeDashUpdate1:
	
		chDir = (dirtype == dirtypeDashUpdate1)?'1':'0';
        Assert(dwTitleId == AUTOUPD_DASH_TITLE_ID);
		sprintf(szPath, DashRebootPath, chDir);

		XWriteTitleInfoAndReboot(
				AUTOUPD_DASH_UPDATE_XBE_NAME, szPath, dwType, dwTitleId, pld);
		break;

	default:
		Assert(FALSE);
		goto Exit;
	}
				

Exit:				
	return;
}

//
// Function to load the Header and Cert information from an XBE
//
HRESULT XoUpdateLoadXBEInfo(
			PSTR					szXBEPath,
			PXBEIMAGE_HEADER		pxbeh,
			PXBEIMAGE_CERTIFICATE	pxbec
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	DWORD	cbSize;

	hFile = CreateFile(szXBEPath,
				GENERIC_READ, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		goto Error;

	// Read the image header
	if (!ReadFile(hFile, (PBYTE)pxbeh, sizeof(XBEIMAGE_HEADER), &cbSize, NULL))
		goto Error;
	if (cbSize != sizeof(XBEIMAGE_HEADER))
		goto InvalidData;

	// Read the image cert
	if (pxbeh->Certificate < pxbeh->BaseAddress)
		goto InvalidData;
	cbSize = (PBYTE)(pxbeh->Certificate) - (PBYTE)(pxbeh->BaseAddress);
	if (!SetFilePointer(hFile, cbSize, NULL, FILE_BEGIN))
		goto Error;
	if (!ReadFile(hFile, (PBYTE)pxbec, sizeof(XBEIMAGE_CERTIFICATE), &cbSize, NULL))
		goto Error;
	if (cbSize != sizeof(XBEIMAGE_CERTIFICATE))
		goto InvalidData;

Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return(hr);

InvalidData:
	SetLastError(ERROR_INVALID_DATA);

Error:
	hr = HRESULT_FROM_WIN32(GetLastError());
	goto Exit;
}

//
// Function to see if an update is present, if so, validates the digest
// and XBE cert. 
//
HRESULT XoUpdateDetectAndVerify(
			DWORD			dwTitleId,
			WORD			wVersionHi,
			WORD			wVersionLo,
			XONLINEDIR_TYPE	dirtype,
			DWORD			dwFlags
			)
{
	HRESULT	hr = S_OK;
	DWORD	dwVersion;
	DWORD	cbSize;
	CHAR	szPath[MAX_PATH];

	DGSTFILE_IDENTIFIER			dfi;
	XBEIMAGE_HEADER				xbeh;
	XBEIMAGE_CERTIFICATE		xbec;

	// Build the digest path
	cbSize = MAX_PATH;
	hr = BuildTargetPath(dwTitleId, 0, 0,
				XONLINECONTENT_MANIFEST_FILE_NAME,
				dirtype, &cbSize, szPath);
	if (FAILED(hr))
		goto NoUpdate;

	// First, validate the digest
	dfi.dwTitleId = dwTitleId;
	dfi.OfferingId = 0;
	dfi.wTitleVersionHi = wVersionHi; 
	dfi.wTitleVersionLo = wVersionLo;
	dwVersion = MAKE_DWORD_VERSION(wVersionHi, wVersionLo);

	// Verify the digest
	hr = DigestVerifySynchronous(szPath, dirtype, &dfi, dwFlags, NULL);
	if (FAILED(hr))
		goto NoUpdate;

	// The digest checks out, now let's be paranoid and make sure the info
	// in the XBE cert also checks out.
	cbSize = MAX_PATH;
	hr = BuildTargetPath(dwTitleId, 0, 0,
				(dirtype==dirtypeTitleUpdate)?
						AUTOUPD_UPDATE_XBE_NAME:AUTOUPD_DASH_UPDATE_XBE_NAME,
				dirtype, &cbSize, szPath);
	if (FAILED(hr))
		goto NoUpdate;
	
	// Load the XBE header info
	hr = XoUpdateLoadXBEInfo(szPath, &xbeh, &xbec);
	if (FAILED(hr))
		goto NoUpdate;

	// Make sure the data checks out
	if (xbec.TitleID != dwTitleId)
		goto NoUpdate;
	if (xbec.Version < dwVersion)
		goto NoUpdate;

	// Everything checks out. This update is valid
	return(S_OK);
	
NoUpdate:
	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}

//
// Function to perform the Dash switcher logic
//
VOID XoUpdateDashSwitcherLogic()
{
	XONLINEDIR_TYPE		dirtype;
	AUTOUPD_DASH_CONFIG	adc;
	DWORD				dwTitleId;
	DWORD				cRetry;
	WORD				wVersionHi;
	WORD				wVersionLo;

	dwTitleId = XeImageHeader()->Certificate->TitleID;
    Assert(dwTitleId == AUTOUPD_DASH_TITLE_ID);

	// This is the dash, we need to mount the dash partition
	XoUpdateMountDashPartition();

	// Now query the config file to see which directory to boot from
	adc.dwActiveDirectory = 0;
	XoUpdateReadDashConfig(&adc, FALSE);

	wVersionHi = (WORD)(XeImageHeader()->Certificate->Version >> 16);
	wVersionLo = (WORD)(XeImageHeader()->Certificate->Version & 0xffff);

	// We will retry both partitions as a best-effort
	for (cRetry = 0; cRetry < 2; cRetry++)
	{
		// Set the dirtype
		dirtype = (adc.dwActiveDirectory==1)?
						dirtypeDashUpdate1:dirtypeDashUpdate0;

		// Reboot into that update if its valid and at least as new as the
		// switcher version
		if (SUCCEEDED(XoUpdateDetectAndVerify(
						dwTitleId, wVersionHi, wVersionLo, dirtype, 
						XONLINECONTENT_VERIFY_FLAG_ALLOW_SAME_VERSION)))
		{
			XoUpdateReboot(dwTitleId, dirtype);
		}

		// If we come here, the first dash is somehow corrupt or invalid
		// We will mark the other partition as active and retry that. If
		// the other one fails as well, then we will have to show the UFM.
		adc.dwActiveDirectory++;
		adc.dwActiveDirectory &= 1;

		XoUpdateWriteDashConfig(&adc);
	}

	// This is bad, we will need to show the UFM here ...
	// BUGBUG: Need to display UFM

	return;
}

// ==============================================================
//
// The following specia section of code should only be enabled for 
// early online titles that have the dash update bundled in their 
// title DVD. We should disable this code when we stop bundling the
// dash update inside titles.
//

#ifdef DASH_UPDATE_IN_TITLE

#define AUTOUPD_DVD_DASH_RETRY_LIMIT		5
#define AUTOUPD_DVD_DASH_COUNTER_FILE_MASK	"t:\\$u????????.?"
#define AUTOUPD_DVD_DASH_COUNTER_FILE_PATH	"%s$u%08x.%c"

//
// Function to check whether the specified number of DVD dash update
// retries (and failures) have occurred. Returns S_OK if the number
// of retries is less than the hard limit, otherwise it returns a failure
// HRESULT
//
HRESULT CXo::UpdateCheckAndBumpRetryCount(
			DWORD	dwUpdateVersion
			)
{
	HRESULT			hr = S_OK;
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	wfd;
	DWORD			dwCounterVersion;
	CHAR			szPath[MAX_PATH];
	CHAR			szPathNew[MAX_PATH];
	CHAR			chCount = '1';
	CHAR			*pch;

	hFind = FindFirstFile(AUTOUPD_DVD_DASH_COUNTER_FILE_MASK, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);

		// Build the full path to the counter file
		strcpy(szPath, AUTOUPD_DVD_TITLE_ROOT_PATH);
		strcat(szPath, wfd.cFileName);
		
		// First, make sure the update version supplied matches the
		// one in the counter file name. If not, this is a different 
		// update and we refresh the coutner file.
		pch = strchr(wfd.cFileName, '.');
		if (!pch)
			goto Refresh;
		*pch = '\0';
		strcpy(szPathNew, wfd.cFileName + 2);
		*pch = '.';
		
		if (FAILED(ConvertToTitleId(szPathNew, &dwCounterVersion)))
			goto Refresh;
		if (dwCounterVersion != dwUpdateVersion)
			goto Refresh;
		
		// See what the counter says
		chCount = wfd.cFileName[strlen(wfd.cFileName) - 1] - '0';
		if (chCount >= AUTOUPD_DVD_DASH_RETRY_LIMIT)
			return(E_FAIL);

		// OK, we still have quota left, bump it up ...
		chCount += '1';
		sprintf(szPathNew, AUTOUPD_DVD_DASH_COUNTER_FILE_PATH, 
					AUTOUPD_DVD_TITLE_ROOT_PATH, dwUpdateVersion, chCount);
		MoveFile(szPath, szPathNew);

		goto Exit;
	}

Create:

	// Create a new counter file, starting at 1
	sprintf(szPath, AUTOUPD_DVD_DASH_COUNTER_FILE_PATH, 
				AUTOUPD_DVD_TITLE_ROOT_PATH, dwUpdateVersion, chCount);
	hFind = CreateFile(szPath,
				GENERIC_WRITE, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				CREATE_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFind != INVALID_HANDLE_VALUE)
		CloseHandle(hFind);

Exit:
	return(S_OK);

Refresh:

	DeleteFile(szPath);
	goto Create;
}

//
// Function to delete the counter file
//
VOID CXo::UpdateResetRetryCount()
{
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	wfd;
	CHAR			szPath[MAX_PATH];

	hFind = FindFirstFile(AUTOUPD_DVD_DASH_COUNTER_FILE_MASK, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);

		// Build the full path to the counter file
		strcpy(szPath, AUTOUPD_DVD_TITLE_ROOT_PATH);
		strcat(szPath, wfd.cFileName);
		DeleteFile(szPath);
	}
}

const CHAR             g_szTitleUpdater[] = "d:\\update.xbe";

//
// Function to detect whether a dash update is needed on the current box.
// This function will only be called from titles that ship with the new
// dash packed in to the title disk
//
VOID CXo::UpdateDashIfNecessary()
{
	HRESULT					hr = S_OK;
	CHAR					szPath[MAX_PATH];
	DWORD					dwError;
	DWORD					cbSize;
	DWORD					dwCurrentVersion;
	DWORD					dwUpdateVersion;
	WORD					wVersionHi;
	WORD					wVersionLo;
	XONLINEDIR_TYPE			dirtype;
	XBEIMAGE_HEADER			xbeh;
	XBEIMAGE_CERTIFICATE	xbec;
	AUTOUPD_DASH_CONFIG		adc;
	XONLINETASK_HANDLE		hTask = NULL;
	LAUNCH_UPDATE_INFO		lui;
	
	// First, we need to make sure the dash is mounted properly
	XoUpdateMountDashPartition();

	// Now figure out what version of the dash we have on the DVD update
	hr = XoUpdateReadDashUpdateVersion(&dwUpdateVersion);
	if (FAILED(hr))
		goto Exit;

	MAKE_COMPOSITE_VERSION(dwUpdateVersion, &wVersionHi, &wVersionLo);

	// Now figure out the current dash version on the hard disk
	cbSize = MAX_PATH;
	hr = BuildTargetPath(AUTOUPD_DASH_TITLE_ID, 0, 0,
				AUTOUPD_DASH_UPDATE_XBE_NAME,
				dirtypeDashRoot,
				&cbSize, szPath);
	if (FAILED(hr))
		goto Exit;

	// Load the XBE header info for the current dash XBE
	hr = XoUpdateLoadXBEInfo(szPath, &xbeh, &xbec);
	if (FAILED(hr))
	{
		// This is the offline dash or the update was not complete
		// either case, just reinstall
		dwCurrentVersion = 0;
	}
	else
	{
		if (xbec.TitleID != AUTOUPD_DASH_TITLE_ID)
			goto Exit;
		dwCurrentVersion = xbec.Version;
	}

	// If the current version is zero, then this is the original 
	// dash and we will update regardless.
	if (dwCurrentVersion > AUTOUPD_OFFLINE_DASH_VERSION)
	{
		// OK, we know dwCurrent is the version of the switcher XBE,
		// now we go in and find out the version of the actual dash XBE
		// Note the if we fail to read the config, we will assume the dash
		// is broken and we will force an update.
		hr = XoUpdateReadDashConfig(&adc, FALSE);
		if (FAILED(hr))
			goto ForceUpdate;

		dirtype = (adc.dwActiveDirectory == 1)?
					dirtypeDashUpdate1:dirtypeDashUpdate0;

		// OK, now we run a check on the integrity of the current 
		// dash installation. If the dash installation is valid and the
		// installed version is GREATER THAN OR EQUAL TO the DVD update
		// version, then we will NOT proceed with the update.
		hr = XoUpdateDetectAndVerify(AUTOUPD_DASH_TITLE_ID, 
					wVersionHi, wVersionLo, dirtype, 
					XONLINECONTENT_VERIFY_FLAG_ALLOW_SAME_VERSION);
		if (SUCCEEDED(hr))
			goto Exit;

		// Ooops, the update is not current, we will update the dash ...
	}

ForceUpdate:

	// Now we know the update verion on the DVD, and we know that we will 
	// have to update the dash. We will then see if we have retried and 
	// failed too many times. If so, then we will skip further attempts 
	// to update (they will have to use another disk and try again)
	//
	// Note: if we enable this code, the title will need to detect
	// successful update and call UpdateResetRetryCount() or subsequent
	// updates will be affected
	//
	//if (FAILED(UpdateCheckAndBumpRetryCount(dwUpdateVersion)))
	//	goto Exit;

	TraceSz(Warning, "Beginning DVD Dash Update\n");

	// Set up the launch info (don't touch the dash params!
	lui.dwTitleId = AUTOUPD_DASH_TITLE_ID;
	lui.dwBaseVersion = dwCurrentVersion;
	lui.dwUpdateFlags = LAUNCH_UPDATE_FLAG_USE_DEFAULT_KEYS;

	sprintf(lui.szUpdatePath, "d:\\$u\\%08x\\update.xcp", AUTOUPD_DASH_TITLE_ID);

	// Reboot; this shouldn't come back
	// Always use the updater in the DVD because it is newer
	dwError = RebootToUpdaterWhilePreservingDDrive(g_szTitleUpdater, &lui);
	hr = HRESULT_FROM_WIN32(dwError);

Exit:
	return;
}

#endif

//
// Function to detect the presence of a newer version of the current
// title. If so, this will reboot to that version.
//
// Note: if the versions are the same, this will NOT reboot.
//

int XoUpdateRebootIfNecessary()
{
	NTSTATUS	Status;
	DWORD		dwTitleId;
	WORD		wVersionHi;
	WORD		wVersionLo;

	// The dash is special and we don't handle the reboot here
    dwTitleId = XeImageHeader()->Certificate->TitleID;
	if (dwTitleId == AUTOUPD_DASH_TITLE_ID)
		return(0);

	// Call the catch-all function to see if we need to reboot
	wVersionHi = (WORD)(XeImageHeader()->Certificate->Version >> 16);
	wVersionLo = (WORD)(XeImageHeader()->Certificate->Version & 0xffff);
	if (SUCCEEDED(XoUpdateDetectAndVerify(dwTitleId, wVersionHi, wVersionLo, dirtypeTitleUpdate, 0)))
	{
		XoUpdateReboot(dwTitleId, dirtypeTitleUpdate);
	}

	// If we come back here, this means either we do not have an update
	// or the update is somehow invalid. We'll assume that this is the
	// latest XBE.

#ifndef DEVKIT
	// The D: drive might have been mapped to t:\$u directory, which may
	// spook off titles. We map this back to the DVD/CD device.
	Status = IoDeleteSymbolicLink((POBJECT_STRING)&DDrive);
	Status = IoCreateSymbolicLink((POBJECT_STRING)&DDrive,
							(POBJECT_STRING) &CdDevice);
#endif

	return(0);
}

#pragma data_seg(".CRT$RIB")
static int (*_update)(void) = XoUpdateRebootIfNecessary;
#pragma data_seg()

#endif
