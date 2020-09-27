/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: vulcandlisttemplate.h
*
* File Comments:
*
*
***********************************************************************/

#ifndef INLINE
#define UNDEF_INLINE
#define INLINE    inline
#endif

/*******************************************************************************
   FILE    : vulcandlisttemplate.h

   CLASSES : CDList2Elem  (template)
             CDList2Iter  (template)
             CDList2      (template)

................................................................................
   CDList2Elem <T>
   ==============
   Any class can be made "Double-Link" ready simply by inheriting from the
   CDList2Elem template class as follows:

         class CBBlock : public CDList2Elem_CBBlock
         {
            ...
         };

   The type argument to all the templates MUST be a POINTER type to the derived
   class.

   The CDList2 template class implements a list of CDList2Elem nodes, and the
   CDList2Iter class implements navigation and modification methods on the List.

................................................................................
   CDList2Iter <T>
   ==============
   This template class implements the navigation and modification methods
   for the doubly-linked list.

   It can be initialized into one of two states:
      - K-state: List is known, current position known.
      - U-state: List is unknown, current position known.

   The U-state can only occur when the Iterator was initialized using the
   following method WITH the ListPtr argument NULL.

      CDList2Iter(CDList2 *ListPtr, CDList2Elem *ElemPtr, T);

   In the U-state, navigation of Prev() and Next() are still in order (1),
   but the insertion and deletion methods (and others) incur a ONE-TIME cost
   of order (n) to determine the List, and thereby, switching over to the
   K-state. In the K-state, most operations are in order (1).

   YES. You can always determine the List on which an Element is on.

   The time cost for each of the Iterator methods are listed here.

                                                      // K-state    U-state
                                                      //========    =======
   void           AddNext(T);                         //  O(1)        O(n)
   void           AddLast(T);                         //  O(1)        O(n)
   void           AddPrev(T);                         //  O(1)        O(n)
   void           AddFirst(T);                        //  O(1)        O(n)
   T              Curr() const;                       //  O(1)   ==   O(1)
   T              Delete();                           //  O(1)        O(n)
   bool           Done() const;                       //  O(1)   ==   O(1)
   CDList2<T>      *GetList();                         //  O(1)        O(n)
   void           Init(CDList2<T> &);                  //  O(1)   ==   O(1)
   void           Init(CDList2<T> &, T, T);            //  O(1)   ==   O(1)
   void           Init(CDList2<T> *, T, T);            //  O(1)   ==   O(1)
   bool           IsEmpty();                          //  O(1)        O(n)
   bool           IsFirst();                          //  O(1)        O(n)
   bool           IsLast();                           //  O(1)        O(n)
   size_t         Length();                           //  O(n)   ==   O(n)
   void           MoveToFirst();                      //  O(1)        O(n)
   void           MoveToLast();                       //  O(1)        O(n)
   void           Next();                             //  O(1)   ==   O(1)
   void           Prev();                             //  O(1)   ==   O(1)
   void           ReverseList();                      //  O(n)   ==   O(n)

................................................................................
   CDList2 <T>
   ==========
   This is the List. Due to the special property of it also being an Elem and
   on the double-linked chain, it is possible to determine the List from any
   Elem on the List.


   EMPTY, the List looks like this:
   ================================

      CDList2 (inherits from CDList2Elem)
      +--------+
      | m_Next |-----+
      |--------|     V
  +---| m_Prev |
  V   |========|
      | m_Head |-+
      +--------+ V


   NOT EMPTY, the List looks like this:
   ====================================

  +------------------------------------------------+
  |                                                |
  |   CDList2 (inherits from CDList2Elem)            |
  |   +--------+                                   |
  +---| m_Next |                                   |
      |--------|<----+                             V
    +-| m_Prev |     |     (First)              (Last)
    V +========+     |     CDList2Elem         CDList2Elem
      |        |     |     X--------X         X--------X
      | m_Head |-----)---->| m_Next |-------->| m_Next |---+
      +--------+     |     |--------|<----\   |--------|   V
                     +-----| m_Prev |      \--| m_Prev |
                           X--------X         X--------X

   SOON, the list will look like:

        CDList2               (First)                               (Last)
      +--------+           CDList2Elem         CDList2Elem         CDList2Elem
      | m_Head | --------->X--------X         X--------X         X--------X
      +--------+ <---------| m_Prev |<--------| m_Prev |<--------| m_Prev |
                           |--------|   /---->|--------|   /---->|--------|
                           | m_Next |--/      | m_Next |--/      | m_Next |---+
                           X--------X         X--------X         X--------X   V


******************************************************************************/

