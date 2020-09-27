/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

// engine.cpp : Implementation of Cengine
#include "stdafx.h"
#include "Qtest.h"
#include "engine.h"
#include <comdef.h>

/////////////////////////////////////////////////////////////////////////////
// Helper template functions

template <class T> HRESULT Dump( T* src, BYTE* rgbDest, DWORD dwDestLen, DWORD* pdwDumped){

	DWORD dwT=sizeof(T);

	if ((dwT>dwDestLen) || (NULL==rgbDest) || (NULL==src))
		return E_INVALIDARG;

	((BASE_MSG_HEADER*)src)->dwMsgLen=dwT-sizeof(BASE_MSG_HEADER);
	memcpy((void*)rgbDest,src,dwT);
	*pdwDumped=dwT;

	//success
	return S_OK;

}//endmethod

template <class T> HRESULT Dump( T* src, const BYTE* rgbExtendedData, DWORD dwExtendedDataLen, BYTE* rgbDest, DWORD dwDestLen, DWORD* pdwDumped){

	DWORD dwT=sizeof(T);
	DWORD dwALL=dwT+dwExtendedDataLen;

	if ((dwALL>dwDestLen) || (NULL==rgbDest) || (NULL==src) || (NULL==rgbExtendedData))
		return E_INVALIDARG;

	((BASE_MSG_HEADER*)src)->dwMsgLen=dwALL-sizeof(BASE_MSG_HEADER);
	memcpy((void*)rgbDest,src,dwT);
	memcpy((void*)(rgbDest+dwT),rgbExtendedData,dwExtendedDataLen);
	*pdwDumped=dwALL;

	//success
	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Cengine

STDMETHODIMP Cengine::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_Iengine
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
#ifndef USE_VC7_WINDOWS
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
#else
		if (InlineIsEqualGUID(*arr[i],riid))
#endif
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP Cengine::Connect(BSTR bstrHost, unsigned int uiPort)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//BSTR wrapper
	_bstr_t l_bstrHost(bstrHost);

	//Convert address
	DWORD dwIP=::InternetAddress(l_bstrHost);

	//Connect to host
	if FAILED(m_pClient->Connect(dwIP,(WORD)uiPort)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::Disconnect()
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	m_pClient->Close(false);

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::GetACKElement(BSTR bstrElementName, unsigned long *pulElement)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Did we receive a reply?
	if (m_dwMsgReplyType!=MSGTYPE_ACK) {
		return E_FAIL;
	}//endif

	BASE_ACK_MSG* pAck;
	pAck=(BASE_ACK_MSG*)m_rgbReply;

	//BSTR wrapper
	_bstr_t l_bstrName(bstrElementName);

	//Return requested element
	if (0==_stricmp(l_bstrName,"ackmsgtype")){
		*pulElement=pAck->dwAckMsgType;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"ackseqnum")){
		*pulElement=pAck->dwAckSeqNum;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"hr")){
		*pulElement=pAck->hrAck;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"louserid")){
		*pulElement=LODWORD(pAck->qwAckUserID);
	}//endif
	if (0==_stricmp(l_bstrName,"hiuserid")){
		*pulElement=HIDWORD(pAck->qwAckUserID);
	}//endif

	//Element not found
	return E_FAIL;

}//endemthod

