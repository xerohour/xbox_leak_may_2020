#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"

#include "xlaunch.h"
#include "cryptkeys.h"
#include "av.h"
#include "timezone.h"
#include "locale.h"

//
// Cached "AV info" in Direct3D. This cache needs to be clear before wide screen mode
// could be turn on/off dynamically.
//

extern "C" { extern DWORD D3D__AvInfo; }

class CConfig : public CNode
{
	DECLARE_NODE(CConfig, CNode)
public:
	CConfig();
	~CConfig();

	int GetLanguage();
	void SetLanguage(int nLanguage);

	int GetGamePCFlags();
	void SetGamePCFlags(int nLanguage);

	int GetMoviePCFlags();
	void SetMoviePCFlags(int nLanguage);

	CStrObject* GetLaunchReason();
    DWORD GetTitleID();

    int GetLaunchContext();
	int GetLaunchParameter1();
	int GetLaunchParameter2();
    int CanDriveBeCleanup(int Drive);
    void BackToLauncher();
    void BackToLauncher2();

	CStrObject* GetAVPackType();
	CStrObject* GetAVRegion();
	CStrObject* GetGameRegion();

//	int GetWidescreen();
//	void SetWidescreen(int bWidescreen);

	// 0=fullscreen, 1=letterbox, 2=widescreen
	int GetVideoMode();
	void SetVideoMode(int nVideoMode);

	int Get480Support();
	void Set480Support(int b480Support);

	int Get720Support();
	void Set720Support(int b720Support);

	int Get1080Support();
	void Set1080Support(int b1080Support);

	int GetPAL60Support();
	void SetPAL60Support(int bPAL60Support);

	// 0=mono, 1=stereo, 2=Dolby Surround
	int GetAudioMode();
	void SetAudioMode(int nVideoMode);

	int GetDolbyDigitalSupport();
	void SetDolbyDigitalSupport(int bDolbyDigitalSupport);

	int GetDTSSupport();
	void SetDTSSupport(int bDTSSupport);

	int GetAutoOff();
	void SetAutoOff(int bAutoOff);

	void SetParentPassword(const TCHAR* szNewPassword);
	int CheckParentPassword(const TCHAR* szCheckPassword);

	int GetTimeZone();
	void SetTimeZone(int nTimeZone);

	int GetDSTAllowed();
	int GetDST();
	void SetDST(int bObserveDST);

    int ForceSetLanguage() { return CheckForcedSettings(XLD_SETTINGS_LANGUAGE); }
    int ForceSetTimeZone() { return CheckForcedSettings(XLD_SETTINGS_TIMEZONE); }
    int ForceSetClock() { return CheckForcedSettings(XLD_SETTINGS_CLOCK); }

    CStrObject* GetRecoveryKey();
	CStrObject* GetROMVersion();
    CStrObject* GetXdashVersion();

protected:
	DECLARE_NODE_FUNCTIONS()

private:
    int CheckForcedSettings(int flag) {
        return ((theApp.m_dwLaunchReason == XLD_LAUNCH_DASHBOARD_SETTINGS) &&
                (theApp.m_dwLaunchParameter1 & flag)) ? 1 : 0;
    }
};

IMPLEMENT_NODE("Config", CConfig, CNode)

START_NODE_FUN(CConfig, CNode)
	NODE_FUN_IV(GetLanguage)
	NODE_FUN_VI(SetLanguage)
	NODE_FUN_IV(GetGamePCFlags)
	NODE_FUN_VI(SetGamePCFlags)
	NODE_FUN_IV(GetMoviePCFlags)
	NODE_FUN_VI(SetMoviePCFlags)
	NODE_FUN_SV(GetLaunchReason)
    NODE_FUN_IV(GetLaunchContext)
	NODE_FUN_IV(GetLaunchParameter1)
	NODE_FUN_IV(GetLaunchParameter2)
    NODE_FUN_II(CanDriveBeCleanup)
    NODE_FUN_VV(BackToLauncher)
    NODE_FUN_VV(BackToLauncher2)
