extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>

//-----------------------------------------------------------------------------
// Name: test_MapEDrive()
// Desc: If the title was booted off the default
//       partition (aka Y:), map D to Y.
//
// This is a little something for test apps.  Sometimes we want to install
// them as y:\default.xbe for ease of use on the production floor.  Ideally,
//
//-----------------------------------------------------------------------------
VOID test_MapEDrive()
{

    OBJECT_STRING strDosDevice, strPath;
    strDosDevice.Buffer = "\\??\\E:";
    strDosDevice.Length = (USHORT)strlen(strDosDevice.Buffer);
    strDosDevice.MaximumLength = strDosDevice.Length + 1;

    /*
    OBJECT_ATTRIBUTES objectAttributes;
    HANDLE handle;
    NTSTATUS status;
    InitializeObjectAttributes(&objectAttributes,
                               &strDosDevice,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);
    status = NtOpenSymbolicLinkObject(&handle, &objectAttributes);
    if (!NT_SUCCESS(status))
    */
    {
        strPath.Buffer = "\\Device\\Harddisk0\\partition1\\devkit";
        strPath.Length = (USHORT)strlen(strPath.Buffer);
        strPath.MaximumLength = strPath.Length + 1;
        IoCreateSymbolicLink(&strDosDevice, &strPath);
    } 
    /*
    else{
        NtClose(handle);
    }*/
}
