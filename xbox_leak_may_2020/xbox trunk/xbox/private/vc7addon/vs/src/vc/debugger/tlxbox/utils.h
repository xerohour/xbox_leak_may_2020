/*++

Copyright (c) Microsoft Corporation

Module Name:

    utils.h

Abstract:
    
    Various Utility Functions And Class - added as needed.

--*/

#ifndef __XBOX_UTILS__
#define __XBOX_UTILS__


//-----------------------------------------------------------------
// A very simple class for holding onto a critical section, within
// a particular scope.
//-----------------------------------------------------------------
class CHoldCriticalSection
{
  public:
     CHoldCriticalSection(CRITICAL_SECTION *pCriticalSection) : 
        m_pCriticalSection(pCriticalSection)
     {
        EnterCriticalSection(m_pCriticalSection);
     }
    ~CHoldCriticalSection()
    {
        LeaveCriticalSection(m_pCriticalSection);
    }
  private:
    CRITICAL_SECTION *m_pCriticalSection;
};

template<class T> inline void ZeroStructure(T& tStruct)
{
    memset(reinterpret_cast<void *>(&tStruct), 0, sizeof(T));
}

#endif //__XBOX_UTILS__