//	NODE_FUN_IV(GetWidescreen)
//	NODE_FUN_VI(SetWidescreen)
	NODE_FUN_IV(Get480Support)
	NODE_FUN_VI(Set480Support)
	NODE_FUN_IV(Get720Support)
	NODE_FUN_VI(Set720Support)
	NODE_FUN_IV(Get1080Support)
	NODE_FUN_VI(Set1080Support)
	NODE_FUN_IV(GetPAL60Support)
	NODE_FUN_VI(SetPAL60Support)
	NODE_FUN_SV(GetAVPackType)
	NODE_FUN_SV(GetAVRegion)
	NODE_FUN_SV(GetGameRegion)
	NODE_FUN_IV(GetAutoOff)
	NODE_FUN_VI(SetAutoOff)
	NODE_FUN_IV(GetVideoMode)
	NODE_FUN_VI(SetVideoMode)
	NODE_FUN_IV(GetAudioMode)
	NODE_FUN_VI(SetAudioMode)
	NODE_FUN_IV(GetDolbyDigitalSupport)
	NODE_FUN_VI(SetDolbyDigitalSupport)
	NODE_FUN_IV(GetDTSSupport)
	NODE_FUN_VI(SetDTSSupport)
	NODE_FUN_IS(CheckParentPassword)
	NODE_FUN_VS(SetParentPassword)
	NODE_FUN_IV(GetTimeZone)
	NODE_FUN_VI(SetTimeZone)
	NODE_FUN_IV(GetDSTAllowed)
	NODE_FUN_IV(GetDST)
	NODE_FUN_VI(SetDST)
	NODE_FUN_IV(ForceSetLanguage)
	NODE_FUN_IV(ForceSetTimeZone)
	NODE_FUN_IV(ForceSetClock)
    NODE_FUN_SV(GetRecoveryKey)
    NODE_FUN_SV(GetROMVersion)
    NODE_FUN_SV(GetXdashVersion)
END_NODE_FUN()

CConfig::CConfig()
{
}

CConfig::~CConfig()
{
}

int CConfig::GetLanguage()
{
	DWORD nLanguage, dwType;
	VERIFY(!XQueryValue(XC_LANGUAGE, &dwType, &nLanguage, 4, NULL));

    if (nLanguage == 0 || nLanguage > 6)
    {
        TRACE(_T("\001Invalid language, default to English\n"));
        nLanguage = 1;
    }

	return (int)nLanguage;
}

void CConfig::SetLanguage(int nLanguage)
{
	VERIFY(!XSetValue(XC_LANGUAGE, REG_DWORD, (DWORD*)&nLanguage, 4));
}

CStrObject* CConfig::GetAVPackType()
{
	DWORD dwAVPack = XGetAVPack();

	const TCHAR* szType = NULL;
	switch (dwAVPack)
	{
	case XC_AV_PACK_STANDARD:
		szType = _T("STANDARD");
		break;

	case XC_AV_PACK_SVIDEO:
		szType = _T("SVIDEO");
		break;

	case XC_AV_PACK_RFU:
		szType = _T("RFU");
		break;

	case XC_AV_PACK_SCART:
		szType = _T("SCART");
		break;

	case XC_AV_PACK_HDTV:
		szType = _T("HDTV");
		break;

	case XC_AV_PACK_VGA:
		szType = _T("VGA");
		break;
	}

	return new CStrObject(szType);
}

CStrObject* CConfig::GetAVRegion()
{
	DWORD dwVideoStd = XGetVideoStandard();

	const TCHAR* szRegion = NULL;
	switch (dwVideoStd)
	{
	case XC_VIDEO_STANDARD_NTSC_M:
		szRegion = _T("NTSC_M");
		break;

	case XC_VIDEO_STANDARD_NTSC_J:
		szRegion = _T("NTSC_J");
		break;

	case XC_VIDEO_STANDARD_PAL_I:
		szRegion = _T("PAL_I");
		break;

	case XC_VIDEO_STANDARD_PAL_M:
		szRegion = _T("PAL_M");
		break;
	}

	return new CStrObject(szRegion);
}