STDMETHODIMP Cengine::GetListReplyElement(BSTR bstrElementName, unsigned long *pulElement)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Did we receive a reply?
	if (m_dwMsgReplyType!=QMSG_LIST_REPLY) {
		return E_FAIL;
	}//endif

	Q_LIST_REPLY_MSG* pReply;
	pReply=(Q_LIST_REPLY_MSG*)m_rgbReply;

	//BSTR wrapper
	_bstr_t l_bstrName(bstrElementName);

	//Return requested element
	if (0==_stricmp(l_bstrName,"hiuserid")){
		*pulElement=HIDWORD(pReply->qwUserID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"louserid")){
		*pulElement=LODWORD(pReply->qwUserID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"qtype")){
		*pulElement=pReply->dwQType;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"hicookie")){
		*pulElement=HIDWORD(pReply->qwCookie);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"locookie")){
		*pulElement=LODWORD(pReply->qwCookie);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"hinextquid")){
		*pulElement=HIDWORD(pReply->qwNextQUID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"lonextquid")){
		*pulElement=LODWORD(pReply->qwNextQUID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"qleft")){
		*pulElement=pReply->dwQLeft;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"numitems")){
		*pulElement=pReply->dwNumItems;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"hr")){
		*pulElement=pReply->hr;
		return S_OK;
	}//endif

	//Element not found
	return E_FAIL;

}//endmethod

STDMETHODIMP Cengine::GetListReplyItemElement(unsigned long ulItem, BSTR bstrElementName, VARIANT *pElement)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Did we receive a reply?
	if (m_dwMsgReplyType!=QMSG_LIST_REPLY) {
		return E_FAIL;
	}//endif

	Q_LIST_REPLY_MSG* pReply;
	pReply=(Q_LIST_REPLY_MSG*)m_rgbReply;

	//Make sure item index is valid
	if ((pReply->dwNumItems<ulItem-1)||(ulItem<=0)){
		return E_FAIL;
	}//endif

	Q_LIST_ITEM* pItem;
	BYTE* pb=m_rgbReply+sizeof(Q_LIST_REPLY_MSG);
	for (unsigned long ul=0; ul<ulItem; ul++){
		pItem=(Q_LIST_ITEM*)pb;
		pb=((BYTE*)(pItem))+sizeof(Q_LIST_ITEM)+pItem->dwItemLen;
	}//endfor

	//BSTR wrapper
	_bstr_t l_bstrName(bstrElementName);

	if (0==_stricmp(l_bstrName,"hiquid")){
		pElement->vt=VT_I4;
		pElement->lVal=HIDWORD(pItem->qwQUID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"loquid")){
		pElement->vt=VT_I4;
		pElement->lVal=LODWORD(pItem->qwQUID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"qtype")){
		pElement->vt=VT_I4;
		pElement->lVal=pItem->dwItemLen;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"data")){
		pElement->vt=VT_BSTR;
		int iLen=pItem->dwItemLen;
		pb=((BYTE*)(pItem))+sizeof(Q_LIST_ITEM);
	
		//Convert array of lo bytes back to BSTR
		BYTE* rgbPacket;
		rgbPacket=new BYTE[iLen*2];
		for (int i=0; i<iLen; i++){
			rgbPacket[i*2]=pb[i];
			rgbPacket[i*2+1]=0;
		}//endfor
		pElement->bstrVal=SysAllocStringByteLen((char*)rgbPacket,iLen*2);
		delete [] rgbPacket;

		return S_OK;
	}//endif

	//Element not found
	return E_FAIL;

}//endmethod

STDMETHODIMP Cengine::GetMessageElement(BSTR bstrElementName, unsigned long *pulElement)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Did we receive a reply?
	if (QMSG_MAX_REPLY_TYPE+1==m_dwMsgReplyType) {
		return E_FAIL;
	}//endif

	BASE_MSG_HEADER* pMsg;
	pMsg=(BASE_MSG_HEADER*)m_rgbReply;

	//BSTR wrapper
	_bstr_t l_bstrName(bstrElementName);

	//Return requested element
	if (0==_stricmp(l_bstrName,"msglen")){
		*pulElement=pMsg->dwMsgLen;
		return S_OK;
	}//endif

	//Return requested element
	if (0==_stricmp(l_bstrName,"msgtype")){
		*pulElement=pMsg->dwMsgType;
		return S_OK;
	}//endif

	//Return requested element
	if (0==_stricmp(l_bstrName,"seqnum")){
		*pulElement=pMsg->dwSeqNum;
		return S_OK;
	}//endif

	//Return requested element
	if (0==_stricmp(l_bstrName,"sessid")){
		*pulElement=pMsg->dwSessID;
		return S_OK;
	}//endif

	//Element not found
	return E_FAIL;

}//endmethod

