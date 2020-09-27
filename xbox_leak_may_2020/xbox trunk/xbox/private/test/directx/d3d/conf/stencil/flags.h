// FILE:        flags.h
// DESC:        flags class header
// AUTHOR:      Todd M. Frost

class CFlags {
    private:
        DWORD m_dwCurrent; // current flags
        DWORD m_dwSaved;   // saved flags

    public:
        inline BOOL bCheck(DWORD dwFlags)
        {
            if (m_dwCurrent & dwFlags)
                return (BOOL) TRUE;
            else
                return (BOOL) FALSE;
        };
        inline VOID vClear(DWORD dwFlags)
        {
            m_dwCurrent &= ~dwFlags;
        };
        inline VOID vSet(DWORD dwFlags)
        {
            m_dwCurrent |= dwFlags;
        };
        inline DWORD dwGet(VOID)
        {
            return m_dwCurrent;
        };
        inline VOID vSave(VOID)
        {
            m_dwSaved = m_dwCurrent;
        };
        inline VOID vRestore(VOID)
        {
            m_dwCurrent = m_dwSaved;
        };
        inline VOID vReplace(DWORD dwFlags)
        {
            m_dwCurrent = dwFlags;
        };
        CFlags(VOID);
        ~CFlags(VOID); 
};
