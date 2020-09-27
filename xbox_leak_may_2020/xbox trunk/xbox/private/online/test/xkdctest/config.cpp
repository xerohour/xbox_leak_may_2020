#include "xkdctest.h"
#include <stdlib.h>
#include "config.h"


Config::Config()
{
    m_cfgChain = NULL;
    m_szFileName = NULL;
}


Config::~Config()
{
    delete m_szFileName;

    m_strings.DeleteAll();
}


HRESULT Config::initialize(char *szFileName)
{
    HRESULT hr = E_FAIL;
    
    char *szT = NULL;
    if (strchr(szFileName, '\\') == NULL)
    {
        szT = new char[strlen(szFileName) + 3];
        szT[0] = '.';
        szT[1] = '\\';
        strcpy(szT+2, szFileName);
    }
    else
    {
        szT = szFileName;
    }

    FILE *pfileT = NULL;
    if(pfileT = fopen(szT, "r"))
    {
        fclose(pfileT);

        m_szFileName = new char[strlen(szT) + 1];

        if (!m_szFileName)
        {
            hr = E_OUTOFMEMORY;
        }
        else
        {
            strcpy(m_szFileName, szT);
            hr = S_OK;
        }
    }

    if (szT != szFileName)
    {
        delete szT;
    }

    return hr;
}


char *Config::copyTrim(char *sz)
{
    INT i = 0;
    INT j = 0;
    char *szTrimmed = new char[strlen(sz)+1];
    if (!szTrimmed)
        return NULL;
    
    while (sz[i])
    {
        if (sz[i] != ' ')
        {
            szTrimmed[j] = sz[i];

            j++;
        }
        i++;
    }
    szTrimmed[j] = 0;
    return szTrimmed;
}


char *Config::StrDupNoComments(char *sz)
{
    char *szDuped = new char[strlen(sz)+1];

    char *szT = szDuped;

    while (*sz && *sz != '#')
    {
        *szT = *sz;
        szT++;
        sz++;
    }
    
    *szT = 0;

    return szDuped;
}


void Config::getSections(Vector<char*> *vctSections)
{
    const IN nBuf = 4096;
    char szBuf[nBuf];
    char *szBufT = szBuf;

    DWORD dwResult = GetPrivateProfileStringA(NULL, NULL, "", szBufT, nBuf, m_szFileName);

    INT i = 0;
    while(szBufT[i])
    {
        char *szT = StrDupNoComments(szBufT);

        if (strlen(szT))
        {
            m_strings.Add(szT);
            vctSections->Add(szT);
        }

        szBufT += strlen(szBufT) + 1;
    }
}


void Config::getEntries(char *szSection, Vector<char*> *vctKeys)
{
    const IN nBuf = 4096;
    char szBuf[nBuf];
    char *szBufT = szBuf;

    DWORD dwResult = GetPrivateProfileStringA(szSection, NULL, "", szBufT, nBuf, m_szFileName);

    INT i = 0;
    while(szBufT[i])
    {
        char *szT = StrDupNoComments(szBufT);

        if (strlen(szT))
        {
            m_strings.Add(szT);
            vctKeys->Add(szT);
        }

        szBufT += strlen(szBufT) + 1;
    }
}


char *Config::getStrDef(char *szSection, char *szKey, char *szDefault)
{
    if (szSection == NULL || szKey == NULL)
    {
        return NULL;
    }

    char *szForceDefault = szDefault ? szDefault : "DeFaUlT";

    const INT nBuf = 256;
    char szBuf[nBuf];

    DWORD dwResult = GetPrivateProfileStringA(szSection, szKey, szForceDefault, szBuf, nBuf, m_szFileName);

    char *szT = NULL;
    
    if (strcmp(szBuf, "DeFaUlT"))
    {
        szT = StrDupNoComments(szBuf);

        m_strings.Add(szT);
    }

    if (szT || !m_cfgChain)
    {
        return szT;
    }
    else
    {
        return m_cfgChain->getStrDef(szSection, szKey, szDefault);
    }
}


BOOL Config::getBool(char *szSection, char *szKey)
{
    BOOL fResult = FALSE;

    char *sz = getStr(szSection, szKey);
    if (sz)
    {
        _strlwr(sz);
        fResult = strcmp(sz, "y") == 0 || strcmp(sz, "yes") == 0 || strcmp(sz, "1") == 0 || strcmp(sz, "true") == 0 || strcmp(sz, "yep") == 0;
    }

    return fResult;
}


