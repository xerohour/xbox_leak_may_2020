//  UNPACKUI.H
//
//  Created 16-Mar-2001

#ifndef _UNPACKUI_H_
#define _UNPACKUI_H_

#include <commctrl.h>
#include <windowsx.h>
#include <tchar.h>
#include <olectl.h>
#include <prsht.h>
#include <shlobj.h>
#include "resource.h"
#include "wiz.h"
#include "wizpage.h"

//---------------------------------------------------------------------
//  Steps to adding a new wizard page
//  1. Draw the new dialog template
//  2. Add strings for Title and Subtitle if necessary
//  3. Bump up MAX_WIZARD_PAGES
//  4. Create the new wizard page class derived from CWizardPage
//      (don't forget to use the new IDD_ value in calling the CWizardPage constructor)
//  5. Instantiate the page in wiz.cpp in DoModal()

#define MAX_WIZARD_PAGES 6

//---------------------------------------------------------------------

class CWizardWelcome : public CWizardPage
{
private:
    virtual VOID OnInit();
    virtual VOID OnSetActive();
    virtual int OnNext();

public:
    CWizardWelcome(CWizard* pWizard, PROPSHEETPAGE* pPSP);
};


//---------------------------------------------------------------------

class CWizardError : public CWizardPage
{
private:
    virtual VOID OnSetActive();
    virtual VOID OnFinish();

    static DWORD m_idLastError;
    static DWORD m_dwErrorCode;
    static TCHAR m_szLastError[];

public:
    CWizardError(CWizard* pWizard, PROPSHEETPAGE* pPSP);

    friend void WizSetLastError(DWORD dwStringID, DWORD dwErrorCode);
    friend void WizSetLastErrorString(LPCTSTR lpString, DWORD dwErrorCode);
    friend LPCTSTR WizGetLastErrorString();
};


inline void
WizSetLastError(
    DWORD dwStringID,
    DWORD dwErrorCode
    )
{
    CWizardError::m_szLastError[0] = 0;
    CWizardError::m_idLastError = dwStringID;
    CWizardError::m_dwErrorCode = dwErrorCode;
}

inline void
WizSetLastErrorString(
    LPCTSTR lpString,
    DWORD dwErrorCode
    )
{
    strcpy(CWizardError::m_szLastError, lpString);
    CWizardError::m_idLastError = 0;
    CWizardError::m_dwErrorCode = dwErrorCode;
}

inline LPCTSTR
WizGetLastErrorString()
{
    return CWizardError::m_szLastError;
}

//---------------------------------------------------------------------

class CWizardLicense : public CWizardPage
{
private:
    virtual VOID OnSetActive();
    virtual int OnNext();
    virtual VOID OnBack();

    LPSTR m_pszTitle;
    LPSTR m_pszSubtitle;
public:
    CWizardLicense(CWizard* pWizard, PROPSHEETPAGE* pPSP);
    ~CWizardLicense()
    {
        if(m_pszTitle) free(m_pszTitle);
        if(m_pszSubtitle) free(m_pszSubtitle);
    }
};


//---------------------------------------------------------------------

class CWizardSelectDir : public CWizardPage
{
private:
    virtual BOOL OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual VOID OnSetActive();
    virtual int OnNext();
    virtual VOID OnBack();

public:
    CWizardSelectDir(CWizard* pWizard, PROPSHEETPAGE* pPSP);
};


//---------------------------------------------------------------------

class CWizardCopying : public CWizardPage
{
private:
    virtual VOID OnSetActive();
    virtual BOOL OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CWizardCopying(CWizard* pWizard, PROPSHEETPAGE* pPSP);
};


//---------------------------------------------------------------------

class CWizardFinish : public CWizardPage
{
private:
    virtual VOID OnSetActive();
    virtual VOID OnFinish();
    virtual BOOL OnQueryCancel();

public:
    CWizardFinish(CWizard* pWizard, PROPSHEETPAGE* pPSP);
};


#endif // #ifndef _UNPACKUI_H_
