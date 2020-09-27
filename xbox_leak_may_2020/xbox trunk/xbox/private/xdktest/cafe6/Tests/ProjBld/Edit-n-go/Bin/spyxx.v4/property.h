//
// property.h
//
// Include file with worker routines used by the property pages.
//

//
// The Handles Table  structure.  This is used to build tables of handles
// for displaying in the property pages.
//
typedef struct
{
	HANDLE handle;
	LPSTR idObject;
	LPSTR pszName;
} HANDLESTABLE;

LPSTR FormatHex(DWORD dwValue);
LPSTR FormatHandle(HANDLE handle);
LPSTR FormatInt(INT iValue);
LPSTR FormatULong(ULONG ulValue);
LPSTR FormatKb(DWORD dwValue);
LPSTR FormatKb(LARGE_INTEGER liValue);
LPSTR FormatTime(LARGE_INTEGER liTime);
LPSTR FormatTime(FILETIME ftTime);
LPSTR FormatTimeDiff(LARGE_INTEGER liTimeBegin, LARGE_INTEGER liTimeEnd);
LPSTR FormatTimeDiff(SYSTEMTIME stTime, FILETIME ftTime);
LPSTR FormatTimeAdd(FILETIME ftTime1, FILETIME ftTime2);
LPSTR FormatWord(WORD w);
LPSTR FormatHandleFromTable(HANDLE handle, HANDLESTABLE *pht);
LPSTR FormatULongFromTable(ULONG ulValue, VALUETABLE *pvt);
void FillLBFromStrList(CListBox* pListBox, CStringList* pStrList);
void FillCBFromStrList(CComboBox* pComboBox, CStringList* pStrList);
MSGTYPE GetMsgType(UINT msg);
MSGTYPE GetMsgType(UINT msg, HWND hwnd);
