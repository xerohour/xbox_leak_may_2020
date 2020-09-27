///////////////////////////////////////////////////////////////////////////////
//      SYSCASES.CPP
//
//      Created by :                    Date :
//              ChrisKoz                                  1/25/96
//
//      Description :
//              Implementation of the creation & classview in Galileo project
//

#include "stdafx.h"
#include <io.h>
#include "syscase.h"
#include "sniff.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//	CConnectTest class


IMPLEMENT_TEST(CConnectTest, CTest, "Connect to SQL server", -1, CDataSubSuite)

BOOL CConnectTest::RunAsDependent(void)
{
		return FALSE; //never run because it confuses
	// name of the project file
	// if(!GetSubSuite()->m_prj.IsOpen())  this does not work
/*	if(!GetSubSuite()->m_projOpen)
	{
		CString strPrjName=m_strCWD + PROJDIR + PROJNAME + ".DSW";
		if(GetSubSuite()->m_prj.Open(strPrjName)!=ERROR_SUCCESS)
			GetLog()->RecordFailure("Coudn't open project - %s",strPrjName);
		else
		{
			GetSubSuite()->m_projOpen=-1;
			Sleep(2000); //safety
		}
	} */
//	if(FindDataInWkspace(&GetSubSuite()->m_DataInfo)==ERROR_SUCCESS)
//	{

//		return FALSE;
//	}
//	else {
//		return TRUE;
//	}
}



void CConnectTest::Run(void)
{
	BOOL bResult=ERROR_ERROR;
	// if(!GetSubSuite()->m_prj.IsOpen())  this does not work
/*	if(!GetSubSuite()->m_projOpen) I nuked the old C++ project stuff
	{
		CString strPrjName=m_strCWD + PROJDIR + PROJNAME + ".DSW";
		if(GetSubSuite()->m_prj.Open(strPrjName)!=ERROR_SUCCESS)
			GetLog()->RecordFailure("Coudn't open project - %s",strPrjName);
		else
		{
			GetSubSuite()->m_projOpen=-1;
			Sleep(2000);
		}
	} */
//	if((bResult=FindDataInWkspace(&GetSubSuite()->m_DataInfo))!=ERROR_SUCCESS)
//	{
		CDataInfo *pDataInfo=&GetSubSuite()->m_DataInfo;
		bResult=GetSubSuite()->m_prj.NewProject(DATABASE_PROJECT,
			CString(PROJNAME),GetCWD()+PROJDIR,GetUserTargetPlatforms(),TRUE);
		bResult=GetSubSuite()->m_prj.AddSQLDataSource(
			pDataInfo->m_strDSN,
			pDataInfo->m_strUser,
			pDataInfo->m_strPassWord,
			pDataInfo->m_strDataBase);
//	}
	if(!GetLog()->RecordCompare(bResult == ERROR_SUCCESS, 
					"Opening the SQL Server '%s' database %s.",
					GetSubSuite()->m_DataInfo.m_strServer,
					GetSubSuite()->m_DataInfo.m_strDataBase))
		throw CTestException("Cannot establish the connection; unable to continue.", 
				CTestException::causeOperationFail);
	GetSubSuite()->m_prj.Close(TRUE); GetSubSuite()->m_projOpen=0;	
} //CConnectTest::Run()



