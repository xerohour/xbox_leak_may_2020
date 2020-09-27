//NONSHIP

/////////////////////////////////////////////////////////////////////////////
// IElementBehaviorFactoryImpl
#define DECLARE_INTERNAL_CREATE(x) public:\
	typedef CComInternalCreator< CComObject<x> > _CreatorClass;

struct _ATL_BEHAVIOR_ENTRY
{
	LPCWSTR wszTag;
	_ATL_CREATORFUNC* pfnCreateInstance;
};

#define BEGIN_BEHAVIOR_MAP()\
static const _ATL_BEHAVIOR_ENTRY *GetBehaviorMap() {\
static const _ATL_BEHAVIOR_ENTRY pMap[] = {
#define END_BEHAVIOR_MAP() { NULL, NULL } }; return pMap; }
#define BEHAVIOR_ENTRY(tagName, class) { tagName, class::_CreatorClass::CreateInstance },

template <class T>
class CHandleBehaviorEvents: public CDHtmlEventSink
{
public:

	void ConnectElementEvents(IHTMLElement *pElem)
	{
		
		if (pElem)
		{
			CComPtr<IDispatch> spDisp;
			pElem->QueryInterface(&spDisp);
			if (spDisp)
			{
				m_pHandler =  new CDHtmlElementEventSink(this, spDisp);
				if (m_pHandler)
					m_pHandler->Advise((LPUNKNOWN)spDisp, __uuidof(HTMLElementEvents));
			}
		}

	}

	void DisconnectElementEvents(IHTMLElement *pElem)
	{
		T *pT = (T*)this;

		if (m_pHandler)
		{
			m_pHandler->UnAdvise((LPUNKNOWN)pElem, __uuidof(HTMLElementEvents));
			delete m_pHandler;
			m_pHandler = NULL;
		}
	}

	//Because this class inherits from CDHtmlEventSink we have to implement this method
	//in order for the HTML element event mapping mechanism to work. Just get the document
	//from the element which we are providing a behavior for.
	HRESULT GetDHtmlDocument(IHTMLDocument2 **ppDoc)
	{
		if (ppDoc == NULL)
			return E_POINTER;
		*ppDoc = NULL;

		T *pT = (T*)this;

		HRESULT hr = E_FAIL;
		if (pT->m_spElement)
		{
			CComPtr<IDispatch> spDispDoc;
			hr = pT->m_spElement->get_document(&spDispDoc);
			if (S_OK == hr)
			{
				hr = spDispDoc->QueryInterface(ppDoc);
			}
		}
		return hr;

	}

	CDHtmlElementEventSink *m_pHandler; //our event handler

};
template <class T>
class ATL_NO_VTABLE IElementBehaviorFactoryImpl : public IElementBehaviorFactory
{
public:
	// IElementBehaviorFacotry methods
    HRESULT STDMETHODCALLTYPE FindBehavior( BSTR bstrBehavior,BSTR bstrBehaviorUrl,
           IElementBehaviorSite *pSite,IElementBehavior **ppBehavior)
	{
		if (ppBehavior == NULL)
			return E_POINTER;
		*ppBehavior = NULL;
		T *pT = static_cast<T*>(this);
		const _ATL_BEHAVIOR_ENTRY *pMap = pT->GetBehaviorMap();
		CComBSTR szBehavior;
		if (bstrBehavior == NULL || *bstrBehavior == NULL)
		{
			if (pSite)
			{
				CComPtr<IHTMLElement> spElem;
				pSite->GetElement(&spElem);
				if (spElem)
				{
					spElem->get_tagName(&szBehavior);
				}
			}

		}
		else
			szBehavior = bstrBehavior;

		if (!szBehavior)
			return E_FAIL;
		while (pMap && pMap->wszTag)
		{	
			if (!wcscmp(szBehavior, pMap->wszTag))
			{
				// found
				return pMap->pfnCreateInstance(NULL, __uuidof(IElementBehavior), (void **) ppBehavior);
			}
			pMap++;
		}
		// not found
		return E_FAIL;
	}

};


template <class T>
class ATL_NO_VTABLE IElementBehaviorImpl : 
	public IElementBehavior

{
public:
	CComPtr<IElementBehaviorSite> m_spBehaveSite;
	CComPtr<IHTMLElement> m_spElement;

// IElementBehavior methods
    STDMETHOD(Init)(IElementBehaviorSite *pBehaviorSite)
	{
		T* pT = (T*)this;

		pBehaviorSite->GetElement(&m_spElement);
		m_spBehaveSite = pBehaviorSite;
		
		pT->RegisterElementEvents();
		return S_OK;
	}
    
    STDMETHOD(Notify)(LONG dwEvent, VARIANT *pVar)
	{
		T* pT = (T*)this;

		switch(dwEvent)
		{
		case BEHAVIOREVENT_CONTENTREADY:
			pT->OnContentReady();
			break;
		case BEHAVIOREVENT_DOCUMENTREADY:
			pT->OnDocumentReady();
			break;
		case BEHAVIOREVENT_APPLYSTYLE:
			pT->OnApplyStyle();
			break;
		case BEHAVIOREVENT_DOCUMENTCONTEXTCHANGE:
			pT->OnDocumentContextChange();
			break;
		}
		return S_OK;
	}

	STDMETHOD(Detach)()
	{
		return S_OK;
	}

	void OnContentReady()
	{
	}

	void OnDocumentReady()
	{
	}

	void OnApplyStyle()
	{
	}

	void OnDocumentContextChange()
	{
	}

	void RegisterElementEvents()
	{
	}





};

template <class T>
class ATL_NO_VTABLE IElementBehaviorRenderImpl : 
	public IElementBehaviorRender
{
public:
	DWORD m_dwBehaviorRenderFlags;
	IElementBehaviorRenderImpl()
	{
		m_dwBehaviorRenderFlags = BEHAVIORRENDERINFO_AFTERCONTENT;
	}

// IElementBehaviorRender methods
    HRESULT STDMETHODCALLTYPE Draw(HDC hdc, LONG dwLayer, LPRECT prc, IUnknown *pReserved)
	{
		T* pT = static_cast<T*>(this);
		return pT->DrawBehavior(hdc, dwLayer, prc, pReserved);
	}
        
    HRESULT STDMETHODCALLTYPE GetRenderInfo(LONG  *pRenderInfo)
	{
		*pRenderInfo = m_dwBehaviorRenderFlags;
		return S_OK;
	}

	 HRESULT STDMETHODCALLTYPE HitTestPoint(POINT  *pPoint,
            IUnknown  *pReserved, BOOL  *pbHit)
	 {
		return S_OK;
	 }
       
};
