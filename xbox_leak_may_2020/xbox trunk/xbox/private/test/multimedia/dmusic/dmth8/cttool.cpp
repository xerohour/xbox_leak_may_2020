//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       cttool.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// cttool.cpp
//
// Test harness implementation of IDirectMusicTool
//
// Functions:
//  CtIDirectMusicTool::CtIDirectMusicTool()
//  CtIDirectMusicTool::~CtIDirectMusicTool()
//  CtIDirectMusicTool::InitTestClass()
//  CtIDirectMusicTool::GetRealObjPtr()
//  CtIDirectMusicTool::QueryInterface()
//  CtIDirectMusicTool::Init()
//  CtIDirectMusicTool::GetMsgDeliveryType()
//  CtIDirectMusicTool::GetMediaTypeArraySize()
//  CtIDirectMusicTool::GetMediaTypes()
//  CtIDirectMusicTool::ProcessPMsg()
//  CtIDirectMusicTool::Flush()
//
// History:
//  01/02/1998 - a-llucar - created
//  01/09/1998 - a-llucar - adapted ProcessEvent(), Flush(), Start(), 
//                          and Stop() to latest header
//  01/10/1998 - a-llucar - added GetQueueType(), GetMediaTypeArraySize(), 
//                          GetMediaTypeArray()
//  03/05/1998 - davidkl - updated to latest dmusic architecture
//  03/31/1998 - davidkl - inheritance update
//===========================================================================
#include "globals.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicTool::CtIDirectMusicTool()
//
// Default constructor
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicTool::CtIDirectMusicTool(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicTool";

} //*** end CtIDirectMusicTool::CtIDirectMusicTool()


//===========================================================================
// CtIDirectMusicTool::~CtIDirectMusicTool()
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
CtIDirectMusicTool::~CtIDirectMusicTool(void)
{
    // nothing to do

} //*** end CtIDirectMusicTool::~CtIDirectMusicTool()


//===========================================================================
// CtIDirectMusicTool::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmTool - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicTool::InitTestClass(IDirectMusicTool *pdmTool)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmTool, sizeof(IDirectMusicTool), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmTool));

} //*** end CtIDirectMusicTool::InitTestClass()


//===========================================================================
// CtIDirectMusicTool::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicTool **ppdmTool - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/02/1998 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicTool::GetRealObjPtr(IDirectMusicTool **ppdmTool)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmTool, sizeof(IDirectMusicTool*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmTool));

} //*** end CtIDirectMusicTool::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicTool::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicTool::QueryInterface(REFIID riid, 
												LPVOID *ppvObj)
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

} //*** end CtIDirectMusicTool::QueryInterface


//===========================================================================
// CtIDirectMusicTool::AddRef()
//
// Encapsulates calls to AddRef
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
/*
DWORD CtIDirectMusicTool::AddRef(void)
{

    // increment our object's refcount
    m_dwRefCount++;

    // call the real AddRef
    tdmAddRef((IUnknown*)m_pdmTool, (LPCTSTR)m_szInterfaceName);

    // return >our< refcount
    return m_dwRefCount;

} //*** end CtIDirectMusicTool::AddRef()
*/	

//===========================================================================
// CtIDirectMusicTool::Release()
//
// Encapsulates calls to Release
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
/*
DWORD CtIDirectMusicTool::Release(void)
{

    // decrement our object's refcount
    m_dwRefCount--;

    // call the real Release
    tdmRelease((IUnknown*)m_pdmTool, (LPCTSTR)m_szInterfaceName);

    // clean ourselves up
    if(0 >= m_dwRefCount)
    {
        delete this;
    }

    // return >our< refcount
    return m_dwRefCount;

} //*** end CtIDirectMusicTool::Release()
*/

 
//===========================================================================
// CtIDirectMusicTool::Init()
//
// Encapsulates calls to Init
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/1998 - davidkl - modified to use wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicTool::Init(CtIDirectMusicGraph* ptdmGraph)
{
    HRESULT             hRes			    = E_NOTIMPL;
    BOOL                fValid_ptdmGraph    = TRUE;
    IDirectMusicGraph   *pdmGraph           = NULL;
    
    // validate ptdmGraph
    if(!helpIsValidPtr((void*)ptdmGraph, sizeof(CtIDirectMusicGraph), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmGraph = FALSE;
        pdmGraph = (IDirectMusicGraph*)ptdmGraph;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmGraph->GetRealObjPtr(&pdmGraph);
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pGraph == %p   %s",
            pdmGraph,
            fValid_ptdmGraph ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTool*)m_pUnk)->Init(pdmGraph);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmGraph && pdmGraph)
    {
        pdmGraph->Release();
    }
    return hRes;

} //*** end CtIDirectMusicTool::Init()


//===========================================================================
// CtIDirectMusicTool::GetMsgDeliveryType()
//
// Encapsulates calls to GetMsgDeliveryType
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicTool::GetMsgDeliveryType(DWORD* pdwDeliveryType)
{
    HRESULT hRes					= E_NOTIMPL;
    BOOL    fValid_pdwDeliveryType	= TRUE;
    
    // validate pdwQueueType
    if(!helpIsValidPtr((void*)pdwDeliveryType, sizeof(DWORD), FALSE))
    {
        fValid_pdwDeliveryType	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetMsgDeliveryType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwDeliveryType == %p   %s",
            pdwDeliveryType,
            fValid_pdwDeliveryType ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTool*)m_pUnk)->GetMsgDeliveryType(pdwDeliveryType);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetMsgDeliveryType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes             == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwDeliveryType)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwDeliveryType == %08Xh",
                *pdwDeliveryType);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTool::GetMsgDeliveryType()


