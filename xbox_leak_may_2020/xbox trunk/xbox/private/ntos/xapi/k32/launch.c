#include "basedll.h"
#include "xlaunch.h"
#include <xboxp.h>
#include <dm.h>
#include <idexchan.h>

static const OBJECT_STRING g_DDosDevicePrefix =  CONSTANT_OBJECT_STRING(OTEXT("\\??\\D:"));
static const CHAR g_szDVDDevicePrefix[] =        "\\Device\\Cdrom0";
static const int  g_cchDVDDevicePrefix =         ARRAYSIZE(g_szDVDDevicePrefix) - 1;

extern PLAUNCH_DATA_PAGE *LaunchDataPage;

DWORD
WINAPI
XLaunchNewImage(
    LPCSTR lpTitlePath,
    PLAUNCH_DATA pLaunchData
    )
{
    //
    // We require the path is NULL or D:\something
    //

    if ((NULL != lpTitlePath) &&
        ((('D' != lpTitlePath[0]) && ('d' != lpTitlePath[0])) ||
         (':' != lpTitlePath[1]) ||
         ('\\' != lpTitlePath[2])))
    {
        XDBGWRN("XAPI", "XLaunchNewImage() ignoring invalid lpTitlePath (%s)", lpTitlePath);
        return ERROR_INVALID_PARAMETER;
    }

    if (NULL == lpTitlePath)
    {
        //
        // NULL lpTitlePath means reboot to the dashboard - don't preserve D: drive mapping
        //

        return XWriteTitleInfoAndReboot(NULL,
                                        NULL,
                                        (NULL != pLaunchData) ? LDT_LAUNCH_DASHBOARD : LDT_NONE,
                                        XeImageHeader()->Certificate->TitleID,
                                        pLaunchData);
    }
    else
    {
        NTSTATUS Status;
        OBJECT_ATTRIBUTES Obja;
        HANDLE LinkHandle;
        OCHAR szLinkTarget[MAX_PATH * 2];
        OCHAR szDDrivePath[MAX_LAUNCH_PATH];
        OBJECT_STRING LinkTarget;
        int cch;

        szDDrivePath[0] = '\0';

        //
        // If the D: drive is mapped in some unique fashion, preserve that mapping
        // across the reboot
        //

        InitializeObjectAttributes(&Obja,
                                   (POBJECT_STRING) &g_DDosDevicePrefix,
                                   OBJ_CASE_INSENSITIVE,
                                   NULL,
                                   NULL);

        Status = NtOpenSymbolicLinkObject(&LinkHandle, &Obja);

        if (NT_SUCCESS(Status))
        {
            LinkTarget.Buffer = szLinkTarget;
            LinkTarget.Length = 0;
            LinkTarget.MaximumLength = sizeof(szLinkTarget);

            Status = NtQuerySymbolicLinkObject(LinkHandle, &LinkTarget, NULL);

            NtClose(LinkHandle);
        }

        if (NT_SUCCESS(Status))
        {
            lstrcpynA(szDDrivePath,
                      szLinkTarget,
                      min(ARRAYSIZE(szDDrivePath), (LinkTarget.Length / sizeof(CHAR)) + 1));
        }
        else
        {
            /* Got no path.  The D: drive should always be mapped to something, so
             * we're in trouble if we get here.  Fall back on the DVD device
             * path */
            ASSERT(FALSE);
            strcpy(szDDrivePath, g_szDVDDevicePrefix);
        }

        return XWriteTitleInfoAndReboot(&(lpTitlePath[3]),
                                        szDDrivePath,
                                        (NULL != pLaunchData) ? LDT_TITLE : LDT_NONE,
                                        XeImageHeader()->Certificate->TitleID,
                                        pLaunchData);
    }
}

NTSTATUS
WINAPI
XWriteTitleInfoNoReboot(
    PCOSTR pszLaunchPath,
    PCOSTR pszDDrivePath,
    DWORD dwLaunchDataType,
    DWORD dwTitleId,
    PLAUNCH_DATA pLaunchData
    )
