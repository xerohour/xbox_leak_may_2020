/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: vulcankdlist.h
*
* File Comments:
*
*
***********************************************************************/

#pragma once

//===============================================================
//  These two classes provide link fields and the basic list operations
//  Here is how it is intended to be used
//  Define the class that you want the linked fields in as
//
//  class mylist : public CKDListElem<mylist>
//  {
//  ....
//  };
//
//  Use DList to instantiate the list
//  DList<mylist*> dl;
//  Use DList for insertion/deletion/.. operations
//  Use navigation (Next/Prev) on CKDListIter<mylist>
//================================================================

template <class T>
class CKDList;

template <class T>
class CKDListIter;


//----------------------------------------------------------------
// CKDListElem
//----------------------------------------------------------------

template <class T>
class CKDListElem {
public:
    friend class CKDList<T>;
    friend class CKDListIter<T>;

    void* m_xor;
};


//----------------------------------------------------------------
// CKDListIter
//
// NOTE: T inherits CKDListElem<T>
//----------------------------------------------------------------

enum Where;

template <class T>
class CKDListIter
{
   friend class CKDList<T>;

   CKDListElem<T>*            m_pCurr;
   CKDListElem<T>*            m_pPrev;
   CKDList<T>*                m_pdl;

public:
                  CKDListIter<T>()  {m_pCurr=NULL; m_pPrev=NULL; m_pdl=NULL;}
                  CKDListIter<T>(CKDList<T> &dl);
                  CKDListIter<T>(CKDList<T>* dl,T curr,T prev);

    void          Init(CKDList<T> &dl);
    T             Curr() const      {return (T)m_pCurr;}
    void          Prev();
    void          Next();
    bool          Done() const      {return m_pCurr == NULL;}
    CKDList<T>*   GetList() const   {return m_pdl;}
    bool          IsFirst() const   {return m_pCurr == m_pdl->head;}
    bool          IsLast()  const;          
    bool          IsEmpty() const   {return m_pdl == NULL || m_pdl->head == NULL;}
    void          MoveToFirst()     {m_pCurr = m_pdl->head; m_pPrev = NULL;}
    void          MoveToLast();

    void          AddFirst(T t)     {if (IsFirst()) {m_pPrev = t;} m_pdl->AddFirst(t);}
    void          AddLast(T t)      {m_pdl->AddLast(t);}
    void          AddPrev(T t)      {m_pdl->AddPrev(t,*this);}
    void          AddNext(T t)      {m_pdl->AddNext(t,*this);}
    void          Add(Where w, T t) {if (w == BEFORE) AddPrev(t); else AddNext(t);}

    T             Delete()          {CKDListElem<T>* pt = m_pCurr; m_pdl->Delete(*this); return (T)pt;}
    int           Length() const    {return m_pdl->Length();}
};


template <class T>
CKDListIter<T>::CKDListIter<T>(CKDList<T> &dl)
{
   m_pCurr = dl.head;
   m_pPrev = NULL;
   m_pdl = &dl;
}

template <class T>
CKDListIter<T>::CKDListIter<T>(CKDList<T>* dl,T curr,T prev)
{
   m_pCurr = (CKDListElem<T>*) curr;
   m_pPrev = (CKDListElem<T>*)prev;
   m_pdl = dl;
}

template <class T>
void CKDListIter<T>::Init(CKDList<T> &dl)
{
   m_pCurr = dl.head;
   m_pPrev = NULL;
   m_pdl = &dl;
}

template <class T>
inline void CKDListIter<T>::Next()
{
   CKDListElem<T>* swap = m_pCurr;
   if (m_pCurr)
   {
      // "^" is XOR
      m_pCurr = (CKDListElem<T>*) (((size_t) m_pPrev) ^ ((size_t)(m_pCurr->m_xor)));
   }
   m_pPrev = swap;
}

template <class T>
inline void CKDListIter<T>::Prev()
{
   CKDListElem<T>* swap = m_pPrev;
   if (m_pPrev)
   {
      // "^" is XOR
      m_pPrev = (CKDListElem<T>*) (((size_t) m_pCurr) ^ ((size_t)(m_pPrev->m_xor)));
   }
   m_pCurr = swap;
}

template <class T>
void CKDListIter<T>::MoveToLast()
{
   CKDListIter<T> iter(*m_pdl);
   for (;iter.Curr();iter.Next())
   {
      *this = iter;
   }
}


template <class T>
bool CKDListIter<T>::IsLast() const
{
    if (m_pCurr == NULL)
    {
        return false;
    }
    
    return NULL == (((size_t) m_pPrev) ^ ((size_t)(m_pCurr->m_xor)));
}



//----------------------------------------------------------------
// CKDList
//
// NOTE: T inherits CKDListElem<T>
//----------------------------------------------------------------

template <class T>
class CKDList
{
private:
   CKDListElem<T>*                  head;
public:
   friend class CKDListIter<T>;

                  CKDList()         {head=NULL;}