//===========================================================================
// CtIDirectMusicTool::GetMediaTypeArraySize()
//
// Encapsulates calls to GetMediaTypeArraySize
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicTool::GetMediaTypeArraySize(DWORD* pdwNumElements)
{
    HRESULT hRes					= E_NOTIMPL;
    BOOL	fValid_pdwNumElements	= TRUE;
    
    // validate pdwNumElements
    if(!helpIsValidPtr((void*)pdwNumElements, sizeof(DWORD), FALSE))
    {
        fValid_pdwNumElements = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetMediaTypeArraySize()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwNumElements == %p   %s",
            pdwNumElements,
            fValid_pdwNumElements ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTool*)m_pUnk)->GetMediaTypeArraySize(pdwNumElements);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetMediaTypeArraySize()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwNumElements)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwNumElements == %08Xh",
                *pdwNumElements);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTool::GetMediaTypeArraySize()


//===========================================================================
// CtIDirectMusicTool::GetMediaTypes()
//
// Encapsulates calls to GetMediaTypes
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/2998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicTool::GetMediaTypes(DWORD** padwMediaTypes, 
                                        DWORD dwNumElements)
{
    HRESULT hRes					= E_NOTIMPL;
	BOOL	fValid_padwMediaTypes	= TRUE;
    
    // validate padwMediaTypes
    if(!helpIsValidPtr((void*)padwMediaTypes, sizeof(DWORD*), FALSE))
    {
        fValid_padwMediaTypes = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetMediaTypes()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "padwMediaTypes == %p   %s",
            padwMediaTypes,
            fValid_padwMediaTypes ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwNumElements == %08Xh",
            dwNumElements);

    // call the real function
    hRes = ((IDirectMusicTool*)m_pUnk)->GetMediaTypes(padwMediaTypes, dwNumElements);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetMediaTypes()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_padwMediaTypes)
    {
        // BUGBUG - log contents of padwMediaTypes @ MAXLOGLEVEL
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTool::GetMediaTypes()


//===========================================================================
// CtIDirectMusicTool::ProcessPMsg()
//
// Encapsulates calls to ProcessPMsg
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/1998 - davidkl - renamed, now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicTool::ProcessPMsg(CtIDirectMusicPerformance* ptdmPerf, 
                                        DMUS_PMSG* pPMSG)
{
    HRESULT                 hRes			= E_NOTIMPL;
    BOOL				    fValid_ptdmPerf	= TRUE;
	BOOL				    fValid_pPMSG	= TRUE;
    IDirectMusicPerformance *pdmPerf        = NULL;
    
    // validate ptdmPerf
    if(!helpIsValidPtr((void*)ptdmPerf, sizeof(CtIDirectMusicPerformance), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmPerf = FALSE;
        pdmPerf = (IDirectMusicPerformance*)ptdmPerf;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmPerf->GetRealObjPtr(&pdmPerf);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pPMSG
    if(!helpIsValidPtr((void*)pPMSG, sizeof(pPMSG), FALSE))
    {
        fValid_pPMSG = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ProcessPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPerf == %p   %s",
            pdmPerf,
            fValid_ptdmPerf ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pPMSG == %p   %s",
            pPMSG,
            fValid_pPMSG ? "" : "BAD");
    if(fValid_pPMSG)
    {
        // BUGBUG - log contents of pPMSG @ STRUCTLOGLEVEL
    }

    // call the real function
    hRes = ((IDirectMusicTool*)m_pUnk)->ProcessPMsg(pdmPerf, pPMSG);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ProcessPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmPerf && pdmPerf)
    {
        pdmPerf->Release();
    }
    return hRes;

} //*** end CtIDirectMusicTool::ProcessPMsg()


//===========================================================================
// CtIDirectMusicTool::Flush()
//
// Encapsulates calls to Flush
//
// History:
//  12/31/1997 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//  04/10/1998 - davidkl - added rtTime param
//===========================================================================
HRESULT CtIDirectMusicTool::Flush(CtIDirectMusicPerformance* ptdmPerf, 
                                    DMUS_PMSG* pPMSG,
                                    REFERENCE_TIME rtTime)
{
    HRESULT                 hRes			= E_NOTIMPL;
    BOOL				    fValid_ptdmPerf	= TRUE;
	BOOL				    fValid_pPMSG	= TRUE;
    IDirectMusicPerformance *pdmPerf        = NULL;
    
    // validate ptdmPerf
    if(!helpIsValidPtr((void*)ptdmPerf, sizeof(CtIDirectMusicPerformance), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmPerf = FALSE;
        pdmPerf = (IDirectMusicPerformance*)ptdmPerf;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmPerf->GetRealObjPtr(&pdmPerf);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pPipelineEvent
    if(!helpIsValidPtr((void*)pPMSG, sizeof(pPMSG), FALSE))
    {
        fValid_pPMSG = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Flush()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPerf  == %p   %s",
            pdmPerf,
            fValid_ptdmPerf ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pPMSG  == %p   %s",
            pPMSG,
            fValid_pPMSG ? "" : "BAD");
    if(fValid_pPMSG)
    {
        // BUGBUG - log contents of pPMSG @ STRUCTLOGLEVEL
    }
    fnsLog(PARAMLOGLEVEL, "rtTime == %016Xh",
            rtTime);

    // call the real function
    hRes = ((IDirectMusicTool*)m_pUnk)->Flush(pdmPerf, pPMSG, rtTime);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Flush()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmPerf && pdmPerf)
    {
        pdmPerf->Release();
    }
    return hRes;

} //*** end CtIDirectMusicTool::Flush()


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================












