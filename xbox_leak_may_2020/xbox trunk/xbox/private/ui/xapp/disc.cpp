#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "Node.h"
#include "ntiosvc.h"
#include "Runner.h"
#include "Sound.h"

int GetDiscType();

////////////////////////////////////////////////////////////////////////////

static const TCHAR* rgszDiscType [] =
{
    _T("none"),
    _T("unknown"),
    _T("Title"),
    _T("Audio"),
    _T("Video"),
};

int g_nDiscType;

void DiscDrive_Init()
{
    g_cdrom.Open(1);
    g_nDiscType = GetDiscType();
}

////////////////////////////////////////////////////////////////////////////

class CDiscDrive : public CNode
{
    DECLARE_NODE(CDiscDrive, CNode)
public:
    CDiscDrive();
    ~CDiscDrive();

    void Advance(float nSeconds);

    TCHAR* m_discType; // none, unknown, Audio, Video, Photo, Title

    // Audio CD (CDDA) Information...
    int getTrackCount();
    CStrObject* FormatTotalTime();
    CStrObject* FormatTrackTime(int nTrack);

    void LaunchDisc();

#ifdef _CD_INFO
    CStrObject* getArtist();
    CStrObject* getTitle();
    CStrObject* getTrackName(int nTrack);
#endif

    int m_nDiscType;
    bool m_locked;

    DECLARE_NODE_PROPS()
    DECLARE_NODE_FUNCTIONS()

    static CNodeArray c_drives;

private:
    ULONG m_trayState;
    bool m_bDeferNotification;
};

// Events:
//
//  OnDiscInserted
//  OnDiscRemoved

CNodeArray CDiscDrive::c_drives;

IMPLEMENT_NODE("DiscDrive", CDiscDrive, CNode)

START_NODE_PROPS(CDiscDrive, CNode)
    NODE_PROP(pt_string, CDiscDrive, discType)
    NODE_PROP(pt_boolean, CDiscDrive, locked)
END_NODE_PROPS()

START_NODE_FUN(CDiscDrive, CNode)
    NODE_FUN_IV(getTrackCount)
    NODE_FUN_SV(FormatTotalTime)
    NODE_FUN_SI(FormatTrackTime)
    NODE_FUN_VV(LaunchDisc)
#ifdef _CD_INFO
    NODE_FUN_SV(getArtist)
    NODE_FUN_SV(getTitle)
    NODE_FUN_SI(getTrackName)
#endif
END_NODE_FUN()

CDiscDrive::CDiscDrive() :
    m_locked(false),
    m_bDeferNotification(false)
{
    c_drives.AddNode(this);
    AddRef();

    //
    // Mark the disc type to DISC_NONE if it's not a DVD movie (bug 9014)
    // so that Advance will catch change notification and reboot to title.
    // For DVD movies, we have to mark as DISC_VIDEO because the way the
    // dash plays DVD is to reboot. If we let Advance to catch notification
    // the dash will end up reboot infinitely.
    //

    if (theApp.m_bHasLaunchData || g_nDiscType == DISC_VIDEO || g_nDiscType == DISC_BAD)
    {
        m_nDiscType = g_nDiscType;
        HalReadSMCTrayState(&m_trayState, 0);
    }
    else
    {
        m_nDiscType = DISC_NONE;
        m_trayState = SMC_TRAY_STATE_NO_MEDIA;
    }

    const TCHAR* szDiscType = rgszDiscType[m_nDiscType];

    m_discType = new TCHAR [_tcslen(szDiscType) + 1];
    _tcscpy(m_discType, szDiscType);

    TRACE(_T("CDiscDrive: %s\n"), szDiscType);
}

CDiscDrive::~CDiscDrive()
{
    c_drives.RemoveNode(this);
    if (c_drives.GetLength() == 0)
        c_drives.RemoveAll();

    delete [] m_discType;
}

