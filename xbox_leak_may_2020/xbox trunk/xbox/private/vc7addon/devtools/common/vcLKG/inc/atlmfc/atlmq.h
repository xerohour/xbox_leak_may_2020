// This is a part of the Active Template Library.
// Copyright (C) 1996-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLMQ_H__
#define __ATLMQ_H__

#pragma once

#include <atlbase.h>
#include <mq.h>                             // MSMQ header file
#include <atlcoll.h>

namespace ATL {

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "mqrt.lib")			// import lib for MSMQ runtime
#endif  // !_ATL_NO_DEFAULT_LIBS

// class CMQPropertyHolder
// Base class for variable MSMQ property structures
// PropT is the base MSMQ structure to derive from
// example: MQMSGPROPS
// Note, this class does NOT free any of the pointer
// values embedded in MQPROPVARIANTs contained
// in m_rgProps
template <class PropT>
class CMQPropertyHolder : public PropT
{
protected:

public:
	CSimpleArray<PROPID> m_rgIds;			// array of property ids
	CSimpleArray<MQPROPVARIANT, CSimpleArrayEqualHelperFalse<MQPROPVARIANT> > m_rgProps;	// array of MQPROPVARIANT property values
	CSimpleArray<HRESULT> m_rgStatus;		// array of HRESULT property status values

	// constructor
	CMQPropertyHolder() throw()
	{
		cProp = 0;
		aPropID = 0;
		aPropVar = NULL;
		aStatus = NULL;
	}


	int Add(PROPID msgPropId, VARTYPE vt, void *pData = NULL, int nElems=0, int nElemSize=0) throw()
	{
		MQPROPVARIANT propVar;
		propVar.vt = vt;
		return Add(msgPropId, &propVar, pData, nElems, nElemSize);
	}


	int Add(PROPID msgPropId, MQPROPVARIANT *pPropVar, void *pData = NULL, int nElems=0, int nElemSize=0) throw()
	{
		ATLASSERT(pPropVar);

		DWORD dw = msgPropId;
		if (!m_rgIds.Add(dw))
			return -1;

		if (!m_rgProps.Add(*pPropVar))
			return -1;
		int nIndex = m_rgProps.GetSize()-1;

#ifdef _DEBUG
		if (pPropVar->vt & VT_VECTOR)
		{
			ATLASSERT(nElemSize != 0);
			ATLASSERT(pData != NULL);
			ATLASSERT(nElems != 0);
		}
		else if (pPropVar->vt == VT_BSTR ||
			pPropVar->vt == VT_CLSID ||
			pPropVar->vt == VT_BLOB ||
			pPropVar->vt == VT_LPSTR ||
			pPropVar->vt == VT_LPWSTR)
		{
			ATLASSERT(pData != NULL);
		}
#endif

		if (pData)
		{
			if (nElemSize)
			{
				m_rgProps[m_rgProps.GetSize()-1].caub.cElems = nElems;
				m_rgProps[m_rgProps.GetSize()-1].caub.pElems = (BYTE*) pData;
			}
			else
				m_rgProps[m_rgProps.GetSize()-1].pwszVal = (WCHAR *) pData;
		}
		HRESULT hr = S_OK;
		if (!m_rgStatus.Add(hr))
			return -1;
		cProp++;
		aPropID = m_rgIds.GetData();
		aPropVar = m_rgProps.GetData();
		aStatus = m_rgStatus.GetData();
		
		return nIndex;
	}

	MQPROPVARIANT &GetProperty(int nIndex) throw()
	{
		return m_rgProps[nIndex];
	}

	int FindProperty(PROPID propId) throw()
	{
		int nSize = m_rgIds.GetSize();
		for (int i=0; i<nSize; i++)
		{
			if (m_rgIds[i]==propId)
				return i;
		}
		return -1;
	}
};

