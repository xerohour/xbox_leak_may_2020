#ifndef __TNCONTRL_SLAVELIST__
#define __TNCONTRL_SLAVELIST__
//#pragma message("Defining __TNCONTRL_SLAVELIST__")






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
class DLLEXPORT CTNSlavesList:public LLIST
{
	public:
		/*
		CTNSlavesList(void);
		virtual ~CTNSlavesList(void);
		*/

		PTNSLAVEINFO GetSlaveByID(PTNCTRLMACHINEID pID);
};






#else //__TNCONTRL_SLAVELIST__
//#pragma message("__TNCONTRL_SLAVELIST__ already included!")
#endif //__TNCONTRL_SLAVELIST__
