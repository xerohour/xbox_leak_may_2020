//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctgraph.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctgraph.cpp
//
// Test harness implementation of IDirectMusicGraph
//
// Functions:
//  CtIDirectMusicGraph::CtIDirectMusicGraph()
//  CtIDirectMusicGraph::~CtIDirectMusicGraph()
//  CtIDirectMusicGraph::InitTestClass()
//  CtIDirectMusicGraph::GetRealObjPtr()
//  CtIDirectMusicGraph::QueryInterface()
//  CtIDirectMusicGraph::StampPMsg()
//  CtIDirectMusicGraph::InsertTool()
//  CtIDirectMusicGraph::GetTool()
//  CtIDirectMusicGraph::RemoveTool()
//
// History:
//  01/02/1998 - a-llucar - created
//  02/11/1998 - a-llucar - added InsertTool(), Shutdown() 
//  03/31/1998 - davidkl - inheritance update
//  09/26/1998 - a-llucar - removed Shutdown()
//===========================================================================

#include "globals.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicGraph::CtIDirectMusicGraph()
//
// Default constructor
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicGraph::CtIDirectMusicGraph(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicGraph";

} //*** end CtIDirectMusicGraph::CtIDirectMusicGraph()


//===========================================================================
// CtIDirectMusicGraph::~CtIDirectMusicGraph()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicGraph::~CtIDirectMusicGraph(void)
{
    // nothing to do

} //*** end CtIDirectMusicGraph::~CtIDirectMusicGraph()


//===========================================================================
// CtIDirectMusicGraph::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmGraph - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicGraph::InitTestClass(IDirectMusicGraph *pdmGraph)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmGraph, sizeof(IDirectMusicGraph), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmGraph));

} //*** end CtIDirectMusicGraph::InitTestClass()


//===========================================================================
// CtIDirectMusicGraph::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicGraph **ppdmGraph - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/02/1998 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicGraph::GetRealObjPtr(IDirectMusicGraph **ppdmGraph)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmGraph, sizeof(IDirectMusicGraph*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmGraph));

} //*** end CtIDirectMusicGraph::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicGraph::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicGraph::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicGraph::QueryInterface


//===========================================================================
// CtIDirectMusicGraph::StampPMsg()
//
// Encapsulates calls to StampPMsg
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicGraph::StampPMsg(DMUS_PMSG* pPMSG)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL	fValid_pPMSG    = TRUE;
    
    // validate pPipelineEvent
    if(!helpIsValidPtr((void*)pPMSG, sizeof(DMUS_PMSG), FALSE))
    {
        fValid_pPMSG	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::StampPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPMSG == %p   %s",
            pPMSG,
            fValid_pPMSG ? "" : "BAD");
    if(fValid_pPMSG)
    {
        // BUGBUG - log contents of pPMSG @ STRUCTLOGLEVEL
    }

    // call the real function
    hRes = ((IDirectMusicGraph*)m_pUnk)->StampPMsg(pPMSG);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::StampPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicGraph::StampPMsg()


