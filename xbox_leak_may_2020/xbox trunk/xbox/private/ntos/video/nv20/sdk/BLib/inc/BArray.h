#ifndef _BARRAY_H
#define _BARRAY_H

//////////////////////////////////////////////////////////////////////////////
// Array
//
template<class T,class R,const unsigned cuGrowSize = 16> class BArray
{
    //
    // members
    //
protected:
    T        *m_pData;
    unsigned  m_uCount;
    unsigned  m_uMax;

    //
    // methods
    //
protected:
    inline void grow (unsigned uMax)
    {
        uMax = (uMax + cuGrowSize - 1) & ~(cuGrowSize - 1);
        if (uMax > m_uMax)
        {
            T* pTemp = new T[uMax];
            if (m_pData)
            {
                for (unsigned u = 0; u < m_uCount; u++) pTemp[u] = m_pData[u];
                delete[] m_pData;
            }
            m_pData = pTemp;
            m_uMax = uMax;
        }
    }
    inline void init (void)
    {
        m_uCount = m_uMax = 0;
        m_pData = NULL;
    }
    inline void kill (void)
    {
        delete[] m_pData;
        init();
    }

public:
    inline R operator[] (unsigned u) const { return m_pData[u]; }

public:
    inline void setAt (unsigned uIndex,R data)       { if (uIndex >= m_uMax) grow (uIndex + 1); if (uIndex >= m_uCount) m_uCount = uIndex + 1; m_pData[uIndex] = data; }
    inline R    getAt (unsigned uIndex)        const { return m_pData[uIndex]; }
    
    inline int      isEmpty  (void) const { return m_uCount == 0; }
    inline unsigned getCount (void) const { return m_uCount; }

    inline void append (R data) { if (m_uCount >= m_uMax) grow (m_uCount + 1); m_pData[m_uCount++] = data; }

    inline void removeAll (void) { kill(); }

	inline BArray & operator=(const BArray &arr) { 
		init(); 
		grow(arr.m_uCount); 
		m_uCount = arr.m_uCount;
		for (unsigned i=0; i<arr.m_uCount; i++) {
			m_pData[i] = arr.m_pData[i];
		}

		return *this;
	}

    //
    // construction
    //
public:
    inline  BArray (void) { init(); }
    inline  BArray (const BArray &arr) { 
		init(); 
		grow(arr.m_uCount); 
		m_uCount = arr.m_uCount;
		for (unsigned i=0; i<m_uCount; i++) {
			m_pData[i] = arr.m_pData[i];
		}
	}
    inline ~BArray (void) { kill(); }
};

#endif