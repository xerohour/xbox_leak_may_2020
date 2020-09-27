/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ntlist.h
 *  Content:    Overrides of the standard NT list management macros.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/06/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __NTLIST_H__
#define __NTLIST_H__

#define IsEntryInList(Entry) \
    (!IsListEmpty(Entry))

//
// Determine if the entry is valid
//

BEGIN_DEFINE_ENUM()
    ASSERT_VALID_ONLY = 0,
    ASSERT_IN_LIST,
    ASSERT_NOT_IN_LIST,
END_DEFINE_ENUM(ASSERT_VALID_ENTRY_LIST);

#ifdef DEBUG

__inline void __AssertValidEntryList(PLIST_ENTRY Entry, ASSERT_VALID_ENTRY_LIST nFlags, LPCSTR pszFile, UINT nLine)
{
    if(!Entry)
    {
        DwDbgAssert(TRUE, "NULL list entry pointer", pszFile, nLine);
    }
    else if(!Entry->Flink || !Entry->Blink)
    {
        DwDbgAssert(TRUE, "List entry contains NULL pointer", pszFile, nLine);
    }
    else if((Entry == Entry->Flink) || (Entry == Entry->Blink))
    {
        if((Entry != Entry->Flink) || (Entry != Entry->Blink))
        {
            DwDbgAssert(TRUE, "List entry partially points to itself", pszFile, nLine);
        }
    }
    else if(ASSERT_IN_LIST == nFlags)
    {
        if(!IsEntryInList(Entry))
        {
            DwDbgAssert(TRUE, "List entry not in list", pszFile, nLine);
        }
    }
    else if(ASSERT_NOT_IN_LIST == nFlags)
    {
        if(IsEntryInList(Entry))
        {
            DwDbgAssert(TRUE, "List entry in list", pszFile, nLine);
        }
    }
}

#define AssertValidEntryList(Entry, nFlags) \
    __AssertValidEntryList(Entry, nFlags, __FILE__, __LINE__)

#else // DEBUG

#define AssertValidEntryList(Entry, nFlags)

#endif // DEBUG

//
// Add an entry to the end of a list or add a node at the position before
// the given node
//

/*
#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }
*/

#undef InsertTailList
__inline void InsertTailList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
    AssertValidEntryList(ListHead, ASSERT_VALID_ONLY);
    AssertValidEntryList(Entry, ASSERT_NOT_IN_LIST);

    Entry->Flink = ListHead;
    Entry->Blink = ListHead->Blink;
    
    Entry->Blink->Flink = Entry;
    Entry->Flink->Blink = Entry;

    AssertValidEntryList(Entry, ASSERT_IN_LIST);
}    

//
// Add an entry to the end of a list or add a node at the position before
// the given node (no valid node check)
//

__inline void InsertTailListUninit(PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
    AssertValidEntryList(ListHead, ASSERT_VALID_ONLY);

    Entry->Flink = ListHead;
    Entry->Blink = ListHead->Blink;
    
    Entry->Flink->Blink = Entry;
    Entry->Blink->Flink = Entry;

    AssertValidEntryList(Entry, ASSERT_IN_LIST);
}    

//
// Add an entry to the head of a list or add a node at the position after
// the given node
//

/*
#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }
*/

#undef InsertHeadList
__inline void InsertHeadList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
    AssertValidEntryList(ListHead, ASSERT_VALID_ONLY);
    AssertValidEntryList(Entry, ASSERT_NOT_IN_LIST);

    Entry->Flink = ListHead->Flink;
    Entry->Blink = ListHead;
    
    Entry->Flink->Blink = Entry;
    Entry->Blink->Flink = Entry;

    AssertValidEntryList(Entry, ASSERT_IN_LIST);
}

//
// Add an entry to the head of a list or add a node at the position after
// the given node (no valid node check)
//

__inline void InsertHeadListUninit(PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
    AssertValidEntryList(ListHead, ASSERT_VALID_ONLY);

    Entry->Flink = ListHead->Flink;
    Entry->Blink = ListHead;
    
    Entry->Flink->Blink = Entry;
    Entry->Blink->Flink = Entry;

    AssertValidEntryList(Entry, ASSERT_IN_LIST);
}

//
// Remove an entry from a list
//

/*
#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }
*/

#undef RemoveEntryList
__inline PLIST_ENTRY RemoveEntryList(PLIST_ENTRY Entry)
{
    AssertValidEntryList(Entry, ASSERT_VALID_ONLY);

    if(IsEntryInList(Entry))
    {
        Entry->Flink->Blink = Entry->Blink;
        Entry->Blink->Flink = Entry->Flink;

        Entry->Flink = Entry->Blink = Entry;
    }

    return Entry;
}

/*
#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}
*/

#undef RemoveHeadList
#define RemoveHeadList(ListHead) \
    RemoveEntryList((ListHead)->Flink)

/*
#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}
*/

#undef RemoveTailList
#define RemoveTailList(ListHead) \
    RemoveEntryList((ListHead)->Blink)

//
// Move an entry from one list to the tail of another
//

__inline PLIST_ENTRY MoveEntryTailList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
    AssertValidEntryList(ListHead, ASSERT_VALID_ONLY);
    AssertValidEntryList(Entry, ASSERT_VALID_ONLY);

    Entry->Flink->Blink = Entry->Blink;
    Entry->Blink->Flink = Entry->Flink;

    Entry->Flink = ListHead;
    Entry->Blink = ListHead->Blink;

    Entry->Flink->Blink = Entry;
    Entry->Blink->Flink = Entry;

    AssertValidEntryList(Entry, ASSERT_IN_LIST);

    return Entry;
}

__inline PLIST_ENTRY MoveHeadTailList(PLIST_ENTRY DestListHead, PLIST_ENTRY SourceListHead)
{
    PLIST_ENTRY             Entry;
    
    AssertValidEntryList(SourceListHead, ASSERT_VALID_ONLY);

    if((Entry = SourceListHead->Flink) != SourceListHead)
    {
        MoveEntryTailList(DestListHead, Entry);
    }
    
    return Entry;
}

#endif // __NTLIST_H__