   bool           IsFirst(const CKDListIter<T> t) const;
   T              FirstElem() const  {return (T)head;}
   T              LastElem() const;
   CKDListIter<T> First() const;
   CKDListIter<T> Last() const;
   void           AddPrev(T t,CKDListIter<T>& pos);
   void           AddNext(T t,CKDListIter<T>& pos);
   void           AddFirst(T t);
   void           AddLast(T t);
   void           Delete(CKDListIter<T>& t);  // Delete element
   int            Length() const;             // Count Elements
   void           Reverse();                  // Reverse the list

private:
    void          InsertFirstAndOnly(T t);
    void          AddMiddle(T t,CKDListIter<T>& left,CKDListIter<T>& right);
    void*         Xor(void* a,void* b) {return (void*)( (size_t)a ^ (size_t)b );}

};


template <class T>
bool CKDList<T>::IsFirst(CKDListIter<T> t) const
{
   return((T)head == t.Curr());
}

template <class T>
CKDListIter<T> CKDList<T>::First() const
{
   // Return the iterator pointing to the first element

   CKDListIter<T> ret;
   ret.m_pCurr = head;
   ret.m_pPrev=NULL;
   ret.m_pdl = (CKDList<T> *)this;
   return ret;
}

template <class T>
CKDListIter<T> CKDList<T>::Last() const
{
   // Return the iterator pointing to the last element

   CKDListIter<T> iter(*(CKDList<T> *)this);
   CKDListIter<T> ret(*(CKDList<T> *)this);
   for (;iter.Curr();iter.Next())
   {
      ret = iter;
   }
   return ret;
}

template <class T>
T CKDList<T>::LastElem() const
{
   CKDListIter<T> iter(*(CKDList<T> *)this);
   CKDListIter<T> ret(*(CKDList<T> *)this);
   for (;iter.Curr();iter.Next())
      ret = iter;
   return ret.Curr();
}

template <class T>
void CKDList<T>::AddPrev(T t, CKDListIter<T>& pos)
{
   if (!pos.Curr())
   {
       AddLast(t);
   }
   else
   {
      CKDListIter<T> prev = pos;
      prev.Prev();
      if (!prev.Curr())
      {
         AddFirst(t);
      }
      else
      {
         AddMiddle(t,prev,pos);
      }
      pos.m_pPrev = (CKDListElem<T>*) t;
   }
}

template <class T>
void CKDList<T>::AddNext(T t, CKDListIter<T>& pos)
{
   if (!pos.Curr())
   {
      AddFirst(t);
   }
   else
   {
      CKDListIter<T> next = pos;
      next.Next();
      if (!next.Curr())
      {
         AddLast(t);
      }
      else
      {
         AddMiddle(t,pos,next);
      }
   }
}

template <class T>
void CKDList<T>::AddFirst(T t)
{
   if (!head)
   {
      InsertFirstAndOnly(t);
   }
   else
   {
      ((CKDListElem<T>*)t)->m_xor = head;
      head->m_xor = Xor(t,head->m_xor);
      head = (CKDListElem<T>*)t;
   }
}

template <class T>
void CKDList<T>::AddLast(T t)
{
   if (!head)
   {
      InsertFirstAndOnly(t);
   }
   else
   {
      CKDListElem<T>* last = (CKDListElem<T>*) (Last().Curr());
      ((CKDListElem<T>*)t)->m_xor = last;
      last->m_xor = Xor(last->m_xor, t);
   }
}

template <class T>
void CKDList<T>::InsertFirstAndOnly(T t)
{
   ((CKDListElem<T>*)t)->m_xor = NULL;
   head = (CKDListElem<T>*)t;
}

template <class T>
void CKDList<T>::AddMiddle(T t,CKDListIter<T>& left,CKDListIter<T>& right)
{
   CKDListIter<T> prev = left;
   prev.Prev();
   CKDListIter<T> next = right;
   next.Next();
   left.m_pCurr->m_xor = Xor(prev.m_pCurr, (CKDListElem<T>*)t);
   right.m_pCurr->m_xor = Xor(next.m_pCurr, (CKDListElem<T>*)t);
   ((CKDListElem<T>*)t)->m_xor = Xor(left.m_pCurr, right.m_pCurr);
}

template <class T>
void CKDList<T>::Delete(CKDListIter<T>& pos)
{
   // Verify (pos.Curr() != NULL);
   CKDListIter<T> prev = pos;
   prev.Prev();
   CKDListIter<T> next = pos;
   next.Next();
   if (head == pos.m_pCurr)
      head = next.m_pCurr;
   if (prev.Curr())
   {
      prev.m_pCurr->m_xor = Xor(prev.m_pPrev, next.Curr());
   }
   if (next.Curr())
   {
      next.m_pCurr->m_xor = Xor(pos.Curr(), Xor(next.m_pCurr->m_xor, prev.Curr()));
   }
   pos.m_pCurr = (CKDListElem<T>*)next.m_pCurr;  //updating iter for user.
}

template <class T>
void CKDList<T>::Reverse()
{
   head = Last().m_pCurr;
}

template <class T>
int CKDList<T>::Length() const
{
   if (head == NULL)
      return 0;
   CKDListIter<T> pos(*(CKDList<T> *)this);
   int count = 0;
   while (pos.Curr())
   {
      ++count;
      pos.Next();
   }
   return count;
}
