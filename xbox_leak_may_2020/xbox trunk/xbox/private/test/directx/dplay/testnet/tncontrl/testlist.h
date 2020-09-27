#ifndef __TNCONTROL_TESTLIST__
#define __TNCONTROL_TESTLIST__
//#pragma message("Defining __TNCONTROL_TESTLIST__")






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
class DLLEXPORT CTNTestMsList:public LLIST
{
	// These are friends so they can access the protected members
#ifndef _XBOX 
	friend CTNMaster;						// Masters not supported
	friend CTNReport;						// Reports not supported
#endif // ! XBOX
	friend CTNSlaveInfo;
	friend CTNTestInstanceM;


	protected:
		CTNTestMsList(void);
		virtual ~CTNTestMsList(void);

		PTNTESTINSTANCEM GetTopLevelTest(DWORD dwUniqueTestID);
		//HRESULT AddTestInOrder(PTNTESTINSTANCE pTest);
};

class DLLEXPORT CTNTestSsList:public LLIST
{
	// These are friends so they can access the protected members
	//friend CTNSlave;
	friend CTNExecutorPriv;


	protected:
		CTNTestSsList(void);
		virtual ~CTNTestSsList(void);

		PTNTESTINSTANCES GetTopLevelTest(DWORD dwUniqueTestID);
		//HRESULT AddTestInOrder(PTNTESTINSTANCE pTest);
};






#else //__TNCONTROL_TESTLIST__
//#pragma message("__TNCONTROL_TESTLIST__ already included!")
#endif //__TNCONTROL_TESTLIST__