CStrObject* CConfig::GetGameRegion()
{
	const TCHAR* szRegion = NULL;

	switch (g_nCurRegion)
	{
	case XC_GAME_REGION_NA:
		szRegion = _T("NA");
		break;

	case XC_GAME_REGION_JAPAN:
		szRegion = _T("JAPAN");
		break;

	case XC_GAME_REGION_RESTOFWORLD:
		szRegion = _T("RESTOFWORLD");
		break;
	}

	return new CStrObject(szRegion);
}

/*
int CConfig::GetWidescreen()
{
#ifdef _XBOX
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));
	return (dwFlags & AV_FLAGS_WIDESCREEN) != 0;
#else
	return GetPrivateProfileInt(_T("Config"), _T("WIDESCREEN"), 0, _T("xdash.ini"));
#endif
}

void CConfig::SetWidescreen(int bWidescreen)
{
#ifdef _XBOX
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if (bWidescreen)
		dwFlags |= AV_FLAGS_WIDESCREEN;
	else
		dwFlags &= ~AV_FLAGS_WIDESCREEN;

	VERIFY(!XSetValue(XC_VIDEO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));
#else
	WritePrivateProfileString(_T("Config"), _T("WIDESCREEN"), bWidescreen ? _T("1") : _T("0"), _T("xdash.ini"));
#endif

	theApp.m_bStretchWidescreen = bWidescreen ? true : false;
	theApp.m_bProjectionDirty = true;
}
*/

int CConfig::GetVideoMode()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if ((dwFlags & AV_FLAGS_LETTERBOX) != 0)
		return 1;

	if ((dwFlags & AV_FLAGS_WIDESCREEN) != 0)
		return 2;

	return 0;
}

void CConfig::SetVideoMode(int nVideoMode)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));

	dwFlags &= ~(AV_FLAGS_WIDESCREEN | AV_FLAGS_LETTERBOX);
	if (nVideoMode == 1)
		dwFlags |= AV_FLAGS_LETTERBOX;
	else if (nVideoMode == 2)
		dwFlags |= AV_FLAGS_WIDESCREEN;

	VERIFY(!XSetValue(XC_VIDEO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));

    bool bWideScreen = nVideoMode == 2;

    if (theApp.m_bStretchWidescreen != bWideScreen)
    {
        if (bWideScreen)
        {
            theApp.m_pp.Flags |= D3DPRESENTFLAG_WIDESCREEN;
        }
        else
        {
            theApp.m_pp.Flags &= ~D3DPRESENTFLAG_WIDESCREEN;
        }

        theApp.m_bStretchWidescreen = bWideScreen;

        D3D__AvInfo = 0; // flush cached "AV info" in D3D
        XAppGetD3DDev()->Reset(&theApp.m_pp);
    }

	theApp.m_bProjectionDirty = true;
}

int CConfig::Get480Support()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));
	return (dwFlags & AV_FLAGS_HDTV_480p) != 0;
}

void CConfig::Set480Support(int b480Support)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if (b480Support)
		dwFlags |= AV_FLAGS_HDTV_480p;
	else
		dwFlags &= ~AV_FLAGS_HDTV_480p;

	VERIFY(!XSetValue(XC_VIDEO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));

#ifdef COOL_XDASH
    if (g_nDiscType == DISC_VIDEO && XGetAVPack() == XC_AV_PACK_HDTV)
#else
    if (g_nDiscType == DISC_VIDEO && XGetAVPack() == XC_AV_PACK_HDTV && (XBOX_480P_MACROVISION_ENABLED & XboxHardwareInfo->Flags))
