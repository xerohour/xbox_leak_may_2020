//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctscript.cpp
//
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
// ctscript.cpp
//
// Test harness implementation of IDirectMusicScript
//
// Functions:
//    CtIDirectMusicScript::CtIDirectMusicScript()
//    CtIDirectMusicScript::~CtIDirectMusicScript()
//    CtIDirectMusicScript::InitTestClass()
//    CtIDirectMusicScript::GetRealObjPtr()
//  IUnknown
//    CtIDirectMusicScript::QueryInterface()
//
//    CtIDirectMusicScript::Init()
//    CtIDirectMusicScript::CallRoutine()
//    CtIDirectMusicScript::SetVariableVariant()
//    CtIDirectMusicScript::GetVariableVariant()
//    CtIDirectMusicScript::SetVariableNumber()
//    CtIDirectMusicScript::GetVariableNumber()
//    CtIDirectMusicScript::SetVariableObject()
//    CtIDirectMusicScript::GetVariableObject()
//    CtIDirectMusicScript::EnumRoutine()
//    CtIDirectMusicScript::EnumVariable()
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - updated for changes to IDMScript changes (removed
//                         IDMScriptError in favor of the structure ptr)
//===========================================================================


#include "globals.h"

//===========================================================================
// CtIDirectMusicScript::CtIDirectMusicScript()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
CtIDirectMusicScript::CtIDirectMusicScript()
{
    // initialize our member variables
    m_pUnk = NULL;
    m_dwRefCount = 1;
    m_szInterfaceName = "IDirectMusicScript";

} //*** end CtIDirectMusicScript::CtIDirectMusicScript()




//===========================================================================
// CtIDirectMusicScript::~CtIDirectMusicScript()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
CtIDirectMusicScript::~CtIDirectMusicScript(void)
{
    // nothing to do

} //*** emd CtIDirectMusicScript::~CtIDirectMusicScript()




//===========================================================================
// CtIDirectMusicScript::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicScript object pointer for future use.
//
// Parameters:
//  CtIDirectMusicScript    *pdmScript    - pointer to real IDirectMusicScript object
//
// Returns: 
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicScript::InitTestClass(IDirectMusicScript *pdmScript)
{
    // validate pointer... if bad, return error
    if(!helpIsValidPtr(pdmScript, sizeof(IDirectMusicScript), FALSE))
    {
        m_pUnk = NULL;
        return E_POINTER;
    }

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmScript));

} //*** end CtIDirectMusicScript::InitTestClass()




//===========================================================================
// CtIDirectMusicScript::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//  IDirectMusicScript **ppdmScript - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicScript::GetRealObjPtr(IDirectMusicScript **ppdmScript)
{

    // validate ppdm
    if(!helpIsValidPtr(ppdmScript, sizeof(IDirectMusicScript*), FALSE))
    {
        return E_POINTER;
    }

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmScript));

} //*** end CtIDirectMusicScript::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicScript::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicScript::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hRes        = E_NOTIMPL;
    LPVOID  *ppvTemp    = NULL;
    BOOL    fMatchFound = FALSE;

    // BUGBUG - validate ppvObj

    // BUGBUG - this will need to do some wrapper class creation
    // based on test harness specific IIDs

    // use the base class to perform the actual QI
    hRes = (CtIUnknown::QueryInterface(riid, ppvObj));

    // BUGBUG - iff harness IID, wrap the interface returned

    // BUGBUG - otherwise, copy ppvTemp to ppvObj

    // done
    return hRes;

} //*** end CtIDirectMusicScript::QueryInterface()




