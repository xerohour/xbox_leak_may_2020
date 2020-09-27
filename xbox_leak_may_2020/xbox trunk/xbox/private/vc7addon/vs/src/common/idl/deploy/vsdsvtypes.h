// vsdsvtypes.h

#pragma once

typedef enum __DSVPLATFORMTYPE
{
  DSVPT_UNKNOWN = 0,
  DSVPT_IX86    = 1,
  DSVPT_ALPHA   = 2,
  DSVPT_MRX000  = 3,
  DSVPT_PPC     = 4,
  DSVPT_MPPC    = 5,
  DSVPT_M68K    = 6
} DSVPLATFORMTYPE;

typedef struct _DSVVERSIONINFO
{
	UINT m_uMajorVersion;
	UINT m_uMinorVersion;
	UINT m_uBuildVersion;
} DSVVersionInfo;

typedef struct _DSVPLATFORMINFO
{
	UINT  m_uCodePage;
    DWORD m_dwMajorVersion;
    DWORD m_dwMinorVersion;
    DWORD m_dwBuildNumber;
    DWORD m_dwPlatformId;
	DSVPLATFORMTYPE m_dsvptPlatform; // Platform of OS (actually, platform DSV was compiled on...)
	WCHAR m_rgwchComputerName[256];
	DWORD m_dwComputerNameSize;
	BOOL m_fIIS4Installed;
} DSVPlatformInfo;

typedef struct _DSVFILEINFO
{
	BOOL m_fFileExists;
    DWORD m_dwFileAttributes;
    DWORD m_ftCreationTimeHigh;
    DWORD m_ftCreationTimeLow;
    DWORD m_ftLastAccessTimeHigh;
    DWORD m_ftLastAccessTimeLow;
    DWORD m_ftLastWriteTimeHigh;
    DWORD m_ftLastWriteTimeLow;
    DWORD m_nFileSizeHigh;
    DWORD m_nFileSizeLow;
	DWORD m_dwVersionSize;
	DWORD m_dwVersionSizeUsed;
	char* m_lpVersionData; // Version info

} DSVFileInfo;

typedef struct _DSVINFO
{
	DSVVersionInfo m_dsvVersionInfo;
	DSVPlatformInfo m_dsvPlatformInfo;
} DSVInfo;

typedef struct _DSVSERVICEINFO
{
	DSVVersionInfo m_dsvVersionInfo; // Version of the service
	GUID m_guidService; // Service GUID
	BOOL m_fServiceExists;
	BOOL m_fServiceCanRun;
	WCHAR szFriendlyName[256];
	WCHAR szDescription[256];
} DSVServiceInfo;

enum __VSDEPLOYMENTFLAGS
{
	VSDEPLOYMENTFLAGS_UninstallOnly =		0x00000001,
	VSDEPLOYMENTFLAGS_GenerateLogFile =		0x00000002,
	VSDEPLOYMENTFLAGS_InstallAllUsers =		0x00000004
};

typedef DWORD VSDEPLOYMENTFLAGS;
