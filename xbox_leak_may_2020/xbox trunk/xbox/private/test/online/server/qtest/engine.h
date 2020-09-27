/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

// engine.h : Declaration of the Cengine

#ifndef __ENGINE_H_
#define __ENGINE_H_

#include "resource.h"       // main symbols
#include "connection.h"		// connection
#include "xqprotocol.h"		// protocol structures

/////////////////////////////////////////////////////////////////////////////
// Cengine
class ATL_NO_VTABLE Cengine : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<Cengine, &CLSID_engine>,
	public ISupportErrorInfo,
	public IengineEx,
	public IDispatchImpl<Iengine, &IID_Iengine, &LIBID_QTESTLib>
{
public:
	Cengine()
	{
	}
	~Cengine()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ENGINE)
DECLARE_NOT_AGGREGATABLE(Cengine)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(Cengine)
	COM_INTERFACE_ENTRY(Iengine)
	COM_INTERFACE_ENTRY(IengineEx)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// Iengine
public:
	STDMETHOD(Flush)(/*[in]*/ unsigned long ulTimeout);
	STDMETHOD(CleanupEx)();
	STDMETHOD(Cleanup)();
	STDMETHOD(Init)();
	STDMETHOD(SetShutdownEvent)();
	STDMETHOD(GetResponseData)(/*[in]*/ BYTE* rgbData,/*[in]*/ unsigned long ulDataLen, /*[in,out]*/ unsigned long* pulDataLen);
	STDMETHOD(InitEx)(/*[in]*/ unsigned long ulcSendBuffers, /*[in]*/ unsigned long ulcSendBufferSize, /*[in]*/ long lcReceiveBufferSize);
	STDMETHOD(MsgAddEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG* rgqwUserIds, /*[in]*/ unsigned long ulNumUsers, /*[in]*/ unsigned long ulQType, /*[in]*/ BYTE* rgbData, /*[in]*/ unsigned long ulDataLen);
	STDMETHOD(MsgDeadXIPEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwXIP);
	STDMETHOD(MsgDeadXRGEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwXRG);
	STDMETHOD(MsgDeleteEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulQType, /*[in]*/ ULONGLONG qwUserId, /*[in]*/ ULONGLONG qwFirstQUID, /*[in]*/ ULONGLONG qwLastQUID);
	STDMETHOD(MsgDeleteMatchesEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG* rgqwUserIds, /*[in]*/ unsigned long ulNumUsers, /*[in]*/ unsigned long ulQType, /*[in]*/ unsigned long ulMatchAttrNum, /*[in]*/ BYTE* rgbData, /*[in]*/ unsigned long ulDataLen);
	STDMETHOD(MsgHelloEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulVersion, /*[in]*/ BSTR bstrDescription, /*[in]*/ unsigned int uiLen=0);
	STDMETHOD(MsgListEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwUserId, /*[in]*/ unsigned long ulQType, /*[in]*/ ULONGLONG qwCookie, /*[in]*/ ULONGLONG qwStartQUID, /*[in]*/ unsigned long ulMaxItems, /*[in]*/ unsigned long ulTotalItemDataSize);
	STDMETHOD(MsgUserInfoEx)(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwUserId, /*[in]*/ ULONGLONG qwXIP, /*[in]*/ ULONGLONG qwXRG, /*[in]*/ unsigned long ulTickleIP, /*[in]*/ unsigned short usTicklePort);

	STDMETHOD(Connect)(/*[in]*/ BSTR bstrHost, /*[in]*/ unsigned int uiPort);
	STDMETHOD(Disconnect)();
	STDMETHOD(GetACKElement)(/*[in]*/ BSTR bstrElementName, /*[out,retval]*/ unsigned long *pulElement);
	STDMETHOD(GetListReplyElement)(/*[in]*/BSTR bstrElementName, /*[out,retval]*/ unsigned long *pulElement);
	STDMETHOD(GetListReplyItemElement)(/*[in]*/ unsigned long ulItem, /*[in]*/ BSTR bstrElementName, /*[out,retval]*/ VARIANT* pElement);
	STDMETHOD(GetMessageElement)(/*[in]*/ BSTR bstrElementName, /*[out,retval]*/ unsigned long* pulElement);
	STDMETHOD(GetReplyType)(/*[out, retval]*/ unsigned long* pulMsgReplyType);
	STDMETHOD(GetTickleElement)(/*[in]*/ BSTR bstrElementName, /*[out,retval]*/ unsigned long* pulElement);
	STDMETHOD(InternetAddress)(/*[in]*/ BSTR bstrHost, /*[out,retval]*/ unsigned long* pulIP);
	STDMETHOD(MsgAdd)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ BSTR bstrUsers, /*[in]*/ unsigned long ulNumUsers, /*[in]*/ unsigned long ulQType, /*[in]*/ BSTR bstrData, /*[in]*/ unsigned long ulDataLen);
	STDMETHOD(MsgDelete)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulQType, /*[in]*/ unsigned long ulHiUserId, /*[in]*/ unsigned long ulLoUserId, /*[in]*/ unsigned long ulHiFirstQUID, /*[in]*/ unsigned long ulLoFirstQUID, /*[in]*/ unsigned long ulHiLastQUID, /*[in]*/ unsigned long ulLoLastQUID);
	STDMETHOD(MsgDeleteMatches)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence,  /*[in]*/ BSTR bstrUsers, /*[in]*/ unsigned long ulNumUsers, /*[in]*/ unsigned long ulQType, /*[in]*/ unsigned long ulMatchAttrNum, /*[in]*/ BSTR bstrData, /*[in]*/ unsigned long ulDataLen);
	STDMETHOD(MsgDeadXIP)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulHiXIP, /*[in]*/ unsigned long ulLoXIP);
	STDMETHOD(MsgDeadXRG)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulHiXRG, /*[in]*/ unsigned long ulLoXRG);
	STDMETHOD(MsgHello)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulVersion, /*[in]*/ BSTR bstrDescription, /*[in]*/ unsigned int uiLen=0);
	STDMETHOD(MsgList)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulHiUserId, /*[in]*/ unsigned long ulLoUserId, /*[in]*/ unsigned long ulQType, /*[in]*/ unsigned long ulHiCookie, /*[in]*/ unsigned long ulLoCookie, /*[in]*/ unsigned long ulHiStartQUID, /*[in]*/ unsigned long ulLoStartQUID, /*[in]*/ unsigned long ulMaxItems, /*[in]*/ unsigned long ulTotalItemDataSize);
	STDMETHOD(MsgUserInfo)(/*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulHiUserId, /*[in]*/ unsigned long ulLoUserId, /*[in]*/ unsigned long ulHiXIP, /*[in]*/ unsigned long ulLoXIP, /*[in]*/ unsigned long ulHiXRG, /*[in]*/ unsigned long ulLoXRG, /*[in]*/ unsigned long ulTickleIP, /*[in]*/ unsigned short usTicklePort);
	STDMETHOD(TickleListen)(/*[in]*/ VARIANT_BOOL fOn, /*[in]*/ unsigned short usTicklePort);
	STDMETHOD(WaitForDisconnect)(/*[in]*/int iTimeout, /*[out,retval]*/ VARIANT_BOOL* pfConnected);
	STDMETHOD(WaitForReply)(/*[in]*/ unsigned long ulTimeout, /*[out,retval]*/ VARIANT_BOOL* pfTimeout);
	STDMETHOD(WaitForTickle)(/*[in]*/ unsigned long ulTimeout, /*[out,retval]*/ VARIANT_BOOL* pfTimeout);

