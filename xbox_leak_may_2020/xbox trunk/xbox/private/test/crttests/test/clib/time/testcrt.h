
struct DTMINFO {
	int nYear;
	int nMonth;
	int nDay;
	int nDayOfWeek;
	int nHour;
	int nMinute;
	int nSecond;
};

DTMINFO tDates[];

struct LangName {
	DWORD	wLANGID;
	DWORD	wSUBLANGID;
	_TCHAR* pszLangName;
};

LangName LangNameTable[];

BOOL compare(_TCHAR *sText, int x, int y);

void PrintPassFail(BOOL bFailed);