// s_MQMsgPropTypes is a map between MSMQ message property
// id to the type of the property
// the type can be retrieved by using s_MQMsgPropTypes[propId-PROPID_M_BASE]
extern "C" const __declspec(selectany) VARTYPE s_MQMsgPropTypes[] = 
{
	/*PROPID_M_BASE*/ VT_NULL,
	/*PROPID_M_CLASS*/ VT_UI2,
	/*PROPID_M_MSGID*/ VT_UI1|VT_VECTOR,
	/*PROPID_M_CORRELATIONID*/VT_UI1|VT_VECTOR,
	/*PROPID_M_PRIORITY*/ VT_UI1,
	/*PROPID_M_DELIVERY*/ VT_UI1,
	/*PROPID_M_ACKNOWLEDGE*/ VT_UI1, 
	/*PROPID_M_JOURNAL*/ VT_UI1,
	/*PROPID_M_APPSPECIFIC*/ VT_UI4,
	/*PROPID_M_BODY*/ VT_UI1|VT_VECTOR,
	/*PROPID_M_BODY_SIZE*/ VT_UI4,
	/*PROPID_M_LABEL*/ VT_LPWSTR,
	/*PROPID_M_LABEL_LEN*/ VT_UI4,
	/*PROPID_M_TIME_TO_REACH_QUEUE*/ VT_UI4,
	/*PROPID_M_TIME_TO_BE_RECEIVED*/ VT_UI4,
	/*PROPID_M_RESP_QUEUE*/ VT_LPWSTR,
	/*PROPID_M_RESP_QUEUE_LEN*/ VT_UI4,
	/*PROPID_M_ADMIN_QUEUE*/ VT_LPWSTR,
	/*PROPID_M_ADMIN_QUEUE_LEN*/ VT_UI4,
	/*PROPID_M_VERSION*/ VT_UI4,
	/*PROPID_M_SENDERID*/ VT_UI1|VT_VECTOR,
	/*PROPID_M_SENDERID_LEN*/ VT_UI4,
	/*PROPID_M_SENDERID_TYPE*/ VT_UI4,
	/*PROPID_M_PRIV_LEVEL*/ VT_UI4,
	/*PROPID_M_AUTH_LEVEL*/ VT_UI4,
	/*PROPID_M_AUTHENTICATED*/ VT_UI1,
	/*PROPID_M_HASH_ALG*/ VT_UI4,
	/*PROPID_M_ENCRYPTION_ALG*/ VT_UI4,
	/*PROPID_M_SENDER_CERT*/ VT_UI1|VT_VECTOR,
	/*PROPID_M_SENDER_CERT_LEN*/ VT_UI4,
	/*PROPID_M_SRC_MACHINE_ID*/ VT_CLSID,
	/*PROPID_M_SENTTIME*/ VT_UI4,
	/*PROPID_M_ARRIVEDTIME*/ VT_UI4,
	/*PROPID_M_DEST_QUEUE*/ VT_LPWSTR,
	/*PROPID_M_DEST_QUEUE_LEN*/ VT_UI4,
	/*PROPID_M_EXTENSION*/ VT_UI1|VT_VECTOR,
	/*PROPID_M_EXTENSION_LEN*/ VT_UI4,
	/*PROPID_M_SECURITY_CONTEXT*/ VT_UI4,
	/*PROPID_M_CONNECTOR_TYPE*/ VT_CLSID,
	/*PROPID_M_XACT_STATUS_QUEUE*/ VT_LPWSTR,
	/*PROPID_M_XACT_STATUS_QUEUE_LEN*/ VT_UI4,
	/*PROPID_M_TRACE*/ VT_UI1,
	/*PROPID_M_BODY_TYPE*/ VT_UI4,
	/*PROPID_M_DEST_SYMM_KEY*/ VT_UI1|VT_VECTOR,
	/*PROPID_M_DEST_SYMM_KEY_LEN*/ VT_UI4,
	/*PROPID_M_SIGNATURE*/ VT_UI1|VT_VECTOR,
	/*PROPID_M_SIGNATURE_LEN*/ VT_UI4,
	/*PROPID_M_PROV_TYPE*/ VT_UI4,
	/*PROPID_M_PROV_NAME*/ VT_LPWSTR,
	/*PROPID_M_PROV_NAME_LEN*/ VT_UI4,
};