#endif
    {
        bool bUpdate = false;

        if ((theApp.m_pp.Flags & D3DPRESENTFLAG_PROGRESSIVE) && !b480Support)
        {
            theApp.m_pp.Flags &= ~D3DPRESENTFLAG_PROGRESSIVE;
            theApp.m_pp.Flags |= D3DPRESENTFLAG_INTERLACED;
            bUpdate = true;
        }
        else if ((theApp.m_pp.Flags & D3DPRESENTFLAG_PROGRESSIVE) == 0 && b480Support)
        {
            theApp.m_pp.Flags |= D3DPRESENTFLAG_PROGRESSIVE;
            theApp.m_pp.Flags &= ~D3DPRESENTFLAG_INTERLACED;
            bUpdate = true;
        }

        if (bUpdate)
        {
            D3D__AvInfo = 0; // flush cached "AV info" in D3D
            XAppGetD3DDev()->Reset(&theApp.m_pp);
        }
    }
}

int CConfig::Get720Support()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));
	return (dwFlags & AV_FLAGS_HDTV_720p) != 0;
}

void CConfig::Set720Support(int b720Support)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if (b720Support)
		dwFlags |= AV_FLAGS_HDTV_720p;
	else
		dwFlags &= ~AV_FLAGS_HDTV_720p;

	VERIFY(!XSetValue(XC_VIDEO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));
}

int CConfig::Get1080Support()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));
	return (dwFlags & AV_FLAGS_HDTV_1080i) != 0;
}

void CConfig::Set1080Support(int b1080Support)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if (b1080Support)
		dwFlags |= AV_FLAGS_HDTV_1080i;
	else
		dwFlags &= ~AV_FLAGS_HDTV_1080i;

	VERIFY(!XSetValue(XC_VIDEO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));
}

int CConfig::GetPAL60Support()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));
	return (dwFlags & AV_FLAGS_60Hz) != 0;
}

void CConfig::SetPAL60Support(int bPAL60Support)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if (bPAL60Support)
		dwFlags |= AV_FLAGS_60Hz;
	else
		dwFlags &= ~AV_FLAGS_60Hz;

	VERIFY(!XSetValue(XC_VIDEO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));
}

int CConfig::GetAudioMode()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_AUDIO_FLAGS, &dwType, &dwFlags, 4, NULL));

    switch(DSSPEAKER_BASIC(dwFlags))
    {
        case DSSPEAKER_MONO:
            return 0;

        case DSSPEAKER_STEREO:
            return 1;

        default:
            return 2;
    }
}

void CConfig::SetAudioMode(int nAudioMode)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_AUDIO_FLAGS, &dwType, &dwFlags, 4, NULL));

    switch(nAudioMode)
    {
        case 0:
            dwFlags = DSSPEAKER_COMBINED(DSSPEAKER_MONO, dwFlags);
            break;

        case 1:
            dwFlags = DSSPEAKER_COMBINED(DSSPEAKER_STEREO, dwFlags);
            break;

        case 2:
            dwFlags = DSSPEAKER_COMBINED(DSSPEAKER_SURROUND, dwFlags);
            break;
    }

	VERIFY(!XSetValue(XC_AUDIO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));
}

int CConfig::GetDolbyDigitalSupport()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_AUDIO_FLAGS, &dwType, &dwFlags, 4, NULL));

	return (dwFlags & DSSPEAKER_ENABLE_AC3) != 0;
}

void CConfig::SetDolbyDigitalSupport(int bDolbyDigitalSupport)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_AUDIO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if (bDolbyDigitalSupport)
		dwFlags |= DSSPEAKER_ENABLE_AC3;
	else
		dwFlags &= ~DSSPEAKER_ENABLE_AC3;

	VERIFY(!XSetValue(XC_AUDIO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));
}

int CConfig::GetDTSSupport()
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_AUDIO_FLAGS, &dwType, &dwFlags, 4, NULL));

	return (dwFlags & DSSPEAKER_ENABLE_DTS) != 0;
}