#pragma warning(disable: 4710)   // code not inlined.
#pragma warning(disable: 4786 )  // truncated identifier


// T is always only a pointer to a class that inherited from CDList2Elem.
//
template <class T> class CDList2;
template <class T> class CDList2Iter;

enum Where;

template <class T>
class CDList2Elem               //name changed to force use of typedef will change back later
{
private:
   VIRTUALPAD;
   CDList2Elem<T>  *m_Next;
   PADDING(1);
   CDList2Elem<T>  *m_Prev;

   friend class CDList2<T>;
   friend class CDList2Iter<T>;

public:
                              CDList2Elem<T>();
               T              First() const;
   INLINE      void           Insert(Where w, T);
               void           InsertPrev(T);
               void           InsertNext(T);
   INLINE      bool           IsInList();
               T              Last() const;
   INLINE      T              Next() const;
   INLINE      T              Prev() const;
               void           Remove();


private:
   INLINE      T              PrevOrHead() const;
   INLINE      void           SetLink(CDList2Elem<T> *, CDList2Elem<T> *);
   INLINE      void           SetNext(CDList2Elem<T> *);
   INLINE      void           SetPrev(CDList2Elem<T> *);
};

template <class T>
class CDList2Iter               //name changed to force use of typedef will change back later
{
   CDList2Elem<T>  *m_Curr;
   CDList2<T>      *m_pList;

   friend class CDList2Elem<T>;

public:
   INLINE                     CDList2Iter<T>();
   INLINE                     CDList2Iter<T>(CDList2<T> &);
   INLINE                     CDList2Iter<T>(CDList2<T> &, T, T);
   INLINE                     CDList2Iter<T>(CDList2<T> *, T, T);

                                                                  // K-state    U-state
                                                                  //========    =======
   INLINE      void           Add(Where w, T t);                  //  O(1)        O(n)
               void           AddNext(T);                         //  O(1)        O(n)
               void           AddLast(T);                         //  O(1)        O(n)
               void           AddPrev(T);                         //  O(1)        O(n)
               void           AddFirst(T);                        //  O(1)        O(n)
   INLINE      T              Curr() const;                       //  O(1)   ==   O(1)
               T              Delete();                           //  O(1)        O(n)
   INLINE      bool           Done() const;                       //  O(1)   ==   O(1)
   INLINE      CDList2<T>     *GetList() const;                   //  O(1)        O(n)
   INLINE      void           Init(CDList2<T> &);                 //  O(1)   ==   O(1)
   INLINE      void           Init(CDList2<T> &, T, T);           //  O(1)   ==   O(1)
   INLINE      void           Init(CDList2<T> *, T, T);           //  O(1)   ==   O(1)
   INLINE      bool           IsEmpty() const;                    //  O(1)        O(n)
   INLINE      bool           IsFirst() const;                    //  O(1)        O(n)
   INLINE      bool           IsLast() const;                     //  O(1)        O(n)
   INLINE      size_t         Length() const;                     //  O(n)   ==   O(n)
   INLINE      void           MoveToFirst();                      //  O(1)        O(n)
   INLINE      void           MoveToLast();                       //  O(1)        O(n)
   INLINE      void           Next();                             //  O(1)   ==   O(1)
               void           Prev();                             //  O(1)   ==   O(1)
               void           ReverseList();                      //  O(n)   ==   O(n)

private:
               void           InsertFirstAndOnly(T);
};


