#define _MAX_SYMBOL	64

void PASCAL DDV_ClassName(CDataExchange* pDX, CString const& s);
void PASCAL DDV_Symbol(CDataExchange* pDX, CString const& s);
void PASCAL DDV_SymbolCanBeginWithNumber(CDataExchange* pDX, CString const& s);
void PASCAL DDV_FileName(CDataExchange* pDX, CString& s,
			const char* pszExtension);
	// it will append suffix if none there
void PASCAL DDV_ProgID(CDataExchange* pDX, CString const& s);
void PASCAL DDV_ExactChars(CDataExchange* pDX, CString const& s, int nLen);
void PASCAL DDV_Title(CDataExchange* pDX, CString const& s);

BOOL IsValidSymbol(const char* psz, BOOL bCanBeginWithNumber, BOOL bCanHaveAngleBrackets=FALSE);
BOOL IsValidFileName(const char* psz, BOOL bAllowDot);

CString Upper(const char* psz);
CString Lower(const char* psz);
CString TcsLeft(const TCHAR* psz, int nMaxTChars);
