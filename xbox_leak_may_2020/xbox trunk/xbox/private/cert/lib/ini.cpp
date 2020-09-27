/*
 *
 * ini.h
 *
 * CIniFile
 *
 */

#include "precomp.h"

const LPCSTR CIniFile::szRootSection = ".";
const LPCSTR CIniFile::szCRCKey = "FileCRC";

CVarBag *CIniBag::PbagGetSection(LPCSTR sz)
{
    CVarBag *pbag = FindProp(sz);
    if(!pbag) {
        pbag = new CVarBag;
        if(!FAddProp(sz, pbag)) {
            delete pbag;
            pbag = NULL;
        }
    }
    return pbag;
}

BOOL CIniFile::FReadFile(LPCSTR szName, BOOL fRequireXsum)
{
    FILE *pfl;
    char szLine[256];
    char *pchKey;
    char *pchValue;
    char *pch;
    CVarBag *pbvpCur;
    VPR vpT;
    PVOID espSav;

    Empty();
    pfl = fopen(szName, "r");
    if(!pfl)
        return FALSE;

    m_pbag = new CIniBag;
    _asm mov espSav, esp

    /* Create the root section */
    pbvpCur = new CVarBag;
    m_pbag->FAddProp(szRootSection, pbvpCur);

    /* Now read the file */
    while(fgets(szLine, sizeof szLine, pfl)) {
        for(pchKey = szLine; isspace(*pchKey); ++pchKey);
        if(*pchKey == '[') {
            /* New section */
            pch = strchr(++pchKey, ']');
            if(pch) {
                *pch = 0;
                /* We coalesce sections with the same name */
                pbvpCur = m_pbag->FindProp(pchKey);
                if(!pbvpCur) {
                    pbvpCur = new CVarBag;
                    m_pbag->FAddProp(pchKey, pbvpCur);
                }
            }
        } else {
            /* Value */
            pchValue = strchr(pchKey, '=');
            if(!pchValue || pchValue == pchKey)
                /* Bogus line */
                continue;
            *pchValue = ' ';
            while(isspace(*pchValue))
                --pchValue;
            *++pchValue = 0;
            while(isspace(*++pchValue));

            if(*pchValue == '"') {
                /* This is a string */
                _asm mov esp, espSav
                vpT.vpType = vpSz;
                vpT.sz = pch = (LPSTR)_alloca(strlen(pchValue + 1));
                /* Copy the string up to the final quote */
                while(*++pchValue && *pchValue != '"') {
                    if(*pchValue == '\\')
                        ++pchValue;
                    *pch++ = *pchValue;
                }
                *pch = 0;
            } else
                /* Assume a dword, unless the parse fails */
                vpT.vpType = 1 == sscanf(pchValue, "%i", &vpT.dw) ?
                    vpDw : vpNone;

            /* Now add this guy to the tree */
            pbvpCur->FAddProp(pchKey, &vpT);
            vpT.vpType = vpNone;
        }
    }

    fclose(pfl);

    /* If there was a checksum in the file, verify it */
    pbvpCur = m_pbag->FindProp(szRootSection);
    if(pbvpCur) {
        VPR *pvpCRC = pbvpCur->FindProp(szCRCKey);
        if(pvpCRC && pvpCRC->vpType == vpDw) {
            CalcXsum();
            if(m_cxc.DwFinalSum() == pvpCRC->dw)
                return TRUE;
        }
    }
    if(fRequireXsum) {
        /* We needed the checksum but we didn't find one we liked */
        Empty();
        return FALSE;
    }

    return TRUE;
}

void CIniFile::CalcXsum(void)
{
    LPCSTR *rgszSections;
    CVarBag **rgpbvp;
    int cbvp;
    int ibvpRoot;
    PVOID espSav;

    m_cxc.Reset();

    /* Get the section list and move the root section to the front, if
     * present */
    cbvp = m_pbag->Cprop();
    rgpbvp = (CVarBag **)_alloca(sizeof(PVOID) * cbvp);
    rgszSections = (LPCSTR *)_alloca(sizeof(PVOID) * cbvp);
    m_pbag->GetProps(rgszSections, rgpbvp);
    if(!m_pbag->FindProp(szRootSection, &ibvpRoot))
        ibvpRoot = -1;
    else if(ibvpRoot > 0) {
        CVarBag *pbvpRoot = rgpbvp[ibvpRoot];
        LPCSTR szRoot = rgszSections[ibvpRoot];

        memmove(rgpbvp + 1, rgpbvp, sizeof(PVOID) * ibvpRoot);
        memmove(rgszSections + 1, rgszSections, sizeof(PVOID) * ibvpRoot);
        rgpbvp[0] = pbvpRoot;
        rgszSections[0] = szRoot;
        ibvpRoot = 0;
    }

    _asm mov espSav, esp

    /* Now enumerate every section and print out its contents */
    for(int ibvp = 0; ibvp < cbvp; ++ibvp) {
        CVarBag *pbvp = rgpbvp[ibvp];
        int cvp = pbvp->Cprop();
        int ivpCRC;
        LPCSTR *rgszKeys;
        VPR **rgpvpValues;
        PVOID espT;
        int cchKey;
        int cchValue;
        char *pchT;
        LPSTR szValue;

        _asm mov esp, espSav
        rgszKeys = (LPCSTR *)_alloca(sizeof(LPCSTR) * cvp);
        rgpvpValues = (VPR **)_alloca(sizeof(LPCSTR) * cvp);
        _asm mov espT, esp

        pbvp->GetProps(rgszKeys, rgpvpValues);
        if(ibvp != ibvpRoot || !pbvp->FindProp(szCRCKey, &ivpCRC))
            ivpCRC = -1;

        /* If not the root section, CRC the section name */
        if(ibvp != ibvpRoot) {
            cchKey = strlen(rgszSections[ibvp]);
            _asm mov esp, espT
            pchT = (char *)_alloca(cchKey + 5);
            sprintf(pchT, "\n[%s]\n", rgszSections[ibvp]);
            m_cxc.SumBytes((LPBYTE)pchT, cchKey + 4);
        }

        /* Enumerate all the key/value pairs and checksum their contents */
        for(int ivp = 0; ivp < cvp; ++rgszKeys, ++rgpvpValues, ++ivp) {
            if(ivp != ivpCRC) {
                _asm mov esp, espT;
                cchKey = strlen(*rgszKeys);
                switch(rgpvpValues[0]->vpType) {
                case vpSz:
                    cchValue = 2 + strlen(rgpvpValues[0]->sz);
                    szValue = (LPSTR)_alloca(cchValue) + 1;
                    sprintf(szValue, "\"%s\"", rgpvpValues[0]->sz);
                    break;
                case vpDw:
                    cchValue = 10;
                    szValue = (LPSTR)_alloca(cchValue) + 1;
                    sprintf(szValue, "0x%08X", rgpvpValues[0]->dw);
                    break;
                default:
                    cchValue = 0;
                    break;
                }
                pchT = (char *)_alloca(cchKey + cchValue + 5);
                if(cchValue) {
                    sprintf(pchT, "%s = %s\n", *rgszKeys, szValue);
                    m_cxc.SumBytes((LPBYTE)pchT, cchKey + cchValue + 4);
                } else {
                    sprintf(pchT, "%s =\n", *rgszKeys);
                    m_cxc.SumBytes((LPBYTE)pchT, cchKey + 3);
                }
            }
        }
    }
}

