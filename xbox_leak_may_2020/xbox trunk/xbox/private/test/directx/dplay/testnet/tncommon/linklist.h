#ifndef __LINKED_LIST__
#define __LINKED_LIST__





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
typedef class LLITEM	LLITEM,		* PLLITEM;
typedef class LLALIAS	LLALIAS,	* PLLALIAS;
typedef class LLIST		LLIST,		* PLLIST;






//==================================================================================
// The data class.
// These are the actual data elements.  You'll want to derive this class and add
// your own data that each element stores.
//==================================================================================
class DLLEXPORT LLITEM
{
	public:
		DWORD		m_dwRefCount; // how many lists and aliases refer to this item
		PLLITEM		m_pPrev; // Previous item in linked list
		PLLITEM		m_pNext; // Next item in linked list
		BOOL		m_fAlias; // is this item an alias instead of the actual item
#ifdef DEBUG
		BOOL		m_fDeleted; // set to false in constructor, true in destructor
#endif //DEBUG


		DLLSAFE_NEWS_AND_DELETES;

		LLITEM(void);
		virtual ~LLITEM(void);
};





//==================================================================================
// Pointers to data elements.
// Used to add another reference to a single item in the same or a different list
// without having to make a duplicate object.
//==================================================================================
class DLLEXPORT LLALIAS:public LLITEM
{
	public:
		PLLITEM		m_pTarget; // Object this alias stands for.


		LLALIAS(void);
		virtual ~LLALIAS(void);
};






//==================================================================================
// The list class.
// This keeps track of the individual data elements.
//==================================================================================
class DLLEXPORT LLIST:public LLITEM
{
	private:
		PLLITEM				m_pFirst; // First item in linked list
		PLLITEM				m_pLast; // Last item in linked list
		int					m_iCount; // Number of items in list
		CRITICAL_SECTION	m_cs; // Lists's critical section


	public:
#ifdef DEBUG
		BOOL				m_fDeleted; // set to FALSE in constructor, TRUE in destructor
#endif //DEBUG


		DLLSAFE_NEWS_AND_DELETES;

		LLIST(void);
		virtual ~LLIST(void);
		

		virtual void EnterCritSection(void);
		virtual void LeaveCritSection(void);

		HRESULT Add(PLLITEM pNewItem);
		HRESULT AddAfter(PLLITEM pNewItem, PLLITEM pAfterItem);
		HRESULT InsertBeforeIndex(PLLITEM pNewItem, int iPos);

		PLLITEM GetItem(int iPos);
		int GetFirstIndex(PLLITEM pItem);
		PLLITEM GetNextItem(PLLITEM pPreviousItem);
		PLLITEM GetPrevItem(PLLITEM pNextItem);

		HRESULT RemoveFirstReference(PLLITEM pItem);
		HRESULT Remove(int iPos);
		HRESULT RemoveAll(void);

		PLLITEM PopFirstItem(void);

		int Count(void);

		HRESULT MoveAfter(int iPos, int iAfterPos);
		HRESULT SwapPosition(int iPos1, int iPos2);
};






#ifdef DEBUG
#ifdef TRACKPOINTERS

//==================================================================================
// External structure definitions
//==================================================================================
typedef struct tagPOINTERTRACKING
{
	PVOID*		aPointers;
	DWORD		dwCurrentNumItems;
	DWORD		dwMaxNumItems;
} POINTERTRACKING, * PPOINTERTRACKING;


//==================================================================================
// External debugging globals
//==================================================================================
extern POINTERTRACKING		g_LLItems;
extern POINTERTRACKING		g_LLists;

#endif // TRACKPOINTERS
#endif // DEBUG



#endif //__LINKED_LIST__
