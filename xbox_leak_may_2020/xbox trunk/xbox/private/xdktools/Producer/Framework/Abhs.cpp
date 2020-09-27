// abhs.cpp
//
// Implements AllocBindHostService.
//
// Important: This .cpp file assumes a zero-initializing global "new" operator.
//
// doc MMCTL
//

#include "stdafx.h"
#include "HelpBindHost.h"


/* @func int | UNICODEToANSI |

        Converts a UNICODE string to ANSI.

@rdesc  Returns the same integer as WideCharToMultiByte.  0 means "failed."

@parm   LPSTR | pchDst | The buffer that will hold the output ANSI string.

@parm   LPCWSTR | pwchSrc | The input UNICODE string.  NULL is interpreted as
        a zero-length string.

@parm   int | cchDstMax | The size of <p pchDst>, in characters.  If <p pchDst>
		is declared as char pchDst[32], for example, <p cchDstMax> should be 32.
		If <p pchDst> isn't large enough to hold the ANSI string (including the
		terminating NULL), <p pchDst> is set to zero length and 0 is returned.
		(In debug versions, an assertion also occurs.)

@comm   If you want to determine the ANSI buffer size required for a given
		UNICODE string pwchSrc, you can call UNICODEToANSI(NULL, pwchSrc, 0).
		This returns the required buffer size in characters, including space
		for the terminating NULL.

@ex		Here is code (without debug checks) that dynamically allocates the ANSI
		buffer and converts the UNICODE string pwchSrc: |

			int cchDst;
			char *pchDst;
			cchDst = UNICODEToANSI(NULL, pwchSrc, 0);
			pchDst = new char [cchDst];
			UNICODEToANSI(pchDst, pwchSrc, cchDst)

*/
STDAPI_(int) UNICODEToANSI(LPSTR pchDst, LPCWSTR pwchSrc, int cchDstMax)
{
	// (We allow the caller to pass a cchDstMax value of 0 and a NULL pchDst to
	// indicate "tell me the buffer size I need, including the NULL.")

	ASSERT(pchDst != NULL || 0 == cchDstMax);
	ASSERT(cchDstMax >= 0);

	#ifdef _DEBUG

	// Make sure we won't exceed the length of the user-supplied buffer,
	// pchDst.  The following call returns the number of characters required to
	// store the converted string, including the terminating NULL.

    if(cchDstMax > 0)
	{
		int iChars;
	
		iChars =
		  	WideCharToMultiByte(CP_ACP, 0, pwchSrc ? pwchSrc : OLESTR(""),
							    -1, NULL, 0, NULL, NULL); 
		ASSERT(iChars <= cchDstMax);
	}

	#endif

	int iReturn;

	iReturn = WideCharToMultiByte(CP_ACP, 0, pwchSrc ? pwchSrc : OLESTR( "" ), 
								  -1, pchDst, cchDstMax, NULL, NULL); 

	if (0 == iReturn)
	{
		// The conversion failed.  Return an empty string.

		if (pchDst != NULL)
			pchDst[0] = 0;

		ASSERT(FALSE);
	}

	return (iReturn);
}

/* @func int | ANSIToUNICODE |

        Converts an ANSI string to UNICODE.

@parm   LPWSTR | pwchDst | The buffer that will hold the output UNICODE string.

@parm   LPCSTR | pchSrc | The input ANSI string.

@parm   int | cwchDstMax | The size of <p pwchDst>, in wide characters.  If
		pwchDst is declared as OLECHAR pwchDst[32], for example, cwchDstMax
		should be 32.

*/
STDAPI_(int) ANSIToUNICODE(LPWSTR pwchDst, LPCSTR pchSrc, int cwchDstMax)
{

	ASSERT( pwchDst );
	ASSERT( pchSrc );

    return MultiByteToWideChar(CP_ACP, 0, pchSrc, -1, pwchDst, cwchDstMax);
}


//////////////////////////////////////////////////////////////////////////////
// CBindHostServiceCallback
//
// Implements IBindStatusCallback.
//