BOOL CIniFile::FWriteFile(LPCSTR szName)
{
    FILE *pfl;
    LPCSTR *rgszSections;
    CVarBag **rgpbvp;
    int cbvp;
    int ibvpRoot;
    PVOID espSav;

    if(!m_pbag)
        /* Call this an empty bag */
        m_pbag = new CIniBag;

    pfl = fopen(szName, "w");
    if(!pfl)
        return FALSE;

    /* Get the section list.  If there is no root section make one; if there
     * is a root section, move it to the front */
    cbvp = m_pbag->Cprop();
    if(!m_pbag->FindProp(szRootSection, &ibvpRoot)) {
        ++cbvp;
        ibvpRoot = -1;
    }
    rgpbvp = (CVarBag **)_alloca(sizeof(PVOID) * cbvp);
    rgszSections = (LPCSTR *)_alloca(sizeof(PVOID) * cbvp);
    m_pbag->GetProps(rgszSections + (ibvpRoot < 0), rgpbvp + (ibvpRoot < 0));
    if(ibvpRoot < 0) {
        rgszSections[0] = szRootSection;
        rgpbvp[0] = new CVarBag;
        m_pbag->FAddProp(rgszSections[0], rgpbvp[0]);
    } else if(ibvpRoot > 0) {
        CVarBag *pbvpRoot = rgpbvp[ibvpRoot];
        LPCSTR szRoot = rgszSections[ibvpRoot];

        memmove(rgpbvp + 1, rgpbvp, sizeof(PVOID) * ibvpRoot);
        memmove(rgszSections + 1, rgszSections, sizeof(PVOID) * ibvpRoot);
        rgpbvp[0] = pbvpRoot;
        rgszSections[0] = szRoot;
    }

    /* Stash the checksum value in the bag */
    CalcXsum();
    VPR vpXsum(m_cxc.DwFinalSum());
    if(!rgpbvp[0]->FChangeProp(szCRCKey, &vpXsum))
        rgpbvp[0]->FAddProp(szCRCKey, &vpXsum);

    _asm mov espSav, esp

    /* Now enumerate every section and print out its contents */
    for(int ibvp = 0; ibvp < cbvp; ++ibvp) {
        CVarBag *pbvp = rgpbvp[ibvp];
        int cvp = pbvp->Cprop();
        LPCSTR *rgszKeys;
        VPR **rgpvpValues;

        _asm mov esp, espSav
        rgszKeys = (LPCSTR *)_alloca(sizeof(LPCSTR) * cvp);
        rgpvpValues = (VPR **)_alloca(sizeof(LPCSTR) * cvp);
        pbvp->GetProps(rgszKeys, rgpvpValues);
        if(ibvp == 0) {
            /* Make sure the CRC key is first in the list */
            int ivpCRC;
            if(pbvp->FindProp(szCRCKey, &ivpCRC) && ivpCRC > 0) {
                VPR *pvpCRC = rgpvpValues[ivpCRC];

                memmove(rgszKeys + 1, rgszKeys, sizeof(LPCSTR) * ivpCRC);
                memmove(rgpvpValues + 1, rgpvpValues, sizeof(LPCSTR) * ivpCRC);
                rgszKeys[0] = szCRCKey;
                rgpvpValues[0] = pvpCRC;
            }
        } else
            /* Need a section name */
            fprintf(pfl, "\n[%s]\n", rgszSections[ibvp]);

        /* Print all the key/value pairs */
        for(; cvp--; ++rgszKeys, ++rgpvpValues) {
            switch(rgpvpValues[0]->vpType) {
            case vpSz:
                fprintf(pfl, "%s = \"%s\"\n", *rgszKeys, rgpvpValues[0]->sz);
                break;
            case vpDw:
                fprintf(pfl, "%s = 0x%08X\n", *rgszKeys, rgpvpValues[0]->dw);
                break;
            default:
                fprintf(pfl, "%s =\n", *rgszKeys);
                break;
            }
        }
    }

    fclose(pfl);

    return TRUE;
}