template <class T>
class CDList2 : private CDList2Elem<T>          //name changed to force use of typedef will change back later
{
private:
   friend class CDList2Iter<T>;
   friend class CDList2Elem<T>;
   PADDING(1);
   CDList2Elem<T>  *m_Head;

public:
   INLINE                     CDList2();

   INLINE      void           AddNext(T, CDList2Iter<T> &);
   INLINE      void           AddLast(T);
   INLINE      void           AddPrev(T, CDList2Iter<T> &);
   INLINE      void           AddFirst(T);
   INLINE      void           Delete(CDList2Iter<T> &);
   INLINE      CDList2Iter<T> First() const;
   INLINE      T              FirstElem() const;
   INLINE      void           Init();
   INLINE      bool           IsEmpty() const;
   INLINE      bool           IsFirst(CDList2Iter<T> &) const;
   INLINE      bool           IsLast(CDList2Iter<T> &) const;
               CDList2Iter<T> Last() const;
   INLINE      T              LastElem() const;
               size_t         Length() const;
               void           Reverse();                    // Reverse the list
};


//--------------------------------------------------------------
// CDList2Elem methods
//--------------------------------------------------------------
//
template <class T> T CDList2Elem<T>::Next() const
{
   return (T)m_Next;
}

template <class T> T CDList2Elem<T>::PrevOrHead() const
{
    return (T)m_Prev;
}

template <class T> T CDList2Elem<T>::Prev() const
{
   if (m_Prev &&
       m_Prev->m_Prev)
   {
      return (T)m_Prev;
   }
   else
   {
      return NULL;
   }
}

template <class T> bool CDList2Elem<T>::IsInList()
{
    return (m_Prev != NULL);
}

template <class T> void CDList2Elem<T>::SetLink(CDList2Elem<T> *prev, CDList2Elem<T> *next)
{
   m_Next = next;
   m_Prev = prev;
}

template <class T> void CDList2Elem<T>::SetNext(CDList2Elem<T> *next)
{
   m_Next = next;
}

template <class T> void CDList2Elem<T>::SetPrev(CDList2Elem<T> *prev)
{
   m_Prev = prev;
}

template <class T> CDList2Elem<T>::CDList2Elem<T>()
{
   m_Next = NULL;
   m_Prev = NULL;
}

template <class T> void CDList2Elem<T>::InsertPrev(T t)
{
   if (t && PrevOrHead() && t!=(T)this)
   {
       t->Remove();

       //for now use the iterator
       CDList2Iter<T> iter(NULL, (T)this, NULL);
       iter.AddPrev(t);
   }
}

template <class T> void CDList2Elem<T>::InsertNext(T t)
{
   if (t && PrevOrHead() && t!=(T)this)
   {
      t->Remove();

      //for now use the iterator
      CDList2Iter<T> iter(NULL, (T)this, NULL);
      iter.AddNext(t);
   }
}

template <class T> void CDList2Elem<T>::Insert(Where w, T t)
{
    if (w == PREV)
    {
        InsertPrev(t);
    }
    else
    {
        InsertNext(t);
    }
}

template <class T> T CDList2Elem<T>::First() const
{
    if (NULL == m_Prev)
    {
        return NULL; //I am not in a list
    }

    CDList2Iter<T> iter(NULL, (T)this, NULL);
    iter.MoveToFirst();
    return iter.Curr();
}

template <class T> T CDList2Elem<T>::Last() const
{
    if (NULL == m_Prev)
    {
        return NULL; //I am not in a list
    }

    CDList2Iter<T> iter(NULL, (T)this, NULL);
    iter.MoveToLast();
    return iter.Curr();
}