struct CBindHostServiceCallback : IBindStatusCallback
{
///// general object state
    ULONG           m_cRef;         // object reference count
    IBindCtx *      m_pbc;          // context that this object is registered in
    IBindStatusCallback* m_pbsc;    // client-provided IBindStatusCallback
    IBinding *      m_pb;           // provided in OnStartBinding()

///// construction and destruction
    CBindHostServiceCallback(IBindStatusCallback *pbsc);
    ~CBindHostServiceCallback();
    void AttachBindCtx(IBindCtx *pbc);

///// IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

///// IBindStatusCallback methods
    STDMETHODIMP OnStartBinding(DWORD dwReserved, IBinding *pib);
    STDMETHODIMP GetPriority(LONG *pnPriority);
    STDMETHODIMP OnLowResource(DWORD reserved);
    STDMETHODIMP OnProgress(ULONG ulProgress, ULONG ulProgressMax,
        ULONG ulStatusCode, LPCWSTR szStatusText);
    STDMETHODIMP OnStopBinding(HRESULT hresult, LPCWSTR szError);
    STDMETHODIMP GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo);
    STDMETHODIMP OnDataAvailable(DWORD grfBSCF, DWORD dwSize,
        FORMATETC *pformatetc, STGMEDIUM *pstgmed);
    STDMETHODIMP OnObjectAvailable(REFIID riid, IUnknown *punk);
};


/////////////////////////////////////////////////////////////////////////////
// CBindHostServiceCallback Creation & Destruction
//

CBindHostServiceCallback::CBindHostServiceCallback(IBindStatusCallback *pbsc) :
	m_pb(NULL),
	m_pbc(NULL)
{
    TRACE("BindHostService 0x%08lx created\n", this);

    // initialize IUnknown
    m_cRef = 1;

    // other initialization
    m_pbsc = pbsc;
    if (m_pbsc != NULL)
        m_pbsc->AddRef();
}

CBindHostServiceCallback::~CBindHostServiceCallback()
{
    TRACE("BindHostService 0x%08lx destroyed\n", this);

    // cleanup
    if (m_pb != NULL)
        m_pb->Release();
    if (m_pbc != NULL)
        m_pbc->Release();
    if (m_pbsc != NULL)
        m_pbsc->Release();
}

void CBindHostServiceCallback::AttachBindCtx(IBindCtx *pbc)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    m_pbc = pbc;
    if (m_pbc != NULL)
        m_pbc->AddRef();
}


//////////////////////////////////////////////////////////////////////////////
// CBindHostServiceCallback IUnknown Implementation
//

STDMETHODIMP CBindHostServiceCallback::QueryInterface(REFIID riid, LPVOID *ppv)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    *ppv = NULL;

#ifdef _DEBUG
//    char ach[200];
//    TRACE("BindHostServiceCallback::QI('%s')\n", DebugIIDName(riid, ach));
#endif

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_IBindStatusCallback))
        *ppv = (IBindStatusCallback *) this;
    else
        return E_NOINTERFACE;

    ((IUnknown *) *ppv)->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) CBindHostServiceCallback::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CBindHostServiceCallback::Release()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if (--m_cRef == 0L)
    {
        // free the object
        delete this;
        return 0;
    }
    else
        return m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CBindHostServiceCallback IBindStatusCallback Methods
//

STDMETHODIMP CBindHostServiceCallback::OnStartBinding(DWORD dwReserved,
    IBinding *pb)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::OnStartBinding\n");

    // cache caller-provided IBinding
    if (m_pb != NULL)
        m_pb->Release();
    m_pb = pb;
    if (m_pb != NULL)
        m_pb->AddRef();

    if (m_pbsc != NULL)
        return m_pbsc->OnStartBinding(dwReserved, pb);
    else
        return S_OK;
}

STDMETHODIMP CBindHostServiceCallback::GetPriority(LONG *pnPriority)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::GetPriority\n");

    if (m_pbsc != NULL)
        return m_pbsc->GetPriority(pnPriority);
    else
        return E_FAIL;
}

STDMETHODIMP CBindHostServiceCallback::OnLowResource(DWORD reserved)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::OnLowResource\n");

    if (m_pbsc != NULL)
        return m_pbsc->OnLowResource(reserved);
    else
        return E_FAIL;
}