void CConfig::SetDTSSupport(int bDTSSupport)
{
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_AUDIO_FLAGS, &dwType, &dwFlags, 4, NULL));

	if (bDTSSupport)
		dwFlags |= DSSPEAKER_ENABLE_DTS;
	else
		dwFlags &= ~DSSPEAKER_ENABLE_DTS;

	VERIFY(!XSetValue(XC_AUDIO_FLAGS, REG_DWORD, (DWORD*)&dwFlags, 4));
}

int CConfig::GetAutoOff()
{
	BOOL bAutoOff;
	XAutoPowerDownGet(&bAutoOff);
	return bAutoOff;
}

void CConfig::SetAutoOff(int bAutoOff)
{
	XAutoPowerDownSet(bAutoOff);
}

extern int GetTimeZoneIndex(const TIME_ZONE_INFORMATION* tzinfo);
extern bool GetTimeZoneInfo(int index, TIME_ZONE_INFORMATION* tzinfo);

int CConfig::GetTimeZone()
{
    TIME_ZONE_INFORMATION tzinfo;
    if ((XapipQueryTimeZoneInformation(&tzinfo, NULL) != ERROR_SUCCESS) ||
        (L'\0' == tzinfo.StandardName[0]))
    {
        //
        // No valid timezone info - return a reasonable default based on the
        // game region
        //

        switch (XGetGameRegion())
        {
            case XC_GAME_REGION_NA:
                //
                // Eastern
                //
                return NA_DEFAULT_TIMEZONE;

            case XC_GAME_REGION_JAPAN:
                //
                // Tokyo
                //
                return JAPAN_DEFAULT_TIMEZONE;

            default:
                //
                // London
                //
                return ROW_DEFAULT_TIMEZONE;
        }
    }

    int index = GetTimeZoneIndex(&tzinfo);
    return (index < 0) ? 0 : index;
}

void CConfig::SetTimeZone(int nTimeZone)
{
    TIME_ZONE_INFORMATION tzinfo;

    if (GetTimeZoneInfo(nTimeZone, &tzinfo)) {
        XapipSetTimeZoneInformation(&tzinfo);
    }
}

int CConfig::GetDSTAllowed()
{
    TIME_ZONE_INFORMATION tzinfo;

    if (XapipQueryTimeZoneInformation(&tzinfo, NULL) != ERROR_SUCCESS)
        return 0;

    //
    // If the wMonth fields are non-zero, that means that this timezone
    // has cutover date info which implies daylight savings time support
    //

    return (tzinfo.StandardDate.wMonth && tzinfo.DaylightDate.wMonth);
}

int CConfig::GetDST()
{
    TIME_ZONE_INFORMATION tzinfo;
    BOOL fUseDST;

    if (XapipQueryTimeZoneInformation(&tzinfo, &fUseDST) != ERROR_SUCCESS)
        return 0;

    //
    // If the wMonth fields are non-zero, that means that this timezone
    // has cutover date info which implies daylight savings time support
    //

    return (tzinfo.StandardDate.wMonth && tzinfo.DaylightDate.wMonth && fUseDST);
}

void CConfig::SetDST(int bObserveDST)
{
    ULONG type, size;
    DWORD flags;

#if DBG
    if (bObserveDST)
    {
        ASSERT(GetDSTAllowed());
    }
#endif // DBG

    if (XQueryValue(XC_MISC_FLAGS, &type, &flags, sizeof(flags), &size) == ERROR_SUCCESS) {
        if (bObserveDST)
            flags &= ~XC_MISC_FLAG_DONT_USE_DST;
        else
            flags |= XC_MISC_FLAG_DONT_USE_DST;

        XSetValue(XC_MISC_FLAGS, REG_DWORD, &flags, sizeof(flags));
    }
}