template <class T> void CDList2Elem<T>::Remove()
{
    if (NULL == m_Prev)
    {
        return; //I am not in a list
    }

   //for now use the iterator
   CDList2Iter<T> iter(NULL, (T)this, NULL);
   iter.Delete();

   //I am not in a list now
   m_Prev = m_Next = NULL;
}



//--------------------------------------------------------------
// CDList2Iter methods
//--------------------------------------------------------------
//
template <class T> CDList2Iter<T>::CDList2Iter<T>()
{
   m_Curr = NULL;
   m_pList = NULL;
}

template <class T> CDList2Iter<T>::CDList2Iter<T>(CDList2<T> &List)
{
   Init(List);
}

template <class T> CDList2Iter<T>::CDList2Iter<T>(CDList2<T> &List, T curr, T turd)
{
   Init(List, curr, turd);
}

template <class T> CDList2Iter<T>::CDList2Iter<T>(CDList2<T> *pList, T curr, T turd)
{
   Init(pList, curr, turd);
}

template <class T> void CDList2Iter<T>::Add(Where w, T t)
{
    if (w == BEFORE)
    {
        AddPrev(t);
    }
    else
    {
        AddNext(t);
    }
}
template <class T> void CDList2Iter<T>::AddNext(T t)
{
   if (Done() &&IsEmpty())
   {
      InsertFirstAndOnly(t);
   }
   else if (Done())
   {
      AddFirst(t);
   }
   else if (!(m_Curr->m_Next) && IsLast())
   {
      AddLast(t);
   }
   else
   {
      CDList2Elem<T> *pe = (CDList2Elem<T> *)t;
      CDList2Elem<T> *Next = m_Curr->Next();
      t->SetLink(m_Curr, Next);
      Next->SetPrev(pe);
      m_Curr->SetNext(pe);
   }
}

template <class T> void CDList2Iter<T>::AddLast(T t)
{
   if (NULL == t)
   {
      return;
   }

   t->Remove();

   if (IsEmpty())
   {
      InsertFirstAndOnly(t);
   }
   else
   {
      CDList2Elem<T> *pe = (CDList2Elem<T> *)t;
      CDList2Elem<T> *Last = m_pList->Next();
      t->SetLink(Last, NULL);
      Last->SetNext(pe);
      m_pList->SetNext(pe);
   }
}

template <class T> void CDList2Iter<T>::AddPrev(T t)
{
   CDList2Elem<T> *pe = (CDList2Elem<T> *)t;
   if (Done() && IsEmpty())
   {
      InsertFirstAndOnly(t);
   }
   else if (Done())
   {
      AddLast(t);
   }
   else if (!(m_Curr->m_Prev->m_Prev) && IsFirst())
   {
      AddFirst(t);
   }
   else
   {
      CDList2Elem<T> *Prev = m_Curr->PrevOrHead();
      t->SetLink(Prev, m_Curr);
      Prev->SetNext(pe);
      m_Curr->SetPrev(pe);
   }
}

template <class T> void CDList2Iter<T>::AddFirst(T t)
{
   if (NULL == t)
   {
      return;
   }

   t->Remove();

   CDList2Elem<T> *pe = (CDList2Elem<T> *)t;
   if (IsEmpty())
   {
      InsertFirstAndOnly(t);
   }
   else
   {
      CDList2Elem<T> *First = m_pList->m_Head;
      t->SetLink(First->PrevOrHead(), First);
      First->SetPrev(pe);
      m_pList->m_Head = pe;

      if (pe->PrevOrHead() != (CDList2Elem<T> *) m_pList)
      {
         //VulcanAssert(__FILE__, __LINE__, "List corrupted", false);
      }
   }
}

template <class T> T CDList2Iter<T>::Curr() const
{
   return (T) m_Curr;
}

template <class T> bool CDList2Iter<T>::Done() const
{
   return (m_Curr == NULL);
}

