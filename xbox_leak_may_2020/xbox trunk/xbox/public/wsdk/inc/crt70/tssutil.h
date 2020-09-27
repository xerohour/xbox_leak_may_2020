#ifndef TSSUTIL_H
#define TSSUTIL_H

#include <bpcreg.h>

#if defined(_AFX)
#include <afxpriv.h>
// build a class to override the standard GUID in basetyps.h
// in order to put them into STL containers and dump them to mfc dumpcontexts.
class GUID2 : public GUID {
public:
        inline GUID2() {}
        inline GUID2(const GUID2 &g) {
            ASSERT(sizeof(*this) == sizeof(g));
            memcpy(this, &g, sizeof(*this));
        }
        inline GUID2(const struct _GUID &g) {
            ASSERT(sizeof(*this) == sizeof(g));
            memcpy(this, &g, sizeof(*this));
        }
        inline GUID2(const struct _GUID *g) {
            ASSERT(sizeof(*this) == sizeof(*g));
            memcpy(this, g, sizeof(*this));
        }
        GUID2 & operator=(const GUID2 &g) {
            if (&g != this) {
                ASSERT(sizeof(*this) == sizeof(struct _GUID));
                memcpy(this, &g, sizeof(*this));
            }
            return *this;
        }
        GUID2 & operator=(const struct _GUID &g) {
            if (&g != this) {
                ASSERT(sizeof(*this) == sizeof(g));
                memcpy(this, &g, sizeof(*this));
            }
            return *this;
        }
        void inline Dump(CDumpContext &dc) const {
            OLECHAR guidstr[64];
            int rc = StringFromGUID2(*this, guidstr, sizeof(guidstr));
            ASSERT(rc);
            USES_CONVERSION;
            dc << OLE2T(guidstr);
            return;
        }
        bool inline operator<(const GUID2 &op2) const {
            if (memcmp(this, &op2, sizeof(GUID2)) < 0) {
                    return true;
            }
            return false;
        }
        bool inline operator>(const GUID2 &op2) const {
            if (memcmp(this, &op2, sizeof(GUID2)) > 0) {
                    return true;
            }
            return false;
        }
        bool inline operator==(const GUID2 &op2) const {
            if (!memcmp(this, &op2, sizeof(GUID2))) {
                    return true;
            }
            return false;
        }
        bool inline operator!=(const GUID2 &op2) const {
            if (memcmp(this, &op2, sizeof(GUID2))) {
                    return true;
            }
            return false;
        }
        bool inline operator<(const GUID &op2) const {
            if (memcmp(this, &op2, sizeof(GUID)) < 0) {
                    return true;
            }
            return false;
        }
        bool inline operator>(const GUID &op2) const {
            if (memcmp(this, &op2, sizeof(GUID)) > 0) {
                    return true;
            }
            return false;
        }
        bool inline operator==(const GUID &op2) const {
            if (!memcmp(this, &op2, sizeof(GUID))) {
                    return true;
            }
            return false;
        }
        bool inline operator!=(const GUID &op2) const {
            if (memcmp(this, &op2, sizeof(GUID))) {
                    return true;
            }
            return false;
        }

};

inline CDumpContext &operator<<(CDumpContext &dc, const GUID2 &g)
{
    g.Dump(dc);
    return dc;
}

inline CDumpContext &operator<<(CDumpContext &dc, const GUID &g)
{
    GUID2 g2(g);
    g2.Dump(dc);
    return dc;
}

class CTuningSpaceInfo {
    inline void copy(CTuningSpaceInfo &lhs, const CTuningSpaceInfo &rhs) {
        ASSERT(&rhs != &lhs);
        lhs.m_lTS = rhs.m_lTS;
        lhs.m_csName = rhs.m_csName;
        lhs.m_csDesc = rhs.m_csDesc;
        lhs.m_FilterClass = rhs.m_FilterClass;
        lhs.m_FilterCategory = rhs.m_FilterCategory;
        lhs.m_csFilterName = rhs.m_csFilterName;
        lhs.m_dwInputType = rhs.m_dwInputType;
    }
public:
    long m_lTS;
    CString m_csName;
    CString m_csDesc;
    CLSID m_FilterClass;
    CLSID m_FilterCategory;
    CString m_csFilterName;
    DWORD m_dwInputType;

    CTuningSpaceInfo() {}
    CTuningSpaceInfo(long lTS) : m_lTS(lTS) {}
    inline CTuningSpaceInfo(const CTuningSpaceInfo &tsi) { copy(*this, tsi); }
    inline CTuningSpaceInfo &operator=(const CTuningSpaceInfo &rhs) {
        if (&rhs != this) {
            copy(*this, rhs);
        }
        return *this;
    }
};