STDMETHODIMP Cengine::GetReplyType(unsigned long* pulMsgReplyType)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//No reply yet
	if (QMSG_MAX_REPLY_TYPE+1==m_dwMsgReplyType)
		return E_FAIL;

	*pulMsgReplyType=m_dwMsgReplyType;

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::GetTickleElement(BSTR bstrElementName, unsigned long *pulElement)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	if (false==m_fTickle){
		return E_FAIL;
	}//endif

	//BSTR wrapper
	_bstr_t l_bstrName(bstrElementName);

	//Return requested element
	if (0==_stricmp(l_bstrName,"hiuserid")){
		*pulElement=HIDWORD(m_qTickle.qwUserID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"louserid")){
		*pulElement=LODWORD(m_qTickle.qwUserID);
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"qtype")){
		*pulElement=m_qTickle.dwQType;
		return S_OK;
	}//endif
	if (0==_stricmp(l_bstrName,"qlength")){
		*pulElement=m_qTickle.dwQLength;
		return S_OK;
	}//endif

	//element not found
	return E_FAIL;

}//endmethod

STDMETHODIMP Cengine::InternetAddress(BSTR bstrHost, unsigned long* pulIP)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//BSTR wrapper
	_bstr_t l_bstrHost(bstrHost);

	//Convert
	*pulIP=ntohl(::InternetAddress(l_bstrHost));
	if (INADDR_NONE == *pulIP){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgAdd(unsigned long ulSessionId, unsigned long ulSequence, BSTR bstrUsers, unsigned long ulNumUsers, unsigned long ulQType, BSTR bstrData, unsigned long ulDataLen)
{
	HRESULT hr=S_OK;

	//Convert BSTR to array of lo bytes
	BYTE* rgbUsers;
	rgbUsers=new BYTE[ulNumUsers*sizeof(QWORD)];
	for (unsigned long ul=0;ul<ulNumUsers*sizeof(QWORD);ul++){
		rgbUsers[ul]=LOBYTE(bstrUsers[ul]);
	}//endfor
	
	//Convert users string to QWORDs
	QWORD* rgqwUserIds=new QWORD[ulNumUsers];
	BYTE* pb=rgbUsers;
	for (ul=0;ul<ulNumUsers;ul++){
		rgqwUserIds[ul]=MAKEQWORD(MAKELONG(MAKEWORD(pb[7],pb[6]),MAKEWORD(pb[5],pb[4])),MAKELONG(MAKEWORD(pb[3],pb[2]),MAKEWORD(pb[1],pb[0])));
		pb+=sizeof(QWORD);
	}//endfor

	//Convert BSTR to array of lo bytes
	BYTE* rgbData;
	rgbData=new BYTE[ulDataLen];
	for (ul=0;ul<ulDataLen;ul++){
		rgbData[ul]=LOBYTE(bstrData[ul]);
	}//endfor

	//Call extended method
	hr=MsgAddEx(true, ulSessionId, ulSequence, rgqwUserIds, ulNumUsers, ulQType, rgbData, ulDataLen);
	
	//Cleanup
	delete [] rgbUsers;
	delete [] rgbData;
	delete [] rgqwUserIds;

	return hr;

}//endmethod

STDMETHODIMP Cengine::MsgDeadXIP(unsigned long ulSessionId, unsigned long ulSequence, unsigned long ulHiXIP, unsigned long ulLoXIP)
{
	return MsgDeadXIPEx(true, ulSessionId, ulSequence, MAKEQWORD(ulLoXIP,ulHiXIP));
}//endmethod

STDMETHODIMP Cengine::MsgDeadXRG(unsigned long ulSessionId, unsigned long ulSequence, unsigned long ulHiXRG, unsigned long ulLoXRG)
{
	return MsgDeadXRGEx(true, ulSessionId, ulSequence, MAKEQWORD(ulLoXRG,ulHiXRG));
}//endmethod

STDMETHODIMP Cengine::MsgDelete(unsigned long ulSessionId, unsigned long ulSequence, unsigned long ulQType, unsigned long ulHiUserId, unsigned long ulLoUserId, unsigned long ulHiFirstQUID, unsigned long ulLoFirstQUID, unsigned long ulHiLastQUID, unsigned long ulLoLastQUID)
{
	return MsgDeleteEx(true, ulSessionId, ulSequence, ulQType, MAKEQWORD(ulLoUserId,ulHiUserId), MAKEQWORD(ulLoFirstQUID,ulHiFirstQUID), MAKEQWORD(ulLoLastQUID,ulHiLastQUID));
}//endmethod

STDMETHODIMP Cengine::MsgDeleteMatches(unsigned long ulSessionId, unsigned long ulSequence, BSTR bstrUsers, unsigned long ulNumUsers, unsigned long ulQType, unsigned long ulMatchAttrNum, BSTR bstrData, unsigned long ulDataLen)
{
	HRESULT hr=S_OK;

	//Convert BSTR to array of lo bytes
	BYTE* rgbUsers;
	rgbUsers=new BYTE[ulNumUsers*sizeof(QWORD)];
	for (unsigned long ul=0;ul<ulNumUsers*sizeof(QWORD);ul++){
		rgbUsers[ul]=LOBYTE(bstrUsers[ul]);
	}//endfor
	
	//Convert users string to QWORDs
	QWORD* rgqwUserIds=new QWORD[ulNumUsers];
	BYTE* pb=rgbUsers;
	for (ul=0;ul<ulNumUsers;ul++){
		rgqwUserIds[ul]=MAKEQWORD(MAKELONG(MAKEWORD(pb[7],pb[6]),MAKEWORD(pb[5],pb[4])),MAKELONG(MAKEWORD(pb[3],pb[2]),MAKEWORD(pb[1],pb[0])));
		pb+=sizeof(QWORD);
	}//endfor

	//Convert BSTR to array of lo bytes
	BYTE* rgbData;
	rgbData=new BYTE[ulDataLen];
	for (ul=0;ul<ulDataLen;ul++){
		rgbData[ul]=LOBYTE(bstrData[ul]);
	}//endfor
	
	hr = MsgDeleteMatchesEx(true, ulSessionId, ulSequence, rgqwUserIds, ulNumUsers, ulQType, ulMatchAttrNum, rgbData, ulDataLen);

	delete [] rgbData;
	delete [] rgbUsers;
	delete [] rgqwUserIds;

	return hr;
}//endmethod

STDMETHODIMP Cengine::MsgHello(unsigned long ulSessionId, unsigned long ulSequence, unsigned long ulVersion, BSTR bstrDescription, unsigned int uiLen)
{
	return MsgHelloEx(true,ulSessionId,ulSequence,ulVersion,bstrDescription,uiLen);
}//endmethod

STDMETHODIMP Cengine::MsgList(unsigned long ulSessionId, unsigned long ulSequence, unsigned long ulHiUserId, unsigned long ulLoUserId, unsigned long ulQType, unsigned long ulHiCookie, unsigned long ulLoCookie, unsigned long ulHiStartQUID, unsigned long ulLoStartQUID, unsigned long ulMaxItems, unsigned long ulTotalItemDataSize)
{
	return MsgListEx(true, ulSessionId, ulSequence, MAKEQWORD(ulLoUserId,ulHiUserId), ulQType, MAKEQWORD(ulLoCookie,ulHiCookie), MAKEQWORD(ulLoStartQUID,ulHiStartQUID), ulMaxItems, ulTotalItemDataSize);
}//endmethod

STDMETHODIMP Cengine::MsgUserInfo(unsigned long ulSessionId, unsigned long ulSequence, unsigned long ulHiUserId, unsigned long ulLoUserId, unsigned long ulHiXIP, unsigned long ulLoXIP, unsigned long ulHiXRG, unsigned long ulLoXRG, unsigned long ulTickleIP, unsigned short usTicklePort)
{
	return MsgUserInfoEx(true, ulSessionId, ulSequence, MAKEQWORD(ulLoUserId,ulHiUserId), MAKEQWORD(ulLoXIP,ulHiXIP), MAKEQWORD(ulLoXRG,ulHiXRG), ulTickleIP, usTicklePort);
}//endmethod

STDMETHODIMP Cengine::TickleListen(VARIANT_BOOL fOn, unsigned short usTicklePort)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	if (VARIANT_TRUE==fOn){
		if (NULL==m_pTickleCon){
			m_pTickleCon= new UDPConnection(m_hShutdownEvent);
		}//endif
		m_pTickleCon->Bind(usTicklePort);
	}else{
		if (NULL!=m_pTickleCon){
			delete m_pTickleCon;
			m_pTickleCon=NULL;
		}//endif
	}//endif

	m_fTickle=false;

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::WaitForDisconnect(int iTimeout, VARIANT_BOOL* pfConnected)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	if (true==m_pClient->WaitForDisconnect(iTimeout)){
		*pfConnected=VARIANT_TRUE;
	}else{
		*pfConnected=VARIANT_FALSE;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::WaitForReply(unsigned long ulTimeout, VARIANT_BOOL *pfTimeout)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	*pfTimeout=VARIANT_FALSE;

	DWORD dwRcv=0L;
	DWORD dwTotal=0L;
	while (dwTotal!=sizeof(BASE_MSG_HEADER)){
		dwRcv=0L;
		if (S_OK!=m_pClient->Receive((char*)m_rgbReply+dwTotal,sizeof(BASE_MSG_HEADER)-dwTotal,&dwRcv,ulTimeout)){
			*pfTimeout=VARIANT_TRUE;
			return S_OK;
		}else{
			dwTotal+=dwRcv;
		}//endif
	}//endwhile

	m_dwMsgReplyType=*((DWORD*)m_rgbReply);

	dwRcv=0L;
	dwTotal=0L;
	DWORD dwData= *((DWORD*)(m_rgbReply+sizeof(DWORD)));
	while (dwTotal!=dwData){
		dwRcv=0L;
		if (S_OK!=m_pClient->Receive((char*)m_rgbReply+sizeof(BASE_MSG_HEADER)+dwTotal,dwData-dwTotal,&dwRcv,ulTimeout)){
			*pfTimeout=VARIANT_TRUE;
			return S_OK;
		}else{
			dwTotal+=dwRcv;
		}//endif
	}//endwhile

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::WaitForTickle(unsigned long ulTimeout, VARIANT_BOOL *pfTimeout)
{
	//Did the user bind?
	if (NULL==m_pTickleCon){
		return E_FAIL;
	}//endif

	char rgchBuffer[sizeof(Q_TICKLE_MSG)];  //Wire data
	DWORD dwRead=0; //Bytes read on one receive
	DWORD dwTotal=0;  //Total bytes read

	//Receive tickles until shutdown event is fired
	dwTotal=0;
	while (dwTotal<sizeof(Q_TICKLE_MSG)){
		dwRead=0;
		//Receive part or all of a tickle
		if (FAILED(m_pTickleCon->ReceiveFrom(rgchBuffer,sizeof(rgchBuffer)-dwRead,&dwRead,ulTimeout))){
			return E_FAIL;
		}
		dwTotal+=dwRead;
	}//endwhile

	//Store in tickle structure
	memcpy(&m_qTickle,rgchBuffer,sizeof(Q_TICKLE_MSG));
	m_fTickle=true;

	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// IengineEx

STDMETHODIMP Cengine::MsgAddEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG* rgqwUserIds, /*[in]*/ unsigned long ulNumUsers, /*[in]*/ unsigned long ulQType, /*[in]*/ BYTE* rgbData, /*[in]*/ unsigned long ulDataLen)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qAddMsg,sizeof(m_qAddMsg));

	//Populate message structure
	m_qAddMsg.dwMsgType=QMSG_ADD;
	m_qAddMsg.dwSeqNum=ulSequence;
	m_qAddMsg.dwQType=ulQType;
	m_qAddMsg.dwUserCount=ulNumUsers;
	m_qAddMsg.dwSessID=ulSessionId;

	BYTE* rgbPacket;
	rgbPacket=new BYTE[ulDataLen+ulNumUsers*sizeof(QWORD)];
	BYTE* pb=rgbPacket;

	//Add userids to blob
	for (unsigned long ul=0;ul<ulNumUsers;ul++){
		*((QWORD*)pb)=rgqwUserIds[ul];
		pb+=sizeof(QWORD);
	}//endfor

	//Add data to blob
	for (ul=0;ul<ulDataLen;ul++){
		pb[ul]=rgbData[ul];
	}//endfor

	//Dump structure to bytes
	if FAILED(Dump(&m_qAddMsg,rgbPacket,ulDataLen+ulNumUsers*sizeof(QWORD),m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
		delete [] rgbPacket;
		return E_FAIL;
	}//endif

	delete [] rgbPacket;

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgDeadXIPEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwXIP)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qDeadXIPMsg,sizeof(m_qDeadXIPMsg));

	//Populate message structure
	m_qDeadXIPMsg.dwMsgType=QMSG_DEAD_XIP;
	m_qDeadXIPMsg.dwSeqNum=ulSequence;
	m_qDeadXIPMsg.qwXIP=qwXIP;
	m_qDeadXIPMsg.dwSessID=ulSessionId;

	//Dump structure to bytes
	if FAILED(Dump(&m_qDeadXIPMsg,m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
		return E_FAIL;
	}//endif

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgDeadXRGEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwXRG)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qDeadXRGMsg,sizeof(m_qDeadXRGMsg));

	//Populate message structure
	m_qDeadXRGMsg.dwMsgType=QMSG_DEAD_XRG;
	m_qDeadXRGMsg.dwSeqNum=ulSequence;
	m_qDeadXRGMsg.qwXRG=qwXRG;
	m_qDeadXRGMsg.dwSessID=ulSessionId;

	//Dump structure to bytes
	if FAILED(Dump(&m_qDeadXRGMsg,m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
		return E_FAIL;
	}//endif

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgDeleteEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulQType, /*[in]*/ ULONGLONG qwUserId, /*[in]*/ ULONGLONG qwFirstQUID, /*[in]*/ ULONGLONG qwLastQUID)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qDeleteMsg,sizeof(m_qDeleteMsg));

	//Populate message structure
	m_qDeleteMsg.dwMsgType=QMSG_DELETE;
	m_qDeleteMsg.dwSeqNum=ulSequence;
	m_qDeleteMsg.dwQType=ulQType;
	m_qDeleteMsg.qwUserID=qwUserId;
	m_qDeleteMsg.qwFirstQUID=qwFirstQUID;
	m_qDeleteMsg.qwLastQUID=qwLastQUID;
	m_qDeleteMsg.dwSessID=ulSessionId;

	//Dump structure to bytes
	if FAILED(Dump(&m_qDeleteMsg,m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
		return E_FAIL;
	}//endif

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgDeleteMatchesEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG* rgqwUserIds, /*[in]*/ unsigned long ulNumUsers, /*[in]*/ unsigned long ulQType, /*[in]*/ unsigned long ulMatchAttrNum, /*[in]*/ BYTE* rgbData, /*[in]*/ unsigned long ulDataLen)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qDeleteMatchesMsg,sizeof(m_qDeleteMatchesMsg));

	//Populate message structure
	m_qDeleteMatchesMsg.dwMsgType=QMSG_DELETE_MATCHES;
	m_qDeleteMatchesMsg.dwSeqNum=ulSequence;
	m_qDeleteMatchesMsg.dwQType=ulQType;
	m_qDeleteMatchesMsg.dwSessID=ulSessionId;
	m_qDeleteMatchesMsg.dwUserCount=ulNumUsers;

	BYTE* rgbPacket;
	rgbPacket=new BYTE[ulDataLen+ulNumUsers*sizeof(QWORD)];
	BYTE* pb=rgbPacket;

	//Add userids to blob
	for (unsigned long ul=0;ul<ulNumUsers;ul++){
		*((QWORD*)pb)=rgqwUserIds[ul];
		pb+=sizeof(QWORD);
	}//endfor

	//Add data to blob
	for (ul=0;ul<ulDataLen;ul++){
		pb[ul]=rgbData[ul];
	}//endfor

	//Dump structure to bytes
	if FAILED(Dump(&m_qDeleteMatchesMsg,rgbPacket,ulDataLen+ulNumUsers*sizeof(QWORD),m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
		delete [] rgbPacket;
		return E_FAIL;
	}//endif

	delete [] rgbPacket;

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgHelloEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ unsigned long ulVersion, /*[in]*/ BSTR bstrDescription, /*[in]*/ unsigned int uiLen)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qHelloMsg,sizeof(m_qHelloMsg));

	//Populate message structure
	m_qHelloMsg.dwMsgType=QMSG_HELLO;
	m_qHelloMsg.dwSeqNum=ulSequence;
	m_qHelloMsg.dwProtocolVersion=ulVersion;
	m_qHelloMsg.dwSessID=ulSessionId;
	
	//BSTR wrapper
	_bstr_t l_bstrDesc(bstrDescription);

	//Dump structure to bytes
	if (0==uiLen){
		if FAILED(Dump(&m_qHelloMsg,(const BYTE*)(char*)l_bstrDesc,l_bstrDesc.length()+1,m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
			return E_FAIL;
		}//endif
	}else{
		if FAILED(Dump(&m_qHelloMsg,(const BYTE*)(char*)l_bstrDesc,uiLen,m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
			return E_FAIL;
		}//endif
	}//endif

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgListEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwUserId, /*[in]*/ unsigned long ulQType, /*[in]*/ ULONGLONG qwCookie, /*[in]*/ ULONGLONG qwStartQUID, /*[in]*/ unsigned long ulMaxItems, /*[in]*/ unsigned long ulTotalItemDataSize)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qListMsg,sizeof(m_qListMsg));

	//Populate message structure
	m_qListMsg.dwMsgType=QMSG_LIST;
	m_qListMsg.dwSeqNum=ulSequence;
	m_qListMsg.dwQType=ulQType;
	m_qListMsg.qwUserID=qwUserId;
	m_qListMsg.qwCookie=qwCookie;
	m_qListMsg.qwStartQUID=qwStartQUID;
	m_qListMsg.dwMaxItems=ulMaxItems;
	m_qListMsg.dwMaxTotalItemDataSize=ulTotalItemDataSize;
	m_qListMsg.dwSessID=ulSessionId;

	//Dump structure to bytes
	if FAILED(Dump(&m_qListMsg, m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
		return E_FAIL;
	}//endif

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::MsgUserInfoEx(/*[in]*/ boolean fFlush, /*[in]*/ unsigned long ulSessionId, /*[in]*/ unsigned long ulSequence, /*[in]*/ ULONGLONG qwUserId, /*[in]*/ ULONGLONG qwXIP, /*[in]*/ ULONGLONG qwXRG, /*[in]*/ unsigned long ulTickleIP, /*[in]*/ unsigned short usTicklePort)
{
	//Check if engine is initialized
	if (!m_fInit) return E_FAIL;

	//Zero message structure
	ZeroMemory(&m_qUserInfoMsg,sizeof(m_qUserInfoMsg));

	//Populate message structure
	m_qUserInfoMsg.dwMsgType=QMSG_USER_INFO;
	m_qUserInfoMsg.dwSeqNum=ulSequence;
	m_qUserInfoMsg.qwUserID=qwUserId;
	m_qUserInfoMsg.qwXIP=qwXIP;
	m_qUserInfoMsg.qwXRG=qwXRG;
	m_qUserInfoMsg.dwTickleIP=ulTickleIP;
	m_qUserInfoMsg.wTicklePort=usTicklePort;
	m_qUserInfoMsg.dwSessID=ulSessionId;

	//Dump structure to bytes
	if FAILED(Dump(&m_qUserInfoMsg,m_rgbData,sizeof(m_rgbData),&m_dwDumped)){
		return E_FAIL;
	}//endif

	//Transmit bytes
	if FAILED(m_pClient->Send((char*)m_rgbData,m_dwDumped,fFlush)){
		return E_FAIL;
	}//endif

	//Success
	return S_OK;

}//endmethod

STDMETHODIMP Cengine::InitEx(unsigned long ulcSendBuffers, unsigned long ulcSendBufferSize, long lcReceiveBufferSize)
{
	Init();
	
	//Delete client connection
	if (NULL!=m_pClient){
		delete m_pClient;
		m_pClient=NULL;
	}//endif

	//Create client connection object
	m_pClient = new TCPClientConnection(m_hShutdownEvent,ulcSendBuffers,ulcSendBufferSize,lcReceiveBufferSize);
	if (NULL==m_pClient){
		m_fInit=false;
		return E_FAIL;
	}//endif

	return S_OK;
}

STDMETHODIMP Cengine::GetResponseData(BYTE *rgbData, unsigned long ulDataLen, unsigned long* pulDataLen)
{
	return m_pClient->Receive((char*)rgbData,ulDataLen,pulDataLen);
}

STDMETHODIMP Cengine::SetShutdownEvent()
{
	SetEvent(m_hShutdownEvent);

	return S_OK;
}

STDMETHODIMP Cengine::Init()
{
	m_fInit=true;
	m_dwDumped=0L;
	m_dwMsgReplyType=QMSG_MAX_REPLY_TYPE+1;
	m_pClient=NULL;
	m_hShutdownEvent=NULL;
	m_pTickleCon=NULL;
	m_fTickle=false;

	//Bring winsock up if not already
	if FAILED(WinsockUP()){
		m_fInit=false;
	}//endif

	//Initialize instance data
	ZeroMemory(m_rgbData,sizeof(m_rgbData));
	ZeroMemory(m_rgbReply,sizeof(m_rgbReply));

	//Create shutdown event
	m_hShutdownEvent=CreateEvent(NULL,true,FALSE,NULL);

	//Create client connection object
	m_pClient = new TCPClientConnection(m_hShutdownEvent,10,1024);
	if (NULL==m_pClient){
		m_fInit=false;
	}//endif

	return S_OK;
}

STDMETHODIMP Cengine::Cleanup()
{
	//Trigger any blocked connection options to abort
	SetEvent(m_hShutdownEvent);
	Sleep(200);

	//Delete client connection
	if (NULL!=m_pTickleCon){
		delete m_pTickleCon;
		m_pTickleCon=NULL;
	}//endif

	//Delete client connection
	if (NULL!=m_pClient){
		delete m_pClient;
		m_pClient=NULL;
	}//endif

	//Delete the shutdown event
	if (NULL!=m_hShutdownEvent){
		CloseHandle(m_hShutdownEvent);
		m_hShutdownEvent=NULL;
	}//endif

	m_fInit=false;
	m_dwDumped=0L;
	m_dwMsgReplyType=QMSG_MAX_REPLY_TYPE+1;
	m_fTickle=false;

	return S_OK;
}

STDMETHODIMP Cengine::CleanupEx()
{
	return Cleanup();
}

STDMETHODIMP Cengine::Flush(unsigned long ulTimeout)
{
	return m_pClient->WaitForAllPendingBuffersToFlush(ulTimeout);
}