// class CMQMessageProps
// Simple class to eliminate having to pass the property
// type of message properties
class CMQMessageProps : public CMQPropertyHolder<MQMSGPROPS>
{
protected:

public:
	int Add(MSGPROPID msgPropId, void *pData = NULL, int nElems=0, int nElemSize=0) throw()
	{
		ATLASSERT(msgPropId - PROPID_M_BASE < sizeof(s_MQMsgPropTypes)/sizeof(VARTYPE));
		return CMQPropertyHolder<MQMSGPROPS>::Add(msgPropId, s_MQMsgPropTypes[msgPropId - PROPID_M_BASE], pData, nElems, nElemSize);
	}

	template <typename T>
	int AddT(MSGPROPID msgPropId, T val)
	{
		ATLASSERT(msgPropId - PROPID_M_BASE < sizeof(s_MQMsgPropTypes)/sizeof(VARTYPE));
		CMQPropVariantIn var (val);
		ATLASSERT(var.vt == s_MQMsgPropTypes[msgPropId - PROPID_M_BASE]);
		return CMQPropertyHolder<MQMSGPROPS>::Add (msgPropId, &var);
	}
};

// s_MQQueuePropTypes is a map between MSMQ queue property
// id to the type of the property
// the type can be retrieved by using s_MQQueuePropTypes[propId-PROPID_Q_BASE]
extern "C" const __declspec(selectany)  VARTYPE s_MQQueuePropTypes[] = 
{
/*PROPID_Q_BASE*/ VT_NULL,
/*PROPID_Q_INSTANCE*/ VT_CLSID,
/*PROPID_Q_TYPE*/ VT_CLSID,
/*PROPID_Q_PATHNAME*/ VT_LPWSTR,
/*PROPID_Q_JOURNAL*/ VT_UI1,
/*PROPID_Q_QUOTA*/ VT_UI4,
/*PROPID_Q_BASEPRIORITY*/ VT_I2,
/*PROPID_Q_JOURNAL_QUOTA*/ VT_UI4,
/*PROPID_Q_LABEL*/ VT_LPWSTR,
/*PROPID_Q_CREATE_TIME*/ VT_I4,
/*PROPID_Q_MODIFY_TIME*/ VT_I4,
/*PROPID_Q_AUTHENTICATE*/ VT_UI1,
/*PROPID_Q_PRIV_LEVEL*/ VT_UI4,
/*PROPID_Q_TRANSACTION*/ VT_UI1
};

// class CMQQueueProps
// Simple class to eliminate having to pass the property
// type of queue properties
// When constructing the class you have to specify
// a transfer direction.  MQ_TRANSFER_DIR_GET means
// the class is being used to retrieve properties
// from MSMQ, so some properties are dynamically
// allocated and must be freed by MSMQ.
// MQ_TRANSFER_DIR_PUT means the class is being
// used to set properties and so the properties
// are allocated and freed by the user.
// 
class CMQQueueProps : public CMQPropertyHolder<MQQUEUEPROPS>
{
protected:

public:
	enum MQ_TRANSFER_DIR { MQ_TRANSFER_DIR_INVALID, MQ_TRANSFER_DIR_GET, MQ_TRANSFER_DIR_PUT };

	MQ_TRANSFER_DIR m_nDirection;

	CMQQueueProps(MQ_TRANSFER_DIR nDirection) throw()
	{
		m_nDirection = MQ_TRANSFER_DIR_INVALID;
		SetDirection(nDirection);
	}

	void SetDirection(MQ_TRANSFER_DIR nDirection) throw()
	{
		// if we're switching from GET to PUT
		// free allocated properties now

		if (m_nDirection == MQ_TRANSFER_DIR_GET && nDirection == MQ_TRANSFER_DIR_PUT)
			FreeAllocatedProperties();
		m_nDirection = nDirection;
	}
	
	void FreeAllocatedProperties() throw()
	{
		// this function should only be called when
		// the class is being used to retrieve queue properties
		ATLASSERT(m_nDirection == MQ_TRANSFER_DIR_GET);

		for (int i=0; i<m_rgProps.GetSize(); i++)
		{
			if (m_rgIds[i] == PROPID_Q_LABEL ||
				m_rgIds[i] == PROPID_Q_PATHNAME && m_rgProps[i].pwszVal)
			{
				MQFreeMemory(m_rgProps[i].pwszVal);
				m_rgProps[i].pwszVal = NULL;
			}
		}
	}