/*
int CConfig::GetAudioFlags()
{
#ifdef _XBOX
	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_AUDIO_FLAGS, &dwType, &dwFlags, 4, NULL));
	return (int)dwFlags;
#else
	return 0;
#endif
}

void CConfig::SetAudioFlags(int nFlags)
{
#ifdef _XBOX
	VERIFY(!XSetValue(XC_AUDIO_FLAGS, REG_DWORD, (DWORD*)&nFlags, 4));
#else
#endif
}
*/

static DWORD EncodePassword(const TCHAR* szPassword)
{
	DWORD dwPassword = 0;
	for (const TCHAR* pch = szPassword; *pch != 0; pch += 1)
	{
		dwPassword <<= 4;

		switch (*pch)
		{
		case 'u': // up
			dwPassword += 1;
			break;

		case 'd': // down
			dwPassword += 2;
			break;

		case 'l': // left
			dwPassword += 3;
			break;

		case 'r': // right
			dwPassword += 4;
			break;

		case 'a': // A
			dwPassword += 5;
			break;

		case 'b': // B
			dwPassword += 6;
			break;

		case 'x': // X
			dwPassword += 7;
			break;

		case 'y': // Y
			dwPassword += 8;
			break;

		case 'B': // black
			dwPassword += 9;
			break;

		case 'W': // white
			dwPassword += 10;
			break;

		case 'L': // L trigger
			dwPassword += 11;
			break;

		case 'R': // R trigger
			dwPassword += 12;
			break;

        default:
            ASSERT(FALSE);
		}
	}

	return dwPassword;
}

int CConfig::CheckParentPassword(const TCHAR* szCheckPassword)
{
	DWORD dwCheckPassword = EncodePassword(szCheckPassword);

	DWORD dwFlags, dwType;
	VERIFY(!XQueryValue(XC_PARENTAL_CONTROL_PASSWORD, &dwType, &dwFlags, 4, NULL));
	return dwFlags == dwCheckPassword;
}

void CConfig::SetParentPassword(const TCHAR* szNewPassword)
{
	DWORD dwPassword = EncodePassword(szNewPassword);
	VERIFY(!XSetValue(XC_PARENTAL_CONTROL_PASSWORD, REG_DWORD, &dwPassword, 4));
}

int CConfig::GetGamePCFlags()
{
	DWORD dwFlags=0, dwType;
	VERIFY(!XQueryValue(XC_PARENTAL_CONTROL_GAMES, &dwType, &dwFlags, 4, NULL));
    ASSERT(dwFlags <= 6);
	return (int)(6-dwFlags); // revert it since we store level backward in EEPROM
}

void CConfig::SetGamePCFlags(int nFlags)
{
    ASSERT(nFlags <= 6);
    nFlags = 6 - nFlags; // revert it since we store level backward in EEPROM
	VERIFY(!XSetValue(XC_PARENTAL_CONTROL_GAMES, REG_DWORD, (DWORD*)&nFlags, 4));
}

int CConfig::GetMoviePCFlags()
{
	DWORD dwFlags=0, dwType;
	VERIFY(!XQueryValue(XC_PARENTAL_CONTROL_MOVIES, &dwType, &dwFlags, 4, NULL));
    ASSERT(dwFlags <= 7);
	return (int)(7-dwFlags); // revert it since we store level backward in EEPROM
}

void CConfig::SetMoviePCFlags(int nFlags)
{
    ASSERT(nFlags <= 7);
    nFlags = 7 - nFlags; // revert it since we store level backward in EEPROM
	VERIFY(!XSetValue(XC_PARENTAL_CONTROL_MOVIES, REG_DWORD, (DWORD*)&nFlags, 4));
}