inline CDumpContext& operator<<(CDumpContext &d, const CTuningSpaceInfo&tsi) {
    d << "Tuning Space = " << tsi.m_lTS << ":\r\n";
    d << "\tName = " << tsi.m_csName << "\r\n";
    d << "\tDesc = " << tsi.m_csName << "\r\n"; 
    d << "\tFilter Class = " << static_cast<GUID2>(tsi.m_FilterClass) << "\r\n";
    d << "\tFilter Category = " << static_cast<GUID2>(tsi.m_FilterCategory) << "\r\n";
    d << "\tFilter Name = " << tsi.m_csFilterName << "\r\n";
    d << "\tInput Type = " << tsi.m_dwInputType << "\r\n";
    return d;
}

#endif

const int g_nMaxNameLen = 64;

extern const LPCTSTR g_strRestrictSuffix;

extern const LPCTSTR g_strAdultsGrp;
extern const LPCTSTR g_strUsersGrp;
extern const LPCTSTR g_strGuestAccountName;
extern const LPCTSTR g_strRestrictQueryOwner;
extern const LPCTSTR g_strRestrictQueryBase;
extern const LPCTSTR g_strQueryBase;


#ifdef _AFX
CString TSS_RestrictionQueryName(LPCTSTR UserName);
#endif
void TSS_RestrictionQueryName(LPCTSTR UserName, TCHAR sz[g_nMaxNameLen]);

bool TSS_QuitDuringInstall(VOID);

#ifdef _AFX
CString TSS_GetProductDir();
#endif
void TSS_GetProductDir(TCHAR sz[MAX_PATH]);

#ifdef _AFX
CString TSS_GetSystemFile();
#endif
void TSS_GetSystemFile(TCHAR sz[MAX_PATH]);

#ifdef _AFX
CString TSS_GetDatabaseFile();
CString TSS_GetDatabaseReplacementFile(VOID);
CString TSS_GetEPGDatabaseFile();
CString TSS_GetAMFilter();
void TSS_GetDefaultVBITuning(long &lTS, long &lChan);
BOOL TSS_GetTuningSpaceInfoEx(class CTuningSpaceInfo &tsi);
void TSS_GetLoaderName(const CString &csGuid, CString &csLoaderName);
long TSS_FindSType(ULONG ulTuningSpace, BYTE ubLCID);
void TSS_MkDeviceName(ULONG ulVendorId, ULONG ulDeviceId, CString &Name);
HRESULT TSS_FindProvider(ULONG ulVendorId, ULONG ulDeviceId, CString &csProviderDll, CString &csProviderDesc);
void TSS_GetAudioPin(LPTSTR *szPinName);
void TSS_SetAudioPin(LPCTSTR szPinName);
#endif
BOOL TSS_GetTuningSpaceInfo(const long lTS, CLSID &cid, DWORD &dwInputType);
const DWORD DEFVOL = 5000;
const DWORD DEFBAL = 0;

void TSS_GetAudio(LPCTSTR szDevName, DWORD &Vol, DWORD &Bal);
void TSS_SetAudio(LPCTSTR szDevName, DWORD Vol, DWORD Bal);