	~CMQQueueProps() throw()
	{
		if (m_nDirection == MQ_TRANSFER_DIR_GET)
			FreeAllocatedProperties();
	}

	int Add(QUEUEPROPID queuePropId, void *pData = NULL, int nElems=0, int nElemSize=0) throw()
	{
		ATLASSERT(queuePropId - PROPID_Q_BASE < sizeof(s_MQQueuePropTypes)/sizeof(VARTYPE));
		return Add(queuePropId, s_MQQueuePropTypes[queuePropId - PROPID_Q_BASE], pData, nElems, nElemSize);
	}

	int Add(PROPID propId, VARTYPE vt, void *pData = NULL, int nElems=0, int nElemSize=0) throw()
	{
		MQPROPVARIANT propVar;
		propVar.vt = vt;
		return Add(propId, &propVar, pData, nElems, nElemSize);
	}

	int Add(PROPID propId, MQPROPVARIANT *pPropVar, void *pData = NULL, int nElems=0, int nElemSize=0) throw()
	{
		if (m_nDirection == MQ_TRANSFER_DIR_GET && 
				(propId==PROPID_Q_LABEL || propId==PROPID_Q_PATHNAME))
		{
			// special handling for these two
			// when getting properties, vt should be set to VT_NULL

			ATLASSERT(pData == NULL);	// the result will be dynamially allocated by MSMQ
			pPropVar->vt = VT_NULL;
		}
		return CMQPropertyHolder<MQQUEUEPROPS>::Add(propId, pPropVar, pData, nElems, nElemSize);
	}
};

// s_MQMachinePropTypes is a map between MSMQ machine property
// id to the type of the property
// the type can be retrieved by using s_MQMachinePropTypes[propId-PROPID_QM_BASE]
extern "C" const __declspec(selectany)  VARTYPE s_MQMachinePropTypes[] = 
{
/*PROPID_QM_BASE*/ VT_NULL,
/*PROPID_QM_SITE_ID*/ VT_CLSID,
/*PROPID_QM_MACHINE_ID*/ VT_CLSID,
/*PROPID_QM_PATHNAME*/ VT_NULL,
/*PROPID_QM_CONNECTION*/ VT_LPWSTR|VT_VECTOR,
/*PROPID_QM_ENCRYPTION_PK*/ NULL,
};

// class CMQMachineProps
// Simple class to eliminate having to pass the property
// type of machine properties
// The class frees properties that are dynamically allocated
// by MSMQ
// 
class CMQMachineProps : public CMQPropertyHolder<MQQMPROPS>
{
protected:

public:
	~CMQMachineProps() throw()
	{
		FreeAllocatedProperties();
	}

	void FreeAllocatedProperties() throw()
	{
		for (int i=0; i<m_rgProps.GetSize(); i++)
		{
			// TODO: I haven't been able to get PROPID_QM_CONNECTION and PROPID_QM_ENCRYPTION_PK
			// to work, with any VT
			// the docs seem to indicate that they should be freed, but not very
			// clear about it!
			if ((m_rgIds[i] ==  PROPID_QM_PATHNAME && m_rgProps[i].pwszVal))
			{
				MQFreeMemory(m_rgProps[i].pwszVal);
				m_rgProps[i].pwszVal = NULL;
			}
		}
	}

	int Add(QMPROPID propId, void *pData = NULL, int nElems=0, int nElemSize=0) throw()
	{
		ATLASSERT(propId - PROPID_QM_BASE < sizeof(s_MQMachinePropTypes)/sizeof(VARTYPE));
		return CMQPropertyHolder<MQQMPROPS>::Add(propId, s_MQMachinePropTypes[propId - PROPID_QM_BASE], pData, nElems, nElemSize);
	}
};


class CMQQueue;


// struct CMQOverlapped
// This is the structure that will be used
// with IoCompletion port based io
struct CMQOverlapped : public OVERLAPPED
{
	CMQMessageProps *m_pProps;	// the message props associated with the request
	CMQQueue *m_pQueue;			// the queue associated with the request
};