void CDiscDrive::Advance(float nSeconds)
{
    // Check if we need to reboot
    if (m_bDeferNotification && !m_locked)
    {
        m_bDeferNotification = false;
        if (m_nDiscType != DISC_NONE)
            CallFunction(this, _T("OnDiscInserted"));
        else
            CallFunction(this, _T("OnDiscRemoved"));
    }

    static XTIME lastPoll = 0.0f;
    if (XAppGetNow() - lastPoll < 0.04f)
        return;
    lastPoll = XAppGetNow();

    // Poll state of the DVD tray by reading from SMC
    NTSTATUS Status;
    ULONG TrayState;

    Status = HalReadSMCTrayState(&TrayState, NULL);
    if (!NT_SUCCESS(Status)) return;

    if (TrayState == m_trayState) return;
    m_trayState = TrayState;

    // Reset screen saver if tray state changes
    ResetScreenSaver();

    g_nDiscType = GetDiscType();

    if (m_nDiscType != g_nDiscType)
    {
        TRACE(_T("\001Disc type: %s\n"), rgszDiscType[g_nDiscType]);
        m_nDiscType = g_nDiscType;

        delete [] m_discType;
        m_discType = new TCHAR [_tcslen(rgszDiscType[m_nDiscType]) + 1];
        _tcscpy(m_discType, rgszDiscType[m_nDiscType]);

        if (!m_locked)
        {
            if (m_nDiscType != DISC_NONE)
                CallFunction(this, _T("OnDiscInserted"));
            else
                CallFunction(this, _T("OnDiscRemoved"));
        }
        else
        {
            m_bDeferNotification = true;
        }
    }
}

int CDiscDrive::getTrackCount()
{
    if (g_nDiscType == DISC_AUDIO)
        return g_cdrom.GetTrackCount();

    return 0;
}

CStrObject* CDiscDrive::FormatTotalTime()
{
    int nMinutes, nSeconds;
    if (!g_cdrom.GetTotalLength(&nMinutes, &nSeconds, NULL))
        return new CStrObject; // empty string

    TCHAR szBuf [8];
    _stprintf(szBuf, _T("%02d:%02d"), nMinutes, nSeconds);
    return new CStrObject(szBuf);
}

CStrObject* CDiscDrive::FormatTrackTime(int nTrack)
{
    int nMinutes, nSeconds;
    if (!g_cdrom.GetTrackLength(nTrack, &nMinutes, &nSeconds, NULL))
        return new CStrObject; // empty string

    TCHAR szBuf [8];
    _stprintf(szBuf, _T("%02d:%02d"), nMinutes, nSeconds);
    return new CStrObject(szBuf);
}

void CDiscDrive::LaunchDisc()
{
    XAppGetD3DDev()->PersistDisplay();
    ASSERT(g_nDiscType != DISC_AUDIO);
    XLaunchNewImage(g_nDiscType == DISC_TITLE ? "d:\\default.xbe" : NULL, NULL);
}

#ifdef _CD_INFO

CStrObject* CDiscDrive::getArtist()
{
    const TCHAR* szArtist = g_cdrom.GetArtist();
    if (szArtist == NULL)
        szArtist = _T("");
    return new CStrObject(szArtist);
}

CStrObject* CDiscDrive::getTitle()
{
    const TCHAR* szTitle = g_cdrom.GetTitle();
    if (szTitle == NULL)
        szTitle = _T("");
    return new CStrObject(szTitle);
}

CStrObject* CDiscDrive::getTrackName(int nTrack)
{
    const TCHAR* szTrackName = g_cdrom.GetTrackName(nTrack);
    if (szTrackName == NULL)
        szTrackName = _T("");
    return new CStrObject(szTrackName);
}

#endif//_CD_INFO

struct DISCTYPECHECK
{
    TCHAR* szPath;
    TCHAR* szDiscType;
    int nDiscType;
};

static const DISCTYPECHECK rgddc [] =
{
    { _T("CDROM0:\\default.xbe"), _T("Title"), DISC_TITLE },
    { _T("CDROM0:\\video_ts\\video_ts.ifo"), _T("Video"), DISC_VIDEO },
    { _T("CDROM0:\\track01.cda"), _T("Audio"), DISC_AUDIO },
};

int GetDiscType()
{
    int nDiscType = DISC_BAD;
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
                    return DISC_BAD;
                }

                CloseHandle(hDevice);
            }
        }

        return DISC_NONE;
    }

    g_cdrom.Open(1);

    if (g_cdrom.IsOpen())
        return DISC_AUDIO;

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
            TRACE(_T("\001Error %d on %s\n"), dwError, rgddc[i].szPath);

            if (bRetry)
            {
                TRACE(_T("Retrying...\n"));
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
