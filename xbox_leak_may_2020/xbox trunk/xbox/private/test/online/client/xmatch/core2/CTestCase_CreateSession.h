#ifndef __CTESTCASE_CREATESESSION_H__
#define __CTESTCASE_CREATESESSION_H__

#include "TestCaseAndResult_CreateSession.h"
#include "CParameter_CreateSession.h"

namespace MatchCoreTestNamespace {

//Parameter combinations
enum DWordCase
{
	DWZero,
	DWOne,
	DWMaxSignedInt,
	DWMinSignedInt,
	DWMaxDword,

	EndOFDWordCase
};

enum AttributeCase
{
	ZeroNum_Null,
	ZeroNum_Alloc,
	ZeroNum_Garbage,
	One_Int,
	One_Str,
	One_Blob,
	One_EachType,
	Int1000,
	Str1000,
	Blob1000,
// Failure cases	
	OneNum_Null,	
	TwoNum_OneAlloc,
	OneNum_Garbage,

	EndOfDAttributeCase
};

enum EventCase
{
	NullEvent,
	ValidEvent,
// Failure cases	
	GarbageEvent,

	EndOfEventCase
};

enum TaskCreateCase
{
	ValildTask,
// Failure cases	
	NullTask,

	EndOfTaskCreateCase
};
	
class CParameter_CreateSession;

class CTestCase_CreateSession
{
private:
	INT				m_nCase;

	TestCaseAndExpected_CreateSession	m_tcae;

	CParameter_CreateSession*	m_pPara;
	CParameter_CreateSession*	m_pParaUpdate;

	void 	EvaluateAttr(TestCase_CreateSession& tc, CParameter_CreateSession* pPara);
	void	EvaluateHEvent(TestCase_CreateSession& tc, CParameter_CreateSession* pPara);
	void	EvaluateTask(TestCase_CreateSession& tc, CParameter_CreateSession* pPara);


	INT	CheckResultOf_SessionCreate(HRESULT hr, XONLINETASK_HANDLE* phMatchTask);
	INT	CheckResultOf_SessionGetID(HRESULT hr, XNKID SessionID);
	INT	CheckResultOf_SessionDelete(HRESULT hr, XNKID SessionID);

public:
	CTestCase_CreateSession();
	~CTestCase_CreateSession();
	void	BringPameterAndExpected();
	void	EvaluateParameters(TestCase_CreateSession& tc, CParameter_CreateSession* pPara);
	
	int		RunTest();
//	CParameter_CreateSession GetPara();
	void SetnCase(int ii) {m_nCase = ii;}


};

DWORD EvaluateSlot(INT nCaseSlot);

}
#endif