// class CMQQueue
// This class encapsulates MSMQ queue related methods
class CMQQueue
{
protected:
	HANDLE m_hQueue;
	WCHAR m_wszFormatName[MQ_MAX_Q_NAME_LEN+1];
	
public:
	CMQQueue() throw()
	{
		m_hQueue = NULL;
		m_wszFormatName[0] = '\0';
	}

	HANDLE GetHandle() throw()
	{
		return m_hQueue;
	}

	operator HANDLE() throw()
	{
		return m_hQueue;
	}

	BOOL SetFormatName(LPCWSTR wszFormatName) throw()
	{
		int nLen = (int) wcslen(wszFormatName);
		if (nLen == 0 || nLen > MQ_MAX_Q_NAME_LEN)
			return FALSE;
		wcscpy(m_wszFormatName, wszFormatName);
		return TRUE;
	}

	HRESULT PathNameToFormatName(LPCWSTR wszPathName) throw()
	{
		DWORD dwDestFormatLen = sizeof(m_wszFormatName)/sizeof(WCHAR);
		return MQPathNameToFormatName(wszPathName, m_wszFormatName, &dwDestFormatLen);
	}

	HRESULT Open(
		LPCWSTR wszPathName,
		DWORD dwAccessMode=MQ_RECEIVE_ACCESS,
		DWORD dwShareMode=MQ_DENY_NONE) throw()
	{
		ATLASSERT(m_hQueue == NULL);	// You should the queue, before opening another one


		HRESULT hr = PathNameToFormatName(wszPathName);
		if (FAILED(hr))
			return hr;

		return Open(dwAccessMode, dwShareMode);
	}

	// open that uses the current format name
	HRESULT Open(DWORD dwAccessMode=MQ_RECEIVE_ACCESS, DWORD dwShareMode=MQ_DENY_NONE) throw()
	{
		ATLASSERT(m_wszFormatName[0]);
		return MQOpenQueue(m_wszFormatName, dwAccessMode, dwShareMode, &m_hQueue);
	}

	HRESULT FormatNameFromHandle(LPWSTR wszFormatName, LPDWORD pdwCount) throw()
	{
		ATLASSERT(m_hQueue);
		return MQHandleToFormatName(m_hQueue,  wszFormatName,  pdwCount);
	}

	HRESULT AssociateCompletionPort(HANDLE hIoCompletion, DWORD dwNumThreads=0) throw()
	{
		ATLASSERT(m_hQueue != NULL);

		HANDLE hIoCompletionRet = CreateIoCompletionPort(m_hQueue, hIoCompletion, (ULONG_PTR) this, dwNumThreads);
		if (!hIoCompletionRet)
			return AtlHresultFromLastError();
		return S_OK;
	}


	HRESULT Receive(MQMSGPROPS *pMsgProps) throw()
	{
		ATLASSERT(m_hQueue != NULL);
		return MQReceiveMessage(m_hQueue, 0, MQ_ACTION_RECEIVE, pMsgProps, NULL, NULL, NULL, NULL);
	}

	HRESULT Peek(MQMSGPROPS *pMsgProps, HANDLE hCursor) throw()
	{
		ATLASSERT(m_hQueue != NULL);
		return MQReceiveMessage(m_hQueue, 0, MQ_ACTION_PEEK_CURRENT, pMsgProps, NULL, NULL, hCursor, NULL);
	}

	HRESULT PeekNext(MQMSGPROPS *pMsgProps, HANDLE hCursor) throw()
	{
		ATLASSERT(m_hQueue != NULL);
		return MQReceiveMessage(m_hQueue, 0, MQ_ACTION_PEEK_NEXT, pMsgProps, NULL, NULL, hCursor, NULL);
	}

	HRESULT ReceiveWithCallback(
		CMQMessageProps *pProps,
		PMQRECEIVECALLBACK pfnCallback,
		DWORD dwTimeout=INFINITE) throw()
	{
		ATLASSERT(m_hQueue != NULL);
		return MQReceiveMessage(m_hQueue, dwTimeout, MQ_ACTION_RECEIVE, pProps, NULL, pfnCallback, NULL, NULL);
	}


