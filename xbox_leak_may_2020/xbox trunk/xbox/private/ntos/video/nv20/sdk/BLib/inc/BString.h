#ifndef _BSTRING_H
#define _BSTRING_H

//////////////////////////////////////////////////////////////////////////////
// string
//
class BString
{
    //
    // members
    //
protected:
    char *m_pszData;
    int   m_iMax;

    //
    // methods
    //
protected:
    void grow (int iNewMax,int bCopy)
    {
        iNewMax = max(iNewMax,4);
        iNewMax = (iNewMax + 127) & ~127;
        if (iNewMax && (iNewMax != m_iMax))
        {
            char *pszTemp = new char[iNewMax];
            if (!pszTemp) throw new BException(BException::outOfMemory);
            if (m_pszData && bCopy) strcpy (pszTemp,m_pszData);
            delete[] m_pszData;
            m_pszData = pszTemp;
            m_iMax    = iNewMax;
        }
    }
    void kill (void)
    {
        delete[] m_pszData;
        m_pszData = NULL; m_iMax = 0;
    }

    //
    // helpers
    //
public:
    inline int length (void) const { return m_pszData ? strlen(m_pszData) : 0; }

    inline void prepareForSize (int iCount) { grow (iCount,TRUE); }

    inline void place  (int pos,const BString& str)
    { 
        assert ((pos >= 0) && (pos <= length())); 
        int l1 = length();
        int l2 = str.length();
        grow (l1 + l2 + 1,1); 
        memcpy (m_pszData + pos,(const char*)str,l2); 
        if ((pos + l2) > l1) m_pszData[pos + l2] = 0;
    }
    inline void insert (int pos,const BString& str)
    { 
        assert ((pos >= 0) && (pos <= length())); 
        int l1 = length();
        int l2 = str.length();
        grow (l1 + l2 + 1,1); 
        memmove (m_pszData + pos + l2,m_pszData + pos,l1 - pos + 1); 
        memcpy (m_pszData + pos,(const char*)str,l2);
        m_pszData[l1 + l2] = 0;
    }
    inline void remove (int pos,int count = 1)
    {
        assert ((pos >= 0) && (pos < length()));
        strcpy (m_pszData + pos,m_pszData + pos + count);
        grow (length(),1); // shrink buffer
    }
    inline BString left (int count) const
    {
        BString str = *this;
        if (count < str.length()) str.m_pszData[count] = 0;
        return str;
    }
    inline BString right (int count) const
    {
        BString str = *this;
        int     len = str.length();
        if (count < len)
        {
            memmove (str.m_pszData,str.m_pszData + len - count,count + 1);
        }
        return str;
    }
    inline BString mid (int index,int count = 0x7fffffff) const
    {
        BString str = *this;
        int     len = str.length();
        if (index < len)
        {
            count = min(len - index,count);
            memmove (str.m_pszData,str.m_pszData + index,count);
            str.m_pszData[count] = 0;
        }
        else
        {
            str.m_pszData[0] = 0;
        }
        return str;
    }

    inline int find (const BString& sub) const
    {
        if (m_pszData && sub.m_pszData)
        {
            char *c = strstr(m_pszData,sub.m_pszData);
            return c ? (c - m_pszData) : -1;
        }
        return -1;
    }

    inline void format (const char *szFormat,...)
    {
        char sz[1024];
        vsprintf (sz,szFormat,(char*)(unsigned(&szFormat) + 4));
        *this = sz;
    }

    inline BString makeUpper (void) { BString str = *this; strupr ((char*)(const char*)str); return str; }
    inline BString makeLower (void) { BString str = *this; strlwr ((char*)(const char*)str); return str; }

    //
    // operators
    //
public:
    inline operator const char* (void) const { return (const char*)(m_pszData ? m_pszData : ""); }

    inline const BString& operator=  (const char* psz)    { grow(strlen(psz),0); strcpy (m_pszData,psz); return *this; }
    inline const BString& operator+= (const char *sz)     { grow(length() + strlen(sz) + 1,1); strcat (m_pszData,sz); return *this; }

    inline const BString& operator=  (const BString& str) { *this  = (const char*)str; return *this; }
    inline const BString& operator+= (const BString& str) { *this += (const char*)str; return *this; }

    //inline const int operator== (const BString& str) { return !strcmp(m_pszData ? m_pszData : "",str.m_pszData ? str.m_pszData : ""); }
    friend inline int operator== (const BString& s1,const BString& s2);

    //
    // construction
    //
public:
    inline  BString (const char* psz)    { m_pszData = NULL; m_iMax = 0; *this = psz; }
    inline  BString (const BString& str) { m_pszData = NULL; m_iMax = 0; *this = str; }
    inline  BString (void)               { m_pszData = NULL; m_iMax = 0; }
    inline ~BString (void)               { kill(); }
};


inline int operator== (const BString& s1,const BString& s2) { return !strcmp(s1.m_pszData ? s1.m_pszData : "",s2.m_pszData ? s2.m_pszData : ""); }

#endif