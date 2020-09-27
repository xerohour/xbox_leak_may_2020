//
// DWList.cpp
//

#include "stdafx.h"  

#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include "DWList.h"

LONG DWListItem::GetCount(void) const
{
    LONG l;
    const DWListItem *li;

    for(l=0,li=this; li!=NULL ; li=li->m_pNext,++l);
    return l;
}

DWListItem* DWListItem::Cat(DWListItem *pItem)
{
    DWListItem *li;

    if(this==NULL)
        return pItem;
    for(li=this ; li->m_pNext!=NULL ; li=li->m_pNext);
    li->m_pNext=pItem;
    return this;
}

DWListItem* DWListItem::Remove(DWListItem *pItem)
{
    DWListItem *li,*prev;

    if(pItem==this)
        return m_pNext;
    prev=NULL;
    for(li=this; li!=NULL && li!=pItem ; li=li->m_pNext)
        prev=li;
    if(li==NULL)     // item not found in list
        return this;

//  here it is guaranteed that prev is non-NULL since we checked for
//  that condition at the very beginning

    prev->SetNext(li->m_pNext);
    li->SetNext(NULL);
    return this;
}

DWListItem* DWListItem::GetPrev(DWListItem *pItem) const
{
    const DWListItem *li,*prev;

    prev=NULL;
    for(li=this ; li!=NULL && li!=pItem ; li=li->m_pNext)
        prev=li;
    return (DWListItem*)prev;
}

DWListItem * DWListItem::GetItem(LONG index)

{
	DWListItem *scan;
	for (scan = this; scan!=NULL && index; scan = scan->m_pNext) index--;
	return (scan);
}

void DWList::InsertBefore(DWListItem *pItem,DWListItem *pInsert)

{
	DWListItem *prev = GetPrev(pItem);
	pInsert->SetNext(pItem);
	if (prev) prev->SetNext(pInsert);
	else m_pHead = pInsert;
}