template <class T> T CDList2Iter<T>::Delete()
{
   if (IsEmpty())
   {
      return NULL;
   }

   if (Done())
   {
      m_Curr = m_pList->m_Head;                    // MoveToFirst()
   }

   if (IsFirst())
   {
      if (IsLast())
      {
         m_pList->m_Head = NULL;                   // DeleteOneAndOnly()
         m_pList->SetNext(NULL);
      }
      else
      {
         m_pList->m_Head = m_Curr->Next();         // Delete First but not only
         m_Curr->Next()->SetPrev(m_Curr->PrevOrHead());  // maintain relation to pList.
         m_pList->m_Head = m_Curr->Next();
      }
   }
   else if (m_Curr->Next() == NULL)                // IsLast()
   {
      m_Curr->PrevOrHead()->SetNext(NULL);               // Delete Last but not only
      m_pList->SetNext(m_Curr->PrevOrHead());
   }
   else
   {
      m_Curr->PrevOrHead()->SetNext(m_Curr->Next());     // Delete middle
      m_Curr->Next()->SetPrev(m_Curr->PrevOrHead());
   }

   T retval = (T) m_Curr;
   m_Curr = m_Curr->Next();
   retval->SetLink(NULL, NULL);
   return  retval;
}

template <class T> CDList2<T> *CDList2Iter<T>::GetList() const
{
   if (!m_pList)
   {
      for (CDList2Elem<T> *temp = m_Curr;m_Curr;((CDList2Iter<T> *)this)->m_Curr = (T) m_Curr->m_Prev)
      {
         ((CDList2Iter<T> *)this)->m_pList = (CDList2<T> *) m_Curr;
      }
      ((CDList2Iter<T> *)this)->m_Curr = temp;
   }

   return m_pList;

}

template <class T> void CDList2Iter<T>::Init(CDList2<T> &List)
{
   m_pList = &List;
   MoveToFirst();
}

template <class T> void CDList2Iter<T>::Init(CDList2<T> &List, T curr, T)
{
   m_pList = &List;
   m_Curr = (CDList2Elem<T> *) curr;
}

template <class T> void CDList2Iter<T>::Init(CDList2<T> *pList, T curr, T)
{
   if (!pList && !curr)
   {
      throw new VErr("Internal Error", VErr::FLG_FATAL);      // can't have both be NULL!
   }

   m_pList = pList;
   m_Curr = (CDList2Elem<T> *) curr;
}

template <class T> bool CDList2Iter<T>::IsEmpty() const
{
   return GetList()->IsEmpty();
}

template <class T> bool CDList2Iter<T>::IsFirst() const
{
   return (m_Curr == GetList()->m_Head);
}

template <class T> bool CDList2Iter<T>::IsLast() const
{
   return (m_Curr == GetList()->Next());
}

template <class T> size_t CDList2Iter<T>::Length() const
{
   return GetList()->Length();
}

template <class T> void CDList2Iter<T>::MoveToFirst()
{
   m_Curr = (IsEmpty()) ? NULL : GetList()->m_Head;
}

template <class T> void CDList2Iter<T>::MoveToLast()
{
   m_Curr = GetList()->Next();
}

template <class T> void CDList2Iter<T>::Next()
{
   m_Curr = m_Curr->Next();
}

template <class T> void CDList2Iter<T>::Prev()
{
   m_Curr = m_Curr->PrevOrHead();

   if (m_Curr->PrevOrHead() == NULL)
   {
      // currpos is extreme left
      //
      if (m_pList && (m_Curr != (CDList2Elem<T> *)m_pList))
      {
         // Was this initialized with the wrong m_pList ??
         //
          throw new VErr("Internal Error", VErr::FLG_FATAL);
      }

      m_pList = (CDList2<T> *) m_Curr;      // initialize List Ptr.
      m_Curr = NULL;
   }
}

