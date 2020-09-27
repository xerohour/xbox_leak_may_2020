#ifndef __LINKED_STRINGS__
#define __LINKED_STRINGS__





//==================================================================================
// Defines
//==================================================================================
#ifdef LOCAL_TNCOMMON

	// define LOCAL_TNCOMMON when including this code directly into your project
	#ifdef DLLEXPORT
		#undef DLLEXPORT
	#endif // DLLEXPORT defined
	#define DLLEXPORT

#else // ! LOCAL_TNCOMMON

	#ifdef TNCOMMON_EXPORTS

		// define TNCOMMON_EXPORTS only when building the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllexport)

	#else // ! TNCOMMON_EXPORTS

		// default behavior is to import the functions from the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllimport)

	#endif // ! TNCOMMON_EXPORTS
#endif // ! LOCAL_TNCOMMON

#ifndef DEBUG
	#ifdef _DEBUG
		#define DEBUG
	#endif // _DEBUG
#endif // DEBUG not defined




//===========================================================================
// Class typedefs
//===========================================================================
typedef class CLString		CLString,		* PLSTRING;
typedef class CLStringList	CLStringList,	* PLSTRINGLIST;




//===========================================================================
// Classes
//===========================================================================
class DLLEXPORT CLString:public LLITEM
{
	private:
		char*	m_pszString; // the string


	public:
		// Overloaded
		CLString(void);
		CLString(char* pszString);

		virtual ~CLString(void);


		HRESULT SetString(char* pszString);
		char* GetString(void);
		HRESULT ConcatenateString(char* pszString);
		BOOL IsKey(char* szKeyName);
		BOOL IsValueTrue(void);
		char* GetKeyValue(void);
};



class DLLEXPORT CLStringList:public LLIST
{
	public:
		/*
		CLStringList(void);
		virtual ~CLStringList(void);
		*/
		
		HRESULT AddString(char* pszString);
		char* GetIndexedString(int iPos);
		int GetStringIndex(char* szString, int iStartPos, BOOL fMatchCase);
		PLSTRING GetStringObject(char* szString, int iStartPos, BOOL fMatchCase);
		char* GetValueOfKey(char* szKeyName);
		BOOL IsKeyTrue(char* szKeyName);
		BOOL ContainsMatchingWildcardForString(char* szString, BOOL fMatchCase);
};




#endif //__LINKED_STRINGS__
