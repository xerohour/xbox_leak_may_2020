//  PROPERTY.H
//
//		Text property handling
//
//	Created 15-Mar-2000 [JonT]

#ifndef __PROPERTY_H__
#define __PROPERTY_H__

class CProperty;

#define TABLE_SIZE 4001		// prime number

class CPropertyBundle
{
private:
	CListHead m_lhHashTable[TABLE_SIZE];
	int HashNameToIndex(LPCTSTR lpszName);
	CProperty* Find(LPCTSTR lpszName);

public:
	LPCTSTR Get(LPCTSTR lpszName);
	void Set(LPCTSTR lpszName, LPCTSTR lpszValue);
};


// CProperty
//	Handles storage for property values. This is a private class, it's never handed back.
//	It's just used for CPropertyBundle's use.

class CProperty : public CNode
{
public:
    CProperty()
        : m_lpszName(NULL),
          m_lpszValue(NULL)
    {
    }

    ~CProperty()
    {
        if (m_lpszName != NULL)
            free(m_lpszName);
        if (m_lpszValue != NULL)
            free(m_lpszValue);
    }

    void SetName(LPCSTR pszName)
    {
        if (m_lpszName != NULL)
            free(m_lpszName);
        m_lpszName = _strdup(pszName);
    }
    LPCSTR GetName()
    {
        return m_lpszName;
    }

    void SetValue(LPCSTR pszValue)
    {
        if (m_lpszValue != NULL)
            free(m_lpszValue);
        m_lpszValue = _strdup(pszValue);
    }
    LPCSTR GetValue()
    {
        return m_lpszValue;
    }


private:
	LPTSTR m_lpszName;
	LPTSTR m_lpszValue;
};


#endif // #ifndef __PROPERTY_H__