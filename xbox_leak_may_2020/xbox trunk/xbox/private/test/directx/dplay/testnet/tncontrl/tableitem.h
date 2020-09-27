#ifndef __TNCONTROL_TESTTABLEITEM__
#define __TNCONTROL_TESTTABLEITEM__
//#pragma message("Defining __TNCONTROL_TESTTABLEITEM__")






//==================================================================================
// Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG



// Whether the built-in scripts should be printed.
#define TNTTPO_BUILTIN				0x01

// Whether the groups should be printed.
#define TNTTPO_GROUPS				0x02

// Whether case descriptions should be printed.
#define TNTTPO_CASEDESCRIPTIONS		0x04


// Whether the number of successful runs should be printed.
#define TNTTPO_SUCCESSES			0x10

// Whether the number of failed runs should be printed.
#define TNTTPO_FAILURES				0x20

// Whether the total times run should be printed.
#define TNTTPO_TOTAL				0x40

// Whether the number of warnings should be printed.
#define TNTTPO_WARNINGS				0x80





//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNTestTableItem:public LLITEM
{
	public:
		char*		m_pszID; // pointer to this item's constant string ID
		char*		m_pszName; // pointer to this item's constant string name
		BOOL		m_fGroup; // is this a group (as opposed to an item)?


		CTNTestTableItem(void);
		virtual ~CTNTestTableItem(void);


		// These are pure virtual functions, derived classes must implement them
		// because we sure don't.
		virtual HRESULT CalculateChecksum(LPCHECKSUM lpChecksum) = 0;
#ifndef _XBOX // no file printing
		virtual HRESULT PrintToFile(HANDLE hFile, DWORD dwOptions) = 0;
#endif // ! XBOX
};






#else //__TNCONTROL_TESTTABLEITEM__
//#pragma message("__TNCONTROL_TESTTABLEITEM__ already included!")
#endif //__TNCONTROL_TESTTABLEITEM__
