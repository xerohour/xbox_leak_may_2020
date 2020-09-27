#include <ntos.h>
#include <ntdddisk.h>
#include <ntddscsi.h>
#include <init.h>
#include <xtl.h>
#include <xboxp.h>
#include <xdbg.h>
#include <xcrypt.h>
#include <cryptkeys.h>

#if DBG
extern "C" ULONG XDebugOutLevel;
#endif

BOOL HDDisablePassword( BYTE* password, DWORD dwLen )
{
    NTSTATUS status;
    HANDLE h;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK iosb;
    OBJECT_STRING name;
    ULONG AtaPassThroughSize;
    ATA_PASS_THROUGH AtaPassThrough;
    IDE_SECURITY_DATA SecurityData;
    BOOL fReturn;
    DWORD cbBytes;

    RtlInitObjectString(&name, "\\Device\\Harddisk0\\Partition0");
    InitializeObjectAttributes(&obja, &name, 0, NULL, 0);

    status = NtOpenFile(&h, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, &obja, &iosb,
        FILE_SHARE_WRITE | FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(status)) {

        RtlZeroMemory(&AtaPassThrough, sizeof(ATA_PASS_THROUGH));
        RtlZeroMemory(&SecurityData, sizeof(IDE_SECURITY_DATA));

        AtaPassThrough.IdeReg.bHostSendsData = TRUE;
        AtaPassThrough.IdeReg.bCommandReg = IDE_COMMAND_SECURITY_DISABLE_PASSWORD;
        AtaPassThrough.DataBufferSize = sizeof(IDE_SECURITY_DATA);
        AtaPassThrough.DataBuffer = &SecurityData;
    	SecurityData.Maximum = TRUE;
	    SecurityData.Master = FALSE;

        RtlCopyMemory(SecurityData.Password, password, dwLen);

        fReturn = DeviceIoControl(h, IOCTL_IDE_PASS_THROUGH,
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &cbBytes, NULL);

        if (!fReturn) {
            XDBGTRC("HDDisablePassword", "API error!");
            return FALSE;
        } else if (AtaPassThrough.IdeReg.bCommandReg & 1) {
            XDBGTRC("HDDisablePassword", "IDE error!");
            return FALSE;
        }

        NtClose(h);
    }

    return TRUE;
}

BOOL HDUnlock( BYTE* password, DWORD dwLen )
{
    NTSTATUS status;
    HANDLE h;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK iosb;
    OBJECT_STRING name;
    ULONG AtaPassThroughSize;
    ATA_PASS_THROUGH AtaPassThrough;
    IDE_SECURITY_DATA SecurityData;
    BOOL fReturn;
    DWORD cbBytes;

    RtlInitObjectString(&name, "\\Device\\Harddisk0\\Partition0");
    InitializeObjectAttributes(&obja, &name, 0, NULL, 0);

    status = NtOpenFile(&h, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, &obja, &iosb,
        FILE_SHARE_WRITE | FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(status)) {

        RtlZeroMemory(&AtaPassThrough, sizeof(ATA_PASS_THROUGH));
        RtlZeroMemory(&SecurityData, sizeof(IDE_SECURITY_DATA));

        AtaPassThrough.IdeReg.bHostSendsData = TRUE;
        AtaPassThrough.IdeReg.bCommandReg = IDE_COMMAND_SECURITY_UNLOCK;
        AtaPassThrough.DataBufferSize = sizeof(IDE_SECURITY_DATA);
        AtaPassThrough.DataBuffer = &SecurityData;
    	SecurityData.Maximum = TRUE;
	    SecurityData.Master = FALSE;

        RtlCopyMemory(SecurityData.Password, password, dwLen);

        fReturn = DeviceIoControl(h, IOCTL_IDE_PASS_THROUGH,
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &cbBytes, NULL);

        if (!fReturn) {
            XDBGTRC("HDUnlock", "API error!");
            return FALSE;
        } else if (AtaPassThrough.IdeReg.bCommandReg & 1) {
            XDBGTRC("HDUnlock", "IDE error!");
            return FALSE;
        }

        NtClose(h);
    }

    return TRUE;
}

BOOL HDSetPassword( BYTE* password, DWORD dwLen )
{
    NTSTATUS status;
    HANDLE h;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK iosb;
    OBJECT_STRING name;
    ULONG AtaPassThroughSize;
    ATA_PASS_THROUGH AtaPassThrough;
    IDE_SECURITY_DATA SecurityData;
    BOOL fReturn;
    DWORD cbBytes;

    RtlInitObjectString(&name, "\\Device\\Harddisk0\\Partition0");
    InitializeObjectAttributes(&obja, &name, 0, NULL, 0);

    status = NtOpenFile(&h, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, &obja, &iosb,
        FILE_SHARE_WRITE | FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(status)) {

        RtlZeroMemory(&AtaPassThrough, sizeof(ATA_PASS_THROUGH));
        RtlZeroMemory(&SecurityData, sizeof(IDE_SECURITY_DATA));

        AtaPassThrough.IdeReg.bHostSendsData = TRUE;
        AtaPassThrough.IdeReg.bCommandReg = IDE_COMMAND_SECURITY_SET_PASSWORD;
        AtaPassThrough.DataBufferSize = sizeof(IDE_SECURITY_DATA);
        AtaPassThrough.DataBuffer = &SecurityData;
    	SecurityData.Maximum = TRUE;
	    SecurityData.Master = FALSE;

        RtlCopyMemory(SecurityData.Password, password, dwLen);

        fReturn = DeviceIoControl(h, IOCTL_IDE_PASS_THROUGH,
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &cbBytes, NULL);

        if (!fReturn) {
            XDBGTRC("HDSetPassword", "API error!");
            return FALSE;
        } else if (AtaPassThrough.IdeReg.bCommandReg & 1) {
            XDBGTRC("HDSetPassword", "IDE error!");
            return FALSE;
        }

        NtClose(h);
    }

    return TRUE;
}

void __cdecl main()
{
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];
    
#if DBG
    XDebugOutLevel = XDBG_EXIT;
#endif

    __asm int 3;

    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
    {
        ASSERT( RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ) );
        RtlZeroMemory( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH );
    }
    
    //
    // Compute Key to use to lock hard drive
    //
    XcHMAC( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey );
                         
    //HDSetPassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE );

    //HDUnlock( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE );

    //HDDisablePassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE );
}

