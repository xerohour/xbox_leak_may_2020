/*************************************************************
Module name: PrfTxtDB.H
*************************************************************/

// Performance Counter Title and Help Text Database.
class CPRFTXTDB
{
	private:
	static LPWSTR	sm_mszCounterText;
	static LPWSTR	sm_mszHelpText;
	static int		sm_nLastCounter, sm_nLastHelp;
	static long		sm_nInstancesOfClass;
	static HANDLE	sm_hEventDataLoaded;

//////////////////////////////////////////////////////////////

	int NumStrings (LPCTSTR mszStrings) const;
	LPCWSTR GetStringNum (LPCTSTR mszStrings, int n) const;

//////////////////////////////////////////////////////////////
	public:

	~CPRFTXTDB(void);
	CPRFTXTDB(int nPreferredLanguageID = LANG_ENGLISH);

	int GetNumOfCounters(void) const;
	int CounterNumFromIndex(int nIndex) const;
	int CounterIndexFromNum(int nNum) const;
	LPCWSTR CounterTextFromIndex(int nIndex) const;
	LPCWSTR CounterTextFromNum(int nNum) const;

	int GetNumOfHelpTexts(void) const;
	int HelpNumFromIndex(int nIndex) const;
	int HelpIndexFromNum(int nNum) const;
	LPCWSTR HelpTextFromIndex(int nIndex) const;
	LPCWSTR HelpTextFromNum(int nNum) const;
};

//////////////////////// End Of File /////////////////////////
