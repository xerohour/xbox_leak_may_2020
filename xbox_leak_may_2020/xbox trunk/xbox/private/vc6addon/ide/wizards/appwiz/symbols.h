/////////////////////////////////////////////////////////////////////////////
// Project Type / Language options

void SetProjectTypeSymbol();
void GetProjectTypeSymbol();
void SetLanguageSymbols();
void GetLanguageSymbols();

/////////////////////////////////////////////////////////////////////////////
// Database options

void SetDBSymbols();
void GetDBSymbols();

/////////////////////////////////////////////////////////////////////////////
// OLE options

void ResetOleSymbols();
void SetOleSymbols();
void GetOleSymbols();

/////////////////////////////////////////////////////////////////////////////
// Application options

void SetAppOptionsSymbols();
void GetAppOptionsSymbols();
void SetSizeMRUSymbols(UINT nSizeMRU);
void GetSizeMRUSymbols();
void SetDocumentSymbols();
void GetDocumentSymbols();
void SetFrameSymbols();
void GetFrameSymbols();
void SetSplitterSymbols();
void GetSplitterSymbols();
void SetChildFrameSymbols();
void GetChildFrameSymbols();
void GetMenuSymbols();
void SetMenuSymbols();

/////////////////////////////////////////////////////////////////////////////
// Project options

void SetProjOptionsSymbols();
void GetProjOptionsSymbols();
void SetDllSymbol();
void GetDllSymbol();

/////////////////////////////////////////////////////////////////////////////
// Classes dialog names

void SetClassesSymbols();
void GetClassesSymbols();
void SetBaseViewSymbols();

/////////////////////////////////////////////////////////////////////////////
// Set miscellaneous symbols

void SetMFCPathSymbol();
void SetYearSymbol();
void RemoveInapplicableSymbols();


/////////////////////////////////////////////////////////////////////////////
// Dictionary lookup functions

void GetSymbol(const TCHAR* tszKey, CString& rStrValue);
void SetSymbol(const TCHAR* tszKey, const TCHAR* tszValue);
CString& CreateSymbol(const TCHAR* tszKey);
BOOL DoesSymbolExist(const TCHAR* tszKey);
void RemoveSymbol(const TCHAR* tszKey);