INT Config::getInt(char *szSection, char *szKey)
{
    INT iResult = 0;

    char *sz = getStr(szSection, szKey);
    if (sz)
    {
        iResult = atoi(sz);
    }

    return iResult;
}


float Config::getFloat(char *szSection, char *szKey)
{
    float dResult = 0;

    char *sz = getStr(szSection, szKey);
    if (sz)
    {
        dResult = (float)atof(sz);
    }

    return dResult;
}


DWORD Config::getIP(char *szSection, char *szKey)
{
    DWORD dwIP = 0;

    char *szValue = getStrListItem(szSection, szKey, 0);
    if (szValue)
    {
        dwIP = ToIP(szValue);
    }

    if (!dwIP)
    {
        TestMsg(XKTVerbose_Status, "The specified %s IP address is invalid.\n", szKey);
    }

    return dwIP;
}


USHORT Config::getPort(char *szSection, char *szKey)
{
    USHORT nPort = (USHORT)getIntListItem(szSection, szKey, 1);

    if (nPort == 0)
    {
        return 88;
    }

    return nPort;
}


INT Config::countItems(char *szList)
{
    INT cItems = 0;

    char *szT = szList;
    BOOL fFoundItem = FALSE;
    while(szT[0])
    {
        if (szT[0] == ',')
        {
            fFoundItem = FALSE;
        }
        else if (!fFoundItem && szT[0] != ' ')
        {
            cItems++;
            fFoundItem = TRUE;
        }
        szT++;
    }

    return cItems;
}


BOOL Config::getIntList(char *szSection, char *szKey, INT **rgiItems, INT &cItems)
{
    *rgiItems = NULL;

    char *sz = getStr(szSection, szKey);

    cItems = 0;
    if (sz)
    {
        cItems = countItems(sz);

        (*rgiItems) = new INT[cItems];
    
        for (INT i = 0; i < cItems; i++)
        {
            (*rgiItems)[i] = atoi(sz);
            sz = strchr(sz, ',');
            if (sz)
            {
                sz++;
                if (sz[0] == ' ')
                {
                    sz++;
                }
            }
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}


BOOL Config::getStrList(char *szSection, char *szKey, char ***rgszItems, INT &cItems)
{
    *rgszItems = NULL;

    char *sz = getStr(szSection, szKey);

    cItems = 0;
    if (sz)
    {
        cItems = countItems(sz);

        *rgszItems = new char*[cItems];
    
        char *szT = sz;
        while (*szT)
        {
            if (*szT == ',')
            {
                *szT = 0;
            }
            szT++;
        }

        for (INT i = 0; i < cItems; i++)
        {
            (*rgszItems)[i] = copyTrim(sz);
            m_strings.Add((*rgszItems)[i]);
            sz += strlen(sz) + 1;
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}


INT Config::getBoolListItem(char *szSection, char *szKey, INT nWhich)
{
    BOOL fResult = FALSE;

    char *sz = getStrListItem(szSection, szKey, nWhich);
    if (sz)
    {
        _strlwr(sz);
        fResult = strcmp(sz, "y") == 0 || strcmp(sz, "yes") == 0 || strcmp(sz, "1") == 0 || strcmp(sz, "true") == 0 || strcmp(sz, "yep") == 0;
    }

    return fResult;
}


INT Config::getIntListItem(char *szSection, char *szKey, INT nWhich)
{
    INT iResult = 0;

    char *sz = getStrListItem(szSection, szKey, nWhich);
    if (sz)
    {
        iResult = atoi(sz);
    }

    return iResult;
}


char *Config::getStrListItem(char *szSection, char *szKey, INT nWhich)
{
    char *szList = getStr(szSection, szKey);

    if (!szList)
    {
        return NULL;
    }

    while (szList && *szList && nWhich != 0)
    {
        if (*szList == ',')
        {
            if (nWhich-- == 0)
            {
                szList++;
                break;
            }
        }
        szList++;
    }

    char *szNew = NULL;

    if (*szList)
    {
        while (*szList && *szList == ' ') szList++;
        if (*szList == '"') szList++;

        // This often gives us more space than we need but it's simpler
        szNew = new char[strlen(szList) + 1];
        char *szT = szNew;

        while (*szList && *szList != ',')
        {
            *szT = *szList;
            szList++;
            szT++;
        }
        szT--;

        while (szT != szNew && *szT == ' ') szT--;
        if (*szT == '"') szT--;

        *szT++;
        *szT = 0;

        m_strings.Add(szNew);
    }

    return szNew;
}

