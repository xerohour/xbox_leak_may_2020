///////////////////////////////////////////
// 
// envrvar.cpp
//
// CEnvironmentVariableList class
// moved from projtool.cpp
///////////////////////////////////////////

#include "stdafx.h"

#pragma hdrstop

#define _SUSHI_PROJECT
#include "utilbld_.h"

///////////////////////////////////////////////////////////////////////////////
//
// -------------------------- CEnvironmentVariableList ------------------------
//
///////////////////////////////////////////////////////////////////////////////
//#define ENVTEST

void CEnvironmentVariableList::AddVariable (
								const TCHAR *pName,
								const TCHAR *pValue
								)
{
	m_VariableNames.Add ( pName );
	m_VariableValues.Add (pValue );
}

#define INIT_BUF_SIZE 128
void CEnvironmentVariableList::SetVariables (BOOL bSaveOldValues  /*= TRUE*/ )
{
	m_bHoldingOld = bSaveOldValues;
	#ifdef ENVTEST
	{
		TCHAR *pc = (char *) GetEnvironmentStrings ();
		TRACE ("Dumping environment before CEnvironmentVariableList::SetVariables:\n");
		while (*pc != 0)
		{
			TRACE ("    %s\n", pc);
			pc += strlen (pc);
			pc++;
		}
	}
	#endif	
	
	int i, j = m_VariableValues.GetSize (), k;
	CString str;
	TCHAR *pc = str.GetBuffer ( INIT_BUF_SIZE );
							
	for (i=0; i<j; i++ )
	{
		if (bSaveOldValues)
		{
		 	k = GetEnvironmentVariable (
		 							(LPTSTR) (LPCSTR) m_VariableNames[i],
		 							pc,
		 							INIT_BUF_SIZE
		 							);
			if ( k > INIT_BUF_SIZE )
			{
				str.ReleaseBuffer ();
				pc = str.GetBuffer (k);
				GetEnvironmentVariable (
									(LPTSTR) (LPCSTR) m_VariableNames[i],
									pc,
									k
									);
			}
			else if (k==0) pc[0] = 0;
		}

		VERIFY (SetEnvironmentVariable (
						(LPTSTR) (LPCSTR) m_VariableNames[i],
						(LPTSTR) (LPCSTR) m_VariableValues[i]
						));	// No idea how this could fail.

		if (bSaveOldValues)	m_VariableValues[i] = pc;
	}

	#ifdef ENVTEST
	{
		TCHAR *pc = (char *) GetEnvironmentStrings ();
		TRACE ("Dumping environment after CEnvironmentVariableList::SetVariables:\n");
		while (*pc != 0)
		{
			TRACE ("    %s\n", pc);
			pc += strlen (pc);
			pc++;
		}
	}
	#endif	
}
