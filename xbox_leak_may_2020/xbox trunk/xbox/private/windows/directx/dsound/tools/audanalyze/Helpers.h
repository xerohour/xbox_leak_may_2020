/********************************************************************************
HELPERS.H
********************************************************************************/

VOID DXUtil_ConvertAnsiStringToWide( WCHAR* wstrDestination, const CHAR* strSource, 
                                     int cchDestChar );
VOID DXUtil_ConvertGenericStringToWide( WCHAR* wstrDestination, const TCHAR* tstrSource, 
                                        int cchDestChar );

HRESULT ChopPath(const char *p_szFullString, LPSTR p_szPathOut, LPSTR p_szFileOut);
LPCSTR PathFindFileName(LPCSTR pPath);


BOOL YesOrNo(void);

HRESULT PrintUsage(void);


struct CLSID_Map
{
	const CLSID *clsid;
	TCHAR *tcstr;
};


class CClassTracker 
{
public:
    CClassTracker() { m_pNext = NULL; m_dwCount = 1; m_fSaved = FALSE; }
    CClassTracker *     m_pNext;
    DWORD               m_dwCount;
    const char *        m_pszClassName;
    const char *        m_pszFunctionName;
    const char *        m_pszComment;
    BOOL                m_fSaved;
};



struct CLSIDElement
{
const GUID*  pclsid;
LPCSTR pszClassName;
LPCSTR pszFunctionName;
LPCSTR pszComment;
};

extern CClassTracker *g_pClassList;
extern CLSIDElement CLSIDTable[];

void InsertClass(const char *pszClassName, const char *pszFunctionName, const char *pszComment);
void RemoveClass(const char *pszClassName);
BOOL CLSIDPresent( LPCSTR pszClassName);


