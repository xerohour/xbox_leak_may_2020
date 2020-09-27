/*************************************************************
Module name: PrfDB.CPP
*************************************************************/

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////


PCPRFOBJTYPE CPRFDB::GetFirstObject(void) const
{
	return((PCPRFOBJTYPE)((PBYTE) m_pPrfDataBlk + m_pPrfDataBlk->HeaderLength));
}


//////////////////////////////////////////////////////////////


PCPRFOBJTYPE CPRFDB::GetNextObject(PCPRFOBJTYPE pPrfObjType) const
{
	return((PCPRFOBJTYPE)((PBYTE) pPrfObjType + pPrfObjType->TotalByteLength));
}


//////////////////////////////////////////////////////////////


int CPRFDB::GetNumObjects(void) const
{
	return(m_pPrfDataBlk->NumObjectTypes);
}


//////////////////////////////////////////////////////////////


PCPRFOBJTYPE CPRFDB::FindObjFromTitleIndex(DWORD dwTitleIndex) const
{
	PCPRFOBJTYPE pPrfObjType = GetFirstObject();
	int n = 0;

	while ((pPrfObjType != NULL) && (n < GetNumObjects())) {
		if (pPrfObjType->ObjectNameTitleIndex == dwTitleIndex)
			return(pPrfObjType);

		pPrfObjType = GetNextObject(pPrfObjType);
		n++;
	}
	return(pPrfObjType);
}


//////////////////////////////////////////////////////////////


PCPRFOBJTYPE CPRFDB::FindObjFromIndex(int nIndexObj) const
{
	PCPRFOBJTYPE pPrfObjType = NULL;
	int n = 0;

	if (nIndexObj < GetNumObjects())
	{
		pPrfObjType = GetFirstObject();
		for (; n < nIndexObj; n++)
			pPrfObjType = GetNextObject(pPrfObjType);
	}
	return(pPrfObjType);
}


//////////////////////////////////////////////////////////////


int CPRFDB::GetNumInstances(PCPRFOBJTYPE pPrfObjType) const 
{
	return(pPrfObjType->NumInstances);
}


//////////////////////////////////////////////////////////////


PCPRFINSTDEF CPRFDB::GetFirstInst(PCPRFOBJTYPE pPrfObjType) const
{
	return((PCPRFINSTDEF)((PBYTE) pPrfObjType + pPrfObjType->DefinitionLength));
}


//////////////////////////////////////////////////////////////


PCPRFINSTDEF CPRFDB::GetNextInst(PCPRFINSTDEF pPrfInstDef) const
{
	PCPRFCNTRBLK pPrfCntrBlk;
	pPrfCntrBlk = (PPERF_COUNTER_BLOCK) ((PBYTE) pPrfInstDef + pPrfInstDef->ByteLength);
	return((PCPRFINSTDEF)((PBYTE) pPrfCntrBlk + pPrfCntrBlk->ByteLength));
}


//////////////////////////////////////////////////////////////


PCPRFINSTDEF CPRFDB::FindInstFromIndex(PCPRFOBJTYPE pPrfObjType, int nIndexInst) const
{
	PCPRFINSTDEF pPrfInstDef = GetFirstInst(pPrfObjType);
	int nIndex = 0;

	if (nIndexInst < pPrfObjType->NumInstances)
	{
		while (nIndex++ < nIndexInst)
		{
			pPrfInstDef = GetNextInst(pPrfInstDef);
		}
	}
	return(pPrfInstDef);
}


//////////////////////////////////////////////////////////////


PCPRFINSTDEF CPRFDB::FindInstPrnt(PCPRFINSTDEF pPrfInstDef) const
{
	PCPRFOBJTYPE pPrfObjType;
	pPrfObjType = FindObjFromTitleIndex(pPrfInstDef->ParentObjectTitleIndex);
	if (pPrfObjType != NULL)
		pPrfInstDef = FindInstFromIndex(pPrfObjType, pPrfInstDef->ParentObjectInstance);
	else
		pPrfInstDef = NULL;
	return(pPrfInstDef);
}


//////////////////////////////////////////////////////////////


PCPRFINSTDEF CPRFDB::GetInst(PCPRFOBJTYPE pPrfObjType, int nIndexInst) const
{
	PCPRFINSTDEF pPrfInstDef = GetFirstInst(pPrfObjType);
	while (nIndexInst--) 
		pPrfInstDef = GetNextInst(pPrfInstDef);
	return(pPrfInstDef);
}


//////////////////////////////////////////////////////////////


LPCTSTR CPRFDB::GetInstName(PCPRFINSTDEF pPrfInstDef) const
{
	return((LPCTSTR)((PBYTE) pPrfInstDef + pPrfInstDef->NameOffset));
}


//////////////////////////////////////////////////////////////


PCPRFCNTRDEF CPRFDB::GetFirstCntr(PCPRFOBJTYPE pPrfObjType) const
{
	return((PCPRFCNTRDEF)((PBYTE) pPrfObjType + pPrfObjType->HeaderLength));
}


//////////////////////////////////////////////////////////////


PCPRFCNTRDEF CPRFDB::GetNextCntr(PCPRFCNTRDEF pPrfCntrDef) const
{
	return((PCPRFCNTRDEF)((PBYTE) pPrfCntrDef + pPrfCntrDef->ByteLength));
}


//////////////////////////////////////////////////////////////


int CPRFDB::GetNumCntrs(PCPRFOBJTYPE pPrfObjType) const
{
	return(pPrfObjType->NumCounters);
}


//////////////////////////////////////////////////////////////


PCPRFCNTRDEF CPRFDB::FindCntrFromTitleIndex(DWORD dwTitleIndex, PCPRFOBJTYPE pPrfObjType) const
{
	PCPRFCNTRDEF pPrfCntrDef = GetFirstCntr(pPrfObjType);
	int n = 0;

	while ((pPrfCntrDef != NULL) && (n < GetNumCntrs(pPrfObjType)))
	{
		if (pPrfCntrDef->CounterNameTitleIndex == dwTitleIndex)
			return(pPrfCntrDef);

		pPrfCntrDef = GetNextCntr(pPrfCntrDef);
		n++;
	}
	return(pPrfCntrDef);
}


//////////////////////////////////////////////////////////////


void * const CPRFDB::GetCntrData(PCPRFINSTDEF pPrfInstDef, PCPRFCNTRDEF pPrfCntrDef) const
{
	PCPRFCNTRBLK pPrfCntrBlk;

	pPrfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE) pPrfInstDef + pPrfInstDef->ByteLength);
	return((PVOID)((PBYTE)pPrfCntrBlk + pPrfCntrDef->CounterOffset));
}


//////////////////////// End Of File /////////////////////////