STDMETHODIMP CBindHostServiceCallback::OnProgress(ULONG ulProgress,
    ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::OnProgress(%u/%u, %d)\n",
        ulProgress, ulProgressMax, ulStatusCode);

#ifdef _DEBUG
    if (szStatusText != NULL)
    {
		int nLen = UNICODEToANSI(NULL, szStatusText, 0);
        char *ach = new char[nLen];
		if (NULL != ach)
		{
			UNICODEToANSI(ach, szStatusText, nLen);
			TRACE("...'%s'\n", ach);
			delete [] ach;
		}
    }
#endif

    if (m_pbsc != NULL)
        return m_pbsc->OnProgress(ulProgress, ulProgressMax, ulStatusCode,
            szStatusText);
    else
        return S_OK;
}

STDMETHODIMP CBindHostServiceCallback::OnStopBinding(HRESULT hresult,
    LPCWSTR szError)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::OnStopBinding\n");

    // free caller-provided IBinding
    if (m_pb != NULL)
        m_pb->Release();
    m_pb = NULL;

#ifdef _DEBUG
    if (szError != NULL)
    {
		int nLen = UNICODEToANSI(NULL, szError, 0);
        char *ach = new char[nLen];
		if (NULL != ach)
		{
			UNICODEToANSI(ach, szError, nLen);
			TRACE("...'%s'\n", ach);
			delete [] ach;
		}
    }
#endif

    // forward OnStopBinding(), but ignore return code
    if (m_pbsc != NULL)
    	m_pbsc->OnStopBinding(hresult, szError);

    // revoke ourselves from the IBindCtx we're registered in
    if (m_pbc != NULL)
    {
        TRACE("BindHostService: RevokeBindStatusCallback\n");
        HelpRevokeBindStatusCallback(m_pbc, this);
    }

    // don't use local variables anymore -- <this> may have been
    // deleted by RevokeBindStatusCallback() above
    return 0;
}

STDMETHODIMP CBindHostServiceCallback::GetBindInfo(DWORD *grfBINDF,
    BINDINFO *pbindinfo)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::GetBindInfo\n");

    if (m_pbsc != NULL)
        return m_pbsc->GetBindInfo(grfBINDF, pbindinfo);

    memset(pbindinfo, 0, pbindinfo->cbSize);
	pbindinfo->cbSize = sizeof(BINDINFO);
    *grfBINDF = BINDF_ASYNCHRONOUS;
    return S_OK;
}

STDMETHODIMP CBindHostServiceCallback::OnDataAvailable(DWORD grfBSCF,
    DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::OnDataAvailable (dwSize=%d)\n",
        dwSize);

    if (m_pbsc != NULL)
        return m_pbsc->OnDataAvailable(grfBSCF, dwSize, pformatetc, pstgmed);
    else
        return S_OK;
}

STDMETHODIMP CBindHostServiceCallback::OnObjectAvailable(REFIID riid,
    IUnknown *punk)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindStatusCallback::OnObjectAvailable\n");

    if (m_pbsc != NULL)
        return m_pbsc->OnObjectAvailable(riid, punk);
    else
        return S_OK;
}



//////////////////////////////////////////////////////////////////////////////
// CBindHostService
//
// Implements IServiceProvider, which provides an implementation of IBindHost
// to the embedded control.
//

struct CBindHostService : IServiceProvider, IBindHost
{
///// general object state
    ULONG           m_cRef;         // object reference count
    OLECHAR         m_aochContainerFileName[_MAX_PATH]; // files rel. to this

///// construction and destruction
    CBindHostService(LPCTSTR szContainerFileName, HRESULT *phr);

///// IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

///// IServiceProvider implementation
    STDMETHODIMP QueryService(REFGUID rsid, REFIID iid, void **ppvObj);

///// IBindHost methods
    STDMETHODIMP CreateMoniker(LPOLESTR szName, IBindCtx *pbc, IMoniker **ppmk,
         DWORD dwReserved);
    STDMETHODIMP MonikerBindToStorage(IMoniker *pmk, IBindCtx *pbc,
        IBindStatusCallback *pbsc, REFIID riid, void **ppvObj);
    STDMETHODIMP MonikerBindToObject(IMoniker *pmk, IBindCtx *pbc,
        IBindStatusCallback *pbsc, REFIID riid, void **ppvObj);
};


