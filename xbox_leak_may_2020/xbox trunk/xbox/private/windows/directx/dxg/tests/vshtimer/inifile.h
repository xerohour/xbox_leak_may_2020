//-----------------------------------------------------------------------------
// FILE: INIFILE.H
//
// Desc: Quick and dirty INI file routines
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

// Routines supplied by the main application to return the values for a given
// string found in an ini file.
bool GetIniConst(const char *szStr, int cchStr, int *pval);
bool GetIniConstf(const char *szStr, int cchStr, float *pval);

//=========================================================================
// IniFile class
//=========================================================================
class CIniFile
{
public:
    CIniFile(LPCSTR szFileName) : m_szFile(NULL), m_ftFileReadTime(0), m_fverbose(false), m_recurselevel(0)
        { lstrcpyA(m_szFileName, szFileName); }
    ~CIniFile()
    {
        if(m_szFile)
        {
            free(m_szFile);
            m_szFile = NULL;
        }
    };

    // get int entry
    int GetIniInt(LPCSTR lpSectionName, LPCSTR lpKeyName, int nDefault);

    // get float entry
    float GetIniFloat(LPCSTR lpSectionName, LPCSTR lpKeyName, float nDefault);

    // check for entry existance
    bool IniEntryExists(LPCSTR lpSectionName, LPCSTR lpKeyName);

    // get ini string value. Up to caller to parse and check for end '\n' or ';'
    bool GetIniStrBuf(LPCSTR lpSectionName, LPCSTR lpKeyName,
        LPCSTR szDefault, char *buf, int buflen);

    // get ini string value. Up to caller to parse and check for end '\n' or ';'
    LPCSTR GetIniStr(LPCSTR lpSectionName, LPCSTR lpKeyName,
        LPCSTR szDefault, LPCSTR *ppszLineStart);

    // read/refresh an ini file
    bool ReadFile();

    // get name of ini file
    LPCSTR GetFileName()
        { return m_szFileName; }

    // has ini file timestamp changed?
    bool FileChanged()
        { return GetFileTime(m_szFileName) != m_ftFileReadTime; }

    bool m_fverbose;

private:
    enum { INIVAL_INT, INIVAL_FLOAT };
    struct INIVAL
    {
        int type;

        int val;
        float valf;

        void SetVal(int newval)
            { val = newval; valf = (float)newval; }
        void SetVal(float newval)
            { val = (int)newval; valf = newval; }
    };

    LPCSTR GetIniConstValue(const char *szStr, INIVAL *pinival);

    bool ParseIntVal(LPCSTR szLineStart, LPCSTR szLine, int *pval);
    bool ParseFloatVal(LPCSTR szLineStart, LPCSTR szLine, float *pval);

    char m_szFileName[MAX_PATH];
    char *m_szFile;
    UINT64 m_ftFileReadTime;

    UINT m_recurselevel;
};

