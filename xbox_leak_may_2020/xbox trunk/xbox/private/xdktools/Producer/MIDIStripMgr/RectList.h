// RectList.h: interface for the CRectList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTLIST_H__13077F85_3C19_11D1_8848_00C04FBF8D15__INCLUDED_)
#define AFX_RECTLIST_H__13077F85_3C19_11D1_8848_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef struct RectElem
{
	RECT rect;
	RectElem *next;
} RectElem;

class CRectList  
{
public:
	CRectList();
	void InsertRect(RECT rect);
	int RemoveBefore(RECT rect);
	const RECT* GetFirst();
	const RECT* GetNext();
	virtual ~CRectList();
	void RemoveAll( void );

private:
	RectElem* m_pList;
	RectElem* m_pRect;
};

#endif // !defined(AFX_RECTLIST_H__13077F85_3C19_11D1_8848_00C04FBF8D15__INCLUDED_)
