#include "std.h"
#include "xapp.h"
#include "Locale.h"

extern "C"
{
	BOOL WINAPI XapiFormatFATVolume(POBJECT_STRING pcVolume);
	NTSTATUS MU_CreateDeviceObject(ULONG Port, ULONG Slot, POBJECT_STRING DeviceName);
	VOID MU_CloseDeviceObject(ULONG Port, ULONG Slot);
	NTSYSAPI BOOLEAN NTAPI RtlTimeFieldsToTime(TIME_FIELDS* TimeFields, PLARGE_INTEGER Time);
}

bool XboxFormatMemoryUnit(UINT nDevicePort, UINT nMemoryUnit)
{
	ANSI_STRING st;
	char pszMU [MAX_MUNAME];
	st.Length = 0;
	st.MaximumLength = sizeof (pszMU) - 1;
	st.Buffer = pszMU;

	if (MU_CreateDeviceObject(nDevicePort, nMemoryUnit, &st) != NO_ERROR)
		return false;

	if (!XapiFormatFATVolume(&st))
		TRACE(_T("XApiFormatFATVolume failed: %d\n"), GetLastError());

	MU_CloseDeviceObject(nDevicePort, nMemoryUnit);

	return true;
}

OBJECT_STRING c_cDrive = CONSTANT_OBJECT_STRING("\\??\\C:");
OBJECT_STRING c_cPath  = CONSTANT_OBJECT_STRING("\\Device\\Harddisk0\\partition1");
OBJECT_STRING c_yDrive = CONSTANT_OBJECT_STRING("\\??\\Y:");
OBJECT_STRING c_yPath  = CONSTANT_OBJECT_STRING("\\Device\\Harddisk0\\partition2");
OBJECT_STRING c_tDrive = CONSTANT_OBJECT_STRING("\\??\\T:");
OBJECT_STRING c_tPath  = CONSTANT_OBJECT_STRING("\\Device\\Harddisk0\\partition1\\TDATA\\fffe0000");

void Xbox_Init()
{
    // BLOCK: Increase file cache to 1MB
    {
        XSetFileCacheSize(1024 * 1024);
    }

	// BLOCK: Setup some symbolic directory links...
	{
        NTSTATUS Status;

		Status = IoCreateSymbolicLink(&c_cDrive, &c_cPath);
        if (!NT_SUCCESS(Status))
        {
            ALERT(_T("Warning: unable to map C: (0x%x)"), Status);
        }

		VERIFY(NT_SUCCESS(IoCreateSymbolicLink(&c_yDrive, &c_yPath)));

        CreateDirectory( "c:\\tdata", NULL );
        CreateDirectory( "c:\\tdata\\fffe0000", NULL );
        CreateDirectory( "c:\\tdata\\fffe0000\\music", NULL );

		Status = IoCreateSymbolicLink(&c_tDrive, &c_tPath);
        if (!NT_SUCCESS(Status))
        {
            ALERT(_T("Warning: unable to map T: (0x%x)"), Status);
        }
	}

    // BLOCK: Initialize core peripheral port support
	{
	    XInitDevices(0, NULL);
	}

    // BLOCK: Clean up dead PCM file
    {
        if (SetFileAttributesA(XappTempPcmFileA, FILE_ATTRIBUTE_NORMAL))
            DeleteFileA(XappTempPcmFileA);
        if (SetFileAttributesA(XappTempWmaFileA, FILE_ATTRIBUTE_NORMAL))
            DeleteFileA(XappTempWmaFileA);
    }

    // BLOCK: cache game region
    {
        g_nCurRegion = XGetGameRegion();
    }
}