//===========================================================================
// CtIDirectMusicGraph::InsertTool()
//
// Encapsulates calls to InsertTool
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicGraph::InsertTool(CtIDirectMusicTool* ptdmTool,
									    DWORD* pdwPChannels, 
                                        DWORD cPChannel,
										LONG lIndex)
{
    HRESULT             hRes				= E_NOTIMPL;
    BOOL				fValid_ptdmTool		= TRUE;
    BOOL				fValid_pdwPChannels	= TRUE;
    IDirectMusicTool    *pdmTool            = NULL;

    // validate ptdmTool
    if(!helpIsValidPtr((void*)ptdmTool, sizeof(CtIDirectMusicTool), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmTool = FALSE;
        pdmTool = (IDirectMusicTool*)ptdmTool;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmTool->GetRealObjPtr(&pdmTool);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pdwPChannels
    if(!helpIsValidPtr((void*)pdwPChannels, sizeof(DWORD), FALSE))
    {
        fValid_pdwPChannels	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::InsertTool()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pTool        == %p   %s",
            pdmTool,
            fValid_ptdmTool ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdwPChannels == %p   %s",
            pdwPChannels,
            fValid_pdwPChannels ? "" : "BAD");
    if(fValid_pdwPChannels)
    {
        // BUGBUG log contents of pdwPChannels array @ MAXLOGLEVEL
    }
    fnsLog(PARAMLOGLEVEL, "cPChannel    == %08Xh",
            cPChannel);
    fnsLog(PARAMLOGLEVEL, "lIndex       == %08Xh",
            lIndex);

    // call the real function
    hRes = ((IDirectMusicGraph*)m_pUnk)->InsertTool(pdmTool, pdwPChannels, 
                                cPChannel, lIndex);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::InsertTool()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmTool && pdmTool)
    {
        pdmTool->Release();
    }    
    return hRes;

}
//*** end CtIDirectMusicGraph::InsertTool()


//===========================================================================
// CtIDirectMusicGraph::GetTool()
//
// Encapsulates calls to GetTool
//
// History:
//  04/03/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicGraph::GetTool(DWORD dwIndex, 
                                    CtIDirectMusicTool **pptdmTool)
{
    HRESULT             hRes                = E_NOTIMPL;
    BOOL                fValid_pptdmTool    = TRUE;
    IDirectMusicTool    *pdmTool            = NULL;
    IDirectMusicTool    **ppdmTool          = NULL;

    //validate pptdmTool
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicTool pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicGraph::GetTool().  Otherwise, we are to create and
    //  return a CtIDirectMusicTool object
    if(!helpIsValidPtr((void*)pptdmTool, sizeof(CtIDirectMusicTool*),
        FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmTool = FALSE;
        ppdmTool = (IDirectMusicTool**)pptdmTool;

    }
    else
    {
        // valid pointer, create a real object
        ppdmTool = &pdmTool;

        // just in case we fail, init test object ptr to NULL
        *pptdmTool = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetTool()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "ppTool  == %p   %s",
            ppdmTool,
            fValid_pptdmTool ? "" : "BAD");

    // call the real thing
    hRes = ((IDirectMusicGraph*)m_pUnk)->GetTool(dwIndex, ppdmTool);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetTool()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes    == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(pptdmTool)
    {
        fnsLog(PARAMLOGLEVEL, "*ppTool == %08Xh",
                pdmTool);
    }

    //create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmTool, pptdmTool);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    //done
    if(fValid_pptdmTool && pdmTool)
    {
        pdmTool->Release();
    }
    return hRes;

} //*** end CtIDirectMusicGraph::GetTool()


//===========================================================================
// CtIDirectMusicGraph::RemoveTool()
//
// Encapsulates calls to RemoveTool
//
// History:
//  04/03/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicGraph::RemoveTool(CtIDirectMusicTool *ptdmTool)
{
    HRESULT             hRes            = E_NOTIMPL;
    BOOL                fValid_ptdmTool = TRUE;
    IDirectMusicTool    *pdmTool        = NULL;

    // validate ptdmTool
    if(!helpIsValidPtr((void*)ptdmTool, sizeof(CtIDirectMusicTool), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmTool = FALSE;
        pdmTool = (IDirectMusicTool*)ptdmTool;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmTool->GetRealObjPtr(&pdmTool);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::RemoveTool()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pTool == %p   %s",
            pdmTool,
            fValid_ptdmTool ? "" : "BAD");

    // call the real thing
    hRes = ((IDirectMusicGraph*)m_pUnk)->RemoveTool(pdmTool);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::RemoveTool()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    //done
END:
    if(fValid_ptdmTool && pdmTool)
    {
        pdmTool->Release();
    }
    return hRes;

} //*** end CtIDirectMusicGraph::RemoveTool()


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