DWORD TSS_GetCountryCode(void);
// get tuning space number from active movie filter class id
BOOLEAN TSS_GetAMFilter(TCHAR sz[MAX_PATH]);
void TSS_GetDatabaseFile(TCHAR sz[MAX_PATH]);
BOOL TSS_GetDatabaseReplacementFile(TCHAR sz[MAX_PATH]);
void TSS_GetEPGDatabaseFile(TCHAR sz[MAX_PATH]);
BOOLEAN TSS_GetLoaderName(LPCTSTR szGuid, LPTSTR szLoaderName, DWORD &dwSize);
BOOLEAN TSS_GetTuningIDs(LPCTSTR szGuid, DWORD *dwTuningID, DWORD &dwBytes);
BOOLEAN TSS_GetTuningIDsAnalogCheck(LPCTSTR szGuid, DWORD *dwTuningID, DWORD &dwBytes);
BOOLEAN TSS_SetAMFilter(TCHAR sz[MAX_PATH]);
BOOLEAN TSS_SetLoaderAttempt(LPCTSTR szGuid);
BOOLEAN TSS_SetLoaderCompletion(LPCTSTR szGuid, DWORD dwResult);
// global
BOOLEAN TSS_GetLoaderAttempt(LPTSTR szGuid, DWORD *pcbLen, SYSTEMTIME *pstTime);
BOOLEAN TSS_GetLoaderCompletion(LPTSTR szGuid, DWORD *pcbLen, SYSTEMTIME *pstTime, DWORD *pdwResult);
// specific loader
BOOLEAN TSS_GetLoaderAttempt(LPCTSTR szGuid, SYSTEMTIME *pstTime);
BOOLEAN TSS_GetLoaderCompletion(LPCTSTR szGuid, SYSTEMTIME *pstTime, DWORD *pdwResult);
BOOLEAN TSS_GetDTVLoaderCSSByte(LPCTSTR szGuid, BYTE *pbCSSByte);
BOOLEAN TSS_SetDTVLoaderCSSByte(LPCTSTR szGuid, BYTE *pbCSSByte);
BOOLEAN TSS_GetLoaderReadPrefix(LPCTSTR szGuid, LPTSTR szPrefix, DWORD &dwSize);
BOOLEAN TSS_GetLoaderWritePrefix(LPCTSTR szGuid, LPTSTR szPrefix, DWORD &dwSize);
BOOLEAN TSS_GetLoaderReadFile(LPCTSTR szGuid, LPTSTR szReadFile, DWORD dwSize);
BOOLEAN TSS_SetLoaderReadFile(LPCTSTR szGuid, LPTSTR szReadFile);
BOOLEAN TSS_GetSSLoaderRegionID(LPCTSTR szGuid, DWORD *pdwRegion);
BOOLEAN TSS_SetSSLoaderRegionID(LPCTSTR szGuid, DWORD dwRegion);
BOOLEAN TSS_GetSSLoaderPreviousRegionID(LPCTSTR szGuid, DWORD *pdwPreviousRegion);
BOOLEAN TSS_SetSSLoaderPreviousRegionID(LPCTSTR szGuid, DWORD dwPreviousRegion);
BOOLEAN TSS_GetSSLoaderRegionIDValid(LPCTSTR szGuid, BOOL *pfRegionIDValid);
BOOLEAN TSS_SetSSLoaderRegionIDValid(LPCTSTR szGuid, BOOL fRegionIDValid);
BOOLEAN TSS_GetSSLoaderScanReturnCode(LPCTSTR szGuid, DWORD *pdwScanReturnCode);
BOOLEAN TSS_SetSSLoaderScanReturnCode(LPCTSTR szGuid, DWORD dwScanReturnCode);
BOOLEAN TSS_GetSSLoaderChannel(LPCTSTR szGuid, DWORD *pdwChannel);
BOOLEAN TSS_SetSSLoaderChannel(LPCTSTR szGuid, DWORD dwChannel);
BOOLEAN TSS_GetSSLoaderLine(LPCTSTR szGuid, DWORD *pdwLine);
BOOLEAN TSS_SetSSLoaderLine(LPCTSTR szGuid, DWORD dwLine);
BOOLEAN TSS_GetLoaderPostalCode(LPTSTR szPostalCode, DWORD cb);
BOOLEAN TSS_SetLoaderPostalCode(LPCTSTR szPostalCode);
BOOLEAN TSS_GetSSLoaderListenReturnCode(LPCTSTR szGuid, DWORD *pdwListenReturnCode);
BOOLEAN TSS_SetSSLoaderListenReturnCode(LPCTSTR szGuid, DWORD dwListenReturnCode);
BOOLEAN TSS_GetSSLoaderListenReturnCodeCount(LPCTSTR szGuid, DWORD *pdwListenReturnCodeCount);
BOOLEAN TSS_SetSSLoaderListenReturnCodeCount(LPCTSTR szGuid, DWORD dwListenReturnCodeCount);
BOOLEAN TSS_GetLoaderLastCompactSize(DWORD *pdwLastCompactSize);
BOOLEAN TSS_SetLoaderLastCompactSize(DWORD dwLastCompactSize);

void TSS_ThrowOleDispatchException(SCODE sc, LPCTSTR lpszDescription = NULL, UINT nHelpID = 0, WORD wCode = 0, LPCTSTR szHelpFile = NULL, LPCTSTR szSource = NULL);
void TSS_ThrowOleDispatchException(SCODE sc, UINT nDescriptionID = 0, UINT nHelpID = 0, WORD wCode = 0, LPCTSTR szHelpFile = NULL, LPCTSTR szSource = NULL);
void TSS_ThrowOleDispatchException(LPEXCEPINFO pInfo);

void TSS_BracketizeTableOrQuery(TCHAR sz[g_nMaxNameLen]);

BOOL TSS_IsValidUserName(LPCTSTR szUserName);

#if defined(_AFX) && defined(__AFXDAO_H)
void ThrowDispatchExceptFromDaoExcept(CDaoException *e);
#endif

#if defined(_DEBUG) && defined(_AFX)
void TSS_GetTraceLevel(int iAppNameSID, DWORD &dwTraceLevel);
#endif

#endif // TSSUTIL_H