//===========================================================================
// CtIDirectMusicScript::Init()
//
// Encapsulates calls to Init
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::Init
(
    CtIDirectMusicPerformance* pPerformance,
    DMUS_SCRIPT_ERRORINFO*     pdmScriptErrorInfo
)
{
    HRESULT                     hRes                = E_FAIL;
    BOOL                        fValid_pPerformance = TRUE;
    BOOL                        fValid_pdmScriptErr = TRUE;
    IDirectMusicPerformance*    pdmPerfomance       = NULL;

    // validate pPerformance
    if(!helpIsValidPtr((void*)pPerformance, sizeof(CtIDirectMusicPerformance), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pPerformance = FALSE;
        pdmPerfomance = (IDirectMusicPerformance*)pPerformance;

    }
    else
    {
        // good pointer, get the real object
        hRes = pPerformance->GetRealObjPtr(&pdmPerfomance);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }


    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL,  "--- Calling %s::Init()",  m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPerfomance == %p   %s",
            pdmPerfomance, fValid_pPerformance ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->Init(pdmPerfomance, pdmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pPerformance && pdmPerfomance)
    {
        pdmPerfomance->Release();
    }
    return hRes;

} //*** end CtIDirectMusicScript::Init()




//===========================================================================
// CtIDirectMusicScript::CallRoutine()
//
// Encapsulates calls to CallRoutine
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::CallRoutine
(
    CHAR                       *pszRoutineName,   
    DMUS_SCRIPT_ERRORINFO*      pdmScriptErrorInfo)
{
    HRESULT     hRes                    = E_FAIL;
    BOOL        fValid_pszRoutineName  = TRUE;
    BOOL        fValid_pdmScriptErr     = TRUE;

    // validate pwszRoutineName
    if(!helpIsValidReadPtr((void*)pszRoutineName, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pszRoutineName = FALSE;
    }
    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs 
    fnsLog(CALLLOGLEVEL,    "--- Calling %s::CallRoutine()",  m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL,   "pszRoutineName == %s (%p)   %s",
           pszRoutineName, pszRoutineName, fValid_pszRoutineName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL,   "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->CallRoutine(pszRoutineName, pdmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CallRoutine()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::CallRoutine()




//===========================================================================
// CtIDirectMusicScript::SetVariableVariant()
//
// Encapsulates calls to SetVariableVariant
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
/*
HRESULT CtIDirectMusicScript::SetVariableVariant
(
    WCHAR                   *pwszVariableName,
    VARIANT                 varValue,//BUGBUG this needs to be a ptr
    BOOL                    fSetRef,
    DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
)
{
    HRESULT         hRes                    = E_FAIL;
    BOOL            fValid_pwszVariableName = TRUE;
    BOOL            fValid_pdmScriptErr     = TRUE;

    DMUS_SCRIPT_ERRORINFO       dmScriptErrorInfo;

    // validate pwszVariableName
    if(!helpIsValidPtr((void*)pwszVariableName, sizeof(WCHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwszVariableName = FALSE;
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetVariableVariant()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pwszVariableName == %s (%p)   %s",
            pwszVariableName, pwszVariableName, fValid_pwszVariableName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "varValue         == %08Xh", varValue);
    fnsLog(PARAMLOGLEVEL, "fSetRef          == %08Xh", fSetRef);
    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->SetVariableVariant(pwszVariableName, varValue, fSetRef, &dmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetVariableVariant()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::SetVariableVariant()




//===========================================================================
// CtIDirectMusicScript::GetVariableVariant()
//
// Encapsulates calls to GetVariableVariant
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::GetVariableVariant
(
    WCHAR                   *pwszVariableName,
    VARIANT                 *pvarValue,         //BUGBUG this needs to be a ptr
    DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
)
{
    HRESULT         hRes                    = E_FAIL;
    BOOL            fValid_pwszVariableName = TRUE;
    BOOL            fValid_pvarValue        = TRUE;
    BOOL            fValid_pdmScriptErr     = TRUE;

    DMUS_SCRIPT_ERRORINFO       dmScriptErrorInfo;


    // validate pwszVariableName
    if(!helpIsValidPtr((void*)pwszVariableName, sizeof(WCHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwszVariableName = FALSE;
    }

    // validate pvarValue
    if(!helpIsValidPtr((void*)pvarValue, sizeof(VARIANT*), FALSE))
    {
        fValid_pvarValue = FALSE;
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetVariableVariant()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pwszVariableName == %ls (%p)   %s", pwszVariableName, pwszVariableName,
            fValid_pwszVariableName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pvarValue        == %p   %s", pvarValue, fValid_pvarValue ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->GetVariableVariant(pwszVariableName,pvarValue, &dmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetVariableVariant()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::GetVariableVariant()

*/


//===========================================================================
// CtIDirectMusicScript::SetVariableNumber()
//
// Encapsulates calls to SetVariableNumber
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::SetVariableNumber
(
    CHAR                   *pszVariableName,
    LONG                    lValue,
    DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
)
{
    HRESULT         hRes                    = E_FAIL;
    BOOL            fValid_pszVariableName = TRUE;
    BOOL            fValid_pdmScriptErr     = TRUE;

    DMUS_SCRIPT_ERRORINFO       dmScriptErrorInfo;

    // validate pszVariableName
    if(!helpIsValidReadPtr((void*)pszVariableName, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pszVariableName = FALSE;
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetVariableNumber()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pszVariableName == %s (%p)   %s",
            pszVariableName, pszVariableName, fValid_pszVariableName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "lValue           == %d", lValue);
    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->SetVariableNumber(pszVariableName,lValue, &dmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetVariableNumber()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::SetVariableNumber()




//===========================================================================
// CtIDirectMusicScript::GetVariableNumber()
//
// Encapsulates calls to GetVariableNumber
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::GetVariableNumber
(
    CHAR                   *pszVariableName,
    LONG                    *plValue,
    DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
)
{
    HRESULT         hRes                    = E_FAIL;
    BOOL            fValid_pszVariableName = TRUE;
    BOOL            fValid_plValue          = TRUE;
    BOOL            fValid_pdmScriptErr     = TRUE;

    DMUS_SCRIPT_ERRORINFO       dmScriptErrorInfo;

    // validate pszVariableName
    if(!helpIsValidReadPtr((void*)pszVariableName, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pszVariableName = FALSE;
    }

    // validate plValue
    if(!helpIsValidPtr((void*)plValue, sizeof(LONG*), FALSE))
    {
        fValid_plValue = FALSE;
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetVariableNumber()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pszVariableName == %s (%p)   %s",
            pszVariableName, pszVariableName, fValid_pszVariableName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "plValue          == %p   %s",
            plValue, fValid_plValue ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->GetVariableNumber(pszVariableName, plValue, &dmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetVariableNumber()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::GetVariableNumber()




//===========================================================================
// CtIDirectMusicScript::SetVariableObject()
//
// Encapsulates calls to SetVariableObject
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//  02/20/2000 - jimmo   - Now takes an IUnknown* instead of CtIUnkown.
//                         It's a type, like BOOL or LONG, so we don't wrap it.
//                         This simplifies testing code.
//===========================================================================
HRESULT CtIDirectMusicScript::SetVariableObject
(
    CHAR                   *pwszVariableName,
    IUnknown                *punkValue,
    DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
)
{
    HRESULT         hRes                        = E_FAIL;
    BOOL            fValid_pwszVariableName     = TRUE;
    BOOL            fValid_punkValue            = TRUE;
    BOOL            fValid_pdmScriptErr         = TRUE;

    DMUS_SCRIPT_ERRORINFO       dmScriptErrorInfo;

    // validate pwszVariableName
    if(!helpIsValidReadPtr((void*)pwszVariableName, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwszVariableName = FALSE;
    }

    // validate punkValue
    if(!helpIsValidPtr((void*)punkValue, sizeof(IUnknown*), FALSE))
    {
        fValid_punkValue = FALSE;
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetVariableObject()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pwszVariableName == %s (%p)   %s",
            pwszVariableName, pwszVariableName, fValid_pwszVariableName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "punkValue        == %p   %s",
            punkValue, fValid_punkValue ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->SetVariableObject(pwszVariableName, punkValue, &dmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetVariableObject()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    return hRes;

} //*** end CtIDirectMusicScript::SetVariableObject()




//===========================================================================
// CtIDirectMusicScript::GetVariableObject()
//
// Encapsulates calls to GetVariableObject
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//  02/20/2000 - jimmo   - Now takes an IUnknown* instead of CtIUnkown.
//                         It's a type, like BOOL or LONG, so we don't wrap it.
//                         This simplifies testing code.
//===========================================================================
HRESULT CtIDirectMusicScript::GetVariableObject
(
	CHAR *pwszVariableName,
	REFIID riid,
	LPVOID FAR *ppv,
	DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
    HRESULT         hRes                    = E_FAIL;
    BOOL            fValid_pwszVariableName = TRUE;
    BOOL            fValid_pdmScriptErr     = TRUE;
  
    // validate pwszVariableName
    if(!helpIsValidReadPtr((void*)pwszVariableName, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwszVariableName = FALSE;
    }

//BUGBUG
// validation
// riid
// ppv

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetVariableObject()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pwszVariableName == %s (%p)   %s", pwszVariableName, pwszVariableName,
            fValid_pwszVariableName ? "" : "BAD");

//BUGBUG
// riid logging
// ppv  logging

    fnsLog(PARAMLOGLEVEL, "pErrorInfo == %p   %s",
            pErrorInfo, fValid_pdmScriptErr ? "" : "BAD");
    
    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->GetVariableObject(pwszVariableName, riid, ppv, pErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetVariableObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pErrorInfo);


    fnsDecrementIndent();

    return hRes;

} //*** end CtIDirectMusicScript::GetVariableObject()




//===========================================================================
// CtIDirectMusicScript::EnumRoutine()
//
// Encapsulates calls to EnumRoutine
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
/*
HRESULT CtIDirectMusicScript::EnumRoutine
(
    DWORD dwIndex,
    WCHAR *pwszName
)
{
    HRESULT hRes            = E_FAIL;
    BOOL    fValid_pwszName = TRUE;

    // validate pwszRoutineName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwszName = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumRoutine()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex  == %08Xh", dwIndex);
    fnsLog(PARAMLOGLEVEL, "pwszName == (%p)   %s",
            pwszName, fValid_pwszName ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->EnumRoutine(dwIndex,pwszName);

    // log results
    fnsLog(PARAMLOGLEVEL, "Found pwszName == %s (%p)   %s",
            pwszName, pwszName, fValid_pwszName ? "" : "BAD");
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumRoutine()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::EnumRoutine()




//===========================================================================
// CtIDirectMusicScript::EnumVariable()
//
// Encapsulates calls to EnumVariable
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::EnumVariable
(
    DWORD dwIndex,
    WCHAR *pwszName
)
{
    HRESULT hRes            = E_FAIL;
    BOOL    fValid_pwszName = TRUE;

    // validate pwszRoutineName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwszName = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumVariable()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex  == %08Xh", dwIndex);
    fnsLog(PARAMLOGLEVEL, "pwszName == %s (%p)   %s",
            pwszName, pwszName, fValid_pwszName ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->EnumVariable(dwIndex,pwszName);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumVariable()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::EnumVariable()



*/


//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================








//===========================================================================
// CtIDirectMusicScript::SetVariableString()
//
// Encapsulates calls to SetVariableString
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::SetVariableString
(
    CHAR                   *pszVariableName,
    CHAR                   *pszValue,
    DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
)
{
    HRESULT         hRes                    = E_FAIL;
    BOOL            fValid_pszVariableName = TRUE;
    BOOL            fValid_pszValue        = TRUE;
    BOOL            fValid_pdmScriptErr    = TRUE;

    DMUS_SCRIPT_ERRORINFO       dmScriptErrorInfo;

    // validate pszVariableName
    if(!helpIsValidReadPtr((void*)pszVariableName, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pszVariableName = FALSE;
    }

    // validate pszValue
    if(!helpIsValidReadPtr((void*)pszValue, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pszValue = FALSE;
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetVariableString()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pszVariableName == %s (%p)   %s",
            pszVariableName, pszVariableName, fValid_pszVariableName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pszValue == %s (%p)   %s",
            pszValue, pszValue, fValid_pszValue ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->SetVariableString(pszVariableName, pszValue, &dmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetVariableString()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",tdmXlatHRESULT(hRes), hRes);
    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::SetVariableString()






//===========================================================================
// CtIDirectMusicScript::GetVariableString()
//
// Encapsulates calls to GetVariableString
//
// NOTE:  We're passing in pdmScriptErrorInfo as a pointer and nota ptr->ptr
//        because the Init API fills in this structure, changing no ptrs.
//        If the API changes, we may have to  modify this scheme.
//
// History:
//  10/26/1999 - kcraven - created
//  02/17/2000 - jimmo   - modified to remove IDMScriptError
//===========================================================================
HRESULT CtIDirectMusicScript::GetVariableString
(
    CHAR                   *pszVariableName,
    CHAR                   *pszValue,
    LONG                    lLength,
    LONG                   *plConverted,
    DMUS_SCRIPT_ERRORINFO  *pdmScriptErrorInfo
)
{
    HRESULT         hRes                   = E_FAIL;
    BOOL            fValid_pszVariableName = TRUE;
    BOOL            fValid_pszValue        = TRUE;
    BOOL            fValid_plConverted     = TRUE;
    BOOL            fValid_pdmScriptErr    = TRUE;

    DMUS_SCRIPT_ERRORINFO       dmScriptErrorInfo;



    // validate pszVariableName
    if(!helpIsValidReadPtr((void*)pszVariableName, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pszVariableName = FALSE;
    }

    // validate pszValue
    if(!helpIsValidPtr((void*)pszValue, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pszValue = FALSE;
    }

    // validate plConverted
    if(!helpIsValidPtr((void*)plConverted, sizeof(CHAR*), FALSE))
    {
        // bogus pointer, use as such
        fValid_plConverted = FALSE;
    }

    // validate DMUS_SCRIPT_ERRORINFO
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO*), FALSE))
    {
        fValid_pdmScriptErr = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetVariableString()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pszVariableName == %s (%p)   %s",
            pszVariableName, pszVariableName, fValid_pszVariableName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pszValue == %p %s",
            pszValue, fValid_pszValue ? "" : "(BAD)");
    fnsLog(PARAMLOGLEVEL, "lLength == %d", lLength);

    fnsLog(PARAMLOGLEVEL, "plConverted == %p %s",
            plConverted, fValid_plConverted ? "" : "(BAD)");

    fnsLog(PARAMLOGLEVEL, "pdmScriptErrorInfo == %p   %s",
            pdmScriptErrorInfo, fValid_pdmScriptErr ? "" : "BAD");

    if (TRUE == fValid_pdmScriptErr)
    {
        dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);
    }

    // call the real function
    hRes = ((IDirectMusicScript*)m_pUnk)->GetVariableString(pszVariableName, pszValue, lLength, plConverted, &dmScriptErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetVariableString()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",tdmXlatHRESULT(hRes), hRes);
    
    if (fValid_pszValue)
        fnsLog(PARAMLOGLEVEL, "pszValue = %s", pszValue);

    if (fValid_plConverted)
        fnsLog(PARAMLOGLEVEL, "plConverted = %d", plConverted);

    dmthLogDMUS_SCRIPT_ERRORINFO(CALLLOGLEVEL, pdmScriptErrorInfo);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScript::GetVariableString()