/////////////////////////////////////////////////////////////////////////////
// BindHostService Creation & Destruction
//

/* @func HRESULT | AllocBindHostService |

        Allocates an implementation of <i IServiceProvider> which provides
        the <i IBindHost> service (SID_IBindHost, IID_IBindHost), which
        provides for relative path name resolution when using functions
        such as <f URLOpenStream>.

@parm   LPCTSTR | szContainerFileName | The name of the file that
        relative path names will be relative to.  For example, if
        <p szContainerFileName> is "c:\\foo\\bar.htm" and the caller
        calls <f URLOpenStream> to open "blorg.gif", then the file
        "file://c:\\foo\\blorg.gif" will be opened (assuming that
        the <i IUnknown *> parameter of <f URLOpenStream> implements
        <i IServiceProvider> which delegates to *<p ppsp>).  If
        <p szContainerFileName> is NULL or "", then file names
        are assumed to be relative to the current directory.
        <p szContainerFileName> should be under _MAX_PATH characters
        in length.

@parm   IServiceProvider * * | ppsp | Where to store a pointer to the
        newly created object.  NULL is stored in *<p ppsp> on error.

@comm   Whenver the container's file name changes, the old <i IBindHost>
        servie provider should be freed and <f AllocBindHostService> should be
        called again to create a new <i IBindHost> service provider.
*/
STDAPI AllocBindHostService(LPCTSTR szContainerFileName,
    IServiceProvider **ppsp)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    HRESULT hr;
    // create the Windows object
    if ((*ppsp = (IServiceProvider *)
            new CBindHostService(szContainerFileName, &hr)) == NULL)
        return E_OUTOFMEMORY;
    if (FAILED(hr))
        (*ppsp)->Release();

    return hr;
}

CBindHostService::CBindHostService(LPCTSTR szContainerFileName, HRESULT *phr)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    // initialize IUnknown
    m_cRef = 1;

    // set <m_aochContainerFileName> to be the path to the currently-loaded
    // file (e.g. HTML file); if there is no HTML file loaded, then set
    // <m_aochContainerFileName> to the name of a hypothetical document "x"
    // in the current directory (so that files opened by e.g. URLOpenStream
    // will be relative to the current directory)
    if ((szContainerFileName == NULL) || (szContainerFileName[0] == 0))
    {
        // no file is loaded, so we'll act as though this file
        // (e.g. the HTML file) is saved in the current directory...

        // make <ach> contain the name of a hypothetical file "x"
        // within the current directory
        char ach[MAX_PATH];
        if (GetCurrentDirectory(sizeof(ach) - 1, ach) == 0)
            goto ERR_FAIL;
        if (ach[0] == 0)
            goto ERR_FAIL;
        char *pch = ach + lstrlen(ach);
        if ((pch[-1] != '\\') && (pch[-1] != ':'))
            lstrcat(ach, "\\x");

        // convert file path to Unicode, in <aochContainerFileName>
        ANSIToUNICODE(m_aochContainerFileName, ach,
            sizeof(m_aochContainerFileName) / 
                sizeof(*m_aochContainerFileName));
    }
    else
    {
        // convert <ach> to a fully qualified path
        OFSTRUCT of;
        if (OpenFile(szContainerFileName, &of, OF_PARSE) == HFILE_ERROR)
            goto ERR_FAIL;

        // convert file path to Unicode, in <aochContainerFileName>
        ANSIToUNICODE(m_aochContainerFileName, of.szPathName,
            sizeof(m_aochContainerFileName) /
                sizeof(*m_aochContainerFileName));
    }

    goto EXIT;

EXIT:

    *phr = S_OK;
    return;

ERR_FAIL:

    *phr = E_FAIL;
    return;
}


//////////////////////////////////////////////////////////////////////////////
// BindHostService IUnknown Implementation
//

