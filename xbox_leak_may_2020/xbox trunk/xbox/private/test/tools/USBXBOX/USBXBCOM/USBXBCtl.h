// USBXBCtl.h : Declaration of the CUSBXBCtl

#ifndef __USBXBCTL_H_
#define __USBXBCTL_H_

#include "resource.h"       // main symbols
#include <atlctl.h>
#include "..\IncLib\USBXBtransfer.h"
#include "utils.h"
#include "ansibstr.h"
#include "USBXBCOMCP.h"


#define MAKE_SPACE 0x01 // flag for spacinating input to client


/////////////////////////////////////////////////////////////////////////////
// CUSBXBCtl
class ATL_NO_VTABLE CUSBXBCtl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IUSBXBCtl, &IID_IUSBXBCtl, &LIBID_USBXBCOMLib>,
	public CComControl<CUSBXBCtl>,
	public IPersistStreamInitImpl<CUSBXBCtl>,
	public IOleControlImpl<CUSBXBCtl>,
	public IOleObjectImpl<CUSBXBCtl>,
	public IOleInPlaceActiveObjectImpl<CUSBXBCtl>,
	public IViewObjectExImpl<CUSBXBCtl>,
	public IOleInPlaceObjectWindowlessImpl<CUSBXBCtl>,
	public IPersistStorageImpl<CUSBXBCtl>,
	public ISpecifyPropertyPagesImpl<CUSBXBCtl>,
	public IQuickActivateImpl<CUSBXBCtl>,
	public IDataObjectImpl<CUSBXBCtl>,
	public IProvideClassInfo2Impl<&CLSID_USBXBCtl, &DIID__IUSBXBCtlEvents, &LIBID_USBXBCOMLib>,
	public CComCoClass<CUSBXBCtl, &CLSID_USBXBCtl>,
	public CProxy_IUSBXBCtlEvents< CUSBXBCtl >,
	public IConnectionPointContainerImpl<CUSBXBCtl>
{
public:
	CUSBXBCtl();
	~CUSBXBCtl();

DECLARE_REGISTRY_RESOURCEID(IDR_USBXBCTL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CUSBXBCtl)
	COM_INTERFACE_ENTRY(IUSBXBCtl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_PROP_MAP(CUSBXBCtl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_MSG_MAP(CUSBXBCtl)
	CHAIN_MSG_MAP(CComControl<CUSBXBCtl>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);



// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IUSBXBCtl
public:
	ULONG m_ulDataSizeIn;
	int ssSend(SOCKET socket, TRANSFERDATA * pSockData, int iSize);
	void Initialize();
	char m_cServerName[30];
	USHORT GetPort(char * service);
	ULONG GetAddr();
	SOCKET m_Socket;
	SOCKET ssConnectTCP();
	STDMETHOD(get_LastXError)(/*[out, retval]*/ int *pVal);
	int m_iLastError;
	BOOL m_fInited;
	SOCKET  m_hXBoxEventsSocket;
	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);

		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL 3.0 : USBXBCtl");
		TextOut(di.hdcDraw, 
			(rc.left + rc.right) / 2, 
			(rc.top + rc.bottom) / 2, 
			pszText, 
			lstrlen(pszText));

		return S_OK;
	}
public :
	STDMETHOD(RefreshConfiguration)(/*[out,retval]*/ UCHAR * pVal);
	STDMETHOD(InArray)(/*[in]*/ BYTE Port, /*[in]*/ BYTE Slot, /*[in]*/ BYTE Interface, /*[out]*/ SAFEARRAY * Data);
	BYTE m_bEnabledEvents;
	HANDLE m_hEventThread;
	STDMETHOD(Setup)(UCHAR Port,UCHAR Slot, UCHAR interfacenumber, UCHAR Endpoint, BSTR * pData, /*[out, retval]*/ UCHAR * pRetVal);
	STDMETHOD(Out)(UCHAR Port,UCHAR Slot, UCHAR interfacenumber, UCHAR Endpoint, UCHAR pid, BSTR * pData, int iDataSize, /*[out, retval]*/ UCHAR * ReturnValue );
	STDMETHOD(In)(UCHAR Port,UCHAR Slot, UCHAR interfacenumber, UCHAR Endpoint, UCHAR Format, /*[out,retval]*/ BSTR * pbsRet);
	STDMETHOD(Reset)(UCHAR Port,UCHAR Slot, UCHAR interfacenumber, /*[out,retval]*/ UCHAR * ucReturn);
	STDMETHOD(EnableEvents)(BYTE bEventMask);
	STDMETHOD(SetUsbTimeout)(ULONG ulTimeout,UCHAR * ucReturn);
	STDMETHOD(get_TimeoutValue)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_TimeoutValue)(/*[in]*/ short newVal);
	HANDLE			m_hInsertMutex;
	short			m_sTimeout;
	BOOLEAN m_fStayAlive;

BEGIN_CONNECTION_POINT_MAP(CUSBXBCtl)
	CONNECTION_POINT_ENTRY(DIID__IUSBXBCtlEvents)
END_CONNECTION_POINT_MAP()

};

#endif //__USBXBCTL_H_
