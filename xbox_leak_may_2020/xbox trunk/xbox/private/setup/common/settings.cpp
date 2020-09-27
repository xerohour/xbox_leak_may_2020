//  SETTINGS.CPP
//
//  Created 12-Mar-2001 [JonT]

#include "common.h"
#define SETTINGS
#include "settings.h"

#define MAX_INI_STRING  4096

//---------------------------------------------------------------------

//  CSettingsFile::SetFile
//      Sets an INI file for use with the library
//      fMakeCopy signals if we should work off a copy so that changes can
//      be made without affecting the master

BOOL
CSettingsFile::SetFile(
    LPCTSTR lpIniFile,
    BOOL fMakeCopy
    )
{
    TCHAR szTempPath[MAX_PATH];
    TCHAR szTempFile[MAX_PATH];

    // Free any previous files
    if (m_lpIniFile)
        Close();

    m_fMakeCopy = fMakeCopy;
    if (!fMakeCopy)
    {
        m_lpIniFile = _strdup(lpIniFile);
    }

    // Make a copy. We only keep track of the new name
    else
    {
        GetTempPath(MAX_PATH, szTempPath);
        GetTempFileName(szTempPath, "pak", 0, szTempFile);
        if (!CopyFile(lpIniFile, szTempFile, FALSE))
            return FALSE;
        //BUG 9441, we edit the copy.  The original is usually read-only
        //(as it is checked in).  Copy file preserves the read-only attributes
        //so we must clear or we will not be able to edit it, or delete when we
        //are done.
        if(!SetFileAttributes(szTempFile, FILE_ATTRIBUTE_NORMAL))
            return FALSE;
        m_lpIniFile = _strdup(szTempFile);
    }

    return TRUE;
}


CSettingsFile::~CSettingsFile()
{
    Close();
}


//  CSettingsFile::Close
//      We're done with the INI file or its copy

void
CSettingsFile::Close()
{
    if (m_lpIniFile)
    {
        if (m_fMakeCopy)
        {
            DeleteFile(m_lpIniFile);
        }
        free (m_lpIniFile);
        m_lpIniFile = NULL;
    }
}


//  CSettingsFile::WriteINIString
//      Writes a string to the INI file

BOOL
CSettingsFile::WriteINIString(
    LPCTSTR lpSection,
    LPCTSTR lpKey,
    LPCTSTR lpValue
    )
{
    return WritePrivateProfileString(lpSection, lpKey, lpValue, m_lpIniFile);
}


//  CSettingsFile::NukeINISection
//      Wipes out a full section

VOID
CSettingsFile::NukeINISection(
    LPCTSTR lpSection
    )
{
    WritePrivateProfileSection(lpSection, "", m_lpIniFile);
}


//  CSettingsFile::GetINIString
//      Returns a preprocessed string in a dynamically allocated buffer.
//      Caller's responsibility to free the buffer.

