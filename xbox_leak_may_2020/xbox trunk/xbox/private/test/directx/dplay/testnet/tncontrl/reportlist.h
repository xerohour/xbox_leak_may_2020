#ifndef _XBOX // reports not supported
#ifndef __TNCONTROL_REPORTLIST__
#define __TNCONTROL_REPORTLIST__
//#pragma message("Defining __TNCONTROL_REPORTLIST__")






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
class DLLEXPORT CTNReportsList:public LLIST
{
	// This is a friends so it can use the object.
#ifndef _XBOX // ! no master supported
	friend class CTNMaster;
#endif // ! XBOX


	protected:
		int		m_iMaxNumEntries; // maximum number of entries to allow to accumulate in the list



		CTNReportsList(void);
		virtual ~CTNReportsList(void);



		HRESULT AddReport(DWORD dwType, int iNumMachines, PTNSLAVEINFO* aMachines,
						PTNTESTINSTANCEM pTest, HRESULT hresult,
						PVOID pvData, DWORD dwDataSize);

		PTNREPORT GetNextMatchingReport(PTNREPORT pStartFromItem,
										DWORD dwMatchFlags,
										PTNSLAVESLIST pMachineList,
										PTNTESTMSLIST pTestList,
										BOOL fRemoveStartFromItem);

		void SetMaxNumberReports(int iNumReports);
};






#else //__TNCONTROL_REPORTLIST__
//#pragma message("__TNCONTROL_REPORTLIST__ already included!")
#endif //__TNCONTROL_REPORTLIST__
#endif // ! XBOX