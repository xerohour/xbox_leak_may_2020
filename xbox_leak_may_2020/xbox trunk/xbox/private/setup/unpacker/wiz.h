//  WIZ.H
//
//  Created 27-Mar-2001 [JonT]

#ifndef _WIZ_H_
#define _WIZ_H_

//---------------------------------------------------------------------

class CWizard
{
private:
    static CWizard* m_spThis;

    HWND m_hwndPropSheet;
    BOOL m_fCentered;
	int m_nReturn;

public:
    CWizard();
    ~CWizard();

    static CWizard* GetWizard();
    static BOOL ActivateCurrentDlg();

    int DoModal(HWND hwndParent);
    VOID SetPropSheetHwnd(HWND hwnd);
    HWND GetPropSheetHwnd();
    BOOL TestAndSetCentered();
	VOID SetReturnValue(int nRet) { m_nReturn = nRet; }
};

inline 
CWizard::CWizard() :
    m_hwndPropSheet(NULL),
    m_fCentered(FALSE)
{
    ASSERT(m_spThis == NULL);
    m_spThis = this;
}

inline
CWizard::~CWizard()
{
    m_spThis = NULL;
}

inline CWizard*
CWizard::GetWizard()
{
    return m_spThis;
}

inline BOOL CWizard::ActivateCurrentDlg()
{
    BOOL bRet = FALSE;
    if (NULL != m_spThis)
    {
        ASSERT(::IsWindow(m_spThis->m_hwndPropSheet));
        ::SetForegroundWindow(m_spThis->m_hwndPropSheet);
        bRet = TRUE;
    }
    return bRet;
}

// The OnInit() of the first wizard page should call this method.
inline VOID 
CWizard::SetPropSheetHwnd(HWND hwnd)
{
    m_hwndPropSheet = hwnd;
}


inline HWND
CWizard::GetPropSheetHwnd()
{
    return (m_hwndPropSheet);
}

inline BOOL
CWizard::TestAndSetCentered()
{
    BOOL fRet = m_fCentered;

    m_fCentered = TRUE;

    return fRet;
}


#endif // #ifndef _WIZ_H_