STDMETHODIMP CBindHostService::QueryInterface(REFIID riid, LPVOID *ppv)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    *ppv = NULL;

#ifdef _DEBUG
//    char ach[200];
//    TRACE("BindHostService::QI('%s')\n", DebugIIDName(riid, ach));
#endif

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_IServiceProvider))
        *ppv = (IServiceProvider *) this;
    else
    if (IsEqualIID(riid, IID_IBindHost))
        *ppv = (IBindHost *) this;
    else
        return E_NOINTERFACE;

    ((IUnknown *) *ppv)->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) CBindHostService::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CBindHostService::Release()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if (--m_cRef == 0L)
    {
        // free the object
        delete this;
        return 0;
    }
    else
        return m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// BindHostService IServiceProvider Implementation
//

STDMETHODIMP CBindHostService::QueryService(REFGUID rsid, REFIID riid,
    void **ppvObj)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

#if 1
    if (!IsEqualGUID(rsid, SID_IBindHost))
        return E_FAIL; // should be SVC_E_UNKNOWNSERVICE, which I can't find

    return QueryInterface(riid, ppvObj);
#else
    return E_FAIL;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// BindHostService IBindHost Implementation
//

STDMETHODIMP CBindHostService::CreateMoniker(LPOLESTR szName, IBindCtx *pbc,
    IMoniker **ppmk, DWORD dwReserved)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    HRESULT         hrReturn = S_OK; // function return code
    CBindHostServiceCallback *pbscTmp = NULL; // for CreateAsyncBindCtx()
    IBindCtx *      pbcTmp = NULL;  // temporary bind context
    IMoniker *      pmkLeft = NULL; // left part of new moniker
    IMoniker *      pmkRight = NULL; // right part of new moniker
    ULONG           ul;

	#ifdef _DEBUG
	{
		int nLen = UNICODEToANSI(NULL, szName, 0);
        char *ach = new char[nLen];
		if (NULL != ach)
		{
			UNICODEToANSI(ach, szName, nLen);
			TRACE("BindHostService: IBindHost::CreateMoniker('%s')\n", ach);
			delete [] ach;
		}
	}
	#endif

    // ensure correct cleanup
    *ppmk = NULL;

    // if <pbc> is NULL, make it be a valid temporary IBindCtx
    if (pbc == NULL)
    {
        if ((pbscTmp = new CBindHostServiceCallback(NULL)) == NULL)
            goto ERR_OUTOFMEMORY;
        if (FAILED(hrReturn = HelpCreateAsyncBindCtx(0, pbscTmp, 0, &pbcTmp)))
            goto ERR_EXIT;
        pbc = pbcTmp;
    }

    // make <pmkLeft> refer to <m_aochContainerFileName>
    if (FAILED(hrReturn = HelpMkParseDisplayNameEx(pbc, m_aochContainerFileName,
            &ul, &pmkLeft)))
        goto ERR_EXIT;

#if 0
    if (FAILED(hrReturn = pbc->RegisterObjectParam(SZ_URLCONTEXT, pmkLeft)))
        goto ERR_EXIT;
#endif
#if 0

    // make <pmkRight> refer to <szName>
    if (FAILED(hrReturn = HelpMkParseDisplayNameEx(pbc, szName, &ul,
            &pmkRight)))
        goto ERR_EXIT;

    // make <*ppmk> refer to <pmkRight> relative to the directory
    // containing <pmkLeft>
    if (FAILED(hrReturn = pmkLeft->ComposeWith(pmkRight, FALSE, ppmk)))
        goto ERR_EXIT;

#else

    // make <*ppmk> refer to <pmkRight> relative to the directory
    // containing <pmkLeft>
    if (FAILED(hrReturn = HelpCreateURLMoniker(pmkLeft, szName, ppmk)))
        goto ERR_EXIT;

#endif