template<class T> void CDList2Iter<T>::ReverseList()
{
   if (IsEmpty())
   {
      return;
   }

   CDList2Elem<T> *currsave = m_Curr;               // for restoration later.
   CDList2Elem<T> *First = m_pList->m_Head;         // for restoration later.
   CDList2Elem<T> *Last = m_pList->Next();          // for restoration later.
   m_pList->m_Head->SetPrev(NULL);                 // temporarily remove link back to m_pList.

   for (CDList2Elem<T> *curr = m_pList->m_Head ; curr ; )
   {
      CDList2Elem<T> *temp = curr->Next();
      curr->SetNext(curr->PrevOrHead());
      curr->SetPrev(temp);
      curr = temp;
   }

   m_pList->m_Head = Last;
   Last->SetPrev((CDList2Elem<T> *) m_pList);       // restore link back to m_pList
   m_pList->SetNext(First);

   m_Curr = currsave;                              // restore currpos.
}


// CDList2Iter private methods.
//
template <class T> void CDList2Iter<T>::InsertFirstAndOnly(T t)
{
#if DEBUG
   if ((m_pList == NULL) ||
       (m_pList->m_Head != NULL) ||
       (m_pList->Next() != NULL) ||
       (m_pList->PrevOrHead() != NULL))
   {
      //VulcanAssert(__FILE__, __LINE__, "List corrupted", false);
   }
#endif

   CDList2Elem<T> *pe = (CDList2Elem<T> *) t;
   t->SetLink((CDList2Elem<T> *) m_pList, NULL);
   m_pList->m_Head = pe;
   m_pList->SetNext(pe);
}


//--------------------------------------------------------------
// CDList2 methods
//--------------------------------------------------------------
//
template <class T> CDList2<T>::CDList2<T>()
{
   SetLink(NULL, NULL);
   m_Head = NULL;
}

template <class T> void CDList2<T>::AddNext(T t, CDList2Iter<T>& iter)
{
   iter.AddNext(t);
}

template <class T> void CDList2<T>::AddLast(T t)
{
   CDList2Iter<T> iter(*this);
   iter.AddLast(t);
}

template <class T> void CDList2<T>::AddPrev(T t, CDList2Iter<T>& iter)
{
   iter.AddPrev(t);
}

template <class T> void CDList2<T>::AddFirst(T t)
{
   CDList2Iter<T> iter(*this);
   iter.AddFirst(t);
}

template <class T> void CDList2<T>::Delete(CDList2Iter<T> &iter)
{
   iter.Delete();
}

template <class T> CDList2Iter<T> CDList2<T>::First() const
{
   CDList2Iter<T> retval(*(CDList2<T> *)this);
   return retval;
}

template <class T> T CDList2<T>::FirstElem() const
{
   return (T) m_Head;
}

template <class T> void CDList2<T>::Init()
{
   SetLink(NULL, NULL);
   m_Head = NULL;
}

template <class T> bool CDList2<T>::IsEmpty() const
{
   return m_Head == NULL;
}

template <class T> bool CDList2<T>::IsFirst(CDList2Iter<T> &iter) const
{
   return iter.IsFirst();
}

template <class T> bool CDList2<T>::IsLast(CDList2Iter<T> &iter) const
{
   return iter.IsLast();
}

template <class T> CDList2Iter<T> CDList2<T>::Last() const
{
   CDList2Iter<T> retval(*(CDList2<T> *)this);
   retval.MoveToLast();
   return retval;
}

template <class T> T CDList2<T>::LastElem() const
{
   return (T) Next();
}

template <class T> size_t CDList2<T>::Length() const
{
   size_t cnt = 0;
   for(CDList2Iter<T> iter(*(CDList2<T> *)this) ; !iter.Done() ; iter.Next())
   {
      cnt++;
   }

   return cnt;
}

template <class T> void CDList2<T>::Reverse()                    // Reverse the list
{
   CDList2Iter<T> iter(*this);
   iter.ReverseList();
}

#ifdef UNDEF_INLINE
#undef UNDEF_INLINE
#undef INLINE
#endif   // UNDEF_INLINE


