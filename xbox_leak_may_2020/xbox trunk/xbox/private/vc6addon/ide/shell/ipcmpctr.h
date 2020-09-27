// ipcmpctr.h : interface of the CIPCompContainerItem class
//

#ifndef __IPCMPCTR_H__
#define __IPCMPCTR_H__

#include <objext.h>
#include <ocdesign.h>

#ifndef __DOCOBCTR_H__
#include "docobctr.h"
#endif

#include "ipcfrmhk.h"

class CIPCompDoc;

//special value to mark undefined UI mode.	Detects when component
//fails to call SetUIMode before calling UIActivateForMe.
//
#define GRPID_UNDEFINED ((DWORD)(-1))

/////////////////////////////////////////////////////////////////////////////
// CIPCompContainerItem 

class CIPCompContainerItem : public CDocObjectContainerItem 
{
	friend CIPCompDoc;
	DECLARE_SERIAL(CIPCompContainerItem )

// Constructors
public:
	CIPCompContainerItem(COleDocument *pContainer = NULL);
	~CIPCompContainerItem();
	
	virtual BOOL OnCompCmdMsg(GUID * pguid, DWORD cmdid, DWORD rgf,
		int nCode, void* pExtra);
	virtual void OnActivateMe();

protected:
	DWORD m_fmodeComponentUI;	// the UI mode that the component is in
	COleRef<IOleComponentUIManager> m_srpCompUIMgr;
	COleRef<IBuilderWizardManager>  m_srpBuilderWizardManager;
	
	DECLARE_INTERFACE_MAP()

	// in addition, we need to implement IOleInPlaceComponentSite and IDispatch
	BEGIN_INTERFACE_PART(OleIPComponentSite, IOleInPlaceComponentSite)
		STDMETHOD(SetUIMode)(DWORD dwUIMode);
	END_INTERFACE_PART(OleIPComponentSite)
	
	BEGIN_INTERFACE_PART(AmbientDispatch, IDispatch)
		STDMETHOD(GetTypeInfoCount)(unsigned int * pctinfo);
		STDMETHOD(GetTypeInfo)(unsigned int itinfo,LCID lcid,ITypeInfo **pptinfo);
		STDMETHOD(GetIDsOfNames)(REFIID riid,LPOLESTR *rgszNames,
	      						unsigned int cNames,LCID lcid,
	      						DISPID *rgdispid);
		STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid,LCID lcid,
	      				  unsigned short wFlags,DISPPARAMS *pdispparams,
	      				  VARIANT *pvarResult,EXCEPINFO *pexcepinfo,
	      				  unsigned int *puArgErr);
	END_INTERFACE_PART(AmbientDispatch)

	BEGIN_INTERFACE_PART(MyOleClientSite, IOleClientSite)
		INIT_INTERFACE_PART(CIPCompContainerItem, OleClientSite)
		STDMETHOD(SaveObject)();
		STDMETHOD(GetMoniker)(DWORD, DWORD, LPMONIKER*);
		STDMETHOD(GetContainer)(LPOLECONTAINER*);
		STDMETHOD(ShowObject)();
		STDMETHOD(OnShowWindow)(BOOL);
		STDMETHOD(RequestNewObjectLayout)();
	END_INTERFACE_PART(MyOleClientSite)

	// the following won't be necessary when the MFC team uses a virtual function in the 
	// IOleInPlaceSite::GetWindowContext method to allow one to use a derived class of 
	// COleFrameHook.
	BEGIN_INTERFACE_PART(MyOleIPSite, IOleInPlaceSite)
		INIT_INTERFACE_PART(CIPCompContainerItem, OleIPSite)
		STDMETHOD(GetWindow)(HWND*);
		STDMETHOD(ContextSensitiveHelp)(BOOL);
		STDMETHOD(CanInPlaceActivate)();
		STDMETHOD(OnInPlaceActivate)();
		STDMETHOD(OnUIActivate)();
		STDMETHOD(GetWindowContext)(LPOLEINPLACEFRAME*,
			LPOLEINPLACEUIWINDOW*, LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO);
		STDMETHOD(Scroll)(SIZE);
		STDMETHOD(OnUIDeactivate)(BOOL);
		STDMETHOD(OnInPlaceDeactivate)();
		STDMETHOD(DiscardUndoState)();
		STDMETHOD(DeactivateAndUndo)();
		STDMETHOD(OnPosRectChange)(LPCRECT);
	END_INTERFACE_PART(MyOleIPSite)

	virtual void OnGetItemPosition(CRect& rPosition);

public:
	CIPCompDoc *GetDocument()
		{ return (CIPCompDoc *)CDocObjectContainerItem::GetDocument(); }
		
	IDispatch * CIPCompContainerItem::GetIDispatch();
	IOleInPlaceComponentSite * CIPCompContainerItem::GetIOleInPlaceComponentSite();
	IOleInPlaceSite* CIPCompContainerItem::GetIOleInPlaceSite();
	virtual HRESULT GetService(REFGUID guidService, REFIID riid, void **ppvObj);
	
	virtual HRESULT ActivateDocObjectView(CView *pView);
	virtual void OnDeactivate();
	virtual void DoClose();
	virtual BOOL CanClose();
	
	BOOL CreateNewFromClsid(CLSID clsid, IStorage * pStg);
	
//	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __IPCMPCTR_H__
