#ifndef _XBOX // ! no master supported
#ifndef __TNCONTROL_MASTERINFO__
#define __TNCONTROL_MASTERINFO__
//#pragma message("Defining __TNCONTROL_MASTERINFO__")






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
// Classes
//==================================================================================
class DLLEXPORT CTNMasterInfo:public LLITEM
{
	// These are friends so they can access the protected members
	friend class CTNMastersList;
	friend class CTNMetaMaster;


	protected:
		TNCTRLMACHINEID		m_id; // ID of this slave
		DWORD				m_dwControlVersion; // version of control layer API master has
		DWORD				m_dwSessionID; // self described session ID of this master
		TNMODULEID			m_moduleID; // ID of module the master is using
		DWORD				m_dwMode; // mode the master is in
		char*				m_pszSessionFilter; // user specified session filter string
		COMMDATA			m_commdata; // structure with ctrl comm related data in it
		char*				m_pszAddress; // string address for slaves to use to reach the master
		int					m_iNumSlaves; // number of slaves connected to it
		BOOL				m_fJoinersAllowed; // whether this master is currently accepting new joiners


	public:
		CTNMasterInfo(void);
		virtual ~CTNMasterInfo(void);
};






#else //__TNCONTROL_MASTERINFO__
//#pragma message("__TNCONTROL_MASTERINFO__ already included!")
#endif //__TNCONTROL_MASTERINFO__
#endif // ! XBOX