CStrObject* CConfig::GetLaunchReason()
{
	const TCHAR* sz = NULL;

	if (theApp.m_bHasLaunchData)
	{
		switch (theApp.m_dwLaunchReason)
		{
		case XLD_LAUNCH_DASHBOARD_ERROR:
			sz = _T("Error");
			break;

		case XLD_LAUNCH_DASHBOARD_MEMORY:
			sz = _T("Memory");
			break;

		case XLD_LAUNCH_DASHBOARD_SETTINGS:
			sz = _T("Settings");
			break;

		case XLD_LAUNCH_DASHBOARD_MUSIC:
			sz = _T("Music");
			break;
		}
	}

	return new CStrObject(sz);
}

int CConfig::GetLaunchContext()
{
    return theApp.m_bHasLaunchData ? theApp.m_dwLaunchContext : 0;
}

int CConfig::GetLaunchParameter1()
{
    return theApp.m_bHasLaunchData ? theApp.m_dwLaunchParameter1 : 0;
}

int CConfig::GetLaunchParameter2()
{
    return theApp.m_bHasLaunchData ? theApp.m_dwLaunchParameter2 : 0;
}

int CConfig::CanDriveBeCleanup(int Drive)
{
    switch (toupper(Drive))
    {
    case 'T':
    case 'U':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
        return true;

    default:
        return false;
    }
}

DWORD CConfig::GetTitleID()
{
    return theApp.m_bHasLaunchData ? theApp.m_dwTitleID : 0;
}

void CConfig::BackToLauncher()
{
    XAppGetD3DDev()->PersistDisplay();

    if (CheckForcedSettings(XLD_SETTINGS_CLOCK|XLD_SETTINGS_TIMEZONE|XLD_SETTINGS_LANGUAGE)) {
        // Dashboard was launched by the kernel to update invalid settings
        HalReturnToFirmware(HalRebootRoutine);
    } else {
        LD_FROM_DASHBOARD fd;
        ZeroMemory(&fd, sizeof(fd));
        fd.dwContext = GetLaunchContext();
        XWriteTitleInfoAndReboot("default.xbe", "\\Device\\CdRom0", LDT_FROM_DASHBOARD, GetTitleID(), (PLAUNCH_DATA)&fd);
    }
}

void CConfig::BackToLauncher2()
{
    XAppGetD3DDev()->PersistDisplay();

    LD_FROM_DASHBOARD fd;
    ZeroMemory(&fd, sizeof(fd));
    fd.dwContext = 'CODA';
    XWriteTitleInfoAndReboot("settings_adoc.xip", "\\Device\\Harddisk0\\Partition2", LDT_FROM_DASHBOARD, 0xfffe0000, (PLAUNCH_DATA)&fd);
}

CStrObject* CConfig::GetRecoveryKey()
{
    CHAR RecoveryKey[RECOVERY_KEY_LEN];
    ComputeRecoveryKey((LPBYTE)XboxHDKey, RecoveryKey);
#ifdef _UNICODE
    TCHAR RecKey[RECOVERY_KEY_LEN];
    Unicode(RecKey, RecoveryKey, RECOVERY_KEY_LEN);
	return new CStrObject(RecKey);
#else  // _UNICODE
	return new CStrObject(RecoveryKey);
#endif // _UNICODE
}

CStrObject* CConfig::GetROMVersion()
{
    TCHAR RomVersion[64];
    _sntprintf(RomVersion, countof(RomVersion), _T("%d.%02d.%d.%02d"),
               XboxKrnlVersion->Major, XboxKrnlVersion->Minor,
               XboxKrnlVersion->Build, (XboxKrnlVersion->Qfe & 0x7FFF));
    return new CStrObject(RomVersion);
}

CStrObject* CConfig::GetXdashVersion()
{
    TCHAR XdashVersion[64];
    const USHORT XdashVerInfo[4] = { VER_PRODUCTVERSION };
    _sntprintf(XdashVersion, countof(XdashVersion), _T("%d.%02d.%d.%02d"),
               XdashVerInfo[0], XdashVerInfo[1], XdashVerInfo[2], XdashVerInfo[3]);
    return new CStrObject(XdashVersion);
}
