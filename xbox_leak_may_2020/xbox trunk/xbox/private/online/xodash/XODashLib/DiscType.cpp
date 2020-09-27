
#include "std.h"
  
#include "ntiosvc.h"
#include "XOConst.h"
#include "fileutil.h"
#include "globals.h"


struct DISCTYPECHECK
{
    TCHAR* szPath;
    TCHAR* szDiscType;
    int nDiscType;
};

static const DISCTYPECHECK rgddc [] =
{
    { _T("CDROM0:\\default.xbe"), _T("Title"), eDiscTitle },
    { _T("CDROM0:\\video_ts\\video_ts.ifo"), _T("Video"), eDiscVideo },
    { _T("CDROM0:\\track01.cda"), _T("Audio"), eDiscAudio },
};



int GetDiscType()
{
    int nDiscType = eDiscBad;
    bool bRetry = true;

    if (g_cdrom.IsOpen())
        g_cdrom.Close();

    OBJECT_STRING DeviceName;
    RtlInitObjectString(&DeviceName, "\\??\\CdRom0:");
    IoDismountVolumeByName(&DeviceName);

    NTSTATUS Status;
    ULONG TrayState;
    Status = HalReadSMCTrayState(&TrayState, NULL);
    if (NT_SUCCESS(Status) && TrayState != SMC_TRAY_STATE_MEDIA_DETECT)
    {
        if (TrayState == SMC_TRAY_STATE_NO_MEDIA)
        {
            HANDLE hDevice;

            // At this point, the drive has told the SMC that media could not be
            // detected.  To decide whether this means that the tray is empty
            // versus the tray have unrecognized media, we need to send an IOCTL
            // to the device.

            hDevice = CreateFileA("cdrom0:", GENERIC_READ, FILE_SHARE_READ, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hDevice != NULL)
            {
                BOOL fReturn;
                DWORD cbReturned;

                fReturn = DeviceIoControl(hDevice, IOCTL_CDROM_CHECK_VERIFY,
                    NULL, 0, NULL, 0, &cbReturned, NULL);

                // If the device reports back that the unit is ready (which it
                // shouldn't since the SMC thinks the tray is empty) or if the
                // media is unrecognized, then the disc is bad.
                if (fReturn || (GetLastError() == ERROR_UNRECOGNIZED_MEDIA))
                {
                    CloseHandle(hDevice);
                    return eDiscBad;
                }

                CloseHandle(hDevice);
            }
        }

        return eDiscNone;
    }

    g_cdrom.Open(1);

    if (g_cdrom.IsOpen())
        return eDiscAudio;

    for (int i = 0; i < sizeof (rgddc) / sizeof (DISCTYPECHECK); )
    {
        if (DoesFileExist(rgddc[i].szPath))
        {
            nDiscType = rgddc[i].nDiscType;
            break;
        }

        DWORD dwError = GetLastError();

        if (dwError != ERROR_FILE_NOT_FOUND && dwError != ERROR_PATH_NOT_FOUND)
        {
            DbgPrint("\001Error %d on %s\n", dwError, rgddc[i].szPath);

            if (bRetry)
            {
                DbgPrint("Retrying...\n");
                bRetry = false;
                Sleep(100);
                continue;
            }
        }

        i += 1;
        bRetry = true;
    }

    return nDiscType;
}




