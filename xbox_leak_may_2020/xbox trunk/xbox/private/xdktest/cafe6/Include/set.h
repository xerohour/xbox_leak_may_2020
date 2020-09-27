/////////////////////////////////////////////////////////////////////////////
// set.h
//
// email	date		change
// briancr	08/02/95	created
//
// copyright 1995 Microsoft

// Interface of the CSet<T> class

#ifndef __SET_H__
#define __SET_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSet class

template <class T> class CSet: public CObject
{
// constructor/destructor
public:
	CSet();
	CSet(const T item);
	CSet(const CSet<T>& set);
	virtual ~CSet();

// operations
public:
	void Add(const T item);
	void Add(const CSet<T>& set);
	void Copy(const CSet<T>& set);
	void RemoveAll(void);

	POSITION GetStartPosition(void) const;
	T GetNext(POSITION& pos) const;

	BOOL Contains(const T& item) const;
	BOOL IsEmpty(void) const;
	int GetSize(void) const;

	CSet<T>& operator=(const CSet<T>& set);
	BOOL operator==(const CSet<T>& set);
	BOOL operator!=(const CSet<T>& set);
	CSet<T> operator+(const CSet<T>& set);

// data types
protected:
	typedef CArray<T, T&> SetArray;

// data
protected:
	SetArray* m_pSet;
};

#endif //__SET_H__
