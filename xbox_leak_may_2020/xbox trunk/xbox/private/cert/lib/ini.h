/*
 *
 * ini.h
 *
 * CIniFile
 *
 */

#ifndef _INI_H
#define _INI_H

#include <propbag.h>
#include <xsum.h>

class CIniBag : public CPropBag <CVarBag>
{
public:
    CIniBag() { m_pfnRemovePv = RemovePv; }
    CVarBag *PbagGetSection(LPCSTR);
protected:
    virtual PVOID PvAdd(PVOID pv) const
    {
        return pv;
    }
    static void RemovePv(PVOID pv)
    {
        delete (CVarBag *)pv;
    }
};

class CIniFile
{
public:
    CIniFile(void) : m_pbag(NULL) {}
    ~CIniFile() { Empty(); }

    void Empty(void)
    {
        if(m_pbag) {
            delete m_pbag;
            m_pbag = NULL;
        }
    }

    BOOL FReadFile(LPCSTR szFileName, BOOL fRequireXsum);
    BOOL FWriteFile(LPCSTR szFileName);
    CIniBag *PbagRoot(void)
    {
        if(!m_pbag)
            m_pbag = new CIniBag;
        return m_pbag;
    }
    DWORD DwXsum(void) const { return m_cxc.DwFinalSum(); }

private:
    CCheckSum m_cxc;
    CIniBag *m_pbag;

    void CalcXsum(void);
    static const LPCSTR szRootSection;
    static const LPCSTR szCRCKey;
};
  
#endif // _INI_H