/*
 *
 * This is a private function, called by the reboot API and by the
 * debugger
 *
 */
{
    LARGE_INTEGER ByteOffset;
    PSTR pszOutput;
    int cch;

    if (NULL == *LaunchDataPage)
    {
        *LaunchDataPage = MmAllocateContiguousMemory(PAGE_SIZE);
    }

    if (NULL == *LaunchDataPage)
    {
        return STATUS_NO_MEMORY;
    }

    MmPersistContiguousMemory(*LaunchDataPage, PAGE_SIZE, TRUE);

#ifndef BUILD_FOR_XBDM
    if (NULL == pszDDrivePath)
    {
        pszDDrivePath = g_szDVDDevicePrefix;
    }
#endif

    pszOutput = ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath;

    ASSERT(PAGE_SIZE == sizeof(LAUNCH_DATA_PAGE));

    RtlZeroMemory(*LaunchDataPage, PAGE_SIZE);

    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwLaunchDataType = dwLaunchDataType;
    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwTitleId = dwTitleId;

    if (dwLaunchDataType != LDT_NONE)
    {
        memcpy(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->LaunchData,
               pLaunchData,
               sizeof(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->LaunchData));
    }

    if (NULL != pszLaunchPath)
    {
#ifdef BUILD_FOR_XBDM
        if(!pszDDrivePath)
            cch = 0;
        else
#endif
        {
        lstrcpynA(pszOutput,
                  pszDDrivePath,
                  ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - 1);
        cch = strlen(pszOutput);
        pszOutput[cch++] = TITLE_PATH_DELIMITER;
        }
        lstrcpynA(&(pszOutput[cch]),
                  pszLaunchPath,
                  ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - (cch));
    }

    return STATUS_SUCCESS;
}

DWORD
WINAPI
XWriteTitleInfoAndReboot(
    LPCSTR pszLaunchPath,
    LPCSTR pszDDrivePath,
    DWORD dwLaunchDataType,
    DWORD dwTitleId,
    PLAUNCH_DATA pLaunchData
    )
{
    NTSTATUS Status = XWriteTitleInfoNoReboot(pszLaunchPath,
                                              pszDDrivePath,
                                              dwLaunchDataType,
                                              dwTitleId,
                                              pLaunchData);

    if (NT_SUCCESS(Status))
    {
        //
        // Notify the debugger that we're about to reboot and then reboot
        //
        DmTell(DMTELL_REBOOT, NULL);

#if DBG
        if (NULL != IdexChannelObject->CurrentIrp)
        {
            XDBGERR("XAPI", "I/O still in progress when XLaunchNewImage was called!");
        }
#endif // DBG

        HalReturnToFirmware(HalQuickRebootRoutine);
    }

    return RtlNtStatusToDosError(Status);
}

DWORD
WINAPI
XGetLaunchInfo(
    OUT PDWORD pdwLaunchDataType,
    OUT PLAUNCH_DATA pLaunchData
    )
{
    RIP_ON_NOT_TRUE("XGetLaunchInfo()", (pdwLaunchDataType != NULL));
    RIP_ON_NOT_TRUE("XGetLaunchInfo()", (pLaunchData != NULL));

    if ((*LaunchDataPage) &&
        (((*LaunchDataPage)->Header.dwLaunchDataType == LDT_FROM_DASHBOARD) ||
         ((*LaunchDataPage)->Header.dwLaunchDataType == LDT_FROM_DEBUGGER_CMDLINE) ||
         ((*LaunchDataPage)->Header.dwTitleId == XeImageHeader()->Certificate->TitleID)))
    {
        PLAUNCH_DATA_PAGE pTemp;

        *pdwLaunchDataType = (*LaunchDataPage)->Header.dwLaunchDataType;
        memcpy(pLaunchData, (*LaunchDataPage)->LaunchData, sizeof(LAUNCH_DATA));

        pTemp = *LaunchDataPage;
        *LaunchDataPage = NULL;
        MmFreeContiguousMemory(pTemp);

        return ERROR_SUCCESS;
    }
    else
    {
        return ERROR_NOT_FOUND;
    }
}
