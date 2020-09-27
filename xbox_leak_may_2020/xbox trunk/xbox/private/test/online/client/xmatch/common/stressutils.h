//==================================================================================
// xmstress.h: header for stress utilities
//==================================================================================

#if !defined(STRESSUTILS_H)
#define STRESSUTILS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <xtl.h>

typedef struct _XMSTRESS_ITEM
{
	LPVOID					pItem;
	struct _XMSTRESS_ITEM	*pNext;
} XMSTRESS_ITEM, *PXMSTRESS_ITEM;

//==================================================================================
// CXMStressList
//==================================================================================

template <class T> class CXMStressList 
{
public:
    CXMStressList();
    ~CXMStressList();
	T* GetFirstItem();
	T* GetNextItem();
	DWORD GetItemCount() { return m_dwItemCount; }
	BOOL AddItem(T* pItem);
	BOOL RemoveItem(T* pItem);
private:
	DWORD m_dwItemCount;
	PXMSTRESS_ITEM* m_pFirstItem;
	PXMSTRESS_ITEM* m_pCurrentItem;
};

#endif