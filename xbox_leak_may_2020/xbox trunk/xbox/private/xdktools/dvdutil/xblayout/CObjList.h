// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CObject.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CObjNode
{
public:
    CObject *m_pobj;
    CObjNode *m_ponNext;
    CObjNode *m_ponPrev;
};

// PlaceableObject list
class CObjList
{
public:
    CObjList();
    ~CObjList();
    CObjNode *GetObjNode(CObject *pobj);

    CObject *GetLast() { m_ponCur = m_ponTail; return GetCur(); }
    CObject *GetPrev() {if (m_ponCur != NULL) m_ponCur = m_ponCur->m_ponPrev; return GetCur();}
    CObject *GetFirst() { m_ponCur = m_ponHead; return GetCur(); }
    CObject *GetNext() {if (m_ponCur != NULL) m_ponCur = m_ponCur->m_ponNext; return GetCur();}
    CObject *GetCur() {if (m_ponCur == NULL) return NULL; else return m_ponCur->m_pobj; }
    void Reset();
    void Add(CObject *ppo);
    void Remove(CObject *ppo);
    bool InsertBefore(CObject *poToAdd, CObject *poLoc);
    bool InsertAfter(CObject *poToAdd, CObject *poLoc);

	bool PersistTo(HANDLE hfileFLD);
	bool CreateFrom(HANDLE hfileFLD);;
private:
    CObjNode *m_ponHead;
    CObjNode *m_ponTail;
    CObjNode *m_ponCur;
};
