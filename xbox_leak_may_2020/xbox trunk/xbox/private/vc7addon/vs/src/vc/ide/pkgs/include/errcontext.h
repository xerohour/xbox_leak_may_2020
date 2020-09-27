// classes in this header
class CBldErrorContext;

#include <vcpb2.h>

/////////////////////////////////////////////////////////////////////////////
//	CBldErrorContext
//
//	Class for error contexts.  These are things you pass to a function to that
//	it can record any error messages it generates.  Thus a typical example is
//
//		BOOL SomeFunc ( argc ... , IVCBuildErrorContext* pEC = NULL );
//
//	The purpose of these things is to keep messages for the user, not to save
//	information for calling functions to examine.  Thus you should not examine
//	the contents of an EC to figure out what happened, rely on return codes or
//	whatever instead.

class CBldErrorContext :
	public IDispatchImpl<IVCBuildErrorContext, &IID_IVCBuildErrorContext, &LIBID_VCProjectEngineLibrary>, 
	public CComObjectRoot
{	 
public:
	CBldErrorContext() {}
	virtual ~CBldErrorContext()	{}
	static HRESULT CreateInstance(IVCBuildErrorContext** ppContext, CBldErrorContext** ppContextObj)
	{
		if (ppContext)
			*ppContext = NULL;
		if (ppContextObj)
			*ppContextObj = NULL;
		HRESULT hr;
		CBldErrorContext *pVar;
		CComObject<CBldErrorContext> *pObj;
		hr = CComObject<CBldErrorContext>::CreateInstance(&pObj);
		if (SUCCEEDED(hr))
		{
			pVar = pObj;
			pVar->AddRef();
			if (ppContext)
				*ppContext = pVar;
			if (ppContextObj)
				*ppContextObj = pObj;
		}
		return(hr);
	}

BEGIN_COM_MAP(CBldErrorContext)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCBuildErrorContext)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CBldErrorContext) 

// IVCBuildErrorContext
public:
	STDMETHOD(AddString)(/*[in]*/ BSTR bstrString) 	{ return S_OK; }
	STDMETHOD(AddLoggableString)(/*[in]*/ BSTR bstrString, /*[in]*/ int fFlags)	{ return S_OK; }
	STDMETHOD(AddLoggableStringFromID)(/*[in]*/ UINT nID, /*[in]*/ int fFlags) { return S_OK; }
	STDMETHOD(WriteLogString)(/*[in]*/ BSTR bstrString, /*[in]*/ VARIANT_BOOL bAddCtlLF) { return S_OK; }
	STDMETHOD(WriteLogStringFromID)(/*[in]*/ UINT nID) { return S_OK; }
	STDMETHOD(Close)() { return S_OK; }
	STDMETHOD(Reset)() { return S_OK; }	//	Clear the content of the context and prepared for a new, unrelated error:
	STDMETHOD(ActivateWindow)() { return S_OK; }
	STDMETHOD(ClearWindow)() { return S_OK; }
	STDMETHOD(AddErrorString)(/*[in]*/ BSTR bstrString) 	{ return S_OK; }
	STDMETHOD(AddWarningString)(/*[in]*/ BSTR bstrString, /*[in]*/ int fFlags)	{ return S_OK; }
	STDMETHOD(get_Warnings)( long *pnErr )
	{
		return S_OK;
	}
	STDMETHOD(get_Errors)( long *pnWrn )
	{
		return S_OK;
	}

protected:
	long m_nErrors;
	long m_nWarnings;


	//	Add a string to the ErrorConext.  Depending on the context, the string
	//	may or may not be displayed at this point:
	virtual void AddString(const TCHAR *pstr) {}
	virtual void AddString(UINT ResID) {}
	virtual void AddLoggableString(const TCHAR *pstr, int fFlags) {}
	virtual void AddLoggableString(UINT ResID, int fFlags) {}

	// Fill a string with all the messages in the context in the order in which
	// they we're added.  This is for _display_ only.  Classes the diplay 
	// messages as they occur may not implement this:
	virtual void GetString(CVCString &rstr) {}
	
	// Fill a string with all the messages in the context in the opposite order 
	// in which they we're added.  This should give a message of increasing 
	// specificity. For example:
	//
	//					Could not load document.
	//					Could not open file "blix.doc."
	//					Disk CRC error in sector la-de-da.
	// This is for _display_ only.  Classes the diplay messages as they occur 
	// may not implement this:
	virtual void GetRevString(CVCString &rstr) {}
};

