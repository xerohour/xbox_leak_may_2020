//  GLOBAL.H
//
//  Created 29-Mar-2001 [JonT]

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//---------------------------------------------------------------------
//  Global 'settings cab' filenames

#define SCAB_MANIFEST       TEXT("manifest.csv")
#define SCAB_SETTINGS       TEXT("settings.ini")
#define SCAB_LICENSE        TEXT("license.txt")
#define SCAB_CUSTOMDLL      TEXT("custom.dll")
#define SCAB_UNINSTALLDIR   TEXT("uninstall")
#define SCAB_UNINSTALLER    TEXT("xbuninst2.exe")
#define SCAB_UNINSTALLFILE  TEXT("uninstall.csv")

//---------------------------------------------------------------------
// Misc global helpers

inline void
AppendSlash(
    LPSTR lpPath
    )
{
    if (*(lpPath + strlen(lpPath) - 1) != '\\')
        strcat(lpPath, "\\");
}



#endif // #ifndef _GLOBAL_H_
