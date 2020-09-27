#ifndef __TNCONTROL_SENDQUEUE__
#define __TNCONTROL_SENDQUEUE__
//#pragma message("Defining __TNCONTROL_SENDQUEUE__")






//==================================================================================
// Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG






//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNSendData:public LLITEM
{
	public:
		BOOL		m_fGuaranteed; // do we care if this actually gets there?
		PVOID		m_pvAddress; // pointer to address data
		DWORD		m_dwAddressSize; // size of address data
		PVOID		m_pvData; // pointer to message data
		DWORD		m_dwDataSize; // size of message data
		PVOID		m_pvSubmittersAddress; // pointer to function which place this item on the queue


		CTNSendData(void);
		virtual ~CTNSendData(void);
};

class DLLEXPORT CTNSendDataQueue:public LLIST
{
	public:
		// This is only public so that the threads can access it
		HANDLE		m_hSendDataEvent; // there is new data to send


		CTNSendDataQueue(void);
		virtual ~CTNSendDataQueue(void);

		HRESULT AddSendData(PTNSENDDATA pNewSendData);
		HRESULT RemoveAllItemsTo(PVOID pvAddress, DWORD dwAddressSize);
};






#else //__TNCONTROL_SENDQUEUE__
//#pragma message("__TNCONTROL_SENDQUEUE__ already included!")
#endif //__TNCONTROL_SENDQUEUE__
