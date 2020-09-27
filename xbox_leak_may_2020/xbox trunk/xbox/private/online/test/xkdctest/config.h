#ifndef CONFIG_H
#define CONFIG_H

#include "xkdctest.h"
#include "vector.h"

class Config {
private:
    Vector<char*> m_strings;
    char *m_szFileName;
    Config *m_cfgChain;

    char *copyTrim(char *sz);
    char *StrDupNoComments(char *sz);
    INT countItems(char *szList);

public:
    Config();
    ~Config();

    HRESULT initialize(char *szFileName);

    void chainConfig(Config *cfgChain) {m_cfgChain = cfgChain;}

    void getSections(Vector<char*> *vctSections);
    void getEntries(char *szSection, Vector<char*> *vctKeys);
    char *getStrDef(char *szSection, char *szKey, char *szDefault);
    char *getStr(char *szSection, char *szKey)  {return getStrDef(szSection, szKey, NULL);}
    BOOL getBool(char *szSection, char *szKey);
    INT getInt(char *szSection, char *szKey);
    float getFloat(char *szSection, char *szKey);
    DWORD getIP(char *szSection, char *szKey);
    USHORT getPort(char *szSection, char *szKey);
    BOOL getIntList(char *szSection, char *szKey, INT **rgiItems, INT &cItems);
    BOOL getStrList(char *szSection, char *szKey, char ***rgszItems, INT &cItems);
    INT getBoolListItem(char *szSection, char *szKey, INT nWhich);
    INT getIntListItem(char *szSection, char *szKey, INT nWhich);
    char *getStrListItem(char *szSection, char *szKey, INT nWhich);
};

#endif
