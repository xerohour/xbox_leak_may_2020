
void DoGetEthernetLinkStatus(HANDLE hLog);
void DoDNSLookup(HANDLE hLog);
void DoStartup(HANDLE hLog);


WINBASEAPI
BOOL
WINAPI
WriteProfileIntA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN int nValue
    );

WINBASEAPI
BOOL
WINAPI
WriteProfileIntW(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN int nValue
    );

#ifdef UNICODE
#define WriteProfileInt  WriteProfileIntW
#else
#define WriteProfileInt  WriteProfileIntA
#endif // !UNICODE