	HRESULT ReceiveOverlapped(
		CMQMessageProps *pProps,
		OVERLAPPED *pOverlapped,
		DWORD dwTimeout=INFINITE) throw()
	{
		ATLASSERT(m_hQueue != NULL);
		return MQReceiveMessage(m_hQueue, dwTimeout, MQ_ACTION_RECEIVE, pProps, pOverlapped, NULL, NULL, NULL);
	}

	HRESULT ReceiveWithIoCompletion(CMQMessageProps *pProps, DWORD dwTimeout=INFINITE) throw()
	{
		ATLASSERT(m_hQueue != NULL);

		// The CMQOverlapped structure allocated here, must be
		// freed when the request is retrieved from the
		// IoCompletion port
		CMQOverlapped *pOverlapped = NULL;
		ATLTRY(pOverlapped = new CMQOverlapped);
		if (!pOverlapped)
			return E_OUTOFMEMORY;
		memset(pOverlapped, 0x00, sizeof(CMQOverlapped));

		pOverlapped->m_pProps = pProps;
		pOverlapped->m_pQueue = this;
		return MQReceiveMessage(m_hQueue, dwTimeout, MQ_ACTION_RECEIVE, pProps, pOverlapped, NULL, NULL, NULL);
	}

	HRESULT Send(MQMSGPROPS *pMsgProps, ITransaction *pTransaction = NULL) throw()
	{
		ATLASSERT(m_hQueue != NULL);
		return MQSendMessage(GetHandle(), pMsgProps, pTransaction);
	}

	HRESULT Close() throw()
	{
		HRESULT hr = S_OK;
		if (m_hQueue)
			hr = MQCloseQueue(m_hQueue);
		m_hQueue = NULL;
		return hr;
	}
		
	~CMQQueue() throw()
	{
		if (m_hQueue)
			Close();
	}

	HRESULT Create(MQQUEUEPROPS *pQueueProps, PSECURITY_DESCRIPTOR pSecurityDescriptor=NULL) throw()
	{
		DWORD dwFormatNameLen = sizeof(m_wszFormatName)/sizeof(WCHAR);
		return MQCreateQueue(pSecurityDescriptor, pQueueProps, m_wszFormatName, &dwFormatNameLen);
	}

	HRESULT Create(
		LPCWSTR wszPathName,
		LPCWSTR wszLabel,
		BOOL bTransactional=FALSE,
		PSECURITY_DESCRIPTOR pSecurityDescriptor=NULL) throw()
	{
		CMQQueueProps qProps(CMQQueueProps::MQ_TRANSFER_DIR_PUT);
		qProps.Add(PROPID_Q_PATHNAME, (void *) wszPathName);
		qProps.Add(PROPID_Q_LABEL, (void *) wszLabel);
		if (bTransactional)
		{
			int nIndex = qProps.Add(PROPID_Q_TRANSACTION);
			qProps.m_rgProps[nIndex].bVal = MQ_TRANSACTIONAL;
		}
		return Create(&qProps, pSecurityDescriptor);
	}


	HRESULT Delete() throw()
	{
		ATLASSERT(m_wszFormatName[0]);	// you should set the format name first
		return MQDeleteQueue(m_wszFormatName);
	}

	HRESULT GetQueueProperties(MQQUEUEPROPS *pQueueProps) throw()
	{
		ATLASSERT(m_wszFormatName[0]);	// you should set the format name first
		return MQGetQueueProperties(m_wszFormatName, pQueueProps);
	}

	HRESULT SetQueueProperties(MQQUEUEPROPS *pQueueProps) throw()
	{
		ATLASSERT(m_wszFormatName[0]);	// you should set the format name first
		return MQSetQueueProperties(m_wszFormatName, pQueueProps);
	}

	HRESULT CreateCursor(HANDLE *phHandle) throw()
	{
		ATLASSERT(m_hQueue);
		ATLASSERT(phHandle);
		return MQCreateCursor(m_hQueue, phHandle);
	}

	static HRESULT CloseCursor(HANDLE hCursor) throw()
	{
		ATLASSERT(hCursor!=NULL);
		return MQCloseCursor(hCursor);
	}

