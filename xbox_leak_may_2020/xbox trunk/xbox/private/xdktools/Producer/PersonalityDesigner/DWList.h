//
// DWList.h
//
#ifndef __DWList_H__
#define __DWList_H__

//#pragma warning(disable: 4505)

class DWListItem
{
public:
    DWListItem() { m_pNext=NULL; };
    DWListItem *GetNext() const {return m_pNext;};
    void SetNext(DWListItem *pNext) {m_pNext=pNext;};
    LONG GetCount() const;
    DWListItem* Cat(DWListItem* pItem);
    DWListItem* AddTail(DWListItem* pItem) {return Cat(pItem);};
    DWListItem* Remove(DWListItem* pItem);
    DWListItem* GetPrev(DWListItem *pItem) const;
    DWListItem* GetItem(LONG index);

private:
    DWListItem *m_pNext;
};

class DWList
{
public:
    DWList() {m_pHead=NULL;};
    DWListItem *GetHead() const { return m_pHead;};

    void RemoveAll() { m_pHead=NULL;};
    LONG GetCount() const {return m_pHead->GetCount();}; 
    DWListItem *GetItem(LONG index) { return m_pHead->GetItem(index);}; 
    void InsertBefore(DWListItem *pItem,DWListItem *pInsert);
    void Cat(DWListItem *pItem) {m_pHead=m_pHead->Cat(pItem);};
    void Cat(DWList *pList)
        {
//            assert(pList!=NULL);
            m_pHead=m_pHead->Cat(pList->GetHead());
        };
    void AddHead(DWListItem *pItem)
        {
//            assert(pItem!=NULL);
            pItem->SetNext(m_pHead);
            m_pHead=pItem;
        }
    void AddTail(DWListItem *pItem) {m_pHead=m_pHead->AddTail(pItem);};
    void Remove(DWListItem *pItem) {m_pHead=m_pHead->Remove(pItem);};
    DWListItem *GetPrev(DWListItem *pItem) const {return m_pHead->GetPrev(pItem);};
    DWListItem *GetTail() const {return GetPrev(NULL);};
    BOOL IsEmpty(void) const {return (m_pHead==NULL);};
    DWListItem *RemoveHead(void)
        {
            DWListItem *li;
            li=m_pHead;
            if(m_pHead)
                m_pHead=m_pHead->GetNext();
            return li;
        }

protected:
    DWListItem *m_pHead;
};

class IndexList : public DWList 
{
public:
				IndexList() { 
					DWList();
					m_index = 0; 
				};
	DWListItem	*GetIndexedItem() { return(GetItem(m_index)); }
	void		SetIndex(LONG index)
				{
					m_index = index;
					if (GetCount() <= (LONG) index) m_index = 0;
				}
	DWListItem	*RemoveIndexedItem()
				{
					DWListItem *item = GetIndexedItem();
					if (item) Remove(item);
					return (item);
				}
	void		AddIndexedItem(DWListItem *item)
				{
					AddHead(item);
					m_index = 0;
				}

	LONG		m_index;
};

#endif // __DWList_H__