private:
	static DWORD WINAPI TickleThreadFunc(LPVOID pData, HANDLE hShutdownEvent);

private:
	bool m_fInit;  //Initialized?
	BYTE m_rgbData[2048];  //Preallocated dump area
	DWORD m_dwDumped;  //Preallocated temp variable
	BYTE m_rgbReply[2048];  //Store 1 list reply
	DWORD m_dwMsgReplyType; //Message reply type

	//Outgoing connection related
	TCPClientConnection* m_pClient;  //TCP communications client
	HANDLE m_hShutdownEvent;  //Shutdown event

	//Tickle processing (Iengine)
	UDPConnection* m_pTickleCon; //UDP connection for receiving tickles
	bool m_fTickle; //Do we have one?

	//Preallocate msg structure for speed improvement (Iengine & IengineEx)
	Q_HELLO_MSG m_qHelloMsg;
	Q_USER_INFO_MSG m_qUserInfoMsg;
	Q_DEAD_XIP_MSG m_qDeadXIPMsg;
	Q_DEAD_XRG_MSG m_qDeadXRGMsg;
	Q_DELETE_MSG m_qDeleteMsg;
	Q_ADD_MSG m_qAddMsg;
	Q_LIST_MSG m_qListMsg;
	Q_DELETE_MATCHES_MSG m_qDeleteMatchesMsg;
	Q_TICKLE_MSG m_qTickle;

};

#endif //__ENGINE_H_