	static HRESULT BeginTransaction(ITransaction **ppTransaction) throw()
	{
		ATLASSERT(ppTransaction);
		return MQBeginTransaction(ppTransaction);
	}
};


// class CMQCursor
// This class encapsulates MSMQ cursor related methods
class CMQCursor
{
protected:
	HANDLE m_hCursor;
public:
	CMQCursor() throw() : m_hCursor(NULL)
	{

	}

	~CMQCursor() throw()
	{
		if (m_hCursor)
			Close();
	}

	void Attach(HANDLE hCursor) throw()
	{
		ATLASSERT(m_hCursor==NULL);
		m_hCursor = hCursor;
	}

	HANDLE Detach() throw()
	{
		HANDLE hCursor = m_hCursor;
		m_hCursor = NULL;
		return hCursor;
	}

	HRESULT Create(HANDLE hQueue) throw()
	{
		ATLASSERT(m_hCursor==NULL);
		return MQCreateCursor(hQueue, &m_hCursor);
	}

	HRESULT Close() throw()
	{
		ATLASSERT(m_hCursor!=NULL);
		HRESULT hr = MQCloseCursor(m_hCursor);
		m_hCursor = NULL;
		return hr;
	}

	operator HANDLE() throw()
	{
		return m_hCursor;
	}
};

// class CMQColumnSet
// Simple wrapper around an MSQM column set
class CMQColumnSet : public MQCOLUMNSET
{
protected:
	CAtlArray<PROPID> m_Ids;

public:
	CMQColumnSet() throw()
	{
		cCol = 0;
		aCol = NULL;
	}

	int Add(PROPID propId) throw()
	{
		int nIndex = (int)m_Ids.Add(propId);
		if (nIndex < 0)
			return nIndex;
		// update the MQCOLUMNSET members
		cCol = (int)m_Ids.GetCount();
		aCol = m_Ids.GetData();
		return nIndex;
	}
};

// class CMQPropVariantIn
// Simple wrapper around an MQPROPVARIANT
// Notice that this class is not suitable
// for receiving property values from MSMQ
// since it allocates memory using malloc
// CMQPropVariantRet can be used to receive
// property values from MSMQ
class CMQPropVariantIn : public MQPROPVARIANT
{
public:
	CMQPropVariantIn() throw()
	{
		vt = VT_NULL;
	}

	~CMQPropVariantIn() throw()
	{
		Clear();
	}

	CMQPropVariantIn(int n, VARTYPE vType=VT_I4) throw()
	{
		Set(n, vType);
	}

	CMQPropVariantIn(UINT n) throw()
	{
		Set((int) n, VT_UI4);
	}

	CMQPropVariantIn(unsigned char src) throw()
	{
		Set(src);
	}

	CMQPropVariantIn(short src) throw()
	{
		Set(src);
	}

	CMQPropVariantIn(USHORT src) throw()
	{
		Set(src);
	}

	CMQPropVariantIn(bool src) throw()
	{
		Set(src);
	}

	void Set(int n, VARTYPE vType=VT_I4) throw()
	{
		Clear();
		vt = vType;
		lVal = (long) n;
	}

	void Set(unsigned char b) throw()
	{
		Clear();
		vt = VT_UI1;
		bVal = b;
	}

	void Set(short s) throw()
	{
		Clear();
		vt = VT_I2;
		iVal = s;
	}

	void Set(USHORT us) throw()
	{
		Clear();
		vt = VT_UI2;
		uiVal = us;
	}