#ifdef _DEBUG
    // display information about <*ppmk>
    {
        LPOLESTR osz;
        if (SUCCEEDED((*ppmk)->GetDisplayName(pbc, NULL, &osz)))
        {
			int nLen = UNICODEToANSI(NULL, osz, 0);
			char *ach = new char[nLen];
			if (NULL != ach)
			{
				UNICODEToANSI(ach, osz, nLen);
	            TRACE("...display name: '%s'\n", ach);
				delete [] ach;
			}

            CoTaskMemFree(osz);
        }
    }
#endif

    goto EXIT;

ERR_OUTOFMEMORY:

    hrReturn = E_OUTOFMEMORY;
    goto ERR_EXIT;

ERR_EXIT:

    // error cleanup
    if (*ppmk != NULL)
    {
        (*ppmk)->Release();
        *ppmk = NULL;
    }
    goto EXIT;

EXIT:

    // normal cleanup
    if (pmkLeft != NULL)
        pmkLeft->Release();
    if (pmkRight != NULL)
        pmkRight->Release();
    if (pbcTmp != NULL)
        pbcTmp->Release();
    if (pbscTmp != NULL)
        pbscTmp->Release();

    return hrReturn;
}

STDMETHODIMP CBindHostService::MonikerBindToStorage(IMoniker *pmk,
    IBindCtx *pbc, IBindStatusCallback *pbsc, REFIID riid, void **ppvObj)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    HRESULT         hrReturn = S_OK; // function return code
    CBindHostServiceCallback *pbscTmp = NULL; // for CreateAsyncBindCtx()
    IBindCtx *      pbcTmp = NULL;  // temporary bind context

    TRACE("BindHostService: IBindHost::MonikerBindToStorage\n");

    // create an IBindStatusCallback to monitor the binding operation;
    // delegate to the caller-provided <pbsc>
    if ((pbscTmp = new CBindHostServiceCallback(pbsc)) == NULL)
        goto ERR_OUTOFMEMORY;

    // if <pbc> is NULL, make it be a valid temporary IBindCtx;
    // in any case, register <pbscTmp> with the IBindCtx
    if (pbc == NULL)
    {
        if (FAILED(hrReturn = HelpCreateAsyncBindCtx(0, pbscTmp, 0, &pbcTmp)))
            goto ERR_EXIT;
        pbc = pbcTmp;
    }
    else
    {
        if (FAILED(hrReturn = HelpRegisterBindStatusCallback(pbc, pbscTmp,
                /*BSCO_ALLONIBSC*/0, 0)))
            goto ERR_EXIT;
    }

    // tell <pbscTmp> that it's registered in <pbc>; <pbscTmp> will call
    // RevokeBindStatusCallback() in its OnStopBinding()
    pbscTmp->AttachBindCtx(pbc);

#ifdef _DEBUG
    // display information about <pmk>
    {
        LPOLESTR osz;
        if (SUCCEEDED(pmk->GetDisplayName(pbc, NULL, &osz)))
        {
			int nLen = UNICODEToANSI(NULL, osz, 0);
			char *ach = new char[nLen];
			if (NULL != ach)
			{
				UNICODEToANSI(ach, osz, nLen);
	            TRACE("...display name: '%s'\n", ach);
				delete [] ach;
			}

            CoTaskMemFree(osz);
        }
    }
#endif

    // bind to the caller-requested type of storage
    if (FAILED(hrReturn = pmk->BindToStorage(pbc, NULL, riid, ppvObj)))
        goto ERR_EXIT;

    goto EXIT;

ERR_OUTOFMEMORY:

    hrReturn = E_OUTOFMEMORY;
    goto ERR_EXIT;

ERR_EXIT:

    // error cleanup
    // (nothing to do)
    goto EXIT;

EXIT:

    // normal cleanup
    if (pbcTmp != NULL)
        pbcTmp->Release();
    if (pbscTmp != NULL)
        pbscTmp->Release();

    return hrReturn;
}

STDMETHODIMP CBindHostService::MonikerBindToObject(IMoniker *pmk,
    IBindCtx *pbc, IBindStatusCallback *pbsc, REFIID riid, void **ppvObj)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    TRACE("BindHostService: IBindHost::MonikerBindToObject: E_NOTIMPL"
        " --> this will case URL file access to fail!\n");
    return E_NOTIMPL;
}

