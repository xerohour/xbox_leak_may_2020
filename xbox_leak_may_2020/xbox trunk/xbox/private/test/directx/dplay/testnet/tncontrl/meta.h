#ifndef _XBOX // ! no master supported
#ifndef __TNCONTROL_META__
#define __TNCONTROL_META__
//#pragma message("Defining __TNCONTROL_META__")






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
class DLLEXPORT CTNMetaMaster:public CTNControlLayer
{
	private:
		CTNMastersList	m_masters;



		HRESULT HandleQueryMetaMasterMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_QUERYMETAMASTER pQueryMetaMasterMsg);

		HRESULT HandleRegisterMasterMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_REGISTERMASTER pRegisterMasterMsg);

		HRESULT HandleMasterUpdateMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_MASTERUPDATE pMasterUpdateMsg);

		HRESULT HandleUnregisterMasterMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_UNREGISTERMASTER pUnregisterMasterMsg);


	public:
		CTNMetaMaster(void);
		virtual ~CTNMetaMaster(void);


		HRESULT StartupMetaMaster(PTNSTARTUPMETAMASTERDATA pStartupMetaMasterData);
		HRESULT CleanupMetaMaster(void);

		// Virtual implementations
		HRESULT HandleMessage(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
							  PVOID pvData, DWORD dwDataSize);
};







#else //__TNCONTROL_META__
//#pragma message("__TNCONTROL_META__ already included!")
#endif //__TNCONTROL_META__
#endif // ! XBOX