	void Set(bool b) throw()
	{
		Clear();
		vt = VT_BOOL;
		boolVal = b ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	BOOL Set(LPCWSTR wsz) throw()
	{
		Clear();
		DWORD dwLen = (DWORD) (wcslen(wsz)+1)*2;
		pwszVal = (LPWSTR) malloc(dwLen);
		if (!pwszVal)
			return FALSE;
		memcpy(pwszVal, wsz, dwLen);
		vt = VT_LPWSTR;
		return TRUE;
	}

	BOOL Set(CLSID *pclsid) throw()
	{
		Clear();
		puuid = (CLSID *) malloc(sizeof(CLSID));
		if (!puuid)
			return FALSE;
		memcpy(puuid, pclsid, sizeof(CLSID));
		vt = VT_CLSID;
		return TRUE;
	}

	// other overrides for set

	void Clear() throw()
	{
		void *pvFree = NULL;

		switch (vt)
		{
		case VT_CLSID:
			pvFree = puuid;
			break;

		case VT_LPWSTR:
			pvFree = pwszVal;
			break;

		}
		if (pvFree)
			free(pvFree);
		vt = VT_NULL;
	}
};


// class CMQPropVariantRet
// Simple wrapper around an MQPROPVARIANT
// Notice that this class is only suitable
// for receiving property values from MSMQ
class CMQPropVariantRet : public MQPROPVARIANT
{
public:
	CMQPropVariantRet() throw()
	{
		vt = VT_NULL;
	}

	~CMQPropVariantRet() throw()
	{
		Clear();
	}

	void Clear() throw()
	{
		void *pvFree = NULL;

		switch (vt)
		{
		case VT_CLSID:
			pvFree = puuid;
			break;

		case VT_LPWSTR:
			pvFree = pwszVal;
			break;

		}
		if (pvFree)
			MQFreeMemory(pvFree);
		vt = VT_NULL;
	}
};


// class CMQPropertyRestriction
// a class that encapsulates an MSMQ property restriction
class CMQPropertyRestriction : public MQPROPERTYRESTRICTION
{
public:
	CMQPropertyRestriction() throw()
	{
		rel = PREQ;
		prop = 0;
		prval.vt = VT_NULL;
	}

	CMQPropertyRestriction(ULONG rel, PROPID propId, MQPROPVARIANT *pVal) throw()
	{
		Set(rel, propId, pVal);
	}

	void Set(ULONG ulRel, PROPID propId, MQPROPVARIANT *pVal) throw()
	{
		ATLASSERT(ulRel==PRLT || ulRel==PRLE || ulRel==PRGE || ulRel==PREQ || ulRel==PRNE);
		ATLASSERT(pVal);

		// todo: figure out how to free current!
		rel = ulRel;
		prop = propId;
		prval = *pVal;
		pVal->vt = VT_NULL;
	}
};


// class CMQRestriction
// a class that encapsulates an MSMQ restriction
class CMQRestriction : public MQRESTRICTION
{
protected:
	CAtlArray<CMQPropertyRestriction, CElementTraitsBase<CMQPropertyRestriction> > m_propRestrictions;

public:
	CMQRestriction() throw()
	{
		cRes = 0;
		paPropRes = NULL;
	}


	int Add(CMQPropertyRestriction &res) throw()
	{
		int nIndex = (int)m_propRestrictions.Add(res);
		if (nIndex < 0)
			return nIndex;
		cRes = (int)m_propRestrictions.GetCount();
		paPropRes = m_propRestrictions.GetData();
		return nIndex;
	}
};

// class CMQLocator
// a class the encapsulates the MSMQ Locator APIs
class CMQLocator
{
protected:
	HANDLE m_hEnum;

public:
	CMQLocator() throw()
	{
		m_hEnum = NULL;
	}
	
	~CMQLocator() throw()
	{
		if (m_hEnum)
			End();
	}

	HRESULT Begin(MQRESTRICTION *pRestriction, MQCOLUMNSET *pColumns, MQSORTSET *pSort=NULL) throw()
	{
		ATLASSERT(m_hEnum == NULL);
		ATLASSERT(pColumns != NULL);
		return MQLocateBegin(NULL, pRestriction, pColumns, pSort, &m_hEnum);
	}
	
	HRESULT Next(DWORD *pcProps, MQPROPVARIANT *aPropVar) throw()
	{
		ATLASSERT(m_hEnum != NULL);
		ATLASSERT(pcProps);
		ATLASSERT(aPropVar);
		return MQLocateNext(m_hEnum, pcProps, aPropVar);
	}

	HRESULT End() throw()
	{
		ATLASSERT(m_hEnum != NULL);
		HRESULT hr = MQLocateEnd(m_hEnum);
		m_hEnum = NULL;
		return hr;
	}
};
} // namespace ATL

#endif // __ATLMQ_H__
