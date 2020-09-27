#ifndef _XBOX // ! no master supported
#ifndef __TNCONTRL_MASTERLIST__
#define __TNCONTRL_MASTERLIST__
//#pragma message("Defining __TNCONTRL_MASTERLIST__")






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
class DLLEXPORT CTNMastersList:public LLIST
{
	public:
		/*
		CTNMastersList(void);
		virtual ~CTNMastersList(void);
		*/

		PTNMASTERINFO GetMasterByID(PTNCTRLMACHINEID pID);
};






#else //__TNCONTRL_MASTERLIST__
//#pragma message("__TNCONTRL_MASTERLIST__ already included!")
#endif //__TNCONTRL_MASTERLIST__
#endif // ! XBOX