LPTSTR
CSettingsFile::GetINIString(
    LPCTSTR lpSection,
    LPCTSTR lpKey
    )
{
    LPTSTR lpStart;
    LPTSTR lp;
    LPTSTR lpT;
    LPTSTR lpNew;
    LPTSTR lpRead;
    int nLen;

    // Allocate a buffer to initially read into
    lpStart = (LPTSTR)malloc(MAX_INI_STRING);

    // Adjust for the current language
    // First check the language specific section, and then fall back to the default string
    TCHAR szLocedSectionName[MAX_PATH];
    sprintf( szLocedSectionName, "%s_%04X", lpSection, GetUserDefaultUILanguage() );

    // Read the string from the INI file - Trye specific language first, then fall back to the default
    if( 0 == GetPrivateProfileString(szLocedSectionName, lpKey, "", lpStart, MAX_INI_STRING, m_lpIniFile) )
        GetPrivateProfileString(lpSection, lpKey, "", lpStart, MAX_INI_STRING, m_lpIniFile);

    // Walk through the string and substitute any additional keys
    for (lp = lpStart ; *lp ; ++lp)
    {
        // Check for environment var
        if (*lp == '%')
        {
            // Walk until we find the end
            for (lpT = lp + 1 ; *lpT != '%' ; ++lpT)
            {
                // If we hit the end of string beforehand, this means there is an unterminated
                // marker. This means the author messed up or didn't realize this was a reserved
                // char. Save them here by just returning the string without any further substitution
                if (*lpT == 0)
                    goto bail;
            }

            // Make a new copy of the string up to the marker
            *lp = 0;
            lpNew = (LPTSTR)malloc(MAX_INI_STRING);
            strcpy(lpNew, lpStart);
            *lpT = 0;

            // Check for empty string. This means it was a %% which should be replaced with %.
            if (lpT == lp + 1)
            {
                strcat(lpNew, "%");
            }

            // If it is not, go ahead and get the length of the env var, allocate buffer and read into it
            else
            {
                nLen = (int)GetEnvironmentVariable(lp + 1, lpT, 0);
                if (nLen)
                {
                    lpRead = (LPTSTR)malloc(nLen);
                    GetEnvironmentVariable(lp + 1, lpRead, nLen);
                    strcat(lpNew, lpRead);
                    free(lpRead);
                }
            }

            // Attach the rest of the string
            lp = lpNew + strlen(lpNew) - 1; // we'll get incremented shortly
            strcat(lpNew, lpT + 1);
            free(lpStart);
            lpStart = lpNew;
        }

        // Check for string substitution
        else if (*lp == '@')
        {
            // Walk until we find the end
            for (lpT = lp + 1 ; *lpT != '@' ; ++lpT)
            {
                // If we hit the end of string beforehand, this means there is an unterminated
                // marker. This means the author messed up or didn't realize this was a reserved
                // char. Save them here by just returning the string without any further substitution
                if (*lpT == 0)
                    goto bail;
            }

            // Make a new copy of the string up to the marker
            *lp = 0;
            lpNew = (LPTSTR)malloc(MAX_INI_STRING);
            strcpy(lpNew, lpStart);
            *lpT = 0;

            // Check for empty string. This means it was a @@ which should be replaced with @
            if (lpT == lp + 1)
            {
                strcat(lpNew, "@");
            }

            // If it's not, read the substitute string and add it on
            else
            {
                lpRead = (LPTSTR)malloc(MAX_INI_STRING);

                // Adjust for the current language
                // First check the language specific section, and then fall back to the default string
                TCHAR szLocedSubSectionName[MAX_PATH];
                sprintf( szLocedSubSectionName, "%s_%04X", m_strTable[CSTR_STRINGS], GetUserDefaultUILanguage() );

                // Read the string from the INI file - Trye specific language first, then fall back to the default
                if( 0 == GetPrivateProfileString(szLocedSubSectionName, lp + 1, "", lpRead, MAX_INI_STRING, m_lpIniFile) )
                    GetPrivateProfileString(m_strTable[CSTR_STRINGS], lp + 1, "", lpRead, MAX_INI_STRING, m_lpIniFile);

                strcat(lpNew, lpRead);
                free(lpRead);
            }

            // Attach the rest of the string
            lp = lpNew + strlen(lpNew) - 1; // we'll get incremented shortly
            strcat(lpNew, lpT + 1);
            free(lpStart);
            lpStart = lpNew;
        }
    }

    // Return a buffer just the right length
bail:
    lp = _strdup(lpStart);
    free(lpStart);
    return lp;
}


#define MAX_KEYNAMES     4096
                                

//  CSettingsFile::EnumerateINISection
//      Returns a structure with an entire section parsed.

LPSECTIONENUM
CSettingsFile::EnumerateINISection(
    LPCTSTR lpSection
    )
{
    TCHAR KeyNames[MAX_KEYNAMES];
    LPTSTR lp;
    DWORD dwc = 0;
    LPSECTIONENUM lpEnum;
    DWORD i;

    // Get all the key names for this section
    // Adjust for the current language
    // First check the language specific section, and then fall back to the default string
    TCHAR szLocedSectionName[MAX_PATH];
    sprintf( szLocedSectionName, "%s_%04X", lpSection, GetUserDefaultUILanguage() );

    // Read the string from the INI file - Trye specific language first, then fall back to the default
    if( 0 == GetPrivateProfileString(szLocedSectionName, NULL, "", KeyNames, MAX_KEYNAMES, m_lpIniFile) )
        GetPrivateProfileString(lpSection, NULL, "", KeyNames, MAX_KEYNAMES, m_lpIniFile);

    // Count all the sections
    lp = KeyNames;

    // If we have at least one key, count them
    if (*lp != 0 || *(lp + 1) != 0)
    {
        while (TRUE)
        {
            // Skip to a zero
            for (; *lp ; ++lp)
                ;

            ++dwc;

            // Two zeros in a row mean we're done
            if (*++lp == 0)
                break;
        }
    }

    // Allocate the structure to return
    lpEnum = (LPSECTIONENUM)malloc(sizeof (SECTIONENUM) + (dwc - 1) * sizeof (KEYINFO));

    // Fill in the structure
    lpEnum->dwcKeys = dwc;
    for (i = 0, lp = KeyNames ; i < dwc ; ++i, lp += strlen(lp) + 1)
    {
        lpEnum->keys[i].lpKey = _strdup(lp);
        lpEnum->keys[i].lpValue = GetINIString(lpSection, lp);
    }

    return lpEnum;
}


//  CSettingsFile::EnumerateFree
//      Frees memory associated with an enumeration

void
CSettingsFile::EnumerateFree(
        LPSECTIONENUM lpEnum
        )
{
    DWORD i;

    for (i = 0 ; i < lpEnum->dwcKeys ; ++i)
    {
        free(lpEnum->keys[i].lpKey);
        free(lpEnum->keys[i].lpValue);
    }

    free(lpEnum);
}
