#ifndef __TNCONTROL_SNDCARDS__
#define __TNCONTROL_SNDCARDS__
//#pragma message("Defining __TNCONTROL_SNDCARDS__")





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
class DLLEXPORT CTNSoundCard:public CLString
{
	// These are friends so they can access the protected members.
	friend CTNMachineInfo;
	friend CTNSoundCardsList;


	protected:
		GUID	m_guidDSoundID; // GUID DSound uses to identify this device
		BOOL	m_fHalfDuplex; // whether this device is known to be half-duplex


	public:
		CTNSoundCard(char* szName, GUID* pguidDSoundID, BOOL fHalfDuplex);
		virtual ~CTNSoundCard(void);

		GUID* GetDSoundGUID(void);
		BOOL IsKnownHalfDuplex(void);
};

class DLLEXPORT CTNSoundCardsList:public CLStringList
{
	// These are friends so they can access the protected members.
	friend CTNMachineInfo;
	friend CTNSlaveInfo;


	protected:
		//CTNSoundCardsList(void);
		//virtual ~CTNSoundCardsList(void);

		HRESULT PackIntoBuffer(PVOID pvBuffer, DWORD* pdwSize);
		HRESULT UnpackFromBuffer(PVOID pvBuffer, DWORD dwSize);
#ifndef _XBOX // no file printing supported
		void PrintToFile(HANDLE hFile);
#endif // ! XBOX
};





#else //__TNCONTROL_SNDCARDS__
//#pragma message("__TNCONTROL_SNDCARDS__ already included!")
#endif //__TNCONTROL_SNDCARDS__
